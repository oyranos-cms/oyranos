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

/** monitor device detection
 * 
 */

/* Date:      31. 01. 2005 */

#define DEBUG 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "limits.h"
#include <unistd.h>  /* intptr_t */

#include "config.h"

#if HAVE_X
# include <X11/Xlib.h>
# include <X11/Xutil.h>
# include <X11/Xatom.h>
# if HAVE_XIN
#  include <X11/extensions/Xinerama.h>
# endif
#endif

#include "oyranos.h"
#include "oyranos_internal.h"
#include "oyranos_monitor_internal.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_sentinel.h"

/* ---  Helpers  --- */

/* --- internal API definition --- */


#if (defined(HAVE_X) && !defined(__APPLE__))
int   oyGetMonitorInfo_           (const char* display,
                                   char**      manufacturer,
                                   char**      model,
                                   char**      serial,
                                   char**      display_geometry,
                                   oyAllocFunc_t allocate_func);
#endif
char* oyGetMonitorProfileName_    (const char *display_name,
                                   oyAllocFunc_t allocate_func);
char* oyGetMonitorProfile_        (const char *display_name,
                                   size_t     *size,
                                   oyAllocFunc_t allocate_func);

int   oyActivateMonitorProfile_   (const char* display_name,
                                   const char* profile_name);
int   oyActivateMonitorProfiles_  (const char* display_name);
int   oySetMonitorProfile_        (const char* display_name,
                                   const char* profile_name);
#if (defined(HAVE_X) && !defined(__APPLE__))
int   oyGetScreenFromDisplayName_ (oyMonitor_s  *disp );
int   oyGetScreenFromPosition_    (const char *display_name,
                                   int         x,
                                   int         y);
#endif
char* oyGetDisplayNameFromPosition_(const char *raw_display_name,
                                   int x,
                                   int y,
                                   oyAllocFunc_t allocate_func);
#if (defined(HAVE_X) && !defined(__APPLE__))
int   oyGetScreenGeometry_        (oyMonitor_s  *disp );
char* oyChangeScreenName_         (const char *display_name, int screen );
char**oyGetAllScreenNames_        (const char *display_name, int *n_scr );
int   oyGetGeometryIdentifier_    (oyMonitor_s  *disp );
char* oyGetAtomName_              (oyMonitor_s  *disp, const char *base );
char* oyChangeXProperty           (oyMonitor_s  *disp,
                                   const char *atom_name,
                                   int         delete_property,
                                   char       *block,
                                   size_t     *size );

/** @internal Display functions */
const char* oyDisplayName_( oyMonitor_s *disp ) { return disp->name; }
const char* oyDisplayHostName_( oyMonitor_s *disp ) { return disp->host; }
const char* oyDisplayIdentifier_( oyMonitor_s *disp ) { return disp->identifier; }
/** @internal the screen appendment for the root window properties */
char*       oyDisplayScreenIdentifier_( oyMonitor_s *disp )
{ char *number = 0;

  oyAllocHelper_m_( number, char, 24, 0, return "");
  number[0] = 0;
  if( disp->geo[1] >= 1 && !disp->screen ) sprintf( number,"_%d", disp->geo[1]);
  return number;
}
Display* oyDisplayDevice_( oyMonitor_s *disp ) { return disp->display; }
int oyDisplayDeviceScreen_( oyMonitor_s *disp ) { return disp->screen; }
int oyDisplayNumber_( oyMonitor_s *disp ) { return disp->geo[0]; }
int oyDisplayScreen_( oyMonitor_s *disp ) { return disp->geo[1]; }
int oyDisplayX_( oyMonitor_s *disp ) { return disp->geo[2]; }
int oyDisplayY_( oyMonitor_s *disp ) { return disp->geo[3]; }
int oyDisplayWidth_( oyMonitor_s *disp ) { return disp->geo[4]; }
int oyDisplayHeight_( oyMonitor_s *disp ) { return disp->geo[5]; }

int
oyFree_( void *oy_structure )
{ int error = 0;
  oyOBJECT_TYPE_e *type = (oyOBJECT_TYPE_e*) oy_structure;

  if( type )
    switch( *type )
    {
    case oyOBJECT_TYPE_DISPLAY_S:
      {
        oyMonitor_s *disp = (oyMonitor_s*)oy_structure;

        oyFree_m_( disp->name )
        oyFree_m_( disp->host )
        oyFree_m_( disp->identifier )
        disp->geo[0] = disp->geo[1] = -1;
        if( disp->display ) { XCloseDisplay( disp->display ); disp->display=0;}
      }
      break;
    default: WARNc_S (("unknown type")); error = 1;
    }
  else
    error = 1;

  return error;
}
#endif

