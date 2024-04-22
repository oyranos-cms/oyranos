/** @file oyjl_string.c
 *
 *  oyjl - string helpers
 *
 *  @par Copyright:
 *            2016-2023 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl core functions
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *
 * Copyright (c) 2004-2023  Kai-Uwe Behrmann  <ku.b@gmx.de>
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

#include <ctype.h>   /* isspace() tolower() isdigit() */
#include <math.h>    /* NAN */
#include <stdarg.h>  /* va_list */
#include <stddef.h>  /* ptrdiff_t size_t */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>   /* time_t localtime() */
#include <unistd.h>
#include <errno.h>
#include <wchar.h>  /* wcslen() */

#include "oyjl.h"
#include "oyjl_macros.h"
#include "oyjl_version.h"
#include "oyjl_tree_internal.h"
#ifdef OYJL_HAVE_LOCALE_H
#include <locale.h>           /* setlocale LC_NUMERIC */
#endif
#ifdef OYJL_HAVE_REGEX_H
# include <regex.h>
#endif

/* --- String_Section --- */

/** \addtogroup oyjl_core
 *  @{ *//* oyjl_core */
/** \addtogroup oyjl_string
 *  @{ *//* oyjl_string */

void       oyjlStr_SetCharAt         ( oyjl_str            str,
                                       char                c,
                                       int                 pos );

/* return the beginning of the next word */
static const char * oyjlStringGetNext_( const char        * text )
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

/**  @internal
 *   Assume some text and extract the found words.
 *   The words can be separated by white space as 
 *   seen by isspace().
 */
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
    while( tmp && tmp[0] && (tmp = oyjlStringGetNext_( tmp )) != NULL ) ++n;

    if((list = (char**) alloc( (n+1) * sizeof(char*) )) == 0) return NULL;
    memset( list, 0, (n+1) * sizeof(char*) );

    {
      const char * start = text;
      if(start && isspace(start[0]))
        start = oyjlStringGetNext_( start );

      for(i = 0; i < n; ++i)
      {
        int len = oyjlStringNextSpace_( start );

        if((list[i] = (char*) alloc( len+1 )) == 0) return NULL;

        memcpy( list[i], start, len );
        list[i][len] = 0;
        start = oyjlStringGetNext_( start );
      }
    }
  }

  if(count)
    *count = n;

  return list;
}

