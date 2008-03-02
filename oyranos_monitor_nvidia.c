/*
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2006  Kai-Uwe Behrmann
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
 *
 * monitor device detection
 * 
 */

/* Date:      14. 04. 2006 */

#define DEBUG 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "limits.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xinerama.h>

#include "oyranos.h"
#include "oyranos_monitor_internal.h"
#include "oyranos_helper.h"
#include "oyranos_debug.h"

#include <NVCtrlLib.h>
//#include <NvCtrlAttributes.h>

/* ---  Helpers  --- */

/* --- internal API definition --- */

unsigned char** oyGetNvidiaEdid( Display* display, int screen, size_t **size);



int
main(int argc, char **argv)
{
  Display *display = XOpenDisplay(NULL);
  size_t *size = (size_t*) calloc(sizeof(size_t), 24);
  unsigned char** data = oyGetNvidiaEdid( display, 0, &size );
  int i;
  struct DDC_EDID1 *edi=0;
  int screen_number = 32;
  int number_of_screens = 0;
  int print_help = 0;
  int put_edid = 0;

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
      if((i+1) <= argc)
      {
        screen_number = atoi(argv[i+1]); ++i;
      }
    } else

    if((strcmp(argv[i], "--put") == 0) ||
       (strcmp(argv[i], "-p") == 0) )
    {
      put_edid = 1;
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
      printf("\n");
      return 0;
  }

  for(i = 0; i < 24; ++i)
    if(data[i] && size[i])
    {
      //printf( "%d: Edid of size %d found.\n", i, (int)size[i]);
      edi = (struct DDC_EDID1*)data[i];
      if(size[i] == 128)
      {
        char *manufacturer=0,
             *model=0,             
             *serial=0;
        char *display_name = XDisplayString( display );


        fprintf( stderr, "EDID version: %d.%d\n", edi->major_version, edi->minor_version);
        oyUnrollEdid1_( edi, &manufacturer, &model, &serial, oyAllocateFunc_ );


        if(screen_number == 32 || screen_number == number_of_screens)
        {
          printf("%s %s %s %s\n", manufacturer, model, serial,
                                  display_name);
          if(put_edid)
          {
            Window w = RootWindow(display, 0);
            char atom_name[48] = {"XFree86_DDC_EDID1_RAWDATA"};
            Atom atom;

            if( number_of_screens > 0 )
              sprintf( &atom_name[strlen( atom_name )], "_%d",
                       number_of_screens);

            atom = XInternAtom( display, atom_name, False );

            XChangeProperty( display, w, atom, XA_CARDINAL,
                             8, PropModeReplace, data[i], (int)size[i] );
          }
        }

        if (manufacturer) free (manufacturer);
        if (model) free (model);
        if (serial) free (serial);
        ++number_of_screens;
      }
    }

  if(screen_number == -1)
    printf( "%d\n", number_of_screens );

  XCloseDisplay( display );

  return 0;
}

unsigned char**
oyGetNvidiaEdid( Display* display, int screen, size_t **size)
{
  int val;
//  ReturnStatus ret;
  int ret;
//  NvCtrlAttributeHandle *nvidia_handle = 0;
  int mask;
  int d;
  int bit,
      len;
  int major, minor;
  unsigned char **data = (unsigned char**) calloc(sizeof(unsigned char*), 24);

  //for(i = 0; i < 24; ++i) data[i] = NULL;

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
        //unsigned short *lut;
        //NvCtrlGetColorRamp( nvidia_handle, channel, &lut, &n );
        if( ret)
          printf(" r: %d/%d g: %d/%d b: %d/%d\n", red_n, red_bits,
                  green_n, green_bits, blue_n, blue_bits);
      }
    }

    //NvCtrlAttributeClose( nvidia_handle );
  }


  return data;
}