/** @internal oyUnrollEdid1_ */
void
oyUnrollEdid1_                    (struct oyDDC_EDID1_s_ *edi,
                                   char**      manufacturer,
                                   char**      model,
                                   char**      serial,
                                   oyAllocFunc_t allocate_func)
{
  char *t = 0;
  int len, i;
  char mnf[4];

  DBG_PROG_START

  /* check */
  if(edi &&
     edi->sig[0] == 0 &&
     edi->sig[1] == 255 &&
     edi->sig[2] == 255 &&
     edi->sig[3] == 255 &&
     edi->sig[4] == 255 &&
     edi->sig[5] == 255 &&
     edi->sig[6] == 255 &&
     edi->sig[7] == 0 
    ) {
    /* verified */
  } else {
    WARNc_S(("Could not verifiy EDID"));
    DBG_PROG_ENDE
    return;
  }

  sprintf( mnf, "%c%c%c",
          (char)((edi->MNF_ID[0] & 124) >> 2) + 'A' - 1,
          (char)((edi->MNF_ID[0] & 3) << 3) + ((edi->MNF_ID[1] & 227) >> 5) + 'A' - 1,
          (char)(edi->MNF_ID[1] & 31) + 'A' - 1 );

  /*printf( "MNF_ID: %d %d SER_ID: %d %d D:%d/%d bxh:%dx%dcm %s\n",
           edi->MNF_ID[0], edi->MNF_ID[1], edi->SER_ID[0], edi->SER_ID[1],
           edi->WEEK, edi->YEAR +1990,
           edi->width, edi->height, mnf );*/


  for( i = 0; i < 4; ++i)
  {
    unsigned char *block = edi->text1 + i * 18;
    char **target = NULL;

    if(block[0] == 0 && block[1] == 0 && block[2] == 0)
    {
      if( block[3] == 255 ) { /* serial */
        target = serial;
      } else if( block[3] == 254 ) { /* vendor */
        target = manufacturer;
      } else if( block[3] == 253 ) { /* frequenz ranges */
      } else if( block[3] == 252 ) { /* model */
        target = model;
      }
      if(target)
      {
        len = strlen((char*)&block[5]); DBG_PROG_V((len))
        if(len) { DBG_PROG
          ++len;
          t = (char*)oyAllocateWrapFunc_( 16, allocate_func );
          snprintf(t, 15, (char*)&block[5]);
          t[15] = '\000';
          *target = t; DBG_PROG_S(( *target ))
        }
      }
    } 
  }

  if(!*manufacturer)
  {
    *manufacturer = (char*)oyAllocateWrapFunc_( 24, allocate_func );
    if(!strcmp(mnf,"APP"))
      sprintf(*manufacturer, "Apple");
    else if(!strcmp(mnf,"PHL"))
      sprintf(*manufacturer, "Philips");
    else if(!strcmp(mnf,"NEC"))
      sprintf(*manufacturer, "NEC");
    else if(!strcmp(mnf,"EIZ"))
      sprintf(*manufacturer, "EIZO");
    else if(!strcmp(mnf,"MEI"))
      sprintf(*manufacturer, "Panasonic");
    else if(!strcmp(mnf,"MIR"))
      sprintf(*manufacturer, "miro");
    else if(!strcmp(mnf,"SNI"))
      sprintf(*manufacturer, "Siemens Nixdorf");
    else if(!strcmp(mnf,"SNY"))
      sprintf(*manufacturer, "Sony");
    else
      sprintf(*manufacturer, mnf);
  }

  DBG_PROG_ENDE
}


#if defined( HAVE_X ) && !defined(__APPLE__)
int
oyGetMonitorInfo_                 (const char* display_name,
                                   char**      manufacturer,
                                   char**      model,
                                   char**      serial,
                                   char**      display_geometry,
                                   oyAllocFunc_t allocate_func)
{
  Display *display = 0;
  int screen = 0;
  Window w;
  Atom atom, a;
  char *atom_name;
  int actual_format_return, len;
  unsigned long nitems_return=0, bytes_after_return=0;
  unsigned char* prop_return=0;
  struct oyDDC_EDID1_s_ *edi=0;
  char *t;
  oyMonitor_s disp;

  DBG_PROG_START

  if(display_name)
    DBG_PROG_S(("display_name %s",display_name));

  if( oyGetDisplay_( display_name , &disp ) )
    return 1;

  display = oyDisplayDevice_( &disp );
  screen = oyDisplayDeviceScreen_( &disp );
  DBG_PROG_V((screen))
  w = RootWindow( display, screen); DBG_PROG_S(("w: %ld", w))
  DBG_PROG 

  if( display_geometry )
  {
    const char *identifier = oyDisplayIdentifier_( &disp );

    len = strlen( identifier );
    ++len;
    t = (char*)oyAllocateWrapFunc_( len, allocate_func );
    sprintf(t, identifier);

    *display_geometry = t; DBG_PROG_S(( *display_geometry ))
  }

  atom_name = oyGetAtomName_( &disp, "XFree86_DDC_EDID1_RAWDATA" );
  atom = XInternAtom(display, atom_name, 1);

  DBG_PROG_S(("atom: %ld", atom))

  DBG_PROG

  if(atom)
    XGetWindowProperty(display, w, atom, 0, 32, 0, AnyPropertyType, &a,
                     &actual_format_return, &nitems_return, &bytes_after_return,
                     &prop_return );


  if( nitems_return != 128 )
  {
    int error = 0;
    char * txt = malloc(1024); txt[0] = 0;

    /* test twinview edid */
    if(oy_debug)
      snprintf( txt, 1024, "OYRANOS_DEBUG=%d ", oy_debug);
    snprintf( &txt[strlen(txt)], 1024, "%s",
              "PATH=" OY_BINDIR ":$PATH; oyranos-monitor-nvidia -p" );

    error = system( txt );
    if(txt) { free(txt); txt = 0; }

    /*if(!error)*/
    {
      atom = XInternAtom(display, atom_name, 1);
      DBG_PROG_S(("atom: %ld", atom))

      if(atom)
        XGetWindowProperty(display, w, atom, 0, 32, 0, AnyPropertyType, &a,
                     &actual_format_return, &nitems_return, &bytes_after_return,
                     &prop_return );
    }
  }

  if( nitems_return != 128 ) {
    WARNc_S((_("unexpected EDID lenght %d"), (int)nitems_return))
    WARNc_S((_("Cant read hardware information from device.")))
    DBG_PROG_ENDE
    return 1;
  }

  /* convert to an deployable struct */
  edi = (struct oyDDC_EDID1_s_*) prop_return;

  oyUnrollEdid1_( edi, manufacturer, model, serial, allocate_func );

  oyFree_( &disp );
  oyFree_m_( atom_name )

  if(prop_return && nitems_return) {
    free (prop_return);
    DBG_PROG_ENDE
    return 0;
  } else {
    DBG_PROG_ENDE
    return 1;
  }
}
#endif

