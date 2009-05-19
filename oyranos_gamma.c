/** @file oyranos_gamma.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2005-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    gamma loader
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2005/02/03
 *
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
void  oyDeAllocFunc ( oyPointer ptr) { if(ptr) free (ptr); }

int main( int argc , char** argv )
{
  char *display_name = getenv("DISPLAY");
  char *monitor_profile = 0;
  int error = 0;
  int erase = 0;
  int list = 0;
  int database = 0;
  char *ptr = NULL;
  int x = 0, y = 0;
  char *oy_display_name = NULL;
  oyProfile_s * prof = 0;
  size_t size = 0;
  const char * filename = 0;
  char * data = 0;

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif
  oyI18NInit_();

#ifndef __APPLE__
  if(!display_name)
  {
    WARNc_S( _("DISPLAY variable not set: giving up.") );
    error = 1;
    return error;
  }
#endif

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
              case 'l': list = 1; monitor_profile = 0; break;
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
                        printf("  %s\n",               _("List devices:"));
                        printf("      %s -l\n",        argv[0]);
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

    if(!monitor_profile && !erase && !list)
    {
      if(database)
      {
        filename = oyGetMonitorProfileNameFromDB( oy_display_name, oyAllocFunc);
        prof = oyProfile_FromFile( filename, 0, 0 );
        data = oyProfile_GetMem( prof, &size, 0, oyAllocFunc );
      } else {
        data = oyGetMonitorProfile(oy_display_name, &size, oyAllocFunc);
        prof = oyProfile_FromMem( size, data, 0, 0 );
        filename = oyProfile_GetFileName( prof, 0 );
      }
      if(size && data) oyDeAllocFunc( data ); data = 0;

      printf("%s:%d profile \"%s\" size: %d\n",__FILE__,__LINE__,
             filename?filename:OY_PROFILE_NONE, (int)size);

      oyProfile_Release( &prof );
    }

    if(list)
    {
      char * text = 0;
      uint32_t n = 0, i;
      oyConfigs_s * devices = 0;
      oyConfig_s * c = 0;

      error = oyDevicesGet( 0, "monitor", 0, &devices );
      n = oyConfigs_Count( devices );
      if(!error)
      {
        for(i = 0; i < n; ++i)
        {
          c = oyConfigs_Get( devices, i );

          if(oy_debug)
          printf("------------------------ %d ---------------------------\n",i);

          error = oyDeviceGetInfo( c, oyNAME_NICK, 0, &text, 0 );
          printf("\"%s\" ", text? text:"???");
          error = oyDeviceGetInfo( c, oyNAME_NAME, 0, &text, 0 );
          printf("%s\n", text? text:"???");

          if(oy_debug)
          {
            error = oyDeviceGetInfo( c, oyNAME_DESCRIPTION, 0, &text, 0 );
            printf("%s\n", text?  text:"???");
          }

          if(text)
            free( text );

          /* verbose adds */
          if(oy_debug)
          {
            oyDeviceAskProfile( c, &prof );
            data = oyProfile_GetMem( prof, &size, 0, oyAllocFunc);
            if(size && data)
              oyDeAllocFunc( data );
            filename = oyProfile_GetFileName( prof, 0 );
            printf( "%s:%d server profile \"%s\" size: %d\n",__FILE__,__LINE__,
                    filename?filename:OY_PROFILE_NONE, (int)size );

            oyDeviceProfileFromDB( c, &text, oyAllocFunc );
            printf( "%s:%d DB profile \"%s\"\n  DB registration key set: %s\n",
                    __FILE__,__LINE__,
                    text?text:OY_PROFILE_NONE,
                    oyConfig_FindString( c, "key_set_name", 0 ) );

            oyProfile_Release( &prof );
            oyDeAllocFunc( text );
          }

          oyConfig_Release( &c );
        }
      }
      oyConfigs_Release( &devices );
    }

    /* make shure the display name is correct including the screen */
    if(monitor_profile)
      oySetMonitorProfile (oy_display_name, monitor_profile);
    if(monitor_profile || erase)
      oySetMonitorProfile (oy_display_name, 0);
  }

  if(argc == 1 || monitor_profile)
    error = oyActivateMonitorProfiles (display_name);

  if(oy_display_name)
    oyDeAllocFunc(oy_display_name);

  return error;
}
