/*  @file oyjl_core.c
 *
 *  oyjl - string, file i/o and other basic helpers
 *
 *  @par Copyright:
 *            2016-2019 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl core functions
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *
 * Copyright (c) 2004-2018  Kai-Uwe Behrmann  <ku.b@gmx.de>
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

#include <math.h>    /* NAN */
#include <stdarg.h>  /* va_list */
#include <stddef.h>  /* ptrdiff_t size_t */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "oyjl.h"
#include "oyjl_version.h"
#include "oyjl_tree_internal.h"
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

/** \addtogroup misc
 *  @{ *//* misc */
/** \addtogroup oyjl
 *  @{ *//* oyjl */

int          oyjlMessageFunc         ( int/*oyjlMSG_e*/    error_code,
                                       const void        * context_object OYJL_UNUSED,
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

  if(error_code == oyjlMSG_INFO) status_text = "Info: ";
  if(error_code == oyjlMSG_CLIENT_CANCELED) status_text = "Client Canceled: ";
  if(error_code == oyjlMSG_INSUFFICIENT_DATA) status_text = "Insufficient data: ";
  if(error_code == oyjlMSG_ERROR) status_text = "!!! ERROR: ";

  if(status_text)
    fprintf( stderr, "%s", status_text );
  if(text)
    fprintf( stderr, "%s\n", text );
  fflush( stderr );

  free( text ); text = 0;

  return error;
}

oyjlMessage_f     oyjlMessage_p = oyjlMessageFunc;

/** @brief   set a custom message listener
 *
 *  @version OpenICC: 0.1.0
 *  @date    2011/10/21
 *  @since   2008/04/03 (OpenICC: 0.1.0)
 */
int            oyjlMessageFuncSet    ( oyjlMessage_f       message_func )
{
  if(message_func)
    oyjlMessage_p = message_func;
  return 0;
}

char **        oyjlStringSplit       ( const char        * text,
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

char *     oyjlStringCopy            ( const char        * string,
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

int        oyjlStringAdd             ( char             ** string,
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

char*      oyjlStringAppendN         ( const char        * text,
                                       const char        * append,
                                       int                 append_len,
                                       void*            (* alloc)(size_t size) )
{
  char * text_copy = NULL;
  int text_len = 0;

  if(text)
    text_len = strlen(text);

  if(text_len || append_len)
  {
    oyjlAllocHelper_m_( text_copy, char,
                        text_len + append_len + 1,
                        alloc, return NULL );

    if(text_len)
      memcpy( text_copy, text, text_len );

    if(append_len)
      memcpy( &text_copy[text_len], append, append_len );

    text_copy[text_len+append_len] = '\000';
  }

  return text_copy;
}

void       oyjlStringAddN            ( char             ** text,
                                       const char        * append,
                                       int                 append_len,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) )
{
  char * text_copy = NULL;

  if(!text) return;

  text_copy = oyjlStringAppendN(*text, append, append_len, alloc);

  if(*text && deAlloc)
    deAlloc(*text);

  *text = text_copy;

  return;
}

char*      oyjlStringReplace         ( const char        * text,
                                       const char        * search,
                                       const char        * replacement,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) )
{
  char * t = 0;
  const char * start = text,
             * end = text;

  void*(* allocate)(size_t size) = alloc?alloc:malloc;
  void (* deAllocate)(void * data ) = deAlloc?deAlloc:free;

  if(text && search && replacement)
  {
    int s_len = strlen(search);
    while((end = strstr(start,search)) != 0)
    {
      oyjlStringAddN( &t, start, end-start, allocate, deAllocate );
      oyjlStringAddN( &t, replacement, strlen(replacement), allocate, deAllocate );
      if(strlen(end) >= (size_t)s_len)
        start = end + s_len;
      else
      {
        if(strstr(start,search) != 0)
          oyjlStringAddN( &t, replacement, strlen(replacement), allocate, deAllocate );
        start = end = end + s_len;
        break;
      }
    }
  }

  if(start && strlen(start))
    oyjlStringAddN( &t, start, strlen(start), allocate, deAllocate );

  return t;
}


char **    oyjlStringListCatList     ( const char       ** list,
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
        nlist[n] = oyjlStringCopy( list[i], alloc );
      n++;
    }

    for(i = 0; i < n_app; ++i)
    {
      nlist[n] = oyjlStringCopy( append[i], alloc );
      n++;
    }

    if(count)
      *count = n;
  }

  return nlist;
}

