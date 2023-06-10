/** @file oyjl_yajl.c
 *
 *  oyjl - Yajl tree extension
 *
 *  @par Copyright:
 *            2016-2022 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl parsing functions
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de> and Florian Forster  <ff at octo.it>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2016/12/17
 */


#include <stdarg.h>  /* va_list */
#include <stddef.h>  /* ptrdiff_t size_t */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>

#include "oyjl_version.h"
#include <yajl/yajl_parse.h>
#ifndef YAJL_VERSION
#include <yajl/yajl_version.h>
#endif

#include "oyjl.h"
#include "oyjl_macros.h"
#include "oyjl_tree_internal.h"
#ifdef OYJL_HAVE_LOCALE_H
#include <locale.h>
#endif

/** \addtogroup oyjl_tree OyjlTree JSON Parsing and Tree Handling
 *  @brief Tree data structure manipulation and I/O
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
 *  are oyjlTreeToPaths() and oyjlPathMatch(). oyjlTreeToPaths()
 *  works on the whole tree to match a extended xpath.
 *
 *  \b Programming \b Tutorial
 *
 *  The following code examples come from @ref tutorial_json_options.c . 
 *  @dontinclude tutorial_json_options.c
 *  @skip testOyjl(void)
 *  @until oyjlTreeFree(
 *  @line }
 */


#define Florian_Forster_SOURCE_GUARD
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
#define STATUS_CONTINUE 1
#define STATUS_ABORT    0

struct oyjl_tree_stack_elem_s;
typedef struct oyjl_tree_stack_elem_s oyjl_tree_stack_elem_t;
struct oyjl_tree_stack_elem_s
{
    char * key;
    oyjl_val value;
    oyjl_tree_stack_elem_t *next;
};

struct oyjl_tree_context_s
{
    oyjl_tree_stack_elem_t *stack;
    oyjl_val root;
    char *errbuf;
    size_t errbuf_size;
};
typedef struct oyjl_tree_context_s oyjl_tree_context_t;

#define RETURN_ERROR(ctx,retval,...) {                                  \
        if ((ctx)->errbuf != NULL)                                      \
            snprintf ((ctx)->errbuf, (ctx)->errbuf_size, __VA_ARGS__);  \
        return (retval);                                                \
    }

/*
 * Parsing nested objects and arrays is implemented using a stack. When a new
 * object or array starts (a curly or a square opening bracket is read), an
 * appropriate value is pushed on the stack. When the end of the object is
 * reached (an appropriate closing bracket has been read), the value is popped
 * off the stack and added to the enclosing object using "oyjlTreeParseYajlContextAddValue_".
 */
