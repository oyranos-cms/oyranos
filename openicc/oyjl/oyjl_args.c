/** @file oyjl_args.c
 *
 *  oyjl - UI helpers
 *
 *  @par Copyright:
 *            2018-2021 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl argument handling
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *
 * Copyright (c) 2018-2021  Kai-Uwe Behrmann  <ku.b@gmx.de>
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


#include <stddef.h>
#include <ctype.h> /* toupper() */
#include <wchar.h>  /* wcslen() */
#include "oyjl.h"
#ifdef OYJL_HAVE_LANGINFO_H
#include <langinfo.h> /* nl_langinfo() */
#endif

#if defined(OYJL_INTERNAL)
#include "oyjl.h"
#include "oyjl_macros.h"
extern int * oyjl_debug;
#include "oyjl_i18n.h"
#include "oyjl_tree_internal.h"

#else /* OYJL_INTERNAL */
int my_debug = 0;
int * oyjl_debug = &my_debug;
#if defined(__GNUC__)
# define  OYJL_DBG_FORMAT "%s:%d %s() "
# define  OYJL_DBG_ARGS   strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : __FILE__,__LINE__,__func__
#else
# define  OYJL_DBG_FORMAT "%s:%d "
# define  OYJL_DBG_ARGS   strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : __FILE__,__LINE__
#endif
#define OYJL_CREATE_VA_STRING(format_, text_, alloc_, error_action) \
{ \
  va_list list; \
  size_t sz = 0; \
  int len = 0; \
  void*(* allocate_)(size_t size) = alloc_; \
  if(!allocate_) allocate_ = malloc; \
\
  text_ = NULL; \
  \
  va_start( list, format_); \
  len = vsnprintf( text_, sz, format_, list); \
  va_end  ( list ); \
\
  { \
    text_ = allocate_( sizeof(char) * len + 2 ); \
    if(!text_) \
    { \
      oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "could not allocate memory", OYJL_DBG_ARGS ); \
      error_action; \
    } \
    va_start( list, format_); \
    len = vsnprintf( text, len+1, format_, list); \
    va_end  ( list ); \
  } \
}

#define oyjlAllocHelper_m(ptr_, type, size_, alloc_func, action) { \
  if ((size_) <= 0) {                                       \
      oyjlMessage_p( oyjlMSG_INSUFFICIENT_DATA, 0, "Nothing to allocate"); \
  } else {                                                  \
      void*(*a)(size_t size) = alloc_func;                  \
      if(!a) a = malloc;                                    \
      ptr_ = (type*) a(sizeof (type) * (size_t)(size_));    \
      memset( ptr_, 0, sizeof (type) * (size_t)(size_) );   \
  }                                                         \
  if (ptr_ == NULL) {                                       \
      oyjlMessage_p( oyjlMSG_ERROR, 0, "Out of memory"); \
    action;                                                 \
  }                                                         \
}

char *   oyjlBT                      ( int                 stack_limit OYJL_UNUSED )
{
  return NULL;
}
extern oyjlMessage_f oyjlMessage_p;
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
oyjlMessage_f oyjlMessage_p = oyjlMessageFunc;
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
int        oyjlStringReplace         ( char             ** text,
                                       const char        * search,
                                       const char        * replacement,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) )
{
  char * t = 0;
  const char * start, * end;
  int n = 0;

  void*(* allocate)(size_t size) = alloc?alloc:malloc;
  void (* deAllocate)(void * data ) = deAlloc?deAlloc:free;

  if(!text)
    return 0;

  start = end = *text;

  if(start && search && replacement)
  {
    int s_len = strlen(search);
    while((end = strstr(start,search)) != 0)
    {
      oyjlStringAddN( &t, start, end-start, allocate, deAllocate );
      oyjlStringAddN( &t, replacement, strlen(replacement), allocate, deAllocate );
      ++n;
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
    if(n && start && end == NULL)
      oyjlStringAddN( &t, start, strlen(start), allocate, deAllocate );
  }

  if(t)
  {
    deAllocate(*text);
    *text = t;
  }

  return n;
}


struct oyjl_string_s
{
    char * s;                          /**< @brief UTF-8 text */
    size_t len;                        /**< @brief string length. */
    size_t alloc_len;                  /**< @brief last string allocation. */
    void*(*alloc)(size_t);             /**< @brief custom allocator; optional, default is malloc */
    void (*deAlloc)(void*);            /**< @brief custom deallocator; optional, default is free */
    int    alloc_count;
};
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
void       oyjlStrClear              ( oyjl_str            string )
{
  struct oyjl_string_s * str = string;
  void (* deAlloc)(void*) = str->deAlloc;
  char * s = oyjlStrPull( string );
  if(s) deAlloc(s);
}
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

const char*oyjlStr                   ( oyjl_str            string )
{
  return (const char*)string->s;
}
int      oyjlStringToLong            ( const char        * text,
                                       long              * value )
{
  char * end = 0;
  *value = strtol( text, &end, 0 );
  if(end && end != text && isdigit(text[0]) && !isdigit(end[0]) )
    return 0;
  else
    return 1;
}
int          oyjlStringToDouble      ( const char        * text,
                                       double            * value )
{
  char * end = NULL, * t = NULL;
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
    goto clean_oyjlStringToDouble;
  }

  /* avoid irritating valgrind output of "Invalid read of size 8"
   * might be a glibc error or a false positive in valgrind */
  oyjlAllocHelper_m( t, char, len + 2*sizeof(double) + 1, malloc, error = 1; goto clean_oyjlStringToDouble);
  memset( t, 0, len + 2*sizeof(double) + 1 );

  /* remove leading empty space */
  while(text[pos] && isspace(text[pos])) pos++;
  memcpy( t, &text[pos], len );

  *value = strtod( t, &end );

  if(end && end != text && isdigit(text[0]) && !isdigit(end[0]) )
    error = 0;
  else if(end && end == t)
  {
    *value = NAN;
    error = 2;
  }

clean_oyjlStringToDouble:

#ifdef OYJL_HAVE_LOCALE_H
  setlocale(LC_NUMERIC, save_locale);
  if(save_locale) free( save_locale );
#endif

  if(t) free( t );

  return error;
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
static int     oyjlStringNextSpace_  ( const char        * text )
{
  int len = 0;
  while( text && text[len] && !isspace(text[len]) ) len++;
  return len;
}
static char ** oyjlStringSplitSpace_ ( const char        * text,
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
        int len = oyjlStringNextSpace_( start );

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

    if(!delimiter)
      return oyjlStringSplitSpace_( text, count, alloc );

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
/*
* Index into the table below with the first byte of a UTF-8 sequence to
* get the number of trailing bytes that are supposed to follow it.
* Note that *legal* UTF-8 values can't have 4 or 5-bytes. The table is
* left as-is for anyone who may want to do such conversion, which was
* allowed in earlier algorithms.
*/
static const char trailingBytesForUTF8[256] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};
int        oyjlStringSplitUTF8       ( const char        * text,
                                       char            *** mbchars,
                                       void*            (* alloc)(size_t) )
{
  int len = strlen(text), wlen = 0;
  int pos = 0;
  if(mbchars)
    oyjlAllocHelper_m( *mbchars, char*, len + 1, alloc, return -2);
  while(pos < len && text[pos])
  {
    const char * ctext = &text[pos];
    int c = (unsigned char)ctext[0];
    int trailing_bytes = trailingBytesForUTF8[c];
    if(trailing_bytes > 3)
      break;
    if(mbchars)
    {
      oyjlAllocHelper_m( (*mbchars)[wlen], char, 4 + 1, alloc, return -2);
      memcpy((*mbchars)[wlen], ctext, trailing_bytes + 1);
    }
    pos += trailing_bytes;
    /*fprintf( stderr, "WString: current char: %d  next letter: %s trailing bytes: %d\n", c, &text[pos+1], trailing_bytes );*/
    ++pos;
    ++wlen;
  }
  return wlen;
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
#include "oyjl_version.h"
int            oyjlVersion           ( int                 type OYJL_UNUSED )
{
  return OYJL_VERSION;
}
#define WARNc_S(...) oyjlMessage_p( oyjlMSG_ERROR, 0, __VA_ARGS__ )
#include <sys/stat.h> /* stat() */
#include <errno.h>
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
static char * oyjl_nls_path_ = NULL;
void oyjlLibRelease() { if(oyjl_nls_path_) { putenv("NLSPATH=C"); free(oyjl_nls_path_); } }
#define OyjlToString2_M(t) OyjlToString_M(t)
#define OyjlToString_M(t) #t
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
      char * var = NULL, * tmp = NULL;
      const char * environment_locale_dir = NULL,
                 * locpath = NULL,
                 * path = NULL,
                 * domain_path = default_locdir;

      if(getenv(env_var_locdir) && strlen(getenv(env_var_locdir)))
      {
        tmp = strdup(getenv(env_var_locdir));
        environment_locale_dir = domain_path = tmp;
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
      if(var) /* do not release */
      {
        putenv(var); /* Solaris */
        if(oyjl_nls_path_)
          free(oyjl_nls_path_);
        oyjl_nls_path_ = var;
      }

      /* LOCPATH appears to be ignored by bindtextdomain("domain", NULL),
       * so it is set here to bindtextdomain(). */
      path = domain_path ? domain_path : locpath;
# ifdef OYJL_HAVE_LIBINTL_H
      bindtextdomain( loc_domain, path );
#endif
      if(*debug_variable)
      {
        char * fn = NULL;
        int stat = -1;
        const char * gettext_call = OyjlToString2_M(_());

        if(path)
          oyjlStringAdd( &fn, 0,0, "%s/de/LC_MESSAGES/%s.mo", path ? path : "", loc_domain);
        if(fn)
          stat = oyjlIsFileFull_( fn, "r" );
        msg( oyjlMSG_INFO, 0,"bindtextdomain(\"%s\") to %s\"%s\" %s for %s  test:%s", loc_domain, locpath?"effectively ":"", path ? path : "", (stat > 0)?"Looks good":"Might fail", gettext_call, _("Example") );
        if(fn) free(fn);
      }
      if(tmp)
        free(tmp);
    }
  }
#endif /* OYJL_USE_GETTEXT */

  return error;
}

#define oyjlMEMORY_ALLOCATION_SECTIONS 0x01
#define oyjlMEMORY_ALLOCATION_ARRAY    0x02
#define oyjlMEMORY_ALLOCATION_GROUPS   0x04
#define oyjlMEMORY_ALLOCATION_OPTIONS  0x08

typedef struct {
  char       ** options; /* detected vanilla args + probably "@" for anonymous args */
  const char ** values; /* the vanilla args from main(argv[]) */
  int           count; /* number of detected options */
  int           group; /* detected group */
  void        * attr; /* oyjl_val attributes */
  int           memory_allocation; /* 0: as usual; 1 - sections, 2 - opts->groups and 4 - opts->array are owned and need to be released */
} oyjlOptsPrivate_s;
#endif /* OYJL_INTERNAL */

#ifdef OYJL_HAVE_LANGINFO_H
#include <langinfo.h> /* nl_langinfo() */
#endif

/** \addtogroup oyjl_args
 *  @{ */

/** @brief    Release dynamic structure
 *  @memberof oyjlOptionChoice_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2018/08/14
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
void oyjlOptionChoice_Release     ( oyjlOptionChoice_s**choices )
{
  int n = 0,i;
  oyjlOptionChoice_s * ca;
  if(!choices || !*choices) return;
  ca = *choices;
  while(ca[n].nick && ca[n].nick[0] != '\000') ++n;
  for(i = 0; i < n; ++i)
  {
    oyjlOptionChoice_s * c = &ca[i];
    if(c->nick) free(c->nick);
    if(c->name) free(c->name);
    if(c->description) free(c->description);
    if(c->help) free(c->help);
  }
  *choices = NULL;
  free(*choices);
}

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
static int oyjlTermColorCheck_()
{
  struct stat sout, serr;
  int color_term = 0;

  if( fstat( fileno(stdout), &sout ) == -1 )
    return 0;

  if(*oyjl_debug)
  switch( sout.st_mode & S_IFMT )
  {
    case S_IFBLK:  fprintf(stderr, "block device\n");            break;
    case S_IFCHR:  fprintf(stderr, "character device\n");        break;
    case S_IFDIR:  fprintf(stderr, "directory\n");               break;
    case S_IFIFO:  fprintf(stderr, "FIFO/pipe\n");               break;
    case S_IFLNK:  fprintf(stderr, "symlink\n");                 break;
    case S_IFREG:  fprintf(stderr, "regular file\n");            break;
    case S_IFSOCK: fprintf(stderr, "socket\n");                  break;
    default:       fprintf(stderr, "unknown?\n");                break;
  }

  if( fstat( fileno(stderr), &serr ) == -1 )
    return color_term;

  if(*oyjl_debug)
  switch( serr.st_mode & S_IFMT )
  {
    case S_IFBLK:  fprintf(stderr, "block device\n");            break;
    case S_IFCHR:  fprintf(stderr, "character device\n");        break;
    case S_IFDIR:  fprintf(stderr, "directory\n");               break;
    case S_IFIFO:  fprintf(stderr, "FIFO/pipe\n");               break;
    case S_IFLNK:  fprintf(stderr, "symlink\n");                 break;
    case S_IFREG:  fprintf(stderr, "regular file\n");            break;
    case S_IFSOCK: fprintf(stderr, "socket\n");                  break;
    default:       fprintf(stderr, "unknown?\n");                break;
  }

  if( S_ISCHR( sout.st_mode ) &&
      S_ISCHR( serr.st_mode ) )
    color_term = 1;
  if(*oyjl_debug)
    fprintf(stderr, "color_term: %d\n", color_term );

  return color_term;
}

#ifndef OYJL_CTEND
/* true color codes */
#define OYJL_RED_TC "\033[38;2;240;0;0m"
#define OYJL_GREEN_TC "\033[38;2;0;250;100m"
#define OYJL_BLUE_TC "\033[38;2;0;150;255m"
/* basic color codes */
#define OYJL_BOLD "\033[1m"
#define OYJL_ITALIC "\033[3m"
#define OYJL_UNDERLINE "\033[4m"
#define OYJL_RED_B "\033[0;31m"
#define OYJL_GREEN_B "\033[0;32m"
#define OYJL_BLUE_B "\033[0;34m"
/* switch back */
#define OYJL_CTEND "\033[0m"
#endif
const char * oyjlTermColor( oyjlTEXTMARK_e rgb, const char * text) {
  int len = strlen(text);
  static char t[256];
  static int colorterm_init = 0;
  static const char * oyjl_colorterm = NULL;
  static int truecolor = 0,
             color = 0;
  if(!colorterm_init)
  {
    colorterm_init = 1;
    oyjl_colorterm = getenv("COLORTERM");
    color = oyjl_colorterm != NULL ? 1 : 0;
    if(!oyjl_colorterm) oyjl_colorterm = getenv("TERM");
    truecolor = oyjl_colorterm && strcmp(oyjl_colorterm,"truecolor") == 0;
    if(!oyjlTermColorCheck_())
      truecolor = color = 0;
    if( getenv("FORCE_COLORTERM") )
      truecolor = color = 1;
    if( getenv("FORCE_NO_COLORTERM") )
      truecolor = color = 0;
  }
  if(len < 200)
  {
    switch(rgb)
    {
      case oyjlRED: sprintf( t, "%s%s%s", truecolor ? OYJL_RED_TC : color ? OYJL_RED_B : "", text, truecolor || color ? OYJL_CTEND : "" ); break;
      case oyjlGREEN: sprintf( t, "%s%s%s", truecolor ? OYJL_GREEN_TC : color ? OYJL_GREEN_B : "", text, truecolor || color ? OYJL_CTEND : "" ); break;
      case oyjlBLUE: sprintf( t, "%s%s%s", truecolor ? OYJL_BLUE_TC : color ? OYJL_BLUE_B : "", text, truecolor || color ? OYJL_CTEND : "" ); break;
      case oyjlBOLD: sprintf( t, "%s%s%s", truecolor || color ? OYJL_BOLD : "", text, truecolor || color ? OYJL_CTEND : "" ); break;
      case oyjlITALIC: sprintf( t, "%s%s%s", truecolor || color ? OYJL_ITALIC : "", text, truecolor || color ? OYJL_CTEND : "" ); break;
      case oyjlUNDERLINE: sprintf( t, "%s%s%s", truecolor || color ? OYJL_UNDERLINE : "", text, truecolor || color ? OYJL_CTEND : "" ); break;
    }
    return t;
  } else
    return text;
}

/** @brief    Return number of array elements
 *  @memberof oyjlOptionChoice_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/06/17
 *  @since   2019/06/17 (Oyjl: 1.0.0)
 */
int  oyjlOptionChoice_Count          ( oyjlOptionChoice_s* list )
{
  int n = 0;
  if(!list) return 0;
  if(list)
    while(list[n].nick && list[n].nick[0] != '\000')
      ++n;
  return n;
}


/** @brief    Return number of "oiwi" array elements
 *  @memberof oyjlOptions_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2018/08/14
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
int oyjlOptions_Count             ( oyjlOptions_s  * opts )
{
  int n = 0;
  if(!(opts && opts->array)) return 0;
  while( *(oyjlOBJECT_e*)&opts->array[n] /*"oiwi"*/ == oyjlOBJECT_OPTION) ++n;
  return n;
}

/** @brief    Return number of "oiwi" groups elements
 *  @memberof oyjlOptions_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2018/08/14
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
int oyjlOptions_CountGroups       ( oyjlOptions_s  * opts )
{
  int n = 0;
  if(!(opts && opts->groups)) return 0;
  while( *(oyjlOBJECT_e*)&opts->groups[n] /*"oiwg"*/ == oyjlOBJECT_OPTION_GROUP) ++n;
  return n;
}

