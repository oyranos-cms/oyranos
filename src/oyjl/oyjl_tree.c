/*
 * Copyright (c) 2010-2011  Florian Forster  <ff at octo.it>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdarg.h>  /* va_list */
#include <stddef.h>  /* ptrdiff_t size_t */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#include <yajl/yajl_parse.h>
#ifndef YAJL_VERSION
#include <yajl/yajl_version.h>
#endif
#include "oyjl_tree.h"
#include "oyjl_tree_internal.h"
#define YA_FREE(afs, ptr) (afs)->free((afs)->ctx, (ptr))


#define STATUS_CONTINUE 1
#define STATUS_ABORT    0

struct stack_elem_s;
typedef struct stack_elem_s stack_elem_t;
struct stack_elem_s
{
    char * key;
    oyjl_val value;
    stack_elem_t *next;
};

struct context_s
{
    stack_elem_t *stack;
    oyjl_val root;
    char *errbuf;
    size_t errbuf_size;
};
typedef struct context_s context_t;

#define RETURN_ERROR(ctx,retval,...) {                                  \
        if ((ctx)->errbuf != NULL)                                      \
            snprintf ((ctx)->errbuf, (ctx)->errbuf_size, __VA_ARGS__);  \
        return (retval);                                                \
    }

static oyjl_val value_alloc (oyjl_type type)
{
    oyjl_val v;

    v = malloc (sizeof (*v));
    if (v == NULL) return (NULL);
    memset (v, 0, sizeof (*v));
    v->type = type;

    return (v);
}

static void oyjl_object_free (oyjl_val v)
{
    size_t i;

    if (!OYJL_IS_OBJECT(v)) return;

    for (i = 0; i < v->u.object.len; i++)
    {
        if(v->u.object.keys && v->u.object.keys[i])
        {
          free((char *) v->u.object.keys[i]);
          v->u.object.keys[i] = NULL;
        }
        if(v->u.object.values && v->u.object.values[i])
        {
          oyjl_tree_free (v->u.object.values[i]);
          v->u.object.values[i] = NULL;
        }
    }

    if(v->u.object.keys)
      free((void*) v->u.object.keys);
    if(v->u.object.values)
      free(v->u.object.values);
}

static void oyjl_array_free (oyjl_val v)
{
    size_t i;

    if (!OYJL_IS_ARRAY(v)) return;

    for (i = 0; i < v->u.array.len; i++)
    {
        if(v->u.array.values && v->u.array.values[i])
        {
          oyjl_tree_free (v->u.array.values[i]);
          v->u.array.values[i] = NULL;
        }
    }

    if(v->u.array.values)
      free(v->u.array.values);
}

/*
 * Parsing nested objects and arrays is implemented using a stack. When a new
 * object or array starts (a curly or a square opening bracket is read), an
 * appropriate value is pushed on the stack. When the end of the object is
 * reached (an appropriate closing bracket has been read), the value is popped
 * off the stack and added to the enclosing object using "context_add_value".
 */
static int context_push(context_t *ctx, oyjl_val v)
{
    stack_elem_t *stack;

    stack = malloc (sizeof (*stack));
    if (stack == NULL)
        RETURN_ERROR (ctx, ENOMEM, "Out of memory");
    memset (stack, 0, sizeof (*stack));

    assert ((ctx->stack == NULL)
            || OYJL_IS_OBJECT (v)
            || OYJL_IS_ARRAY (v));

    stack->value = v;
    stack->next = ctx->stack;
    ctx->stack = stack;

    return (0);
}

static oyjl_val context_pop(context_t *ctx)
{
    stack_elem_t *stack;
    oyjl_val v;

    if (ctx->stack == NULL)
        RETURN_ERROR (ctx, NULL, "context_pop: "
                      "Bottom of stack reached prematurely");

    stack = ctx->stack;
    ctx->stack = stack->next;

    v = stack->value;

    free (stack);

    return (v);
}

static int object_add_keyval(context_t *ctx,
                             oyjl_val obj, char *key, oyjl_val value)
{
    char **tmpk;
    oyjl_val *tmpv;

    /* We're checking for NULL in "context_add_value" or its callers. */
    assert (ctx != NULL);
    assert (obj != NULL);
    assert (key != NULL);
    assert (value != NULL);

    /* We're assuring that "obj" is an object in "context_add_value". */
    assert(OYJL_IS_OBJECT(obj));

    tmpk = realloc((void *) obj->u.object.keys, sizeof(*(obj->u.object.keys)) * (obj->u.object.len + 1));
    if (tmpk == NULL)
        RETURN_ERROR(ctx, ENOMEM, "Out of memory");
    obj->u.object.keys = tmpk;

    tmpv = realloc(obj->u.object.values, sizeof (*obj->u.object.values) * (obj->u.object.len + 1));
    if (tmpv == NULL)
        RETURN_ERROR(ctx, ENOMEM, "Out of memory");
    obj->u.object.values = tmpv;

    obj->u.object.keys[obj->u.object.len] = key;
    obj->u.object.values[obj->u.object.len] = value;
    obj->u.object.len++;

    return (0);
}

