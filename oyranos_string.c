/** @file oyranos_string.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2004-2010 (C) Kai-Uwe Behrmann
 *
 *  @brief    pure string handling functions
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2010/11/16
 */

#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iconv.h>

#include "config.h"
#include "oyranos.h"
#include "oyranos_alpha.h"
#include "oyranos_debug.h"
#include "oyranos_elektra.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"
#include "oyranos_io.h"
#include "oyranos_sentinel.h"
#include "oyranos_xml.h"
#include "oyranos_alpha.h"
#include "oyranos_string.h"

/* --- Helpers  --- */

/* --- static variables   --- */

/* --- structs, typedefs, enums --- */

/* --- internal API definition --- */

/* separate from the external functions */

int          oyStringToLong          ( const char        * text,
                                       long              * value )
{
  char * p = 0;
  *value = strtol( text, &p, 0 );
  if(p && p != text && p[0] == '\000' )
    return 0;
#if 0
  else if(errno)
    return errno;
#endif
  else
    return 1;
}

int          oyStringToDouble        ( const char        * text,
                                       double            * value )
{
  char * p = 0;
  *value = strtod( text, &p );
  if(p && p != text && p[0] == '\000')
    return 0;
#if 0
  else if(errno)
    return errno;
#endif
  else
    return 1;
}


/* string manipulation */

char*
oyStringCopy_      ( const char    * text,
                     oyAlloc_f       allocateFunc )
{
  char * text_copy = NULL;
    
  if(text)
  {
      oyAllocHelper_m_( text_copy, char, oyStrlen_(text) + 1,
                        allocateFunc, return 0 );
      oyStrcpy_( text_copy, text );
  }
  return text_copy;
}

/** @internal 
 *  @brief append a string and care about allocation
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */
char*              oyStringAppend_   ( const char        * text,
                                       const char        * append,
                                       oyAlloc_f           allocateFunc )
{
  char * text_copy = NULL;
  int text_len = 0, append_len = 0;

  if(text)
    text_len = oyStrlen_(text);

  if(append)
    append_len = oyStrlen_(append);

  if(text_len || append_len)
  {
    oyAllocHelper_m_( text_copy, oyChar,
                      text_len + append_len + 1,
                      allocateFunc, return 0 );

    if(text_len)
      memcpy( text_copy, text, text_len );
      
    if(append_len)
      memcpy( &text_copy[text_len], append, append_len );
    text_copy[text_len+append_len] = '\000';
  }

  return text_copy;
}

/** @internal 
 *  @brief   search in a data pointer for text
 *
 *  Extract only the visible text. Stop for any other non print or space sign. 
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/06 (Oyranos: 0.1.10)
 *  @date    2009/01/06
 */
int                oyStringFromData_ ( const oyPointer     ptr,
                                       size_t              size,
                                       char             ** text_new,
                                       size_t            * text_new_size,
                                       oyAlloc_f           allocateFunc )
{
  const char * text = (const char*) ptr;
  char * text_tmp = 0;
  int j,
      error = 0;

  if(ptr && size)
  {
    j = 0;

    while(j < size)
      if(!isprint( text[j] ) && !isspace( text[j] ))
      {
        text = 0;
        break;
      } else
        ++j;

    if(text)
    {
      text_tmp = allocateFunc( j );
      error = !text_tmp;
      if(!error)
      {
        error = !memcpy( text_tmp, text, j-1 );
        if(error)
          text_tmp[j-1] = 0;
      }
    }
  }

  if(!error && text)
  {
    *text_new = text_tmp;
    *text_new_size = j - 1;
  }

  return error;
}

/** @internal 
 *  @brief add a string and care about de-/allocation
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */
void               oyStringAdd_      ( char             ** text,
                                       const char        * append,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocFunc )
{
  char * text_copy = NULL;

  text_copy = oyStringAppend_(*text, append, allocateFunc);

  if(text && *text && deallocFunc)
    deallocFunc(*text);

  *text = text_copy;

  return;
}

/** @internal 
 *  @brief   printf style string add
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/07 (Oyranos: 0.1.10)
 *  @date    2009/02/07
 */