#define OYJL_E(x_) (x_?x_:"")
#define OYJL_IS_NOT_O( x ) (!o->o || strcmp(o->o,x) != 0)
#define OYJL_IS_O( x ) (o->o && strcmp(o->o,x) == 0)
enum {
  oyjlOPTIONSTYLE_ONELETTER = 0x01,                     /* the single dash '-' style option is prefered: -o */
  oyjlOPTIONSTYLE_STRING = 0x02,                        /* add double dash '-' style option as well: --long-option */
  oyjlOPTIONSTYLE_OPTIONAL_START = 0x04,                /* print the option as optional: EBNF [--option] */
  oyjlOPTIONSTYLE_OPTIONAL_END = 0x08,                  /* print the option as optional: EBNF [--option] */
  oyjlOPTIONSTYLE_OPTIONAL_INSIDE_GROUP = 0x10,         /* add second level grouping: [[--option1|--option2]] */
  oyjlOPTIONSTYLE_MAN = 0x20,                           /* add Unix MAN page groff syntax decoration */
  oyjlOPTIONSTYLE_MARKDOWN = 0x40                       /* add markdown syntax decoration */
};
#define oyjlOPTIONSTYLE_OPTIONAL (oyjlOPTIONSTYLE_OPTIONAL_START | oyjlOPTIONSTYLE_OPTIONAL_END)
#define oyjlOPTIONSTYLE_LINK_GROUP     0x100            /* add link to group, e.g. from synopsis line in markdown */
#define oyjlOPTIONSTYLE_LINK_SYNOPSIS  0x200            /* add link to synopsis, e.g. in markdown */
static
char *       oyjlOption_PrintArg     ( oyjlOption_s      * o,
                                       int                 style )
{
  char * text = NULL;
  int sub_command = ((style & OYJL_GROUP_FLAG_SUBCOMMAND) && o->option) ? OYJL_GROUP_FLAG_SUBCOMMAND : 0;
  if(!o) return oyjlStringCopy("", malloc);
  if(style & oyjlOPTIONSTYLE_OPTIONAL_START)
    oyjlStringAdd( &text, malloc, free, "[" );
  if((style & oyjlOPTIONSTYLE_ONELETTER || !o->option) && o->o && o->o[0] && OYJL_IS_NOT_O("@") && OYJL_IS_NOT_O("#") && !sub_command)
  {
    if(style & oyjlOPTIONSTYLE_MAN)
      oyjlStringAdd( &text, malloc, free, "\\fB\\-%s\\fR", o->o?o->o:o->option );
    else if(style & oyjlOPTIONSTYLE_MARKDOWN)
      oyjlStringAdd( &text, malloc, free, "<strong>-%s</strong>", o->o?o->o:o->option );
    else
      oyjlStringAdd( &text, malloc, free, "-%s", oyjlTermColor( oyjlBOLD, o->o?o->o:o->option ) );
  }
  if(style & oyjlOPTIONSTYLE_ONELETTER && OYJL_IS_O("#"))
    oyjlStringAdd( &text, malloc, free, "|" );
  if(style & oyjlOPTIONSTYLE_ONELETTER && style & oyjlOPTIONSTYLE_STRING && OYJL_IS_NOT_O("@") && OYJL_IS_NOT_O("#") && o->o && o->o[0] && o->option && !sub_command)
    oyjlStringAdd( &text, malloc, free, "|" );
  if( o->option &&
      ( style & oyjlOPTIONSTYLE_STRING ||
        ( !(o->o && o->o[0]) &&
          OYJL_IS_NOT_O("@") &&
          OYJL_IS_NOT_O("#")
        ) ||
        sub_command
      )
    )
  {
    if(style & oyjlOPTIONSTYLE_MAN)
      oyjlStringAdd( &text, malloc, free, "\\fB%s%s\\fR", sub_command ? "" : "\\-\\-", o->option );
    else if(style & oyjlOPTIONSTYLE_MARKDOWN)
      oyjlStringAdd( &text, malloc, free, "<strong>%s%s</strong>", sub_command ? "" : "--", o->option );
    else
      oyjlStringAdd( &text, malloc, free, "%s%s", sub_command ? "" : "--", oyjlTermColor( oyjlBOLD, o->option ) );
  }
  if(o->value_name)
  {
    const char * value_name = o->value_name;
    int m = value_name[0] == '['; /* m=move : migth be explicitely used together with OYJL_OPTION_FLAG_ACCEPT_NO_ARG */
    if(m) /* expect optional arg(s) for this option and *move* the first opening '[' square bracket before the equal '=' sign */
      ++value_name;
    if(style & oyjlOPTIONSTYLE_MAN)
      oyjlStringAdd( &text, malloc, free, "%s\\fI%s%s%s\\fR",
          OYJL_IS_NOT_O("@") && OYJL_IS_NOT_O("#") && !(m == 0 && o->flags&OYJL_OPTION_FLAG_ACCEPT_NO_ARG) ? " ":"",
          (m == 0 && o->flags&OYJL_OPTION_FLAG_ACCEPT_NO_ARG)?"[=":"",
          o->value_name,
          (m == 0 && o->flags&OYJL_OPTION_FLAG_ACCEPT_NO_ARG)?"]":"" );
    else
    {
      if(style & oyjlOPTIONSTYLE_MARKDOWN)
        oyjlStringAdd( &text, malloc, free, "%s%s%s%s%s</em>",
            (m||o->flags&OYJL_OPTION_FLAG_ACCEPT_NO_ARG)?"<em>[":"",
            OYJL_IS_NOT_O("@") && !(style & oyjlOPTIONSTYLE_STRING)?"=":" ",
            (m||o->flags&OYJL_OPTION_FLAG_ACCEPT_NO_ARG) ? "" : "<em>",
            value_name,
            (m == 0 && o->flags&OYJL_OPTION_FLAG_ACCEPT_NO_ARG)?"]":"" ); /* allow for easier word wrap in table */
      else if(style & oyjlOPTIONSTYLE_OPTIONAL_INSIDE_GROUP)
        oyjlStringAdd( &text, malloc, free, "%s%s",
            (!o->o || strcmp(o->o, "@")) != 0?"=":"",
            oyjlTermColor(oyjlITALIC,o->value_name) );
      else
      {
        char * t = NULL;
        oyjlStringAdd( &t, malloc, free, "%s%s%s%s",
            (m||o->flags&OYJL_OPTION_FLAG_ACCEPT_NO_ARG)?"[":"",
            (!o->o || strcmp(o->o, "@")) != 0?"=":"",
            value_name,
            (m == 0 && o->flags&OYJL_OPTION_FLAG_ACCEPT_NO_ARG)?"]":"" );
        oyjlStringAdd( &text, malloc, free, "%s", oyjlTermColor(oyjlITALIC,t) );
        free(t);
      }
    }
  }
  if(o->flags & OYJL_OPTION_FLAG_REPETITION)
    oyjlStringAdd( &text, malloc, free, "..." );
  if(style & oyjlOPTIONSTYLE_OPTIONAL_END)
    oyjlStringAdd( &text, malloc, free, "]" );
  return text;
}
#define oyjlHELP 0x01
#define oyjlDESCRIPTION 0x02
/** @internal
 * 
 * @param[in]      flags               modifier
 *                                     - oyjlDESCRIPTION
 *                                     - oyjlHELP
 */
static char* oyjlOption_PrintArgDouble_( oyjlOption_s    * o,
                                       int                 flags )
{
  char * text = NULL;
  oyjlStringAdd( &text, malloc, free, "%s%s%s (%s%s%g [≥%g ≤%g Δ%g])",
      (o->description && flags & oyjlDESCRIPTION) ? o->description:"",
      (o->help && flags & oyjlHELP)?": ":"", (o->help && flags & oyjlHELP)?o->help :"",
      o->value_name?o->value_name:"", o->value_name?":":"", o->values.dbl.d, o->values.dbl.start, o->values.dbl.end, o->values.dbl.tick );
  return text;
}

oyjlOptionChoice_s oyjl_h_choices_[] = {
                                    {"-", "", "", ""},
                                    {"synopsis", "", "", ""},
                                    {"","","",""}};
oyjlOptionChoice_s oyjl_X_choices_[] = {
                                    {"man", "", "", ""},
                                    {"markdown", "", "", ""},
                                    {"json", "", "", ""},
                                    {"json+command", "", "", ""},
                                    {"export", "", "", ""},
                                    {"","","",""}};
oyjlOptionChoice_s oyjl_R_choices_[] = {
                                    {"gui", "", "", ""},
                                    {"web", "", "", ""},
                                    {"-","","",""},
                                    {"","","",""}};
static void oyjlOptions_EnrichInbuild_( oyjlOption_s * o )
{
  /* enrich inbuild variables */
  if(!o->o)
    return;
  if(strcmp(o->o, "h") == 0)
  {
    if(o->value_type == oyjlOPTIONTYPE_CHOICE && o->values.choices.list == NULL)
    {
      oyjl_h_choices_[0].name = _("Full Help");
      oyjl_h_choices_[0].description = _("Print help for all groups");
      oyjl_h_choices_[0].help = "";
      oyjl_h_choices_[1].name = _("Synopsis");
      oyjl_h_choices_[1].description = _("List groups");
      oyjl_h_choices_[1].help = _("Show all groups including syntax");
      o->values.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)oyjl_h_choices_, sizeof(oyjl_h_choices_), malloc );
      if(o->value_name == NULL)
      {
        o->value_name = "synopsis|...";
        if(o->name == NULL)
        {
          o->name = _("help");
          if(o->description == NULL)
          {
            o->description = _("Print help text");
            if(o->help == NULL)
              o->help = _("Show usage information and hints for the tool.");
          }
        }
      }
    }
  }
  if(strcmp(o->o, "X") == 0)
  {
    if(o->value_type == oyjlOPTIONTYPE_CHOICE && o->values.choices.list == NULL)
    {
      oyjl_X_choices_[0].name = _("Man");
      oyjl_X_choices_[0].description = _("Unix Man page");
      oyjl_X_choices_[0].help = _("Get a unix man page");
      oyjl_X_choices_[1].name = _("Markdown");
      oyjl_X_choices_[1].description = _("Formated text");
      oyjl_X_choices_[1].help = _("Get formated text");
#if defined(OYJL_INTERNAL)
      oyjl_X_choices_[2].name = _("Json");
      oyjl_X_choices_[2].description = _("GUI");
      oyjl_X_choices_[2].help = _("Get a Oyjl Json UI declaration");
      oyjl_X_choices_[3].name = _("Json + Command");
      oyjl_X_choices_[3].description = _("GUI + Command");
      oyjl_X_choices_[3].help = _("Get Oyjl Json UI declaration incuding command");
      oyjl_X_choices_[4].name = _("Export");
      oyjl_X_choices_[4].description = _("All available data");
      oyjl_X_choices_[4].help = _("Get UI data for developers");
#else
      oyjl_X_choices_[2].nick = "";
      oyjl_X_choices_[3].nick = "";
      oyjl_X_choices_[4].nick = "";
#endif
      o->values.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)oyjl_X_choices_, sizeof(oyjl_X_choices_), malloc );
      if(o->value_name == NULL)
      {
#if defined(OYJL_INTERNAL)
        o->value_name = "json|json+command|man|markdown";
#else
        o->value_name = "man|markdown";
#endif
        if(o->name == NULL)
        {
          o->name = _("export");
          if(o->description == NULL)
          {
            o->description = _("Export formated text");
            if(o->help == NULL)
              o->help = _("Get UI converted into text formats");
          }
        }
      }
    }
  }
  if(strcmp(o->o, "R") == 0)
  {
    if(o->value_type == oyjlOPTIONTYPE_CHOICE && o->values.choices.list == NULL)
    {
#if defined(OYJL_INTERNAL)
      oyjl_R_choices_[0].name = _("Gui");
      oyjl_R_choices_[0].description = _("Show UI");
      oyjl_R_choices_[0].help = _("Display a interactive graphical User Interface.");
      oyjl_R_choices_[1].name = _("Web");
      oyjl_R_choices_[1].description = _("Start Web Server");
      oyjl_R_choices_[1].help = _("Start a local Web Service to connect a Webbrowser with.");
#endif
      o->values.choices.list = (oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)oyjl_R_choices_, sizeof(oyjl_R_choices_), malloc );
      if(o->value_name == NULL)
      {
#if defined(OYJL_INTERNAL)
        o->value_name = "gui|web|";
#endif
        if(o->name == NULL)
        {
          o->name = _("render");
          if(o->description == NULL)
          {
            o->description = _("Select Renderer");
            if(o->help == NULL)
              o->help = _("Select and possibly configure Renderer. -R=\"gui\" will just launch a graphical UI. -R=\"port_number:api_path:TLS_private_key:TLS_CA_certificate:style.css\" will launch a local Web Server, which listens on local port.");
          }
        }
      }
    }
  }
}

static int oyjlStringDelimiterCount_( const char * text, const char * delimiter )
{
  int i,j, dn = delimiter ? strlen(delimiter) : 0, len = text?strlen(text):0, n = 0;
  if(len) ++n;
  for(j = 0; j < len; ++j)
    for(i = 0; i < dn; ++i)
      if(text[j] && text[j] == delimiter[i])
        ++n;
  return n;
}

/** @brief    Obtain the specified option from one letter member::o
 *  @memberof oyjlOptions_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/07/29
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
oyjlOption_s * oyjlOptions_GetOption ( oyjlOptions_s     * opts,
                                       const char        * ol )
{
  int i;
  int nopts = oyjlOptions_Count( opts );
  oyjlOption_s * o = NULL;

  if(!ol) return o;

  for(i = 0; i < nopts; ++i)
  {
    o = &opts->array[i];
    if(o->o && strcmp(o->o, ol) == 0)
    {
      if(strcmp(ol, "h") == 0 && o->value_type == oyjlOPTIONTYPE_CHOICE && o->values.choices.list == NULL)
        oyjlOptions_EnrichInbuild_(o);
      if(strcmp(ol, "X") == 0 && o->value_type == oyjlOPTIONTYPE_CHOICE && o->values.choices.list == NULL)
        oyjlOptions_EnrichInbuild_(o);
      if(strcmp(ol, "R") == 0 && o->value_type == oyjlOPTIONTYPE_CHOICE && o->values.choices.list == NULL)
        oyjlOptions_EnrichInbuild_(o);

      return o;
    }
    else
      o = NULL;
  }
  return o;
}

static int oyjlOptions_IsHelp_       ( oyjlOptions_s     * opts )
{
  int help = 0;
  oyjlOption_s * h = oyjlOptions_GetOption( opts, "h" );
  if(h && h->variable_type == oyjlINT && h->variable.i)
    help = *h->variable.i;
  if(h && h->variable_type == oyjlSTRING && h->variable.s)
    help = *h->variable.s && (*h->variable.s)[0];
  if(h && h->variable_type == oyjlNONE)
    oyjlOptions_GetResult( opts, h->o, 0, 0, &help );
  return help;
}
/** @brief    Obtain the specified option from option string
 *  @memberof oyjlOptions_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/03/04
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
oyjlOption_s * oyjlOptions_GetOptionL( oyjlOptions_s     * opts,
                                       const char        * ostring,
                                       int                 flags )
{
  int i;
  int nopts = oyjlOptions_Count( opts );
  oyjlOption_s * o = NULL;
  char * str = oyjlStringCopy(ostring, malloc);
  char * t = strchr(str, '=');
  char ol[8];

  memset(ol, 0, 8);
  if(t)
    t[0] = '\000';
  t = strchr(str, '.');
  if(t)
    t[0] = '\000';

  if(str[0])
  {
    int len = strlen(str);
    if(len == 1)
      strcpy( ol, str );
    else
    {
      int l = 0;
      oyjlStringDelimiter(str, "|,", &l);
      if(l > 0)
        str[l] = '\000';

      if(oyjlStringSplitUTF8(str,NULL,0) == 1)
        strcpy( ol, str );
    }
  }

  for(i = 0; i < nopts; ++i)
  {
    o = &opts->array[i];
    if( (ol[0] && o->o && strcmp(o->o, ol) == 0) ||
        (!ol[0] && o->option && strcmp(o->option, str) == 0)
      )
    {
      if(strcmp(str, "help") == 0 && o->value_type == oyjlOPTIONTYPE_CHOICE && o->values.choices.list == NULL)
        oyjlOptions_EnrichInbuild_(o);
      if(strcmp(str, "export") == 0 && o->value_type == oyjlOPTIONTYPE_CHOICE && o->values.choices.list == NULL)
        oyjlOptions_EnrichInbuild_(o);
      if(strcmp(str, "render") == 0 && o->value_type == oyjlOPTIONTYPE_CHOICE && o->values.choices.list == NULL)
        oyjlOptions_EnrichInbuild_(o);
      free(str);

      return o;
    }
    else
      o = NULL;
  }
  if( !(flags & OYJL_QUIET) &&
      !oyjlOptions_IsHelp_( opts ) )
  {
    fprintf( stderr, "%s%s: %s %d\n", *oyjl_debug?oyjlBT(0):"", _("Option not found"), str, flags );
  }
  free(str);

  return o;
}
static oyjlOPTIONSTATE_e oyjlOptions_Check_(
                                       oyjlOptions_s  * opts )
{
  int i,j;
  int nopts = oyjlOptions_Count( opts );
  oyjlOption_s * o = NULL, * b = NULL;

  for(i = 0; i < nopts; ++i)
  {
    o = &opts->array[i];
    for(j = i+1; j < nopts; ++j)
    {
      b = &opts->array[j];
      if(o->o && b->o && strcmp(o->o, b->o) == 0)
      {
        fprintf( stderr, "%s %s \'%s\'\n", oyjlTermColor(oyjlRED,_("Usage Error:")), _("Double occuring option"), b->o );
        return oyjlOPTION_DOUBLE_OCCURENCE;
      }

    }
    /* some error checking */
    /*if(!(('0' <= o->o && o->o <= '9') ||
         ('a' <= o->o && o->o <= 'z') ||
         ('A' <= o->o && o->o <= 'Z') ||
         o->o == '@' || o->o == '#' || o->o == '\000' || o->o == '|'))
    {
      fprintf( stderr, "%s %s \'%s\' %s\n", _("Usage Error:"), _("Option not supported"), o->o, _("need range 0-9 or a-z or A-Z") );
      return oyjlOPTION_NOT_SUPPORTED;
    }*/
    if(OYJL_IS_NOT_O("#") && o->value_name && o->value_name[0] && o->value_type == oyjlOPTIONTYPE_NONE)
    {
      fputs( oyjlTermColor(oyjlRED,_("Usage Error:")), stderr ); fputs( " ", stderr );
      fprintf( stderr, "%s \'%s\': %s\n", _("Option not supported"), oyjlTermColor(oyjlBOLD,o->o), _("need a value_type") );
      return oyjlOPTION_NOT_SUPPORTED;
    }
    if( OYJL_IS_NOT_O("#") &&
        OYJL_IS_NOT_O("h") &&
        OYJL_IS_NOT_O("X") &&
        OYJL_IS_NOT_O("R") &&
        o->value_type == oyjlOPTIONTYPE_CHOICE &&
        !((o->flags & OYJL_OPTION_FLAG_EDITABLE) || o->values.choices.list))
    {
      fputs( oyjlTermColor(oyjlRED,_("Usage Error:")), stderr ); fputs( " ", stderr );
      fprintf( stderr, "%s \'%s\' %s\n", _("Option not supported"), oyjlTermColor(oyjlBOLD,o->o), _("needs OYJL_OPTION_FLAG_EDITABLE or choices") );
      return oyjlOPTION_NOT_SUPPORTED;
    }
  }
  return oyjlOPTION_NONE;
}
#if 0
/* list of static strings */
void       oyjlStringListStaticAdd   ( const char      *** list,
                                       int               * n,
                                       const char        * string,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) )
{
  const char ** nlist = 0;
  int n_alt;

  if(!list || !n) return;

  if(!deAlloc) deAlloc = free;
  n_alt = *n;

  oyjlAllocHelper_m(nlist, const char*, n_alt + 2, alloc, return);

  memmove( nlist, *list, sizeof(const char*) * n_alt);
  nlist[n_alt] = string;
  nlist[n_alt+1] = NULL;

  *n = n_alt + 1;

  if(*list)
    deAlloc(*list);

  *list = nlist;
}
#endif

