/** @file oyranos_debug.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2005-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    internal helpers
 *  @internal
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

#include "oyranos_debug.h"

int level_PROG = 0;
clock_t oyranos_clock_ = 0;
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