#ifdef __APPLE__
DisplayIDType
oyDisplayNameToOsxID( const char* display_name )
{
  DisplayIDType screenID=0;
  GDHandle device = 0;

  if(!display_name || 
     atoi(display_name) <= 0)
  {
    device = DMGetFirstScreenDevice(true); /*GetDeviceList();*/
    DMGetDisplayIDByGDevice(device, &screenID, false);
  } else
    screenID = atoi(display_name);

  return screenID;
}
#endif

char*
oyGetMonitorProfile_          (const char* display_name,
                               size_t *size,
                               oyAllocFunc_t allocate_func)
{
  char       *moni_profile=0;

#ifdef __APPLE__

  CMProfileRef prof=NULL;
  DisplayIDType screenID=0;
  CMProfileLocation loc;
  int err = 0;
  char * block = NULL;

  DBG_PROG_START

  screenID = oyDisplayNameToOsxID( display_name );

  CMGetProfileByAVID(screenID, &prof);
  CMGetProfileLocation(prof, &loc);

  err = oyGetProfileBlockOSX (prof, &block, size, allocate_func);
  moni_profile = block;
  if (prof) CMCloseProfile(prof);

#else /* HAVE_X */

  Display *display;
  int screen = 0;
  Window w;
  Atom atom = 0, a;
  int actual_format_return;
  unsigned long nitems_return=0, bytes_after_return=0;
  unsigned char* prop_return=0;
  oyMonitor_s disp;

  DBG_PROG_START

  if(display_name)
    DBG_PROG_S(("display_name %s",display_name));

  if( oyGetDisplay_( display_name, &disp ) )
    return NULL;

  display = oyDisplayDevice_( &disp );
  oyPostAllocHelper_m_( display, 1,
    WARNc_S((_("open X Display failed"))); *size = 0; DBG_PROG_ENDE return 0; )

  screen = oyDisplayDeviceScreen_( &disp ); DBG_PROG_V((screen))
  w = RootWindow(display, screen); DBG_PROG_S(("w: %ld", w))
  DBG_PROG 

  {
    char *atom_name = 0;

    atom_name = oyGetAtomName_( &disp, "_ICC_PROFILE" );

    if( atom_name )
    {
      atom = XInternAtom (display, atom_name, True);
      if (atom == None) {
        WARNc_S((_("Could not get Xatom \"%s\""), atom_name));
      }
    }
    oyFree_m_( atom_name )
  }

  DBG_PROG

  if(atom)
    XGetWindowProperty(display, w, atom, 0, INT_MAX, 0, XA_CARDINAL, &a,
                     &actual_format_return, &nitems_return, &bytes_after_return,
                     &prop_return );

  *size = nitems_return + bytes_after_return;
  moni_profile = (char*)oyAllocateWrapFunc_( *size, allocate_func );
  memcpy( moni_profile, prop_return, *size );
  XFree( prop_return );
  oyFree_( &disp );
#endif /* TODO WIN */

  DBG_PROG_ENDE
  return moni_profile;
}

char*
oyGetMonitorProfileName_          (const char* display_name,
                                   oyAllocFunc_t allocate_func)
{
  char       *moni_profile=0;

#ifdef __APPLE__

  CMProfileRef prof=NULL;
  DisplayIDType screenID=0;
  CMProfileLocation loc;

  DBG_PROG_START

  screenID = oyDisplayNameToOsxID( display_name );

  CMGetProfileByAVID(screenID, &prof);
  CMGetProfileLocation(prof, &loc);

  moni_profile = oyGetProfileNameOSX (prof, allocate_func);
  if (prof) CMCloseProfile(prof);

#else /* HAVE_X */

  char       *manufacturer=0,
             *model=0,
             *serial=0,
             *display_geometry=0;
  const char *host_name = 0;
  oyMonitor_s disp;

  DBG_PROG_START

  oyGetMonitorInfo_( display_name,
                     &manufacturer, &model, &serial, &display_geometry,
                     oyAllocateFunc_);

  if( oyGetDisplay_( display_name, &disp ) ) return NULL;
  host_name = oyDisplayHostName_ ( &disp );


  /* search the profile in the local database */
  /* It's not network transparent. */
  /* If working remotely, better fetch the whole profile instead. */
  moni_profile = oyGetDeviceProfile( oyDISPLAY, manufacturer, model, serial,
                                     host_name, display_geometry,
                                     0,0,0, allocate_func);

  if(manufacturer) free(manufacturer);
  if(model) free(model);
  if(serial) free(serial);
  if(display_geometry) free (display_geometry);
  oyFree_( &disp );

#endif

  DBG_PROG_ENDE
  return moni_profile;
}

char**
oyGetAllScreenNames_            (const char *display_name,
                                 int *n_scr)
{
  int i = 0;
  char** list = 0;

#ifdef __APPLE__

  int ids[256];
  GDHandle                                device;
  DisplayIDType                           screenID;

  device = GetDeviceList();
  while (device)
  {
    DMGetDisplayIDByGDevice(device, &screenID, false);
    ids[i++] = screenID;

    device = GetNextDevice( device );
  }

  *n_scr = i;
  oyAllocHelper_m_( list, char*, *n_scr, 0, return NULL )

  for ( i = 0; i < *n_scr ; ++i )
  {
    oyAllocHelper_m_( list[i], char, 24, 0, return NULL )
    snprintf( list[i], 24, "%d", ids[i] );
  }

#else /* HAVE_X */
  Display *display = 0;
  int len = 0;
  oyMonitor_s disp;

  *n_scr = 0;

  if( oyGetDisplay_( display_name, &disp ) )
    return NULL;

  display = oyDisplayDevice_( &disp );

  if( (len = ScreenCount( display )) == 0 )
    return 0;

# if HAVE_XIN
  /* test for Xinerama screens */
  if( len == 1 )
  if( XineramaIsActive( display ) )
  {
    int n_scr_info = 0;
    XineramaScreenInfo *scr_info = XineramaQueryScreens( display, &n_scr_info );
    oyPostAllocHelper_m_(scr_info, n_scr_info, return 0 )

    if( n_scr_info >= 1 )
      len = n_scr_info;

    XFree( scr_info );
  }
# endif

  oyAllocHelper_m_( list, char*, len, 0, return NULL )

  for (i = 0; i < len; ++i)
    if( (list[i] = oyChangeScreenName_( display_name, i )) == 0 )
      return NULL;

  *n_scr = len;
  oyFree_( &disp );

#endif

  return list;
}

