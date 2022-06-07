/** @file oyjl_core.c
 *
 *  oyjl - string, file i/o and other basic helpers
 *
 *  @par Copyright:
 *            2016-2022 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl core functions
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *
 * Copyright (c) 2004-2022  Kai-Uwe Behrmann  <ku.b@gmx.de>
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

/*     Sections     */
/* --- Debug_Section --- */
/* --- Message_Section --- */
/* --- String_Section --- */
/* --- IO_Section --- */
/* --- Render_Section --- */
/* --- Init_Section --- */
/* --- I18n_Section --- */
/* --- Misc_Section --- */


/* --- Debug_Section --- */
int oyjl_debug_local_ = 0;
int * oyjl_debug = &oyjl_debug_local_;

/** @brief   set own debug variable */
void       oyjlDebugVariableSet      ( int               * debug )
{ oyjl_debug = debug; }

#ifdef OYJL_HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#define oyjlFree_m_(x) { free(x); x = NULL; }
char * oyjlFindApplication_(const char * app_name);

/** @brief backtrace
 *
 *  Create backtrace of execution stack.
 *
 *  @param[in]      stack_limit         set limit of stack depth
 *  @return                             one line string with function names and belonging lines of code
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/03/20
 *  @since   2021/03/20 (Oyjl: 1.0.0)
 */
char *   oyjlBT                      ( int                 stack_limit )
{
  char * text = NULL;
  static int oyjl_init_has_addr2line_ = 0;
  static int oyjl_has_addr2line_ = 0;
  static int oyjl_has_eu_addr2line_ = 0;

          int j, nptrs;
          void *buffer[BT_BUF_SIZE];
          char **strings;

          nptrs = backtrace(buffer, BT_BUF_SIZE);

          strings = backtrace_symbols(buffer, nptrs);
          if( strings == NULL )
          {
            perror("backtrace_symbols");
          } else
          {
            int size = 0;
            char * prog,
                 * main_prog = NULL;
            char * addr_infos = NULL;
            char * txt = NULL;

            int start = nptrs-1;
            do { --start; } while( start >= 0 && (strstr(strings[start], "(main+") == NULL) );
            if(start < 0) start = nptrs-1; /* handle threads */

            if( oyjl_init_has_addr2line_ == 0 )
            {
              ++oyjl_init_has_addr2line_;
              oyjl_has_addr2line_ = 0;
              oyjl_has_eu_addr2line_ = 0;
              if(oyjlHasApplication( "eu-addr2line" ))
                ++oyjl_has_eu_addr2line_;
              if(oyjlHasApplication( "addr2line" ))
                ++oyjl_has_addr2line_;
            }

            for(j = start; j >= (*oyjl_debug?0:1); j--)
            {
              const char * line = strings[j],
                         * tmp = strchr( line, '(' ),
                         * addr = strchr( tmp?tmp:line, '[' );

              prog = oyjlStringCopy( line, NULL );
              txt = strchr( prog, '(' );
              if(txt) txt[0] = '\000';

              if(j == start)
              {
                main_prog = oyjlStringCopy( prog, 0 );
                if(!oyjlIsFile(main_prog, "r", NULL, 0))
                {
                  char *app = NULL;
                  if((app = oyjlFindApplication_( main_prog )) != NULL &&
                      oyjlIsFile(app, "r", NULL, 0))
                  {
                    if( main_prog ) oyjlFree_m_( main_prog );
                    main_prog = app;
                    app = NULL;
                  }
                  if(app) oyjlFree_m_( app );
                }
                if(*oyjl_debug)
                  fprintf(stderr, "prog = %s main_prog = %s\n", prog, main_prog );
              }

              if( main_prog && strstr(main_prog, prog) == NULL && oyjl_has_eu_addr2line_)
              {
                char * addr2 = NULL;
                txt = strchr( tmp?tmp:line, '(' );
                if(txt) addr2 = oyjlStringCopy( txt+1, NULL );
                if(addr2) txt = strchr( addr2, ')' );
                if(txt) txt[0] = '\000';
                if(addr2)
                {
                  addr_infos = oyjlReadCommandF( &size, "r", malloc, "eu-addr2line -s --pretty-print -i -f -C -e %s %s", prog, addr2 );
                  oyjlFree_m_(addr2);
                  if(addr_infos)
                  {
                    txt = strrchr(addr_infos, ':');
                    if(txt) txt[0] = '\000';
                  }
                }
              }
              else if(addr && oyjl_has_addr2line_)
              {
                char * addr2 = oyjlStringCopy( addr+1, NULL );
                addr2[strlen(addr2)-1] = '\000';
                addr_infos = oyjlReadCommandF( &size, "r", NULL, "addr2line -spifCe %s %s", main_prog ? main_prog : prog, addr2 );
                oyjlFree_m_(addr2);
              }

              if(*oyjl_debug > 1)
                fprintf(stderr, "%s\n", line);

              {
                char * t = NULL, * txt = NULL, * addr_info = NULL, * line_number = NULL , * func_name = NULL, * discriminator = NULL;
                if(addr_infos)
                {
                  addr_info = oyjlStringCopy( addr_infos, NULL );

                  if( addr_info[strlen(addr_info)-1] == '\n' ) addr_info[strlen(addr_info)-1] = '\000';

                  if( addr_info[strlen(addr_info)-1] == ')' &&
                      strrchr( addr_info, '(' ) )
                  {
                    txt = strrchr( addr_info, '(' );
                    discriminator = oyjlStringCopy( txt, NULL );
                    txt[-1] = '\000';
                  } 

                  txt = strrchr( addr_info, ' ' );
                  if(txt && strrchr( txt, ' '))
                  {
                    func_name = oyjlStringCopy( addr_info, NULL );
                    txt = strrchr( func_name, ' ' );
                    if(txt) txt = strrchr( txt, ' ' );
                    if(txt) txt[0] = '\000';
                    txt = strrchr( func_name, ' ' ); /* at */
                    if(txt) txt[0] = '\000';
                    else oyjlFree_m_(func_name);

                    if(func_name) txt = strrchr( addr_info, ' ' ) + 1;
                    if(txt) line_number = oyjlStringCopy( txt, NULL );
                  } else
                  {
                    txt = strchr( addr_info, '(' );
                    if(txt) txt[-1] = '\000';
                  }
                }
                if(func_name) t = oyjlStringCopy( func_name, NULL );
                else
                {
                  if(tmp)
                  {
                    t = oyjlStringCopy( tmp[0] == '(' ? &tmp[1] : tmp, NULL );
                    txt = strchr(t, '+');
                    if(txt) txt[0] = '\000';
                  }
                  else
                    t = oyjlStringCopy( addr_infos, NULL );
                }
                if(t)
                {
                  if(j == (*oyjl_debug ? 0 : 1))
                  {
                    oyjlStringAdd( &text, 0,0, "%s", stack_limit >= 0 ? oyjlTermColor(oyjlBOLD, t) : t );
                    oyjlStringAdd( &text, 0,0, "(%s) ", line_number ? stack_limit >= 0 ? oyjlTermColor(oyjlITALIC, line_number ) : line_number : "");
                  }
                  else
                  {
                    oyjlStringAdd( &text, 0,0, "%s", stack_limit >= 0 ? oyjlTermColor(oyjlBOLD, t) : t );
                    oyjlStringAdd( &text, 0,0, "(%s)->", line_number ? stack_limit >= 0 ? oyjlTermColor(oyjlITALIC, line_number ) : line_number  : "");
                  }
                  oyjlFree_m_(t);
                }
                oyjlFree_m_(addr_info);
                if(line_number) oyjlFree_m_(line_number);
                if(func_name) oyjlFree_m_(func_name);
                if(discriminator) oyjlFree_m_(discriminator);
              }
              oyjlFree_m_( addr_infos );
              oyjlFree_m_( prog );
            }
            oyjlStringAdd( &text, 0,0, "\n" );
            free(strings);
            oyjlFree_m_( main_prog );
          }
  return text;
}
#else
char *   oyjlBT                      ( int                 stack_limit OYJL_UNUSED )
{
  return NULL;
}
#endif

