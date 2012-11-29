/** @file oyranos_monitor_nvidia.c
 *
 * Oyranos is an open source Colour Management System 
 * 
 *  @par Copyright:
 *            2006-2010 (C) Kai-Uwe Behrmann
 *
 *  @autor:   Kai-Uwe Behrmann <ku.b@gmx.de>
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
 *  @internal
 *  @brief example on how to set the EDID tag for multi monitor configurations.
 *  @since    2006/04/14
 *
 *  monitor device detection
 * 
 */

#define DEBUG 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>

#include "config.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
# include <X11/Xcm/XcmEdidParse.h>
#if define(HAVE_XINERAMA)
# include <X11/extensions/Xinerama.h>
#endif

#include "oyranos.h"
#include "oyranos_monitor_internal_x11.h"
#include "oyranos_monitor_internal.h"
#include "oyranos_helper.h"
#include "oyranos_debug.h"

#include <NVCtrlLib.h>
/*#include <NvCtrlAttributes.h>*/
#ifndef NV_CTRL_STRING_XINERAMA_SCREEN_INFO
#define NV_CTRL_STRING_XINERAMA_SCREEN_INFO                    26   /* R--- */
#endif
#ifndef NV_CTRL_STRING_TWINVIEW_XINERAMA_INFO_ORDER
#define NV_CTRL_STRING_TWINVIEW_XINERAMA_INFO_ORDER            27   /* RW-- */
#endif
#define ATOM_NAME  "XFree86_DDC_EDID1_RAWDATA"

/* ---  Helpers  --- */

/* --- internal API definition --- */

unsigned char** oyGetNvidiaEdid( Display* display, int screen, size_t **size);

const char *       oyNVmaskToPortName( int                 mask )
{
  static char * text = 0;
  double d = log2((double)mask);
  if(!text)
    text = (char*)malloc(128);
  text[0] = 0;
  switch(mask)
  {
    case 1:
    case 2:
    case 4:
    case 8:
    case 16:
    case 32:
    case 64:
    case 128:
         sprintf(text, "CRT-%d", (int)d);
         break;
    case 256:
    case 512:
    case 1024:
    case 2048:
    case 4096:
    case 8192:
    case 16384:
    case 32768:
         sprintf(text, "TV-%d", (int)(d-8));
         break;
    case 65536:
    case 131072:
    case 262144:
    case 524288:
    case 1048576:
    case 2097152:
    case 4194304:
    case 8388608: /* 2^23 */
         sprintf(text, "DFP-%d", (int)(d-16));
         break;
  }
  return text;
}

