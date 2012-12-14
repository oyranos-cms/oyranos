/** @file yajl_tree_parser.c
 *
 *  @par Copyright:
 *            2010-2012 (C) Kai-Uwe Behrmann
 *
 *  @brief    tree parser extension to yajl
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2010/09/15
 *
 *  This file contains a JSON to in memory object converter.
 *  The data structures can be easily accessed.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>  /* size_t ptrdiff_t */
#include <unistd.h>
#include <stdarg.h>

#include <yajl/yajl_parse.h>
#ifndef YAJL_VERSION
#include <yajl/yajl_version.h>
#endif
#include "oyjl_tree.h"

#if (YAJL_VERSION) > 20000
yajl_callbacks oyjl_tree_callbacks = {
  oyjl_tree_parse_null,
  oyjl_tree_parse_boolean,
  (int(*)(void*,long long))oyjl_tree_parse_integer,
  oyjl_tree_parse_double,
  NULL,
  (int(*)(void*,const unsigned char*,size_t))oyjl_tree_parse_string,
  oyjl_tree_parse_start_map,
  (int(*)(void*,const unsigned char*,size_t))oyjl_tree_parse_map_key,
  oyjl_tree_parse_end_map,
  oyjl_tree_parse_start_array,
  oyjl_tree_parse_end_array
};
#else
yajl_callbacks oyjl_tree_callbacks = {
  oyjl_tree_parse_null,
  oyjl_tree_parse_boolean,
  oyjl_tree_parse_integer,
  oyjl_tree_parse_double,
  NULL,
  oyjl_tree_parse_string,
  oyjl_tree_parse_start_map,
  oyjl_tree_parse_map_key,
  oyjl_tree_parse_end_map,
  oyjl_tree_parse_start_array,
  oyjl_tree_parse_end_array
};
#endif

#ifndef MIN
#define MIN(a,b) (a)<(b) ? (a) : (b)
#endif



/* ---------------------- Implementation ---------------------- */

/*  yail 1.0.11 misses a object orianted API for C structures.
 *  callbacks for each individual type. Hey, C has unions, structs and arrays.
 *  The oyjl callbacks results in fat parsing code. This file provides a 
 *  <marketing> easy to use, light wigth, stable </marketing>
 *  API to access JSON data structures in a object oriented fashion. 
 */

/** @func    oyjl_message_func
 *  @brief   default oyjl_tree message function
 *
 *  @version oyjl-tree: 1.0.11-1
 *  @since   2008/04/03 (oyjl-tree: 1.0.11-1)
 *  @date    2010/09/18
 */
yajl_status    oyjl_message_func     ( oyjl_message_e      code,
                                       const void        * context,
                                       const char        * format,
                                       ... )
{
  char * text = 0;
  va_list list;
  int i,len;
  size_t sz = 256;

  if(code == oyjl_message_info)
    return yajl_status_ok;

  text = calloc( sizeof(char), sz );
  if(!text)
  {
    fprintf(stderr,
    "oyjl_tree_parse.c:79 oyjl_message_func() Could not allocate 256 byte of memory.\n");
    return yajl_status_error;
  }

  text[0] = 0;
  va_start( list, format);
  len = vsnprintf( text, sz-1, format, list);
  va_end  ( list );

  if (len >= (sz - 1))
  {
    text = realloc( text, (len+1)*sizeof(char) );
    va_start( list, format);
    len = vsnprintf( text, len+1, format, list);
    va_end  ( list );
  }

  switch(code)
  {
    case oyjl_message_info:
    case oyjl_message_client_canceled:
         fprintf( stderr, "WARNING");
         break;
    case oyjl_message_insufficient_data:
    case oyjl_message_error:
         fprintf( stderr, "!!! ERROR");
         break;
  }

  i = 0;
  while(text[i])
    fputc(text[i++], stderr);
  fprintf( stderr, "\n" );

  free( text );

  return yajl_status_ok;
}
oyjl_message_f oyjl_message = oyjl_message_func;
yajl_status    oyjl_message_func_set ( oyjl_message_f      message_func )
{
  oyjl_message = message_func;
  return yajl_status_ok;
}

void * oyjl_malloc( size_t size )
{ return malloc(size); }
void   oyjl_free_memory( void ** ptr )
{ free(*ptr); *ptr = 0; }