/** @brief print current date time
 *
 *  Create a static string to contain ISO conforming date/time string.
 *
 *  @param[in]      flags               0 default so ISO dateTtime+-TimeZoneDiff == OYJL_DATE | OYJL_TIME | OYJL_OYJL_TIME_ZONE_DIFF
 *                                      - OYJL_DATE : %F
 *                                      - OYJL_TIME : %H:%M:%S
 *                                      - OYJL_OYJL_TIME_ZONE : %Z
 *                                      - OYJL_OYJL_TIME_ZONE_DIFF : %z
 *                                      - OYJL_BRACKETS : [datetime]
 *  @param[in]      mark                set text marking
 *  @return                             one line string with function names and belonging lines of code
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/05/21
 *  @since   2021/05/21 (Oyjl: 1.0.0)
 */
const char * oyjlPrintTime           ( int                 flags,
                                       oyjlTEXTMARK_e      mark )
{
  static char t[64];
  struct tm * gmt;
  time_t cutime = time(NULL); /* time right NOW */
  gmt = localtime( &cutime );
  t[0] = '\000';

  if(flags == 0)
    strftime( t, 64, "%FT%H:%M:%S%z", gmt );
  else
  {
    /** One can use OYJL_BRACKETS alone and has dateTtime+-TimeZoneDiff included. */
    if(flags == OYJL_BRACKETS)
      flags |= OYJL_DATE | OYJL_TIME | OYJL_TIME_ZONE_DIFF;
    /** One can use OYJL_TIME_ZONE or OYJL_TIME_ZONE_DIFF alone and has dateTtime included. */
    if(!(flags & OYJL_DATE || flags & OYJL_TIME))
      flags |= OYJL_DATE | OYJL_TIME;

    if(flags & OYJL_BRACKETS)
      sprintf( &t[strlen(t)], "[" );
    if(flags & OYJL_DATE)
      strftime( &t[strlen(t)], 60, "%F", gmt );
    if(flags & OYJL_DATE && flags & OYJL_TIME)
      sprintf( &t[strlen(t)], "T" );
    if(flags & OYJL_TIME)
      strftime( &t[strlen(t)], 50, "%H:%M:%S", gmt );
    if(flags & OYJL_TIME_ZONE)
      strftime( &t[strlen(t)], 40, "%Z", gmt );
    if(flags & OYJL_TIME_ZONE_DIFF)
      strftime( &t[strlen(t)], 40, "%z", gmt );
    if(flags & OYJL_BRACKETS)
      sprintf( &t[strlen(t)], "]" );
  }
  return oyjlTermColor(mark,t);
}

