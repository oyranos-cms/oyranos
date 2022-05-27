/** @file oyranos_string.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2004-2014 (C) Kai-Uwe Behrmann
 *
 *  @brief    pure string handling functions
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2010/11/16
 */

#include <math.h>             /* NAN */
#include <sys/stat.h>
#include <stddef.h>           /* size_t ptrdiff_t */
#include <stdlib.h>
#include <stdint.h>           /* uint64_t uintptr_t */
#include <stdio.h>
#include <string.h>
#include "oyranos_config_internal.h"
#if defined(HAVE_ICONV_H) && !defined(__ANDROID__)
# define USE_ICONV_H 1
# include <iconv.h>
#endif

#include "oyranos.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"
#include "oyranos_io.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"

/* --- Helpers  --- */

/* --- static variables   --- */

/* --- structs, typedefs, enums --- */

/* --- internal API definition --- */

/* separate from the external functions */


/* string manipulation */

void               oyStringFree_     ( char             ** text,
                                       oyDeAlloc_f         deallocFunc )
{
  if(text)
  {
    if(deallocFunc)
      deallocFunc(*text);
    *text = NULL;
  }
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
  int append_len = 0;

  if(append)
    append_len = oyStrlen_(append);

  text_copy = oyStringAppendN(text, append, append_len, allocateFunc);

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
int                oyStringFromData_ ( const void        * ptr,
                                       size_t              size,
                                       char             ** text_new,
                                       size_t            * text_new_size,
                                       oyAlloc_f           allocateFunc )
{
  const char * text = (const char*) ptr;
  char * text_tmp = 0;
  int j = 0,
      error = 0;

  if(ptr && size)
  {
    while((size_t)j < size)
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

  if(!text) return;

  text_copy = oyStringAppend_(*text, append, allocateFunc);

  if(!deallocFunc) deallocFunc = oyDeAllocateFunc_;

  if(*text && deallocFunc)
    deallocFunc(*text);

  *text = text_copy;

  return;
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

int                oyStringCaseCmp_  ( const char        * a,
                                       const char        * b )
{
#ifdef HAVE_POSIX
  return strcasecmp(a,b);
#else
  char * la = oyStringCopy_(a,0),
       * lb = oyStringCopy_(b,0),
       * t;
  int n = strlen(a), i;
  int result = 0;
  /* the following un caseing is portable,
   * still strcasecmp() might be faster? */
  t = la;
  for(i = 0; i < n; ++i)
    if(isalpha(t[i]))
      t[i] = tolower(t[i]);

  n = strlen(b);
  t = lb;
  for(i = 0; i < n; ++i)
    if(isalpha(t[i]))
      t[i] = tolower(t[i]);
  
  result = strcmp( la, lb );
  oyFree_m_(la);
  oyFree_m_(lb);
  return result;
#endif
}

int                oyStringSegments_ ( const char        * text,
                                       char                delimiter )
{
  int n = 0;

  if(text[0] == '\000') return 1;
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

  if(text[0] == '\000') return 1;
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
  uintptr_t end_pos = 0;
  int i = 0;
  char * t = text;

  for(; i < segment; ++i)
  {
    t = oyStrchr_( t, delimiter );
    if(t) ++t;
  }

  end_pos = (uintptr_t) oyStrchr_(t, delimiter);

  if(end_pos == 0)
  {
    *end = oyStrlen_(t);
    return t;
  }

  *end = (int) (end_pos - (uintptr_t) t);

  return t;
}

char *             oyStringSegmentN_ ( char              * text,
                                       int                 len,
                                       char                delimiter,
                                       int                 segment,
                                       int               * end )
{
  uintptr_t end_pos = (uintptr_t)text;
  int i = 0;
  char * t = text;

  for(; i < segment; ++i)
  {
    t = oyStrchr_( t, delimiter );
    ++t;
  }

  if(len)
  {
    end_pos = (uintptr_t) oyStrnchr_(t, delimiter, text + len - t);

    if(end_pos == 0)
    {
      *end = text + len - t;
      return t;
    }
  }

  *end = (int) (end_pos - (uintptr_t) t);

  return t;
}

int    oyStringSegmentX_             ( const char        * text,
                                       char                delimiter,
                                       int               * count OY_UNUSED,
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
        ptrdiff_t len = 0;
        char * end = oyStrchr_(start, delimiter);

        if(end > start)
          len = end - start;
        else if (end == start)
          len = 0;
        else
          len = oyStrlen_(start);

        *pos[i] = (int)((ptrdiff_t)(start - text + len));
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


/** @internal
 *  @brief   find duplicates with strstr()
 *
 *  @version Oyranos: 0.9.5
 *  @since   2013/12/07 (Oyranos: 0.9.5)
 *  @date    2013/12/07
 */
int                oyStringListHas_  ( const char       ** list,
                                       int                 list_n,
                                       const char        * string )
{
  int n = 0;
  int i;

  for(i = 0; i < list_n; ++i)
  {
    if(list[i] && list[i][0])
    {
      if(string && string[0])
      {
        const char * t = oyStrstr_(list[i], string);
        if(t)
          ++n;
      }
    }
  }

  return n;
}

void               oyStringListReplaceBy (
                                       char             ** list,
                                       int                 list_n,
                                       char *           (* replacer)(const char*, oyAlloc_f),
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deAllocateFunc )
{
  int i;
  if(!deAllocateFunc) deAllocateFunc = oyDeAllocateFunc_;
  for(i =  0; i < list_n; ++i)
  {
    char * t = replacer(list[i], allocateFunc);
    if(list[i]) deAllocateFunc(list[i]);
    list[i] = t;
  }
}

void oyStringListSetHeadingWhiteSpace( char             ** list,
                                       int                 n,
                                       int                 count,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocateFunc )
{
  int i;
  char * head = NULL;
  if(!allocateFunc) allocateFunc = oyAllocateFunc_;
  if(!deallocateFunc) deallocateFunc = oyDeAllocateFunc_;

  if(count)
  {
    head = (char*)allocateFunc( sizeof(char) * (count + 4) );
    for(i = 0; i < count; ++i)
      head[i] = ' ';
    head[count] = '\000';
  }
  else
    head = oyStringCopy("", 0);

  for(i = 0; i < n; ++i)
  {
    int h = 0;
    char * t = list[i], * tmp = NULL;
    if(!t) return;
    while(t[h] == ' ') ++h;
    if(h != count)
    {
      oyStringAddPrintf( &tmp, allocateFunc, deallocateFunc, "%s%s", head, &list[i][h]  );
      deallocateFunc( list[i] );
      list[i] = tmp; tmp = NULL;
    }
  }

  if(head) deallocateFunc(head);
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

/* rename some symbols */
#include "oyranos_db.h"

int     oyStrlen_( const char * str_ )
{
  if(!str_)
  { WARNc_S("string missed");
    return 0;
  }
  return strlen(str_); }
void    oyStrcpy_( char * targ_, const char * src_ )
{
  if(!src_ || !targ_)
  { WARNc_S("string missed"); return; }
  strcpy(targ_,src_); }
char *  oyStrchr_( const char * str_, char c_ )
{
  if(!str_)
  { WARNc_S("string missed"); return NULL; }
  return strchr(str_,c_); }
char *  oyStrrchr_( const char * str_, char c_ )
{
  if(!str_)
  { WARNc_S("string missed"); return NULL; }
  return strrchr(str_,c_); }
char *  oyStrstr_( const char * str1_, const char * str2_ )
{
  if(!str1_ || !str2_)
  { WARNc_S("string missed"); return NULL; }
  return strstr(str1_,str2_); }
int     oyStrcmp_( const char * str1_, const char * str2_ )
{
  if(!str1_ || !str2_)
  { WARNc_S("string missed"); return -1; }
  return strcmp(str1_,str2_); }
char    oyToupper_( char c_ ) { return toupper(c_); }

void *   oyMemMem                    ( const void        * haystack,
                                       size_t              haystacklen,
                                       const void        * needle,
                                       size_t              needlelen )
{
  char * hay_buf = (char*) haystack,
       * needle_buf = (char*) needle,
       * pos = hay_buf;
  size_t len = haystacklen;

  do
  {
    pos = memchr( pos, needle_buf[0], len );
    len = hay_buf + haystacklen - pos;

    if(pos && len >= needlelen &&
       memcmp( pos, needle, needlelen ) == 0)
      return pos;

  } while( pos++ && len > needlelen );

  return NULL;
}



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


/** @func  oyIconv
 *  @brief convert between codesets
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/05/27
 *  @since   2008/07/23 (Oyranos: 0.1.8)
 */
int                oyIconv           ( const char        * input,
                                       size_t              len_in,
                                       size_t              len_out,
                                       char              * output,
                                       const char        * from_codeset,
                                       const char        * to_codeset )
{
  int error = 0;

  char * out_txt = output;
  char * in_txt = (char*)input;
  const char * src = from_codeset ? from_codeset : oy_domain_codeset;
  const char * loc_env = 
# ifdef HAVE_LOCALE_H
  setlocale( LC_MESSAGES, 0 )
# else
  0
# endif
  , *loc = to_codeset;
#ifdef USE_ICONV_H
  iconv_t cd;
#endif
  size_t size, in_left = len_in, out_left = len_out;

  /* application codeset */
  if(!loc && oy_domain_codeset)
    loc = oy_domain_codeset;
  /* environment codeset */
  if(!loc && loc_env)
  {
    char * loc_tmp = strchr(loc_env, '.');
    if(loc_tmp && strlen(loc_tmp) > 2)
      loc = loc_tmp + 1;
  }
  /* fallback codeset */
  if(!loc)
    loc = "UTF-8";

#ifdef USE_ICONV_H
  if(!from_codeset && !oy_domain_codeset)
#endif
  {
    error = !memcpy(output, input, sizeof(char) * OY_MIN(len_in,len_out));
    output[len_out] = 0;
#ifndef USE_ICONV_H
    if(!src)
      return error;

    /* cheap fallback for UTF-16 to ASCII */
    if(strcmp(src,"UTF-16BE") == 0 ||
       strcmp(src,"UTF-16LE") == 0)
    {
      int i;
      int low_byte = strcmp(src,"UTF-16BE") == 0;

      for(i = 0 ; i < len_in && i < len_out; i += 2)
        output[i/2] = input[i+low_byte];

      output[i/2] = 0;
    }
#endif
    return error;
  }

#ifdef USE_ICONV_H
  cd = iconv_open( loc, src );
  size = iconv( cd, &in_txt, &in_left, &out_txt, &out_left);
  iconv_close( cd );
  *out_txt = 0;

  if(size == (size_t)-1)
    error = -1;
  else
    error = size;
#endif

  return error;
}

/** @func  oyIconvGet
 *  @brief convert between codesets
 *
 *  @version Oyranos: 0.1.13
 *  @date    2010/10/24
 *  @since   2010/10723 (Oyranos: 0.1.13)
 */
int          oyIconvGet              ( const char        * text,
                                       void             ** string,
                                       int               * len,
                                       const char        * encoding_from,
                                       const char        * encoding_to,
                                       oyAlloc_f           alloc )
{
  int error = 0;
    *len = strlen(text) * 4 + 4;
    *string = alloc( *len );
    memset( *string, 0, *len );
    error = oyIconv( text, strlen(text), *len-2, *string,
                     encoding_from, encoding_to );
    if(error)
      WARNc2_S("something went wrong. %s:%d\n", text, *len);
    return error;
}