oyjl_value_s  *  oyjl_value_new      ( )
{
  oyjl_value_s  * s = oyjl_malloc(sizeof(oyjl_value_s ));
  memset( s, 0, sizeof(oyjl_value_s ) );
  return s;
}

oyjl_object_s *  oyjl_object_create  ( const unsigned char * key,
                                       const unsigned int    len )
{
  oyjl_object_s  * s = oyjl_malloc(sizeof(oyjl_object_s ));
  memset( s, 0, sizeof(oyjl_object_s ) );

  s->key.text = oyjl_string_dup( key, len );
  s->key.len = len;

  return s;
}

oyjl_value_s  ** oyjl_array_new      ( int                 count )
{
  int size = (count + 1) * sizeof(oyjl_value_s*);
  oyjl_value_s  ** s = oyjl_malloc( size );
  memset( s, 0, size );
  return s;
}

yajl_status  oyjl_value_array_move_in( oyjl_value_s    *** array,
                                       oyjl_value_s     ** value )
{
  int count = 0;
  oyjl_value_s  ** new_array = 0;

  if(!value || !*value)
    return yajl_status_error;

  if(*array)
    while((*array)[count] && (*array)[count]->type != oyjl_type_none) ++count;

  new_array = oyjl_array_new( count + 2 );
  if(!new_array) return yajl_status_error;

  memset( new_array, 0, (count + 2) * sizeof(oyjl_value_s *) );

  if(*array)
  {
    memcpy( new_array, *array, count * sizeof(oyjl_value_s *) );
    oyjl_free_memory( (void**)array );
  }

  new_array[count] = *value;
  *value = 0;
  *array = new_array;

  return yajl_status_ok;
}


oyjl_tree_parse_context_s * oyjl_tree_parse_context_new( void )
{
  int error = 0;
  oyjl_tree_parse_context_s * s;
  s = oyjl_malloc( sizeof(oyjl_tree_parse_context_s) );
  error = !s;
  if(!error)
  {
    memset(s, 0, sizeof(oyjl_tree_parse_context_s));
    memcpy(s->type,"yopc",4);
  }
  return s;
}
int  oyjl_tree_parse_context_free  ( oyjl_tree_parse_context_s ** context )
{
  int error = !context || !*context;
  if(!error)
  {
    oyjl_free_memory( (void**)&(*context)->stack );
    oyjl_free_memory( (void**)context );
  }

  return error;
}

/* the parser callbacks */
int oyjl_tree_parse_null(void * context)
{
  oyjl_tree_parse_context_s * s = (oyjl_tree_parse_context_s*) context;
  oyjl_value_s * c = oyjl_tree_parse_get_current(s);
  yajl_status error = yajl_status_error;
  oyjl_value_s * new_value = oyjl_value_new();

  if(new_value)
  {
    new_value->type = oyjl_type_boolean;
    new_value->value.boolean = 0;

    if(c->type == oyjl_type_object)
    {
      oyjl_message( oyjl_message_info, NULL, 
               "%s[null]=%d\n", oyjl_print_text(&c->value.object->key), 0 );
      c->value.object->value = new_value;
      error = yajl_status_ok;
    }
    else if(c->type == oyjl_type_array)
    {
      oyjl_message( oyjl_message_info, NULL, 
               "[null]=%d\n", 0 );
      oyjl_value_array_move_in( &c->value.array, &new_value );
      error = yajl_status_ok;
    }
  }

  return !error;
}

int oyjl_tree_parse_boolean(void * context, int i)
{
  oyjl_tree_parse_context_s * s = (oyjl_tree_parse_context_s*) context;
  oyjl_value_s * c = oyjl_tree_parse_get_current(s);
  yajl_status error = yajl_status_error;
  oyjl_value_s * new_value = oyjl_value_new();

  if(new_value)
  {
    new_value->type = oyjl_type_boolean;
    new_value->value.boolean = i;

    if(c->type == oyjl_type_object)
    {
      oyjl_message( oyjl_message_info, NULL, 
               "%s[boolean]=%d\n", oyjl_print_text(&c->value.object->key), i);
      c->value.object->value = new_value;
      error = yajl_status_ok;
    }
    else if(c->type == oyjl_type_array)
    {
      oyjl_message( oyjl_message_info, NULL, 
               "[boolean]=%d\n", i );
      oyjl_value_array_move_in( &c->value.array, &new_value );
      error = yajl_status_ok;
    }
  }

  return !error;
}