/* --- Debug_Section --- */

/* --- Message_Section --- */
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

  if(error_code == oyjlMSG_INFO) status_text = oyjlTermColor(oyjlGREEN,"Info: ");
  if(error_code == oyjlMSG_CLIENT_CANCELED) status_text = oyjlTermColor(oyjlBLUE,"Client Canceled: ");
  if(error_code == oyjlMSG_INSUFFICIENT_DATA) status_text = oyjlTermColor(oyjlRED,"Insufficient data: ");
  if(error_code == oyjlMSG_ERROR) status_text = oyjlTermColor(oyjlRED,_("Usage Error:"));
  if(error_code == oyjlMSG_PROGRAM_ERROR) status_text = oyjlTermColor(oyjlRED,_("Program Error:"));

  if(status_text)
    fprintf( stderr, "%s ", status_text );
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
/* --- Message_Section --- */

/** \addtogroup oyjl_core OyjlCore
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
 *  need fewer allocations and copies. oyjlStr_New() allocates a new object,
 *  or oyjlStr_NewFrom() wrappes a existing string array into a new object.
 *  oyjlStr() lets you see the contained char array. oyjlStr_AppendN()
 *  performs fast concatenation. oyjlStr_Replace() uses the object advantages.
 *  oyjlStr_Pull() directly takes the char array out of control of the oyjl_str
 *  object and oyjlStr_Release() frees the object and all memory.
 *
 *  @{ *//* oyjl_core */

/* --- String_Section --- */
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
  return oyjlStringSplit2( text, d, oyjlStringDelimiter, count, NULL, alloc );
}

/** @brief   convert a string into list
 *
 *  @param[in]     text                source string
 *  @param[in]     delimiter           the ASCII char which marks the split;
 *                                     e.g. comma ","; optional;
 *                                     default zero: extract white space separated words
 *  @param[in]     splitFunc           function for splitting, default is
 *                                     oyjlStringSplit(); optional
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
    do { ++n;
    } while( (tmp = splitFunc(tmp + 1, delimiter, NULL)) );

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
        const char * end = splitFunc(start, delimiter, &length);
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
void       oyjlStringListAddString   ( char            *** list,
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
 *  @param[in]     text                string
 *  @param[out]    value               resulting number
 *  @param[out]    end                 possibly part after number
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
  *value = strtol( text, &end_, 0 );
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
                                       const char       ** end )
{
  char * end_ = NULL, * t = NULL;
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
    l_error = oyjlStringToDouble( val, &d, 0 );
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
 *  @return                            result:
 *                                     - 0: no match
 *                                     - >0: first string adress in text
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/01/06
 *  @since   2020/07/28 (Oyjl: 1.0.0)
 */
char *     oyjlRegExpFind            ( char              * text,
                                       const char        * regex )
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
    match = &text[re_match.rm_so];
#endif

  if(match == NULL)
    match = strstr(text, regex);

  return match;
}

const char * oyjlRegExpDelimiter ( const char * text, const char * delimiter, int * length )
{
  const char * pos = oyjlRegExpFind( (char*)text, delimiter ),
             * pos2 = pos ? oyjlRegExpFind( (char*)pos+1, delimiter ) : NULL;
  if(pos)
  {
    if(length)
      *length = pos2!=NULL ? pos2-pos : (int)strlen(pos);
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
 *  @param         replacement         substitute all matches of regex in text
 *  @return                            count of replacements
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/09/29
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
  const char * txt;
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
      oyjlStr_AppendN( str, tail, strlen(tail) );
      free(tail);
    }
    txt = oyjlStr(str);
    ++count;
    if(!n) break;
  }
  regfree( &re );
  *text = oyjlStr_Pull( str );
  oyjlStr_Release( &str );

#else

  count = oyjlStringReplace(&text, regex, replacement, 0,0);

#endif
  return count;
}


void oyjlNoBracketCb_(const char * text OYJL_UNUSED, const char * start, const char * end, const char * search, const char ** replace, void * data OYJL_UNUSED)
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
  oyjlStr_AppendN( tmp, t, strlen(t) );
  oyjlRegExpEscape2_( tmp );
  out = oyjlStr_Pull(tmp); 
  oyjlStr_Release( &tmp );
  return out;
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
    if((append_len + str->len) >= str->alloc_len - 1)
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
    oyjlStr_AppendN( string, text, strlen(text) );
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
 *  @param[in,out] user_data           optional user data for modifyReplacement
 *  @return                            number of occurences
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/10/24
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
                                                              void * user_data),
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
      if(!t) t = oyjlStr_New(10,0,0);
      oyjlStr_AppendN( t, start, end-start );
      if(modifyReplacement) modifyReplacement( oyjlStr(text), start, end, search, &replacement, user_data );
      oyjlStr_AppendN( t, replacement, strlen(replacement) );
      ++n;
      if(strlen(end) >= (size_t)s_len)
        start = end + s_len;
      else
      {
        if(strstr(start,search) != 0)
          oyjlStr_AppendN( t, replacement, strlen(replacement) );
        start = end = end + s_len;
        break;
      }
    }
    if(n && start && end == NULL)
      oyjlStr_AppendN( t, start, strlen(start) );
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
      oyjlStr_AppendN( str, oyjlStr(t), strlen(oyjlStr(t)) );
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
/* --- String_Section --- */

