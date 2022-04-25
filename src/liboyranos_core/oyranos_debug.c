/** @internal
 *  @file oyranos_debug.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2005-2016 (C) Kai-Uwe Behrmann
 *
 *  @brief    internal helpers
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2005/02/01
 */

#include "src/include/oyranos_types.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>  /* system() */

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
# define   OY_GETPID() _getpid()
#else
# include <time.h>
# include <sys/time.h>
# define   TIME_DIVIDER 10000
# include <unistd.h>
# define   OY_GETPID() getpid()
#endif

#include <math.h>

#include "oyranos_debug.h"

int level_PROG = 0;
clock_t oyranos_clock_ = 0;
/** @brief    Set debug level
 *
 *  Maps to ::OY_DEBUG environment variable.
 *  The usefull value range is 1-20.
 *
 *  @see @ref debug_vars
 *
 *  @version  Oyranos: 0.0.1
 *  @date     2005/02/01
 *  @since    2005/02/01 (Oyranos: 0.0.1)
 */
int oy_debug = 0;

int oy_debug_write_id = 0; /* debug write image ID */

void oy_backtrace_()
{
#   define TMP_FILE "/tmp/oyranos_gdb_temp.txt"
    int pid = (int)OY_GETPID();
    FILE * fp = fopen( TMP_FILE, "w" );

    if(fp)
    {
      fprintf(fp, "attach %d\n", pid);
      fprintf(fp, "backtrace\ndetach" );
      fclose(fp);
      {
        int r OY_UNUSED;
        fprintf( stderr, "GDB output:\n" );
        r = system("gdb -batch -x " TMP_FILE);
      }
    } else
      fprintf( stderr, "could not open "TMP_FILE "\n" );
}
                
#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100

#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_io.h" /* oyFindApplication() */
int oyHasApplication_(const char * app_name)
{               
  char * full_app_name = oyFindApplication(app_name);
  int found = full_app_name == NULL ? 0 : 1;
  if(full_app_name) free(full_app_name);
  return found; 
}

/* @param[in]      stack_limit         set limit of stack depth
 *                                     - -1 : omit color/emphasize
 */
char *   oyBT                        ( int                 stack_limit )
{
  char * text = NULL;
  static int oy_init_has_addr2line_ = 0;
  static int oy_has_addr2line_ = 0;
  static int oy_has_eu_addr2line_ = 0;

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
            char * command = NULL;
            size_t size = 0;
            char * prog,
                 * main_prog = NULL;
            char * addr_infos = NULL;
            char * txt = NULL;

            int start = nptrs-1;
            do { --start; } while( start >= 0 && (strstr(strings[start], "(main+") == NULL) );
            if(start < 0) start = nptrs-1; /* handle threads */

            if( oy_init_has_addr2line_ == 0 )
            {
              ++oy_init_has_addr2line_;
              oy_has_addr2line_ = 0;
              oy_has_eu_addr2line_ = 0;
              if(oyHasApplication_( "eu-addr2line" ))
                ++oy_has_eu_addr2line_;
              if(oyHasApplication_( "addr2line" ))
                ++oy_has_addr2line_;
            }

            for(j = start; j >= (oy_debug?0:1); j--)
            {
              const char * line = strings[j],
                         * tmp = strchr( line, '(' ),
                         * addr = strchr( tmp?tmp:line, '[' );

              prog = oyStringCopy( line, NULL );
              txt = strchr( prog, '(' );
              if(txt) txt[0] = '\000';

              if(j == start)
              {
                main_prog = oyStringCopy( prog, 0 );
                if(!oyIsFile_(main_prog))
                {
                  char *app = NULL;
                  if((app = oyFindApplication( main_prog )) != NULL &&
                      oyIsFile_(app))
                  {
                    oyFree_m_( main_prog );
                    main_prog = app;
                    app = NULL;
                  }
                  if(app) oyFree_m_( app );
                }
                if(oy_debug)
                  fprintf(stderr, "prog = %s main_prog = %s\n", prog, main_prog );
              }

              if( main_prog && prog && strstr(main_prog, prog) == NULL && oy_has_eu_addr2line_)
              {
                char * addr2 = NULL;
                txt = strchr( tmp?tmp:line, '(' );
                if(txt) addr2 = oyStringCopy( txt+1, NULL );
                if(addr2) txt = strchr( addr2, ')' );
                if(txt) txt[0] = '\000';
                if(addr2)
                {
                  oyStringAddPrintf( &command, 0,0, "eu-addr2line -s --pretty-print -i -f -C -e %s %s", prog, addr2 );
                  oyFree_m_(addr2);
                  addr_infos = oyReadCmdToMem_( command, &size, "r", NULL );
                  if(addr_infos)
                  {
                    txt = strrchr(addr_infos, ':');
                    if(txt) txt[0] = '\000';
                  }
                }
              }
              else if(addr && oy_has_addr2line_)
              {
                char * addr2 = oyStringCopy( addr+1, NULL );
                addr2[strlen(addr2)-1] = '\000';
                oyStringAddPrintf( &command, 0,0, "addr2line -spifCe %s %s", main_prog ? main_prog : prog, addr2 );
                oyFree_m_(addr2);
                addr_infos = oyReadCmdToMem_( command, &size, "r", NULL );
              }

              if(oy_debug > 1)
                fprintf(stderr, "%s\n", line);

              {
                char * t = NULL, * txt = NULL, * addr_info = NULL, * line_number = NULL , * func_name = NULL, * discriminator = NULL;
                if(addr_infos)
                {
                  addr_info = oyStringCopy( addr_infos, NULL );

                  if(addr_info[strlen(addr_info)-1] == '\n') addr_info[strlen(addr_info)-1] = '\000';

                  if(addr_info)
                  {
                    if( addr_info[strlen(addr_info)-1] == ')' &&
                        strrchr( addr_info, '(' ) )
                    {
                      txt = strrchr( addr_info, '(' );
                      discriminator = oyStringCopy( txt, NULL );
                      txt[-1] = '\000';
                    } 
                  }

                  txt = strrchr( addr_info, ' ' );
                  if(txt && strrchr( txt, ' '))
                  {
                    func_name = oyStringCopy( addr_info, NULL );
                    txt = strrchr( func_name, ' ' );
                    if(txt) txt = strrchr( txt, ' ' );
                    if(txt) txt[0] = '\000';
                    txt = strrchr( func_name, ' ' ); /* at */
                    if(txt) txt[0] = '\000';
                    else oyFree_m_(func_name);

                    if(func_name) txt = strrchr( addr_info, ' ' ) + 1;
                    if(txt) line_number = oyStringCopy( txt, NULL );
                  } else
                  {
                    txt = strchr( addr_info, '(' );
                    if(txt) txt[-1] = '\000';
                  }
                }
                if(func_name) t = oyStringCopy( func_name, NULL );
                else
                {
                  if(tmp)
                  {
                    t = oyStringCopy( tmp[0] == '(' ? &tmp[1] : tmp, NULL );
                    txt = strchr(t, '+');
                    if(txt) txt[0] = '\000';
                  }
                  else
                    t = oyStringCopy( addr_infos, NULL );
                }
                if(t)
                {
                  if(j == (oy_debug ? 0 : 1))
                  {
                    oyStringAddPrintf( &text, 0,0, "%s", stack_limit >= 0 ? oyjlTermColor(oyjlBOLD, t) : t );
                    oyStringAddPrintf( &text, 0,0, "(%s) ", line_number ? stack_limit >= 0 ? oyjlTermColor(oyjlITALIC, line_number ) : line_number : "");
                  }
                  else
                  {
                    oyStringAddPrintf( &text, 0,0, "%s", stack_limit >= 0 ? oyjlTermColor(oyjlBOLD, t) : t );
                    oyStringAddPrintf( &text, 0,0, "(%s)->", line_number ? stack_limit >= 0 ? oyjlTermColor(oyjlITALIC, line_number ) : line_number  : "");
                  }
                  oyFree_m_(t);
                }
                if(addr_info) oyFree_m_(addr_info);
                if(line_number) oyFree_m_(line_number);
                if(func_name) oyFree_m_(func_name);
                if(discriminator) oyFree_m_(discriminator);
              }
              if(addr_infos) oyFree_m_( addr_infos );
              oyFree_m_( prog );
              if(command) oyFree_m_( command );
            }
            oyStringAddPrintf( &text, 0,0, "\n" );
            free(strings);
            oyFree_m_( main_prog );
          }
  return text;
}
#else
char *   oyBT                        ( int                 stack_limit OY_UNUSED )
{
  return NULL;
}
#endif