void oyjlOptions_Print_              ( oyjlOptions_s     * opts,
                                       int                 pos )
{
  int i;
  for(i = 0; i < opts->argc; ++i)
    fprintf( stderr, "%s ", i == pos ? oyjlTermColor( oyjlBOLD, opts->argv[i] ) : opts->argv[i] );
  fprintf( stderr, "\n" );
}

/** @brief    Parse the options into a private data structure
 *  @memberof oyjlOptions_s
 *
 *  The returned status can be used to detect usage errors and hint them on
 *  the command line.
 *  In the usual case where the variable fields are set, the results
 *  will be set too.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2020/10/14
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
oyjlOPTIONSTATE_e oyjlOptions_Parse  ( oyjlOptions_s     * opts )
{
  oyjlOPTIONSTATE_e state = oyjlOPTION_NONE;
  oyjlOption_s * o;
  oyjlOptsPrivate_s * result;

  if(!opts) return state;

  result = (oyjlOptsPrivate_s*) opts->private_data;
  if(!result)
    result = (oyjlOptsPrivate_s*) calloc( 1, sizeof(oyjlOptsPrivate_s) );

  /* parse the command line arguments */
  if(!result->values)
  {
    int i, len = 0;
    for(i = 1; i < opts->argc; ++i)
      if(opts->argv[i])
        len += strlen(opts->argv[i]);
    /* The first string contains the detected single char options */
    result->options = (char**) calloc( len + 1, sizeof(char*) );
    result->values = (const char**) calloc( len + 1, sizeof(char*) );
    result->group = -1;
    if((state = oyjlOptions_Check_(opts)) != oyjlOPTION_NONE)
      goto clean_parse;
    for(i = 1; i < opts->argc; ++i)
    {
      const char * str = opts->argv[i];
      const char * long_arg = NULL;
      const char * value = NULL;
      int l = 0,
          require_value, might_have_value = 0;

      if(str)
          l = strlen(str);
      else
          continue;

      if(strstr(str,"-qmljsdebugger") != NULL) /* detect a QML option */
        continue;

      /* parse -a | -a value | -a=value | -ba | -ba value | -ba=value */
           if(l > 1 && str[0] == '-' && str[1] != '-')
      {
        int j;
        char ** multi_byte_letters = NULL;
        l = oyjlStringSplitUTF8(str, &multi_byte_letters, malloc);
        for(j = 1; j < l; ++j)
        {
          const char * arg = multi_byte_letters[j];
          o = oyjlOptions_GetOption( opts, arg );
          if( !o )
          {
            oyjlOptions_Print_( opts, i );
            fputs( oyjlTermColor(oyjlRED,_("Usage Error:")), stderr ); fputs( " ", stderr );
            fprintf( stderr, "%s \'%s\'\n", _("Option not supported"), oyjlTermColor(oyjlBOLD,arg) );
            state = oyjlOPTION_NOT_SUPPORTED;
            break;
          }
          require_value = o->value_type != oyjlOPTIONTYPE_NONE;
          if(o->flags & OYJL_OPTION_FLAG_ACCEPT_NO_ARG)
            might_have_value = 1;

          value = NULL;

          if( (require_value || might_have_value) &&
              j == l-1 && opts->argc > i+1 && (opts->argv[i+1][0] != '-' || strlen(opts->argv[i+1]) <= 1) )
          {
            value = opts->argv[i+1];
            ++i;
          }
          else if( str[j+1] == '=' )
          {
            ++j;
            value = &str[j+1];
            j = l;
          }

          if( require_value )
          {
            if(!(value && value[0]) && !might_have_value)
            {
                char * t = oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING);
                oyjlOptions_Print_( opts, i );
                fputs( oyjlTermColor(oyjlRED,_("Usage Error:")), stderr ); fputs( " ", stderr );
                fprintf( stderr, "%s \'%s\' (%s)\n", _("Option needs a argument"), oyjlTermColor(oyjlBOLD,arg), t );
                free(t);
              state = oyjlOPTION_MISSING_VALUE;
            }

            result->options[result->count] = strdup(o->o?o->o:o->option);
            result->values[result->count] = value?value:"1";
            ++result->count;
          }
          else if( might_have_value )
          {
            result->options[result->count] = strdup(o->o?o->o:o->option);
            result->values[result->count] = value?value:"1";
            ++result->count;
          }
          else if(!require_value && !(j < l-1 && str[j+1] == '='))
          {
            result->options[result->count] = strdup(o->o?o->o:o->option);
            result->values[result->count] = "1";
            ++result->count;
          }
          else
          {
            char * t = oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING);
            oyjlOptions_Print_( opts, i );
            fputs( oyjlTermColor(oyjlRED,_("Usage Error:")), stderr ); fputs( " ", stderr );
            fprintf( stderr, "%s \'%s\' (%s)\n", _("Option has a unexpected argument"), arg?arg:"", t );
            free(t);
            state = oyjlOPTION_UNEXPECTED_VALUE;
            j = l;
          }
        }
        oyjlStringListRelease( &multi_byte_letters, l, free );
      }
      /* parse --arg | --arg value | --arg=value */
      else if(l > 2 && str[0] == '-' && str[1] == '-')
      {
        long_arg = &str[2];
        o = oyjlOptions_GetOptionL( opts, long_arg, 0 );
        if( !o ||
            (o && o->flags & OYJL_OPTION_FLAG_NO_DASH) )
        {
          oyjlOptions_Print_( opts, i );
          fputs( oyjlTermColor(oyjlRED,_("Usage Error:")), stderr ); fputs( " ", stderr );
          fprintf( stderr, "%s \'%s\'\n", _("Option not supported with double dash"), oyjlTermColor(oyjlBOLD,long_arg) );
          state = oyjlOPTION_NOT_SUPPORTED;
          goto clean_parse;
        }
        require_value = o->value_type != oyjlOPTIONTYPE_NONE;
        if(o->flags & OYJL_OPTION_FLAG_ACCEPT_NO_ARG)
          might_have_value = 1;

        value = NULL;

        if( strchr(str, '=') != NULL )
          value = strchr(str, '=') + 1;
        else if( (require_value || might_have_value) &&
            opts->argc > i+1 && opts->argv[i+1][0] != '-' )
        {
          value = opts->argv[i+1];
          ++i;
        }

        if( require_value )
        {
          if( !(value && value[0]) && !might_have_value )
          {
            char * t = oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING);
            oyjlOptions_Print_( opts, i );
            fputs( oyjlTermColor(oyjlRED,_("Usage Error:")), stderr ); fputs( " ", stderr );
            fprintf( stderr, "%s \'%s\' (%s)\n", _("Option needs a argument"), oyjlTermColor(oyjlBOLD,long_arg), t );
            free(t);
            state = oyjlOPTION_MISSING_VALUE;
          }

          result->options[result->count] = strdup(o->o?o->o:o->option);
          result->values[result->count] = value?value:"1";
          ++result->count;
        } else
        {
          if(!value)
          {
            result->options[result->count] = strdup(o->o?o->o:o->option);
            result->values[result->count] = "1";
            ++result->count;
          }
          else if( might_have_value && value )
          {
            result->options[result->count] = strdup(o->o?o->o:o->option);
            result->values[result->count] = value;
            ++result->count;
          }
          else
          {
            char * t = oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING);
            oyjlOptions_Print_( opts, i );
            fprintf( stderr, "%s %s \'%s\' (%s)\n", oyjlTermColor(oyjlRED,_("Usage Error:")), _("Option has a unexpected argument"), value?value:"", t );
            free(t);
            state = oyjlOPTION_UNEXPECTED_VALUE;
          }
        }
      }
      /* parse anonymous value, if requested */
      else
      {
        o = NULL;

        /* support sub command group selector */
        if( l > 2 )
        {
          o = oyjlOptions_GetOptionL( opts, str, OYJL_QUIET );
          if(o && o->flags & OYJL_OPTION_FLAG_ACCEPT_NO_ARG)
            might_have_value = 1;
          if(o && o->value_type == oyjlOPTIONTYPE_NONE)
          {
            result->options[result->count] = strdup(o->o?o->o:o->option);
            result->values[result->count] = "1";
            state = oyjlOPTION_SUBCOMMAND;
          }
          if( might_have_value )
          {
            value = NULL;

            if( strchr(str, '=') != NULL )
              value = strchr(str, '=') + 1;
            else if( opts->argc > i+1 && opts->argv[i+1][0] != '-' )
            {
              value = opts->argv[i+1];
              ++i;
            }
            if(!result->options[result->count])
              result->options[result->count] = strdup(o->o?o->o:o->option);
            if(value)
              result->values[result->count] = value;
            else
              result->values[result->count] = "1";
            state = oyjlOPTION_SUBCOMMAND;
          }
          else if( o && !might_have_value && value )
          {
            char * t = oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING);
            oyjlOptions_Print_( opts, i );
            fputs( oyjlTermColor(oyjlRED,_("Usage Error:")), stderr ); fputs( " ", stderr );
            fprintf( stderr, "%s (%s)\n", _("This option expects arguments"), t );
            fprintf( stderr, "%s\n", _("Options with arguments are not allowed in sub command style. A sub command has no leading '--'. It is a mandatory option of a option group.") );
            free(t);
            result->options[result->count] = strdup(o->o?o->o:o->option);
            result->values[result->count] = "0";
            state = oyjlOPTION_NOT_ALLOWED_AS_SUBCOMMAND;
          }
          else if(o)
          {
            if(!result->options[result->count])
              result->options[result->count] = strdup(o->o?o->o:o->option);
            result->values[result->count] = "1";
          }
        }

        if(!o)
        {
          o = oyjlOptions_GetOption( opts, "@" );
          if(o)
            value = str;
          if(value)
          {
            result->values[result->count] = value;
            result->options[result->count] = strdup("@");
          }
          else
          {
            oyjlOptions_Print_( opts, i );
            fputs( oyjlTermColor(oyjlRED,_("Usage Error:")), stderr ); fputs( " ", stderr );
            fprintf( stderr, "%s: \"%s\"\n", _("Unbound options are not supported"), oyjlTermColor(oyjlBOLD,opts->argv[i]) );
            state = oyjlOPTION_NOT_SUPPORTED;
            goto clean_parse;
          }
        }
        ++result->count;
      }
    }
    opts->private_data = result;

    i = 0;
    while(result->options[i])
    {
      oyjlOption_s * o = oyjlOptions_GetOptionL( opts, result->options[i], 0 );
      if(o)
      switch(o->variable_type)
      {
        case oyjlNONE:   break;
        case oyjlSTRING: oyjlOptions_GetResult( opts, o->o, o->variable.s, 0, 0 ); break;
        case oyjlDOUBLE: oyjlOptions_GetResult( opts, o->o, 0, o->variable.d, 0 ); break;
        case oyjlINT:    oyjlOptions_GetResult( opts, o->o, 0, 0, o->variable.i ); break;
      }
      ++i;
    }

    o = oyjlOptions_GetOption( opts, "#" );
    if(opts->argc == 1)
    {
      if(!o)
      {
        oyjlOptions_Print_( opts, 0 );
        fprintf( stderr, "%s %s\n", oyjlTermColor(oyjlRED,_("Usage Error:")), _("Optionless mode not supported. (That would need '#' option declaration.)") );
        state = oyjlOPTIONS_MISSING;
        return state;
      } else if(o->variable_type == oyjlINT && o->variable.i)
        *o->variable.i = 1;
    }

    /** Put the count of found anonymous arguments into '@' options variable.i of variable_type oyjlINT. */
    o = oyjlOptions_GetOption( opts, "@" );
    if(o && o->variable_type == oyjlINT && o->variable.i)
    {
      int count = 0;
      /* detect all '@' anonymous arguments */
      char ** results = oyjlOptions_ResultsToList( opts, "@", &count );
      *o->variable.i = count;
      oyjlStringListRelease( &results, count, free );
    }
  }

  return state;

clean_parse:
  oyjlStringListRelease( &result->options, result->count, free );
  free(result->values);
  result->values = NULL;
  result->count = 0;
  result->group = -1;

  return state;
}

/** @brief    Obtain the parsed result
 *  @memberof oyjlOptions_s
 *
 *  This function is only useful, if the results shall be obtained
 *  independently from oyjlOptions_Parse().
 *
 *  If the option was not specified the state oyjlOPTION_NONE will be
 *  returned and otherwise oyjlOPTION_USER_CHANGED. With result_int and
 *  a option type of oyjlOPTIONTYPE_NONE, the number of occurences is
 *  obtained, e.g. -vvv will give result_int = 3. A option type
 *  oyjlOPTIONTYPE_DOUBLE can ask for the floating point result with a
 *  result_dbl argument.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/08/04
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
oyjlOPTIONSTATE_e oyjlOptions_GetResult (
                                       oyjlOptions_s     * opts,
                                       const char        * opt,
                                       const char       ** result_string,
                                       double            * result_dbl,
                                       int               * result_int )
{
  oyjlOPTIONSTATE_e state = oyjlOPTION_NONE;
  int pos = -1, i, hits = 0;
  const char * t;
  oyjlOptsPrivate_s * results;
  oyjlOption_s * o = oyjlOptions_GetOption( opts, opt );

  if(!opts) return state;
  results = opts->private_data;

  if(!results || !results->values)
  /* parse the command line arguments */
    state = oyjlOptions_Parse( opts );
  if(state != oyjlOPTION_NONE)
    return state;

  results = opts->private_data;
  if(!results)
    return state;

  if(opt == NULL && results->count)
  {
    if(result_int)
      *result_int = results->count;
    return oyjlOPTION_USER_CHANGED;
  }

  /* flat search */
  for(i = 0; i < results->count; ++i)
  {
    if(strcmp(results->options[i], opt) == 0)
    {
      pos = i;
      ++hits;
      state = oyjlOPTION_USER_CHANGED;
    }
  }
  /* object search */
  if(pos == -1)
  for(i = 0; i < results->count; ++i)
  {
    if((o->o && strcmp(results->options[i], o->o) == 0) ||
       (o->option && strcmp(results->options[i], o->option) == 0))
    {
      pos = i;
      ++hits;
      state = oyjlOPTION_USER_CHANGED;
    }
  }

  if(pos == -1)
    return oyjlOPTION_NONE;

  t = results->values[pos];

  if(result_string)
    *result_string = t;

  if(result_dbl)
  {
    oyjlStringToDouble( t, result_dbl );
    if( o->value_type == oyjlOPTIONTYPE_DOUBLE &&
        ( o->values.dbl.start > *result_dbl ||
          o->values.dbl.end < *result_dbl) )
    {
      char * t = oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING);
      char * desc = oyjlOption_PrintArgDouble_( o, 0 );
      i = pos + 1;
      oyjlOptions_Print_( opts, i );
      fputs( oyjlTermColor(oyjlRED,_("Usage Error:")), stderr ); fputs( " ", stderr );
      fprintf( stderr, "%s: \"%s\" %s %s !: %g\n", _("Option has a different value range"), oyjlTermColor(oyjlBOLD,opts->argv[i]), t, desc, *result_dbl );
      free( t );
      free( desc );
    }
  }
  if(result_int)
  {
    if(o->value_type == oyjlOPTIONTYPE_NONE)
    {
      *result_int = hits;
    } else if(t)
    {
      long lo = 0;
      if(oyjlStringToLong( t, &lo ) == 0)
        *result_int = lo;
    }
  }

  return state;
}