/* --- IO_Section --- */
/** @brief read FILE into memory
 *
 *  allocators are malloc()/realloc()
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

#define WARNc_S(...) oyjlMessage_p( oyjlMSG_ERROR, 0, __VA_ARGS__ )
#include <errno.h>
/** @brief read local file into memory
 *
 *  uses malloc()
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
      text = (char*) malloc(size+1);
      if(text == NULL)
      {
        WARNc_S( "Could allocate memory: %lu", (long unsigned int)size);
        fclose( fp );
        return NULL;
      }
      s = fread(text, sizeof(char), size, fp);
      text[size] = '\000';
      if(s != size)
        WARNc_S( "fread %lu but should read %lu",
                (long unsigned int) s, (long unsigned int)size);
      fclose( fp );
    } else
    {
      WARNc_S( "%s\"%s\"", _("Could not open: "), file_name);
    }
  }

  if(size_ptr)
    *size_ptr = size;

  return text;
}

/** @internal
 *  Copy to external allocator */
char *       oyjlReAllocFromStdMalloc_(char              * mem,
                                       int               * size,
                                       void*            (* alloc)(size_t) )
{
  if(mem)
  {
    if(alloc != malloc)
    {
      char* temp = mem;

      mem = alloc( *size + 1 );
      if(mem)
      {
        memcpy( mem, temp, *size );
        mem[*size] = '\000';
      }
      else
        *size = 0;

      free( temp );
    } else
      mem[*size] = '\000';
  }

  return mem;
}

int oyjlIsFileFull_ (const char* fullFileName, const char * read_mode);
/* resembles which */
char * oyjlFindApplication_(const char * app_name)
{
  const char * path = getenv("PATH");
  char * full_app_name = NULL;
  if(path && app_name)
  {
    int paths_n = 0, i;
    char ** paths = oyjlStringSplit( path, ':', &paths_n, malloc );
    for(i = 0; i < paths_n; ++i)
    {
      char * full_name = NULL;
      int found;
      oyjlStringAdd( &full_name, 0,0, "%s/%s", paths[i], app_name );
      found = oyjlIsFileFull_( full_name, "rb" );
      if(found)
      {
        i = paths_n;
        full_app_name = strdup( full_name );
      }
      free( full_name );
      if(found) break;
    }
    oyjlStringListRelease( &paths, paths_n, free );
  }
  return full_app_name;
}

/** @brief detect program
 *
 *  Search for a command in the executeable path. It resembles 'which'.
 *
 *  @param[in]      app_name            application name withou path
 *  @return                             1 - if found, otherwise 0
 *
 *  @version Oyjl: 1.0.0
 *  @date    2022/04/23
 *  @since   2022/04/23 (Oyjl: 1.0.0)
 */
int        oyjlHasApplication        ( const char        * app_name)
{
  char * full_app_name = oyjlFindApplication_(app_name);
  int found = full_app_name == NULL ? 0 : 1;
  if(full_app_name) free(full_app_name);
  return found;
}

/** @internal
 *  Read a file stream without knowing its size in advance.
 */
char * oyjlReadCmdToMem_             ( const char        * command,
                                       int               * size,
                                       const char        * mode,
                                       void*            (* alloc)(size_t) )
{
  char * text = 0;
  FILE * fp = 0;

  if(!alloc) alloc = malloc;

  if(command && command[0] && size )
  {
    {
      if(*oyjl_debug && (strstr(command, "addr2line") == NULL || *oyjl_debug > 1))
        oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "%s", OYJL_DBG_ARGS, command );
      fp = oyjlPOPEN_m( command, mode );
    }
    if(fp)
    {
      int mem_size = 0;
      char* mem = NULL;

      text = oyjlReadFileStreamToMem(fp, size);

      if(!feof(fp))
      {
        if(text) { free( text ); text = NULL; }
        *size = 0;
        mem_size = 1024;
        mem = (char*) malloc(mem_size+1);
        oyjlPCLOSE_m(fp);
        fp = oyjlPOPEN_m( command, mode );
      }
      if(fp)
      while(!feof(fp))
      {
        if(*size >= mem_size)
        {
          mem_size *= 10;
          mem = realloc( mem, mem_size+1 );
          if(!mem) { *size = 0; break; }
        }
        if(mem)
          *size += fread( &mem[*size], sizeof(char), mem_size-*size, fp );
      }
      if(fp && mem)
      {
        mem = oyjlReAllocFromStdMalloc_( mem, size, alloc );
        text = mem;
      }
      if(fp)
        oyjlPCLOSE_m(fp);
      fp = 0;

      if(*size == 0)
      {
        char * t = strdup(command);
        char * end = strstr( t?t:"", " " ), * app;
        if(end)
          end[0] = '\000';

        if((app = oyjlFindApplication_( t )) == NULL)
          oyjlMessage_p( oyjlMSG_ERROR,0, OYJL_DBG_FORMAT "%s: \"%s\"",
                         OYJL_DBG_ARGS, _("Program not found"), command?command:"");

        if(t) free(t);
        if(app) free(app);
      }
    }
  }

  return text;
}

/** @brief Read a stream from shell command.
 */
