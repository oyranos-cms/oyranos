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

#include "oyjl_version.h"
#include <yajl/yajl_parse.h>
#ifndef YAJL_VERSION
#include <yajl/yajl_version.h>
#endif

#include "oyjl.h"
#include "oyjl_version.h"
#include "oyjl_tree_internal.h"
#ifdef HAVE_LOCALE_H
#include <locale.h>
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

#if (YAJL_VERSION) > 20000
static int handle_string (void *ctx,
                          const unsigned char *string, long unsigned int string_length)
#else
static int handle_number (void *ctx, const char *string, unsigned int string_length)
#endif
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
    char * save_locale = oyjlStringCopy( setlocale(LC_NUMERIC, 0 ), malloc );
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
 *  are oyjlTreeToPaths() and oyjlPathMatch(). oyjlTreeToPaths()
 *  works on the whole tree to match a extended xpath.
 *
 *  \b Programming \b Tutorial
 *
 *  The following code examples come from @ref tutorial_json_options.c . 
 *  @dontinclude tutorial_json_options.c
 *  @skip testOyjl(void)
 *  @until oyjlTreeFree(
 *  @{ *//* oyjl */

/** @brief read a json text string into a C data structure
 *
 *  @dontinclude tutorial_json_options.c
 *  @skipline const char * text
 *  @skip error_buffer
 *  @until oyjlTreeParse
 */
oyjl_val oyjlTreeParse   (const char *input,
                          char *error_buffer, size_t error_buffer_size)
{
#if (YAJL_VERSION) > 20000
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
    if (status != yajl_status_ok)
    {
        internal_err_str = (char *) yajl_get_error(handle, 1,
                     (const unsigned char *) input,
                     strlen(input));
        if (error_buffer != NULL && error_buffer_size > 0)
             snprintf(error_buffer, error_buffer_size, "%s", internal_err_str);
#if YAJL_VERSION > 19999
        status = yajl_complete_parse (handle);
#endif
        oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT_ "%s", OYJL_DBG_ARGS_,
                       internal_err_str );
        yajl_free_error( handle, (unsigned char*)internal_err_str );
        internal_err_str = 0;
        yajl_free (handle);
        return NULL;
    }

#if YAJL_VERSION > 19999
    status = yajl_complete_parse (handle);
#endif
    yajl_free (handle);
    return (ctx.root);
}

#if defined(OYJL_HAVE_LIBXML2)
/** @brief read a XML text string into a C data structure
 */
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "oyjl.h"


char *             oyjlXML2NodeName  ( xmlNodePtr          cur )
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

/** @brief obtain a new node object possibly in array
 *
 *  The node can even have the same name. It works only for flat path level.
 *
 *  @param[in.out] root                the node
 *  @param[in]     name                flat path, without leveling slash
 *  @param[out]    array_ret           tell if the node is child of that array
 *  @param[out]    pos_ret             position of returned node in array_ret
 *                                     - -1 means object is direct child without array
 *                                     - >= 0 -> index in array
 *  @return                            new node
 */
oyjl_val oyjlTreeGetNewValueFromArray( oyjl_val root, const char * name, oyjl_val * array_ret, int * pos_ret )
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
      node = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "%s/[0]", name);
      memcpy( node, copy, size );
      free(copy); copy = NULL;
      pos = 1;
      node = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "%s/[%d]", name, pos);
    }
    else
    {
      array = oyjlTreeGetValue( root, 0, name );
      node = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "%s/[%d]", name, pos);
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

int              oyjlXMLNodeIsText   ( xmlNodePtr          cur )
{
  return cur->type == XML_TEXT_NODE && !(cur->next || cur->prev) &&
         cur->content ;
}

static int oyjl_number_detection = 1;
void             oyjlParseXMLDoc_    ( xmlDocPtr           doc,
                                       xmlNodePtr          cur,
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
      name = oyjlXML2NodeName( cur );
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
            prop = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "%s/[%d]/@%s", name, count, attr );
          else
            prop = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "%s/@%s", name, attr );
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
            prop = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "%s/[%d]/@%s", name, count, attr );
          else
            prop = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "%s/@%s", name, attr );
          oyjlValueSetString( prop, val );
          xcur = xcur->next;
        }
      }
    }
    else
    if( oyjlXMLNodeIsText(cur) )
    {
      const char * val = (const char *) cur->content;
      double d;
      int err = -1;
      if(oyjl_number_detection)
        err = oyjlStringToDouble( val, &d );
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
      }
      else
        oyjlValueSetString( root, val );
    }

    if(cur->xmlChildrenNode)
    {
      oyjl_val text = NULL;
      xmlNodePtr cur_ = cur->xmlChildrenNode;
      if( oyjlXMLNodeIsText(cur_) && node->type == oyjl_t_object )
        text = oyjlTreeGetValue( node, OYJL_CREATE_NEW, "@text" );
      oyjlParseXMLDoc_( doc, cur->xmlChildrenNode,
                        text ? text : node );
    }

    if(name)
      free( name );

    cur = cur->next;
  }
}