/** @brief    Convert the parsed content to a text list
 *  @memberof oyjlOptions_s
 *
 *  @param[in]     opts                the argument object
 *  @param[in]     oc                  a filter; use NULL to get all results;
 *                                     e.g. use "@" for all anonymous results
 *  @param[out]    count               the number of matched results
 *  @return                            a possibly filterd string list of results;
 *                                     Without a filter it contains the argument
 *                                     Id followed by double point and the
 *                                     result following.
 *                                     With filter it contains only results if
 *                                     apply and without Id.
 *                                     The memory is owned by caller.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/08/06
 *  @since   2019/03/25 (Oyjl: 1.0.0)
 */
char **  oyjlOptions_ResultsToList   ( oyjlOptions_s     * opts,
                                       const char        * oc,
                                       int               * count )
{
  char * text = NULL,
       ** list = NULL;
  oyjlOptsPrivate_s * results = NULL;
  int i,list_len = 0;
  oyjlOption_s * o = oyjlOptions_GetOption( opts, oc );

  if(!opts) return NULL;
  results = opts->private_data;
  if(!results || !results->values)
  {
    if(oyjlOptions_Parse( opts ))
      return NULL;

    results = opts->private_data;
    if(!results)
      return NULL;
  }

  for(i = 0; i < results->count; ++i)
  {
    const char * option = results->options[i];
    const char * value = results->values[i];
    if(oc == NULL)
      oyjlStringAdd( &text, malloc, free, "%s:%s", option, value );
    else if(option && option[0] &&
            ((o->o && o->o[0] && strcmp(option,o->o) == 0) ||
             (o->option && o->option[0] && strcmp(option,o->option) == 0)))
      oyjlStringAdd( &text, malloc, free, "%s", value );
    if(text)
    {
      oyjlStringListAddStaticString( &list, &list_len, text, malloc, free );
      free(text); text = NULL;
    }
  }
  if(count)
    *count = list_len;

  return list;
}

/** @brief    Convert the parsed content to simple text
 *  @memberof oyjlOptions_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2018/08/14
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
char * oyjlOptions_ResultsToText  ( oyjlOptions_s  * opts )
{
  char * text = NULL;
  oyjlOptsPrivate_s * results = opts->private_data;
  int i;

  if(!results || !results->values)
  {
    if(oyjlOptions_Parse( opts ))
      return NULL;

    results = opts->private_data;
    if(!results)
      return NULL;
  }

  for(i = 0; i < results->count; ++i)
  {
    const char * option = results->options[i];
    const char * value = results->values[i];
    oyjlStringAdd( &text, malloc, free, "%s:%s\n", option, value );
  }

  return text;
}

/** @internal
 *  @brief   provide identifier string for a group
 *  @memberof oyjlOptions_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2020/04/18
 *  @since   2020/04/18 (Oyjl: 1.0.0)
 */
const char * oyjlOptions_GetGroupId_ ( oyjlOptions_s  *    opts,
                                       oyjlOptionGroup_s * g )
{
  char ** m_list = NULL;
  const char * group_id = NULL;
  int m = 0;

  m_list = oyjlStringSplit2( g->mandatory, "|,", &m, NULL, malloc );

  if(m_list && m)
  {
    const char * option = m_list[0];
    oyjlOption_s * o = oyjlOptions_GetOptionL( opts, option, 0 );
    if(option[0] != '@' && !(option[0] == '#' && m) && o)
      group_id = o->option;
  }

  oyjlStringListRelease( &m_list, m, free );
  return group_id;
}
            
/** @internal
 *  @brief    Print synopsis of a option group to stderr
 *  @memberof oyjlOptions_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2020/04/13
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
char *       oyjlOptions_PrintHelpSynopsis_ (
                                       oyjlOptions_s  *    opts,
                                       oyjlOptionGroup_s * g,
                                       int                 style )
{
  int i;
  char ** m_list = NULL, ** on_list = NULL;
  int * m_index = NULL, * on_index = NULL;
  int m = oyjlStringDelimiterCount_(g->mandatory, ",|");
  int on = oyjlStringDelimiterCount_(g->optional, ",|");
  int opt_group = 0;
  int gstyle = style | g->flags;
  const char * prog = opts->argv[0];
  char * text = oyjlStringCopy( "", malloc );
  if(prog && strchr(prog,'/'))
    prog = strrchr(prog,'/') + 1;

  style |= g->flags;

  if( m || on )
  {
    if(style & oyjlOPTIONSTYLE_MAN)
      oyjlStringAdd( &text, malloc, free, "\\fB%s\\fR", prog );
    else if(style & oyjlOPTIONSTYLE_MARKDOWN)
    {
      if(style & oyjlOPTIONSTYLE_LINK_SYNOPSIS)
        oyjlStringAdd( &text, malloc, free, " <a href=\"#synopsis\">" );
      oyjlStringAdd( &text, malloc, free, "<strong>%s</strong>", prog );
      if(style & oyjlOPTIONSTYLE_LINK_SYNOPSIS)
        oyjlStringAdd( &text, malloc, free, "</a>" );
    }
    else
      oyjlStringAdd( &text, malloc, free, "%s", oyjlTermColor(oyjlBOLD,prog) );
  }
  else
    return text;

  m_list = oyjlStringSplit2( g->mandatory, "|,", &m, &m_index, malloc );
  for(i = 0; i < m; ++i)
  {
    char * option = m_list[i];
    oyjlOption_s * o = oyjlOptions_GetOptionL( opts, option, 0 );
    char next_delimiter = g->mandatory[m_index[i]];
    if(!o)
    {
      fprintf(stdout, "%s %s: option not declared: \"%s\" \"%s\"\n", oyjlBT(0), g->name?oyjlTermColor(oyjlBOLD,g->name):"---", option, g->mandatory);
      if(!getenv("OYJL_NO_EXIT")) exit(1);
    }
    if(option[0] != '@' && !(option[0] == '#' && m+on == 1))
    {
      int s = style;
      if(i) s = style & (~OYJL_GROUP_FLAG_SUBCOMMAND);
      char * t = oyjlOption_PrintArg(o, s);
      const char * group_id = oyjlOptions_GetGroupId_( opts, g );
      if(i == 0 && o->option && style & oyjlOPTIONSTYLE_MARKDOWN && style & oyjlOPTIONSTYLE_LINK_GROUP && group_id)
        oyjlStringAdd( &text, malloc, free, " <a href=\"#%s\">%s</a>", group_id, t );
      else
        oyjlStringAdd( &text, malloc, free, " %s", t );
      free(t);
    }
    if(next_delimiter == '|')
      oyjlStringAdd( &text, malloc, free, " |" );
  }

  on_list = oyjlStringSplit2( g->optional, "|,", &on, &on_index, malloc );
  style = style & (~OYJL_GROUP_FLAG_SUBCOMMAND);
  for(i = 0; i < on; ++i)
  {
    char * option = on_list[i];
    oyjlOption_s * o = oyjlOptions_GetOptionL( opts, option, 0 );
    char next_delimiter = g->optional[on_index[i]];
    gstyle = style | oyjlOPTIONSTYLE_OPTIONAL;
    if(i < on - 1 && next_delimiter == '|')
    {
      if(opt_group == 0)
        gstyle = style | oyjlOPTIONSTYLE_OPTIONAL_START | oyjlOPTIONSTYLE_OPTIONAL_INSIDE_GROUP;
      else
        gstyle = style | oyjlOPTIONSTYLE_OPTIONAL_INSIDE_GROUP;
      opt_group = 1;
    }
    else if(opt_group)
    {
      gstyle = style | oyjlOPTIONSTYLE_OPTIONAL_END;
      opt_group = 0;
    }
    else if(!o)
    {
      fprintf(stdout, "%s%s: option not declared: %s\n", oyjlBT(0), g->name?g->name:"---", &g->optional[i]);
      if(!getenv("OYJL_NO_EXIT")) exit(1);
    }
    {
      char * t = oyjlOption_PrintArg(o, gstyle);
      oyjlStringAdd( &text, malloc, free, "%s%s", gstyle & oyjlOPTIONSTYLE_OPTIONAL_START ? " ":"", t );
      free(t);
    }
    if(next_delimiter == '|')
    {
      oyjlStringAdd( &text, malloc, free, "|" );
    }
  }
  for(i = 0; i < m; ++i)
  {
    char * option = m_list[i];
    oyjlOption_s * o = oyjlOptions_GetOptionL( opts, option, 0 );
    char next_delimiter = g->mandatory[m_index[i]];
    if(next_delimiter != '|' && !o)
    {
      fprintf(stdout, "%s %s: option not declared: %s\n", oyjlBT(0), g->name?g->name:"---", option);
      if(!getenv("OYJL_NO_EXIT")) exit(1);
    }
    if(strcmp(option, "@") == 0)
      oyjlStringAdd( &text, malloc, free, " %s%s",
          o->value_name?o->value_name:"...",
          o->value_name && o->flags & OYJL_OPTION_FLAG_REPETITION ? "..." : "" );
  }
  oyjlStringListRelease( &m_list, m, free );
  oyjlStringListRelease( &on_list, on, free );
  free( m_index ); m_index = NULL;
  free( on_index ); on_index = NULL;
  return text;
}

/* @return  a options index in the list of a groups mandatory options
 * - >= 0 - if found
 * - -1 - if not found
 */
int oyjlOptionMandatoryIndex_         ( oyjlOption_s      * opt,
                                       oyjlOptionGroup_s * g )
{
  int found = -1;
  const char * mandatory = g->mandatory,
             * m;
  int n = 0, i;
  char ** list = oyjlStringSplit2( mandatory, "|,", &n, NULL, malloc );
  for(i = 0; i < n; ++i)
  {
    m = list[i];
    if( (opt->o && strcmp(m, opt->o) == 0) ||
        (opt->option && strcmp(m, opt->option) == 0) )
      found = i;
  }
  oyjlStringListRelease( &list, n, free );
  return found;
}

oyjlOptionChoice_s * oyjlOption_EnrichInbuildFunc_( oyjlOption_s * o, int * selected OYJL_UNUSED, oyjlOptions_s * opts )
{
  /* enrich inbuild variables */
  oyjlOptionChoice_s * h_choices = NULL;

  if(o && strcmp(o->o, "h") == 0)
  {
    int ng = oyjlOptions_CountGroups(opts),
        pos = 0, i;

    h_choices = (oyjlOptionChoice_s*) calloc( sizeof(oyjlOptionChoice_s), ng + 3 );

    h_choices[pos].nick = "1";
    h_choices[pos].name = _("Full Help");
    h_choices[pos].description = _("Print help for all groups");
    h_choices[pos].help = "";
    ++pos;
    h_choices[pos].nick = "synopsis";
    h_choices[pos].name = _("Synopsis");
    h_choices[pos].description = _("List groups");
    h_choices[pos].help = _("Show all groups including syntax");
    ++pos;
    for(i = 0; i < ng; ++i)
    {
      oyjlOptionGroup_s * g = &opts->groups[i];
      h_choices[pos].nick = (char*)g->name;
      h_choices[pos].name = (char*)g->description;
      h_choices[pos].description = (char*)g->help;
      ++pos;
    }

    if(o->value_name == NULL)
    {
      o->value_name = "synopsis|...";
      if(o->name == NULL)
      {
        o->name = _("Help");
        if(o->description == NULL)
        {
          o->description = _("Print help text");
          if(o->help == NULL)
            o->help = _("Show usage information and hints for the tool.");
        }
      }
    }
  }

  return h_choices;
}

static oyjlOptionChoice_s ** oyjl_get_choices_list_ = NULL;
static int * oyjl_get_choices_list_selected_;
oyjlOptionChoice_s * oyjlOption_GetChoices_ (
                                       oyjlOption_s      * o,
                                       int               * selected,
                                       oyjlOptions_s     * opts )
{
  int nopts = oyjlOptions_Count( opts ), pos = -1, i;
  if(!o) return NULL;

  for(i = 0; i < nopts; ++i)
    if(o == &opts->array[i])
      pos = i;

  if(!oyjl_get_choices_list_)
  {
    int i;
    oyjl_get_choices_list_selected_ = calloc( sizeof(int), nopts + 1 );
    for(i = 0; i < nopts; ++i) oyjl_get_choices_list_selected_[i] = -1;
    oyjl_get_choices_list_ = calloc( sizeof(oyjlOptionChoice_s*), nopts + 1 );
  }

  if( !oyjl_get_choices_list_[pos] ||
      (selected && oyjl_get_choices_list_selected_[pos] == -1) )
  {
    if(!o->values.getChoices)
      o->values.getChoices = oyjlOption_EnrichInbuildFunc_;
    oyjl_get_choices_list_[pos] = o->values.getChoices(o, selected ? &oyjl_get_choices_list_selected_[pos] : selected, opts );
  }

  if(selected)
    *selected = oyjl_get_choices_list_selected_[pos];
  return oyjl_get_choices_list_[pos];
}