#if defined( HAVE_X ) && !defined(__APPLE)
/** @internal This function will only with Xinerama hit exact results
 *  Anyway, we handle multiple screens too.
 */
int
oyGetScreenFromPosition_        (const char *display_name,
                                 int x,
                                 int y)
{
  int len = 0, i;
  char** screens = 0;
  int screen = 0;
  Display *display = 0;
  oyMonitor_s disp;

  if( oyGetDisplay_( display_name, &disp ) )
    return 0;

  display = oyDisplayDevice_( &disp );

  screens = oyGetAllScreenNames_( display_name, &len );
  oyPostAllocHelper_m_( screens, len, return 0 );

  if(len)
  if(ScreenCount( display ) > 1)
  {
    char *ptr = NULL;
    if(!display_name)
      if(!strlen(display_name))
        display_name = getenv("DISPLAY");

    if(display_name &&
       (ptr = strchr(display_name,':')) != 0)
      if( (ptr = strchr(ptr, '.')) != 0 )
        ++ptr;
    if(ptr)
    {
      Screen *scr = XScreenOfDisplay( display, atoi(ptr) );
      int scr_nr = XScreenNumberOfScreen( scr );
      screen = scr_nr;
    }

    goto clean_up;
  }

  for (i = 0; i < len; ++i)
    {
      oyMonitor_s disp;
      char *screen_name = 0;

      screen_name = oyChangeScreenName_( display_name, i );
      oyPostAllocHelper_m_( screen_name, 1, oyFree_( &disp ); return 0 )

      if( oyGetDisplay_( screen_name, &disp ) )
      {
        oyFree_( &disp );
        return 0;
      }

      DBG_PROG_S(( "i %d x %d y %d dispxy %d,%d %s", i, x,y, oyDisplayX_(&disp), oyDisplayY_(&disp) ,screen_name ))

      if( (x >= oyDisplayX_(&disp) &&
           x < oyDisplayX_(&disp) + oyDisplayWidth_(&disp) &&
           y >= oyDisplayY_(&disp) &&
           y < oyDisplayY_(&disp) + oyDisplayHeight_(&disp)) )
      {
        screen = i;
      }
      oyFree_( &disp );
      oyFree_m_( screens[i] )
      oyFree_m_( screen_name )
    }

  clean_up:
  oyFree_( &disp );
  oyFree_m_( screens )
  return screen;
}
#endif

/** @internal This function will only with Xinerama hit exact results
 *  Anyway, we handle multiple screens too.
 */
char*
oyGetDisplayNameFromPosition_     (const char *display_name,
                                   int x,
                                   int y,
                                   oyAllocFunc_t allocate_func)
{
  char *new_display_name = NULL;

#ifdef __APPLE__

  GDHandle      device;
  DisplayIDType screenID;
  Rect          r = {0,0,640,480};

  device = GetDeviceList();
  while (device)
  {
    r = (**device).gdRect;
    if( r.left <= x && x < r.right &&
        r.top <= y && y < r.bottom )
    {
      DMGetDisplayIDByGDevice(device, &screenID, false);
      new_display_name = oyAllocateWrapFunc_( 24, allocate_func );
      snprintf( new_display_name, 24, "%d", (int)screenID );

      return new_display_name;
    }
  }

#else
  int len = 0, i;
  char** screens = 0;
  int screen = -1;
  Display *display = 0;
  oyMonitor_s disp;

  if( oyGetDisplay_( display_name, &disp ) )
    return 0;

  display = oyDisplayDevice_( &disp );

  screens = oyGetAllScreenNames_( display_name, &len );
  oyPostAllocHelper_m_( screens, len, return 0 );

  if(len)
  if(ScreenCount( display ) > 1)
  {
    char *ptr = NULL;
    if(!display_name)
      if(!strlen(display_name))
        display_name = getenv("DISPLAY");

    if(display_name &&
       (ptr = strchr(display_name,':')) != 0)
      if( (ptr = strchr(ptr, '.')) != 0 )
        ++ptr;
    if(ptr)
    {
      Screen *scr = XScreenOfDisplay( display, atoi(ptr) );
      int scr_nr = XScreenNumberOfScreen( scr );
      screen = scr_nr;
    }

    goto clean_up;
  }

  for (i = 0; i < len; ++i)
    {
      oyMonitor_s disp;
      char *screen_name = 0;

      screen_name = oyChangeScreenName_( display_name, i );
      oyPostAllocHelper_m_( screen_name, 1, oyFree_( &disp ); return 0 )

      if( oyGetDisplay_( screen_name, &disp ) )
      {
        oyFree_( &disp );
        return 0;
      }

      DBG_PROG_S(( "i %d x %d y %d dispxy %d,%d %s", i, x,y, oyDisplayX_(&disp), oyDisplayY_(&disp) ,screen_name ))

      if( (x >= oyDisplayX_(&disp) &&
           x < oyDisplayX_(&disp) + oyDisplayWidth_(&disp) &&
           y >= oyDisplayY_(&disp) &&
           y < oyDisplayY_(&disp) + oyDisplayHeight_(&disp)) )
      {
        screen = i;
      }
      oyFree_( &disp );
      oyFree_m_( screen_name )
    }

  if (screen >= 0)
  {
    int len = sizeof(char) * strlen(screens[screen]) + 1;
    new_display_name = oyAllocateWrapFunc_( len, allocate_func );
    snprintf( new_display_name, len, screens[screen] ) ;
  }

  clean_up:
  oyFree_( &disp );
  for (i = 0; i < len; ++i)
    {
      oyFree_m_( screens[i] )
    }
  oyFree_m_( screens )
#endif

  return new_display_name;
}

