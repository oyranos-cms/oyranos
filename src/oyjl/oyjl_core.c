/*
 * Copyright (c) 2004-2016  Kai-Uwe Behrmann  <ku.b@gmx.de>
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "oyjl_tree_internal.h"

yajl_status  oyjl_message_func       ( oyjl_message_e      error_code,
                                       const void        * context_object __attribute__((unused)),
                                       const char        * format,
                                       ... )
{
  char * text = 0;
  int error = 0;
  va_list list;
  size_t sz = 0;
  int len = 0;
  const char * status_text = NULL;


  va_start( list, format);
  len = vsnprintf( text, sz, format, list);
  va_end  ( list );

  {
    text = calloc( sizeof(char), len+2 );
    if(!text)
    {
      fprintf(stderr,
      OYJL_DBG_FORMAT_"Could not allocate 256 byte of memory.\n",OYJL_DBG_ARGS_);
      return 1;
    }
    va_start( list, format);
    len = vsnprintf( text, len+1, format, list);
    va_end  ( list );
  }

  if(error_code == oyjl_message_info) status_text = "Info: ";
  if(error_code == oyjl_message_client_canceled) status_text = "Client Canceled: ";
  if(error_code == oyjl_message_insufficient_data) status_text = "Insufficient data: ";
  if(error_code == oyjl_message_error) status_text = "!!! ERROR: ";

  if(status_text)
    fprintf( stderr, "%s", status_text );
  if(text)
    fprintf( stderr, "%s\n", text );
  fflush( stderr );

  free( text ); text = 0;

  return error;
}

oyjl_message_f     oyjl_message_p = oyjl_message_func;

/** @brief   set a custom message listener
 *
 *  @version OpenICC: 0.1.0
 *  @date    2011/10/21
 *  @since   2008/04/03 (OpenICC: 0.1.0)
 */
yajl_status    oyjl_message_func_set ( oyjl_message_f    message_func )
{
  if(message_func)
    oyjl_message_p = message_func;
  return 0;
}

char **        oyjl_string_split     ( const char        * text,
                                       const char          delimiter,
                                       int               * count,
                                       void*            (* alloc)(size_t))
{
  char ** list = 0;
  int n = 0, i;

  /* split the path search string by a delimiter */
  if(text && text[0] && delimiter)
  {
    const char * tmp = text;

    if(!alloc) alloc = malloc;

    if(tmp[0] == delimiter) ++n;
    do { ++n;
    } while( (tmp = strchr(tmp + 1, delimiter)) );

    tmp = 0;

    if((list = alloc( (n+1) * sizeof(char*) )) == 0) return NULL;
    memset( list, 0, (n+1) * sizeof(char*) );

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

        if((list[i] = alloc( len+1 )) == 0) return NULL;

        memcpy( list[i], start, len );
        list[i][len] = 0;
        start += len + 1;
      }
    }
  }

  if(count)
    *count = n;

  return list;
}

char *     oyjl_string_copy          ( const char        * string,
                                       void*            (* alloc)(size_t))
{
  char * text_copy = NULL;

  if(string)
  {
    oyjlAllocHelper_m_( text_copy, char, strlen(string) + 1,
                        alloc, return NULL );
    strcpy( text_copy, string );
  }
    
  return text_copy;
}

int        oyjl_string_add           ( char             ** string,
                                       void*            (* alloc)(size_t size),
                                       void             (* deAlloc)(void * data ),
                                       const char        * format,
                                                           ... )
{
  char * text_copy = NULL;
  char * text = 0;
  va_list list;
  int len;
  size_t sz = 0;

  void*(* allocate)(size_t size) = alloc?alloc:malloc;
  void (* deAllocate)(void * data ) = deAlloc?deAlloc:free;

  va_start( list, format);
  len = vsnprintf( text, sz, format, list );
  va_end  ( list );

  {
    oyjlAllocHelper_m_(text, char, len + 1, allocate, return 1);
    va_start( list, format);
    len = vsnprintf( text, len+1, format, list );
    va_end  ( list );
  }

  if(string && *string)
  {
    int l = strlen(*string);
    text_copy = allocate( len + l + 1 );
    strcpy( text_copy, *string );
    strcpy( &text_copy[l], text );

    deAllocate(*string);
    *string = text_copy;

    deAllocate(text);

  } else if(string)
    *string = text;
  else if(text)
    deAllocate( text );

  return 0;
}