char *     oyjlReadCommandF          ( int               * size,
                                       const char        * mode,
                                       void*            (* alloc)(size_t),
                                       const char        * format,
                                                           ... )
{
  char * result = NULL;
  char * text = 0;

  if(!alloc) alloc = malloc;

  OYJL_CREATE_VA_STRING(format, text, malloc, return NULL)

  result = oyjlReadCmdToMem_( text, size, mode, alloc );

  free(text);

  return result;
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
    int error = stat(fullname, &status);
    if( error )
      return 0;
#   if defined(__APPLE__) || defined(BSD)
    mod_time = status.st_mtime ;
    mod_time += status.st_mtimespec.tv_nsec/1000000. ;
#   elif defined(WIN32)
    mod_time = (double)status.st_mtime ;
#   elif defined(__ANDROID__)
    mod_time = status.st_mtime ;
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
  if(path_name && path_name[0] == '\000')
    oyjlStringAdd( &path_name, 0,0, "%s", full_name );
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
                                       const void        * mem,
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
      if(*oyjl_debug && *oyjl_debug > 1)
        oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "%s(%d)", OYJL_DBG_ARGS, full_name, size );
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
/* --- IO_Section --- */
/** @} *//* oyjl_core */

/* --- Render_Section --- */
#if !defined(COMPILE_STATIC) || !defined(HAVE_QT)
#warning "compile dynamic section"
#ifdef COMPILE_STATIC
#warning "COMPILE_STATIC defined"
#endif
#ifdef HAVE_QT
#warning "HAVE_QT defined"
#endif

#ifdef HAVE_DL
#include <dlfcn.h>
static void *  oyjl_args_render_lib_ = NULL;
static char *  oyjlLibNameCreate_    ( const char        * lib_base_name,
                                       int                 version )
{
  char * fn = NULL;

#ifdef __APPLE__
    oyjlStringAdd( &fn, 0,0, "%s%s.%d%s", OYJL_LIB_PREFIX, lib_base_name, version, OYJL_LIB_SUFFIX );
#elif defined(_WIN32)
    oyjlStringAdd( &fn, 0,0, "%s%s-%d%s", OYJL_LIB_PREFIX, lib_base_name, version, OYJL_LIB_SUFFIX );
#else
    oyjlStringAdd( &fn, 0,0, "%s%s%s.%d", OYJL_LIB_PREFIX, lib_base_name, OYJL_LIB_SUFFIX, version );
#endif
  return fn;
}
static char *  oyjlFuncNameCreate_   ( const char        * base_name )
{
  char * func = NULL;

  func = oyjlStringCopy( base_name, NULL );
  if(func)
  {
    func[0] = tolower(func[0]);
    oyjlStringAdd( &func, 0,0, "_" );
  }
  return func;
}
#else
#warning "HAVE_DL not defined (possibly dlfcn.h not found?): dynamic loading of libOyjlArgsQml will not be possible"
#endif /* HAVE_DL */

#ifdef __cplusplus
extern "C" { // "C" API wrapper 
#endif
typedef int (*oyjlArgsRender_f)     ( int                 argc,
                                      const char       ** argv,
                                      const char        * json,
                                      const char        * commands,
                                      const char        * output,
                                      int                 debug,
                                      oyjlUi_s          * ui,
                                      int               (*callback)(int argc, const char ** argv));
static int (*oyjlArgsRender_p)       ( int                 argc,
                                       const char       ** argv,
                                       const char        * json,
                                       const char        * commands,
                                       const char        * output,
                                       int                 debug,
                                       oyjlUi_s          * ui,
                                       int               (*callback)(int argc, const char ** argv)) = NULL;
static int oyjl_args_render_init_ = 0;
static int oyjlArgsRendererLoad_( const char * render_lib )
{
  const char * name = render_lib;
  char * fn = oyjlLibNameCreate_(name, 1), * func = NULL;
  int error = -1;

#ifdef HAVE_DL
  if(oyjl_args_render_lib_)
    dlclose( oyjl_args_render_lib_ );
  oyjl_args_render_lib_ = NULL;
  oyjlArgsRender_p = NULL;
  oyjl_args_render_lib_ = dlopen(fn, RTLD_LAZY);
  if(!oyjl_args_render_lib_)
  {
    oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "not existent: %s", OYJL_DBG_ARGS, fn );
    error = 1;
  }
  else
  {
    func = oyjlFuncNameCreate_(name);
    oyjlArgsRender_p = (oyjlArgsRender_f)dlsym( oyjl_args_render_lib_, func );
    if(oyjlArgsRender_p)
      error = 0; /* found */
    else
      oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "%s: %s", OYJL_DBG_ARGS, func, dlerror() );
  }
#else
  oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "no dlopen() API available for %s", OYJL_DBG_ARGS, fn );
  error = 1;
#endif
  free(fn);

  return error;
}