#if defined( HAVE_X ) && !defined(__APPLE)
/** @internal
    takes a chaked display name and point as argument and
    gives a string back for search in the db
 */
int
oyGetGeometryIdentifier_         (oyMonitor_s  *disp)
{
  int len = 64;

  oyAllocHelper_m_( disp->identifier, char, len, 0, return 1 )

  snprintf( disp->identifier, len, "%d_%d+%d+%dx%d", 
            oyDisplayScreen_(disp), oyDisplayX_(disp), oyDisplayY_(disp),
            oyDisplayWidth_(disp), oyDisplayHeight_(disp) );

  return 0;
}

char*
oyGetAtomName_                  (oyMonitor_s *disp,
                                   const char   *base )
{
  int len = 64;
  char *atom_name = 0;
  char *screen_number = oyDisplayScreenIdentifier_( disp );

  oyPostAllocHelper_m_( screen_number, 1, return 0 )
  oyAllocHelper_m_( atom_name, char, len, 0, return 0 )

  snprintf( atom_name, len, "%s%s", base, screen_number );

  oyFree_m_( screen_number );

  return atom_name;
}
#endif

/** @internal
    1. get all monitors / screens / Xinerama screens
    2. get the profile names for
    3. set the profile data to a Xatom
 */
int
oyActivateMonitorProfiles_        (const char* display_name)
{
  int error = 0;
  int n_scr = 0;
  int i;
  char **screen_names = 0;

  DBG_PROG_START

  if(display_name)
    DBG_PROG_S(("display_name %s",display_name));


  screen_names = oyGetAllScreenNames_( display_name, &n_scr );

  for( i = 0; i < n_scr; ++i )
  {
    error = oyActivateMonitorProfile_( screen_names[i], 0 );
    free( screen_names[i] ); screen_names[i] = 0;
  }

  if (screen_names) free (screen_names);

  DBG_PROG_ENDE
  return error;
}

int
oyActivateMonitorProfile_         (const char* display_name,
                                   const char* profil_name )
{
  int error = 0;
  char       *profil_pathname;
  const char *profil_basename;
  char* profile_name_ = 0;
#if defined( HAVE_X ) && !defined(__APPLE__)
  oyMonitor_s disp;
  char       *dpy_name = NULL;
  char *text = 0;
#endif

  DBG_PROG_START

#if defined( HAVE_X ) && !defined(__APPLE__)
  if( oyGetDisplay_( display_name, &disp ) ) return 1;

  dpy_name = calloc( sizeof(char), 1024 );
  if( display_name && !strstr( disp.host, display_name ) )
    snprintf( dpy_name, 1024, ":%d", disp.geo[0] );
  else
    snprintf( dpy_name, 1024, "%s:%d", disp.host, disp.geo[0] );
#endif

  if(profil_name)
  {
    DBG_PROG_S(( "profil_name = %s", profil_name ));
    profil_pathname = oyGetPathFromProfileName( profil_name, oyAllocateFunc_ );
  } else
  {
    profile_name_ = oyGetMonitorProfileName_( display_name, oyAllocateFunc_);
    profil_pathname = oyGetPathFromProfileName( profile_name_, oyAllocateFunc_);
    profil_name = profile_name_;
  }

  if( profil_pathname && strlen(profil_pathname) )
  {

    if(profil_name && strrchr(profil_name,OY_SLASH_C))
      profil_basename = strrchr(profil_name,OY_SLASH_C)+1;
    else
      profil_basename = profil_name;

#ifdef __APPLE__
    {
      CMProfileLocation loc;
      CMError err = 0;
      CMProfileRef prof=NULL;
      DisplayIDType screenID = 0;

      loc.locType = cmPathBasedProfile;
      snprintf( loc.u.pathLoc.path, 255, "%s%s%s",
                profil_pathname, OY_SLASH, profil_basename);

      err = CMOpenProfile ( &prof, &loc );
      screenID = oyDisplayNameToOsxID( display_name );

      if( screenID && !err )
        err = CMSetProfileByAVID ( screenID, prof );

      CMCloseProfile( prof );
    }
#else /* HAVE_X */

    oyAllocHelper_m_( text, char, MAX_PATH, 0, error = 1; goto Clean )
    DBG_PROG_S(( "profil_pathname %s", profil_pathname ));

    /* set vcgt tag with xcalib
       not useable with multihead Xinerama at one screen

       @todo TODO xcalib should be configurable as a module
     */
    sprintf(text,"xcalib -d %s -s %d \'%s%s%s\'", dpy_name, disp.geo[1],
                               profil_pathname, OY_SLASH, profil_basename);
    {
      Display * display = oyDisplayDevice_( &disp );
      if(ScreenCount( display ) > 1 || oyDisplayScreen_( &disp ) == 0)
        error = system(text);
      if(error &&
         error != 65280) { /*/ hack*/
        WARNc_S((_("No monitor gamma curves by profile: %s"),
                oyNoEmptyName_m_(profil_basename) ))
      }
    }

    DBG_PROG_S(( "system: %s", text ))

    /* set _ICC_PROFILE atom in X like with xicc */
    {
      Display *display;
      Atom atom = 0;
      int screen = 0;
      Window w;

      unsigned char *moni_profile=0;
      size_t      size=0;
      char       *atom_name=0;
      int         result = 0;

      if(display_name)
        DBG_PROG_S(("display_name %s",display_name));

      display = oyDisplayDevice_( &disp );

      screen = oyDisplayDeviceScreen_( &disp );
      DBG_PROG_V((screen))
      w = RootWindow(display, screen); DBG_PROG_S(("w: %ld", w))

      moni_profile = oyGetProfileBlock( profil_name, &size, oyAllocateFunc_ );
      if(!size || !moni_profile)
        WARNc_S((_("Error obtaining profile")));

      atom_name = oyGetAtomName_( &disp, "_ICC_PROFILE" );
      if( atom_name )
      {
        atom = XInternAtom (display, atom_name, False);
        if (atom == None) {
          WARNc_S((_("Error setting up atom \"%s\""), atom_name));
        }
      } else WARNc_S((_("Error setting up atom")));

      if( atom )
      result = XChangeProperty( display, w, atom, XA_CARDINAL,
                       8, PropModeReplace, moni_profile, (int)size );

      oyFree_m_( moni_profile )
      oyFree_m_( atom_name )
    }

    DBG_PROG_S(( "system: %s", text ))
    oyFree_m_( text );
#endif
  }

#if defined( HAVE_X ) && !defined(__APPLE__)
  Clean:
  oyFree_( &disp );
#endif
  if(profil_pathname) oyFree_m_( profil_pathname );
  if(profile_name_) oyFree_m_( profile_name_ );

  DBG_PROG_ENDE
  return error;
}