int oyjl_tree_parse_integer(void * context, long i)
{
  oyjl_tree_parse_context_s * s = (oyjl_tree_parse_context_s*) context;
  oyjl_value_s * c = oyjl_tree_parse_get_current(s);
  yajl_status error = yajl_status_error;
  oyjl_value_s * new_value = oyjl_value_new();

  if(new_value)
  {
    new_value->type = oyjl_type_integer;
    new_value->value.integer = i;

    if(c->type == oyjl_type_object)
    {
      oyjl_message( oyjl_message_info, NULL, 
               "%s[integer]=%ld\n", oyjl_print_text(&c->value.object->key), i);

      c->value.object->value = new_value;
      error = yajl_status_ok;
    }
    else if(c->type == oyjl_type_array)
    {
      oyjl_message( oyjl_message_info, NULL, 
               "[integer]=%ld\n", i );
      oyjl_value_array_move_in( &c->value.array, &new_value );
      error = yajl_status_ok;
    }
  }

  return !error;
}

int oyjl_tree_parse_double(void * context, double g)
{
  oyjl_tree_parse_context_s * s = (oyjl_tree_parse_context_s*) context;
  oyjl_value_s * c = oyjl_tree_parse_get_current(s);
  yajl_status error = yajl_status_error;
  oyjl_value_s * new_value = oyjl_value_new();

  if(new_value)
  {
    new_value->type = oyjl_type_double;
    new_value->value.floating = g;

    if(c->type == oyjl_type_object)
    {
      oyjl_message( oyjl_message_info, NULL, 
               "%s[float]=%g\n", oyjl_print_text(&c->value.object->key), g);
      c->value.object->value = new_value;
      error = yajl_status_ok;
    }
    else if(c->type == oyjl_type_array)
    {
      oyjl_message( oyjl_message_info, NULL, 
               "[float]=%g\n", g );
      oyjl_value_array_move_in( &c->value.array, &new_value );
      error = yajl_status_ok;
    }
  }

  return !error;
}

unsigned char * oyjl_string_dup      ( const unsigned char * text,
                                       unsigned int        len )
{
  unsigned char * t = 0;
  t = oyjl_malloc( len + 1 );
  if(t)
  {
    memcpy( t, text, len );
    t[len] = '\000';
  }
  return t;
}

int oyjl_tree_parse_string ( void * context, const unsigned char * text,
                             unsigned int len)
{
  oyjl_tree_parse_context_s * s = (oyjl_tree_parse_context_s*) context;
  oyjl_value_s * c = oyjl_tree_parse_get_current(s),
               * new_value = oyjl_value_new();
  yajl_status error = yajl_status_error;

  if(new_value)
  {
    new_value->type = oyjl_type_text;
    new_value->value.text.text = oyjl_string_dup( text, len );
    new_value->value.text.len = len;

    if(c->type == oyjl_type_object)
    {
      char * tmp = strdup( oyjl_print_text(&c->value.object->key) );
      c->value.object->value = new_value;
      error = yajl_status_ok;

      oyjl_message( oyjl_message_info, NULL, 
               "%s[string]=%s\n", tmp,
                          oyjl_print_text(&c->value.object->value->value.text));
      free( tmp );
    } else if(c->type == oyjl_type_array)
    {
      oyjl_message( oyjl_message_info, NULL, 
               "0x%x [string]=%s 0x%x 0x%x %u\n", new_value,
                    oyjl_print_text(&new_value->value.text),
                    new_value->value.text.text, text, len );
      oyjl_value_array_move_in( &c->value.array, &new_value );
      error = yajl_status_ok;
    }
  }

  return !error;
}

int oyjl_tree_parse_start_map(void * context)
{
  oyjl_tree_parse_context_s * s = (oyjl_tree_parse_context_s*) context;
  oyjl_value_s * c = 0;
  oyjl_value_s * value = 0, * new_value = 0;
  int count = 0;
  yajl_status error = yajl_status_error;

  /* initialise the stack */
  if(!s->stack)
    s->stack = oyjl_array_new( 0 );
  else
    c = oyjl_tree_parse_get_current(s);

  while(s->stack && s->stack[count]) ++count;

  /* Assign the new value as usual to the tree. */
  new_value = oyjl_value_new();
  if(new_value)
  {
    new_value->value.object = oyjl_object_create( NULL, 0 ); 
    new_value->type = oyjl_type_object;
    if(new_value->value.object)
    {
      new_value->value.object->value = oyjl_value_new();
      if(new_value->value.object->value)
      {
        new_value->value.object->value->type = oyjl_type_array;
        new_value->value.object->value->value.array = oyjl_array_new( 0 );
      }
    }

    if(!(new_value->value.object &&
         new_value->value.object->value &&
         new_value->value.object->value->value.array))
      return yajl_status_error;

    if(c)
    {
      if(c->type == oyjl_type_object)
        c->value.object->value = new_value;
      else if(c->type == oyjl_type_array)
      {
        value = new_value;
        oyjl_value_array_move_in( &c->value.array, &value );
      }
    }
  }

  /* add a new level to the stack */
  error = oyjl_value_array_move_in( &s->stack, &new_value );

  oyjl_message( oyjl_message_info, NULL, 
           "start object container%s\n", error ? " with error":"");
  return !error;
}

