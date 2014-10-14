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

#include <yajl/yajl_parse.h>
#ifndef YAJL_VERSION
#include <yajl/yajl_version.h>
#endif
#include "oyjl_tree.h"
#define YA_FREE(afs, ptr) (afs)->free((afs)->ctx, (ptr))

#if defined(_MSC_VER) 
#define snprintf sprintf_s
#endif

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
        free((char *) v->u.object.keys[i]);
        v->u.object.keys[i] = NULL;
        oyjl_tree_free (v->u.object.values[i]);
        v->u.object.values[i] = NULL;
    }

    free((void*) v->u.object.keys);
    free(v->u.object.values);
    free(v);
}

static void oyjl_array_free (oyjl_val v)
{
    size_t i;

    if (!OYJL_IS_ARRAY(v)) return;

    for (i = 0; i < v->u.array.len; i++)
    {
        oyjl_tree_free (v->u.array.values[i]);
        v->u.array.values[i] = NULL;
    }

    free(v->u.array.values);
    free(v);
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
    const char **tmpk;
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
    v->u.number.i = strtol(v->u.number.r, 0, 10);
    if (errno == 0)
        v->u.number.flags |= OYJL_NUMBER_INT_VALID;

    endptr = NULL;
    errno = 0;
    v->u.number.d = strtod(v->u.number.r, &endptr);
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

char *             oyjl_string_copy  ( char              * string,
                                       void*            (* alloc)(size_t size))
{
  char * text = 0;

  if(!alloc) alloc = malloc;

  text = alloc( strlen(string) + 1 );
  strcpy( text, string );
    
  return text;
}

int                   oyjl_string_add( char             ** string,
                                       const char        * format,
                                                           ... )
{
  char * text_copy = NULL;
  char * text = 0;
  va_list list;
  int len;
  size_t sz = 0;

  va_start( list, format);
  len = vsnprintf( text, sz, format, list );
  va_end  ( list );

  {
    text = malloc( len + 1 );
    va_start( list, format);
    len = vsnprintf( text, len+1, format, list );
    va_end  ( list );
  }

  if(string && *string)
  {
    int l = strlen(*string);
    text_copy = malloc( len + l + 1 );
    strcpy( text_copy, *string );
    strcpy( &text_copy[l], text );
    

    free(*string);
    *string = text_copy;

    free(text);

  } else
    *string = text;

  return 0;
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
           oyjl_string_add (&t, "%g", v->u.number.d);
         else
           oyjl_string_add (&t, "%ld", v->u.number.i);
         break;
    case oyjl_t_true:
         oyjl_string_add (&t, "1"); break;
    case oyjl_t_false:
         oyjl_string_add (&t, "0"); break;
    case oyjl_t_string:
         oyjl_string_add (&t, "%s", v->u.string); break;
    case oyjl_t_array:
    case oyjl_t_object:
         break;
    default:
         fprintf( stderr, "unknown type: %d\n", v->type );
         break;
  }

  if(t)
  {
    text = oyjl_string_copy (t, alloc);
    free (t); t = 0;
  }

  return text;
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
           oyjl_string_add (json, "%g", v->u.number.d);
         else
           oyjl_string_add (json, "%ld", v->u.number.i);
         break;
    case oyjl_t_true:
         oyjl_string_add (json, "1"); break;
    case oyjl_t_false:
         oyjl_string_add (json, "0"); break;
    case oyjl_t_string:
         oyjl_string_add (json, "\"%s\"", v->u.string); break;
    case oyjl_t_array:
         {
           int i,
               count = v->u.array.len;

           oyjl_string_add( json, "[" );

           *level += 2;
           for(i = 0; i < count; ++i)
           {
             oyjl_tree_to_json( v->u.array.values[i], level, json );
             if(count > 1)
             {
               if(i < count - 1)
                 oyjl_string_add( json, "," );
             }
           }
           *level -= 2;

           oyjl_string_add( json, "]");
         } break;
    case oyjl_t_object:
         {
           int i,
               count = v->u.object.len;

           oyjl_string_add( json, "{" );

           *level += 2;
           for(i = 0; i < count; ++i)
           {
             oyjl_string_add( json, "\n");
             n = *level; while(n--) oyjl_string_add(json, " ");
             oyjl_string_add( json, "\"%s\": ", v->u.object.keys[i] );
             oyjl_tree_to_json( v->u.object.values[i], level, json );
             if(count > 1)
             {
               if(i < count - 1)
                 oyjl_string_add( json, "," );
             }
           }
           *level -= 2;

           oyjl_string_add( json, "\n");
           n = *level; while(n--) oyjl_string_add(json, " ");
           oyjl_string_add( json, "}");
         }
         break;
    default:
         fprintf( stderr, "unknown type: %d\n", v->type );
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

char **        oyjl_string_split     ( const char        * text,
                                       int               * count )
{
  char ** list = 0;
  int n = 0, i;
  char delimiter = '/';

  /* split the path search string by a delimiter */
  if(text && text[0] && delimiter)
  {
    const char * tmp = text;

    if(tmp[0] == delimiter) ++n;
    do { ++n;
    } while( (tmp = strchr(tmp + 1, delimiter)) );

    tmp = 0;

    if((list = malloc( (n+1) * sizeof(char*) )) == 0) return NULL;

    {
      const char * start = text;
      for(i = 0; i < n; ++i)
      {
        intptr_t len = 0;
        char * end = strchr(start, delimiter);

        if(end > start)
          len = end - start;
        else if (end == start)
          len = 0;
        else
          len = strlen(start);

        if((list[i] = malloc( len+1 )) == 0) return NULL;

        memcpy( list[i], start, len );
        list[i][len] = 0;
        start += len + 1;
      }
    }
  }

  *count = n;

  return list;
}

oyjl_val   oyjl_tree_get_value       ( oyjl_val            v,
                                       const char        * xpath )
{
  oyjl_val level = 0;
  int n = 0, i, found = 0;
  char ** list = oyjl_string_split(xpath, &n),
        * ttmp = 0;

  /* follow the search path term */
  level = v;
  found = n;
  for(i = 0; i < n; ++i)
  {
    char * term = list[i],
         * tindex = strrchr(term,'[');
    int count = oyjl_value_count( level );
    int j;
    int pos = -1;

    
    if(tindex != NULL)
    {
      ptrdiff_t size;
      ++tindex;
      size = strrchr(term,']') - tindex;
      if(size > 0)
      {
        ttmp = malloc(size + 1);
        memcpy( ttmp, tindex, size );
        ttmp[size] = '\000';
        pos = atoi(ttmp);
        size = strrchr(term,'[') - term;
        memcpy( ttmp, term, size );
        ttmp[size] = '\000';
        term = ttmp;
      }
    }

    if(found == 0) break;
    found = 0;

    if(!(term && term[0]) && pos != -1)
    {
      level = oyjl_value_pos_get( level, pos );
      found = 1;
    } else
    for(j = 0; j < count; ++j)
    {
        if(term &&
           strcmp( level->u.object.keys[j], term ) == 0)
        {
          ++found;
          if(pos == -1 ||
             (found-1) == pos)
          {
            level = level->u.object.values[j];
            break;
          }
        }
    }
  }

  /* clean up temorary memory */
  for(i = 0; i < n; ++i)
    free(list[i]);
  if(list)
    free(list);

  if(ttmp)
    free( ttmp );

  if(found && level)
    return level;
  else
    return NULL;
}

/** Function oyjl_tree_get_valuef
 *  @brief   get a child node
 *
 *  @param[in]     v                   the oyjl node
 *  @param[in]     format              the xpath format
 *  @param[in]     ...                 the variable argument list
 *  @return                            the childs text value
 *
 *  @version Oyranos: 0.9.5
 *  @since   2011/09/24 (Oyranos: 0.3.3)
 *  @date    2013/02/24
 */
oyjl_val   oyjl_tree_get_valuef      ( oyjl_val            v,
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
    fprintf( stderr, "!!! ERROR: could not allocate memory\n" );
    return 0;
  }

  text[0] = 0;

  va_start( list, format);
  len = vsnprintf( text, sz, format, list );
  va_end  ( list );

  if (len >= sz)
  {
    text = realloc( text, (len+1)*sizeof(char) );
    va_start( list, format);
    len = vsnprintf( text, len+1, format, list );
    va_end  ( list );
  }

  value = oyjl_tree_get_value( v, text );

  if(text) free(text);

  return value;
}


void oyjl_tree_free (oyjl_val v)
{
    if (v == NULL) return;

    if (OYJL_IS_STRING(v))
    {
        free(v->u.string);
        free(v);
    }
    else if (OYJL_IS_NUMBER(v))
    {
        free(v->u.number.r);
        free(v);
    }
    else if (OYJL_GET_OBJECT(v))
    {
        oyjl_object_free(v);
    }
    else if (OYJL_GET_ARRAY(v))
    {
        oyjl_array_free(v);
    }
    else /* if (oyjl_t_true or oyjl_t_false or oyjl_t_null) */
    {
        free(v);
    }
}
