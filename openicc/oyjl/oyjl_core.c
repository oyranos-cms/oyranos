/** @file oyjl_core.c
 *
 *  oyjl - string, file i/o and other basic helpers
 *
 *  @par Copyright:
 *            2016-2019 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl core functions
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *
 * Copyright (c) 2004-2019  Kai-Uwe Behrmann  <ku.b@gmx.de>
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

#include <ctype.h>   /* isspace() */
#include <math.h>    /* NAN */
#include <stdarg.h>  /* va_list */
#include <stddef.h>  /* ptrdiff_t size_t */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>  /* wcslen() */

#include "oyjl.h"
#include "oyjl_macros.h"
#include "oyjl_version.h"
#include "oyjl_tree_internal.h"
#ifdef OYJL_HAVE_LOCALE_H
#include <locale.h>           /* setlocale LC_NUMERIC */
#endif
int oyjl_debug_local = 0;
int * oyjl_debug = &oyjl_debug_local;

/** @brief   set own debug variable */
void       oyjlDebugVariableSet      ( int               * debug )
{ oyjl_debug = debug; }

/** @brief   the default message handler to stderr
 *
 *  @version OpenICC: 0.1.0
 *  @date    2011/10/21
 *  @since   2008/04/03 (OpenICC: 0.1.0)
 */
