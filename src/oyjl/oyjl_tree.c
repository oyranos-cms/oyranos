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

void       oyjl_tree_to_paths        ( oyjl_val            v,
                                       int                 levels,
                                       char            *** xpaths )
{
  int pos = 0, n = 0;
  char * base = NULL;

  while(xpaths && *xpaths && (*xpaths)[pos]) ++pos;
  n = pos;
  if(xpaths && pos)
    base = oyjl_string_copy( (*xpaths)[pos-1], malloc );
  else
    base = oyjl_string_copy( "",malloc );

  if(v)
  switch(v->type)
  {
    case oyjl_t_null:
    case oyjl_t_number:
    case oyjl_t_true:
    case oyjl_t_false:
    case oyjl_t_string:
         break;
    case oyjl_t_array:
         {
           int i,
               count = v->u.array.len;

           for(i = 0; i < count; ++i)
           {
             char * xpath = NULL;
             oyjl_string_add( &xpath, 0,0, "%s%s[%d]",base,base[0]?"/":"",i );
             oyjl_string_list_add_static_string( xpaths, &n, xpath, malloc,free );
             free(xpath);
             if(levels != 1)
             {
               oyjl_tree_to_paths( v->u.array.values[i], levels-1, xpaths );
               while(xpaths && *xpaths && (*xpaths)[n]) ++n;
             }
           }

         } break;
    case oyjl_t_object:
         {
           int i,
               count = v->u.object.len;

           for(i = 0; i < count; ++i)
           {
             char * xpath = NULL;
             const char * key = v->u.object.keys[i];

             oyjl_string_add( &xpath, 0,0, "%s%s%s", base,base[0]?"/":"", key );
             oyjl_string_list_add_static_string( xpaths, &n, xpath, malloc,free );
             free(xpath);
             if(levels != 1)
             {
               oyjl_tree_to_paths( v->u.object.values[i], levels-1, xpaths );
               while(xpaths && *xpaths && (*xpaths)[n]) ++n;
             }
           }
         }
         break;
    default:
         oyjl_message_p( oyjl_message_error, 0, OYJL_DBG_FORMAT_"unknown type: %d", OYJL_DBG_ARGS_, v->type );
         break;
  }

  free(base);

  return;
}

void oyjl_tree_to_json (oyjl_val v, int * level, char ** json)
{
  int n = *level;

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
             oyjl_string_add( json, 0,0, "\n");
             n = *level; while(n--) oyjl_string_add(json, 0,0, " ");
             if(!v->u.object.keys || !v->u.object.keys[i])
             {
               oyjl_message_p( oyjl_message_error, 0, OYJL_DBG_FORMAT_"missing key", OYJL_DBG_ARGS_ );
               if(json && *json) free(*json);
               *json = NULL;
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

           oyjl_string_add( json, 0,0, "\n");
           n = *level; while(n--) oyjl_string_add(json, 0,0, " ");
           oyjl_string_add( json, 0,0, "}");
         }
         break;
    default:
         oyjl_message_p( oyjl_message_error, 0, OYJL_DBG_FORMAT_"unknown type: %d", OYJL_DBG_ARGS_, v->type );
         break;
  }
  return;
}

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

int        oyjl_tree_paths_get_index ( const char        * term,
                                       int               * index )
{
  char * tindex = strrchr(term,'['),
       * ttmp = NULL;
  int pos = -1;
  int error = -1;

  if(tindex != NULL)
  {
    ptrdiff_t size;
    ++tindex;
    size = strrchr(term,']') - tindex;
    if(size > 0)
    {
      long signed int num = 0;
      ttmp = malloc(size + 1);
      memcpy( ttmp, tindex, size );
      ttmp[size] = '\000';

      error = oyjl_string_to_long( ttmp, &num );
      if(!error)
        pos = num;
    }
  }

  *index = pos;

  if(ttmp)
    free( ttmp );

  return error;
}