static int array_add_value (context_t *ctx,
                            oyjl_val array, oyjl_val value)
{
    oyjl_val *tmp;

    /* We're checking for NULL pointers in "context_add_value" or its
     * callers. */
    assert (ctx != NULL);
    assert (array != NULL);
    assert (value != NULL);

    /* "context_add_value" will only call us with array values. */
    assert(OYJL_IS_ARRAY(array));
    
    tmp = realloc(array->u.array.values,
                  sizeof(*(array->u.array.values)) * (array->u.array.len + 1));
    if (tmp == NULL)
        RETURN_ERROR(ctx, ENOMEM, "Out of memory");
    array->u.array.values = tmp;
    array->u.array.values[array->u.array.len] = value;
    array->u.array.len++;

    return 0;
}

/*
 * Add a value to the value on top of the stack or the "root" member in the
 * context if the end of the parsing process is reached.
 */
static int context_add_value (context_t *ctx, oyjl_val v)
{
    /* We're checking for NULL values in all the calling functions. */
    assert (ctx != NULL);
    assert (v != NULL);

    /*
     * There are three valid states in which this function may be called:
     *   - There is no value on the stack => This is the only value. This is the
     *     last step done when parsing a document. We assign the value to the
     *     "root" member and return.
     *   - The value on the stack is an object. In this case store the key on the
     *     stack or, if the key has already been read, add key and value to the
     *     object.
     *   - The value on the stack is an array. In this case simply add the value
     *     and return.
     */
    if (ctx->stack == NULL)
    {
        assert (ctx->root == NULL);
        ctx->root = v;
        return (0);
    }
    else if (OYJL_IS_OBJECT (ctx->stack->value))
    {
        if (ctx->stack->key == NULL)
        {
            if (!OYJL_IS_STRING (v))
                RETURN_ERROR (ctx, EINVAL, "context_add_value: "
                              "Object key is not a string (%#04x)",
                              v->type);

            ctx->stack->key = v->u.string;
            v->u.string = NULL;
            free(v);
            return (0);
        }
        else /* if (ctx->key != NULL) */
        {
            char * key;

            key = ctx->stack->key;
            ctx->stack->key = NULL;
            return (object_add_keyval (ctx, ctx->stack->value, key, v));
        }
    }
    else if (OYJL_IS_ARRAY (ctx->stack->value))
    {
        return (array_add_value (ctx, ctx->stack->value, v));
    }
    else
    {
        RETURN_ERROR (ctx, EINVAL, "context_add_value: Cannot add value to "
                      "a value of type %#04x (not a composite type)",
                      ctx->stack->value->type);
    }
}