int
oySetMonitorProfile_              (const char* display_name,
                                   const char* profil_name )
{
  int error = 0;

#ifdef __APPLE__

  error = oyActivateMonitorProfile_( display_name, profil_name );

#else /* HAVE_X */

  char       *manufacturer=0,
             *model=0,
             *serial=0,
             *display_geometry=0;
  char       *profil_pathname = 0;
  oyMonitor_s disp;

  DBG_PROG_START

  error  =
    oyGetMonitorInfo_ (display_name,
                       &manufacturer, &model, &serial, &display_geometry,
                       oyAllocateFunc_);

  if( oyGetDisplay_( display_name, &disp ) ) {
    DBG_PROG_ENDE
    return error;
  }

  if(!profil_name)
  {
    /* unset the _ICC_PROFILE atom in X */
      Display *display;
      Atom atom;
      int screen = 0;
      Window w;
      char *atom_name = 0;

      if(display_name)
        DBG_PROG_S(("display_name %s",display_name));

      display = oyDisplayDevice_( &disp );

      screen = oyDisplayDeviceScreen_( &disp );
      DBG_PROG_V((screen))
      w = RootWindow(display, screen); DBG_PROG_S(("w: %ld", w))

      DBG_PROG

      atom_name = oyGetAtomName_( &disp, "_ICC_PROFILE" );
      atom = XInternAtom (display, atom_name, True);
      if (atom == None) {
        WARNc_S((_("Error getting atom \"%s\""), atom_name));
      }

      XDeleteProperty( display, w, atom );

      {
        char *dpy_name = strdup( display_name );
        char  command[1024];
        char *ptr = NULL;
        int r;

        if( (ptr = strchr(dpy_name,':')) != 0 )
          if( (ptr = strchr(ptr,'.')) != 0 )
            ptr[0] = '\000';

        snprintf(command, 1024, "xgamma -gamma 1.0 -screen %d -display %s", disp.geo[1], dpy_name);
        r = system( command );
      }

      oyFree_m_( atom_name )
      DBG_PROG
    goto finish;
  }

  DBG_PROG_S(( "profil_name = %s", profil_name ))

  error =  oySetDeviceProfile(oyDISPLAY, manufacturer, model, serial,
                              oyDisplayHostName_(&disp), display_geometry,
                              0,0,0, profil_name, 0,0);

  oyFree_( &disp );

  profil_pathname = oyGetPathFromProfileName( profil_name, oyAllocateFunc_ );
  DBG_PROG_S(( "profil_pathname %s", profil_pathname ))

  if( profil_pathname ) {
    error = oyActivateMonitorProfile_(display_name, profil_name);
  }

  finish:
  DBG_PROG
  if (manufacturer) free (manufacturer);
  if (model) free (model);
  if (serial) free (serial);
  if (display_geometry) free (display_geometry);
  if (profil_pathname) free (profil_pathname);
#endif

  DBG_PROG_ENDE
  return error;
}

#if defined( HAVE_X ) && !defined(__APPLE)
int
oyGetDisplayNumber_        (oyMonitor_s *disp)
{
  int dpy_nummer = 0;
  const char *display_name = oyDisplayName_(disp);

  DBG_PROG_START

  if( display_name )
  {
    char ds[8];             /* display.screen*/
    const char *txt = strchr( display_name, ':' );
    
    if( !txt )
    { WARNc_S(( "invalid display name" ))
      return -1;
    }

    ++txt;
    snprintf( ds, 8, txt );
    if( strrchr( ds, '.' ) )
    {
      char *end = strchr( ds, '.' );
      if( end )
        *end = 0;
    }
    dpy_nummer = atoi( ds );
  }

  DBG_PROG_ENDE
  return dpy_nummer;
}