oyjl_val   oyjlTreeParseXml          ( const char        * xml,
                                       int                 flags,
                                       char              * error_buffer,
                                       size_t              error_buffer_size)
{
  xmlDocPtr doc = NULL;
  xmlNodePtr cur = NULL;
  oyjl_val jroot =  NULL;

  if(!xml) return jroot;

  if(flags & OYJL_NUMBER_DETECTION)
    oyjl_number_detection = 1;
  else
    oyjl_number_detection = 0;

  doc = xmlParseMemory( xml, strlen(xml) );
  cur = xmlDocGetRootElement(doc);

  if(doc && cur)
  {
    jroot = oyjlTreeNew( NULL );
    oyjlParseXMLDoc_( doc, cur, jroot );
    xmlFreeDoc( doc );
  }
  else if(error_buffer)
    snprintf( error_buffer, error_buffer_size, "XML loading failed" );

  if(error_buffer && error_buffer[0])
    oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT_ "%s",
                         OYJL_DBG_ARGS_, error_buffer );

  return jroot;
}
#endif
#if defined(OYJL_HAVE_YAML)
#include <yaml.h>
int oyjlYamlGetCount( yaml_node_t * n )
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

int oyjlYamlGetId( yaml_node_t * n, int index, int key )
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

static int oyjlYamlReadNode( yaml_document_t * doc, yaml_node_t * node, int is_key, char ** json )
{
  int error = 0;
  int count, i;
  if( !node ) return 1;
  count = oyjlYamlGetCount( node );
  if( node->type == YAML_SCALAR_NODE )
  {
    char * t = (char*)node->data.scalar.value, * tmp = NULL;
    if(t && strstr(t, ":\\ "))
      t = tmp = oyjlStringReplace( t, ":\\ ", ": ", 0, 0);
    if(t)
    {
      double d;
      int err = -1;
      if(oyjl_number_detection && is_key != 1)
        err = oyjlStringToDouble( t, &d );
      if(err == 0)
        oyjlStringAdd( json, 0,0, "%s", t );
      else
        oyjlStringAdd( json, 0,0, "\"%s\"", t );
    }
    if(tmp) free(tmp);
  }
  if( node->type == YAML_SEQUENCE_NODE )
  {
    oyjlStringAdd( json, 0,0, "[");
    for(i = 0; i < count && !error; ++i)
    {
      int id = oyjlYamlGetId( node, i, 0 );
      yaml_node_t * n =
      yaml_document_get_node( doc, id );
      error = oyjlYamlReadNode(doc, n, 0, json);
      if(i < count - 1) oyjlStringAdd( json, 0,0, ",");
    }
    oyjlStringAdd( json, 0,0, "]");
  }
  if( node->type == YAML_MAPPING_NODE )
    for(i = 0; i < count; ++i)
    {
      int key_id = oyjlYamlGetId( node, i, 1 );
      int val_id = oyjlYamlGetId( node, i, 0 );
      yaml_node_t * key =
      yaml_document_get_node( doc, key_id );
      yaml_node_t * val =
      yaml_document_get_node( doc, val_id );

      if(i == 0) oyjlStringAdd( json, 0,0, "{");

      error = oyjlYamlReadNode(doc, key, 1, json);
      if( key->type == YAML_SCALAR_NODE &&
          !error )
      {
        oyjlStringAdd( json, 0,0, ":");
      }

      error = oyjlYamlReadNode(doc, val, 0, json);
      if(i < count - 1) oyjlStringAdd( json, 0,0, ",");
      else if( i == count - 1 ) oyjlStringAdd( json, 0,0, "}");
    }
  return error;
}
/** @brief read a YAML text string into a C data structure
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
    oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT_ "%s", OYJL_DBG_ARGS_,
                   "YAML initialisation failed" );
    return jroot;
  }

  if(flags & OYJL_NUMBER_DETECTION)
    oyjl_number_detection = 1;
  else
    oyjl_number_detection = 0;

  yaml_parser_set_input_string( &parser, (const unsigned char*) yaml, strlen(yaml));

  yaml_parser_load( &parser, &document );
  if( parser.error != YAML_NO_ERROR)
  {
    if(error_buffer)
      snprintf( error_buffer, error_buffer_size, "%s\n", parser.problem ? parser.problem : "" );
    oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT_ "%s", OYJL_DBG_ARGS_,
                   parser.problem ? parser.problem : "" );

    return jroot;
  }

  root = yaml_document_get_root_node(&document);
  error = oyjlYamlReadNode( &document, root, 1, &json );
  if( error )
  {
    if(error_buffer)
      snprintf( error_buffer, error_buffer_size, "Found problem while parsing document tree.\n" );
    oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT_ "%s", OYJL_DBG_ARGS_,
                   "Found problem while parsing document tree." );
    return jroot;
  }

  jroot = oyjlTreeParse( json, error_buffer, error_buffer_size );

  yaml_parser_delete(&parser);
  yaml_document_delete(&document);

  return jroot;
}
#endif

/** @} *//* oyjl */
/** @} *//* misc */