int          oyjlMessageFunc         ( int/*oyjlMSG_e*/    error_code,
                                       const void        * context_object OYJL_UNUSED,
                                       const char        * format,
                                       ... )
{
  char * text = NULL;
  int error = 0;
  const char * status_text = NULL;

  OYJL_CREATE_VA_STRING(format, text, malloc, return 1)

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

/** \addtogroup oyjl_core Core
 *  @brief I/O and String Handling
 *
 *  Basic C FILE input and output is provided by oyjlWriteFile(), oyjlReadFile()
 *  and oyjlReadFileStreamToMem().
 *
 *  A convinient set of string API's is available in the oyjlStringXXX family.
 *  Those API's handle plain string arrays. oyjlStringAdd() uses variable args
 *  to format and append to a existing string. oyjlStringListXXX
 *  API's handle plain arrays of strings.
 *
 *  The oyjl_str based oyStrXXX API's use a more carful memory
 *  management and thus perform way faster on larger memory arrays as they
 *  need fewer allocations and copies. oyjlStrNew() allocates a new object,
 *  or oyjlStrNewFrom() wrappes a existing string array into a new object.
 *  oyjlStr() lets you see the contained char array. oyjlStrAppendN()
 *  performs fast concatenation. oyjlStrReplace() uses the object advantages.
 *  oyjlStrPull() directly takes the char array out of control of the oyjl_str
 *  object and oyjlStrRelease() frees the object and all memory.
 *
 *  @{ *//* oyjl_core */

/* return the beginning of the next word */
const char *   oyjlStringGetNext     ( const char        * text )
{
  /* remove leading white space */
  if(text && text[0] && isspace(text[0]))
  {
    while( text && text[0] && isspace(text[0]) ) text++;
    return text;
  }

  /* find the end of the word */
  while( text && text[0] && !isspace(text[0]) ) text++;

  /* find the next word */
  while( text && text[0] && isspace(text[0]) ) text++;

  return text && text[0] ? text : NULL;
}
int            oyjlStringNextSpace   ( const char        * text )
{
  int len = 0;
  while( text && text[len] && !isspace(text[len]) ) len++;
  return len;
}

/**
 *   Assume some text and extract the found words.
 *   The words can be separated by white space as 
 *   seen by isspace().
 */
char **        oyjlStringSplitSpace  ( const char        * text,
                                       int               * count,
                                       void*            (* alloc)(size_t))
{
  char ** list = NULL;
  int n = 0, i;

  /* split the string by empty space */
  if(text && text[0])
  {
    const char * tmp = text;

    if(!alloc) alloc = malloc;

    if(tmp && tmp[0] && !isspace(tmp[0])) ++n;
    while( tmp && tmp[0] && (tmp = oyjlStringGetNext( tmp )) != NULL ) ++n;

    if((list = alloc( (n+1) * sizeof(char*) )) == 0) return NULL;
    memset( list, 0, (n+1) * sizeof(char*) );

    {
      const char * start = text;
      if(start && isspace(start[0]))
        start = oyjlStringGetNext( start );

      for(i = 0; i < n; ++i)
      {
        int len = oyjlStringNextSpace( start );

        if((list[i] = alloc( len+1 )) == 0) return NULL;

        memcpy( list[i], start, len );
        list[i][len] = 0;
        start = oyjlStringGetNext( start );
      }
    }
  }

  if(count)
    *count = n;

  return list;
}

const char * oyjlStringDelimiter ( const char * text, const char * delimiter, int * length )
{
  int i,j, dn = delimiter ? strlen(delimiter) : 0, len = text?strlen(text):0;
  for(j = 0; j < len; ++j)
    for(i = 0; i < dn; ++i)
      if(text[j] && text[j] == delimiter[i])
      {
        if(length)
          *length = j;
        return &text[j];
      }
  return NULL;
}

/** @brief   convert a string into list
 *
 *  @param[in]     text                source string
 *  @param[in]     delimiter           the char which marks the split;
 *                                     e.g. comma ','; optional;
 *                                     default zero: extract white space separated words
 *  @param[out]    count               number of detected string segments; optional
 *  @param[in]     alloc               custom allocator; optional, default is malloc
 *  @return                            array of detected string segments
 */
char **        oyjlStringSplit       ( const char        * text,
                                       const char          delimiter,
                                       int               * count,
                                       void*            (* alloc)(size_t))
{
  char d[2] = { delimiter, '\000' };
  return oyjlStringSplit2( text, d, count, NULL, alloc );
}

/** @brief   convert a string into list
 *
 *  @param[in]     text                source string
 *  @param[in]     delimiter           the ASCII char which marks the split;
 *                                     e.g. comma ","; optional;
 *                                     default zero: extract white space separated words
 *  @param[out]    count               number of detected string segments; optional
 *  @param[out]    index               to be allocated array of detected delimiter indexes; The array will contain the list of indexes in text, which lead to the actual split positional index.; optional
 *  @param[in]     alloc               custom allocator; optional, default is malloc
 *  @return                            array of detected string segments
 */
char **        oyjlStringSplit2      ( const char        * text,
                                       const char        * delimiter,
                                       int               * count,
                                       int              ** index,
                                       void*            (* alloc)(size_t))
{
  char ** list = 0;
  int n = 0, i;

  /* split the path search string by a delimiter */
  if(text && text[0])
  {
    const char * tmp = text;

    if(!alloc) alloc = malloc;

    if(!delimiter || !delimiter[0])
      return oyjlStringSplitSpace( text, count, alloc );

    tmp = oyjlStringDelimiter(tmp, delimiter, NULL);
    if(tmp == text) ++n;
    tmp = text;
    do { ++n;
    } while( (tmp = oyjlStringDelimiter(tmp + 1, delimiter, NULL)) );

    tmp = 0;

    if((list = alloc( (n+1) * sizeof(char*) )) == NULL) return NULL;
    memset( list, 0, (n+1) * sizeof(char*) );
    if(index && (*index = alloc( n * sizeof(int) )) == NULL) { free(list); return NULL; }
    if(index) memset( *index, 0, n * sizeof(int) );

    {
      const char * start = text;
      for(i = 0; i < n; ++i)
      {
        intptr_t len = 0;
        int length = 0;
        const char * end = oyjlStringDelimiter(start, delimiter, &length);
        if(index && length) (*index)[i] = length + start - text;

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

/** @brief   duplicate a string with custom allocator
 *
 *  The function adds the allocator over standard strdup().
 *
 *  @param[in]     string              source string
 *  @param[in]     alloc               custom allocator; optional, default is malloc
 *  @return                            copy
 */
char *     oyjlStringCopy            ( const char        * string,
                                       void*            (* alloc)(size_t))
{
  char * text_copy = NULL;

  if(string)
  {
    oyjlAllocHelper_m( text_copy, char, strlen(string) + 1,
                        alloc, return NULL );
    strcpy( text_copy, string );
  }
    
  return text_copy;
}

/** @brief   sprintf with de-/allocator
 *
 *  The function adds memory management over standard sprintf().
 *
 *  @param[in]     string              source string
 *  @param[in]     alloc               custom allocator; optional, default is malloc
 *  @param[in]     deAlloc             custom deallocator matching alloc; optional, default is free
 *  @param[in]     format              printf style format string
 *  @param[in]     ...                 argument list for format
 *  @return                            error
 */
int        oyjlStringAdd             ( char             ** string,
                                       void*            (* alloc)(size_t size),
                                       void             (* deAlloc)(void * data ),
                                       const char        * format,
                                                           ... )
{
  char * text_copy = NULL;
  char * text = 0;

  void*(* allocate)(size_t size) = alloc?alloc:malloc;
  void (* deAllocate)(void * data ) = deAlloc?deAlloc:free;

  OYJL_CREATE_VA_STRING(format, text, alloc, return 1)

  if(string && *string)
  {
    int l = strlen(*string),
        l2 = strlen(text);
    text_copy = allocate( l2 + l + 1 );
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

/** @brief   append to the string end
 *
 *  For a plain memory duplication use:
 *  @code
    void * copy = oyjlStringAppendN( NULL, (const char*)ptr, ptr_len, malloc );
    @endcode
 *
 *  @param[in]     text                source string; optional
 *  @param[in]     append              to be added text to string; optional
 *  @param[in]     append_len          length of append
 *  @param[in]     alloc               custom allocator; optional, default is malloc
 *  @return                            constructed string+append
 */
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
    oyjlAllocHelper_m( text_copy, char,
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

/** @brief   append to the string end
 *
 *  @param[in]     text                source string
 *  @param[in]     append              to be added text to string
 *  @param[in]     append_len          length of append
 *  @param[in]     alloc               custom allocator; optional, default is malloc
 *  @param[in]     deAlloc             custom deallocator matching alloc; optional, default is free
 *  @return                            constructed string+append
 */
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

/** @brief   substitute pattern in a string
 *
 *  @param[in,out] text                source string for in place manipulation
 *  @param[in]     search              pattern to be tested in text
 *  @param[in]     replacement         string to be put in place of search sub string
 *  @param[in]     alloc               custom allocator; optional, default is malloc
 *  @param[in]     deAlloc             custom deallocator matching alloc; optional, default is free
 *  @return                            number of occurences
 */
int        oyjlStringReplace         ( char             ** text,
                                       const char        * search,
                                       const char        * replacement,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) )
{
  char * t = 0;
  oyjl_str str;
  int n = 0;

  if(!text || !*text || !(*text)[0])
    return 0;

  str = oyjlStrNewFrom(text, 0, alloc,deAlloc);
  n = oyjlStrReplace( str, search, replacement, 0, NULL );
  t = oyjlStrPull(str);
  *text = t;
  oyjlStrRelease( &str );

  return n;
}


/** @brief append a string list to an other string list */
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
      oyjlAllocHelper_m(nlist, char*, n_alt + n_app +1, alloc, return NULL);

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

/** @brief free a string list */
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

/** @brief append a string to a string list */
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

  if(!deAlloc) deAlloc = free;
  n_alt = *n;

  oyjlAllocHelper_m(nlist, char*, n_alt + 2, alloc, return);

  memmove( nlist, *list, sizeof(char*) * n_alt);
  nlist[n_alt] = oyjlStringCopy( string, alloc );
  nlist[n_alt+1] = NULL;

  *n = n_alt + 1;

  if(*list)
    deAlloc(*list);

  *list = nlist;
}

/** @brief filter doubles out
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

/** @brief append a string list to an other and handle memory */
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


/** show better const behaviour and return instant error status over strtol()
 *
 *  @return                            error
 */
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

/** @brief   text to double conversion
 *
 *  @param[in]     text                string
 *  @param[out]    value               resulting number
 *  @return                            error
 *                                     - 0 : text input was completely read as number
 *                                     - -1 : text input was read as number with white space or other text after
 *                                     - 1 : missed text input
 *                                     - 2 : no number detected
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/03/18
 *  @since   2011/11/17 (Oyranos: 0.2.0)
 */
int          oyjlStringToDouble      ( const char        * text,
                                       double            * value )
{
  char * p = NULL, * t = NULL;
  int len, pos = 0;
  int error = -1;
#ifdef OYJL_HAVE_LOCALE_H
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
  oyjlAllocHelper_m( t, char, len + 2*sizeof(double) + 1, malloc, return 1);
  memset( t, 0, len + 2*sizeof(double) + 1 );

  /* remove leading empty space */
  while(text[pos] && isspace(text[pos])) pos++;
  memcpy( t, &text[pos], len );

  *value = strtod( t, &p );

#ifdef OYJL_HAVE_LOCALE_H
  setlocale(LC_NUMERIC, save_locale);
  if(save_locale) free( save_locale );
#endif

  if(p && p != t && p[0] == '\000')
    error = 0;
  else if(p && p == t)
  {
    *value = NAN;
    error = 2;
  }

  free( t );

  return error;
}

/** @brief   text to double list
 *
 *  @param[in]     text                source string
 *  @param[in]     delimiter           the ASCII char(s) which mark the split; e.g. comma ","
 *  @param[out]    count               number of detected string segments; optional
 *  @param[in]     alloc               custom allocator; optional, default is malloc
 *  @return                            error
 *                                     - 0 : text input was completely read as number
 *                                     - -1 : text input was read as number with white space or other text after
 *                                     - 1 : missed text input
 *                                     - 2 : no number detected
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/07/30
 *  @since   2019/01/23 (Oyranos: 0.9.7)
 */
int          oyjlStringsToDoubles    ( const char        * text,
                                       const char        * delimiter,
                                       int               * count,
                                       void*            (* alloc)(size_t),
                                       double           ** value )
{
  int error = 0, l_error = 0;
  char ** list = NULL;
  int n = 0, i;
  double d;
  char * val;

  if(!text || !text[0])
    return 0;

  list = oyjlStringSplit2( text, delimiter, &n, NULL, alloc );
  if(n)
    oyjlAllocHelper_m( *value, double, n + 1, alloc, return 1);
  for( i = 0; i < n; ++i )
  {
    val = list[i];
    l_error = oyjlStringToDouble( val, &d );
    (*value)[i] = d;
    if(!error || l_error > 0) error = l_error;
    if(l_error > 0) break;
  }
  if(count)
    *count = n;

  return error;
}

/** @brief   number of letters in a UTF-8 string
 *
 *  A convinience wrapper for wcslen().
 *
 *  @param[in]     text                source string
 *  @return                            letters
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/08/06
 *  @since   2019/08/06 (Oyjl: 1.0.0)
 */
int        oyjlWStringLen            ( const char        * text )
{
  int len = strlen(text), wlen = 0;
  wchar_t * wcs = (wchar_t*) calloc( len + 1, sizeof(wchar_t) );
  if(wcs)
  {
    mbstowcs( wcs, text, len + 1 );
    wlen = wcslen(wcs);
    free(wcs);
  }
  return wlen;
}


/**
 *  A string representation with preallocation for faster memory
 *  work on especially larger strings.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/02/14
 *  @since   2019/02/14 (Oyjl: 1.0.0)
 */
struct oyjl_string_s
{
    char * s;                          /**< @brief UTF-8 text */
    size_t len;                        /**< @brief string length. */
    size_t alloc_len;                  /**< @brief last string allocation. */
    void*(*alloc)(size_t);             /**< @brief custom allocator; optional, default is malloc */
    void (*deAlloc)(void*);            /**< @brief custom deallocator; optional, default is free */
    int    alloc_count;
};

/** @brief   allocate string object
 *
 *  @param[in]     length              the preallocation length
 *  @param[in]     alloc               custom allocator; optional, default is malloc
 *  @param[in]     deAlloc             custom deallocator; optional, default is free
 *  @return                            the object
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/02/14
 *  @since   2019/02/14 (Oyjl: 1.0.0)
 */
oyjl_str   oyjlStrNew                ( size_t              length,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) )
{
  struct oyjl_string_s * string = NULL;
  if(!alloc) alloc = malloc;
  if(!deAlloc) deAlloc = free;

  oyjlAllocHelper_m( string, struct oyjl_string_s, 1, alloc, return NULL );
  string->len = 0;
  if(length == 0)
    length = 8;
  oyjlAllocHelper_m( string->s, char, length, alloc, return NULL );
  string->s[0] = '\000';
  string->alloc_len = length;
  string->alloc = alloc;
  string->deAlloc = deAlloc;
  string->alloc_count = 1;

  return (oyjl_str) string;
}

/** @brief   allocate string object from chars
 *
 *  Use this function to avoid allocation of already in memory char array.
 *  Be careful, that the memory alloc and deAlloc args match the text storage.
 *
 *  @param[in]     text                text to pull into new object
 *  @param[in]     length              the preallocation size of text or zero if unknown
 *  @param[in]     alloc               custom allocator; optional, default is malloc
 *  @param[in]     deAlloc             custom deallocator; optional, default is free
 *  @return                            the object
 *
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/02/15
 *  @since   2019/02/15 (Oyjl: 1.0.0)
 */
oyjl_str   oyjlStrNewFrom            ( char             ** text,
                                       size_t              length,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) )
{
  struct oyjl_string_s * string = NULL;
  if(!alloc) alloc = malloc;
  if(!deAlloc) deAlloc = free;

  oyjlAllocHelper_m( string, struct oyjl_string_s, 1, alloc, return NULL );
  string->len = strlen(*text);
  if(length == 0)
    length = string->len;
  string->s = *text;
  *text = NULL;
  string->alloc_len = length;
  string->alloc = alloc;
  string->deAlloc = deAlloc;
  string->alloc_count = 1;

  return (oyjl_str) string;
}

/** @brief   fast append to the string end
 *
 *  @param[in]     string              string object
 *  @param[in]     append              to be added text to string
 *  @param[in]     append_len          length of append
 *  @return                            error
 */
int        oyjlStrAppendN            ( oyjl_str            string,
                                       const char        * append,
                                       int                 append_len )
{
  struct oyjl_string_s * str = string;
  int error = 0;
  if(append && append_len)
  {
    if((append_len + str->len) >= str->alloc_len)
    {
      int len = (append_len + str->len) * 2;
      char * edit = str->s;
      oyjlAllocHelper_m( str->s, char, len, str->alloc, return 1 );
      str->alloc_len = len;
      ++str->alloc_count;
      memcpy(str->s, edit, str->len);
      str->deAlloc(edit);
    }
    memcpy( &str->s[str->len], append, append_len );
    str->len += append_len;
    str->s[str->len] = '\000';
  }
  return error;
}

/** @brief   sprintf for oyjl_str
 *
 *  The function adds memory management over standard sprintf().
 *
 *  @param[in]     string              source string
 *  @param[in]     format              printf style format string
 *  @param[in]     ...                 argument list for format
 *  @return                            error
 */
int        oyjlStrAdd                ( oyjl_str            string,
                                       const char        * format,
                                                           ... )
{
  struct oyjl_string_s * str = string;
  char * text = 0;

  void*(* allocate)(size_t size) = str->alloc;
  void (* deAllocate)(void * data ) = str->deAlloc;

  OYJL_CREATE_VA_STRING(format, text, allocate, return 1)

  if(text)
  {
    oyjlStrAppendN( string, text, strlen(text) );
    deAllocate( text );
  }

  return 0;
}


/** @brief   substitute pattern in a string
 *
 *  @param[in,out] text                source string for in place manipulation
 *  @param[in]     search              pattern to be tested in text
 *  @param[in]     replacement         string to be put in place of search sub string
 *  @param[in]     modifyReplacement   hook to dynamically modify the replacement text; optional
 *                                     - text: the full search text
 *                                     - start: current start inside text
 *                                     - end: current end inside text
 *                                     - search: used term to find actual start
 *                                     - replace: possibly modified replacement text
 *                                     - context: user data
 *  @param[in,out] context             optional user data for modifyReplacement
 *  @return                            number of occurences
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/08/02
 *  @since   2019/02/15 (Oyjl: 1.0.0)
 */
int        oyjlStrReplace            ( oyjl_str            text,
                                       const char        * search,
                                       const char        * replacement,
                                       void             (* modifyReplacement)(const char * text, const char * start, const char * end, const char * search, const char ** replace, void * user_data),
                                       void              * user_data )
{
  struct oyjl_string_s * str = text;
  oyjl_str t = NULL;
  const char * start, * end;
  int n = 0;

  if(!text)
    return 0;

  start = end = oyjlStr(text);

  if(start && search && replacement)
  {
    int s_len = strlen(search);
    while((end = strstr(start,search)) != 0)
    {
      if(!t) t = oyjlStrNew(10,0,0);
      oyjlStrAppendN( t, start, end-start );
      if(modifyReplacement) modifyReplacement( oyjlStr(text), start, end, search, &replacement, user_data );
      oyjlStrAppendN( t, replacement, strlen(replacement) );
      ++n;
      if(strlen(end) >= (size_t)s_len)
        start = end + s_len;
      else
      {
        if(strstr(start,search) != 0)
          oyjlStrAppendN( t, replacement, strlen(replacement) );
        start = end = end + s_len;
        break;
      }
    }
    if(n && start && end == NULL)
      oyjlStrAppendN( t, start, strlen(start) );
  }

  if(t)
  {
    void (* deAlloc)(void*) = str->deAlloc;
    if(str->s && str->alloc_len) deAlloc(str->s);
    str->len = str->alloc_len = 0;

    deAlloc = t->deAlloc;
    if(str->alloc == t->alloc)
    {
      str->s = t->s;
      str->len = t->len;
      str->alloc_len = t->alloc_len;
      str->alloc_count = t->alloc_count;
      deAlloc(t); t = NULL;
    } else
    {
      int length = 8;
      oyjlAllocHelper_m( str->s, char, length, str->alloc, return 0 );
      str->s[0] = '\000';
      str->alloc_len = length;
      oyjlStrAppendN( str, oyjlStr(t), strlen(oyjlStr(t)) );
      oyjlStrRelease( &t );
    }
  }

  return n;
}

/** @brief   move the wrapped char array out of the object
 *
 *  This function might be usefull if a plain string copy is too
 *  expensive and the object memory allocation can be further
 *  handled for the returned char pointer. After the operation
 *  the objects internal char array will be reset to zero length.
 *
 *  @param[in,out] str                 the object, which will be reseted
 *  @return                            the char array from str
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/02/15
 *  @since   2019/02/15 (Oyjl: 1.0.0)
 */
char *     oyjlStrPull               ( oyjl_str            str )
{
  struct oyjl_string_s * string;
  char * t = NULL;
  int length = 8;

  if(!str) return t;

  string = str;
  t = string->s;
  string->s = NULL;

  string->len = 0;
  oyjlAllocHelper_m( string->s, char, length, string->alloc, return NULL );
  string->s[0] = '\000';
  string->alloc_len = length;
  string->alloc_count = 1;
  
  return t;
}

/** @brief   clear text in a string object
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/06/14
 *  @since   2019/06/14 (Oyjl: 1.0.0)
 */
void       oyjlStrClear              ( oyjl_str            string )
{
  struct oyjl_string_s * str = string;
  void (* deAlloc)(void*) = str->deAlloc;
  char * s = oyjlStrPull( string );
  if(s) deAlloc(s);
}

/** @brief   release a string object
 *
 *  All references from previous oyjlStr() calls will be void.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/02/14
 *  @since   2019/02/14 (Oyjl: 1.0.0)
 */
void       oyjlStrRelease            ( oyjl_str          * string_ptr )
{
  struct oyjl_string_s * str;
  if(!string_ptr) return;
  str = *string_ptr;
  void (* deAlloc)(void*) = str->deAlloc;
  if(str->s) deAlloc(str->s);
  deAlloc(str);
  *string_ptr = NULL;
}

/** @brief   read only the wrapped char array
 *
 *  The returned array might become invalid with each
 *  further call to the string object.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/02/14
 *  @since   2019/02/14 (Oyjl: 1.0.0)
 */
const char*oyjlStr                   ( oyjl_str            string )
{
  return (const char*)string->s;
}

/** @brief read FILE into memory
 */
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

#include <errno.h>
#define WARNc_S(...) oyjlMessage_p( oyjlMSG_ERROR, 0, __VA_ARGS__ )
/** @brief read local file into memory
 */
char *    oyjlReadFile( const char * file_name,
                        int        * size_ptr )
{
  FILE * fp = NULL;
  int size = 0, s = 0;
  char * text = NULL;

  if(file_name)
  {
    fp = fopen(file_name,"rb");
    if(fp)
    {
      fseek( fp, 0L, SEEK_END );
      size = ftell( fp );
      if(size == -1)
      {
        switch(errno)
        {
          case EBADF:        WARNc_S("Not a seekable stream %d", errno); break;
          case EINVAL:       WARNc_S("Wrong argument %d", errno); break;
          default:           WARNc_S("%s", strerror(errno)); break;
        }
        if(size_ptr)
          *size_ptr = size;
        fclose( fp );
        return NULL;
      }
      rewind(fp);
      text = malloc(size+1);
      if(text == NULL)
      {
        WARNc_S( "Error: Could allocate memory: %lu", (long unsigned int)size);
        fclose( fp );
        return NULL;
      }
      s = fread(text, sizeof(char), size, fp);
      text[size] = '\000';
      if(s != size)
        WARNc_S( "Error: fread %lu but should read %lu",
                (long unsigned int) s, (long unsigned int)size);
      fclose( fp );
    } else
    {
      WARNc_S( "Error: Could not open file - \"%s\"", file_name);
    }
  }

  if(size_ptr)
    *size_ptr = size;

  return text;
}


#include <sys/stat.h> /* stat() */
int oyjlIsFileFull_ (const char* fullFileName, const char * read_mode)
{
  struct stat status;
  int r = 0;
  const char* name = fullFileName;

  memset(&status,0,sizeof(struct stat));
  if(name && name[0])
    r = stat(name, &status);

  if(r != 0 && *oyjl_debug > 1)
  switch (errno)
  {
    case EACCES:       WARNc_S("Permission denied: %s", name); break;
    case EIO:          WARNc_S("EIO : %s", name); break;
    case ENAMETOOLONG: WARNc_S("ENAMETOOLONG : %s", name); break;
    case ENOENT:       WARNc_S("A component of the name/file_name does not exist, or the file_name is an empty string: \"%s\"", name); break;
    case ENOTDIR:      WARNc_S("ENOTDIR : %s", name); break;
    case ELOOP:        WARNc_S("Too many symbolic links encountered while traversing the name: %s", name); break;
    case EOVERFLOW:    WARNc_S("EOVERFLOW : %s", name); break;
    default:           WARNc_S("%s : %s", strerror(errno), name); break;
  }

  r = !r &&
       (   ((status.st_mode & S_IFMT) & S_IFREG)
        || ((status.st_mode & S_IFMT) & S_IFLNK)
                                                );

  if (r)
  {
    FILE* fp = fopen (name, read_mode);
    if (!fp) {
      oyjlMessage_p( oyjlMSG_INFO, 0, "not existent: %s", name );
      r = 0;
    } else {
      fclose (fp);
    }
  }

  return r;
}
int oyjlIsDirFull_ (const char* name)
{
  struct stat status;
  int r = 0;

  if(!name) return 0;

  memset(&status,0,sizeof(struct stat));
  if(name && name[0])
    r = stat(name, &status);

  if(r != 0 && *oyjl_debug > 1)
  switch (errno)
  {
    case EACCES:       WARNc_S("Permission denied: %s", name); break;
    case EIO:          WARNc_S("EIO : %s", name); break;
    case ENAMETOOLONG: WARNc_S("ENAMETOOLONG : %s", name); break;
    case ENOENT:       WARNc_S("A component of the name/file_name does not exist, or the file_name is an empty string: \"%s\"", name); break;
    case ENOTDIR:      WARNc_S("ENOTDIR : %s", name); break;
#ifdef HAVE_POSIX
    case ELOOP:        WARNc_S("Too many symbolic links encountered while traversing the name: %s", name); break;
    case EOVERFLOW:    WARNc_S("EOVERFLOW : %s", name); break;
#endif
    default:           WARNc_S("%s : %s", strerror(errno), name); break;
  }
  r = !r &&
       ((status.st_mode & S_IFMT) & S_IFDIR);

  return r;
}

int   oyjlIsFile                     ( const char        * fullname,
                                       const char        * mode,
                                       char              * info,
                                       int                 info_len )
{
  struct stat status;
  int r;
  memset(&status,0,sizeof(struct stat));
  double mod_time = 0.0;

  r = oyjlIsFileFull_( fullname, mode );

  if (r)
  {
    stat(fullname, &status);
#   if defined(__APPLE__) || defined(BSD)
    mod_time = status.st_mtime ;
    mod_time += status.st_mtimespec.tv_nsec/1000000. ;
#   elif defined(WIN32)
    mod_time = (double)status.st_mtime ;
#   else
    mod_time = status.st_mtim.tv_sec ;
    mod_time += status.st_mtim.tv_nsec/1000000. ;
#   endif
    if(info)
      snprintf( info, info_len, "%.30f", mod_time );
  }

  return r;
}

char* oyjlExtractPathFromFileName_ (const char* file_name)
{
  char * path_name = NULL;
  char * ptr = NULL;

  if(!file_name) return NULL;

  path_name = strdup( file_name );
  if(path_name)
  {
    ptr = strrchr (path_name, '/');
    if(ptr)
      ptr[0] = 0;
    else
    {
      free(path_name);
      path_name = NULL;
    }
  }

  if(!path_name)
    path_name = strdup( "." );

  return path_name;
}

char * oyjlPathGetParent_ (const char* name)
{
  char *parentDir = 0, *ptr = 0;

  parentDir = strdup( name );
  ptr = strrchr( parentDir, '/');
  if (ptr)
  {
    if (ptr[1] == 0) /* ending dir separator */
    {
      ptr[0] = 0;
      if (strrchr( parentDir, '/'))
      {
        ptr = strrchr (parentDir, '/');
        ptr[0+1] = 0;
      }
    }
    else
      ptr[0+1] = 0;
  }

  return parentDir;
}

int oyjlMakeDir_ (const char* path)
{
  const char * full_name = path;
  char * path_parent = 0,
       * path_name = 0;
  int rc = !full_name;

#ifdef HAVE_POSIX
  mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH; /* 0755 */
#endif

  if(full_name)
    path_name = oyjlExtractPathFromFileName_(full_name);
  if(path_name)
  {
    if(!oyjlIsDirFull_(path_name))
    {
      path_parent = oyjlPathGetParent_(path_name);
      if(!oyjlIsDirFull_(path_parent))
        rc = oyjlMakeDir_(path_parent);
      if(path_parent) free( path_parent );

      if(!rc)
        rc = mkdir (path_name
#ifdef HAVE_POSIX
                            , mode
#endif
                                  );
      if(rc && *oyjl_debug > 1)
      switch (errno)
      {
        case EACCES:       WARNc_S("Permission denied: %s", path); break;
        case EIO:          WARNc_S("EIO : %s", path); break;
        case ENAMETOOLONG: WARNc_S("ENAMETOOLONG : %s", path); break;
        case ENOENT:       WARNc_S("A component of the path/file_name does not exist, or the file_name is an empty string: \"%s\"", path); break;
        case ENOTDIR:      WARNc_S("ENOTDIR : %s", path); break;
#ifdef HAVE_POSIX
        case ELOOP:        WARNc_S("Too many symbolic links encountered while traversing the path: %s", path); break;
        case EOVERFLOW:    WARNc_S("EOVERFLOW : %s", path); break;
#endif
        default:           WARNc_S("%s : %s", strerror(errno), path); break;
      }
    }
    free( path_name );;
  }

  return rc;
}


/** @brief write memory to FILE
 */
int  oyjlWriteFile                   ( const char        * filename,
                                       void              * mem,
                                       int                 size )
{
  FILE *fp = 0;
  const char * full_name = filename;
  int r = !filename;
  int written_n = 0;
  char * path = 0;

  if(!r)
  {
    path = oyjlExtractPathFromFileName_( full_name );
    r = oyjlMakeDir_( path );
  }

  if(!r)
  {
    fp = fopen(full_name, "wb");
    if ((fp != 0)
     && mem
     && size)
    {
#if 0
      do {
        r = fputc ( block[pt++] , fp);
      } while (--size);
#else
      written_n = fwrite( mem, 1, size, fp );
      if(written_n != size)
        r = errno;
#endif
    } else 
      if(mem && size)
        r = errno;
      else
        WARNc_S("no data to write into: \"%s\"", filename );

    if(r && *oyjl_debug > 1)
    {
      switch (errno)
      {
        case EACCES:       WARNc_S("Permission denied: %s", filename); break;
        case EIO:          WARNc_S("EIO : %s", filename); break;
        case ENAMETOOLONG: WARNc_S("ENAMETOOLONG : %s", filename); break;
        case ENOENT:       WARNc_S("A component of the path/file_name does not exist, or the file_name is an empty string: \"%s\"", filename); break;
        case ENOTDIR:      WARNc_S("ENOTDIR : %s", filename); break;
#ifdef HAVE_POSIX
        case ELOOP:        WARNc_S("Too many symbolic links encountered while traversing the path: %s", filename); break;
        case EOVERFLOW:    WARNc_S("EOVERFLOW : %s", filename); break;
#endif
        default:           WARNc_S("%s : %s", strerror(errno), filename);break;
      }
    }

    if (fp) fclose (fp);
  }

  if(path) free( path );

  return written_n;
}
/** @} *//* oyjl_core */

/** \addtogroup oyjl
 *  @{ *//* oyjl */


#define OyjlToString2_M(t) OyjlToString_M(t)
#define OyjlToString_M(t) #t
/** @brief   init the libraries language; optionaly
 *
 *  Additionally use setlocale() somewhere in your application.
 *  The message catalog search path is detected from the project specific
 *  environment variable specified in \em env_var_locdir and
 *  the LOCPATH environment variables. If those are not present
 *  a expected fall back directory from \em default_locdir is used.
 *
 *  @param         project_name        project name display string; e.g. "MyProject"
 *  @param         environment_debug_variable string; e.g. "MP_DEBUG"
 *  @param         debug_variable      int C variable; e.g. my_project_debug
 *  @param         use_gettext         switch gettext support on or off
 *  @param         env_var_locdir      environment variable string for locale path; e.g. "MP_LOCALEDIR"
 *  @param         default_locdir      default locale path C string; e.g. "/usr/local/share/locale"
 *  @param         loc_domain          locale domain string related to your pot, po and mo files; e.g. "myproject"
 *  @param         msg                 your message function of type oyjlMessage_f; optional - default is Oyjl message function
 *  @return                            error
 *                                     - -1 : issue
 *                                     - 0 : success
 *                                     - >= 1 : found error
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/01/13
 *  @since   2019/01/13 (Oyjl: 1.0.0)
 */
int oyjlInitLanguageDebug            ( const char        * project_name,
                                       const char        * env_var_debug,
                                       int               * debug_variable,
                                       int                 use_gettext,
                                       const char        * env_var_locdir,
                                       const char        * default_locdir,
                                       const char        * loc_domain,
                                       oyjlMessage_f       msg )
{
  int error = -1;

  if(!msg) msg = oyjlMessage_p;

  if(getenv(env_var_debug))
  {
    *debug_variable = atoi(getenv(env_var_debug));
    if(*debug_variable)
    {
      int v = oyjlVersion(0);
      if(*debug_variable)
        msg( oyjlMSG_INFO, 0, "%s (Oyjl compile v: %s runtime v: %d)", project_name, OYJL_VERSION_NAME, v );
    }
  }

#ifdef OYJL_USE_GETTEXT
  {
    if( use_gettext )
    {
      char * var = NULL;
      const char * environment_locale_dir = NULL,
                 * locpath = NULL,
                 * path = NULL,
                 * domain_path = default_locdir;

      if(getenv(env_var_locdir) && strlen(getenv(env_var_locdir)))
      {
        environment_locale_dir = domain_path = strdup(getenv(env_var_locdir));
        if(*debug_variable)
          msg( oyjlMSG_INFO, 0,"found environment variable: %s=%s", env_var_locdir, domain_path );
      } else
        if(environment_locale_dir == NULL && getenv("LOCPATH") && strlen(getenv("LOCPATH")))
      {
        domain_path = NULL;
        locpath = getenv("LOCPATH");
        if(*debug_variable)
          msg( oyjlMSG_INFO, 0,"found environment variable: LOCPATH=%s", locpath );
      } else
        if(*debug_variable)
        msg( oyjlMSG_INFO, 0,"no %s or LOCPATH environment variable found; using default path: %s", env_var_locdir, domain_path );

      if(domain_path || locpath)
      {
        oyjlStringAdd( &var, 0,0, "NLSPATH=");
        oyjlStringAdd( &var, 0,0, domain_path ? domain_path : locpath);
      }
      if(var)
        putenv(var); /* Solaris */

      /* LOCPATH appears to be ignored by bindtextdomain("domain", NULL),
       * so it is set here to bindtextdomain(). */
      path = domain_path ? domain_path : locpath;
# ifdef OYJL_HAVE_LIBINTL_H
      bindtextdomain( loc_domain, path );
#endif
      if(*debug_variable)
      {\
        char * fn = NULL;
        int stat = -1;
        const char * gettext_call = OyjlToString2_M(_());

        if(path)
          oyjlStringAdd( &fn, 0,0, "%s/de/LC_MESSAGES/%s.mo", path ? path : "", loc_domain);
        if(fn)
          stat = oyjlIsFileFull_( fn, "r" );
        msg( oyjlMSG_INFO, 0,"bindtextdomain(\"%s\") to %s\"%s\" %s for %s", loc_domain, locpath?"effectively ":"", path ? path : "", (stat > 0)?"Looks good":"Might fail", gettext_call );
        if(fn) free(fn);
      }
    }
  }
#endif /* OYJL_USE_GETTEXT */

  return error;
}


#include "oyjl_version.h"
#include <yajl/yajl_parse.h>
#include <yajl/yajl_version.h>
/** @brief  give the compiled in library version
 *
 *  @param[in]  type           request API type
 *                             - 0 - Oyjl API
 *                             - 1 - Yajl API
 *  @return                    OYJL_VERSION at library compile time
 */
int            oyjlVersion           ( int                 type )
{
  if(type == 1)
    return YAJL_VERSION;

  return OYJL_VERSION;
}

/** @} *//* oyjl */