int oyjl_tree_parse_map_key(void * context, const unsigned char * text,
                             unsigned int len)
{
  oyjl_tree_parse_context_s * s = (oyjl_tree_parse_context_s*) context;
  oyjl_value_s *** array = 0;
  oyjl_value_s * value = 0, * new_value = 0;
  int count = 0;
  yajl_status error = yajl_status_error;
  while(s->stack && s->stack[count]) ++count;
  value = s->stack[count-1];

  /* get the current array */
  if(value->type == oyjl_type_array)
    array = &value->value.array;
  else if(value->type == oyjl_type_object)
    array = &value->value.object->value->value.array;

  new_value = oyjl_value_new();
  if(new_value)
  {
    new_value->value.object = oyjl_object_create( text, len ); 
    new_value->type = oyjl_type_object;

    if(new_value->type == oyjl_type_object &&
       new_value->value.object->key.text)
      oyjl_message( oyjl_message_info, NULL, 
             "new object:%s\n", oyjl_print_text(&new_value->value.object->key));
  }

  error = oyjl_value_array_move_in( array, &new_value );
 
  return !error;
}

int oyjl_tree_parse_end_map(void * context)
{
  oyjl_tree_parse_context_s * s = (oyjl_tree_parse_context_s*) context;
  int count = 0;
  yajl_status error = yajl_status_error;

  while(s->stack && s->stack[count]) ++count;
  /* forget the last stack level,
   * but keep the root value to return at the end. */
  if(count > 1)
    s->stack[count-1] = 0;
  error = yajl_status_ok;

  oyjl_message( oyjl_message_info, NULL, 
           "end container object\n");
  return !error;
}

int oyjl_tree_parse_start_array(void * context)
{
  oyjl_tree_parse_context_s * s = (oyjl_tree_parse_context_s*) context;
  oyjl_value_s * c = 0;
  oyjl_value_s * value = 0, * new_value = 0;
  int count = 0;
  yajl_status error = yajl_status_error;

  /* initialise the stack */
  if(!s->stack)
    s->stack = oyjl_array_new( 0 );
  else
    c = oyjl_tree_parse_get_current(s);

  while(s->stack && s->stack[count]) ++count;

  /* Assign the new value as usual to the tree. */
  new_value = oyjl_value_new();
  if(new_value)
  {
    new_value->value.array = oyjl_array_new( 0 ); 
    new_value->type = oyjl_type_array;

    if(c)
    {
      if(c->type == oyjl_type_object)
        c->value.object->value = new_value;
      else if(c->type == oyjl_type_array)
      {
        value = new_value;
        oyjl_value_array_move_in( &c->value.array, &value );
      }
    }

    oyjl_message( oyjl_message_info, NULL, 
             "new array container\n");
  }

  /* add a new level to the stack */
  error = oyjl_value_array_move_in( &s->stack, &new_value );

  return !error;
}

int oyjl_tree_parse_end_array(void * context)
{
  oyjl_tree_parse_context_s * s = (oyjl_tree_parse_context_s*) context;
  int count = 0;
  yajl_status error = yajl_status_error;

  while(s->stack && s->stack[count]) ++count;
  /* forget the last stack level,
   * but keep the root value to return at the end. */
  if(count > 1)
    s->stack[count-1] = 0;
  error = yajl_status_ok;

  oyjl_message( oyjl_message_info, NULL, 
           "end container array\n");
  return !error;
}

