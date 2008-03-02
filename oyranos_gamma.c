/*
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2005-2007  Kai-Uwe Behrmann
 *
 * Autor: Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * -----------------------------------------------------------------------------
 */

/** @file @internal
 * @brief gamma loader - put it somethere in your xinitrc
 *
 * It reads the default profile(s) from the Oyranos CMS and recalls this
 * profile(s)
 * as new default profile for a screen, including a possible curves upload to
 * the video card.
 * Currently You need xcalib installed to do the curves upload.
 * 
 */

/* Date:      03. 02. 2005 */


#include "oyranos.h"
#include "oyranos_monitor.h"
#include "oyranos_debug.h"
#include "oyranos_internal.h"
#include "oyranos_config.h"
#include "oyranos_version.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void* oyAllocFunc(size_t size) {return malloc (size);}

int main( int argc , char** argv )
{
  char *display_name = getenv("DISPLAY");
  char *monitor_profile = 0;
  int error = 0;
  int erase = 0;
  char *ptr = NULL;
  int x = 0, y = 0;
  char *oy_display_name = NULL;

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif
  oyI18NInit_();

  if(!display_name)
  {
    printf("DISPLAY variable not set: giving up\n");
    error = 1;
    return error;
  }

  /* cut off the screen information */
  if(display_name &&
     (ptr = strchr(display_name,':')) != 0)
    if( (ptr = strchr(ptr, '.')) != 0 )
      ptr[0] = '\000';

  if(argc != 1)
  {
    int pos = 1;
    char *wrong_arg = 0;
    printf("argc: %d\n", argc);
    while(pos < argc)
    {
      switch(argv[pos][0])
      {
        case '-':
            switch (argv[pos][1])
            {
              case 'e': erase = 1; monitor_profile = 0; break;
              case 'x': if( pos + 1 < argc )
                        { x = atoi( argv[pos+1] );
                          if( x == 0 && strcmp(argv[pos+1],"0") )
                            wrong_arg = "-x";
                        } else wrong_arg = "-x";
                        if(oy_debug) printf("x=%d\n",x); ++pos; break;
              case 'y': if( pos + 1 < argc )
                        { y = atoi( argv[pos+1] ); ++pos; break;
                          if( y == 0 && strcmp(argv[pos+1],"0") )
                            wrong_arg = "-y";
                        } else wrong_arg = "-y";
                        if(oy_debug) printf("y=%d\n",y); ++pos; break;
              case 'h': printf("\n");
                        printf("oyranos-monitor v%d.%d.%d %s\n",
                        OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C,
                                _("is a colour profile administration tool for monitors"));
                        printf("%s\n",                 _("Usage"));
                        printf("  %s\n",               _("Set new profile:"));
                        printf("      %s\n",           argv[0]);
                        printf("            -x pos -y pos  %s\n",
                                                       _("profile name"));
                        printf("\n");
                        printf("  %s\n",               _("Unset profile:"));
                        printf("      %s -e\n",        argv[0]);
                        printf("            -x pos -y pos\n");
                        printf("\n");
                        printf("  %s\n",               _("Activate profiles:"));
                        printf("      %s\n",           argv[0]);
                        printf("\n");
                        exit (0);
                        break;
            }
            break;
        default:
            monitor_profile = argv[pos];
            /* activate all profiles at once */
            /*error = oyActivateMonitorProfiles (display_name); */

            erase = 0;
      }
      if( wrong_arg )
      {
        printf("%s %s\n", _("wrong argument to option:"), wrong_arg);
        exit(1);
      }
      ++pos;
    }
    if(oy_debug) printf( "%s\n", argv[1] );

    oy_display_name = oyGetDisplayNameFromPosition( display_name, x,y,
                                                    oyAllocFunc);

    if(oy_debug) {
      size_t size = 0;
      oyGetMonitorProfile(oy_display_name, &size, oyAllocFunc);
      printf("%s:%d Profilgroesse: %d\n",__FILE__,__LINE__,(int)size);
    }

    /* make shure the display name is correct including the screen */
    oySetMonitorProfile (oy_display_name, monitor_profile);
  }

  /* check the default paths */
  oyPathAdd( OY_PROFILE_PATH_USER_DEFAULT );

  error = oyActivateMonitorProfiles (display_name);

  if(oy_debug) {
    size_t size = 0;
    oyGetMonitorProfile(oy_display_name, &size, oyAllocFunc);
    printf("%s:%d Profilgroesse: %d\n",__FILE__,__LINE__,(int)size);
  }

  return error;
}