oyjl_val   oyjl_tree_get_value       ( oyjl_val            v,
                                       int                 flags,
                                       const char        * xpath )
{
  oyjl_val level = 0, parent = v;
  int n = 0, i, found = 0;
  char ** list = oyjl_string_split(xpath, '/', &n, malloc);

  /* follow the search path term */
  for(i = 0; i < n; ++i)
  {
    char * term = list[i];
    /* is object or array */
    int count = oyjl_value_count( parent );
    int j;
    int pos = -1;

    found = 0;

    /* requests index in object or array */
    if((oyjl_tree_paths_get_index( term, &pos ) == 0 && pos != -1) ||
       /* request a empty index together with OYJL_CREATE_NEW */
       strcmp(term,"[]") == 0)
    {
      if(count > pos)
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
            oyjl_tree_free_content( parent );
            parent->type = oyjl_t_array;
            oyjlAllocHelper_m_( parent->u.array.values, oyjl_val, 2, malloc, return NULL );
          } else
          {
            oyjl_val *tmp;

            tmp = realloc(parent->u.array.values,
                    sizeof(*(parent->u.array.values)) * (parent->u.array.len + 1));
            if (tmp == NULL)
            {
              oyjl_message_p( oyjl_message_error, 0, OYJL_DBG_FORMAT_"could not allocate memory", OYJL_DBG_ARGS_ );
              return NULL;
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
        if(term &&
           strcmp( parent->u.object.keys[j], term ) == 0)
        {
          found = 1;
          level = parent->u.object.values[j];
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
            oyjl_tree_free_content( parent );
            parent->type = oyjl_t_object;
            oyjlAllocHelper_m_( parent->u.object.values, oyjl_val, 2, malloc, return NULL );
            oyjlAllocHelper_m_( parent->u.object.keys, char*, 2, malloc, return NULL );
          } else
          {
            oyjl_val *tmp;
            char ** keys;

            tmp = realloc(parent->u.object.values,
                    sizeof(*(parent->u.object.values)) * (parent->u.object.len + 1));
            if (tmp == NULL)
            {
              oyjl_message_p( oyjl_message_error, 0, OYJL_DBG_FORMAT_"could not allocate memory", OYJL_DBG_ARGS_ );
              return NULL;
            }
            parent->u.object.values = tmp;

            keys = realloc(parent->u.object.keys,
                    sizeof(*(parent->u.object.keys)) * (parent->u.object.len + 1));
            if (keys == NULL)
            {
              oyjl_message_p( oyjl_message_error, 0, OYJL_DBG_FORMAT_"could not allocate memory", OYJL_DBG_ARGS_ );
              return NULL;
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
    parent = level;
    level = NULL;
  }

  /* clean up temorary memory */
  for(i = 0; i < n; ++i)
    free(list[i]);
  if(list)
    free(list);

  if(found && parent)
    return parent;
  else
    return NULL;
}

/** @internal
 *  Function oyjl_tree_get_valuef
 *  @brief   get a child node
 *
 *  A path string is constructed of terms and the slash delimiter '/'.
 *  Understood terms are object names or the squared brackets index operator [].
 *  Example: "foo/[3]/bar" will return the "bar" node with the "found" string.
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
 *
 *  Creating a new node inside a existing tree needs just a root node - v.
 *  The flags should contain OYJL_CREATE_NEW.
 *  Example: "foo/[]/bar" will append a node to the foo array and create
 *  the bar node, which is empty.
 *
 *  @param[in]     v                   the oyjl node
 *  @param[in]     flags               OYJL_CREATE_NEW - returns nodes even
 *                                     if they did not yet exist
 *  @param[in]     format              the format for the slashed path string
 *  @param[in]     ...                 the variable argument list; optional
 *  @return                            the requested node or zero
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/10/28
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
    va_start( list, format);
    len = vsnprintf( text, len+1, format, list );
    va_end  ( list );
  }

  value = oyjl_tree_get_value( v, flags, text );

  if(text) free(text);

  return value;
}

int        oyjl_value_set_string     ( oyjl_val            v,
                                       const char        * string )
{
  int error = -1;
  if(v)
  {
    oyjl_tree_free_content( v );
    v->type = oyjl_t_string;
    error = oyjl_string_add( &v->u.string, 0,0, "%s", string );
  }
  return error;
}

void oyjl_tree_free_content (oyjl_val v)
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

void oyjl_tree_free_node             ( oyjl_val            root,
                                       const char        * xpath )
{
  int n = 0, i, pos, count;
  char ** list = oyjl_string_split(xpath, '/', &n, malloc);
  char * path = oyjl_string_copy( xpath, malloc );

  for(pos = 0; pos < (n-1); ++pos)
  {
    oyjl_val p; /* parent */
    oyjl_val o = oyjl_tree_get_value( root, 0, path );
    int delete_parent = 0;

    char * parent_path = oyjl_string_copy( path, malloc ),
         * t = strrchr(parent_path, '/');
    if(t)
      t[0] = '\000';

    p = oyjl_tree_get_value( root, 0, parent_path );
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
               p->u.array.values[i] = NULL;

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

           for(i = 0; i < count; ++i)
           {
             if( p->u.object.values[i] == o )
             {
               oyjl_tree_free( o );
               p->u.object.keys[i] = NULL;
               p->u.object.values[i] = NULL;

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

    if(delete_parent == 0)
      break;
  }

  for(i = 0; i < n; ++i) free(list[i]);
  if(list) free(list);
  if(path) free(path);
}

void oyjl_tree_free (oyjl_val v)
{
    if (v == NULL) return;

    oyjl_tree_free_content (v);
    free(v);
}