int
oyGetScreenFromDisplayName_        (oyMonitor_s *disp)
{
  int scr_nummer = 0;
  const char *display_name = oyDisplayName_(disp);

  DBG_PROG_START

  if( display_name )
  {
    char ds[8];             /* display.screen*/
    const char *txt = strchr( display_name, ':' );
    
    if( !txt )
    { WARNc_S(( "invalid display name" ))
      return -1;
    }

    snprintf( ds, 8, txt );
    if( strrchr( display_name, '.' ) )
    {
      char *nummer_text = strchr( ds, '.' );
      if( nummer_text )
        scr_nummer = atoi( &nummer_text[1] );
    }
  }

  DBG_PROG_ENDE
  return scr_nummer;
}


/**  @internal
 *  extract the host name or get from environment
 */
char*
oyExtractHostName_           (const char* display_name)
{
  char* host_name = 0;

  DBG_PROG_START

  oyAllocHelper_m_( host_name, char, strlen( display_name ) + 48,0,return NULL);

  /* Is this X server identifyable? */
  if(!display_name)
  {
    char *host = getenv ("HOSTNAME");
    if (host) {
        sprintf( host_name, host );
    }
  } else if (strchr(display_name,':') == display_name ||
             !strchr( display_name, ':' ) )
  {
    char *host = getenv ("HOSTNAME");
    /* good */
    if (host) {
        sprintf( host_name, host );
    }
  } else if ( strchr( display_name, ':' ) )
  {
    char* ptr = 0;
    sprintf( host_name, display_name );
    ptr = strchr( host_name, ':' );
    ptr[0] = 0;
  }

  DBG_PROG_S(( "host_name = %s", host_name ))

  DBG_PROG_ENDE
  return host_name;
}

/** @internal Do a full check and change the screen name,
 *  if the screen arg is appropriate. Dont care about the host part
 */
char*
oyChangeScreenName_                (const char* display_name,
                                    int         screen)
{
  char* host_name = 0;

  DBG_PROG_START

  /* Is this X server identifyable? */
  if(!display_name)
    display_name = ":0.0";


  oyAllocHelper_m_( host_name, char, strlen( display_name ) + 48,0,return NULL);

  sprintf( host_name, display_name );

  /* add screen */
  {
    const char *txt = strchr( host_name, ':' );

    /* fail if no display was given */
    if( !txt )
    { WARNc_S(( "invalid display name" ))
      host_name[0] = 0;
      return host_name;
    }

    if( !strchr( display_name, '.' ) )
    {
      sprintf( &host_name[ strlen(host_name) ], ".%d", screen );
    } else
    if( screen >= 0 )
    {
      char *txt_scr = strchr( txt, '.' );
      sprintf( txt_scr, ".%d", screen );
    }
  }

  DBG_PROG_S(( "host_name = %s", host_name ))

  DBG_PROG_ENDE
  return host_name;
}

/** @internal get the geometry of a screen 
 *
 *  @param          disp      display info structure
 *  @return         error
 */
int
oyGetScreenGeometry_            (oyMonitor_s *disp)
{
  int error = 0;
  int screen = 0;

  disp->display = XOpenDisplay (disp->name);

  /* switch to Xinerama mode */
  if( !disp->display ) {
    char *text = oyChangeScreenName_( disp->name, 0 );
    oyPostAllocHelper_m_( text, 1, return 1 )

    disp->display = XOpenDisplay( text );
    oyFree_m_( text );

    if( !disp->display )
      oyPostAllocHelper_m_( disp->display, 1,
                            WARNc_S((_("open X Display failed"))); return 1 )

    disp->screen = 0;
  }

  disp->geo[0] = oyGetDisplayNumber_( disp );
  disp->geo[1] = screen = oyGetScreenFromDisplayName_( disp );

# if HAVE_XIN
  if( XineramaIsActive( disp->display ) )
  {
    int n_scr_info = 0;

    XineramaScreenInfo *scr_info = XineramaQueryScreens( disp->display,
                                                         &n_scr_info );
    oyPostAllocHelper_m_(scr_info, n_scr_info, return 1 )

    if( !scr_info )
    {
      WARNc_S((_("Xinerama request failed")))
      return 1;
    }
    {
        disp->geo[1] = screen;
        disp->geo[2] = scr_info[screen].x_org;
        disp->geo[3] = scr_info[screen].y_org;
        disp->geo[4] = scr_info[screen].width;
        disp->geo[5] = scr_info[screen].height;
    }
    XFree( scr_info );
  }
  else
# endif
  {
    Screen *scr = XScreenOfDisplay( disp->display, screen );
    oyPostAllocHelper_m_(scr, 1, WARNc_S((_("open X Screen failed"))); return 1;)
    {
        disp->geo[1] = screen;
        disp->geo[2] = 0;
        disp->geo[3] = 0;
        disp->geo[4] = XWidthOfScreen( scr );
        disp->geo[5] = XHeightOfScreen( scr );
        disp->screen = screen;
    }
  }

  return error;
}

int
oyGetDisplay_                      (const char *display_name,
                                    oyMonitor_s  *disp)
{
  int error = 0;
  int i = 0;

  DBG_PROG_START

  disp->name = 0;
  disp->host = 0;
  disp->identifier = 0;
  disp->display = 0;
  disp->screen = 0;

  disp->type = oyOBJECT_TYPE_DISPLAY_S;
  if( display_name )
  {
    if( strlen( display_name ) )
      disp->name = strdup( display_name );
  } else
  {
    if(getenv("DISPLAY") && strlen(getenv("DISPLAY")))
      disp->name = strdup( getenv("DISPLAY") );
    else
      disp->name = strdup( ":0" );
  }


  if( !error &&
      (disp->host = oyExtractHostName_( disp->name )) == 0 )
    error = 1;

  for( i = 0; i < 6; ++i ) disp->geo[i] = -1;
  if( !error &&
      oyGetScreenGeometry_( disp ) )
    error = 1;

  if( !error &&
      oyGetGeometryIdentifier_( disp ) ) 
    error = 1;


  DBG_PROG_ENDE
  return error;
}
#endif


