/** @file oyranos_gamma.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2005-2008  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    gamma loader
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license: new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2005/02/03
 */

/**
 * It reads the default profile(s) from the Oyranos CMS and recalls this
 * profile(s)
 * as new default profile for a screen, including a possible curves upload to
 * the video card.
 * Currently You need xcalib installed to do the curves upload.
 * 
 */



#include "oyranos.h"
#include "oyranos_alpha.h"
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
  int database = 0;
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

  if(getenv("OYRANOS_DEBUG"))
  {
    int value = atoi(getenv("OYRANOS_DEBUG"));
    if(value > 0)
      oy_debug = value;
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
    while(pos < argc)
    {
      switch(argv[pos][0])
      {
        case '-':
            switch (argv[pos][1])
            {
              case 'e': erase = 1; monitor_profile = 0; break;
              case 'b': database = 1; monitor_profile = 0; break;
              case 'x': if( pos + 1 < argc )
                        { x = atoi( argv[pos+1] );
                          if( x == 0 && strcmp(argv[pos+1],"0") )
                            wrong_arg = "-x";
                        } else wrong_arg = "-x";
                        if(oy_debug) printf("x=%d\n",x); ++pos; break;
              case 'y': if( pos + 1 < argc )
                        { y = atoi( argv[pos+1] );
                          if( y == 0 && strcmp(argv[pos+1],"0") )
                            wrong_arg = "-y";
                        } else wrong_arg = "-y";
                        if(oy_debug) printf("y=%d\n",y); ++pos; break;
              case 'v': oy_debug += 1; break;
              case 'h':
              default:
                        printf("\n");
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
                        printf("  %s\n",               _("Query server profile:"));
                        printf("      %s\n",           argv[0]);
                        printf("            -x pos -y pos\n");
                        printf("\n");
                        printf("  %s\n",               _("Query device data base profile:"));
                        printf("      %s -b\n",        argv[0]);
                        printf("            -x pos -y pos\n");
                        printf("\n");
                        printf("  %s\n",               _("General options:"));
                        printf("      %s\n",           _("-v verbose"));
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

    if(oy_debug || (!monitor_profile && !erase))
    {
      size_t size = 0;
      oyProfile_s * prof = 0;
      const char * filename = 0;
      char * data = 0;

      if(database)
      {
        filename = oyGetMonitorProfileNameFromDB( oy_display_name, oyAllocFunc);
      } else {
        data = oyGetMonitorProfile(oy_display_name, &size, oyAllocFunc);
        prof = oyProfile_FromMem( size, data, 0, 0 );
        filename = oyProfile_GetFileName( prof, 0 );
      }
      printf("%s:%d profile \"%s\" size: %d\n",__FILE__,__LINE__,
             filename?filename:OY_PROFILE_NONE, (int)size);
    }

    /* make shure the display name is correct including the screen */
    if(monitor_profile || erase)
      oySetMonitorProfile (oy_display_name, monitor_profile);
    if(monitor_profile)
      oySetMonitorProfile (oy_display_name, 0);
  }

  if(argc == 1 || monitor_profile)
    error = oyActivateMonitorProfiles (display_name);

  if(oy_debug) {
    size_t size = 0;
    oyGetMonitorProfile(oy_display_name, &size, oyAllocFunc);
    printf("%s:%d profile size: %d\n",__FILE__,__LINE__,(int)size);
  }

  return error;
}