int
main(int argc, char **argv)
{
  Display *display = XOpenDisplay(NULL);
  size_t *size = (size_t*) calloc(sizeof(size_t), 24);
  unsigned char** data = NULL;
  int i, j, ret;
  XcmEdid_s * edi=0;
  int screen_number = 32;
  int number_of_screens = 1;
  int monitors = 0;
  int print_help = 0;
  int put_edid = 0;
  char *app_name = argv[0];
  int traditional_screens_b = 1;
# ifdef HAVE_XINERAMA
  XineramaScreenInfo* xin_screens = 0;
# endif

  if(getenv(OY_DEBUG))
  {
    int value = atoi(getenv(OY_DEBUG));
    if(value > 0)
      oy_debug = value;
  }

  if(!display) return 1;

  for( i = 1; i < argc; ++i)
  {
    if((strcmp(argv[i], "--number") == 0) ||
       (strcmp(argv[i], "-n") == 0) )
    {
      screen_number = -1;
      break;
    } else

    if((strcmp(argv[i], "--screen") == 0) ||
       (strcmp(argv[i], "-s") == 0) )
    {
      if((i+1) < argc)
      {
        screen_number = atoi(argv[i+1]); ++i;
        fprintf(stderr, "using only screen: %d\n", screen_number );
      } else {
        fprintf(stderr, "%s: missing integer argument for --screen option\n",
                app_name);
        print_help = 1;
      }
    } else

    if((strcmp(argv[i], "--put") == 0) ||
       (strcmp(argv[i], "-p") == 0) )
    {
      put_edid = 1;
    } else

    if((strcmp(argv[i], "-v") == 0) ||
       (strcmp(argv[i], "--verbose") == 0) )
    {
      oy_debug += 1;
      break;
    } else

    if((strcmp(argv[i], "--help") == 0) ||
       (strcmp(argv[i], "-h") == 0) ||
       (strcmp(argv[i], "-?") == 0) )
    {
      print_help = 1;
      break;
    } else
    {
      printf("\n Error:\n\n");
      print_help = 1;
      break;
    }
  }

  if(print_help)
  {
      print_help = 1;
      printf("\n");
      printf("Usage: \n");
      printf(" %s -h/--help      -- this help text\n", argv[0]);
      printf(" %s -n/--number    -- the number of available screens\n",argv[0]);
      printf(" %s -p/--put       -- put the edid info into the root window\n",argv[0]);
      printf(" %s -s/--screen x  -- the requested screen\n", argv[0]);
      printf("              -v/--verbose   -- print some messages\n");
      printf("\n");
      return 1;
  }

  if( ScreenCount( display ) > 1 )
  {
    number_of_screens = ScreenCount( display );
    traditional_screens_b = 1;
  }
# ifdef HAVE_XINERAMA
  else
  {
    if( XineramaIsActive( display ) )
    {
      xin_screens = XineramaQueryScreens( display, &number_of_screens );
      traditional_screens_b = 0;
    }
  }
# endif

  /*printf("ScreenCount: %d number_of_screens: %d\n", ScreenCount( display ), number_of_screens);*/

  for( i = 0; i < number_of_screens; ++i)
  {
    int monitors_in_traditional_screen = 0;
    int traditional_screen = traditional_screens_b ? i : 0;
    char *ptr = NULL,
         * p = NULL;
    char txt[24][48], * t;
    const char * ct;
    int n = 0, x = 0, y = 0, xineramas[24][2], k,
        meaningful_order = 0;

    if( !traditional_screens_b )
      monitors_in_traditional_screen = monitors;

    data = oyGetNvidiaEdid(display, i, &size);
    if(data)
    {
      for(j = 0; j < 24; ++j)
        if(data[j] && size[j])
          ++y;

      /* create a list of outputs sorted in some kind of Xinerama order */
      ret = XNVCTRLQueryStringAttribute ( display, i, 1 << j,
                            NV_CTRL_STRING_TWINVIEW_XINERAMA_INFO_ORDER, &ptr );
      if(oy_debug)
      fprintf( stderr, "found %d EDID%s\n""  order list: %s\n", y, y<=1?"":"s", ptr?ptr:"" );
      if(ret && ptr)
      {
        p = ptr;
        sprintf( txt[n], "%s", p );
        t = strchr( txt[n],',' ); if(t) t[0] = 0;
        ++n;
        while( (p = strchr( p,',' )) != NULL )
        {
          ++p; if(n >= 24) break;
          while(p[0] == ' ') ++p;
          sprintf( txt[n], "%s", p );
          t = strchr( txt[n],',' ); if(t) t[0] = 0;
          ++n;
        }
      }

      if(n > 0 && txt[0][0])
      {
        if(ptr && strchr(ptr,'-') != 0)
          meaningful_order = 1;
        else
          for(j = 0; j < 24; ++j)
          {
            if(data[j] && size[j])
            {
              ct = oyNVmaskToPortName( 1 << j );
              if(strcmp( ct, txt[0] ) == 0)
                meaningful_order = 1;
            }
          }
      }

      for(j = 0; j < 24; ++j)
      {
        xineramas[j][0] = -1;
        xineramas[j][1] = -1;
      }

      if(!meaningful_order)
      {
        fprintf( stderr, "found %d EDID%s but %d outputs  ", y, y<=1?"":"s", n );
        for(j = 0; j < n; ++j)
          fprintf( stderr, "%s ", txt[j] );
        fputs( "\n", stderr);

        /* create a xinerama output table mapping to the nvidia mask */
        /* ignore nvidia output order list */
        for(k = 0; k < y; ++k)
        {
          for(j = 0; j < 24; ++j)
          {
            if(data[j] && size[j])
            {
              ct = oyNVmaskToPortName( 1 << j );
              xineramas[x][0] = j;
              sprintf( txt[k], "%s", ct );
              xineramas[x++][1] = k;
            }
          }
        }
      } else
      {
      /* create a xinerama output table mapping to the nvidia mask */
        if(y > n)
        {
          /* expect just the main output being named in the output order list */
          for(k = 0; k < y; ++k)
          {
            if(k < n)
              t = txt[k];
            for(j = 0; j < 24; ++j)
            {
              if(data[j] && size[j])
              {
                ct = oyNVmaskToPortName( 1 << j );
                if((t && strcmp( ct, t ) == 0) ||
                   (!t && j != xineramas[x-1][0]))
                {
                  xineramas[x][0] = j;
                  xineramas[x++][1] = k;
                }
              }
            }
            t = 0;
          }
        } else
          /* rely on the nvidia output order list to name all active outputs */
          for(k = 0; k < n; ++k)
          {
            if(k < n)
              t = txt[k];
            for(j = 0; j < 24; ++j)
            {
              if(data[j] && size[j])
              {
                ct = oyNVmaskToPortName( 1 << j );
                if(strcmp( ct, t ) == 0)
                {
                  xineramas[x][0] = j;
                  xineramas[x++][1] = k;
                }
              }
            }
          }
      }
    }

    /* iterate over the xinerama output table */
    if(data)
    for(k = 0; k < y; ++k)
    {
      j = xineramas[k][0];
      t = txt[xineramas[k][1]];

      /*/printf( "%d: Edid of size %d found.\n", j, (int)size[j]);*/
      edi = (XcmEdid_s*) data[j];

      if(size[j] == 128 || size[j] == 256)
      {
        char *manufacturer=0,
             *mnft=0,
             *model=0,             
             *serial=0,
             *vendor=0;
        char  display_name[256] = {""}, display_base[256] = {""};
        double c[9];
        uint32_t week=0, year=0, mnft_id=0, prod_id=0;

        snprintf(display_name, 256, "%s", XDisplayString( display ));
        if( (ptr = strchr(display_name, ':')) != 0 )
          if( (ptr = strchr(ptr, '.')) != 0 )
            ptr[0] = '\000';

        if(strlen(display_name))
        {
          strcpy( display_base, display_name );
          snprintf( &display_name[strlen(display_name)], 256, "_%d", i );
        }

        ptr = 0;
        ret = XNVCTRLSetAttributeAndGetStatus( display, i, 1 << j,
                                    NV_CTRL_XINERAMA,
                                    NV_CTRL_XINERAMA_ON );
        ret = XNVCTRLQueryStringAttribute ( display, i, 1 << j,
                                    NV_CTRL_STRING_XINERAMA_SCREEN_INFO, &ptr );

        fprintf( stderr, "EDID version: %d.%d in .%d[%d] \"%s\" %s",
                 edi->major_version, edi->minor_version, i, j,
                 oyNVmaskToPortName( 1 << j ), (ret && ptr)?ptr:"");
        oyUnrollEdid1_( edi, &manufacturer, &mnft, &model, &serial, &vendor,
                        &week, &year, &mnft_id, &prod_id, c, oyAllocateFunc_ );

        if(model)
          fprintf( stderr, " \"%s\"", model );
        fprintf(stderr, "\t%s.%d %dx%d+%d+%d",
                      display_base,
                      monitors_in_traditional_screen,
                      xin_screens[monitors_in_traditional_screen].width,
                      xin_screens[monitors_in_traditional_screen].height,
                      xin_screens[monitors_in_traditional_screen].x_org,
                      xin_screens[monitors_in_traditional_screen].y_org
                      );
        fprintf( stderr, "\n" );
                   

        if(screen_number == 32 || screen_number == i)
        {
          DBG_PROG5_S("%s %s %s %s\n", manufacturer, model, serial, vendor,
                                  display_name);

          /* we must rely on eighter screens or Xinerama 
           * otherwise we split the behaviour without compensating missing
           * capabilities
           */

          if( traditional_screens_b &&
              monitors_in_traditional_screen > 1 )
          {
            fprintf(stderr, "%s:\n", app_name);
		    fprintf(stderr, "Will not set up EDID atom in X!\n"
                            "(The above monitor will not be detectable.)\n\n");
          } else
          if(put_edid)
          {
            Window w = RootWindow(display, traditional_screen);
            char atom_name[48] = {ATOM_NAME};
            Atom atom;

            if( !w )
            {
              fprintf(stderr, "%s:\n", app_name);
              fprintf(stderr, "\n could not find root window for display %s screen %d.\n\n", display_name, traditional_screen);
            }

            if( monitors_in_traditional_screen >= 1 )
              sprintf( &atom_name[strlen( ATOM_NAME )], "_%d",
                       monitors_in_traditional_screen);

            atom = XInternAtom( display, atom_name, False );

            if( atom == None )
            {
              fprintf(stderr, "%s:\n", app_name);
              fprintf(stderr,"\n could not find atom \"%s\" in display %s.\n\n",
                      atom_name, display_name);
            } else
              XChangeProperty( display, w, atom, XA_CARDINAL,
                               8, PropModeReplace, data[j], (int)size[j] );
          }
        }

        if (manufacturer) oyDeAllocateFunc_ (manufacturer);
        if (model) oyDeAllocateFunc_ (model);
        if (serial) oyDeAllocateFunc_ (serial);
        if (vendor) oyDeAllocateFunc_ (vendor);
        ++monitors;
        ++monitors_in_traditional_screen;
      }
    }

    if(data)
      oyDeAllocateFunc_ (data);
  }

  if( monitors < number_of_screens )
  {
    fprintf(stderr, "\n%s:", app_name);
    fprintf(stderr, "\nmissing monitors: %d\n\n", number_of_screens - monitors);
  } else
  if( monitors > number_of_screens )
  {
    fprintf(stderr, "\n%s:", app_name);
    fprintf(stderr, "\ntoo less screens: %d\n", monitors - number_of_screens);
    fprintf(stderr, "\n -> Xinerama is NOT enabled.\n\n");
  }

  if(screen_number == -1)
    DBG_PROG1_S( "%d\n", monitors );

  XCloseDisplay( display );

  return 0;
}