/* separate from the internal functions */

/** @brief pick up monitor information with Xlib
 *  @deprecated because sometimes is no ddc information available
 *  @todo include connection information - grafic cart
 *
 *  @param      display       the display string
 *  @param[out] manufacturer  the manufacturer of the monitor device
 *  @param[out] model         the model of the monitor device
 *  @param[out] serial        the serial number of the monitor device
 *  @param      allocate_func the allocator for the above strings
 *  @return     error
 *
 */
int
oyGetMonitorInfo                  (const char* display,
                                   char**      manufacturer,
                                   char**      model,
                                   char**      serial,
                                   oyAllocFunc_t allocate_func)
{
  int err = 0;

  DBG_PROG_START
  if( oyExportStart_(EXPORT_MONITOR))
    oyActivateMonitorProfiles_(display);

#if (defined(HAVE_X) && !defined(__APPLE__))
  err = oyGetMonitorInfo_( display, manufacturer, model, serial, 0,
                           allocate_func);
#else
  err = 1;
#endif

  DBG_PROG_V(( strlen(*manufacturer) ))
  if(*manufacturer)
    DBG_PROG_S(( *manufacturer ));
  if(*model)
    DBG_PROG_S(( *model ));
  if(*serial)
    DBG_PROG_S(( *serial ));

  oyExportEnd_();
  DBG_PROG_ENDE
  return err;
}

/** @brief get the monitor profile

 *  @param      display       the display string
 *  @param[out] size          the size of profile
 *  @param      allocate_func function used to allocate memory for the profile
 *  @return                   the memory block containing the profile
 */
char*
oyGetMonitorProfile           (const char* display,
                               size_t *size,
                               oyAllocFunc_t allocate_func)
{
  char* moni_profile = 0;

  DBG_PROG_START
  if( oyExportStart_(EXPORT_PATH | EXPORT_SETTING | EXPORT_MONITOR) )
    oyActivateMonitorProfiles_(display);

  moni_profile = oyGetMonitorProfile_( display, size, allocate_func );

  oyExportEnd_();
  DBG_PROG_ENDE
  return moni_profile;
}

/** @brief get the monitor profile filename

 *  @param      display       the display string
 *  @param      allocate_func function used to allocate memory for the string
 *  @return                   the profiles filename (if localy available)
 */
char*
oyGetMonitorProfileName           (const char* display,
                                   oyAllocFunc_t allocate_func)
{
  char* moni_profile = 0;

  DBG_PROG_START
  if( oyExportStart_(EXPORT_PATH | EXPORT_SETTING | EXPORT_MONITOR) )
    oyActivateMonitorProfiles_(display);

  moni_profile = oyGetMonitorProfileName_( display, allocate_func );

  oyExportEnd_();
  DBG_PROG_ENDE
  return moni_profile;
}

/** @brief set the monitor profile by filename

 *  @param      display_name  the display string
 *  @param      profil_name   the file to use as monitor profile
 *  @return                   error
 */
int
oySetMonitorProfile               (const char* display_name,
                                   const char* profil_name )
{
  int error = 0;

  DBG_PROG_START
  if( oyExportStart_(EXPORT_PATH | EXPORT_SETTING | EXPORT_MONITOR) )
    oyActivateMonitorProfiles_(display_name);

  error = oySetMonitorProfile_( display_name, profil_name );

  oyExportEnd_();
  DBG_PROG_ENDE
  return error;
}

/** @brief activate the monitor using the stored configuration

 *  @param      display_name  the display string
 *  @return                   error
 *  @see oySetMonitorProfile for permanently configuring a monitor
 */
int
oyActivateMonitorProfiles         (const char* display_name)
{
  int error = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  error = oyActivateMonitorProfiles_( display_name );

  oyExportEnd_();
  DBG_PROG_ENDE
  return error;
}

/** @brief screen number from position
 *
 *  This function will hit exact results only with Xinerama. \n
 *  a platform specific function
 *
 *  @param      display_name  the display string
 *  @param      x             x position on screen
 *  @param      y             y position on screen
 *  @return                   screen
 */
int
oyGetScreenFromPosition         (const char *display_name,
                                 int x,
                                 int y)
{
  int screen = 0;

  DBG_PROG_START
  if( oyExportStart_(EXPORT_MONITOR) )
    oyActivateMonitorProfiles_(display_name);

#if (defined(HAVE_X) && !defined(__APPLE__))
  screen = oyGetScreenFromPosition_( display_name, x,y );
#endif

  DBG_PROG_S(( "x %d y %d screen %d\n", x,y,screen ));

  oyExportEnd_();
  DBG_PROG_ENDE
  return screen;
}

/** @brief display name from position
 *
 *  This function will hit exact results only with Xinerama.
 *
 *  @param      raw_display_name  raw display string
 *  @param      x             x position on screen
 *  @param      y             y position on screen
 *  @param      allocate_func function used to allocate memory for the string
 *  @return                   display name
 */
char*
oyGetDisplayNameFromPosition      (const char *display_name,
                                   int x,
                                   int y,
                                   oyAllocFunc_t allocate_func)
{
  char *new_display_name = 0;

  DBG_PROG_START
  if( oyExportStart_(EXPORT_MONITOR) )
    oyActivateMonitorProfiles_(display_name);

  new_display_name = oyGetDisplayNameFromPosition_( display_name, x,y,
                                                    allocate_func );
  DBG_PROG_S(( "x %d y %d new_display_name %s\n", x,y,new_display_name ));

  oyExportEnd_();
  DBG_PROG_ENDE
  return new_display_name;
}