static int oyjlArgsRendererSelect_  (  oyjlUi_s          * ui )
{
  const char * arg = NULL, * name = NULL;
  oyjlOption_s * R;
  int error = 0;

  if( !ui )
  {
    oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "no \"ui\" argument passed in", OYJL_DBG_ARGS );
    return 1;
  }

  R = oyjlOptions_GetOptionL( ui->opts, "R", 0 );
  if(!R)
  {
    oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "no \"-R|--render\" argument found: Can not select", OYJL_DBG_ARGS );
    return 1;
  }

  if(R->variable_type != oyjlSTRING)
  {
    oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "no \"-R|--render\" oyjlSTRING variable declared", OYJL_DBG_ARGS );
    return 1;
  }

  arg = oyjlStringCopy( *R->variable.s, NULL );
  if(!arg)
  {
    oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "no \"-R|--render\" oyjlSTRING variable found", OYJL_DBG_ARGS );
    return 1;
  }
  else
  {
    if(arg[0])
    {
      char * low = oyjlStringToLower_( arg );
      if(low)
      {
        if(strlen(low) >= strlen("gui") && memcmp("gui",low,strlen("gui")) == 0)
          name = "OyjlArgsQml";
        else
        if(strlen(low) >= strlen("qml") && memcmp("qml",low,strlen("qml")) == 0)
          name = "OyjlArgsQml";
        else
        if(strlen(low) >= strlen("cli") && memcmp("cli",low,strlen("cli")) == 0)
          name = "OyjlArgsCli";
        else
        if(strlen(low) >= strlen("web") && memcmp("web",low,strlen("web")) == 0)
          name = "OyjlArgsWeb";
        if(!name)
        {
          oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "\"-R|--render\" not supported: %s|%s", OYJL_DBG_ARGS, arg,oyjlTermColor(oyjlBOLD,low) );
          free(low);
          return 1;
        }
        free(low);
        error = oyjlArgsRendererLoad_( name );
      }
    }
    else /* report all available renderers */
    {
      if(oyjlArgsRendererLoad_( "OyjlArgsQml" ) == 0)
        oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "OyjlArgsQml found - option -R=\"gui\"", OYJL_DBG_ARGS );
      if(oyjlArgsRendererLoad_( "OyjlArgsCli" ) == 0)
        oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "OyjlArgsCli found - option -R=\"cli\"", OYJL_DBG_ARGS );
      if(oyjlArgsRendererLoad_( "OyjlArgsWeb" ) == 0)
        oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "OyjlArgsWeb found - option -R=\"web\"", OYJL_DBG_ARGS );
    }
  }

  if(!oyjl_args_render_init_)
  {
    char * fn = oyjlLibNameCreate_(name, 1);
    ++oyjl_args_render_init_;

#ifdef HAVE_DL
#else
    oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "no dlopen() API available for %s", OYJL_DBG_ARGS, fn );
#endif

    free(fn);
  }

  return error;
}


/** \addtogroup oyjl_args
 *  @{ *//* oyjl_args */
/** @brief Load renderer for graphical rendering options
 *
 *  The function version in libOyjlCore dynamicaly dlopen() s libOyjlArgsQml.
 *  The function version in liboyjl-core-static needs as well
 *  liboyjl-args-qml-static to work. You should check wich library is linked
 *  and possibly guard the according code in case no QML library is available:
 *  @code
    #if !defined(NO_OYJL_ARGS_RENDER)
    if(render && ui)
      // code with oyjlArgsRender() goes here
    #endif
    @endcode
 *
 *  @param[in]     argc                number of arguments from main()
 *  @param[in]     argv                arguments from main()
 *  @param[in]     json                JSON UI text; optional, can be generated from ui
 *  @param[in]     commands            JSON commands/config text; optional, can be generated from ui or passed in as --render="XXX" option
 *  @param[in]     output              write ui interaction results; optional
 *  @param[in]     debug               set debug level
 *  @param[in]     ui                  user interface structure
 *  @param[in,out] callback            the function resembling main() to call into;
 *                                     It will be used to parse args, show help texts,
 *                                     all options handling and data processing
 *  @return                            return value for main()
 *
 *  @version Oyjl: 1.0.0
 *  @date    2020/03/05
 *  @since   2019/12/14 (Oyjl: 1.0.0)
 */
int oyjlArgsRender                   ( int                 argc,
                                       const char       ** argv,
                                       const char        * json,
                                       const char        * commands,
                                       const char        * output,
                                       int                 debug,
                                       oyjlUi_s          * ui,
                                       int               (*callback)(int argc, const char ** argv))
{
  int result = -1;
  oyjlArgsRendererSelect_(ui);
  if(oyjlArgsRender_p)
    result = oyjlArgsRender_p(argc, argv, json, commands, output, debug, ui, callback );
  fflush(stdout);
  fflush(stderr);
  return result;
}
/** @} *//* oyjl_args */
#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* !COMPILE_STATIC || !HAVE_QT */


/** \addtogroup oyjl
    @section oyjl_intro Introduction
  
    Oyjl API provides a platformindependent C interface for JSON I/O, conversion to and from
    XML + YAML, string helpers, file reading, testing and argument handling.

    The API's are quite independent. 

    @section oyjl_api API Documentation
    The API of the @ref oyjl is declared in the oyjl.h header file.
    - @ref oyjl_tree - OyjlTree JSON modeled C data structure with data I/O API: *libOyjlCore*, all parsers (JSON,YAML,XML) reside in *libOyjl*
    - @ref oyjl_core - OyjlCore API: *libOyjlCore*
    - @ref oyjl_test - OyjlTest API: header only implementation in *oyjl_test.h* and *oyjl_test_main.h*
    - @ref oyjl_args - OyjlArgs Argument Handling API: link to *libOyjlCore* or with slightly reduced functionality in the stand alone *oyjl_args.c* version

    @section oyjl_tools Tools Documentation
    Oyjl comes with a few tools, which use the Oyjl API's.
    - @ref oyjl - JSON manipulation
    - @ref oyjltranslate - localisation helper tool
    - @ref oyjlargs - code generation tool
    - @ref oyjlargsqml - interactive option renderer written in Qt's QML
 *  @{ *//* oyjl */