/* length is the length of the delimiter found in text */
const char * oyjlStringDelimiter ( const char * text, const char * delimiter, int * length )
{
  int i,j, dn = delimiter ? strlen(delimiter) : 0, len = text?strlen(text):0;
  for(j = 0; j < len; ++j)
    for(i = 0; i < dn; ++i)
      if(text[j] && text[j] == delimiter[i])
      {
        if(length)
          *length = 1;
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
  return oyjlStringSplit2( text, d, oyjlStringDelimiter, count, NULL, alloc );
}

/** @brief   convert a string into list
 *
 *  @param[in]     text                source string
 *  @param[in]     delimiter           the ASCII char which marks the split;
 *                                     e.g. comma ","; optional;
 *                                     default zero: extract white space separated words
 *  @param[in]     splitFunc           function for splitting, default is
 *                                     oyjlStringDelimiter(); optional
 *  @param[out]    count               number of detected string segments; optional
 *  @param[out]    index               to be allocated array of detected delimiter indexes; The array will contain the list of indexes in text, which lead to the actual split positional index.; optional
 *  @param[in]     alloc               custom allocator; optional, default is malloc
 *  @return                            array of detected string segments
 */
char **        oyjlStringSplit2      ( const char        * text,
                                       const char        * delimiter,
                                       const char        *(splitFunc)( const char * text, const char * delimiter, int * length ),
                                       int               * count,
                                       int              ** index,
                                       void*            (* alloc)(size_t))
{
  char ** list = 0;
  int n = 0, i;

  if(!splitFunc)
    splitFunc = oyjlStringDelimiter;

  /* split the path search string by a delimiter */
  if(text && text[0])
  {
    const char * tmp = text;

    if(!alloc) alloc = malloc;

    if(!delimiter || !delimiter[0])
      return oyjlStringSplitSpace_( text, count, alloc );

    tmp = splitFunc(tmp, delimiter, NULL);
    if(tmp == text) ++n;
    tmp = text;
    do { ++n; } while( (tmp = splitFunc(tmp + 1, delimiter, NULL)) );

    tmp = 0;

    if((list = alloc( (n+1) * sizeof(char*) )) == NULL) return NULL;
    memset( list, 0, (n+1) * sizeof(char*) );
    if(index && (*index = alloc( n * sizeof(int) )) == NULL) { free(list); return NULL; }
    if(index) memset( *index, 0, n * sizeof(int) );

    {
      const char * start = text;
      for(i = 0; i < n; ++i)
      {
        char * t = NULL;
        intptr_t len = 0;
        int length = 0,
            pos;
        const char * end = splitFunc(start, delimiter, &length);
        pos = end - text;
        if(index && length) (*index)[i] = pos;

        if(end > start)
          len = end - start;
        else if (end == start)
          len = 0;
        else
          len = strlen(start);

        if((t = alloc( len+1 )) == 0) return NULL;

        memcpy( t, start, len );
        t[len] = 0;
        list[i] = t; t = NULL;
        start += len + length;
      }
    }
  }

  if(count)
    *count = n;

  return list;
}

/** @brief    Match a string from pattern 
 * 
 *  @param[in]     text                text string
 *  @param[in]     pattern             text to search for. If empty it will return the matched string count; optional
 *  @param[in]     flags               flags for behaviour; default is ::OYJL_COMPARE_EXACT match with strcmp() == 0
 *                                     - ::OYJL_COMPARE_CASE case independent compare function (default: exact strcmp)
 *                                     - ::OYJL_COMPARE_LAZY search for pattern in sub string fron separated list using strstr()
 *                                     - ::OYJL_COMPARE_STARTS_WITH search for sub string using oyjlStringStartsWith()
 *                                     - ::OYJL_REGEXP regex compare using oyjlRegExpFind()
 *                                     - ::OYJL_REVERSE swap the matching arguments and try to find a match from set for pattern
 *  @return                            result,
 *                                     -  0 - for not found
 *                                     -  1 - for matched
 *                                     - -1 on issue like text or pattern args are missed
 * */
int        oyjlStringFind            ( const char        * text,
                                       const char        * pattern,
                                       int                 flags )
{
  const char * search = pattern;
  int found = 0;

  if(!(text && text[0]) || !(pattern && pattern[0]))
  {
    found = -1;
    return found;
  }

  if(flags & OYJL_REVERSE)
  {
    search = text;
    text = pattern;
  }

  if(flags & OYJL_COMPARE_STARTS_WITH)
  {
    if(oyjlStringStartsWith(text, search, flags))
      found = 1;
  }
  else if(flags & OYJL_COMPARE_CASE)
  {
    if(strcasecmp(text, search) == 0)
      found = 1;
  }
  else if(flags & OYJL_COMPARE_FIND_NEEDLE)
  {
    if(strstr(text, search) != NULL)
      found = 1;
  }
  else if(flags & OYJL_COMPARE_LAZY)
  {
    char * t_low = oyjlStringToLower_(text),
         * s_low = oyjlStringToLower_(search);
    if(strstr(t_low, s_low) != NULL)
      found = 1;
    free(t_low); free(s_low);
  }
  else if(flags & OYJL_REGEXP)
  {
    if(oyjlRegExpFind((char*)text, search, NULL) != NULL)
      found = 1;
  }
  else
    if(strcmp(text, search) == 0)
      found = 1;

  return found;
}

/** @brief    Find a string in a list
 * 
 *  @param[in]     list                text string list
 *  @param[in]     pattern             text to search for. If empty it will return the matched string count; optional
 *  @param[in]     flags               flags for behaviour
 *                                     - ::OYJL_REMOVE remove matched part from list
 *                                     - ::OYJL_COMPARE_CASE case independent compare function (default: exact strcmp)
 *                                     - ::OYJL_COMPARE_FIND_NEEDLE search for pattern in sub string fron separated list using strstr()
 *                                     - ::OYJL_COMPARE_LAZY search for pattern in sub string fron separated list using strstr() all lower case
 *                                     - ::OYJL_COMPARE_STARTS_WITH search for sub string using oyjlStringStartsWith()
 *                                     - ::OYJL_REGEXP regex compare using oyjlRegExpFind()
 *                                     - ::OYJL_REVERSE swap the matching arguments and try to find a match from set for pattern
 *  @param[out]    match               result depending on flags; optional
 *  @return                            index of pattern match in list, or count with OYJL_REMOVE
 *                                     - -1 if nothing is found
 * */
int        oyjlStringListFind        ( char             ** list,
                                       int               * list_n,
                                       const char        * pattern,
                                       int                 flags,
                                       void             (* deAlloc)(void*) )
{
  int found = -1, i, n = *list_n;
  if(deAlloc == 0) deAlloc = free;

  if(flags & OYJL_REMOVE) found = 0;

  for( i = 0; i  < n; ++i )
  {
    const char * text = list[i];
    int pos = -1;

    if(oyjlStringFind(text, pattern, flags) == 1)
      pos = i;

    if(flags & OYJL_REMOVE)
    {
      if(pos != -1)
      {
        oyjlStringListFree( list, list_n, pos, 1, deAlloc );
        ++found;
      }
    } else
    {
      if( pos != -1 )
      {
        found = pos;
        break;
      }
    }
  }

  return found;
}

/** @brief    Find a sub string in a delimited list
 * 
 *  @param[in,out] set                 text string for oyjlStringSplit2
 *  @param[in]     delimiters          one ore more separating chars or empty for empty space
 *  @param[in]     pattern             text to search for. If empty it will return the sub string count; optional
 *  @param[in]     flags               flags for behaviour
 *                                     - ::OYJL_REMOVE remove matched part from list (creates new set)
 *                                     - ::OYJL_TO_JSON convert to list in JSON array format, can be combined with OYJL_NO_MARKUP
 *                                     - ::OYJL_TO_TEXT convert to string list with highlighted matches
 *                                     - ::OYJL_MARK convert to marked list in JSON format
 *                                     - ::OYJL_COMPARE_CASE case independent compare function (default: exact strcmp)
 *                                     - ::OYJL_COMPARE_FIND_NEEDLE search for pattern in sub string fron separated list using strstr()
 *                                     - ::OYJL_COMPARE_LAZY search for pattern in sub string fron separated list using strstr() all lower case
 *                                     - ::OYJL_COMPARE_STARTS_WITH search for sub string using oyjlStringStartsWith()
 *                                     - ::OYJL_REGEXP regex compare using oyjlRegExpFind()
 *                                     - ::OYJL_REVERSE swap the matching arguments and try to find a match from set for pattern
 *  @param[out]    result              results depending on flags; optional
 *  @param[in]     alloc               custom malloc; optional
 *  @param[in]     deAlloc             custom free; optional
 *  @return                            index of pattern match in list,
 *                                     - -1 if nothing is found
 * */
int        oyjlStringSplitFind       ( const char        * set,
                                       const char        * delimiters,
                                       const char        * pattern,
                                       int                 flags,
                                       char             ** result,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) )
{
  int found = -1;
  char ** list;
  int i, n;

  if(!alloc) alloc = malloc;
  if(!deAlloc) deAlloc = free;

  if(set && set[0])
  {
    n = 0;
    list = oyjlStringSplit2( set, delimiters, 0, &n, NULL, alloc );
    if(!pattern)
      found = n;
    else
    {
      char * new_set = NULL;
      for( i = 0; i  < n; ++i )
      {
        const char * text = list[i];
        int pos = -1;

        if(oyjlStringFind(text, pattern, flags) == 1)
        {
          pos = found = i;
          if(*oyjl_debug >= 2)
            fprintf( stderr, OYJL_DBG_FORMAT "%s found inside %s\n", OYJL_DBG_ARGS, pattern, set );
        }
 
        if(flags & OYJL_TO_JSON && !(pos == -1 && flags & OYJL_REMOVE))
          oyjlStringAdd( &new_set, alloc,deAlloc, "%s\"%s\"", i&&new_set?", ":"", oyjlTermColor(flags&OYJL_NO_MARKUP?oyjlNO_MARK:oyjlRED,text) );
        if(flags & OYJL_TO_TEXT && !(pos == -1 && flags & OYJL_REMOVE))
        {
          char sep[4] = {delimiters?delimiters[0]:' ',0,0,0};
          oyjlStringAdd( &new_set, alloc,deAlloc,0, "%s%s", i&&new_set?sep:"", oyjlTermColor(flags&OYJL_NO_MARKUP?oyjlNO_MARK:oyjlRED,text) );
        }
        else if(pos == -1 && flags & OYJL_REMOVE)
          oyjlStringAdd( &new_set, alloc,deAlloc,0, "%s%s", i&&new_set?",":"", text );
        else if(!(flags & OYJL_REMOVE) && result && new_set == NULL && *result == NULL && pos != -1)
          *result = oyjlStringCopy( text, alloc );
      }

      if(flags & OYJL_REMOVE)
      { *result = new_set; }
      else
      { deAlloc(new_set); new_set = NULL; }
    }
    oyjlStringListRelease( &list, n, deAlloc );
  }

  return found;
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

  if(string && *string && text)
  {
    int l = strlen(*string),
        l2 = strlen(text);
    text_copy = (char*)allocate( l2 + l + 1 );
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
  if(!deAlloc && !alloc)
    deAlloc = free;

  text_copy = oyjlStringAppendN(*text, append, append_len, alloc);

  if(*text && deAlloc)
    deAlloc(*text);

  *text = text_copy;

  return;
}

void       oyjlStringPush            ( char             ** text,
                                       const char        * append,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) )
{
  oyjlStringAddN(text, append, strlen(append), alloc, deAlloc );
}

