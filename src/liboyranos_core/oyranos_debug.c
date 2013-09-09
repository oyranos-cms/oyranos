/** @internal
 *  @file oyranos_debug.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2005-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    internal helpers
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2005/02/01
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>  /* system() */
#ifdef HAVE_POSIX
#include <unistd.h>  /* getpid() */
#endif

#          if defined(__GNUC__) || defined(LINUX) || defined(APPLE) || defined(SOLARIS)
# include <sys/time.h>
# define   ZEIT_TEILER 10000
#          else /* WINDOWS TODO */
# define   ZEIT_TEILER CLOCKS_PER_SEC;
#          endif

#ifndef WIN32
# include <unistd.h>
#endif

#include <math.h>
#include <time.h>

#include "oyranos_debug.h"

int level_PROG = 0;
clock_t oyranos_clock_ = 0;
/** Variable  oy_debug
 *  @brief    Set debug level
 *
 *  @version  Oyranos: 0.0.1
 *  @date     2005/02/01
 *  @since    2005/02/01 (Oyranos: 0.0.1)
 */
int oy_debug = 0;

void oy_backtrace_()
{
#   define TMP_FILE "/tmp/oyranos_gdb_temp.txt"
#ifdef __POAIX__
    pid_t pid = (int)getpid();
#else
    int pid = 0;
#endif
    FILE * fp = fopen( TMP_FILE, "w" );

    if(fp)
    {
      fprintf(fp, "attach %d\n", pid);
      fprintf(fp, "backtrace\ndetach" );
      fclose(fp);
      {
        fprintf( stderr, "GDB output:\n" );
        system("gdb -batch -x " TMP_FILE);
      }
    } else
      fprintf( stderr, "could not open "TMP_FILE "\n" );
}

time_t             oyTime            ( )
{
           time_t zeit_;
           double teiler = ZEIT_TEILER;
#          if defined(__GNUC__) || defined(APPLE) || defined(SOLARIS) || defined(BSD)
           struct timeval tv;
           double tmp_d;
           gettimeofday( &tv, NULL );
           zeit_ = tv.tv_usec/(1000000/(time_t)teiler)
                   + (time_t)(modf( (double)tv.tv_sec / teiler,&tmp_d )
                     * teiler*teiler);
#          else /* WINDOWS TODO */
           zeit_ = clock();
#          endif
    return zeit_;
}
double             oySeconds         ( )
{
           time_t zeit_ = oyTime();
           double teiler = ZEIT_TEILER;
           double dzeit = zeit_ / teiler;
    return dzeit;
}
double             oyClock           ( )
{ return oySeconds()*1000000; }