const char * oyjl_print_text         ( oyjl_text_s       * text )
{
  static int len = 0;
  static char * t = NULL;

  if(!text || len < text->len)
  {
    if(t)
      oyjl_free_memory( (void**)&t );
    if(text)
      len = text->len;
    else
      len = 24;
    t = oyjl_malloc(len+1);
  }
  if(text)
  {
    memcpy( t, text->text, text->len );
    t[text->len] = '\000';
  } else
    memset( t, 0, 24 );

  return t;
}

oyjl_value_s * oyjl_tree_parse_get_current ( oyjl_tree_parse_context_s * s )
{
  oyjl_value_s * value = 0,
               ** array = 0;
  int count = 0;
  while(s->stack && s->stack[count]) ++count;
  value = s->stack[count-1];

  if(value->type == oyjl_type_array)
    return value;
  else
  if(value->type == oyjl_type_object)
    array = value->value.object->value->value.array;
  else
    oyjl_message( oyjl_message_info, NULL, 
             "%s:%d no structured element in stack\n", __FILE__,__LINE__);

  count = 0;
  while( array && array[count] ) ++count;

  value = array[count-1];

  return value;
}


oyjl_text_s *  oyjl_string_create    ( const unsigned char * text,
                                       unsigned int        len )
{
  int error = !text || !len;
  oyjl_text_s * t = 0;

  if(!error)
  {
    t = oyjl_malloc( sizeof(oyjl_text_s) );
    memset( t, 0, sizeof(oyjl_text_s) );
  }

  if(text)
  {
    t->text = oyjl_string_dup( text, len );
    t->len = len;
  }

  return t;
}

yajl_status    oyjl_string_free      ( oyjl_text_s      ** text )
{
  if(text && *text)
  {
    (*text)->len = 0;
    if((*text)->text) oyjl_free_memory((void**)&(*text)->text);
    (*text)->text = NULL;
    oyjl_free_memory( (void**)text );
  }
  return yajl_status_ok;
}


/** @func    oyjl_tree_from_json
 *  @brief   convert text into a C data structure
 *
 *  @param[in]     text                the JSON text
 *  @param[out]    value               the resulting object tree
 *  @return                            0 - success
 *
 *  @version oyjl: 1.0.11
 *  @since   2010/09/15 (oyjl: 1.0.11)
 *  @date    2010/09/16
 */
yajl_status  oyjl_tree_from_json     ( const char        * text,
                                       oyjl_value_s     ** value,
                                       void              * user_data )
{
  int error = !text || !value;
  size_t len = 0;
  oyjl_tree_parse_context_s * context = oyjl_tree_parse_context_new();
  yajl_status ystatus;
#if YAJL_VERSION < 20000
  yajl_parser_config yconfig = { 1, 1 };
#endif
  yajl_handle yhandle = 0;

  if(!error && context)
  {
    len = strlen(text);
    /* setup the oyjl library */
    yhandle = yajl_alloc( &oyjl_tree_callbacks,
#if YAJL_VERSION < 20000
                                                &yconfig,
#endif
                                                          NULL, (void*)context);

    error = !yhandle;
  }

  if(!error)
  {
    ystatus = yajl_parse( yhandle, (unsigned char*) text, len );

    if(ystatus != yajl_status_ok)
    {
      unsigned char * txt = yajl_get_error( yhandle, 1, (unsigned char*)text,
                                            len );
      oyjl_message( (oyjl_message_e)ystatus, user_data, "%s", (char*)txt );

      yajl_free_error( yhandle, txt ); txt = 0;
      error = yajl_status_error;
    }
  }

  if(context && context->stack)
  {
    *value = context->stack[0];
    (*value)->priv_ = yhandle;
  } else
    yajl_free( yhandle );
  oyjl_tree_parse_context_free( &context );

  return error;
}

yajl_status    oyjl_tree_free        ( oyjl_value_s     ** object )
{
  yajl_status error = yajl_status_ok;
  oyjl_value_s * v = *object;

  if(v)
  {
    if(v->priv_)
      yajl_free( v->priv_ );

    switch(v->type)
    {
    case oyjl_type_none:
    case oyjl_type_boolean:
    case oyjl_type_integer:
    case oyjl_type_double:
         break;
    case oyjl_type_text:
         if(v->value.text.text) oyjl_free_memory((void**)&v->value.text.text);
         oyjl_free_memory( (void**)object );
         break;
    case oyjl_type_array:
         {
           int count = 0, i;
           while(v->value.array[count]) ++count;
           for(i = 0; i < count; ++i)
             oyjl_tree_free( &v->value.array[i] );
         }
         oyjl_free_memory( (void**)&v->value );
         oyjl_free_memory( (void**)object );
         break;
    case oyjl_type_object:
         oyjl_tree_free( &v->value.object->value );
         oyjl_free_memory( (void**)&v->value.object );
         oyjl_free_memory( (void**)object );
         break;
    default:
         fprintf( stderr, "unknown type: %d\n", v->type );
         break;
    }
  }

  return error;
}