#define OYJL_HELP_SUBSECTION "  "
#define OYJL_HELP_COMMAND    "    "
#define OYJL_HELP_OPTION     "      "
#define OYJL_HELP_ARG        "        "
#define OYJL_HELP_HELP       "          "
#include <stdarg.h> /* va_list */
FILE * oyjl_help_zout = NULL;
/** @brief    Print help text to stderr
 *  @memberof oyjlOptions_s
 *
 *  @param   opts                      options to print
 *  @param   ui                        more info for e.g. from the documentation section for the description block; optional
 *  @param   verbose                   gives debug output
 *                                     - -1 : print help only for detected group
 *                                     - -2 : print only Synopsis 
 *  @param   motto_format              prints a customised intoduction line
 *
 *  @version Oyjl: 1.0.0
 *  @date    2020/04/05
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
void  oyjlOptions_PrintHelp          ( oyjlOptions_s     * opts,
                                       oyjlUi_s          * ui,
                                       int                 verbose,
                                       const char        * motto_format,
                                                           ... )
{
  int i,ng;
  va_list list;
  oyjlUiHeaderSection_s * section = NULL;
  oyjlOptsPrivate_s * results = ui->opts->private_data;

  if(oyjl_help_zout == stdout || oyjl_help_zout == stderr || oyjl_help_zout == NULL)
    oyjl_help_zout = verbose >= -2 ? stdout : stderr;

  if(verbose >= 0)
    fprintf( oyjl_help_zout, "\n");
  if(verbose > 0)
  {
    for(i = 0; i < opts->argc; ++i)
      fprintf( oyjl_help_zout, "\'%s\' ", oyjlTermColor( oyjlITALIC, opts->argv[i] ));
    fprintf( oyjl_help_zout, "\n");
  }

  if(!(verbose == -1 && results && results->group > -1) && verbose >= 0)
  {
    if(!motto_format)
    {
      oyjlUiHeaderSection_s * version = oyjlUi_GetHeaderSection( ui,
                                                                 "version" );
      fprintf( oyjl_help_zout, "%s v%s - %s", oyjlTermColor( oyjlBOLD, opts->argv[0] ),
                              version && version->name ? version->name : "",
                              ui->description ? ui->description : ui->name ? ui->name : "" );
      if(version && version->name && version->description && *oyjl_debug)
        fprintf( oyjl_help_zout, "\n  %s", version->description );
    }
    else
    {
      va_start( list, motto_format );
      vfprintf( oyjl_help_zout, motto_format, list );
      va_end  ( list );
    }
    fprintf( oyjl_help_zout, "\n");
  }

  ng = oyjlOptions_CountGroups(opts);
  if(!ng) return;

  if( ui && (section = oyjlUi_GetHeaderSection(ui, "documentation")) != NULL &&
      section->description &&
      !(verbose == -1 && results && results->group > -1) &&
      verbose >= 0
    )
    fprintf( oyjl_help_zout, "\n%s:\n" OYJL_HELP_SUBSECTION "%s\n", oyjlTermColor(oyjlBOLD,_("Description")), section->description );

  if(verbose != -1)
  {
    fprintf( oyjl_help_zout, "\n%s:\n", oyjlTermColor(oyjlBOLD,_("Synopsis")) );
    for(i = 0 ; i < ng; ++i)
    {
      oyjlOptionGroup_s * g = &opts->groups[i];
      char * t = oyjlOptions_PrintHelpSynopsis_( opts, g, oyjlOPTIONSTYLE_ONELETTER );
      fprintf( oyjl_help_zout, OYJL_HELP_SUBSECTION "%s\n", t );
      free(t);
    }
  }
  if(verbose == -2)
  {
    fprintf( oyjl_help_zout, "\n" );
    return;
  }

  fprintf( oyjl_help_zout, "\n%s:\n", oyjlTermColor(oyjlBOLD,_("Usage"))  );
  for(i = (verbose == -1 && results && results->group > -1) ? results->group : 0 ; i < ng; ++i)
  {
    oyjlOptionGroup_s * g = &opts->groups[i];
    int d = 0,
        j;
    char ** d_list = oyjlStringSplit2( g->detail, "|,", &d, NULL, malloc );
    fprintf( oyjl_help_zout, OYJL_HELP_SUBSECTION "%s\n", g->description?oyjlTermColor(oyjlUNDERLINE,g->description):"" );
    if(g->mandatory && g->mandatory[0])
    {
      char * t = oyjlOptions_PrintHelpSynopsis_( opts, g, oyjlOPTIONSTYLE_ONELETTER );
      fprintf( oyjl_help_zout, OYJL_HELP_COMMAND "%s\n", t );
      free(t);
    }
    if(g->help)
    {
      fprintf( oyjl_help_zout, OYJL_HELP_COMMAND "%s\n", g->help );
    }
    fprintf( oyjl_help_zout, "\n" );
    for(j = 0; j < d; ++j)
    {
      const char * option = d_list[j];
      oyjlOption_s * o = oyjlOptions_GetOptionL( opts, option, 0 );
      int mandatory_index = oyjlOptionMandatoryIndex_( o, g );
      int style = oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING;
      if(mandatory_index == 0)
        style |= g->flags;
      if(!o)
      {
        fprintf(oyjl_help_zout, "%s %s: option not declared: %s\n", oyjlBT(0), g->name?g->name:"---", &g->detail[j]);
        if(!getenv("OYJL_NO_EXIT")) exit(1);
      }
      switch(o->value_type)
      {
        case oyjlOPTIONTYPE_CHOICE:
          {
            int n = 0,l;
            if(o->value_name)
            {
              char * t = oyjlOption_PrintArg(o, style);
              fprintf( oyjl_help_zout, OYJL_HELP_OPTION );
              fprintf( oyjl_help_zout, "%s", t );
              fprintf( oyjl_help_zout, "\t%s\n", o->description ? o->description:"" );
              if(o->help)
                fprintf(oyjl_help_zout,OYJL_HELP_HELP "%s\n", o->help );
              free(t);
            }
            if(o->flags & OYJL_OPTION_FLAG_EDITABLE)
              break;
            while(o->values.choices.list && o->values.choices.list[n].nick && o->values.choices.list[n].nick[0] != '\000')
              ++n;
            for(l = 0; l < n; ++l)
              fprintf( oyjl_help_zout, OYJL_HELP_ARG "  -%s %s\t\t# %s%s%s%s%s\n",
                  o->o,
                  o->values.choices.list[l].nick,
                  o->values.choices.list[l].name && o->values.choices.list[l].nick[0] ? o->values.choices.list[l].name : o->values.choices.list[l].description,
                  o->values.choices.list[l].description&&o->values.choices.list[l].description[0]?" : ":"",
                  o->values.choices.list[l].description?o->values.choices.list[l].description:"",
                  o->values.choices.list[l].help&&o->values.choices.list[l].help[0]?" - ":"",
                  o->values.choices.list[l].help?o->values.choices.list[l].help:"" );
          }
          break;
        case oyjlOPTIONTYPE_FUNCTION:
          {
            int n = 0,l;
            oyjlOptionChoice_s * list;
            if(o->value_name)
            {
              char * t = oyjlOption_PrintArg(o, style);
              fprintf( oyjl_help_zout, OYJL_HELP_OPTION );
              fprintf( oyjl_help_zout, "%s", t );
              fprintf( oyjl_help_zout, "\t%s%s%s\n", o->description ? o->description:"", o->help?": ":"", o->help?o->help :"" );
              free(t);
            }
            if(o->flags & OYJL_OPTION_FLAG_EDITABLE)
              break;
            list = oyjlOption_GetChoices_(o, NULL, opts );
            if(list)
              while(list[n].nick && list[n].nick[0] != '\000')
                ++n;
            for(l = 0; l < n; ++l)
              fprintf( oyjl_help_zout, OYJL_HELP_ARG "-%s %s\t\t# %s\n", o->o, list[l].nick, list[l].name && list[l].nick[0] ? list[l].name : list[l].description );
            /* not possible, as the result of oyjlOption_GetChoices_() is cached - oyjlOptionChoice_Release( &list ); */
          }
          break;
        case oyjlOPTIONTYPE_DOUBLE:
          {
            char * t = oyjlOption_PrintArg(o, style);
            
            fprintf( oyjl_help_zout, OYJL_HELP_OPTION );
            fprintf( oyjl_help_zout, "%s", t );
            { char * desc = oyjlOption_PrintArgDouble_( o, oyjlDESCRIPTION | oyjlHELP ); fprintf( oyjl_help_zout, "\t%s\n", desc ); free(desc); }
            free(t);
          }
          break;
        case oyjlOPTIONTYPE_NONE:
          {
            char * t = oyjlOption_PrintArg(o, style);
            fprintf( oyjl_help_zout, OYJL_HELP_OPTION );
            fprintf( oyjl_help_zout, "%s", t );
            fprintf( oyjl_help_zout, "\t%s\n", o->description ? o->description:"" );
            if(o->help)
              fprintf(oyjl_help_zout,OYJL_HELP_HELP "%s\n", o->help );
            free(t);
          }
        break;
        case oyjlOPTIONTYPE_START: break;
        case oyjlOPTIONTYPE_END: break;
      }
    }
    oyjlStringListRelease( &d_list, d, free );
    if(d) fprintf( oyjl_help_zout, "\n" );
    if(verbose == -1 && results && results->group > -1) break;
  }
  fprintf( oyjl_help_zout, "\n" );
}

/** @brief    Allocate a new options structure
 *  @memberof oyjlOptions_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/03/24
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
oyjlOptions_s * oyjlOptions_New      ( int                 argc,
                                       const char       ** argv )
{
  oyjlOptions_s * opts = (oyjlOptions_s*) calloc( sizeof(oyjlOptions_s), 1 );
  oyjlOptsPrivate_s * results;;
  memcpy( opts->type, "oiws", 4 );

  opts->argc = argc;
  opts->argv = argv;

  results = (oyjlOptsPrivate_s*) calloc( sizeof(oyjlOptsPrivate_s), 1 );
  results->memory_allocation = oyjlMEMORY_ALLOCATION_OPTIONS;
  opts->private_data = results;

  return opts;
}

/** @brief    Allocate a new ui structure
 *  @memberof oyjlUi_s
 *
 *  The oyjlUi_s contains already options in the opts member.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/03/24
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
oyjlUi_s* oyjlUi_New                 ( int                 argc,
                                       const char       ** argv )
{
  oyjlUi_s * ui = calloc( sizeof(oyjlUi_s), 1 );
  memcpy( ui->type, "oiui", 4 );
  ui->opts = oyjlOptions_New( argc, argv );
  return ui;
}

/** @brief    Copy structure
 *  @memberof oyjlUi_s
 *
 *  The oyjlUi_s string members are mostly references.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2020/07/31
 *  @since   2020/07/31 (Oyjl: 1.0.0)
 */
oyjlUi_s *         oyjlUi_Copy       ( oyjlUi_s          * src )
{
  oyjlUi_s * ui = NULL;
  if(!src) return ui;
  int size;

  if( *(oyjlOBJECT_e*)src != oyjlOBJECT_UI)
  {
    char * a = (char*)src;
    char type[5] = {a[0],a[1],a[2],a[3],0};
    fprintf(stderr, "Unexpected object: \"%s\"(expected: \"oyjlUi_s\")\n", type );
    return ui;
  }

  size = sizeof(oyjlUi_s);
  ui = (oyjlUi_s*)oyjlStringAppendN( NULL, (const char*)src, size, malloc );
  size = sizeof(oyjlOptions_s);
  ui->opts = (oyjlOptions_s*)oyjlStringAppendN( NULL, (const char*)src->opts, size, malloc );
  if(src->opts->private_data)
  {
    oyjlOptsPrivate_s * results = src->opts->private_data;
    if(results)
    {
      oyjlOptsPrivate_s * results_dst = (oyjlOptsPrivate_s*) calloc( 1, sizeof(oyjlOptsPrivate_s) );
      results_dst->options = oyjlStringListCatList( (const char**)results->options, results->count+1, NULL,0, &results_dst->count, malloc );
      /* values are only references */
      if(results->values)
      {
        int n = 0;
        while(results->values[n]) ++n;
        results_dst->values = (const char**)oyjlStringAppendN( NULL, (const char*)results->values, n+1, malloc );;
      }
      results_dst->group = results->group;
      results_dst->memory_allocation = oyjlMEMORY_ALLOCATION_OPTIONS;
      ui->opts->private_data = results_dst;
    }
  }

  return ui;
}

static oyjlOPTIONSTATE_e oyjlUi_Check_(oyjlUi_s          * ui,
                                       int                 flags OYJL_UNUSED )
{
  oyjlOPTIONSTATE_e status = oyjlOPTION_NONE;
  int i,ng;
  oyjlOptions_s * opts;
 
  if(!ui) return status;
  opts = ui->opts;

  ng = oyjlOptions_CountGroups(opts);
  if(!ng)
  {
    fprintf(stderr, "no ui::opts::groups\n");
    status = oyjlOPTION_MISSING_VALUE;
  }

  if(!ui->nick || !ui->nick[0])
  {
    fprintf(stderr, "no ui::nick\n");
    status = oyjlOPTION_MISSING_VALUE;
  }

  if(!ui->name || !ui->name[0])
  {
    fprintf(stderr, "no ui::name\n");
    status = oyjlOPTION_MISSING_VALUE;
  }

  for(i = 0; i < ng; ++i)
  {
    oyjlOptionGroup_s * g = &opts->groups[i];
    int * d_index = NULL, d = 0;
    char ** d_list = oyjlStringSplit2( g->detail, "|,", &d, &d_index, malloc );
    int j;
    if(g->mandatory && g->mandatory[0])
    {
      int n = 0;
      char ** list = oyjlStringSplit2( g->mandatory, "|,", &n, NULL, malloc );
      for( j = 0; j  < n; ++j )
      {
        const char * option = list[j];
        if( !g->detail ||
            (!strstr(g->detail, option) &&
             strcmp(option, "|") &&
             strcmp(option, "#")) )
        {
          fprintf(stderr, "\"%s\" not found in group->details\n", option );
          status = oyjlOPTION_MISSING_VALUE;
        }
      }
      oyjlStringListRelease( &list, n, free );
    }
    for(j = 0; j < d; ++j)
    {
      const char * option = d_list[j];
      oyjlOption_s * o = oyjlOptions_GetOptionL( opts, option, 0 );
      if(!o)
      {
        fprintf(stdout, "%s %s: option not declared: %s\n", oyjlBT(0), g->name?g->name:"---", option);
        if(!getenv("OYJL_NO_EXIT")) exit(1);
      }
      switch(o->value_type)
      {
        case oyjlOPTIONTYPE_CHOICE:
          {
            int n = 0;
            while(o->values.choices.list && o->values.choices.list[n].nick && o->values.choices.list[n].nick[0] != '\000')
              ++n;
            if(!o->value_name)
            {
              char * t = oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING);
              fputs( oyjlTermColor(oyjlRED,_("Program Error:")), stderr ); fputs( " ", stderr );
              fprintf( stderr, "%s%s (%s)\n", oyjlBT(0), _("This option needs oyjlOption_s::value_name defined"), t );
              if(!getenv("OYJL_NO_EXIT")) exit(1);
            }
            if( !n && !(o->flags & OYJL_OPTION_FLAG_EDITABLE) &&
                strcmp(o->o, "h") != 0 && strcmp(o->o, "X") != 0 && strcmp(o->o, "R") != 0 )
            {
              fputs( oyjlTermColor(oyjlRED,_("Usage Error:")), stderr ); fputs( " ", stderr );
              fprintf( stderr, "%s \'%s\' %s\n", _("Option not supported"), oyjlTermColor(oyjlBOLD,o->o), _("needs OYJL_OPTION_FLAG_EDITABLE or choices") );
              status = oyjlOPTION_NOT_SUPPORTED;
            }
          }
          break;
        case oyjlOPTIONTYPE_FUNCTION:
          break;
        case oyjlOPTIONTYPE_DOUBLE:
          break;
        case oyjlOPTIONTYPE_NONE:
        break;
        case oyjlOPTIONTYPE_START: break;
        case oyjlOPTIONTYPE_END: break;
      }
    }
    oyjlStringListRelease( &d_list, d, free );
    free( d_index );
  }

  return status;
}

static const char * oyjlOPTIONSTATE_eToString_( oyjlOPTIONSTATE_e i )
{
  switch(i)
  {
    case oyjlOPTION_NONE: return "oyjlOPTION_NONE";
    case oyjlOPTION_USER_CHANGED: return "oyjlOPTION_USER_CHANGED";
    case oyjlOPTION_MISSING_VALUE: return "oyjlOPTION_MISSING_VALUE";
    case oyjlOPTION_UNEXPECTED_VALUE: return "oyjlOPTION_UNEXPECTED_VALUE";
    case oyjlOPTION_NOT_SUPPORTED: return "oyjlOPTION_NOT_SUPPORTED";
    case oyjlOPTION_DOUBLE_OCCURENCE: return "oyjlOPTION_DOUBLE_OCCURENCE";
    case oyjlOPTIONS_MISSING: return "oyjlOPTIONS_MISSING";
    case oyjlOPTION_NO_GROUP_FOUND: return "oyjlOPTION_NO_GROUP_FOUND";
    case oyjlOPTION_SUBCOMMAND: return "oyjlOPTION_SUBCOMMAND";
    case oyjlOPTION_NOT_ALLOWED_AS_SUBCOMMAND: return "oyjlOPTION_NOT_ALLOWED_AS_SUBCOMMAND";
  }
  return "";
}
/** @brief    Create a new UI structure from options
 *  @memberof oyjlUi_s
 *
 *  This is a high level convinience function.
 *  The returned oyjlUi_s is a comlete description of the UI and can be
 *  used instantly. The options are parsed, errors are printed, help text
 *  is printed for the boolean -h/--help option. Boolean -v/--verbose
 *  is handled too. The results are set to the declared variables. 
 *  The app_type defaults to "tool", but it can be replaced if needed.
 *
 *  @code
  oyjlUi_s * ui = oyjlUi_FromOptions ( "myCl",
                                       _("My Command"),
                                       _("My Command line tool from Me"),
                                       "my_logo",
                                       info, opts, NULL )
    @endcode
 *
 *  @param[in]     nick                four byte string; e.g. "myCl"
 *  @param[in]     name                short name of the tool; i18n;
 *                 e.g. _("My Command")
 *  @param[in]     description         compact sentence starting with full name; i18n;
 *                 e.g. _("My Command line tool from Me")
 *  @param[in]     logo                icon name; This variable must contain
 *                 the file name only, without ending. The icon needs
 *                 to be installed in typical icon search path and will be
 *                 detected there. e.g. "my_logo" points to "my_logo.{png|svg}"
 *  @param[in]     info                general information for rich UI's and
 *                                     for help text
 *  @param[in,out] opts                The main option declaration, with
 *                 syntax declaration and variable passing for setting results.
 *                 The option grouping declares
 *                 dependencies of options and provides a UI layout.
 *  @param[in,out] state               inform about processing
 *                                     - ::oyjlUI_STATE_HELP : help was detected, printed and oyjlUi_s was released
 *                                     - ::oyjlUI_STATE_EXPORT : export of json, man or markdown was detected, printed and oyjlUi_s was released
 *                                     - ::oyjlUI_STATE_VERBOSE : verbose was detected
 *                                     - ::oyjlUI_STATE_OPTION+ : error occured in option parser, message printed, ::oyjlOPTIONSTATE_e is placed in >> ::oyjlUI_STATE_OPTION and oyjlUi_s was released
 *                                     - ::oyjlUI_STATE_NO_CHECKS : skip any checks during creation; Only useful if part of the passed in data is omitted or needs to be passed through.
 *  @return                            UI object for later use
 *
 *  @version Oyjl: 1.0.0
 *  @date    2020/10/13
 *  @since   2020/10/13 (Oyjl: 1.0.0)
 */