static int handle_string (void *ctx,
                          const unsigned char *string, unsigned int string_length)
{
    oyjl_val v;

    v = value_alloc (oyjl_t_string);
    if (v == NULL)
        RETURN_ERROR ((context_t *) ctx, STATUS_ABORT, "Out of memory");

    v->u.string = malloc (string_length + 1);
    if (v->u.string == NULL)
    {
        free (v);
        RETURN_ERROR ((context_t *) ctx, STATUS_ABORT, "Out of memory");
    }
    memcpy(v->u.string, string, string_length);
    v->u.string[string_length] = 0;

    return ((context_add_value (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

#if (YAJL_VERSION) > 20000
static int handle_number (void *ctx, const char *string, size_t string_length)
#else
static int handle_number (void *ctx, const char *string, unsigned int string_length)
#endif
{
    oyjl_val v;
    char *endptr;

    v = value_alloc(oyjl_t_number);
    if (v == NULL)
        RETURN_ERROR((context_t *) ctx, STATUS_ABORT, "Out of memory");

    v->u.number.r = malloc(string_length + 1);
    if (v->u.number.r == NULL)
    {
        free(v);
        RETURN_ERROR((context_t *) ctx, STATUS_ABORT, "Out of memory");
    }
    memcpy(v->u.number.r, string, string_length);
    v->u.number.r[string_length] = 0;

    v->u.number.flags = 0;

    errno = 0;
    /*v->u.number.i = yajl_parse_integer((const unsigned char *) v->u.number.r,
                                       strlen(v->u.number.r));*/
    v->u.number.i = strtol(v->u.number.r, 0, 10);
    if (errno == 0)
        v->u.number.flags |= OYJL_NUMBER_INT_VALID;

    endptr = NULL;
    errno = 0;
#ifdef HAVE_LOCALE_H
    char * save_locale = oyjl_string_copy( setlocale(LC_NUMERIC, 0 ), malloc );
    setlocale(LC_NUMERIC, "C");
#endif
    v->u.number.d = strtod(v->u.number.r, &endptr);
#ifdef HAVE_LOCALE_H
    setlocale(LC_NUMERIC, save_locale);
    if(save_locale)
      free( save_locale );
#endif
    if ((errno == 0) && (endptr != NULL) && (*endptr == 0))
        v->u.number.flags |= OYJL_NUMBER_DOUBLE_VALID;

    return ((context_add_value(ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int handle_start_map (void *ctx)
{
    oyjl_val v;

    v = value_alloc(oyjl_t_object);
    if (v == NULL)
        RETURN_ERROR ((context_t *) ctx, STATUS_ABORT, "Out of memory");

    v->u.object.keys = NULL;
    v->u.object.values = NULL;
    v->u.object.len = 0;

    return ((context_push (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int handle_end_map (void *ctx)
{
    oyjl_val v;

    v = context_pop (ctx);
    if (v == NULL)
        return (STATUS_ABORT);

    return ((context_add_value (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int handle_start_array (void *ctx)
{
    oyjl_val v;

    v = value_alloc(oyjl_t_array);
    if (v == NULL)
        RETURN_ERROR ((context_t *) ctx, STATUS_ABORT, "Out of memory");

    v->u.array.values = NULL;
    v->u.array.len = 0;

    return ((context_push (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int handle_end_array (void *ctx)
{
    oyjl_val v;

    v = context_pop (ctx);
    if (v == NULL)
        return (STATUS_ABORT);

    return ((context_add_value (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int handle_boolean (void *ctx, int boolean_value)
{
    oyjl_val v;

    v = value_alloc (boolean_value ? oyjl_t_true : oyjl_t_false);
    if (v == NULL)
        RETURN_ERROR ((context_t *) ctx, STATUS_ABORT, "Out of memory");

    return ((context_add_value (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int handle_null (void *ctx)
{
    oyjl_val v;

    v = value_alloc (oyjl_t_null);
    if (v == NULL)
        RETURN_ERROR ((context_t *) ctx, STATUS_ABORT, "Out of memory");

    return ((context_add_value (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

/*
 * Public functions
 */

/** \addtogroup misc
 *  @{ *//* misc */
/** \addtogroup oyjl Oyjl JSON Parsing
 *  @brief   Easy to use JSON API
 *
 *  The API is designed to be easily useable without much boilerplate.
 *  It includes a xpath alike syntax to obtain or create nodes inside
 *  a tree. A path string is constructed of terms and the slash 
 *  delimiter '/'. Understood terms are object names or the squared 
 *  brackets index operator [].
 *
 *  \b Path \b Example:
 *
 *  "foo/[3]/bar" will return the "bar" node with the "found" string.
 *  @verbatim
    {
      "foo": [
        { "ignore": 0 },
        { "ignore_too": 0 },
        { "ignore_it": 0 },
        { "bar": "found" }
      ]
    }
    @endverbatim
 *  Some API's accept extended paths expressions. Those can contain empty
 *  terms, like "//", which matches all keys in the above example. Those
 *  are oyjl_tree_to_paths() and oyjl_path_match(). oyjl_tree_to_paths()
 *  works on the whole tree to match a extended xpath.
 *
 *  \b Programming \b Tutorial
 *
 *  The following code examples come from @ref tutorial_json_options.c . 
 *  @dontinclude tutorial_json_options.c
 *  @skip testOyjl(void)
 *  @until oyjl_tree_free(
 *  @{ *//* oyjl */

/** @brief read a json text string into a C data structure
 *
 *  @dontinclude tutorial_json_options.c
 *  @skipline text
 *  @skip error_buffer
 *  @until oyjl_tree_parse
 */
oyjl_val oyjl_tree_parse (const char *input,
                          char *error_buffer, size_t error_buffer_size)
{
#if (YAJL_VERSION) > 20000
static yajl_callbacks oyjl_tree_callbacks = {
  handle_null,
  handle_boolean,
  NULL, //handle_integer,
  NULL, //handle_double,
  handle_number,
  (int(*)(void*,const unsigned char*,size_t))handle_string,
  handle_start_map,
  (int(*)(void*,const unsigned char*,size_t))handle_string,
  handle_end_map,
  handle_start_array,
  handle_end_array
};
#else
static yajl_callbacks oyjl_tree_callbacks = {
  handle_null,
  handle_boolean,
  NULL, //handle_integer,
  NULL, //handle_double,
  handle_number,
  handle_string,
  handle_start_map,
  handle_string,
  handle_end_map,
  handle_start_array,
  handle_end_array
};
#endif
#if 0
    static const yajl_callbacks callbacks =
        {
            /* null        = */ handle_null,
            /* boolean     = */ handle_boolean,
            /* integer     = */ NULL,
            /* double      = */ NULL,
            /* number      = */ handle_number,
            /* string      = */ handle_string,
            /* start map   = */ handle_start_map,
            /* map key     = */ handle_string,
            /* end map     = */ handle_end_map,
            /* start array = */ handle_start_array,
            /* end array   = */ handle_end_array
        };
#endif
    yajl_handle handle;
#if YAJL_VERSION < 20000
    yajl_parser_config yconfig = { 1, 1 };
#endif
    yajl_status status;
    char * internal_err_str;
	context_t ctx = { NULL, NULL, NULL, 0 };

  if(!input) return NULL;

  ctx.errbuf = error_buffer;
	ctx.errbuf_size = error_buffer_size;

    if (error_buffer != NULL)
        memset (error_buffer, 0, error_buffer_size);

    handle = yajl_alloc( &oyjl_tree_callbacks,
#if YAJL_VERSION < 20000
                                                &yconfig,
#endif
                                                          NULL, (void*)&ctx);
#if YAJL_VERSION > 19999
    yajl_config(handle, yajl_allow_comments, 1);
#endif

    status = yajl_parse(handle,
                        (unsigned char *) input,
                        strlen (input));
#if YAJL_VERSION > 19999
    status = yajl_complete_parse (handle);
#endif
    if (status != yajl_status_ok) {
        if (error_buffer != NULL && error_buffer_size > 0) {
               internal_err_str = (char *) yajl_get_error(handle, 1,
                     (const unsigned char *) input,
                     strlen(input));
             snprintf(error_buffer, error_buffer_size, "%s", internal_err_str);
             yajl_free_error( handle, (unsigned char*)internal_err_str );
             internal_err_str = 0;
        }
        yajl_free (handle);
        return NULL;
    }

    yajl_free (handle);
    return (ctx.root);
}

oyjl_val oyjl_tree_get(oyjl_val n, const char ** path, oyjl_type type)
{
    if (!path) return NULL;
    while (n && *path) {
        size_t i;
        size_t len;

        if (n->type != oyjl_t_object) return NULL;
        len = n->u.object.len;
        for (i = 0; i < len; i++) {
            if (!strcmp(*path, n->u.object.keys[i])) {
                n = n->u.object.values[i];
                break;
            }
        }
        if (i == len) return NULL;
        path++;
    }
    if (n && type != oyjl_t_any && type != n->type) n = NULL;
    return n;
}

int        oyjl_path_term_get_index  ( const char        * term,
                                       int               * index );

/** @brief get the value as text string with user allocator */
char * oyjl_value_text (oyjl_val v, void*(*alloc)(size_t size))
{
  char * t = 0, * text = 0;

  if(v)
  switch(v->type)
  {
    case oyjl_t_null:
         break;
    case oyjl_t_number:
         if(v->u.number.flags & OYJL_NUMBER_DOUBLE_VALID)
         {
#ifdef HAVE_LOCALE_H
           char * save_locale = oyjl_string_copy( setlocale(LC_NUMERIC, 0 ), malloc );
           setlocale(LC_NUMERIC, "C");
#endif
           oyjl_string_add (&t, 0,0, "%g", v->u.number.d);
#ifdef HAVE_LOCALE_H
           setlocale(LC_NUMERIC, save_locale);
           if(save_locale)
             free( save_locale );
#endif
         }
         else
           oyjl_string_add (&t, 0,0, "%ld", v->u.number.i);
         break;
    case oyjl_t_true:
         oyjl_string_add (&t, 0,0, "1"); break;
    case oyjl_t_false:
         oyjl_string_add (&t, 0,0, "0"); break;
    case oyjl_t_string:
         oyjl_string_add (&t, 0,0, "%s", v->u.string); break;
    case oyjl_t_array:
    case oyjl_t_object:
         break;
    default:
         oyjl_message_p( oyjl_message_error, 0, OYJL_DBG_FORMAT_"unknown type: %d", OYJL_DBG_ARGS_, v->type );
         break;
  }

  if(t)
  {
    text = oyjl_string_copy (t, alloc);
    free (t); t = 0;
  }

  return text;
}

static void  oyjl_tree_find_         ( oyjl_val            root,
                                       int                 level,
                                       int                 levels,
                                       const char       ** terms,
                                       int                 flags,
                                       const char        * base,
                                       char            *** paths)
{
  int n = 0;
  int pos = -1;
  int i;
  const char * term = NULL;
  int count = oyjl_value_count( root );
  char * xpath = NULL;
  int tn = 0;

  while(terms && terms[tn]) ++tn;
  if(terms && level < tn)
    term = terms[level];

  if(term)
    oyjl_path_term_get_index( term, &pos );

  switch(root->type)
  {
    case oyjl_t_null:
    case oyjl_t_number:
    case oyjl_t_true:
    case oyjl_t_false:
    case oyjl_t_string:
         break;
    case oyjl_t_array:
         {
           for(i = 0; i < count; ++i)
           {
             if(pos >= 0 && pos != i) continue;

             if(base)
               oyjl_string_add( &xpath, 0,0, "%s%s[%d]",base,base[0]?"/":"",i );

             if(flags & OYJL_PATH)
             {
               n = 0; while(paths && *paths && (*paths)[n]) ++n;
               if(tn-1 <= level)
                 oyjl_string_list_add_static_string( paths, &n, xpath, malloc,free );
             }

             if(levels != 1)
               oyjl_tree_find_( root->u.array.values[i], level+1, levels-1, terms, flags, xpath, paths );
             if(xpath) { free(xpath); xpath = NULL; }
           }

         } break;
    case oyjl_t_object:
         {
           for(i = 0; i < count; ++i)
           {
             int ocount = oyjl_value_count( root->u.object.values[i]);
             const char * key = root->u.object.keys[i];

             if(pos >= 0 && pos != i)
               continue;
             if(pos <  0 && term && key && !(strcmp(term,key) == 0 ||
                                             term[0] == '\000'))
               continue;

             if(base && key)
               oyjl_string_add( &xpath, 0,0, "%s%s%s", base,base[0]?"/":"", key );

             if( (flags & OYJL_PATH && ocount) ||
                 (flags & OYJL_KEY && ocount == 0) )
             {
               n = 0; while(paths && *paths && (*paths)[n]) ++n;
               if(tn-1 <= level)
                 oyjl_string_list_add_static_string( paths, &n, xpath, malloc,free );
             }

             if(levels != 1)
               oyjl_tree_find_( root->u.object.values[i], level+1, levels-1, terms, flags, xpath, paths );
             if(xpath) { free(xpath); xpath = NULL; }
           }
         }
         break;
    default:
         oyjl_message_p( oyjl_message_error, 0, OYJL_DBG_FORMAT_"unknown type: %d", OYJL_DBG_ARGS_, root->type );
         break;
  }
}
/** @brief find matching paths
 *
 *  The function works on the whole tree to match a xpath.
 *
 *  @param         root                node
 *  @param         levels              desired level depth
 *  @param         xpath               extented path expression;
 *                                     It accepts even empty terms.
 *  @param         flags               support filters:
 *                                     - OYJL_KEY: only keys
 *                                     - OYJL_PATH: only paths
 *                                     - 0 for both, paths and keys
 *  @param         xpaths              the resulting string list
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/11/12
 *  @since   2017/11/10 (Oyranos: 0.9.7)
 */
void       oyjl_tree_to_paths        ( oyjl_val            root,
                                       int                 levels,
                                       const char        * xpath,
                                       int                 flags,
                                       char            *** paths )
{
  int pos = 0;
  int n = 0;
  char * base = NULL;
  char ** terms = oyjl_string_split(xpath, '/', &n, malloc);

  if(!root) return;

  if(!flags) flags = OYJL_PATH | OYJL_KEY;

  while(paths && *paths && (*paths)[pos]) ++pos;
  if(paths && pos)
    base = oyjl_string_copy( (*paths)[pos-1], malloc );
  else
    base = oyjl_string_copy( "",malloc );

  if(base)
  {
    oyjl_tree_find_( root, 0, levels, (const char**) terms, flags, base, paths );

    free(base);
  }

  oyjl_string_list_release( &terms, n, free );
}

static void oyjl_json_indent ( char ** json, const char * before, int level, const char * after )
{
  char * njson;
  int len;

  if(!json) return;

  len = *json ? strlen(*json) : 0;
  len += before ? strlen( before ) : 0;
  len += level;
  len += after ? strlen( after ) : 0;
  len += 1;

  njson = malloc( len );
  if(!njson) return;
  njson[0] = 0;

  if(*json)
  {
    strcpy( njson, *json );
    free( *json );
  }
  if(before) strcpy( &njson[strlen(njson)], before );
  if(level)
  {
    len = strlen(njson);
    memset( &njson[len], ' ', level );
    njson[len+level] = '\000';
  }
  if(after) strcpy( &njson[strlen(njson)], after );
  *json = njson;
}

/** @brief convert a C tree into a JSON string */
void oyjl_tree_to_json (oyjl_val v, int * level, char ** json)
{
  if(v)
  switch(v->type)
  {
    case oyjl_t_null:
         break;
    case oyjl_t_number:
         if(v->u.number.flags & OYJL_NUMBER_DOUBLE_VALID)
         {
#ifdef HAVE_LOCALE_H
           char * save_locale = oyjl_string_copy( setlocale(LC_NUMERIC, 0 ), malloc );
           setlocale(LC_NUMERIC, "C");
#endif
           oyjl_string_add (json, 0,0, "%g", v->u.number.d);
#ifdef HAVE_LOCALE_H
           setlocale(LC_NUMERIC, save_locale);
           if(save_locale)
             free( save_locale );
#endif
         }
         else
           oyjl_string_add (json, 0,0, "%ld", v->u.number.i);
         break;
    case oyjl_t_true:
         oyjl_string_add (json, 0,0, "1"); break;
    case oyjl_t_false:
         oyjl_string_add (json, 0,0, "0"); break;
    case oyjl_t_string:
         oyjl_string_add (json, 0,0, "\"%s\"", v->u.string); break;
    case oyjl_t_array:
         {
           int i,
               count = v->u.array.len;

           oyjl_string_add( json, 0,0, "[" );

           *level += 2;
           for(i = 0; i < count; ++i)
           {
             oyjl_tree_to_json( v->u.array.values[i], level, json );
             if(count > 1)
             {
               if(i < count - 1)
                 oyjl_string_add( json, 0,0, "," );
             }
           }
           *level -= 2;

           oyjl_string_add( json, 0,0, "]");
         } break;
    case oyjl_t_object:
         {
           int i,
               count = v->u.object.len;

           oyjl_string_add( json, 0,0, "{" );

           *level += 2;
           for(i = 0; i < count; ++i)
           {
             oyjl_json_indent( json, "\n", *level, NULL );
             if(!v->u.object.keys || !v->u.object.keys[i])
             {
               oyjl_message_p( oyjl_message_error, 0, OYJL_DBG_FORMAT_"missing key", OYJL_DBG_ARGS_ );
               if(json && *json)
               {
                 free(*json);
                 *json = NULL;
               }
               return;
             }
             oyjl_string_add( json, 0,0, "\"%s\": ", v->u.object.keys[i] );
             oyjl_tree_to_json( v->u.object.values[i], level, json );
             if(count > 1)
             {
               if(i < count - 1)
                 oyjl_string_add( json, 0,0, "," );
             }
           }
           *level -= 2;

           oyjl_json_indent( json, "\n", *level, "}" );
         }
         break;
    default:
         oyjl_message_p( oyjl_message_error, 0, OYJL_DBG_FORMAT_"unknown type: %d", OYJL_DBG_ARGS_, v->type );
         break;
  }
  return;
}

/** @brief return the number of members if any at the node level
 *
 *  This function is useful to traverse through objects and arrays of a
 *  unknown JSON tree. */
int            oyjl_value_count      ( oyjl_val            v )
{
  int count = 0;

  if(!v)
    return count;

  if(v->type == oyjl_t_object)
    count = v->u.object.len;
  else if(v->type == oyjl_t_array)
    count = v->u.array.len;

  return count;
}

/** @brief obtain a child node at the nth position from a object or array node */
oyjl_val       oyjl_value_pos_get    ( oyjl_val            v,
                                       int                 pos )
{
  if(!v)
    return NULL;

  if(v->type == oyjl_t_object)
    return v->u.object.values[pos];
  else if(v->type == oyjl_t_array)
    return v->u.array.values[pos];

  return NULL;
}

/** @internal 
 *  @brief tell about xpath segment
 *
 *  @param         term                xpath segment
 *  @param         index               resulting array position,
 *                                     - is a index: set index from term
 *                                     - is a wildcard: keeps index untouched
 *                                     - is not an index or wildcard: set position to -1
 *  @return                            status
 *                                     - 0  : index or wildcard
 *                                     - 1  : error
 *                                     - -1 : no suitable term, will set index to -1
 */
int        oyjl_path_term_get_index  ( const char        * term,
                                       int               * index )
{
  char * tindex;
  int pos = -1;
  int error = -1;

  if(!term) { *index = pos; return 1; }

  tindex = strrchr(term,'[');

  /* pick wildcards "", "[]" */
  if(term[0] == '\000' ||
     strcmp(term,"[]") == 0)
  {
    pos = *index;
    error = 0;
  }
  else
  if(tindex != NULL)
  {
    ptrdiff_t size;
    ++tindex;
    size = strrchr(term,']') - tindex;
    if(size > 0)
    {
      long signed int num = 0;
      char * ttmp = malloc(size + 1);
      if(!ttmp) return 1;
      memcpy( ttmp, tindex, size );
      ttmp[size] = '\000';

      error = oyjl_string_to_long( ttmp, &num );
      if(!error)
        pos = num;

      if(ttmp) free( ttmp );
    }
  }

  *index = pos;

  return error;
}
/** @brief search for xpath pattern matching in a full path
 *
 *  The function tries to match a single path expression level by level.
 *
 *  @code
    // the second xpath expression matches the first path
    int matches = oyjl_path_match( "org/free/[1]/s2key_d", "org///s2key_d", 0 );
    // "//[1]/s2key_d" or "///s2key_d" would fit as well;  "//[0]/s2key_d" not
    @endcode
 *
 *  @param         path                a path expression
 *  @param         xpath               a extented path expression
 *  @param         flags               optional switches
 *                                     - 0 : match all xpaths from start
 *                                     - OYJL_PATH_MATCH_LEN : match all xpaths of the exact same number of terms
 *                                     - OYJL_PATH_MATCH_LAST_ITEMS : search the last terms(s) from xpath
 *  @return                            0 - fail, 1 - match
 */
int        oyjl_path_match           ( const char        * path,
                                       const char        * xpath,
                                       int                 flags )
{
  int match = 0, i,pn=0,xn=0,diff=0;
  char ** xlist = oyjl_string_split(xpath, '/', &xn, malloc);
  char ** plist = oyjl_string_split(path, '/', &pn, malloc);

  if(flags & OYJL_PATH_MATCH_LAST_ITEMS)
    diff = pn - xn;

  if(!xlist || !plist) return 0;
  if(flags & OYJL_PATH_MATCH_LEN)
  {
    if(pn == xn) match = 1;
  } else {
    if(pn >= xn) match = 1;
  }

  /* follow the search path term */
  for(i = diff; i < (diff + xn) && match; ++i)
  {
    char * xterm = xlist[i - diff],
         * pterm = plist[i];
    int xindex = -2,
        pindex = -2;

    oyjl_path_term_get_index( xterm, &xindex );
    oyjl_path_term_get_index( pterm, &pindex );

    if(!(strcmp(xterm, pterm) == 0 ||
        (pindex >= 0 && xindex == pindex) ||
        (xindex == -2)))
      match = 0;
  }

  oyjl_string_list_release( &xlist, xn, free );
  oyjl_string_list_release( &plist, pn, free );

  return match;
}


/* split new root allocation from inside root manipulation */
static oyjl_val  oyjl_tree_get_value_( oyjl_val            v,
                                       int                 flags,
                                       const char        * xpath )
{
  oyjl_val level = 0, parent = v, root = NULL;
  int n = 0, i, found = 0;
  char ** list = oyjl_string_split(xpath, '/', &n, malloc);

  /* follow the search path term */
  for(i = 0; i < n; ++i)
  {
    char * term = list[i];
    /* is object or array */
    int count = oyjl_value_count( parent );
    int j;
    int pos = 0;

    found = 0;
    if(count == 0 && !(flags & OYJL_CREATE_NEW)) break;

    oyjl_path_term_get_index( term, &pos );

    /* requests index in object or array */
    if(pos != -1)
    {
      if(0 <= pos && pos < count)
        level = oyjl_value_pos_get( parent, pos );
      else
        level = NULL;

      /* add new leave */
      if(!level &&
         flags & OYJL_CREATE_NEW)
      {
        level = value_alloc( oyjl_t_null );

        if(parent)
        {
          if(parent->type != oyjl_t_array)
          {
            oyjl_value_clear( parent );
            parent->type = oyjl_t_array;
            oyjlAllocHelper_m_( parent->u.array.values, oyjl_val, 2, malloc, oyjl_tree_free( level ); goto clean );
            parent->u.array.len = 0;
          } else
          {
            oyjl_val *tmp;

            tmp = realloc(parent->u.array.values,
                    sizeof(*(parent->u.array.values)) * (parent->u.array.len + 1));
            if (tmp == NULL)
            {
              oyjl_message_p( oyjl_message_error, 0, OYJL_DBG_FORMAT_"could not allocate memory", OYJL_DBG_ARGS_ );
              oyjl_tree_free( level );
              goto  clean;
            }
            parent->u.array.values = tmp;
          }
          parent->u.array.values[parent->u.array.len] = level;
          parent->u.array.len++;
        }
      }

      found = 1;
    } else
    {
      /* search for name in object */
      for(j = 0; j < count; ++j)
      {
        if((parent->type == oyjl_t_object && strcmp( parent->u.object.keys[j], term ) == 0) ||
            /* a empty term matches to everything */
           term[0] == '\000')
        {
          found = 1;
          level = oyjl_value_pos_get( parent, j );
          break;
        }
      }

      /* add new leave */
      if(!level &&
         flags & OYJL_CREATE_NEW)
      {
        level = value_alloc( oyjl_t_null );

        if(parent)
        {
          if(parent->type != oyjl_t_object)
          {
            oyjl_value_clear( parent );
            parent->type = oyjl_t_object;
            oyjlAllocHelper_m_( parent->u.object.values, oyjl_val, 2, malloc, oyjl_tree_free( level ); goto clean );
            oyjlAllocHelper_m_( parent->u.object.keys, char*, 2, malloc, oyjl_tree_free( level ); goto clean );
            parent->u.object.len = 0;
          } else
          {
            oyjl_val *tmp;
            char ** keys;

            tmp = realloc(parent->u.object.values,
                    sizeof(*(parent->u.object.values)) * (parent->u.object.len + 1));
            if (tmp == NULL)
            {
              oyjl_message_p( oyjl_message_error, 0, OYJL_DBG_FORMAT_"could not allocate memory", OYJL_DBG_ARGS_ );
              oyjl_tree_free( level );
              goto clean;
            }
            parent->u.object.values = tmp;

            keys = realloc(parent->u.object.keys,
                    sizeof(*(parent->u.object.keys)) * (parent->u.object.len + 1));
            if (keys == NULL)
            {
              oyjl_message_p( oyjl_message_error, 0, OYJL_DBG_FORMAT_"could not allocate memory", OYJL_DBG_ARGS_ );
              oyjl_tree_free( level );
              goto clean;
            }
            parent->u.object.keys = keys;
          }
          parent->u.object.keys[parent->u.object.len] = oyjl_string_copy( term, malloc );
          parent->u.object.values[parent->u.object.len] = level;
          parent->u.object.len++;
        }
      } 

      found = 1;
    }
    if(!v && !root)
    {
      root = level;
      --i;
    }
    parent = level;
    level = NULL;
  }

  /* clean up temorary memory */
clean:
  for(i = 0; i < n; ++i)
    free(list[i]);
  if(list)
    free(list);

  if(found && root)
    return root;
  if(found && parent)
    return parent;
  else
  {
    if(root)
      oyjl_tree_free(root);
    else if(!v && parent)
      oyjl_tree_free(parent);
    return NULL;
  }
}
/** @brief create a node by a path expression
 *
 *  A NULL argument allocates just a node of type oyjl_t_null.
 *
 *  @see oyjl_tree_get_valuef() */
oyjl_val   oyjl_tree_new             ( const char        * path )
{
  if(path && path[0])
    return oyjl_tree_get_value_( NULL, OYJL_CREATE_NEW, path );
  else
    return value_alloc( oyjl_t_null );
}

/** @brief obtain a node by a path expression
 *
 *  @see oyjl_tree_get_valuef() */
oyjl_val   oyjl_tree_get_value       ( oyjl_val            v,
                                       int                 flags,
                                       const char        * xpath )
{
  if(!v || !xpath)
    return NULL;
  else
    return oyjl_tree_get_value_(v,flags,xpath);
}


/** Function oyjl_tree_get_valuef
 *  @brief   get a child node by a path expression
 *
 *  Creating a new node inside a existing tree needs just a root node - v.
 *  The flags should contain OYJL_CREATE_NEW.
 *  @code
    oyjl_val new_node = oyjl_tree_get_valuef( root, OYJL_CREATE_NEW, "my/new/node" );
    @endcode
 *
 *  Example: "foo/[]/bar" will append a node to the foo array and create
 *  the bar node, which is empty.
 *  @code
    oyjl_val new_node = oyjl_tree_get_valuef( root, OYJL_CREATE_NEW, "foo/[]/bar" );
    @endcode
 *
 *
 *  @param[in]     v                   the oyjl node
 *  @param[in]     flags               OYJL_CREATE_NEW - returns nodes even
 *                                     if they did not yet exist
 *  @param[in]     format              the format for the slashed path string
 *  @param[in]     ...                 the variable argument list; optional
 *  @return                            the requested node or a new tree or zero
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/10/12
 *  @since   2011/09/24 (Oyranos: 0.3.3)
 */
oyjl_val   oyjl_tree_get_valuef      ( oyjl_val            v,
                                       int                 flags,
                                       const char        * format,
                                                           ... )
{
  oyjl_val value = 0;

  char * text = 0;
  va_list list;
  int len;
  size_t sz = strlen(format) * 2;

  text = malloc( sz );
  if(!text)
  {
    oyjl_message_p( oyjl_message_error, 0, OYJL_DBG_FORMAT_"could not allocate memory", OYJL_DBG_ARGS_ );
    return 0;
  }

  text[0] = 0;

  va_start( list, format);
  len = vsnprintf( text, sz, format, list );
  va_end  ( list );

  if ((size_t)len >= sz)
  {
    text = realloc( text, (len+1)*sizeof(char) );
    if(!text) return NULL;
    va_start( list, format);
    len = vsnprintf( text, len+1, format, list );
    va_end  ( list );
  }

  value = oyjl_tree_get_value( v, flags, text );

  if(text) free(text);

  return value;
}

/** @brief set the node value to a string */
int        oyjl_value_set_string     ( oyjl_val            v,
                                       const char        * string )
{
  int error = -1;
  if(v)
  {
    oyjl_value_clear( v );
    v->type = oyjl_t_string;
    v->u.string = NULL;
    error = oyjl_string_add( &v->u.string, 0,0, "%s", string );
  }
  return error;
}

/** @brief release all childs recursively */
void oyjl_value_clear        (oyjl_val v)
{
    if (v == NULL) return;

    if (OYJL_IS_STRING(v)) {
        if(v->u.string) free(v->u.string);
        v->u.string = NULL;
    } else if (OYJL_IS_NUMBER(v)) {
        if(v->u.number.r) free(v->u.number.r);
        v->u.number.r = NULL;
    } else if (OYJL_GET_OBJECT(v))
        oyjl_object_free(v);
    else if (OYJL_GET_ARRAY(v))
        oyjl_array_free(v);

    v->type = oyjl_t_null;
}

/** @brief release a specific node and all its childs
 *
 *  In case parents have no children, release them or clear root.
 */
void oyjl_tree_clear_value           ( oyjl_val            root,
                                       const char        * xpath )
{
  int n = 0, i, pos, count;
  char ** list;
  char * path;
  int delete_parent = 0;

  if(!root) return;

  list = oyjl_string_split(xpath, '/', &n, malloc);
  path = oyjl_string_copy( xpath, malloc );

  for(pos = 0; pos < n; ++pos)
  {
    oyjl_val p; /* parent */
    oyjl_val o = oyjl_tree_get_value( root, 0, path );

    char * parent_path = oyjl_string_copy( path, malloc ),
         * t = strrchr(parent_path, '/');
    if(t)
    {
      t[0] = '\000';
      p = oyjl_tree_get_value( root, 0, parent_path );
    }
    else
      p = root;

    delete_parent = 0;
    if(p)
    {
      switch(p->type)
      {
      case oyjl_t_array:
         {
           count = p->u.array.len;

           for(i = 0; i < count; ++i)
           {
             if( p->u.array.values[i] == o )
             {
               oyjl_tree_free( o );
               p->u.array.values[i] = o = NULL;

               if(count > 1)
                 memmove( &p->u.array.values[i], &p->u.array.values[i+1],
                          sizeof(oyjl_val *) * (count - i - 1) );
               else
                 delete_parent = 1;

               --p->u.array.len;
               break;
             }
           }
         }
         break;
      case oyjl_t_object:
         {
           count = p->u.object.len;

           if(count == 0)
             delete_parent = 1;

           for(i = 0; i < count; ++i)
           {
             if( p->u.object.values[i] == o )
             {
               if(p->u.object.keys[i])
                 free(p->u.object.keys[i]);
               p->u.object.keys[i] = NULL;

	       oyjl_tree_free( o );
               p->u.object.values[i] = o = NULL;

               if(count > 1)
               {
                 memmove( &p->u.object.keys[i], &p->u.object.keys[i+1],
                          sizeof(char *) * (count - i - 1) );
                 memmove( &p->u.object.values[i], &p->u.object.values[i+1],
                          sizeof(oyjl_val *) * (count - i - 1) );
               }
               else
                 delete_parent = 1;

               --p->u.object.len;
               break;
             }
           }
         }
         break;
      default: break; /* ok */
      }
    }

    if(path) free(path);
    path = parent_path;
    parent_path = NULL;
    oyjl_tree_free( o );
    o = NULL;

    if(delete_parent == 0)
      break;
  }

  /* The root node has no name here. So we need to detect that case.
   * Keep the node itself, as it is still referenced by the caller. */
  if(path && delete_parent && strchr(path,'/') == NULL)
    oyjl_value_clear(root);

  for(i = 0; i < n; ++i) free(list[i]);
  if(list) free(list);
  if(path) free(path);
}

/** @brief release a node and all its childs recursively */
void oyjl_tree_free (oyjl_val v)
{
    if (v == NULL) return;

    oyjl_value_clear (v);
    free(v);
}

/** @} *//* oyjl */
/** @} *//* misc */