yajl_status    oyjl_tree_print( oyjl_value_s * v, int * level, FILE * fp )
{
  int n = *level;
  while(n--) fprintf(fp, " ");

  if(v)
  switch(v->type)
  {
    case oyjl_type_none:
         fprintf(fp, "oyjl_type_none\n");
    case oyjl_type_boolean:
         fprintf(fp, "oyjl_type_boolean: %d\n", v->value.boolean); break;
    case oyjl_type_integer:
         fprintf(fp, "oyjl_type_integer: %lu\n", v->value.integer); break;
    case oyjl_type_double:
         fprintf(fp, "oyjl_type_double: %g\n", v->value.floating); break;
    case oyjl_type_text:
         fprintf(fp, "oyjl_type_text: %s 0x%lx %u\n", oyjl_print_text(&v->value.text), (intptr_t)v->value.text.text, v->value.text.len); break;
    case oyjl_type_array:
         {
           int count = 0, i;
           while(v->value.array[count]) ++count;
           fprintf( fp, "oyjl_type_array elements: %d\n", count );
           ++ *level;
           for(i = 0; i < count; ++i)
             oyjl_tree_print( v->value.array[i], level, fp );
           -- *level;
           n = *level; while(n--) fprintf(fp, " ");
           fprintf( fp, "oyjl_type_array: end\n");
         } break;
    case oyjl_type_object:
         fprintf( fp, "oyjl_type_object: %s\n",
                 oyjl_print_text(&v->value.object->key) );
           ++ *level;
         oyjl_tree_print( v->value.object->value, level, fp );
           -- *level;
         n = *level; while(n--) fprintf(fp, " ");
         fprintf( fp, "oyjl_type_object: end\n");
         break;
    default:
         fprintf( fp, "unknown type: %d\n", v->type );
         break;
  }
  return yajl_status_ok;
}

char *   oyjl_value_text             ( oyjl_value_s      * v,
                                       void              * (*my_alloc)(size_t len) )
{
  char * text = NULL;
  const char * tmp;

  if(!my_alloc)
    my_alloc = malloc;

  if(v &&
     v->type == oyjl_type_object &&
     v->value.object->value)
  switch(v->value.object->value->type)
  {
    case oyjl_type_none:
         break;
    case oyjl_type_boolean:
         text = my_alloc(12);
         if(text) sprintf( text, "%d", v->value.object->value->value.boolean); break;
    case oyjl_type_integer:
         text = my_alloc(80);
         if(text) sprintf( text, "%li", v->value.object->value->value.integer); break;
    case oyjl_type_double:
         text = my_alloc(80);
         if(text) sprintf( text, "%g", v->value.object->value->value.floating); break;
    case oyjl_type_text:
         tmp = oyjl_print_text(&v->value.object->value->value.text);
         if(tmp) text = my_alloc(strlen(tmp)+1);
         if(text) sprintf( text, "%s", tmp); break;
    default:
         break;
  } else
    if(v &&
       v->type == oyjl_type_text &&
       v->value.text.len)
    {
         tmp = (char*)v->value.text.text;
         if(tmp) text = my_alloc(v->value.text.len+1);
         if(text)
         {
           memcpy( text, tmp, v->value.text.len);
           text[v->value.text.len] = 0;
         }
    }

  return text;
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

    if((list = oyjl_malloc( (n+1) * sizeof(char*) )) == 0) return NULL;

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

        if((list[i] = oyjl_malloc( len+1 )) == 0) return NULL;

        memcpy( list[i], start, len );
        list[i][len] = 0;
        start += len + 1;
      }
    }
  }

  *count = n;

  return list;
}