oyjlUi_s *         oyjlUi_FromOptions( const char        * nick,
                                       const char        * name,
                                       const char        * description,
                                       const char        * logo,
                                       oyjlUiHeaderSection_s * info,
                                       oyjlOptions_s     * opts,
                                       int               * status )
{
  int help = 0, verbose = 0, version = 0, i,ng,nopts, * rank_list = 0, max = -1, pass_group = 0;
  const char * export = NULL;
  const char * help_string = NULL;
  oyjlOption_s * v, * X, * V, * h = NULL;
  oyjlOptionGroup_s * g = NULL;
  oyjlOPTIONSTATE_e opt_state = oyjlOPTION_NONE;
  oyjlOptsPrivate_s * results;
  int flags = 0, optionless = 0;//, anonymous = 0;
  char * t;

  int use_gettext = 0;
#ifdef OYJL_USE_GETTEXT
  use_gettext = 1;
#endif
  oyjlInitLanguageDebug( "Oyjl", "OYJL_DEBUG", oyjl_debug, use_gettext, "OYJL_LOCALEDIR", OYJL_LOCALEDIR, OYJL_DOMAIN, NULL );

  if(status)
    flags = *status;

  /* allocate options structure */
  oyjlUi_s * ui = calloc( sizeof(oyjlUi_s), 1 ); /* argc+argv are required for parsing the command line options */
  memcpy( ui->type, "oiui", 4 );
  ui->opts = opts;
  /* tell about the tool */
  if(!(flags & oyjlUI_STATE_NO_CHECKS))
    ui->app_type = "tool";
  ui->nick = nick;
  ui->name = name;
  ui->description = description;
  ui->logo = logo;

  /* Select from *version*, *manufacturer*, *copyright*, *license*, *url*,
   * *support*, *download*, *sources*, *oyjl_module_author* and
   * *documentation* what you see fit. Add new ones as needed. */
  ui->sections = info;

  /* get results and check syntax ... */
  results = ui->opts->private_data;
  if(!results || !results->values)
    opt_state = oyjlOptions_Parse( ui->opts );
  if(opt_state == oyjlOPTION_NOT_SUPPORTED)
    goto FromOptions_done;
  results = ui->opts->private_data;

  V = oyjlOptions_GetOption( ui->opts, "V" );
  if(V && V->variable_type == oyjlINT && V->variable.i)
    version = *V->variable.i;
  X = oyjlOptions_GetOption( ui->opts, "X" );
  if(X && X->variable_type == oyjlSTRING && X->variable.s)
    export = *X->variable.s;
  help = oyjlOptions_IsHelp_( ui->opts );;
  h = oyjlOptions_GetOption( ui->opts, "h" );
  v = oyjlOptions_GetOption( ui->opts, "v" );
  if(v && v->variable_type == oyjlINT && v->variable.i)
  {
    verbose = *v->variable.i;
    if(status && verbose)
      *status |= oyjlUI_STATE_VERBOSE;
    if(verbose)
    {
      flags |= oyjlUI_STATE_VERBOSE;
      fprintf( stderr, "verbose\n" );
    }
  }
  /* enrich inbuild */
  oyjlOptions_GetOption( ui->opts, "R" );

  /* detect valid group match(es) and report missing mandatory one */
  ng = oyjlOptions_CountGroups(ui->opts);
  if(ng)
    oyjlAllocHelper_m( rank_list, int, ng, malloc, return NULL );

  for(i = 0; i < ng; ++i)
  {
    int mgoup_index = 0;
    oyjlOption_s * o;
    const char * option;
    char ** list = NULL;
    int n = 0, j;
    g = &ui->opts->groups[i];
    /* test mandatory options for correct numbers */
    if(g->mandatory && g->mandatory[0])
    {
      const char * mandatory = g->mandatory;
      int k, found = 0;

      list = oyjlStringSplit2( mandatory, "|,", &n, NULL, malloc );

      if( strchr(mandatory,'#') != NULL &&
          results ? (results->count == 0) : 0 )
        optionless = 1;

      ++mgoup_index;

      for( j = 0; j  < n; ++j )
      {
        const char * moption = list[j];
        o = oyjlOptions_GetOptionL( ui->opts, moption, 0 );
        if(!o)
        {
          fputs( oyjlTermColor(oyjlRED,_("Program Error:")), stderr ); fputs( " ", stderr );
          fprintf( stderr, "%s (%s)\n", _("This option is not defined"), moption );
        }
        for( k = 0; k  < (results?results->count:0); ++k )
        {
          const char * roption = results->options[k];
          if(strcmp(roption, "h") == 0)
          {
            const char * v = results->values[k];
            o = NULL;
            if(strcmp(v, "1") != 0) /* skip --help no arg results */
              o = oyjlOptions_GetOptionL( ui->opts, v, 0 );
            if(o)
              roption = o->o?o->o:o->option;
            if(g->name && strcmp(v,g->name) == 0)
              ++found;
          }
          if( strcmp(moption, roption) == 0 &&
              strcmp(roption, "h") != 0 )
            ++found;
          if(i == mgoup_index && j == 0)
          {
            o = oyjlOptions_GetOptionL( ui->opts, roption, 0 );
            if(o && o->flags & OYJL_OPTION_FLAG_MAINTENANCE)
              ++pass_group; 
          }
        }
      }
      oyjlStringListRelease( &list, n, free );
      rank_list[i] = found;
      if(found && max < found)
        max = found;
    } else
    if(help)
    {
      int k, found = 0;
      for( k = 0; k  < (results?results->count:0); ++k )
      {
        const char * roption = results->options[k];
        if(strcmp(roption, "h") == 0)
        {
          const char * v = results->values[k];
          if(g->name && strcmp(v,g->name) == 0)
            ++found;
        }
      }
      rank_list[i] = found;
      if(found && max < found)
        max = found;
      if(h && h->variable_type == oyjlSTRING && h->variable.s)
        help_string = *h->variable.s;
      if(help_string && strcmp(help_string, "oyjl-list") == 0 && h->value_type == oyjlOPTIONTYPE_FUNCTION)
      {
        int selected = 0;
        oyjlOptionChoice_s * choices = oyjlOption_EnrichInbuildFunc_( h, &selected, opts );
        int n = oyjlOptionChoice_Count( choices );
        for(k = 0; k < n; ++k)
          fprintf( stdout, "%s\n", choices[k].nick );
        oyjlUi_ReleaseArgs( &ui);
        if(status)
          *status |= oyjlUI_STATE_EXPORT;
        return NULL;
      }
    }

    list = oyjlStringSplit2( g->optional, "|,", &n, NULL, malloc );
    for( j = 0; j  < n; ++j )
    {
      option = list[j];
      o = oyjlOptions_GetOptionL( ui->opts, option, 0 );
      if(!o)
      {
        fputs( oyjlTermColor(oyjlRED,_("Program Error:")), stderr ); fputs( " ", stderr );
        fprintf( stderr, "%s (%s)\n", _("This option is not defined"), option );
      }
    }
    oyjlStringListRelease( &list, n, free );

    list = oyjlStringSplit2( g->detail, "|,", &n, NULL, malloc );
    for( j = 0; j  < n; ++j )
    {
      option = list[j];
      o = oyjlOptions_GetOptionL( ui->opts, option, 0 );
      if(!o)
      {
        fputs( oyjlTermColor(oyjlRED,_("Program Error:")), stderr ); fputs( " ", stderr );
        fprintf( stderr, "%s (%s)\n", _("This option is not defined"), option );
      }
    }
    oyjlStringListRelease( &list, n, free );
  }

  if(results && results->count == 0)
  {
    oyjlOption_s * o = oyjlOptions_GetOption( ui->opts, "#" );
    if(o && o->flags & OYJL_OPTION_FLAG_MAINTENANCE)
      optionless = 1;
  }

  if(max > -1)
  {
    for(i = 0; i < ng; ++i)
      if(rank_list[i] == max)
      {
        g = &ui->opts->groups[i];
        if(results)
          results->group = i;
        break;
      }
  }
  else if(!optionless && opt_state != oyjlOPTION_NOT_SUPPORTED && !help && !pass_group && !version && !export)
  {
    if(opt_state == oyjlOPTION_NONE || opt_state)
      oyjlOptions_Print_( ui->opts, 0 );
    fputs( oyjlTermColor(oyjlRED,_("Usage Error:")), stderr ); fputs( " ", stderr );
    fputs( _("Missing mandatory option. No usage mode in synopsis lines found."), stderr );
    fputs( "\n", stderr );

    if(opt_state == oyjlOPTION_NONE)
      opt_state = oyjlOPTION_NO_GROUP_FOUND;

    if(opt_state != oyjlOPTIONS_MISSING)
      oyjlOptions_PrintHelp( ui->opts, ui, -2, NULL );
  }
  free(rank_list);
  if(results && results->group >= 0)
  {
    i = results->group;
    g = &ui->opts->groups[i];
    if(g->flags & OYJL_GROUP_FLAG_SUBCOMMAND)
    {
      int li_n = 0, i;
      char ** li = oyjlStringSplit2( g->mandatory, "|,", &li_n, NULL, 0 );
      const char * first_opt = results->options[0];
      int found_mandatory_at_first_pos = 0;
      for( i = 0; i < li_n; ++i )
        if(strcmp(first_opt, li[i]) == 0)
          ++found_mandatory_at_first_pos;
      opt_state = oyjlOPTION_SUBCOMMAND;

      if(!found_mandatory_at_first_pos)
      {
        oyjlOptions_Print_( ui->opts, 1 );
        fputs( oyjlTermColor(oyjlRED,_("Usage Error:")), stderr ); fputs( " ", stderr );
        fprintf(stderr, "%s:", _("A mandatory sub command option needs to be placed first") );
        for( i = 0; i < li_n; ++i )
        {
          const char * option = li[i];
          oyjlOption_s * o = oyjlOptions_GetOptionL( ui->opts, option, 0 );
          char * t = oyjlOption_PrintArg( o, oyjlOPTIONSTYLE_STRING | OYJL_GROUP_FLAG_SUBCOMMAND );
          fprintf(stderr, " %s", oyjlTermColor(oyjlBOLD, t));
          free(t);
        }
        fprintf(stderr, "\n");
        if(status)
          *status |= oyjlOPTION_NOT_ALLOWED_AS_SUBCOMMAND;
        opt_state = oyjlOPTION_NOT_ALLOWED_AS_SUBCOMMAND;
      }
      oyjlStringListRelease( &li, li_n, 0 );
    }
  }

  if(opt_state != oyjlOPTION_NONE && *oyjl_debug)
  {
    fputs( oyjlOPTIONSTATE_eToString_( opt_state ), stderr );
    fputs( "\n", stderr );
  }

  if( opt_state == oyjlOPTIONS_MISSING ||
      (opt_state == oyjlOPTION_MISSING_VALUE && results && results->group >= 0) )
  {
    oyjlOptions_PrintHelp( ui->opts, ui, results && results->group >= 0?-1:-2, NULL );
    oyjlUi_ReleaseArgs( &ui);
    if(status)
      *status |= oyjlUI_STATE_HELP;
    return NULL;
  }
  if( ( opt_state == oyjlOPTION_NONE ||
        opt_state == oyjlOPTION_SUBCOMMAND ) &&
      !(flags & oyjlUI_STATE_NO_CHECKS)
    )
    opt_state = oyjlUi_Check_(ui, 0);

  if( help &&
      ( opt_state == oyjlOPTION_NONE ||
        opt_state == oyjlOPTION_MISSING_VALUE ||
        opt_state == oyjlOPTION_SUBCOMMAND ||
        opt_state == oyjlOPTION_NOT_ALLOWED_AS_SUBCOMMAND )
    )
  {
    oyjlOption_s * synopsis = oyjlOptions_GetOptionL( ui->opts, "synopsis", 0 );
    oyjlOptions_PrintHelp( ui->opts, ui, (results && results->group >= 0) ? -1 : (results && results->count >= 1 && strcasecmp(results->values[0],"synopsis") == 0 && synopsis) ? -2 : verbose, NULL );
    oyjlUi_ReleaseArgs( &ui);
    if(status)
      *status |= oyjlUI_STATE_HELP;
    return NULL;
  }

  if(version)
  {
    oyjlUiHeaderSection_s * version = oyjlUi_GetHeaderSection( ui, "version" );
    oyjlUiHeaderSection_s * author = oyjlUi_GetHeaderSection( ui, "manufacturer" );
    oyjlUiHeaderSection_s * copyright = oyjlUi_GetHeaderSection( ui, "copyright" );
    oyjlUiHeaderSection_s * license = oyjlUi_GetHeaderSection( ui, "license" );
    char * prog = NULL;
    char * v = version && version->name ? oyjlStringCopy( oyjlTermColor( oyjlITALIC, version->name ), NULL ) : NULL;

    if(!verbose && ui->opts->argv[0] && strchr(ui->opts->argv[0],'/'))
      prog = oyjlStringCopy( oyjlTermColor( oyjlBOLD, strrchr(ui->opts->argv[0],'/') + 1 ), NULL );
    else
      prog = oyjlStringCopy( oyjlTermColor( oyjlBOLD, ui->opts->argv[0] ), NULL );
    fprintf( stdout, "%s v%s%s%s%s - %s\n%s\n%s%s%s\n%s%s%s\n\n", prog,
                                      v ? v : "",
                                      version && version->description ? "(" : "",
                                      version && version->description ? version->description : "",
                                      version && version->description ? ")" : "",
                                      ui->description ? ui->description : ui->name ? ui->name : "",
                                      copyright && copyright->name ? copyright->name : "",
                                      license ? _("License"):"", license?":\t":"", license && license->name ? license->name : "",
                                      author ? _("Author"):"", author?": \t":"", author && author->name ? author->name : "" );
    oyjlUi_ReleaseArgs( &ui);
    free(prog);
    if(v) free(v);
    if(status)
      *status |= oyjlUI_STATE_HELP;
    return NULL;
  }

  if(export)
  {
    if(status)
      *status |= oyjlUI_STATE_EXPORT;
#if defined(OYJL_INTERNAL)
    if(strcmp(export, "json") == 0)
    {
      t = oyjlUi_ToJson( ui, flags );
      if(t) puts( t );
      oyjlUi_ReleaseArgs( &ui);
      return NULL;
    }
#endif
    if(strcmp(export, "man") == 0)
    {
      t = oyjlUi_ToMan( ui, flags );
      if(t) puts( t );
      oyjlUi_ReleaseArgs( &ui);
      return NULL;
    }
    if(strcmp(export, "markdown") == 0)
    {
      t = oyjlUi_ToMarkdown( ui, flags );
      if(t) puts( t );
      oyjlUi_ReleaseArgs( &ui);
      return NULL;
    }
#if defined(OYJL_INTERNAL)
    if(strcmp(export, "export") == 0)
    {
      puts( oyjlUi_ExportToJson( ui, flags ) );
      oyjlUi_ReleaseArgs( &ui);
      return NULL;
    }
#endif
  }

  nopts = oyjlOptions_Count( ui->opts );
  if(opt_state != oyjlOPTION_NOT_ALLOWED_AS_SUBCOMMAND)
  for(i = 0; i < nopts; ++i)
  {
    const char * value = NULL;
    oyjlOption_s * o = &ui->opts->array[i];
    oyjlOptions_GetResult( opts, o->o, &value, 0, 0 );
    if(value && strcmp(value, "oyjl-list") == 0 && o->value_type == oyjlOPTIONTYPE_FUNCTION)
    {
      int n = 0,l;
      oyjlOptionChoice_s * list;
      list = oyjlOption_GetChoices_(o, NULL, opts );
      if(list)
        while(list[n].nick && list[n].nick[0] != '\000')
          ++n;
      for(l = 0; l < n; ++l)
        fprintf( stdout, "%s\n", list[l].nick );
      /* not possible, as the result of oyjlOption_GetChoices_() is cached - oyjlOptionChoice_Release( &list ); */

      if(status)
        *status |= oyjlUI_STATE_EXPORT;
      oyjlUi_ReleaseArgs( &ui);
      return NULL;
    }
  }
  /* done with options handling */

  FromOptions_done:
  /* ... and report detected errors */
  if(!export && !version && opt_state != oyjlOPTION_NONE)
  {
    fputs( "\n", stderr );
    fputs( _("... try with --help|-h option for usage text. give up"), stderr );
    fputs( "\n", stderr );
    oyjlUi_ReleaseArgs( &ui);
    if(status)
      *status = opt_state << oyjlUI_STATE_OPTION;
    return NULL;
  }

  return ui;
}

/** @brief    Create a new UI structure
 *  @memberof oyjlUi_s
 *
 *  This is a high level convinience function.
 *  The returned oyjlUi_s is a comlete description of the UI and can be
 *  used instantly. The options are parsed, errors are printed, help text
 *  is printed for the boolean -h/--help option. Boolean -v/--verbose
 *  is handled too. The results are set to the declared variables. 
 *  The app_type defaults to "tool", but it can be replaced if needed.
 *
 *  @code
  oyjlUi_s * ui = oyjlUi_Create( argc, argv,
                                       "myCl",
                                       _("My Command"),
                                       _("My Command line tool from Me"),
                                       "my_logo",
                                       info, options, groups, NULL )
    @endcode
 *
 *  @param[in]     argc                number of command line arguments
 *  @param[in]     argv                command line args from C/C++ main()
 *  @param[in]     nick                four byte string; e.g. "myCl"
 *  @param[in]     name                short name of the tool; i18n;
 *                 e.g. _("My Command")
 *  @param[in]     description         compact sentence starting with full name; i18n;
 *                 e.g. _("My Command line tool from Me")
 *  @param[in]     logo                icon name; This variable must contain
 *                 the file name only, without ending. The icon needs
 *                 to be installed in typical icon search path and will be
 *                 detected there. e.g. "my_logo" points to "my_logo.{png|svg}"
 *  @param[in]     info                general information for rich UI's and
 *                                     for help text
 *  @param[in,out] options             the main option declaration, with
 *                 syntax declaration and variable passing for setting results
 *  @param[in]     groups              the option grouping declares
 *                 dependencies of options and provides a UI layout
 *  @param[in,out] state               inform about processing
 *                                     - ::oyjlUI_STATE_HELP : help was detected, printed and oyjlUi_s was released
 *                                     - ::oyjlUI_STATE_EXPORT : export of json, man or markdown was detected, printed and oyjlUi_s was released
 *                                     - ::oyjlUI_STATE_VERBOSE : verbose was detected
 *                                     - ::oyjlUI_STATE_OPTION+ : error occured in option parser, message printed, ::oyjlOPTIONSTATE_e is placed in >> ::oyjlUI_STATE_OPTION and oyjlUi_s was released
 *                                     - ::oyjlUI_STATE_NO_CHECKS : skip any checks during creation; Only useful if part of the passed in data is omitted or needs to be passed through.
 *  @return                            UI object for later use
 *
 *  @version Oyjl: 1.0.0
 *  @date    2020/04/05
 *  @since   2018/08/20 (OpenICC: 0.1.1)
 */
oyjlUi_s *  oyjlUi_Create            ( int                 argc,
                                       const char       ** argv,
                                       const char        * nick,
                                       const char        * name,
                                       const char        * description,
                                       const char        * logo,
                                       oyjlUiHeaderSection_s * info,
                                       oyjlOption_s      * options,
                                       oyjlOptionGroup_s * groups,
                                       int               * status )
{
  /* allocate options structure */
  oyjlOptions_s * opts = oyjlOptions_New( argc, argv ); /* argc+argv are required for parsing the command line options */
  opts->array = options;
  opts->groups = groups;
  oyjlUi_s * ui = oyjlUi_FromOptions( nick, name, description, logo, info, opts, status );

  return ui;
}