static int oyjlTreeParseYajlContextPush_(oyjl_tree_context_t *ctx, oyjl_val v)
{
    oyjl_tree_stack_elem_t *stack;

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

static oyjl_val oyjlTreeParseYajlContextPop_(oyjl_tree_context_t *ctx)
{
    oyjl_tree_stack_elem_t *stack;
    oyjl_val v;

    if (ctx->stack == NULL)
        RETURN_ERROR (ctx, NULL, "oyjlTreeParseYajlContextPop_: "
                      "Bottom of stack reached prematurely");

    stack = ctx->stack;
    ctx->stack = stack->next;

    v = stack->value;

    free (stack);

    return (v);
}

static int oyjlTreeParseYajlObjectAddKeyval_(oyjl_tree_context_t *ctx,
                             oyjl_val obj, char *key, oyjl_val value)
{
    char **tmpk;
    oyjl_val *tmpv;

    /* We're checking for NULL in "oyjlTreeParseYajlContextAddValue_" or its callers. */
    assert (ctx != NULL);
    assert (obj != NULL);
    assert (key != NULL);
    assert (value != NULL);

    /* We're assuring that "obj" is an object in "oyjlTreeParseYajlContextAddValue_". */
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

static int oyjlTreeParseYajlArrayAddValue_ (oyjl_tree_context_t *ctx,
                            oyjl_val array, oyjl_val value)
{
    oyjl_val *tmp;

    /* We're checking for NULL pointers in "oyjlTreeParseYajlContextAddValue_" or its
     * callers. */
    assert (ctx != NULL);
    assert (array != NULL);
    assert (value != NULL);

    /* "oyjlTreeParseYajlContextAddValue_" will only call us with array values. */
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
static int oyjlTreeParseYajlContextAddValue_ (oyjl_tree_context_t *ctx, oyjl_val v)
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
                RETURN_ERROR (ctx, EINVAL, "oyjlTreeParseYajlContextAddValue_: "
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
            return (oyjlTreeParseYajlObjectAddKeyval_ (ctx, ctx->stack->value, key, v));
        }
    }
    else if (OYJL_IS_ARRAY (ctx->stack->value))
    {
        return (oyjlTreeParseYajlArrayAddValue_ (ctx, ctx->stack->value, v));
    }
    else
    {
        RETURN_ERROR (ctx, EINVAL, "oyjlTreeParseYajlContextAddValue_: Cannot add value to "
                      "a value of type %#04x (not a composite type)",
                      ctx->stack->value->type);
    }
}

#if (YAJL_VERSION) > 20000
static int oyjlTreeParseYajlHandleString_ (void *ctx,
                          const unsigned char *string, long unsigned int string_length)
#else
static int oyjlTreeParseYajlHandleString_ (void *ctx, const char *string, unsigned int string_length)
#endif
{
    oyjl_val v;

    v = oyjlValueAlloc_ (oyjl_t_string);
    if (v == NULL)
        RETURN_ERROR ((oyjl_tree_context_t *) ctx, STATUS_ABORT, "Out of memory");

    v->u.string = malloc (string_length + 1);
    if (v->u.string == NULL)
    {
        free (v);
        RETURN_ERROR ((oyjl_tree_context_t *) ctx, STATUS_ABORT, "Out of memory");
    }
    memcpy(v->u.string, string, string_length);
    v->u.string[string_length] = 0;

    return ((oyjlTreeParseYajlContextAddValue_ (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

#if (YAJL_VERSION) > 20000
static int oyjlTreeParseYajlHandleNumber_ (void *ctx, const char *string, size_t string_length)
#else
static int oyjlTreeParseYajlHandleNumber_ (void *ctx, const char *string, unsigned int string_length)
#endif
{
    oyjl_val v;
    char *endptr;

    v = oyjlValueAlloc_(oyjl_t_number);
    if (v == NULL)
        RETURN_ERROR((oyjl_tree_context_t *) ctx, STATUS_ABORT, "Out of memory");

    v->u.number.r = malloc(string_length + 1);
    if (v->u.number.r == NULL)
    {
        free(v);
        RETURN_ERROR((oyjl_tree_context_t *) ctx, STATUS_ABORT, "Out of memory");
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
#ifdef OYJL_HAVE_LOCALE_H
    char * save_locale = oyjlStringCopy( setlocale(LC_NUMERIC, 0 ), malloc );
    setlocale(LC_NUMERIC, "C");
#endif
    v->u.number.d = strtod(v->u.number.r, &endptr);
#ifdef OYJL_HAVE_LOCALE_H
    setlocale(LC_NUMERIC, save_locale);
    if(save_locale)
      free( save_locale );
#endif
    if ((errno == 0) && (endptr != NULL) && (*endptr == 0))
        v->u.number.flags |= OYJL_NUMBER_DOUBLE_VALID;

    return ((oyjlTreeParseYajlContextAddValue_(ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int oyjlTreeParseYajlHandleStartMap_ (void *ctx)
{
    oyjl_val v;

    v = oyjlValueAlloc_(oyjl_t_object);
    if (v == NULL)
        RETURN_ERROR ((oyjl_tree_context_t *) ctx, STATUS_ABORT, "Out of memory");

    v->u.object.keys = NULL;
    v->u.object.values = NULL;
    v->u.object.len = 0;

    return ((oyjlTreeParseYajlContextPush_ (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int oyjlTreeParseYajlHandleEndMap_ (void *ctx)
{
    oyjl_val v;

    v = oyjlTreeParseYajlContextPop_ (ctx);
    if (v == NULL)
        return (STATUS_ABORT);

    return ((oyjlTreeParseYajlContextAddValue_ (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int oyjlTreeParseYajlHandleStartArray_ (void *ctx)
{
    oyjl_val v;

    v = oyjlValueAlloc_(oyjl_t_array);
    if (v == NULL)
        RETURN_ERROR ((oyjl_tree_context_t *) ctx, STATUS_ABORT, "Out of memory");

    v->u.array.values = NULL;
    v->u.array.len = 0;

    return ((oyjlTreeParseYajlContextPush_ (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int oyjlTreeParseYajlHandleEndArray_ (void *ctx)
{
    oyjl_val v;

    v = oyjlTreeParseYajlContextPop_ (ctx);
    if (v == NULL)
        return (STATUS_ABORT);

    return ((oyjlTreeParseYajlContextAddValue_ (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int oyjlTreeParseYajlHandleBoolean_ (void *ctx, int boolean_value)
{
    oyjl_val v;

    v = oyjlValueAlloc_ (boolean_value ? oyjl_t_true : oyjl_t_false);
    if (v == NULL)
        RETURN_ERROR ((oyjl_tree_context_t *) ctx, STATUS_ABORT, "Out of memory");

    return ((oyjlTreeParseYajlContextAddValue_ (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

static int oyjlTreeParseYajlHandleNull_ (void *ctx)
{
    oyjl_val v;

    v = oyjlValueAlloc_ (oyjl_t_null);
    if (v == NULL)
        RETURN_ERROR ((oyjl_tree_context_t *) ctx, STATUS_ABORT, "Out of memory");

    return ((oyjlTreeParseYajlContextAddValue_ (ctx, v) == 0) ? STATUS_CONTINUE : STATUS_ABORT);
}

/*
 * Public functions
 */

/** \addtogroup oyjl_tree
 *  @{ *//* oyjl_tree */

oyjl_val oyjlTreeParse   (const char *input,
                          char *error_buffer, size_t error_buffer_size)
{
#if (YAJL_VERSION) > 20000
static yajl_callbacks oyjl_tree_parse_callbacks_ = {
  oyjlTreeParseYajlHandleNull_,
  oyjlTreeParseYajlHandleBoolean_,
  NULL, //oyjl_tree_handle_integer,
  NULL, //oyjl_tree_handle_double,
  oyjlTreeParseYajlHandleNumber_,
  oyjlTreeParseYajlHandleString_,
  oyjlTreeParseYajlHandleStartMap_,
  oyjlTreeParseYajlHandleString_,
  oyjlTreeParseYajlHandleEndMap_,
  oyjlTreeParseYajlHandleStartArray_,
  oyjlTreeParseYajlHandleEndArray_
};
#else
static yajl_callbacks oyjl_tree_parse_callbacks_ = {
  oyjlTreeParseYajlHandleNull_,
  oyjlTreeParseYajlHandleBoolean_,
  NULL, //oyjl_tree_handle_integer,
  NULL, //oyjl_tree_handle_double,
  oyjlTreeParseYajlHandleNumber_,
  oyjlTreeParseYajlHandleString_,
  oyjlTreeParseYajlHandleStartMap_,
  oyjlTreeParseYajlHandleString_,
  oyjlTreeParseYajlHandleEndMap_,
  oyjlTreeParseYajlHandleStartArray_,
  oyjlTreeParseYajlHandleEndArray_
};
#endif
#if 0
    static const yajl_callbacks callbacks =
        {
            /* null        = */ oyjlTreeParseYajlHandleNull_,
            /* boolean     = */ oyjlTreeParseYajlHandleBoolean_,
            /* integer     = */ NULL,
            /* double      = */ NULL,
            /* number      = */ oyjlTreeParseYajlHandleNumber_,
            /* string      = */ oyjlTreeParseYajlHandleString_,
            /* start map   = */ oyjlTreeParseYajlHandleStartMap_,
            /* map key     = */ oyjlTreeParseYajlHandleString_,
            /* end map     = */ oyjlTreeParseYajlHandleEndMap_,
            /* start array = */ oyjlTreeParseYajlHandleStartArray_,
            /* end array   = */ oyjlTreeParseYajlHandleEndArray_
        };
#endif
    yajl_handle handle;
#if YAJL_VERSION < 20000
    yajl_parser_config yconfig = { 1, 1 };
#endif
    yajl_status status;
    char * internal_err_str;
	oyjl_tree_context_t ctx = { NULL, NULL, NULL, 0 };

  if(!input) return NULL;

  ctx.errbuf = error_buffer;
	ctx.errbuf_size = error_buffer_size;

    if (error_buffer != NULL)
        memset (error_buffer, 0, error_buffer_size);

    handle = yajl_alloc( &oyjl_tree_parse_callbacks_,
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
    if (status != yajl_status_ok)
    {
        char * t;
        internal_err_str = (char *) yajl_get_error(handle, 1,
                     (const unsigned char *) input,
                     strlen(input));
        if (error_buffer != NULL && error_buffer_size > 0)
             snprintf(error_buffer, error_buffer_size, "%s", internal_err_str);
#if YAJL_VERSION > 19999
        status = yajl_complete_parse (handle);
#endif
        t = oyjlBT(0);
        oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "%s\n%s", OYJL_DBG_ARGS,
                       internal_err_str, t );
        free(t);
        yajl_free_error( handle, (unsigned char*)internal_err_str );
        internal_err_str = 0;
        yajl_free (handle);
        while(ctx.stack)
        {
          if(ctx.stack->key)
            free(ctx.stack->key);
          ctx.stack->key = NULL;
          if(ctx.stack->value)
          {
            oyjlValueClear(ctx.stack->value);
            free(ctx.stack->value);
            ctx.stack->value = NULL;
          }
          oyjlTreeParseYajlContextPop_(&ctx);
        }
        oyjlTreeFree( ctx.root );
        return NULL;
    }

#if YAJL_VERSION > 19999
    status = yajl_complete_parse (handle);
#endif
    yajl_free (handle);
    return (ctx.root);
}
#undef Florian_Forster_SOURCE_GUARD

/** @brief obtain a new node object possibly in array
 *
 *  The node can even have the same name. It works only for flat path level.
 *
 *  @param[in,out] root                the node
 *  @param[in]     name                flat path, without leveling slash
 *  @param[out]    array_ret           tell if the node is child of that array
 *  @param[out]    pos_ret             position of returned node in array_ret
 *                                     - -1 means object is direct child without array
 *                                     - >= 0 -> index in array
 *  @return                            new node
 */
oyjl_val oyjlTreeGetNewValueFromArray( oyjl_val            root,
                                       const char        * name,
                                       oyjl_val          * array_ret,
                                       int               * pos_ret )
{
  oyjl_val array = NULL, node;
  int pos = -1;
  node = oyjlTreeGetValue( root, 0, name );
  if(node)
  {
    pos = oyjlValueCount( node );
    if(node->type != oyjl_t_array)
    {
      oyjl_val copy;
      size_t size = sizeof(*copy);
      copy = calloc( 1, size );
      if(!copy) return NULL;
      memcpy( copy, node, size );
      memset( node, 0, size );
      array = node;
      array->type = oyjl_t_array;
      node = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, "%s/[0]", name);
      memcpy( node, copy, size );
      free(copy); copy = NULL;
      pos = 1;
      node = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, "%s/[%d]", name, pos);
    }
    else
    {
      array = oyjlTreeGetValue( root, 0, name );
      node = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, "%s/[%d]", name, pos);
    }
  }
  else
    node = oyjlTreeGetValue( root, OYJL_CREATE_NEW, name );

  if(array_ret)
    *array_ret = array;
  if(pos_ret)
    *pos_ret = pos;

  return node;
}
/** @} *//* oyjl_tree */

#if defined(OYJL_HAVE_LIBXML2) || defined(DOXYGEN)
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "oyjl.h"


char *             oyjlTreeParseXMLNodeName  ( xmlNodePtr          cur )
{
  char * name = NULL;
  const xmlChar * prefix = cur->ns && cur->ns->prefix ? cur->ns->prefix : 0;
  if(prefix)
  {
    oyjlStringAdd( &name, 0,0, (char*)prefix );
    oyjlStringAdd( &name, 0,0, ":" );
  }
  oyjlStringAdd( &name, 0,0, (char*)cur->name );
  return name;
}

int              oyjlTreeParseXMLNodeIsText   ( xmlNodePtr          cur )
{
  return cur->type == XML_TEXT_NODE && !(cur->next || cur->prev) &&
         cur->content ;
}

int              oyjlTreeParseXMLNodeIsCData  ( xmlNodePtr          cur )
{
  return cur->type == XML_CDATA_SECTION_NODE && !(cur->next || cur->prev) &&
         cur->content ;
}

void             oyjlTreeParseXMLDoc_( xmlDocPtr           doc,
                                       xmlNodePtr          cur,
                                       int                 flags,
                                       oyjl_val            root )
{
  while(cur != NULL)
  {
    char * name = 0;
    oyjl_val node = NULL;

    if(cur->type == XML_ELEMENT_NODE)
    {
      oyjl_val array = NULL;
      int count = -1;
      name = oyjlTreeParseXMLNodeName( cur );
      node = oyjlTreeGetNewValueFromArray( root, name, &array, &count );

      if(cur->nsDef)
      {
        xmlNsPtr xcur = cur->nsDef; 
        while(xcur != NULL)
        {
          const char * prefix = (const char *) xcur->prefix;
          const char * val = (const char *) xcur->href;
          char * attr = NULL;
          oyjl_val prop;
          oyjlStringAdd( &attr, 0,0, "xmlns:%s", prefix );
          if(!attr) return;
          if(array)
            prop = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, "%s/[%d]/@%s", name, count, attr );
          else
            prop = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, "%s/@%s", name, attr );
          oyjlValueSetString( prop, val );
          free( attr );
          xcur = xcur->next;
        }
      }

      if(cur->properties)
      {
        xmlAttrPtr xcur = cur->properties; 
        while(xcur != NULL)
        {
          const char * attr = (const char *) xcur->name;
          const char * val = (const char *) xcur->children->content;
          oyjl_val prop;
          if(array)
            prop = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, "%s/[%d]/@%s", name, count, attr );
          else
            prop = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, "%s/@%s", name, attr );
          oyjlValueSetString( prop, val );
          xcur = xcur->next;
        }
      }
    }
    else
    if( oyjlTreeParseXMLNodeIsText(cur) )
    {
      const char * val = (const char *) cur->content;
      double d;
      int err = -1;

      if(flags & OYJL_NUMBER_DETECTION)
        err = oyjlStringToDouble( val, &d, 0,0 );
      if(err == 0)
      {
        root->type = oyjl_t_number;
        root->u.number.r = strdup(val);
        root->u.number.d = d;
        root->u.number.flags |= OYJL_NUMBER_DOUBLE_VALID;
        errno = 0;
        root->u.number.i = strtol(root->u.number.r, 0, 10);
        if (errno == 0)
          root->u.number.flags |= OYJL_NUMBER_INT_VALID;
      } else if(flags & OYJL_NUMBER_DETECTION)
      {
        if(strcmp(val,"true") == 0)
        {
          err = 0;
          root->type = oyjl_t_true;
        } else if(strcmp(val,"false") == 0)
        {
          err = 0;
          root->type = oyjl_t_false;
        }
      }

      if(err != 0)
        oyjlValueSetString( root, val );

    } else
    if( oyjlTreeParseXMLNodeIsCData(cur) )
    {
      const char * val = (const char *) cur->content;
      oyjlValueSetString( root, val );
    }

    if(cur->xmlChildrenNode)
    {
      oyjl_val text = NULL;
      xmlNodePtr cur_ = cur->xmlChildrenNode;

      if( oyjlTreeParseXMLNodeIsText(cur_) && node && node->type == oyjl_t_object )
        text = oyjlTreeGetValue( node, OYJL_CREATE_NEW, "@text" );
      else
      if( oyjlTreeParseXMLNodeIsCData(cur_) && node && (node->type == oyjl_t_object ||
                                               node->type == oyjl_t_null) )
        text = oyjlTreeGetValue( node, OYJL_CREATE_NEW, "@cdata" );
      /*else
      if( cur->children && cur->children->type == XML_CDATA_SECTION_NODE && !(cur->children->next || cur->children->prev) &&
         cur->children->content )
        text = oyjlTreeGetValue( node, OYJL_CREATE_NEW, "@cdata" );*/

      oyjlTreeParseXMLDoc_( doc, cur->xmlChildrenNode, flags,
                        text ? text : node );
    }

    if(name)
      free( name );

    cur = cur->next;
  }
}

/** \addtogroup oyjl_tree
 *  @{ *//* oyjl_tree */
/** @brief read a XML text string into a C data structure (libOyjl)
 *
 *  XML attributes are prefixed with the at '@' char. Inner strings are placed
 *  as objects with key '\@text'. Repeating XML nodes are placed into a array
 *  below a object with the key name of the nodes.
 *
 *  This function needs linking to libOyjl.
 *
 *  @see oyjlTreeToXml()
 *
 *  @param[in]     xml                 the XML text
 *  @param[in]     flags               for processing
 *                                     - ::OYJL_NUMBER_DETECTION for parsing
 *                                       of inner strings as possibly numbers
 *  @param[out]    error_buffer        place a error message
 *  @param[out]    error_buffer_size   size of error_buffer
 *  @return                            object tree on success,
 *                                     else check error_buffer
 */
oyjl_val   oyjlTreeParseXml          ( const char        * xml,
                                       int                 flags,
                                       char              * error_buffer,
                                       size_t              error_buffer_size)
{
  xmlDocPtr doc = NULL;
  xmlNodePtr cur = NULL;
  oyjl_val jroot =  NULL;

  if(!xml) return jroot;

  doc = xmlParseMemory( xml, strlen(xml) );
  cur = xmlDocGetRootElement(doc);

  if(doc && cur)
  {
    jroot = oyjlTreeNew( NULL );
    oyjlTreeParseXMLDoc_( doc, cur, flags, jroot );
  }
  else if(error_buffer)
    snprintf( error_buffer, error_buffer_size, "XML loading failed" );

  if(error_buffer && error_buffer[0])
  {
    char * t = oyjlBT(0);
    oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "%s\n%s",
                         OYJL_DBG_ARGS, error_buffer, t );
    free(t);
  }

  if(doc)
    xmlFreeDoc( doc );

  return jroot;
}
/** @} *//* oyjl_tree */
#endif

#if defined(OYJL_HAVE_YAML) || defined(DOXYGEN)
#include <yaml.h>
int oyjlTreeParseYamlGetCount( yaml_node_t * n )
{
  int i = 0;
  if( !n ) return i;
  switch( n->type )
  {
    case YAML_SCALAR_NODE: i = n->data.scalar.length; break;
    case YAML_SEQUENCE_NODE: i = n->data.sequence.items.top - n->data.sequence.items.start; break;
    case YAML_MAPPING_NODE: i = n->data.mapping.pairs.top - n->data.mapping.pairs.start; break;
    default: break;
  }
  return i;
}

int oyjlTreeParseYamlGetId( yaml_node_t * n, int index, int key )
{
  int id = 0;
  if( !n ) return id;
  if( n->type == YAML_SEQUENCE_NODE )
    id = n->data.sequence.items.start[index];

  if( n->type == YAML_MAPPING_NODE )
  {
    if(key == 1)
      id = n->data.mapping.pairs.start[index].key;
    else
      id = n->data.mapping.pairs.start[index].value;
  }

  return id;
}

static int oyjlTreeParseYamlReadNode_( yaml_document_t * doc, yaml_node_t * node, int flags, int is_key, char ** json )
{
  int error = 0;
  int count, i;
  if( !node ) return 1;
  count = oyjlTreeParseYamlGetCount( node );
  if( node->type == YAML_SCALAR_NODE )
  {
    char * t = (char*)node->data.scalar.value,
         * tmp = oyjlStringCopy(t,malloc);
    oyjlStringReplace( &tmp, ":\\ ", ": ", 0, 0);
    if(tmp)
    {
      double d;
      int err = -1;
      if(flags & OYJL_NUMBER_DETECTION && is_key != 1)
        err = oyjlStringToDouble( tmp, &d, 0,0 );
      if(err == 0)
        oyjlStringAdd( json, 0,0, "%s", tmp );
      else
        oyjlStringAdd( json, 0,0, "\"%s\"", tmp );
    }
    if(tmp) free(tmp);
  }
  if( node->type == YAML_SEQUENCE_NODE )
  {
    oyjlStringAdd( json, 0,0, "[");
    for(i = 0; i < count && !error; ++i)
    {
      int id = oyjlTreeParseYamlGetId( node, i, 0 );
      yaml_node_t * n =
      yaml_document_get_node( doc, id );
      error = oyjlTreeParseYamlReadNode_(doc, n, flags, 0, json);
      if(i < count - 1) oyjlStringAdd( json, 0,0, ",");
    }
    oyjlStringAdd( json, 0,0, "]");
  }
  if( node->type == YAML_MAPPING_NODE )
    for(i = 0; i < count; ++i)
    {
      int key_id = oyjlTreeParseYamlGetId( node, i, 1 );
      int val_id = oyjlTreeParseYamlGetId( node, i, 0 );
      yaml_node_t * key =
      yaml_document_get_node( doc, key_id );
      yaml_node_t * val =
      yaml_document_get_node( doc, val_id );

      if(i == 0) oyjlStringAdd( json, 0,0, "{");

      error = oyjlTreeParseYamlReadNode_(doc, key, flags, 1, json);
      if( key->type == YAML_SCALAR_NODE &&
          !error )
      {
        oyjlStringAdd( json, 0,0, ":");
      }

      error = oyjlTreeParseYamlReadNode_(doc, val, flags, 0, json);
      if(i < count - 1) oyjlStringAdd( json, 0,0, ",");
      else if( i == count - 1 ) oyjlStringAdd( json, 0,0, "}");
    }
  return error;
}

/** \addtogroup oyjl_tree
 *  @{ *//* oyjl_tree */
/** @brief read a YAML text string into a C data structure (libOyjl)
 *
 *  This function needs linking to libOyjl.
 *
 *  @see oyjlTreeToYaml()
 *
 *  @param[in]     yaml                the YAML text
 *  @param[in]     flags               for processing
 *                                     - ::OYJL_NUMBER_DETECTION for parsing
 *                                       of values as possibly numbers
 *  @param[out]    error_buffer        place a error message
 *  @param[out]    error_buffer_size   size of error_buffer
 *  @return                            object tree on success,
 *                                     else check error_buffer
 */
oyjl_val   oyjlTreeParseYaml         ( const char        * yaml,
                                       int                 flags,
                                       char              * error_buffer,
                                       size_t              error_buffer_size)
{
  yaml_parser_t parser;
  yaml_document_t document;
  yaml_node_t * root = NULL;
  char * json = NULL;
  oyjl_val jroot = NULL;
  int error = 0;

  if(!yaml) return jroot;

  if(!yaml_parser_initialize(&parser))
  {
    if(error_buffer)
      snprintf( error_buffer, error_buffer_size, "YAML initialisation failed" );
    oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "%s", OYJL_DBG_ARGS,
                   "YAML initialisation failed" );
    return jroot;
  }

  yaml_parser_set_input_string( &parser, (const unsigned char*) yaml, strlen(yaml));

  yaml_parser_load( &parser, &document );
  if( parser.error != YAML_NO_ERROR)
  {
    if(error_buffer)
      snprintf( error_buffer, error_buffer_size, "%s\n", parser.problem ? parser.problem : "" );
    oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "%s\n%s", OYJL_DBG_ARGS,
                   parser.problem ? parser.problem : "", yaml );

    return jroot;
  }

  root = yaml_document_get_root_node(&document);
  error = oyjlTreeParseYamlReadNode_( &document, root, flags, 1, &json );
  if( error )
  {
    if(error_buffer)
      snprintf( error_buffer, error_buffer_size, "Found problem while parsing document tree.\n" );
    oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "%s", OYJL_DBG_ARGS,
                   "Found problem while parsing document tree." );
    if(json) free(json);
    json = NULL;
    return jroot;
  }

  jroot = oyjlTreeParse( json, error_buffer, error_buffer_size );

  yaml_parser_delete(&parser);
  yaml_document_delete(&document);
  if(json) free(json);

  return jroot;
}
#endif

int oyjlIsNumber( const char c )
{
  if(((int)'0' <= (int)c &&
        (int)c <= (int)'9') ||
       c == '.' ||
       c == '-' ||
       c == 'e' ||
       c == 'E' ||
       c == ' '
      )
    return 1;
  return 0;
}

/** @brief read a CSV text string into a C data structure (libOyjl)
 *
 *  Expected is a 2D table on input.
 *
 *  @see oyjlTreeToCsv()
 *
 *  @param[in]     text                the CSV text
 *  @param[in]     flags               for processing
 *                                     - ::OYJL_NUMBER_DETECTION for parsing
 *                                       of values as possibly numbers
 *                                     - ::OYJL_DELIMITER_COMMA: default
 *                                     - ::OYJL_DELIMITER_SEMICOLON: ';'
 *  @param[out]    error_buffer        place a error message
 *  @param[out]    error_buffer_size   size of error_buffer
 *  @return                            object tree on success,
 *                                     else check error_buffer
 */
oyjl_val   oyjlTreeParseCsv          ( const char        * text,
                                       int                 flags,
                                       char              * error_buffer OYJL_UNUSED,
                                       size_t              error_buffer_size OYJL_UNUSED)
{
  oyjl_val jroot = NULL;
  int rows_n = 0, /* lines */
      cols_n = 0, len;
  char delimiter = ',';
  char ** rows, ** cols, * row;
  if(!text) return jroot;
  if(flags & OYJL_DELIMITER_SEMICOLON)
    delimiter = ';';
  if(*oyjl_debug)
    fprintf( stderr, "lines: %d\n", rows_n );

#ifdef OYJL_HAVE_LOCALE_H
  char * save_locale = oyjlStringCopy( setlocale(LC_NUMERIC, 0 ), malloc );
  if(flags & OYJL_NUMBER_DETECTION)
    setlocale(LC_NUMERIC, "C");
#endif

  rows = oyjlStringSplit( text, '\n', &rows_n, malloc );
  cols = oyjlStringSplit( rows[0], delimiter, &cols_n, malloc );
  oyjlStringListRelease( &cols, cols_n, free );

  if(cols_n >= 1)
  {
    int i,index;
    jroot = oyjlTreeNew( "" );
    cols_n = 0;

    for(i = 0; i < rows_n; ++i)
    {
      oyjl_val row_node, node;
      row = rows[i];
      len = strlen(row);
      if(len > 1 && row[len-1] == '\r')
        row[len-1] = '\000'; /* clean DOS linebreak '\r\n' */
      cols = oyjlStringSplit( row, delimiter, &cols_n, malloc );

      row_node = oyjlTreeGetValueF( jroot, OYJL_CREATE_NEW, "[%d]", i );
      row_node->type = oyjl_t_array;
      oyjlAllocHelper_m( row_node->u.array.values, oyjl_val, cols_n + 1, malloc,  goto clean_parse_csv );
      row_node->u.array.len = cols_n + 1;
      for(index = 0; index < cols_n; ++index)
      {
        node = NULL;
        node = calloc( 1, sizeof(*node) );
        if(!node) goto clean_parse_csv;
        node->type = oyjl_t_null;
        row_node->u.array.values[index] = node;
      }

      for(index = 0; index < cols_n; ++index)
      {
        int err = -1;
        double d = -1;
        char * val = cols[index];

        node = row_node->u.array.values[index];

        if(flags & OYJL_NUMBER_DETECTION)
        {
          char * number = oyjlStringCopy( val, 0 );
          if(flags & OYJL_DECIMAL_SEPARATOR_COMMA)
          {
            char * t = strrchr( number, ',' );
            if(t) t[0] = '.';
          }
          err = oyjlStringToDouble( number, &d, 0, OYJL_KEEP_LOCALE );
          if(err == 0)
            oyjlValueSetDouble( node, d );
          if(err == 0)
          {
            free(node->u.number.r);
            node->u.number.r = number;
            free(val);
            val = cols[index] = NULL;
          }
          else if(err != 0)
          {
            len = strlen(val);
            if(oyjlStringStartsWith(val,"true"))
            {
              err = 0;
              node->type = oyjl_t_true;
            } else if(oyjlStringStartsWith(val,"false"))
            {
              err = 0;
              node->type = oyjl_t_false;
            }
            if(number) { free(number); number = NULL; }
          }
        }

        if(err != 0)
          oyjlValueSetString( node, val );
      }

      if(text)
      {
        text = strchr( text, '\n' );
        if(text) ++text;

        if(*oyjl_debug > 1) fprintf( stderr, "\n" );
      }
      oyjlStringListRelease( &cols, cols_n, free ); cols_n = 0;
    }
  }
  oyjlStringListRelease( &rows, rows_n, free );

clean_parse_csv:
#ifdef OYJL_HAVE_LOCALE_H
  if(flags & OYJL_NUMBER_DETECTION)
    setlocale(LC_NUMERIC, save_locale);
  if(save_locale) free( save_locale );
  oyjlStringListRelease( &cols, cols_n, free );
#endif

  return jroot;
}


const char*oyjlPARSE_STATE_eToString ( int                 state )
{
  switch(state)
  {
    case oyjlPARSE_STATE_NONE: return "nothing to report";
    case oyjlPARSE_STATE_NOT_COMPILED: return "the format is currently not compiled";
    case oyjlPARSE_STATE_FORMAT_ERROR: return "check with oyjlDataFormat()";
    case oyjlPARSE_STATE_PARSER_ERROR: return "message is sent to oyjlMessage_p";
    case oyjlPARSE_STATE_RETURN_STATIC: return "oyjl_val is static and must not be freed; needs OYJL_ALLOW_STATIC flag";
    default: return "----";
  }
}
/** @brief read a JSON, XML or YAML text string into a C data structure (libOyjl)
 *
 *  This function needs linking to libOyjl.
 *
 *  JSON, Xml and Yaml parsers can not handle term color codes
 *  in the data structures. They need to be removed before parsing.
 *
 *  @code
    const char * data_text = "{ \"key\": true }";
    int status = 0;
    oyjl_val root = oyjlTreeParse2( data_text, OYJL_NO_MARKUP, __func__, &status );
    if(state)
      fprintf( stderr, "%s() found issue: %s\n", __func__, oyjlPARSE_STATE_eToString(status) );
    @endcode
 *
 *  @see oyjlTreeParse() oyjlTreeParseXml oyjlTreeParseYaml() oyjlTreeToText() oyjlDataFormat()
 *
 *  @param[in]     text                the JSON/XML/YAML text
 *  @param[in]     flags               for processing
 *                                     - ::OYJL_NUMBER_DETECTION for parsing
 *                                       of values as possibly numbers
 *                                     - ::OYJL_QUIET for error reporting as oyjlMSG_INFO
 *                                     - ::OYJL_NO_MARKUP omit remove of term color codes;
 *                                       deactivates oyjlTermColorToPlain( flags )
 *                                     - ::OYJL_REGEXP for oyjlTermColorToPlain()
 *  @param[in]     error_name          add text for error messages
 *  @param[out]    state               report ::oyjlPARSE_STATE_e
 *  @return                            object tree on success,
 *                                     else check error_buffer
 */
oyjl_val   oyjlTreeParse2            ( const char        * text,
                                       int                 flags,
                                       const char        * error_name,
                                       int               * status )
{
  oyjl_val root = NULL;
#define oyjl_error_buffer_size_ 256
  char oyjl_error_buffer_[oyjl_error_buffer_size_] = {0};
  int data_format = oyjlDataFormat(text);
  oyjlPARSE_STATE_e state = oyjlPARSE_STATE_NONE;
  oyjlMSG_e msg_type = flags & OYJL_QUIET ? oyjlMSG_INFO : oyjlMSG_ERROR;


  if(!(flags & OYJL_NO_MARKUP) && text && strstr(text, "\033[0") != NULL)
  {
    text = oyjlTermColorToPlain(text, flags);
    if(*oyjl_debug)
      oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "removing markup", OYJL_DBG_ARGS );
  }

  if(oyjlStringStartsWith(text, "oiJS"))
    /* static OYJL JSON */
  {
    if(flags & OYJL_ALLOW_STATIC)
    {
      root = (oyjl_val)text;
      state = oyjlPARSE_STATE_RETURN_STATIC;
    }
    else
      state = oyjlPARSE_STATE_FORMAT_ERROR;
  }
  else if(data_format == 7)
    /* JSON */
    root = oyjlTreeParse( text, oyjl_error_buffer_, oyjl_error_buffer_size_ );
  else if(data_format == 8)
    /* XML */
#if defined(OYJL_HAVE_LIBXML2) || defined(DOXYGEN)
    root = oyjlTreeParseXml( text, flags, oyjl_error_buffer_, oyjl_error_buffer_size_ );
#else
    state = oyjlPARSE_STATE_NOT_COMPILED;
#endif

  else if(data_format == 9)
    /* YAML */
#if defined(OYJL_HAVE_YAML) || defined(DOXYGEN)
    root = oyjlTreeParseYaml( text, flags, oyjl_error_buffer_, oyjl_error_buffer_size_ );
#else
    state = oyjlPARSE_STATE_NOT_COMPILED;
#endif
  else if(flags & OYJL_DELIMITER_COMMA || flags & OYJL_DELIMITER_SEMICOLON)
    root = oyjlTreeParseCsv( text, flags, oyjl_error_buffer_, oyjl_error_buffer_size_ );
  else
    state = oyjlPARSE_STATE_FORMAT_ERROR;

  if(oyjl_error_buffer_[0] != '\000')
  {
    char * e = oyjlStringCopy(oyjlTermColor(oyjlRED,_("Usage Error:")), 0);
    oyjlMessage_p( msg_type, 0, OYJL_DBG_FORMAT "%s %s\t\"%s\"\n", OYJL_DBG_ARGS,
                   e, error_name?error_name:"", oyjl_error_buffer_ );
    free(e);
    state = oyjlPARSE_STATE_PARSER_ERROR;
  } else if( ( state == oyjlPARSE_STATE_FORMAT_ERROR ||
               state == oyjlPARSE_STATE_NOT_COMPILED ) &&
             error_name)
    oyjlMessage_p( msg_type, "file parsed:\t\"%s\" %s %s\n", error_name,
                   oyjlDataFormatToString( data_format ),
                   state == oyjlPARSE_STATE_NOT_COMPILED ? "format not compiled" :  "" );

  if(status)
    *status = state;

  return root;
}
/** @} *//* oyjl_tree */
