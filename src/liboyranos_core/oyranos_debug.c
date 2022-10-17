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
    FILE * fp = oyjlFopen( TMP_FILE, "w" );

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
                
char *   oyBT                        ( int                 stack_limit )
{
  return oyjlBT(stack_limit);
}

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