oyjl_value_s * oyjl_tree_get_value   ( oyjl_value_s      * root,
                                       const char        * xpath )
{
  oyjl_value_s * tmp = 0, * level = 0;
  int n = 0, i, found = 0;
  char ** list = oyjl_string_split(xpath, &n),
        * ttmp = 0;

  /* follow the search path term */
  level = root;
  found = n;
  for(i = 0; i < n; ++i)
  {
    char * term = list[i],
         * tindex = strrchr(term,'[');
    int count = oyjl_value_count( level );
    int len = 0,j;
    int pos = -1;

    
    if(tindex != NULL)
    {
      ptrdiff_t size;
      ++tindex;
      size = strrchr(term,']') - tindex;
      if(size > 0)
      {
        ttmp = oyjl_malloc(size + 1);
        memcpy( ttmp, tindex, size );
        ttmp[size] = '\000';
        pos = atoi(ttmp);
        size = strrchr(term,'[') - term;
        memcpy( ttmp, term, size );
        ttmp[size] = '\000';
        term = ttmp;
      }
    }

    if(term)
      len = strlen(term);

    if(found == 0) break;
    found = 0;

    if(!(term && term[0]) && pos != -1)
    {
      level = oyjl_value_pos_get( level, pos );
      found = 1;
    } else
    for(j = 0; j < count; ++j)
    {
      tmp = oyjl_value_pos_get( level, j );
      if(tmp->type == oyjl_type_object &&
         tmp->value.object && tmp->value.object->key.text)
      {
        if(term &&
           len == tmp->value.object->key.len &&
           memcmp( tmp->value.object->key.text, term, len) == 0)
        {
          ++found;
          if(pos == -1 ||
             (found-1) == pos)
          {
            level = tmp->value.object->value;
            break;
          }
        }
      } 
    }
  }

  /* clean up temorary memory */
  for(i = 0; i < n; ++i)
    oyjl_free_memory((void**)&list[i]);
  if(list)
    oyjl_free_memory((void**)&list);

  if(ttmp)
    oyjl_free_memory( (void**)&ttmp );

  if(found && level)
    return level;
  else
    return NULL;
}

/** Function oyjl_tree_get_valuef
 *  @brief   get a child node
 *
 *  @param[in]     root                the oyjl node
 *  @param[in]     format              the xpath format
 *  @param[in]     ...                 the variable argument list
 *  @return                            the childs text value
 *
 *  @version Oyranos: 0.3.3
 *  @since   2011/09/24 (Oyranos: 0.3.3)
 *  @date    2011/09/24
 */
oyjl_value_s * oyjl_tree_get_valuef  ( oyjl_value_s      * root,
                                       const char        * format,
                                                           ... )
{
  oyjl_value_s * value = 0;

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

  value = oyjl_tree_get_value( root, text );

  if(text) free(text);

  return value;
}


int            oyjl_value_count      ( oyjl_value_s      * value )
{
  int count = 0;

  if(!value)
    return count;

  /* encapsulated container */
  if(value->type == oyjl_type_object &&
     value->value.object->value &&
     value->value.object->value->type == oyjl_type_object &&
     value->value.object->value->value.object &&
     value->value.object->value->value.object->value &&
     value->value.object->value->value.object->value->type == oyjl_type_array &&
     value->value.object->value->value.object->value->value.array)
    while( value->value.object->value->value.object->value->value.array[count] )
      ++count;
  else
  /* container */
  if(value->type == oyjl_type_object &&
     value->value.object->value &&
     value->value.object->value->type == oyjl_type_array &&
     value->value.object->value->value.array)
    while( value->value.object->value->value.array[count] ) ++count;
  else
  /* container */
  if(value->type == oyjl_type_array &&
     value->value.array)
    while( value->value.array[count] ) ++count;
  else
  if(value->type != oyjl_type_none)
    ++count;

  return count;
}

oyjl_value_s * oyjl_value_pos_get    ( oyjl_value_s      * value,
                                       int                 pos )
{
  if(value->type == oyjl_type_object &&
     value->value.object->value &&
     value->value.object->value->type == oyjl_type_object &&
     value->value.object->value->value.object &&
     value->value.object->value->value.object->value &&
     value->value.object->value->value.object->value->type == oyjl_type_array &&
     value->value.object->value->value.object->value->value.array)
    return value->value.object->value->value.object->value->value.array[pos];
  else
  if(value->type == oyjl_type_object &&
     value->value.object->value &&
     value->value.object->value->type == oyjl_type_array &&
     value->value.object->value->value.array)
    return value->value.object->value->value.array[pos];
  else
  if(value->type == oyjl_type_array &&
     value->value.array)
    return value->value.array[pos];
  else
  if(value->type != oyjl_type_none)
    return value;

  return NULL;
  
}