void       oyjlStringPrepend         ( char             ** text,
                                       const char        * prepend,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) )
{
  char * t = NULL;
  t = oyjlStringCopy( prepend, alloc );
  if(*text)
  {
    oyjlStringAddN(&t, *text, strlen(*text), alloc, deAlloc );
    if(deAlloc) deAlloc(*text); else free(*text);
  }
  *text = t; t = NULL;
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

  str = oyjlStr_NewFrom(text, 0, alloc,deAlloc);
  n = oyjlStr_Replace( str, search, replacement, 0, NULL );
  t = oyjlStr_Pull(str);
  *text = t;
  oyjlStr_Release( &str );

  return n;
}

extern char * oyjl_term_color_html_;
const char * oyjlStringColor         ( oyjlTEXTMARK_e      mark,
                                       int                 flags,
                                       const char        * format,
                                                           ... )
{
  char * tmp = NULL;
  const char * t = NULL,
             * text = format;

  if(strchr(format, '%'))
  { OYJL_CREATE_VA_STRING(format, tmp, malloc, return NULL)
    text = tmp;
  }

  if(flags & OYJL_HTML)
  {
    if(oyjl_term_color_html_) free(oyjl_term_color_html_);
    oyjl_term_color_html_ = NULL;
    switch(mark)
    {
      case oyjlNO_MARK: oyjlStringAdd( &oyjl_term_color_html_, 0,0, "%s", text ); break;
      case oyjlRED: oyjlStringAdd( &oyjl_term_color_html_, 0,0, "<font color=red>%s</font>", text ); break;
      case oyjlGREEN: oyjlStringAdd( &oyjl_term_color_html_, 0,0, "<font color=green>%s</font>", text ); break;
      case oyjlBLUE: oyjlStringAdd( &oyjl_term_color_html_, 0,0, "<font color=blue>%s</font>", text ); break;
      case oyjlBOLD: oyjlStringAdd( &oyjl_term_color_html_, 0,0, "<strong>%s</strong>", text ); break;
      case oyjlITALIC: oyjlStringAdd( &oyjl_term_color_html_, 0,0, "<em>%s</em>", text ); break;
      case oyjlUNDERLINE: oyjlStringAdd( &oyjl_term_color_html_, 0,0, "<u>%s</u>", text ); break;
    }
    t = oyjl_term_color_html_;
  }
  else
    t = oyjlTermColor( mark, text );

  if(tmp) free(tmp);
  return t;
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
void       oyjlStringListRelease     ( char            *** l,
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
        {
          (list)[i][0] = '?';
          deAlloc( (list)[i] );
          (list)[i] = NULL;
        }
      deAlloc( list );
      *l = NULL;
    }
  }
}