unsigned char**
oyGetNvidiaEdid( Display* display, int screen, size_t **size)
{
  int val;
/*/  ReturnStatus ret;*/
  int ret;
/*/  NvCtrlAttributeHandle *nvidia_handle = 0;*/
  int mask;
  int d;
  int bit,
      len;
  int major, minor;
  unsigned char **data = (unsigned char**) calloc(sizeof(unsigned char*), 24);

  /*/for(i = 0; i < 24; ++i) data[i] = NULL;*/

  {

    if(!XNVCTRLIsNvScreen ( display, screen ))
      return NULL;

#   if 0
    nvidia_handle = NvCtrlAttributeInit( display, screen,
                                     NV_CTRL_ATTRIBUTES_XF86VIDMODE_SUBSYSTEM |
                                     NV_CTRL_ATTRIBUTES_NV_CONTROL_SUBSYSTEM);
#   endif

    for (bit = 0; bit < 24; bit++)
    {
      NVCTRLAttributeValidValuesRec valid;
      mask = 1 << bit;

#     if 0
      ret = NvCtrlGetAttribute(nvidia_handle, NV_CTRL_ENABLED_DISPLAYS, &d);    
      if(ret != NvCtrlSuccess)
        continue;
#     else
      if(!XNVCTRLQueryValidAttributeValues (display, screen,
                                mask, NV_CTRL_ENABLED_DISPLAYS, &valid))
        return data;

      if(valid.type == ATTRIBUTE_TYPE_BITMASK)
        if(!XNVCTRLQueryAttribute(display, screen, mask, NV_CTRL_ENABLED_DISPLAYS, &d))
          return data;
#     endif

      if ((d & mask) == 0x0) continue;

      ret = XNVCTRLQueryAttribute( display, screen, mask,
                                   NV_CTRL_EDID_AVAILABLE, &val);

      if ((!ret) || (val != NV_CTRL_EDID_AVAILABLE_TRUE))
      {
        **size = 0;
        return data;
      }

      /* Grab EDID information */
#     if 1
      if( XNVCTRLQueryVersion ( display, &major, &minor ))
        if ((major < 1) ||
            ((major == 1) && (minor < 7)))
          return data;

      if(! XNVCTRLQueryBinaryData (display, screen, mask,
                                     NV_CTRL_BINARY_DATA_EDID,
                                     &data[bit], &len))
         return data;
#     else
      ret = NvCtrlGetBinaryAttribute(nvidia_handle,
                                     mask,
                                     NV_CTRL_BINARY_DATA_EDID,
                                     &data[bit], &len);
#     endif
      if (ret) 
        (*size)[bit] = len;

      if(0) {
        unsigned int red_n, green_n, blue_n, red_bits, green_bits, blue_bits;
        ret = XNVCTRLQueryDDCCILutSize( display, screen, mask,
                 &red_n, &green_n, &blue_n, &red_bits, &green_bits, &blue_bits);
        /*/unsigned short *lut;
        //NvCtrlGetColorRamp( nvidia_handle, channel, &lut, &n );*/
        if( ret)
          printf(" r: %d/%d g: %d/%d b: %d/%d\n", red_n, red_bits,
                  green_n, green_bits, blue_n, blue_bits);
      }
    }

    /*/NvCtrlAttributeClose( nvidia_handle );*/
  }


  return data;
}