static char * oyjl_nls_path_ = NULL;
char * oyjl_debug_node_path_ = NULL;
char * oyjl_debug_node_value_ = NULL;
extern char * oyjl_term_color_html_;
void oyjlLibRelease() {
  int i;
  if(oyjl_nls_path_)
  {
    putenv("NLSPATH=C"); free(oyjl_nls_path_); oyjl_nls_path_ = NULL;
  }
#if defined(HAVE_DL) && (!defined(COMPILE_STATIC) || !defined(HAVE_QT))
  if(oyjl_args_render_lib_)
  {
    dlclose(oyjl_args_render_lib_); oyjl_args_render_lib_ = NULL; oyjl_args_render_init_ = 0;
  }
#endif
  if(oyjl_tr_context_)
  {
    i = 0;
    while(oyjl_tr_context_[i])
    {
      oyjlTr_Release( &oyjl_tr_context_[i] );
      ++i;
    }
    free(oyjl_tr_context_);
    oyjl_tr_context_ = NULL;
  }
  if(oyjl_debug_node_path_)
  {
    free(oyjl_debug_node_path_);
    oyjl_debug_node_path_ = NULL;
  }
  if(oyjl_debug_node_value_)
  {
    free(oyjl_debug_node_value_);
    oyjl_debug_node_value_ = NULL;
  }
  if(oyjl_term_color_html_)
  {
    free(oyjl_term_color_html_);
    oyjl_term_color_html_ = NULL;
  }
  if(oyjl_term_color_plain_)
  {
    free(oyjl_term_color_plain_);
    oyjl_term_color_plain_ = NULL;
  }
}
/* --- Render_Section --- */

/* --- Init_Section --- */
#define OyjlToString2_M(t) OyjlToString_M(t)
#define OyjlToString_M(t) #t
void   oyjlGettextSetup_             ( int                 use_gettext OYJL_UNUSED,
                                       const char        * loc_domain OYJL_UNUSED,
                                       const char        * env_var_locdir OYJL_UNUSED,
                                       const char        * default_locdir OYJL_UNUSED )
{
#ifdef OYJL_HAVE_LIBINTL_H
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
        if(*oyjl_debug)
          oyjlMessage_p( oyjlMSG_INFO, 0,"found environment variable: %s=%s", env_var_locdir, domain_path );
      } else
        if(environment_locale_dir == NULL && getenv("LOCPATH") && strlen(getenv("LOCPATH")))
      {
        domain_path = NULL;
        locpath = getenv("LOCPATH");
        if(*oyjl_debug)
          oyjlMessage_p( oyjlMSG_INFO, 0,"found environment variable: LOCPATH=%s", locpath );
      } else
        if(*oyjl_debug)
        oyjlMessage_p( oyjlMSG_INFO, 0,"no %s or LOCPATH environment variable found; using default path: %s", env_var_locdir, domain_path );

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
      const char * d = textdomain( NULL );
      const char * dpath = bindtextdomain( loc_domain, path );
      if(*oyjl_debug)
      {
        char * fn = NULL;
        int stat = -1;
        const char * gettext_call = OyjlToString2_M(_());
        const char * domain = textdomain(NULL);

        oyjlMessage_p( oyjlMSG_INFO, 0,"bindtextdomain( \"%s\", \"%s\" ) = %s textdomain( NULL ) = %s", loc_domain, path, dpath, d );
        if(path)
          oyjlStringAdd( &fn, 0,0, "%s/de/LC_MESSAGES/%s.mo", path ? path : "", loc_domain);
        if(fn)
          stat = oyjlIsFileFull_( fn, "r" );
        oyjlMessage_p( oyjlMSG_INFO, 0,"bindtextdomain(\"%s\"/%s) to %s\"%s\" %s for %s  test:%s", loc_domain, domain, locpath?"effectively ":"", path ? path : "", (stat > 0)?"Looks good":"Might fail", gettext_call, _("Example") );
        if(fn) free(fn);
      }
      if(tmp)
        free(tmp);
    }
  }
#endif /* OYJL_HAVE_LIBINTL_H */
}
void   oyjlInitI18n_                 ( const char        * loc )
{
#ifndef OYJL_SKIP_TRANSLATE
  oyjl_val oyjl_catalog = NULL;
  oyjlTr_s * trc = NULL;
  int use_gettext = 0;
#ifdef OYJL_USE_GETTEXT
  use_gettext = 1;
#else
# include "liboyjl.i18n.h"
  int size = sizeof(liboyjl_i18n_oiJS);
  oyjl_catalog = (oyjl_val) oyjlStringAppendN( NULL, (const char*) liboyjl_i18n_oiJS, size, malloc );
  if(*oyjl_debug)
    oyjlMessage_p( oyjlMSG_INFO, 0,OYJL_DBG_FORMAT "loc: \"%s\" domain: \"%s\" catalog-size: %d", OYJL_DBG_ARGS, loc, OYJL_DOMAIN, size );
#endif
  oyjlGettextSetup_( use_gettext, OYJL_DOMAIN, "OYJL_LOCALEDIR", OYJL_LOCALEDIR );
  trc = oyjlTr_New( loc, OYJL_DOMAIN, &oyjl_catalog, 0,0,0, *oyjl_debug > 1?OYJL_OBSERVE:0 );
  oyjlTr_SetFlags( trc, 0 );
  oyjlTr_Set( &trc );
#endif
}