/** @brief    Release "oiui"
 *  @memberof oyjlUi_s
 *
 *  Release oyjlUi_s::opts, oyjlUi_s::private_data and oyjlUi_s.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2018/08/14
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
void           oyjlUi_ReleaseArgs ( oyjlUi_s      ** ui )
{
  int memory_allocation = 0;
  if(!ui || !*ui) return;
  if( *(oyjlOBJECT_e*)*ui != oyjlOBJECT_UI)
  {
    char * a = (char*)*ui;
    char type[5] = {a[0],a[1],a[2],a[3],0};
    fprintf(stderr, "Unexpected object: \"%s\"(expected: \"oyjlUi_s\")\n", type );
    return;
  }
  if((*ui)->opts->private_data)
  {
    oyjlOptsPrivate_s * results = (*ui)->opts->private_data;
    if(results)
    {
      memory_allocation = results->memory_allocation;
      if(results->memory_allocation)
      {
        int nopts = oyjlOptions_Count( (*ui)->opts ), i;
        if(memory_allocation & oyjlMEMORY_ALLOCATION_ARRAY)
        for(i = 0; i < nopts; ++i)
        {
          oyjlOption_s * opt = &(*ui)->opts->array[i];
          if(opt->value_type == oyjlOPTIONTYPE_CHOICE && opt->values.choices.list)
            free(opt->values.choices.list);
        }
        if((*ui)->sections && memory_allocation & oyjlMEMORY_ALLOCATION_SECTIONS)
          free((*ui)->sections);
        if((*ui)->opts->array && memory_allocation & oyjlMEMORY_ALLOCATION_ARRAY)
          free((*ui)->opts->array);
        if((*ui)->opts->groups && memory_allocation & oyjlMEMORY_ALLOCATION_GROUPS)
          free((*ui)->opts->groups);
      }
      oyjlStringListRelease( &results->options, results->count, free );
      results->count = 0;
      free(results->options);
      free(results->values);
      free((*ui)->opts->private_data);
    }
  }
  if((*ui)->opts && memory_allocation & oyjlMEMORY_ALLOCATION_OPTIONS) free((*ui)->opts);
  free((*ui));
  *ui = NULL;
}

/** @brief    Return the number of sections of type "oihs"
 *  @memberof oyjlUiHeaderSection_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2020/06/01
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
int     oyjlUiHeaderSection_Count    ( oyjlUiHeaderSection_s * sections )
{
  int n = 0;
  if(!sections) return 0;
  while( *(oyjlOBJECT_e*)&sections[n] /*"oihs"*/ == oyjlOBJECT_UI_HEADER_SECTION) ++n;
  return n;
}

/** @brief    Add new header section at end
 *  @memberof oyjlUiHeaderSection_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2020/07/16
 *  @since   2020/06/05 (Oyjl: 1.0.0)
 */
oyjlUiHeaderSection_s * oyjlUiHeaderSection_Append (
                                       oyjlUiHeaderSection_s * sections,
                                       const char        * nick,
                                       const char        * label,
                                       const char        * name,
                                       const char        * description )
{
  int n = oyjlUiHeaderSection_Count( sections );
  oyjlUiHeaderSection_s * info = (oyjlUiHeaderSection_s*) calloc(sizeof(oyjlUiHeaderSection_s), n+2);

  if(!info) return NULL;

  if(n)
    memcpy( info, sections, sizeof(oyjlUiHeaderSection_s) * n );

  sprintf( info[n].type, "%s", "oihs" );
  info[n].nick = nick;
  info[n].label = label;
  info[n].name = name;
  info[n].description = description;

  return info;
}

/** @brief    Return the section which was specified by its nick name
 *  @memberof oyjlUi_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2018/08/14
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
oyjlUiHeaderSection_s * oyjlUi_GetHeaderSection (
                                       oyjlUi_s          * ui,
                                       const char        * nick )
{
  oyjlUiHeaderSection_s * section = NULL;
  int i, count = oyjlUiHeaderSection_Count(ui->sections);
  for(i = 0; i < count; ++i)
    if( strcmp(ui->sections[i].nick, nick) == 0 )
      section = &ui->sections[i];
  return section;
}

char *       oyjlStringToUpper       ( const char        * t )
{
  char * text = oyjlStringCopy(t, malloc);
  int slen = strlen(t), i;
  for(i = 0; i < slen; ++i)
    text[i] = toupper(t[i]);
  return text;
}
char *       oyjlStringToLower       ( const char        * t )
{
  char * text = oyjlStringCopy(t, malloc);
  int slen = strlen(t), i;
  for(i = 0; i < slen; ++i)
    text[i] = tolower(t[i]);
  return text;
}


#define ADD_SECTION( sec, link, format, ... ) { \
  oyjlStringAdd( &text, malloc, free, "\n<h2>%s <a href=\"#toc\" name=\"%s\">&uarr;</a></h2>\n\n" format, sec, link, __VA_ARGS__ ); \
  oyjlStringListAddStaticString( sections, sn, sec, 0,0 ); \
  oyjlStringListAddStaticString( sections, sn, link, 0,0 ); }

static char * oyjlExtraManSection_   ( oyjlOptions_s     * opts,
                                       const char        * opt_name,
                                       int                 flags,
                                       char            *** sections,
                                       int               * sn )
{
  oyjlOption_s * o = oyjlOptions_GetOptionL( opts, opt_name, 0 );
  char * text = NULL;
  if(o)
  {
    int n = 0,l;
    if(o->value_type == oyjlOPTIONTYPE_CHOICE)
    {
      oyjlOptionChoice_s * list = o->values.choices.list;
      while(o->values.choices.list[n].nick && o->values.choices.list[n].nick[0] != '\000') ++n;
      if(n)
      {
        char * up = oyjlStringToUpper( &opt_name[4] );
        oyjlStringReplace( &up, "_", " ", malloc, free );
        const char * section = up;
        if(strcmp(section,"EXAMPLES") == 0)
          section = _("EXAMPLES");
        else if(strcmp(section,"EXIT-STATE") == 0)
          section = _("EXIT-STATE");
        else if(strcmp(section,"ENVIRONMENT VARIABLES") == 0)
          section = _("ENVIRONMENT VARIABLES");
        else if(strcmp(section,"HISTORY") == 0)
          section = _("HISTORY");
        else if(strcmp(section,"FILES") == 0)
          section = _("FILES");
        else if(strcmp(section,"SEE AS WELL") == 0)
          section = _("SEE AS WELL");
        else if(strcmp(section,"SEE ALSO") == 0)
          section = _("SEE ALSO");
        if(flags & oyjlOPTIONSTYLE_MARKDOWN)
        {
          char * low = oyjlStringToLower( &opt_name[4] );
          oyjlStringReplace( &low, "_", "", malloc, free );
          ADD_SECTION( _(section), low, "", "" )
          free(low);
        }
        else
          oyjlStringAdd( &text, malloc, free, ".SH %s\n", _(section) );
        for(l = 0; l < n; ++l)
        {
          if(flags & oyjlOPTIONSTYLE_MARKDOWN)
          {
            if( strcmp(up,"SEE AS WELL") == 0 ||
                strcmp(up,"SEE ALSO") == 0 )
            {
              int li_n = 0, i;
              char ** li = oyjlStringSplit2( list[l].nick, 0, &li_n, NULL, malloc );
              for(i = 0; i < li_n; ++i)
              {
                char * md = oyjlStringCopy( li[i], 0 );
                int len = strlen(md), is_man_page = 0;
                if(len > 3 && md[len-3] == '(' && md[len-1] == ')')
                  ++is_man_page;
                if(is_man_page)
                {
                  char * end = oyjlStringCopy( &md[len-3], malloc );
                  char * t;
                  md[len-3] = '\000';
                  t = oyjlStringCopy( md, 0 );
                  oyjlStringReplace( &md, "-", "", malloc, free );

                  oyjlStringAdd( &text, malloc, free, "&nbsp;&nbsp;[%s](%s.html)<a href=\"%s.md\">%s</a>", t, md, md, end );
                  free( t );
                  free( end );
                }
                else
                  oyjlStringAdd( &text, malloc, free, " %s", md );

                free( md );
              }
              oyjlStringAdd( &text, malloc, free, "\n\n" );
              oyjlStringListRelease( &li, li_n, free );
            }
            else
            oyjlStringAdd( &text, malloc, free, "#### %s\n", list[l].nick );
            if(list[l].name && list[l].name[0])
            {
              if(strlen(list[l].name) > 5 && memcmp(list[l].name,"http",4) == 0)
                oyjlStringAdd( &text, malloc, free, "&nbsp;&nbsp;<a href=\"%s\">%s</a>\n", list[l].name, list[l].name );
               else
                oyjlStringAdd( &text, malloc, free, "&nbsp;&nbsp;%s\n", list[l].name );
            }
            if(list[l].description && list[l].description[0])
            oyjlStringAdd( &text, malloc, free, "  <br />\n&nbsp;&nbsp;%s\n", list[l].description );
            if(list[l].help && list[l].help[0])
            oyjlStringAdd( &text, malloc, free, "  <br />\n&nbsp;&nbsp;%s\n", list[l].help );
          }
          else
          {
            oyjlStringAdd( &text, malloc, free, ".TP\n%s\n.br\n", list[l].nick );
            if(list[l].name && list[l].name[0])
            oyjlStringAdd( &text, malloc, free, "%s\n", list[l].name );
            if(list[l].description && list[l].description[0])
            oyjlStringAdd( &text, malloc, free, ".br\n%s\n", list[l].description );
            if(list[l].help && list[l].help[0])
            oyjlStringAdd( &text, malloc, free, ".br\n%s\n", list[l].help );
          }
        }
        free(up);
      }
    }
  }
  return text;
}

static char * oyjlExtraManSections_( oyjlOptions_s  * opts, int flags, char *** sections, int * sn )
{
  char * text = NULL;
  int nopts = oyjlOptions_Count( opts );
  int l;
  for(l = 0; l < nopts; ++l)
  {
    oyjlOption_s * o = &opts->array[l];
    const char * option = o->option;
    int olen = option ? strlen(option) : 0;
    if(olen > 7 && option[0] == 'm' && option[1] == 'a' && option[2] == 'n' && option[3] == '-')
    {
      char * tmp = oyjlExtraManSection_(opts, option, flags, sections, sn);
      if(tmp)
      {
        oyjlStringAddN( &text, tmp, strlen(tmp), malloc, free );
        free(tmp);
      }
    }
  }
  return text;
}

/** @brief    Return a MAN page from options
 *  @memberof oyjlUi_s
 *
 *  Some manual pages (MAN pages) might contain some additional sections.
 *  They are supported as options. To generate a custom MAN page section,
 *  add a blind option to your options list and set the oyjlOption_s::o
 *  char to something non interupting like, dot '.' or similar.
 *  The oyjlOption_s::option string
 *  contains "man-section_head", with "section_head" being adapted to your
 *  needs. The "man-" identifier part will be cut off and 
 *  "section_head" will become uppercase and underline '_' become empty
 *  space, e.g.: "SECTION HEAD".
 *  Use oyjlOption_s::value_type=oyjlOPTIONTYPE_CHOICE
 *  and place your string list into oyjlOptionChoice_s by filling it's
 *  members. Each choice is shown as own subsection.
 *  oyjlOptionChoice_s::nick is handled as a subsection headline and
 *  oyjlOptionChoice_s::name is shown as link or normal text.
 *  A "man-see_also" section oyjlOptionChoice_s::nick is scanned for
 *  MAN page cross references, e.g: "oyjl(1) oyjl(args(1)" and links are
 *  created appropriately.
 *  Translated section heads are "EXAMPLES, "SEE AS WELL", "HISTORY",
 *  "ENVIRONMENT VARIABLES", "EXIT-STATE" and "FILES".
 *
 *  @version Oyjl: 1.0.0
 *  @date    2020/04/15
 *  @since   2018/10/10 (OpenICC: 0.1.1)
 */
char *       oyjlUi_ToMan            ( oyjlUi_s          * ui,
                                       int                 flags OYJL_UNUSED )
{
  char * text = NULL, * tmp;
  const char * date = NULL,
             * desc = NULL,
             * mnft = NULL, * mnft_url = NULL,
             * copy = NULL, * lice = NULL, * lice_url = NULL,
             * bugs = NULL, * bugs_url = NULL,
             * vers = NULL;
  int i,n,ng;
  oyjlOptions_s * opts;
 
  if(!ui) return text;
  opts = ui->opts;

  n = oyjlUiHeaderSection_Count( ui->sections );
  for(i = 0; i < n; ++i)
  {
    oyjlUiHeaderSection_s * s = &ui->sections[i];
    if(strcmp(s->nick, "manufacturer") == 0) { mnft = s->name; mnft_url = s->description; }
    else if(strcmp(s->nick, "copyright") == 0) copy = s->name;
    else if(strcmp(s->nick, "license") == 0) { lice = s->name; lice_url = s->description; }
    else if(strcmp(s->nick, "url") == 0) continue;
    else if(strcmp(s->nick, "support") == 0) { bugs = s->name; bugs_url = s->description; }
    else if(strcmp(s->nick, "download") == 0) continue;
    else if(strcmp(s->nick, "sources") == 0) continue;
    else if(strcmp(s->nick, "development") == 0) continue;
    else if(strcmp(s->nick, "oyjl_module_author") == 0) continue;
    else if(strcmp(s->nick, "documentation") == 0) desc = s->description ? s->description : s->name;
    else if(strcmp(s->nick, "version") == 0) vers = s->name;
    else if(strcmp(s->nick, "date") == 0) date = s->description ? s->description : s->name;
  }

  ng = oyjlOptions_CountGroups(opts);
  if(!ng) return NULL;

  if(ui->app_type && ui->app_type[0])
  {
    int tool = strcmp( ui->app_type, "tool" ) == 0;
    oyjlStringAdd( &text, malloc, free, ".TH \"%s\" %d \"%s\" \"%s\"\n", ui->nick,
                   tool?1:7, date?date:"", tool?"User Commands":"Misc" );
  }

  oyjlStringAdd( &text, malloc, free, ".SH %s\n%s %s%s \\- %s\n", _("NAME"), ui->nick, vers?"v":"", vers?vers:"", ui->name );

  oyjlStringAdd( &text, malloc, free, ".SH %s\n", _("SYNOPSIS") );
  for(i = 0; i < ng; ++i)
  {
    oyjlOptionGroup_s * g = &opts->groups[i];
    char * syn = oyjlOptions_PrintHelpSynopsis_( opts, g,
                         oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MAN );
    if(syn && syn[0])
      oyjlStringAdd( &text, malloc, free, "%s\n%s", syn, (i < (ng-1)) ? ".br\n" : "" );
    free(syn);
  }

  if(desc)
    oyjlStringAdd( &text, malloc, free, ".SH %s\n%s\n", _("DESCRIPTION"), desc );

  oyjlStringAdd( &text, malloc, free, ".SH %s\n", _("OPTIONS") );
  for(i = 0; i < ng; ++i)
  {
    oyjlOptionGroup_s * g = &opts->groups[i];
    int dn = 0,
        j;
    char ** d_list = oyjlStringSplit2( g->detail, "|,", &dn, NULL, malloc );
    if(g->description)
      oyjlStringAdd( &text, malloc, free, ".SS\n%s\n", g->description  );
    else
    if(g->name)
      oyjlStringAdd( &text, malloc, free, ".SS\n%s\n", g->name );
    else
      oyjlStringAdd( &text, malloc, free, "\n"  );
    if(g->mandatory && g->mandatory[0])
    {
      char * t = oyjlOptions_PrintHelpSynopsis_( opts, g, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MAN );
      oyjlStringAdd( &text, malloc, free, "%s\n", t );
      free(t);
    }
    oyjlStringAdd( &text, malloc, free, ".br\n"  );
    if(g->help)
    {
      oyjlStringAdd( &text, malloc, free, "%s\n.br\n.sp\n.br\n", g->help );
    }
    for(j = 0; j < dn; ++j)
    {
      const char * option = d_list[j];
      char * t;
      oyjlOption_s * o = oyjlOptions_GetOptionL( opts, option, 0 );
      int mandatory_index = oyjlOptionMandatoryIndex_( o, g );
      int style = oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING | oyjlOPTIONSTYLE_MAN;
      if(mandatory_index == 0)
        style |= g->flags;
      if(!o)
      {
        fprintf(stdout, "%s %s: option not declared: %s\n", oyjlBT(0), g->name?g->name:"---", option);
        if(!getenv("OYJL_NO_EXIT")) exit(1);
      }
      switch(o->value_type)
      {
        case oyjlOPTIONTYPE_CHOICE:
          {
            int n = 0,l;
            t = oyjlOption_PrintArg(o, style);
            oyjlStringAdd( &text, malloc, free, "%s", t );
            free(t);
            if(o->name && !o->description)
              oyjlStringAdd( &text, malloc, free, "\t%s", o->name );
            oyjlStringAdd( &text, malloc, free, "\t%s%s%s%s", o->description ? o->description:"", o->help?"\n.RS\n":"", o->help?o->help:"", o->help?"\n.RE\n":"\n.br\n" );
            if(o->flags & OYJL_OPTION_FLAG_EDITABLE)
              break;
            while(o->values.choices.list[n].nick && o->values.choices.list[n].nick[0] != '\000')
              ++n;
            for(l = 0; l < n; ++l)
              oyjlStringAdd( &text, malloc, free, "\t\\-%s %s\t\t# %s%s%s%s%s\n.br\n",
                  o->o,
                  o->values.choices.list[l].nick,
                  o->values.choices.list[l].name && o->values.choices.list[l].name[0] ? o->values.choices.list[l].name : o->values.choices.list[l].description,
                  o->values.choices.list[l].description&&o->values.choices.list[l].description[0]?" : ":"",
                  o->values.choices.list[l].description?o->values.choices.list[l].description:"",
                  o->values.choices.list[l].help&&o->values.choices.list[l].help[0]?" - ":"",
                  o->values.choices.list[l].help?o->values.choices.list[l].help:"" );
          }
          break;
        case oyjlOPTIONTYPE_FUNCTION:
          {
            int n = 0,l;
            oyjlOptionChoice_s * list;
            t = oyjlOption_PrintArg(o, style);
            oyjlStringAdd( &text, malloc, free, "%s", t );
            free(t);
            oyjlStringAdd( &text, malloc, free, "\t%s%s%s%s", o->description ? o->description:"", o->help && o->help[0]?"\n.RS\n":"", o->help?o->help:"", o->help?"\n.RE\n":"\n.br\n" );
            if(o->flags & OYJL_OPTION_FLAG_EDITABLE)
              break;
            list = oyjlOption_GetChoices_(o, NULL, opts );
            if(list)
              while(list[n].nick && list[n].nick[0] != '\000')
                ++n;
            for(l = 0; l < n; ++l)
              oyjlStringAdd( &text, malloc, free, "\t\\-%s %s\t\t# %s%s%s\n.br\n",
                  o->o,
                  list[l].nick,
                  list[l].name && list[l].name[0] ? list[l].name : list[l].description,
                  (list[l].name && list[l].name[0] && list[l].description && list[l].description[0]) ? " - " : "",
                  (list[l].name && list[l].name[0] && list[l].description && list[l].description[0]) ? list[l].description : ""
                );
            /* not possible, as the result of oyjlOption_GetChoices_() is cached - oyjlOptionChoice_Release( &list ); */
          }
          break;
        case oyjlOPTIONTYPE_DOUBLE:
          {
            char * desc = oyjlOption_PrintArgDouble_( o, oyjlDESCRIPTION );
            t = oyjlOption_PrintArg(o, style);
            oyjlStringAdd( &text, malloc, free, "%s", t );
            free(t);
            oyjlStringAdd( &text, malloc, free, "\t%s%s%s%s", desc, o->help?"\n.RS\n":"", o->help?o->help:"", o->help?"\n.RE\n":"\n.br\n" );
            free(desc);
          }
          break;
        case oyjlOPTIONTYPE_NONE:
          {
            t = oyjlOption_PrintArg(o, style);
            oyjlStringAdd( &text, malloc, free, "%s", t );
            free(t);
            oyjlStringAdd( &text, malloc, free, "\t%s%s%s%s", o->description ? o->description:"", o->help?"\n.RS\n":"", o->help?o->help:"", o->help?"\n.RE\n":"\n.br\n" );
          }
        break;
        case oyjlOPTIONTYPE_START: break;
        case oyjlOPTIONTYPE_END: break;
      }
    }
    oyjlStringListRelease( &d_list, dn, free );
  }

  tmp = oyjlExtraManSections_( opts, oyjlOPTIONSTYLE_MAN, NULL, NULL );
  if(tmp)
  {
    oyjlStringAdd( &text, malloc, free, "%s", tmp );
    free(tmp);
  }

  if(mnft)
    oyjlStringAdd( &text, malloc, free, ".SH %s\n%s %s\n", _("AUTHOR"), mnft, mnft_url?mnft_url:"" );

  if(lice || copy)
  {
    oyjlStringAdd( &text, malloc, free, ".SH %s\n%s\n", _("COPYRIGHT"), copy?copy:"" );
    if(lice)
      oyjlStringAdd( &text, malloc, free, ".br\n%s: %s %s\n", _("License"), lice?lice:"", lice_url?lice_url:"" );
  }

  if(bugs)
    oyjlStringAdd( &text, malloc, free, ".SH %s\n%s %s\n", _("BUGS"), bugs, bugs_url?bugs_url:"" );
  else if(bugs_url)
    oyjlStringAdd( &text, malloc, free, ".SH %s\n%s\n", _("BUGS"), bugs_url );

  return text;
}