time_t             oyTime            ( )
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
double             oySeconds         ( )
{
           time_t zeit_ = oyTime();
           double teiler = TIME_DIVIDER;
           double dzeit = zeit_ / teiler;
    return dzeit;
}
double             oyClock           ( )
{ return oySeconds()*1000000; }

void               oySplitHour       ( double              hours,
                                       int               * hour,
                                       int               * minute,
                                       int               * second )
{
  *hour   = (int)floor(hours);
  *minute = (int)floor(hours*  60) - *hour  *60;
  *second = (int)floor(hours*3600) - *minute*60 - *hour*3600;
}
double   oyGetCurrentLocalHour       ( double              time,
                                       int                 gmt_diff_sec )
{
  if((time + gmt_diff_sec/3600.0) > 24.0)
    return time + gmt_diff_sec/3600.0 - 24.0;
  if((time + gmt_diff_sec/3600.0) < 0.0)
    return time + gmt_diff_sec/3600.0 + 24.0;
  else
    return time + gmt_diff_sec/3600.0;
}
double   oyGetCurrentGMTHour         ( int               * gmt_to_local_time_diff_sec )
{
  time_t cutime;         /* Time since epoch */
  struct tm * ctime;
  int    sec, min, tm_hour;
  double dtime;

  cutime = time(NULL); /* time right NOW */
  ctime = gmtime(&cutime);
  tm_hour = ctime->tm_hour;
  min = ctime->tm_min;
  sec = ctime->tm_sec;
  if(gmt_to_local_time_diff_sec)
  {
    ctime = localtime(&cutime);
    *gmt_to_local_time_diff_sec = ctime->tm_gmtoff;
  }

  dtime = tm_hour + min/60.0 + sec/3600.0;

  return dtime;
}
const char *       oyPrintTime       ( )
{
  int hour, minute, second, gmt_diff_second;
  static char time[64];

  oyGetCurrentGMTHour( &gmt_diff_second );
  oySplitHour( oyGetCurrentLocalHour( oyGetCurrentGMTHour(0), gmt_diff_second ), &hour, &minute, &second );
  sprintf( time, "%d:%.2d:%.2d.%04.d", hour, minute, second, (int)floor((oySeconds() - (long)oySeconds())*10000) );
  return time;
}