/** @brief append a string to a string list */
void       oyjlStringListPush        ( char            *** list,
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

  if(*list)
    memmove( nlist, *list, sizeof(char*) * n_alt);
  nlist[n_alt] = oyjlStringCopy( string, alloc );
  nlist[n_alt+1] = NULL;

  *n = n_alt + 1;

  if(*list)
    deAlloc(*list);

  *list = nlist;
}

/** @brief add a variable string to a string list */
int        oyjlStringListAdd         ( char            *** list,
                                       int               * n,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*),
                                       const char        * format,
                                                           ... )
{
  char * text = 0;

  void (* deAllocate)(void * data ) = deAlloc?deAlloc:free;

  OYJL_CREATE_VA_STRING(format, text, alloc, return 1)

  if(text)
  {
    oyjlStringListPush( list, n, text, alloc, deAlloc );
    deAllocate( text );
  }

  return 0;
}

/** @brief modify a string list */
void       oyjlStringListDo          ( char             ** list,
                                       int                 n,
                                       const char        * string,
                                       void             (* listDo)(char**,const char*, void*(*)(size_t),void(*)(void*)),
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) )
{
  int i;
  for(i=0; i < n; ++i)
  {
    char * text = list[i];
    listDo( &text, string, alloc, deAlloc );
    list[i] = text;
  }
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

/** @brief   remove elements from array
 *
 *  @version Oyjl: 1.0.0
 *  @date    2024/03/22
 *  @since   2024/03/22 (Oyjl: 1.0.0)
 */
void       oyjlStringListFree        ( char             ** list,
                                       int               * list_n,
                                       int                 start,
                                       int                 count,
                                       void             (* deAlloc)(void*) )
{
  int i, n, tail;
  if(deAlloc == 0) deAlloc = free;

  if(!list || !*list || !list_n || !(*list_n) || start >= (*list_n) -1 || count <= 0)
    return;

  n = *list_n;
  if(start + count > n)
    count = n - start;
  tail = n - start - count;

  for( i = start; i < start + count; ++i )
  {
    if(list[i])
      deAlloc( list[i] );
    list[i] = NULL;
    --(*list_n);
  }

  if(count)
  {
    memmove( &list[start], &list[start+count], sizeof(char*) * tail );
    list[ start + count + 1 ] = NULL;
  }
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
 *  @param[in]     text                string
 *  @param[out]    value               resulting number; optional
 *  @param[out]    end                 possibly part after number; optional
 *  @return                            error
 *                                     - 0 : text input was completely read as number
 *                                     - -1 : text input was read as number with white space or other text after; can be seen in end argument
 *                                     - 1 : missed text input
 *                                     - 2 : no number detected
 *
 */
int      oyjlStringToLong            ( const char        * text,
                                       long              * value,
                                       const char       ** end )
{
  char * end_ = 0;
  int error = -1;
  long l;
  l = strtol( text, &end_, 0 );
  if(value)
    *value = l;
  if(end_ && end_ != text && isdigit(text[0]) && !isdigit(end_[0]) )
  {
    if(end_[0] && end_ != text)
    {
      error = -1;
      if(end)
        *end = end_;
    }
    else
      error = 0;
  }
  else
    error = 1;
  return error;
}

/** @brief   text to double conversion
 *
 *  @param[in]     text                string
 *  @param[out]    value               resulting number
 *  @param[out]    end                 possibly part after number
 *  @param[in]     flags               OYJL_KEEP_LOCALE; default is to switch actual LC_NUMERIC to "C"
 *  @return                            error
 *                                     - 0 : text input was completely read as number
 *                                     - -1 : text input was read as number with white space or other text after; can be seen in end argument
 *                                     - 1 : missed text input
 *                                     - 2 : no number detected
 *
 *  @version Oyjl: 1.0.0
 *  @date    2022/04/17
 *  @since   2011/11/17 (Oyranos: 0.2.0)
 */
int          oyjlStringToDouble      ( const char        * text,
                                       double            * value,
                                       const char       ** end,
                                       int                 flags )
{
  char * end_ = NULL, * t = NULL;
  int len, pos = 0;
  int error = -1;
#ifdef OYJL_HAVE_LOCALE_H
  char * save_locale = oyjlStringCopy( setlocale(LC_NUMERIC, 0 ), malloc );
  if(!(flags & OYJL_KEEP_LOCALE))
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

  *value = strtod( t, &end_ );

  if(end_ && end_ != text && isdigit(text[0]) && !isdigit(end_[0]) )
  {
    if(end_[0] && end_ != text)
    {
      error = -1;
      if(end)
      {
        end_ = strstr( text, end_ );
        *end = end_;
      }
    }
    else
      error = 0;
  }
  else if(end_ && end_ == t)
  {
    *value = NAN;
    error = 2;
  }

clean_oyjlStringToDouble:

#ifdef OYJL_HAVE_LOCALE_H
  if(!(flags & OYJL_KEEP_LOCALE))
    setlocale(LC_NUMERIC, save_locale);
  if(save_locale) free( save_locale );
#endif

  if(t) free( t );

  return error;
}

/** @brief   text to double list
 *
 *  @param[in]     text                source string
 *  @param[in]     delimiter           the ASCII char(s) which mark the split;
 *                                     e.g. comma ","
 *  @param[out]    count               number of detected string segments; optional
 *  @param[in]     alloc               custom allocator; optional, default is malloc
 *  @param[out]    value               array of detected number of count elements
 *  @return                            error
 *                                     - 0 : text input was completely read as
 *                                           number
 *                                     - -1 : text input was read as number with
 *                                           white space or other text after
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

  list = oyjlStringSplit2( text, delimiter, oyjlStringDelimiter, &n, NULL, alloc );
  if(n)
    oyjlAllocHelper_m( *value, double, n + 1, alloc, return 1);
  for( i = 0; i < n; ++i )
  {
    val = list[i];
    l_error = oyjlStringToDouble( val, &d, 0,0 );
    (*value)[i] = d;
    if(!error || l_error > 0) error = l_error;
    if(l_error > 0) break;
  }
  if(count)
    *count = n;

  oyjlStringListRelease(&list, n, free);

  return error;
}

/** @brief   search for pattern
 *
 *  This function behaves like strstr(), but extends to regular expressions.
 *  Test for OYJL_HAVE_REGEX_H macro to see if regexec() API is
 *  used. Otherwise only C strstr() API will be called.
 *
 *  @param         text                string to search in
 *  @param         regex               regular expression to try with text
 *  @param         length              length of returned value
 *  @return                            result:
 *                                     - 0: no match
 *                                     - >0: first string adress in text
 *
 *  @version Oyjl: 1.0.0
 *  @date    2022/10/19
 *  @since   2020/07/28 (Oyjl: 1.0.0)
 */
char *     oyjlRegExpFind            ( char              * text,
                                       const char        * regex,
                                       int               * length )
{
  char * match = NULL;
  if( !text || !regex )
    return match;

#ifdef OYJL_HAVE_REGEX_H
  int status = 0;
  regex_t re;
  regmatch_t re_match = {0,0};
  int error = 0;
  if((error = regcomp(&re, regex, REG_EXTENDED)) != 0)
  {
    char * err_msg = calloc( 1024, sizeof(char) );
    regerror( error, &re, err_msg, 1024 );
    oyjlMessage_p( oyjlMSG_INFO, 0,
                   OYJL_DBG_FORMAT "regcomp(\"%s\") %s",
                   OYJL_DBG_ARGS,  regex, err_msg );
    if(err_msg) free(err_msg);
    return 0;
  }
  status = regexec( &re, text, (size_t)1, &re_match, 0 );
  regfree( &re );
  if(status == 0 && re_match.rm_so != -1)
  {
    match = &text[re_match.rm_so];
    if(length)
      *length = re_match.rm_eo - re_match.rm_so;
  }
#endif

  if(match == NULL)
  {
    match = strstr(text, regex);
    if(length)
      *length = strlen(regex);
  }

  return match;
}

/* length is the length of the delimiter found in text */
const char * oyjlRegExpDelimiter ( const char * text, const char * delimiter, int * length )
{
  int len = 0;
  const char * pos = oyjlRegExpFind( (char*)text, delimiter, &len );
  if(pos)
  {
    if(length)
      *length = len;
    return pos;
  }
  return NULL;
}

/** @brief   replace pattern
 *
 *  Test for OYJL_HAVE_REGEX_H macro to see if regexec() API is
 *  used.
 *
 *  @param         text                string to search in
 *  @param         regex               regular expression to try with text
 *  @param         replacement         substitute all matches of regex in text; it may contain up to two occurences of %s regex matching placeholders, e.g. "%s=%s"
 *  @return                            count of replacements
 *
 *  @version Oyjl: 1.0.0
 *  @date    2022/10/21
 *  @since   2021/09/29 (Oyjl: 1.0.0)
 */
int        oyjlRegExpReplace         ( char             ** text,
                                       const char        * regex,
                                       const char        * replacement )
{
  int count = 0;
  if( !text || !*text || !regex || !replacement )
    return 0;

#ifdef OYJL_HAVE_REGEX_H
  int status = 0;
  regex_t re;
  int n;
  regmatch_t re_match = {0,0};
  oyjl_str str;
  int error = 0;
  if((error = regcomp(&re, regex, REG_EXTENDED)) != 0)
  {
    char * err_msg = calloc( 1024, sizeof(char) );
    regerror( error, &re, err_msg, 1024 );
    oyjlMessage_p( oyjlMSG_INFO, 0,
                   OYJL_DBG_FORMAT "regcomp(\"%s\") %s",
                   OYJL_DBG_ARGS,  regex, err_msg );
    if(err_msg) free(err_msg);
    return 0;
  }

  if(strstr(replacement,"%s"))
  {
    const char * txt;
    int pos = 0,
        replace_count = strstr(strstr(replacement,"%s"),"%s") ? 2 : 1,
        replacement_len = strlen(replacement) - 2*replace_count;
    str = oyjlStr_NewFrom(text, 0, malloc, free);
    n = re.re_nsub;
    txt = oyjlStr(str);
    while((status = regexec( &re, txt, (size_t)1, &re_match, 0 )) == 0)
    {
      int len, found_len;
      char * tail = NULL,
           * found_string;
      len = strlen(txt);
      found_string = oyjlStringCopy( &txt[re_match.rm_so], NULL );
      found_string[re_match.rm_eo - re_match.rm_so] = '\000';
      found_len = strlen(found_string);
      pos += re_match.rm_so;
      if(len >= re_match.rm_eo || re_match.rm_so == 0)
      {
        tail = oyjlStringCopy( &txt[re_match.rm_eo], 0 );
        oyjlStr_SetCharAt( str, '\000', pos );
      }
      if(replace_count == 1)
        oyjlStr_Add( str, replacement, found_string );
      else if(replace_count == 2)
        oyjlStr_Add( str, replacement, found_string, found_string );
      pos += found_len*replace_count + replacement_len;
      free(found_string);
      if(len > re_match.rm_eo)
      {
        oyjlStr_Push( str, tail );
      }
      if(tail) { free(tail); tail = NULL; }
      txt = oyjlStr(str);
      txt += pos;
      ++count;
    }
    regfree( &re );
    *text = oyjlStr_Pull( str );
    oyjlStr_Release( &str );
  }
  else
  {
    const char * txt;
    str = oyjlStr_NewFrom(text, 0, malloc, free);
    n = re.re_nsub;
    txt = oyjlStr(str);
    while((status = regexec( &re, txt, (size_t)1, &re_match, 0 )) == 0)
    {
      int len = strlen(txt);
      char * tail = NULL;
      if(len > re_match.rm_eo)
        tail = oyjlStringCopy( &txt[re_match.rm_eo], 0 );
      n = oyjlStr_Replace( str, &txt[re_match.rm_so], replacement, 0, NULL );
      if(len > re_match.rm_eo)
      {
        oyjlStr_Push( str, tail );
        free(tail);
      }
      txt = oyjlStr(str);
      ++count;
      if(!n) break;
    }
    regfree( &re );
    *text = oyjlStr_Pull( str );
    oyjlStr_Release( &str );
  }

#else

  count = oyjlStringReplace(&text, regex, replacement, 0,0);

#endif
  return count;
}


void oyjlNoBracketCb_(const char * text OYJL_UNUSED, const char * start, const char * end, const char * search, const char ** replace, int * r_len OYJL_UNUSED, void * data OYJL_UNUSED)
{
  if(start < end)
  {
    const char * word = start;
    while(word && (word = strstr(word+1,"\\[")) != NULL && word < end)
      *replace = search;
  }
}
void       oyjlRegExpEscape2_        ( oyjl_str            text )
{
  oyjl_str tmp = text;
  if(!text) return;

#ifdef OYJL_HAVE_REGEX_H
  oyjlStr_Replace( tmp, "\\", "\\\\", oyjlNoBracketCb_, NULL );
  oyjlStr_Replace( tmp, ".", "\\.", 0, NULL );
  oyjlStr_Replace( tmp, "^", "\\^", 0, NULL );
  oyjlStr_Replace( tmp, "$", "\\$", 0, NULL );
  oyjlStr_Replace( tmp, "*", "\\*", 0, NULL );
  oyjlStr_Replace( tmp, "+", "\\+", 0, NULL );
  oyjlStr_Replace( tmp, "?", "\\?", 0, NULL );
  //oyjlStr_Replace( tmp, "!", "\\!", 0, NULL );
  oyjlStr_Replace( tmp, "(", "\\(", 0, NULL );
  oyjlStr_Replace( tmp, ")", "\\)", 0, NULL );
  oyjlStr_Replace( tmp, "[", "\\[", 0, NULL );
  //oyjlStr_Replace( tmp, "]", "\\]", 0, NULL );
  oyjlStr_Replace( tmp, "{", "\\{", 0, NULL );
  //oyjlStr_Replace( tmp, "}", "\\}", 0, NULL );
  //oyjlStr_Replace( tmp, ",", "\\,", 0, NULL );
  oyjlStr_Replace( tmp, "|", "\\|", 0, NULL );
#endif
}

/** @brief   use a pattern literaly
 *
 *  This function detects OYJL_HAVE_REGEX_H macro internally to
 *  fit the oyjlRegExpMatch() implementation.
 *
 *  It is escaping: \n
 *  .$*+?()[{\|
 *
 *  @param         text                string to escape
 *  @return                            escaped string
 *
 *  @version Oyjl: 1.0.0
 *  @date    2020/08/14
 *  @since   2020/08/14 (Oyjl: 1.0.0)
 */
char *     oyjlRegExpEscape          ( const char        * text )
{
  char * out = NULL;
  const char * t = text;
  oyjl_str tmp;
  if(!text) return NULL;

  tmp = oyjlStr_New(10,0,0);
  oyjlStr_Push( tmp, t );
  oyjlRegExpEscape2_( tmp );
  out = oyjlStr_Pull(tmp); 
  oyjlStr_Release( &tmp );
  return out;
}

/** @brief   simple and fast test
 *
 *  @param         text                string to be searched in
 *  @param         pattern             string to search in text
 *  @param         flags               ::OYJL_COMPARE_CASE
 *  @return                            result is match
 *
 *  @version Oyjl: 1.0.0
 *  @date    2023/05/21
 *  @since   2023/05/21 (Oyjl: 1.0.0)
 */
int oyjlStringStartsWith             ( const char        * text,
                                       const char        * pattern,
                                       int                 flags )
{
  int text_len = text ? strlen( text ) : 0,
      pattern_len = pattern ? strlen( pattern ) : 0;

  if(text_len && text_len >= pattern_len)
  {
    if(flags & OYJL_COMPARE_CASE)
    {
      char * t = oyjlStringCopy( text, 0 );
      int match = 0;
      if(t)
      {
        t[pattern_len] = '\000';
        match = strcasecmp(t,pattern) == 0;
        free(t);
      }
      return match;
    }
    else
      if(memcmp(text, pattern, pattern_len) == 0)
        return 1;
  }

  return 0;
}


/*
* Index into the table below with the first byte of a UTF-8 sequence to
* get the number of trailing bytes that are supposed to follow it.
* Note that *legal* UTF-8 values can't have 4 or 5-bytes. The table is
* left as-is for anyone who may want to do such conversion, which was
* allowed in earlier algorithms.
*/
static const char oyjlTrailingBytesForUTF8_[256] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

/** @brief   split letters of a UTF-8 string
 *
 *  @param[in]     text                source string in UTF-8 format
 *  @param[out]    mbchars             NULL terminated array of count letters in UTF-8 format; optional
 *  @param[in]     alloc               custom allocator; optional, default is malloc
 *  @return                            count of letters
 *
 *  @version Oyjl: 1.0.0
 *  @date    2020/04/23
 *  @since   2020/04/23 (Oyjl: 1.0.0)
 */
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
    int trailing_bytes = oyjlTrailingBytesForUTF8_[c];
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
 *  @date    2021/10/24
 *  @since   2019/02/14 (Oyjl: 1.0.0)
 */
oyjl_str   oyjlStr_New               ( size_t              length,
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
 *  @date    2021/10/24
 *  @since   2019/02/15 (Oyjl: 1.0.0)
 */
oyjl_str   oyjlStr_NewFrom           ( char             ** text,
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

/** @brief   set the char at position
 *
 *  @param[in,out] str                 the object, which will be modified
 *  @return                            
 *
 *  @version Oyjl: 1.0.0
 *  @date    2022/10/19
 *  @since   2022/10/19 (Oyjl: 1.0.0)
 */
void       oyjlStr_SetCharAt         ( oyjl_str            str,
                                       char                c,
                                       int                 pos )
{
  struct oyjl_string_s * string = str;

  if(!str) return;

  string->s[pos] = c;
  if(c == '\000')
    string->len = pos;
}

/** @brief   fast append to the string end
 *
 *  @param[in]     string              string object
 *  @param[in]     append              to be added text to string
 *  @param[in]     append_len          length of append
 *  @return                            error
 */
int        oyjlStr_AppendN           ( oyjl_str            string,
                                       const char        * append,
                                       int                 append_len )
{
  struct oyjl_string_s * str = string;
  int error = 0;
  if(append && append_len)
  {
    if(str->alloc_len == 0 || (append_len + str->len) >= str->alloc_len - 1)
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
    str->s[str->len] = 0;
  }
  return error;
}
/** @brief   fast append to the string end
 *
 *  @param[in]     string              string object
 *  @param[in]     text                to be added text to string
 *  @return                            error
 */
int        oyjlStr_Push              ( oyjl_str            string,
                                       const char        * text )
{
  return oyjlStr_AppendN( string, text, strlen(text) );
}

/* slow - just for demonstration */
int        oyjlStr_Append            ( oyjl_str            string,
                                       const char        * append )
{
  struct oyjl_string_s * str = string;
  int error = 0;
  int append_len = append?strlen(append):0;
  if(append && append_len)
  {
    if((append_len + str->len) >= str->alloc_len - (str->alloc_len?1:0))
    {
      int len = (append_len + str->len) * 2;
      char * edit = str->s;
      oyjlAllocHelper_m( str->s, char, len, str->alloc, return 1 );
      str->alloc_len = len;
      ++str->alloc_count;
      if(edit)
      {
        strcpy(str->s, edit);
        str->deAlloc(edit);
      }
    }
    strcpy( &str->s[str->len], append );
    str->len = strlen(str->s);
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
int        oyjlStr_Add               ( oyjl_str            string,
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
    oyjlStr_Push( string, text );
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
 *                                     - start: current start inside text; on first occurence it provides &text[0]
 *                                     - end: current end inside text; the actual occurence of search in text
 *                                     - search: used term
 *                                     - replace: possibly modified replacement text for search
 *                                     - replace_len: length of original replace; can be changed
 *                                     - context: user data
 *  @param[in,out] user_data           optional user data for modifyReplacement
 *  @return                            number of occurences
 *
 *  @version Oyjl: 1.0.0
 *  @date    2022/09/04
 *  @since   2019/02/15 (Oyjl: 1.0.0)
 */
int        oyjlStr_Replace           ( oyjl_str            text,
                                       const char        * search,
                                       const char        * replacement,
                                       void             (* modifyReplacement)
                                                             (const char * text,
                                                              const char * start,
                                                              const char * end,
                                                              const char * search,
                                                              const char ** replace,
                                                              int * replace_len,
                                                              void * user_data),
                                       void              * user_data )
{
  struct oyjl_string_s * str = text;
  oyjl_str t = NULL;
  const char * start, * end, * last;
  int n = 0;

  if(!text)
    return 0;

  start = end = oyjlStr(text);

  if(start && search && replacement)
  {
    int s_len = strlen(search);
    last = start + strlen(start);
    while((end = strstr(start,search)) != 0)
    {
      if(!t) t = oyjlStr_New(10,0,0);
      oyjlStr_AppendN( t, start, end-start );
      if(modifyReplacement) modifyReplacement( oyjlStr(text), start, end, search, &replacement, &s_len, user_data );
      oyjlStr_Push( t, replacement );
      ++n;
      if((end + s_len) <= last)
        start = end + s_len;
      else
      {
        if(strstr(start,search) != 0)
          oyjlStr_Push( t, replacement );
        start = end = end + s_len;
        break;
      }
    }
    if(n && start && end == NULL)
      oyjlStr_Push( t, start );
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
      oyjlStr_Push( str, oyjlStr(t) );
      oyjlStr_Release( &t );
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
 *  @date    2021/10/24
 *  @since   2019/02/15 (Oyjl: 1.0.0)
 */
char *     oyjlStr_Pull              ( oyjl_str            str )
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
 *  @date    2021/10/24
 *  @since   2019/06/14 (Oyjl: 1.0.0)
 */
void       oyjlStr_Clear             ( oyjl_str            string )
{
  struct oyjl_string_s * str = string;
  void (* deAlloc)(void*) = str->deAlloc;
  char * s = str->s;
  str->s = NULL;
  if(s) deAlloc(s);
  str->len = 0;
  str->alloc_len = 0;
  str->alloc_count = 0;
}

/** @brief   release a string object
 *
 *  All references from previous oyjlStr() calls will be void.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/10/24
 *  @since   2019/02/14 (Oyjl: 1.0.0)
 */
void       oyjlStr_Release           ( oyjl_str          * string_ptr )
{
  struct oyjl_string_s * str;
  if(!string_ptr) return;
  str = *string_ptr;
  void (* deAlloc)(void*) = str->deAlloc;
  oyjlStr_Clear(str);
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
extern char * oyjl_term_color_plain_;
const char * oyjlTermColorToPlainArgs( const char        * text );
/** @brief   remove term color marks
 *
 *  The OYJL_REGEXP flag uses: int count = oyjlRegExpReplace( &t, "\033[[0-9;]*m", "" ); .
 *
 *  @param[in]     text                input
 *  @param[in]     flags               support ::OYJL_REGEXP slower but work as well outside Oyjl
 *  @return                            cleaned text
 *
 *  @version Oyjl: 1.0.0
 *  @date    2022/06/04
 *  @since   2022/04/03 (Oyjl: 1.0.0)
 */
const char * oyjlTermColorToPlain    ( const char        * text,
                                       int                 flags )
{
  if(flags & OYJL_REGEXP)
  {
    char * t = text ? oyjlStringCopy(text,0) : NULL;
    int count = t ? oyjlRegExpReplace( &t, "\033[[0-9;]*m", "" ) : 0;
    if(count)
    {
      if(oyjl_term_color_plain_) free(oyjl_term_color_plain_);
      oyjl_term_color_plain_ = t;
      t = NULL;
      text = oyjl_term_color_plain_;
    }
    else if(t)
      free(t);
  } else
  {
    text = oyjlTermColorToPlainArgs( text );
  }
  return text;
}
/** @} *//* oyjl_string */
/** @} *//* oyjl_core */
/* --- String_Section --- */
