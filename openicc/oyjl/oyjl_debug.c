/** @file oyjl_debug.c
 *
 *  oyjl - debug helpers
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

/* --- Debug_Section --- */

/** \addtogroup oyjl_core
 *  @{ *//* oyjl_core */

int oyjl_debug_local_ = 0;
int * oyjl_debug = &oyjl_debug_local_;

/** @brief   set own debug variable */
void       oyjlDebugVariableSet      ( int               * debug )
{ oyjl_debug = debug; }

#include <execinfo.h>
#define BT_BUF_SIZE 100
#define oyjlFree_m_(x) { free(x); x = NULL; }
char * oyjlFindApplication_(const char * app_name);

/** @brief backtrace
 *
 *  Create backtrace of execution stack.
 *  Honour ::OYJL_NO_BACKTRACE environment variable.
 *
 *  @param[in]      stack_limit         set limit of stack depth
 *  @return                             one line string with function names and belonging lines of code
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/03/20
 *  @since   2021/03/20 (Oyjl: 1.0.0)
 */
char *   oyjlBT                      ( int                 stack_limit OYJL_UNUSED )
{
#ifdef OYJL_HAVE_BACKTRACE
  char * text = NULL;
  static int oyjl_init_has_addr2line_ = 0;
  static int oyjl_has_addr2line_ = 0;
  static int oyjl_has_eu_addr2line_ = 0;

          int j, nptrs;
          void *buffer[BT_BUF_SIZE];
          char **strings;
          if(getenv("OYJL_NO_BACKTRACE"))
            return strdup("");

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
                if(*oyjl_debug == 2)
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

              if(*oyjl_debug == 2 && !getenv("FORCE_COLORTERM"))
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
#else
  return  strdup("");
#endif
}

#if defined(_WIN32) && !defined(__GNU__)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdint.h> // portable: uint64_t   MSVC: __int64 

// MSVC defines this in winsock2.h!?
typedef struct timeval {
    long tv_sec;
    long tv_usec;
} timeval;

int gettimeofday(struct timeval * tp, struct timezone * tzp)
{
    // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
    static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL);

    SYSTEMTIME  system_time;
    FILETIME    file_time;
    uint64_t    time;

    GetSystemTime( &system_time );
    SystemTimeToFileTime( &system_time, &file_time );
    time =  ((uint64_t)file_time.dwLowDateTime )      ;
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    tp->tv_sec  = (long) ((time - EPOCH) / 10000000L);
    tp->tv_usec = (long) (system_time.wMilliseconds * 1000);
    return 0;
}
# define   TIME_DIVIDER CLOCKS_PER_SEC
#else
# include <time.h>
# include <sys/time.h>
# define   TIME_DIVIDER 10000
# include <unistd.h>
#endif

time_t             oyjlTime          ( )
{
  time_t time_;
  double divider = TIME_DIVIDER;
  struct timeval tv;
  double tmp_d;
  gettimeofday( &tv, NULL );
  time_ = tv.tv_usec/(1000000/(time_t)divider)
                   + (time_t)(modf( (double)tv.tv_sec / divider,&tmp_d )
                     * divider*divider);
  return time_;
}
double             oyjlSeconds       ( )
{
           time_t zeit_ = oyjlTime();
           double teiler = TIME_DIVIDER;
           double dzeit = zeit_ / teiler;
    return dzeit;
}

/** @brief print current date time
 *
 *  Create a static string to contain ISO conforming date/time string.
 *
 *  @param[in]      flags               0 default so ISO dateTtime+-TimeZoneDiff == OYJL_DATE | OYJL_TIME | OYJL_OYJL_TIME_ZONE_DIFF
 *                                      - OYJL_DATE : %F
 *                                      - OYJL_TIME : %H:%M:%S
 *                                      - OYJL_TIME_MILLI : %H:%M:%S.%d
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
    double tmp_d;
    /** One can use OYJL_BRACKETS alone and has dateTtime+-TimeZoneDiff included. */
    if(flags == OYJL_BRACKETS)
      flags |= OYJL_DATE | OYJL_TIME_MILLI | OYJL_TIME_ZONE_DIFF;
    /** One can use OYJL_TIME_ZONE or OYJL_TIME_ZONE_DIFF alone and has dateTtime included. */
    if(!(flags & OYJL_DATE || flags & OYJL_TIME))
      flags |= OYJL_DATE | OYJL_TIME;

    if(flags & OYJL_BRACKETS)
      sprintf( &t[strlen(t)], "[" );
    if(flags & OYJL_DATE)
      strftime( &t[strlen(t)], 60, "%F", gmt );
    if(flags & OYJL_DATE && (flags & OYJL_TIME || flags & OYJL_TIME_MILLI))
      sprintf( &t[strlen(t)], "T" );
    if(flags & OYJL_TIME || flags & OYJL_TIME_MILLI)
    {
      strftime( &t[strlen(t)], 50, "%H:%M:%S", gmt );
      if(flags & OYJL_TIME_MILLI)
        snprintf( &t[strlen(t)], 44, ".%03d", (int)(modf(oyjlSeconds(),&tmp_d)*1000) );
    }
    if(flags & OYJL_TIME_ZONE)
      strftime( &t[strlen(t)], 40, "%Z", gmt );
    if(flags & OYJL_TIME_ZONE_DIFF)
      strftime( &t[strlen(t)], 40, "%z", gmt );
    if(flags & OYJL_BRACKETS)
      sprintf( &t[strlen(t)], "]" );
  }
  return oyjlTermColor(mark,t);
}

/** @} *//* oyjl_core */
/* --- Debug_Section --- */