int                oyStringAddPrintf_( char             ** string,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocFunc,
                                       const char        * format,
                                                           ... )
{
  char * text_copy = NULL;
  char * text = 0;
  va_list list;
  int len;
  size_t sz = 256;

  text = malloc( sz );
  if(!text)
  {
    fprintf(stderr,
     "oyranos.c:80 oyMessageFunc_() Could not allocate 256 byte of memory.\n");
    return 1;
  }

  text[0] = 0;

  va_start( list, format);
  len = vsnprintf( text, sz, format, list);
  va_end  ( list );

  if (len >= sz)
  {
    text = realloc( text, (len+1)*sizeof(char) );
    va_start( list, format);
    len = vsnprintf( text, len+1, format, list);
    va_end  ( list );
  }


  text_copy = oyStringAppend_(*string, text, allocateFunc);

  if(string && *string && deallocFunc)
    deallocFunc(*string);

  *string = text_copy;

  free(text);

  return 0;
}


char**             oyStringSplit     ( const char    * text,
                                       const char      delimiter,
                                       int           * count,
                                       oyAlloc_f       allocateFunc )
{
  return oyStringSplit_(text, delimiter,count,allocateFunc);
}

char**             oyStringSplit_    ( const char    * text,
                                       const char      delimiter,
                                       int           * count,
                                       oyAlloc_f       allocateFunc )
{
  char ** list = 0;
  int n = 0;

  if(text && text[0] && delimiter)
  {
    int i;
    const char * tmp = text;

    if(!allocateFunc)
      allocateFunc = oyAllocateFunc_;

    if(tmp[0] == delimiter) ++n;
    do { ++n;
    } while( (tmp = oyStrchr_(tmp + 1, delimiter)) );

    tmp = 0;

    if((list = allocateFunc( (n+1) * sizeof(char*) )) == 0) return 0;
    memset( list, 0, (n+1) * sizeof(char*) );

    {
      const char * start = text;
      for(i = 0; i < n; ++i)
      {
        intptr_t len = 0;
        char * end = oyStrchr_(start, delimiter);

        if(end > start)
          len = end - start;
        else if (end == start)
          len = 0;
        else
          len = oyStrlen_(start);

        if((list[i] = allocateFunc( len+1 )) == 0) return 0;

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


char *             oyStrnchr_        ( char              * text,
                                       char                delimiter,
                                       int                 len )
{
  int i = 0;

  while(i < len)
  {
    if(text[i] == delimiter)
      return &text[i];
    else if(text[i] == 0)
      return 0;
    ++i;
  }

  return 0;
}

char *             oyStrnchrN_       ( char              * text,
                                       char                delimiter,
                                       int                 len,
                                       int               * end )
{
  *end = 0;

  while(*end < len)
  {
    if(text[*end] == delimiter)
      return &text[*end];
    else if(text[*end] == 0)
      return 0;
    ++ (*end);
  }

  return 0;
}

int                oyStringSegments_ ( const char        * text,
                                       char                delimiter )
{
  int n = 0;

  if(text[0] == delimiter) ++n;
  do { ++n;
  } while( (text = oyStrchr_(text + 1, delimiter)) );

  return n;
}

int                oyStringSegmentsN_( const char        * text,
                                       int                 len,
                                       char                delimiter )
{
  int n = 0;
  int end = 0;

  if(text[0] == delimiter) ++n;
  do {
    ++n;
    len -= end;
  } while( (text = oyStrnchrN_((char*)text + 1, delimiter, len, &end)) &&
           len-end-1 > 0 );

  return n;
}

char *             oyStringSegment_  ( char              * text,
                                       char                delimiter,
                                       int                 segment,
                                       int               * end )
{
  intptr_t end_pos = 0;
  int i = 0;
  char * t = text;

  for(; i < segment; ++i)
  {
    t = oyStrchr_( t, delimiter );
    ++t;
  }

  end_pos = (intptr_t) oyStrchr_(t, delimiter);

  if(end_pos == 0)
  {
    *end = oyStrlen_(t);
    return t;
  }

  *end = (int) (end_pos - (intptr_t) t);

  return t;
}

char *             oyStringSegmentN_ ( char              * text,
                                       int                 len,
                                       char                delimiter,
                                       int                 segment,
                                       int               * end )
{
  intptr_t end_pos = (intptr_t)text;
  int i = 0;
  char * t = text;

  for(; i < segment; ++i)
  {
    t = oyStrchr_( t, delimiter );
    ++t;
  }

  if(len)
  {
    end_pos = (intptr_t) oyStrnchr_(t, delimiter, text + len - t);

    if(end_pos == 0)
    {
      *end = text + len - t;
      return t;
    }
  }

  *end = (int) (end_pos - (intptr_t) t);

  return t;
}

int    oyStringSegmentX_             ( const char        * text,
                                       char                delimiter,
                                       int               * count,
                                       int              ** pos,
                                       char             ** max_segment )
{
  int n = 0, i, max_segment_len = 0;
  const char * tmp = text;
  static int int_size = sizeof(int);
  const char * start = text;

  if(tmp[0] == delimiter) ++n;
  do { ++n;
  } while( (tmp = oyStrchr_(tmp + 1, delimiter)) );

  if(n > *pos[0])
  {
    oyDeAllocateFunc_( *pos );
    *pos = oyAllocateFunc_( int_size * (n+1) );
    if(!*pos) return 1;
    *pos[0] = n;
  }

  for(i = 0; i < n; ++i)
  {
        intptr_t len = 0;
        char * end = oyStrchr_(start, delimiter);

        if(end > start)
          len = end - start;
        else if (end == start)
          len = 0;
        else
          len = oyStrlen_(start);

        *pos[i] = (int)((intptr_t)(start - text + len));
        start += len + 1;

    if(len > max_segment_len)
      max_segment_len = len;
  }

  if(!max_segment ||
     ((int)*((uint8_t*)&max_segment[0]) < max_segment_len &&
      (int)*((uint8_t*)&max_segment[0]) != 255))
  {
    *((uint8_t*)&max_segment[0]) = OY_MIN(255, max_segment_len);
    *max_segment = oyAllocateFunc_( *((uint8_t*)&max_segment[0]) );
    if(!*max_segment) return 1;
  }

  return 0;
}


void               oyStringListAdd_  ( char            *** list,
                                       int               * n,
                                       const char       ** append,
                                       int                 n_app,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocateFunc )
{
  int alt_n = *n;
  char ** tmp = oyStringListAppend_((const char**)*list, alt_n, append, n_app,
                                     n, allocateFunc);

  oyStringListRelease_(list, alt_n, deallocateFunc);

  *list = tmp;
}

void               oyStringListAddStaticString_ ( char *** list,
                                       int               * n,
                                       const char        * string,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocateFunc )
{
  int alt_n = *n;
  char ** tmp = oyStringListAppend_((const char**)*list, alt_n,
                                    (const char**)&string, 1,
                                     n, allocateFunc);

  oyStringListRelease_(list, alt_n, deallocateFunc);

  *list = tmp;
}

void               oyStringListAddString_ ( char       *** list,
                                       int               * n,
                                       char             ** string,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocateFunc )
{
  int alt_n = *n;
  char ** tmp = oyStringListAppend_((const char**)*list, alt_n,
                                    (const char**)string, 1,
                                     n, allocateFunc);

  deallocateFunc(*string); *string = 0;
  oyStringListRelease_(list, alt_n, deallocateFunc);

  *list = tmp;
}

char**             oyStringListAppend_( const char   ** list,
                                        int             n_alt,
                                        const char   ** append,
                                        int             n_app,
                                        int           * count,
                                        oyAlloc_f       allocateFunc )
{
  char ** nlist = 0;

  {
    int i = 0;
    int n = 0;

    if(n_alt || n_app)
      oyAllocHelper_m_(nlist, char*, n_alt + n_app +1, allocateFunc, return NULL);

    for(i = 0; i < n_alt; ++i)
    {
      if(list[i] /*&& oyStrlen_(list[i])*/)
        nlist[n] = oyStringCopy_( list[i], allocateFunc );
      n++;
    }

    for(i = 0; i < n_app; ++i)
    {
      if(1/*oyStrlen_( append[i] )*/)
        nlist[n] = oyStringCopy_( append[i], allocateFunc );
      n++;
    }

    if(count)
      *count = n;
  }

  return nlist;
}


/** @internal
 *  @brief reducing filter
 *
 *  In order to collect all possible matches chain this filter.
 *
 *  @version Oyranos: 0.3.0
 *  @since   2007/12/00 (Oyranos: 0.1.8)
 *  @date    2011/01/31
 */
char**             oyStringListFilter_(const char   ** list,
                                       int             list_n,
                                       const char    * dir_string,
                                       const char    * string,
                                       const char    * suffix,
                                       int           * count,
                                       oyAlloc_f       allocateFunc )
{
  char ** nl = 0;
  int nl_n = 0;
  int i;

  for(i = 0; i < list_n; ++i)
  {
    int b = 1;

    if(list[i] && list[i][0])
    {
      if(dir_string && dir_string[0])
      {
        char * t = oyStrstr_(list[i], dir_string);
        if(t)
          b = (oyStrstr_(t+oyStrlen_(dir_string), OY_SLASH)) ? 1:0;
        else
          b = 0;
        if(!b) continue;
      }

      if(b && string && string[0])
      {
        const char * fn;
        fn = oyStrrchr_( list[i], OY_SLASH_C );
        if(!fn)
          fn = list[i];
        else
          fn++;
        b = (oyStrstr_(fn, string))?1:0;
        if(!b) continue;
      }

      if(b && suffix && suffix[0])
      {
        const char * fn = list[i],
                   * f1 = fn + oyStrlen_(fn) - oyStrlen_(suffix);
        b = oyStrcmp_(f1, suffix) == 0;
        if(!b) continue;
      }
    }

    if(b)
    {
      if(!nl)
        oyAllocHelper_m_( nl, char*, list_n + 1, allocateFunc, return 0);

      nl[nl_n] = oyStringCopy_( list[i], allocateFunc );
      if(nl[nl_n])
        ++nl_n;
    }
  }

  if(count)
    *count = nl_n;

  return nl;
}

void          oyStringListRelease_    ( char          *** l,
                                        int               size,
                                        oyDeAlloc_f       deallocFunc )
{
  char *** list = l;

  DBG_MEM_START

  if( l )
  {
    size_t i;

    for(i = 0; i < size; ++i)
      if((*list)[i])
        deallocFunc( (*list)[i] );
    if(*list)
      deallocFunc( *list );
    *list = NULL;
  }

  DBG_MEM_ENDE
}

int     oyStrlen_( const char * str_ )
{
  if(!str_)
    WARNc_S("string missed");
  return strlen(str_); }
void    oyStrcpy_( char * targ_, const char * src_ )
{
  if(!src_ || !targ_)
    WARNc_S("string missed");
  strcpy(targ_,src_); }
char *  oyStrchr_( const char * str_, char c_ )
{
  if(!str_)
    WARNc_S("string missed");
  return strchr(str_,c_); }
char *  oyStrrchr_( const char * str_, char c_ )
{
  if(!str_)
    WARNc_S("string missed");
  return strrchr(str_,c_); }
char *  oyStrstr_( const char * str1_, const char * str2_ )
{
  if(!str1_ || !str2_)
    WARNc_S("string missed");
  return strstr(str1_,str2_); }
int     oyStrcmp_( const char * str1_, const char * str2_ )
{
  if(!str1_ || !str2_)
    WARNc_S("string missed");
  return strcmp(str1_,str2_); }
char    oyToupper_( char c_ ) { return toupper(c_); }


/** @internal returns the binary lenth of a string */
size_t oyStrblen_            (const char * start)
{
  char *end = NULL;

  if( start )
  {
    end = strchr( start, 0 );

    if( end )
      return end - start + 1;
  }

  return 0;
}