char **    oyjl_string_list_cat_list ( const char       ** list,
                                       int                 n_alt,
                                       const char       ** append,
                                       int                 n_app,
                                       int               * count,
                                       void*            (* alloc)(size_t) )
{
  char ** nlist = 0;

  {
    int i = 0;
    int n = 0;

    if(n_alt || n_app)
      oyjlAllocHelper_m_(nlist, char*, n_alt + n_app +1, alloc, return NULL);

    for(i = 0; i < n_alt; ++i)
    {
      if(list[i])
        nlist[n] = oyjl_string_copy( list[i], alloc );
      n++;
    }

    for(i = 0; i < n_app; ++i)
    {
      nlist[n] = oyjl_string_copy( append[i], alloc );
      n++;
    }

    if(count)
      *count = n;
  }

  return nlist;
}

void       oyjl_string_list_release  ( char            *** l,
                                       int                 size,
                                       void             (* deAlloc)(void*) )
{
  if(!deAlloc) deAlloc = free;

  if( l )
  {
    char ** list = *l;

    if(list)
    {
      size_t i;
      for(i = 0; (int)i < size; ++i)
        if((list)[i])
          deAlloc( (list)[i] );
      deAlloc( list );
      *l = NULL;
    }
  }
}

void       oyjl_string_list_add_static_string (
                                       char            *** list,
                                       int               * n,
                                       const char        * string,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) )
{
  char ** nlist = 0;
  int n_alt;

  if(!list || !n) return;

  n_alt = *n;

  oyjlAllocHelper_m_(nlist, char*, n_alt + 2, alloc, return);

  memmove( nlist, *list, sizeof(char*) * n_alt);
  nlist[n_alt] = oyjl_string_copy( string, alloc );
  nlist[n_alt+1] = NULL;

  *n = n_alt + 1;

  if(*list)
    deAlloc(*list);

  *list = nlist;
}

/** @internal
 *  @brief filter doubles out
 *
 *  @version Oyranos: 0.9.6
 *  @date    2015/08/04
 *  @since   2015/08/04 (Oyranos: 0.9.6)
 */
void       oyjl_string_list_free_doubles (
                                       char             ** list,
                                       int               * list_n,
                                       void             (* deAlloc)(void*) )
{
  int n, i, pos;

  if(!list) return;

  n = *list_n;
  pos = n ? 1 : 0;

  if(!deAlloc) deAlloc = free;

  for(i = pos; i < n; ++i)
  {
    int k, found = 0;
    char * ti = list[i];
    for( k = 0; k < i; ++k )
    {
      char * tk = list[k];
      if(ti && tk && strcmp(ti, tk) == 0)
      {
        deAlloc( ti );
        list[i] = ti = NULL;
        found = 1;
        continue;
      }
    }

    if(found == 0)
    {
      list[pos] = ti;
      ++pos;
    }
  }

  list[pos] = NULL;

  *list_n = pos;
}
void     oyjl_string_list_add_list   ( char            *** list,
                                       int               * n,
                                       const char       ** append,
                                       int                 n_app,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) )
{
  int alt_n = 0;
  char ** tmp;

  if(!list) return;

  if(n) alt_n = *n;
  tmp = oyjl_string_list_cat_list((const char**)*list, alt_n, append, n_app,
                                     n, alloc);

  oyjl_string_list_release(list, alt_n, deAlloc);

  *list = tmp;
}


/* show better const behaviour and return instant error status */
int      oyjl_string_to_long         ( const char        * text,
                                       long              * value )
{
  char * end = 0;
  *value = strtol( text, &end, 0 );
  if(end && end != text && end[0] == '\000' )
    return 0;
  else
    return 1;
}