void       oyjlStringListRelease  ( char            *** l,
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

void       oyjlStringListAddStaticString (
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
  nlist[n_alt] = oyjlStringCopy( string, alloc );
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
void       oyjlStringListFreeDoubles (
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
void     oyjlStringListAddList       ( char            *** list,
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
  tmp = oyjlStringListCatList((const char**)*list, alt_n, append, n_app,
                                     n, alloc);

  oyjlStringListRelease(list, alt_n, deAlloc);

  *list = tmp;
}


/* show better const behaviour and return instant error status */
int      oyjlStringToLong            ( const char        * text,
                                       long              * value )
{
  char * end = 0;
  *value = strtol( text, &end, 0 );
  if(end && end != text && end[0] == '\000' )
    return 0;
  else
    return 1;
}

/** @internal 
 *  @brief   text to double conversion
 *
 *  @return                            error
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/03/18
 *  @since   2011/11/17 (Oyranos: 0.2.0)
 */
int          oyjlStringToDouble      ( const char        * text,
                                       double            * value )
{
  char * p = NULL, * t = NULL;
  int len;
  int error = -1;
#ifdef HAVE_LOCALE_H
  char * save_locale = oyjlStringCopy( setlocale(LC_NUMERIC, 0 ), malloc );
  setlocale(LC_NUMERIC, "C");
#endif

  if(text && text[0])
    len = strlen(text);
  else
  {
    *value = NAN;
    error = 1;
    return error;
  }

  /* avoid irritating valgrind output of "Invalid read of size 8"
   * might be a glibc error or a false positive in valgrind */
  oyjlAllocHelper_m_( t, char, len + 2*sizeof(double) + 1, malloc, return 1);
  memset( t, 0, len + 2*sizeof(double) + 1 );

  memcpy( t, text, len );

  *value = strtod( t, &p );

#ifdef HAVE_LOCALE_H
  setlocale(LC_NUMERIC, save_locale);
  if(save_locale) free( save_locale );
#endif

  if(p && p != text && p[0] == '\000')
    error = 0;

  free( t );

  return error;
}


char *     oyjlReadFileStreamToMem   ( FILE              * fp,
                                       int               * size )
{
  size_t mem_size = 256;
  char* mem;
  int c;

  if(!fp) return NULL;

  mem = (char*) malloc(mem_size+1);
  if(!mem) return NULL;

  if(size)
  {
    *size = 0;
    do
    {
      c = getc(fp);
      if(*size >= (int)mem_size)
      {
        mem_size *= 2;
        mem = (char*) realloc( mem, mem_size+1 );
        if(!mem) { *size = 0; return NULL; }
      }
      mem[(*size)++] = c;
    } while(!feof(fp));

    --*size;
    mem[*size] = '\000';
  }

  return mem;
}

#include "oyjl_version.h"
#include <yajl/yajl_parse.h>
#include <yajl/yajl_version.h>
/** @brief  give the compiled in library version
 *
 *  @param[in]  type           0 - Oyjl API
 *                             1 - Yajl API
 *
 *  @return                    OYJL_VERSION at library compile time
 */
int            oyjlVersion           ( int                 type )
{
  if(type == 1)
    return YAJL_VERSION;

  return OYJL_VERSION;
}

/** @} *//* oyjl */
/** @} *//* misc */