static void replaceOutsideHTML_(const char * text OYJL_UNUSED, const char * start, const char * end, const char * search, const char ** replace, void * data)
{
  if(start < end)
  {
    const char * word = start;
    int * insideXML = (int*) data,
          inside_table = 0,
          inside_xml;

    word = start;
    while(word && (word = strstr(word+1,"<")) != NULL && word < end)
      ++insideXML[1];
    word = start;
    while(word && (word = strstr(word+1,">")) != NULL && word < end)
      --insideXML[1];
    inside_xml = insideXML[1];

    if(!inside_xml)
    {
      word = start;
      while(word && (word = strstr(word+1,"<")) != NULL && word && word[0] != '/' && word < end)
        ++insideXML[0];
      word = start;
      while(word && (word = strstr(word+1,"</")) != NULL && word < end)
        --insideXML[0];
      inside_table = insideXML[0];
    }


    if( inside_table || inside_xml )
      *replace = search;
    else
    {
      if(strcmp(search,"`") == 0)
        *replace = "\\`";
      if(strcmp(search,"-") == 0)
        *replace = "\\-";
      if(strcmp(search,"_") == 0)
        *replace = "\\_";
    }

  }
}

/** @brief    Return markdown formated text from options
 *  @memberof oyjlUi_s
 *
 *  This function supports extra sections in MAN page style.
 *  @see oyjlUi_ToMan()
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/08/02
 *  @since   2018/11/07 (OpenICC: 0.1.1)
 */
char *       oyjlUi_ToMarkdown       ( oyjlUi_s          * ui,
                                       int                 flags )
{
  char * text = NULL, * tmp, * doxy_link = NULL;
  const char * date = NULL,
             * desc = NULL,
             * mnft = NULL, * mnft_url = NULL,
             * copy = NULL, * lice = NULL, * lice_url = NULL,
             * bugs = NULL, * bugs_url = NULL,
             * vers = NULL,
             * country = NULL;
  int i,n,ng;
  oyjlOptions_s * opts;
  char ** sections_ = NULL, *** sections = &sections_;
  int sn_ = 0, * sn = &sn_;

  if( !ui ) return text;

  opts = ui->opts;
  n = oyjlUiHeaderSection_Count( ui->sections );
  for(i = 0; i < n; ++i)
  {
    oyjlUiHeaderSection_s * s = &ui->sections[i];
    if(strcmp(s->nick, "manufacturer") == 0) { mnft = s->name; mnft_url = s->description; }
    else if(strcmp(s->nick, "copyright") == 0) copy = s->name;
    else if(strcmp(s->nick, "license") == 0) { lice = s->name; lice_url = s->description; }
    else if(strcmp(s->nick, "url") == 0) continue;
    else if(strcmp(s->nick, "support") == 0) { bugs = s->name; bugs_url = s->description; }
    else if(strcmp(s->nick, "download") == 0) continue;
    else if(strcmp(s->nick, "sources") == 0) continue;
    else if(strcmp(s->nick, "development") == 0) continue;
    else if(strcmp(s->nick, "oyjl_module_author") == 0) continue;
    else if(strcmp(s->nick, "documentation") == 0) desc = s->description ? s->description : s->name;
    else if(strcmp(s->nick, "version") == 0) vers = s->name;
    else if(strcmp(s->nick, "date") == 0) date = s->description ? s->description : s->name;
  }

  ng = oyjlOptions_CountGroups(opts);
  if(!ng && !(flags & oyjlUI_STATE_NO_CHECKS)) return NULL;

#if defined(OYJL_HAVE_LANGINFO_H) && !defined(__ANDROID__)
  country = nl_langinfo( _NL_ADDRESS_LANG_AB );
#endif
  if(flags & oyjlUI_STATE_VERBOSE)
    fprintf(stderr, "country: %s\n", country?country:"");

  oyjlStringAdd( &doxy_link, malloc, free, "{#%s%s}", ui->nick, country?country:"" );
  oyjlStringReplace( &doxy_link, "-", "", malloc, free );

  if(ui->app_type && ui->app_type[0])
  {
    int tool = strcmp( ui->app_type, "tool" ) == 0;
    oyjlStringAdd( &text, malloc, free, "<strong>\"%s\"</strong> *%d* <em>\"%s\"</em> \"%s\"\n", ui->nick,
                   tool?1:7, date?date:"", tool?"User Commands":"Misc" );
  }

  ADD_SECTION( _("NAME"), "name", "%s %s%s - %s\n", ui->nick, vers?"v":"", vers?vers:"", ui->name )

  if(ng)
  ADD_SECTION( _("SYNOPSIS"), "synopsis", "", "" )
  for(i = 0; i < ng; ++i)
  {
    oyjlOptionGroup_s * g = &opts->groups[i];
    char * syn = oyjlOptions_PrintHelpSynopsis_( opts, g,
                         oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MARKDOWN | oyjlOPTIONSTYLE_LINK_GROUP );
    if(syn[0])
      oyjlStringAdd( &text, malloc, free, "%s\n%s", syn, (i < (ng-1)) ? "<br />\n" : "" );
    free(syn);
  }

  if(desc)
    ADD_SECTION( _("DESCRIPTION"), "description", "%s\n", desc )

  if(ng)
  ADD_SECTION( _("OPTIONS"), "options", "", "" )
  for(i = 0; i < ng; ++i)
  {
    oyjlOptionGroup_s * g = &opts->groups[i];
    int d = 0,
        j;
    char ** d_list = oyjlStringSplit2( g->detail, "|,", &d, NULL, malloc ),
         * t;
    if(g->description)
    {
      const char * group_id = oyjlOptions_GetGroupId_( opts, g );
      if(group_id)
        oyjlStringAdd( &text, malloc, free, "<h3 id=\"%s\">%s</h3>\n", group_id, g->description );
      else
        oyjlStringAdd( &text, malloc, free, "<h3>%s</h3>\n", g->description );
      oyjlStringAdd( &text, malloc, free, "\n"  );
    }
    if(g->mandatory && g->mandatory[0])
    {
      char * t = oyjlOptions_PrintHelpSynopsis_( opts, g, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MARKDOWN | oyjlOPTIONSTYLE_LINK_SYNOPSIS );
      oyjlStringAdd( &text, malloc, free, "&nbsp;&nbsp;%s\n", t );
      free(t);
    }
    oyjlStringAdd( &text, malloc, free, "\n"  );
    if(g->help)
    {
      oyjlStringAdd( &text, malloc, free, "&nbsp;&nbsp;%s\n\n", g->help );
    }
    if(d)
      oyjlStringAdd( &text, malloc, free, "<table style='width:100%'>\n" );
    for(j = 0; j < d; ++j)
    {
      const char * option = d_list[j];
      oyjlOption_s * o = oyjlOptions_GetOptionL( opts, option, 0 );
      int mandatory_index = oyjlOptionMandatoryIndex_( o, g );
      int style = oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING | oyjlOPTIONSTYLE_MARKDOWN;
      if(mandatory_index == 0)
        style |= g->flags;
      if(!o)
      {
        fprintf(stdout, "%s %s: option not declared: %s\n", oyjlBT(0), g->name?g->name:"---", option );
        if(!getenv("OYJL_NO_EXIT")) exit(1);
      }
#define OYJL_LEFT_TD_STYLE " style='padding-left:1em;padding-right:1em;vertical-align:top;width:25%'"
      switch(o->value_type)
      {
        case oyjlOPTIONTYPE_CHOICE:
          {
            int n = 0,l;
            t = oyjlOption_PrintArg(o, style);
            oyjlStringAdd( &text, malloc, free, " <tr><td" OYJL_LEFT_TD_STYLE ">%s</td>", t );
            free(t);
            oyjlStringAdd( &text, malloc, free, " <td>%s%s%s", o->description ? o->description:"", o->help?"<br />":"", o->help?o->help :"" );
            if(o->flags & OYJL_OPTION_FLAG_EDITABLE)
              break;
            while(o->values.choices.list[n].nick && o->values.choices.list[n].nick[0] != '\000')
              ++n;
            if(n) oyjlStringAdd( &text, malloc, free, "\n  <table>\n");
            for(l = 0; l < n; ++l)
              oyjlStringAdd( &text, malloc, free, "   <tr><td style='padding-left:0.5em'><strong>-%s %s</strong></td><td># %s</td></tr>\n",
                  o->o, o->values.choices.list[l].nick,
                  o->values.choices.list[l].name && o->values.choices.list[l].nick[0] ? o->values.choices.list[l].name : o->values.choices.list[l].description,
                  o->values.choices.list[l].description&&o->values.choices.list[l].description[0]?" : ":"",
                  o->values.choices.list[l].description?o->values.choices.list[l].description:""
                  );
            if(n) oyjlStringAdd( &text, malloc, free, "  </table>\n");
            oyjlStringAdd( &text, malloc, free, "  </td>\n");
          }
          break;
        case oyjlOPTIONTYPE_FUNCTION:
          {
            int n = 0,l;
            oyjlOptionChoice_s * list;
            t = oyjlOption_PrintArg(o, style);
            oyjlStringAdd( &text, malloc, free, " <tr><td" OYJL_LEFT_TD_STYLE ">%s</td>", t );
            free(t);
            oyjlStringAdd( &text, malloc, free, " <td>%s%s%s", o->description ? o->description:"", o->help?"<br />":"", o->help?o->help :"" );
            if(o->flags & OYJL_OPTION_FLAG_EDITABLE)
              break;
            list = oyjlOption_GetChoices_(o, NULL, opts );
            if(list)
              while(list[n].nick && list[n].nick[0] != '\000')
                ++n;
            if(n) oyjlStringAdd( &text, malloc, free, "\n  <table>\n");
            for(l = 0; l < n; ++l)
              oyjlStringAdd( &text, malloc, free, "   <tr><td style='padding-left:0.5em'><strong>-%s %s</strong></td><td># %s</td></tr>\n", o->o, list[l].nick, list[l].name && list[l].nick[0] ? list[l].name : list[l].description );
            if(n) oyjlStringAdd( &text, malloc, free, "  </table>\n");
            oyjlStringAdd( &text, malloc, free, "  </td>\n");
            /* not possible, as the result of oyjlOption_GetChoices_() is cached - oyjlOptionChoice_Release( &list ); */
          }
          break;
        case oyjlOPTIONTYPE_DOUBLE:
          {
            char * desc = oyjlOption_PrintArgDouble_( o, oyjlDESCRIPTION | oyjlHELP );
            t = oyjlOption_PrintArg(o, style);
            oyjlStringAdd( &text, malloc, free, " <tr><td" OYJL_LEFT_TD_STYLE ">%s</td>", t );
            free(t);
            oyjlStringAdd( &text, malloc, free, " <td>%s</td>", desc );
            free( desc );
          }
          break;
        case oyjlOPTIONTYPE_NONE:
          t = oyjlOption_PrintArg(o, style);
          oyjlStringAdd( &text, malloc, free, " <tr><td" OYJL_LEFT_TD_STYLE ">%s</td>", t );
          free(t);
          oyjlStringAdd( &text, malloc, free, " <td>%s%s%s</td>", o->description ? o->description:"", o->help?"<br />":"", o->help?o->help :"" );
        break;
        case oyjlOPTIONTYPE_START: break;
        case oyjlOPTIONTYPE_END: break;
      }
      oyjlStringAdd( &text, malloc, free, " </tr>\n" );
    }
    oyjlStringListRelease( &d_list, d, free );
    if(d)
      oyjlStringAdd( &text, malloc, free, "</table>\n" );
    oyjlStringAdd( &text, malloc, free, "\n"  );
  }

  tmp = oyjlExtraManSections_( opts, oyjlOPTIONSTYLE_MARKDOWN, sections, sn );
  if(tmp)
  {
    oyjlStringAdd( &text, malloc, free, "%s", tmp );
    free(tmp);
  }

  if(mnft)
    ADD_SECTION( _("AUTHOR"), "author", "%s %s\n", mnft, mnft_url?mnft_url:"" )

  if(lice || copy)
  {
    ADD_SECTION( _("COPYRIGHT"), "copyright", "*%s*\n", copy?copy:"" )
    if(lice)
      oyjlStringAdd( &text, malloc, free, "\n\n<a name=\"license\"></a>\n### %s\n%s", _("License"), lice?lice:"" );
    if(lice_url)
      oyjlStringAdd( &text, malloc, free, " <a href=\"%s\">%s</a>", lice_url, lice_url );
    if(lice || lice_url)
      oyjlStringAdd( &text, malloc, free, "\n" );
  }

  if(bugs && bugs_url)
    ADD_SECTION( _("BUGS"), "bugs", "%s <a href=\"%s\">%s</a>\n", bugs, bugs_url, bugs_url )
  else if(bugs)
    ADD_SECTION( _("BUGS"), "bugs", "<a href=\"%s\">%s</a>\n", bugs, bugs )

  {
    char * txt = NULL;
    int i;

    oyjlStringAdd( &txt, malloc, free, "# %s %s%s %s\n<a name=\"toc\"></a>\n", ui->nick, vers?"v":"", vers?vers:"", doxy_link );
    for(i = 0; i < sn_/2; ++i)
      oyjlStringAdd( &txt, malloc, free, "[%s](#%s) ", sections_[2*i+0], sections_[2*i+1] );
    oyjlStringAdd( &txt, malloc, free, "\n\n%s", text );
    free(text);
    text = txt;
  }

  {
    const char * t;
    int insideXML[3] = {0,0,0};
    oyjl_str tmp = oyjlStrNew(10,0,0);
    oyjlStrAppendN( tmp, text, strlen(text) );
    oyjlStrReplace( tmp, "`", "\\`", replaceOutsideHTML_, insideXML );
    oyjlStrReplace( tmp, "-", "\\-", replaceOutsideHTML_, insideXML );
    oyjlStrReplace( tmp, "_", "\\_", replaceOutsideHTML_, insideXML );
    t = oyjlStr(tmp);
    text[0] = 0;
    oyjlStringAdd( &text, malloc,free, "%s", t );
    oyjlStrRelease( &tmp );
  }

  free(doxy_link);
  oyjlStringListRelease( sections, sn_, free );

  return text;
}


/** 
 *  @} *//* oyjl_args
 */