/** @brief   init the libraries language; optionaly
 *
 *  Additionally use setlocale() to obtain locale in your application.
 *  The message catalog search path is detected from the project specific
 *  environment variable specified in \em env_var_locdir and
 *  the \em LOCPATH environment variables. If those are not present
 *  a expected fall back directory from \em default_locdir is used.
 *
 *  @param         project_name        project name display string; e.g. "MyProject"
 *  @param         env_var_debug       environment debug variable string;
 *                                     e.g. "MP_DEBUG"
 *  @param         debug_variable      int C variable; e.g. my_project_debug
 *  @param         use_gettext         switch gettext support on or off
 *  @param         env_var_locdir      environment variable string for locale path;
 *                                     e.g. "MP_LOCALEDIR"
 *  @param         default_locdir      default locale path C string;
 *                                     e.g. "/usr/local/share/locale"
 *  @param         context             locale, domain and possibly more information
 *                                     - domain: po and mo files; e.g. "myproject"
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
                                       int                 use_gettext OYJL_UNUSED,
                                       const char        * env_var_locdir OYJL_UNUSED,
                                       const char        * default_locdir OYJL_UNUSED,
                                       oyjlTr_s         ** context,
                                       oyjlMessage_f       msg )
{
  int error = -1;
  oyjlTr_s * trc = context?*context:NULL;
  const char * loc = oyjlTr_GetLang( trc );
  const char * loc_domain = oyjlTr_GetDomain( trc );

  if(!msg) msg = oyjlMessage_p;

  if(debug_variable)
    oyjlDebugVariableSet( debug_variable );
  oyjlMessageFuncSet(msg);

  if(debug_variable && *debug_variable)
    oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "loc: %s loc_domain: %s", OYJL_DBG_ARGS, loc, loc_domain );

  if(debug_variable && getenv(env_var_debug))
  {
    *debug_variable = atoi(getenv(env_var_debug));
    if(*debug_variable)
    {
      int v = oyjlVersion(0);
      if(*debug_variable)
        msg( oyjlMSG_INFO, 0, "%s (Oyjl compile v: %s runtime v: %d)", project_name, OYJL_VERSION_NAME, v );
    }
  }

  oyjlInitI18n_( loc );

  if(loc_domain)
  {
    oyjlGettextSetup_( use_gettext, loc_domain, env_var_locdir, default_locdir );
    int state = oyjlTr_Set( context ); /* just pass domain in */
    if(*oyjl_debug)
      msg( oyjlMSG_INFO, 0, "use_gettext: %d loc_domain: %s env_var_locdir: %s default_locdir: %s oyjlTr_Set: %d", use_gettext, loc_domain, env_var_locdir, default_locdir, state );
  }

  return error;
}
/* --- Init_Section --- */

/* --- I18n_Section --- */
/** @brief   obtain language part of i18n locale code
 *
 *  @param         loc                 locale name as from setlocale("")
 *  @return                            language part
 *
 *  @version Oyjl: 1.0.0
 *  @date    2020/07/27
 *  @since   2020/07/27 (Oyjl: 1.0.0)
 */
char *         oyjlLanguage          ( const char        * loc )
{
  if(strchr(loc,'_') != NULL)
  {
    char * t = strdup(loc);
    char * tmp = strchr(t,'_');
    tmp[0] = '\000';
    return t;
  } else
    return strdup(loc);
}

/** @brief   obtain country part of i18n locale code
 *
 *  @param         loc                 locale name as from setlocale("")
 *  @return                            country part
 *
 *  @version Oyjl: 1.0.0
 *  @date    2020/07/27
 *  @since   2020/07/27 (Oyjl: 1.0.0)
 */
char *         oyjlCountry           ( const char        * loc )
{
  if(strchr(loc,'_') != NULL)
  {
    char * t = strdup( strchr(loc,'_') + 1 );
    if(strchr(t,'.') != NULL)
    {
      char * tmp = strchr(t,'.');
      tmp[0] = '\000';
    }
    return t;
  }
  else
    return NULL;
}

/* --- I18n_Section --- */

/* --- Misc_Section --- */
#include "oyjl_version.h"
#include <yajl/yajl_parse.h>
#include <yajl/yajl_version.h>
/** @brief  give the compiled in library version
 *
 *  @param[in]  vtype          request API type
 *                             - 0 - Oyjl API
 *                             - 1 - Yajl API
 *  @return                    OYJL_VERSION at library compile time
 */
int            oyjlVersion           ( int                 vtype )
{
  if(vtype == 1)
    return YAJL_VERSION;

  return OYJL_VERSION;
}
/* --- Misc_Section --- */

/** @} *//* oyjl */

/* additional i18n strings */
void oyjlDummy_(void)
{
  char * t = oyjlStringCopy( _("Information"), 0 );
  free(t);
}


