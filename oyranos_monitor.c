/** @file oyranos_monitor.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2005-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    monitor device detection
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2005/01/31
 */

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
# ifdef HAVE_XF86VMODE
#  include <X11/extensions/xf86vmode.h>
# endif
#endif

#include "oyranos.h"
#include "oyranos_internal.h"
#include "oyranos_monitor.h"
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
                                   oyAlloc_f     allocate_func);
#endif
char* oyGetMonitorProfileName_    (const char *display_name,
                                   oyAlloc_f     allocate_func);
char* oyGetMonitorProfile_        (const char *display_name,
                                   size_t     *size,
                                   oyAlloc_f     allocate_func);

int   oyActivateMonitorProfile_   (const char* display_name,
                                   const char* profile_name);
int   oyActivateMonitorProfiles_  (const char* display_name);
int   oySetMonitorProfile_        (const char* display_name,
                                   const char* profile_name);
#if (defined(HAVE_X) && !defined(__APPLE__))
int   oyMonitor_getScreenFromDisplayName_( oyMonitor_s   * disp );
int   oyGetScreenFromPosition_    (const char *display_name,
                                   int         x,
                                   int         y);
#endif
char* oyGetDisplayNameFromPosition_(const char *raw_display_name,
                                   int x,
                                   int y,
                                   oyAlloc_f     allocate_func);
#if (defined(HAVE_X) && !defined(__APPLE__))
int   oyMonitor_getScreenGeometry_   ( oyMonitor_s       * disp );
int   oyMonitor_getGeometryIdentifier_(oyMonitor_s       * disp );
char* oyMonitor_getAtomName_         ( oyMonitor_s       * disp,
                                       const char        * base );
char* oyChangeScreenName_            ( const char        * display_name,
                                       int                 screen );
char**oyGetAllScreenNames_        (const char *display_name, int *n_scr );

/** @internal Display functions */
const char* oyMonitor_name_( oyMonitor_s *disp ) { return disp->name; }
const char* oyMonitor_hostName_( oyMonitor_s *disp ) { return disp->host; }
const char* oyMonitor_identifier_( oyMonitor_s *disp ) { return disp->identifier; }
/** @internal the screen appendment for the root window properties */
char*       oyMonitor_screenIdentifier_( oyMonitor_s *disp )
{ char *number = 0;

  oyAllocHelper_m_( number, char, 24, 0, return "");
  number[0] = 0;
  if( disp->geo[1] >= 1 && !disp->screen ) sprintf( number,"_%d", disp->geo[1]);
  return number;
}
Display* oyMonitor_device_( oyMonitor_s *disp ) { return disp->display; }
int oyMonitor_deviceScreen_( oyMonitor_s *disp ) { return disp->screen; }
int oyMonitor_number_( oyMonitor_s *disp ) { return disp->geo[0]; }
int oyMonitor_screen_( oyMonitor_s *disp ) { return disp->geo[1]; }
int oyMonitor_x_( oyMonitor_s *disp ) { return disp->geo[2]; }
int oyMonitor_y_( oyMonitor_s *disp ) { return disp->geo[3]; }
int oyMonitor_width_( oyMonitor_s *disp ) { return disp->geo[4]; }
int oyMonitor_height_( oyMonitor_s *disp ) { return disp->geo[5]; }

int
oyFree_( void *oy_structure )
{ int error = 0;
  oyOBJECT_e *type = (oyOBJECT_e*) oy_structure;

  if( type )
    switch( *type )
    {
    case oyOBJECT_MONITOR_S:
      {
        oyMonitor_s *disp = (oyMonitor_s*)oy_structure;

        oyFree_m_( disp->name )
        oyFree_m_( disp->host )
        oyFree_m_( disp->identifier )
        disp->geo[0] = disp->geo[1] = -1;
        if( disp->display ) { XCloseDisplay( disp->display ); disp->display=0;}
      }
      break;
    default: WARNc_S ("unknown type"); error = 1;
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
                                   oyAlloc_f     allocate_func)
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
    WARNc_S("Could not verifiy EDID")
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
          oySnprintf_(t, 15, (char*)&block[5]);
          t[15] = '\000';
          *target = t; DBG_PROG_S( *target )
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
                                   oyAlloc_f     allocate_func)
{
  Display *display = 0;
  int screen = 0;
  Window w = 0;
  Atom atom, a;
  char *atom_name;
  int actual_format_return, len;
  unsigned long nitems_return=0, bytes_after_return=0;
  unsigned char* prop_return=0;
  struct oyDDC_EDID1_s_ *edi=0;
  char *t;
  oyMonitor_s * disp = 0;

  DBG_PROG_START

  if(display_name)
    DBG_PROG1_S("display_name %s",display_name);

  disp = oyMonitor_newFrom_( display_name );
  if(!disp)
    return 1;

  display = oyMonitor_device_( disp );
  screen = oyMonitor_deviceScreen_( disp );
  DBG_PROG_V((screen))
  if(display)
  {
    w = RootWindow( display, screen); DBG_PROG1_S("w: %ld", w)
  } else {
    return 1;
  }
  DBG_PROG 

  if( display_geometry )
  {
    const char *identifier = oyMonitor_identifier_( disp );

    len = strlen( identifier );
    ++len;
    t = (char*)oyAllocateWrapFunc_( len, allocate_func );
    sprintf(t, identifier);

    *display_geometry = t; DBG_PROG_S( *display_geometry )
  }

  atom_name = oyMonitor_getAtomName_( disp, "XFree86_DDC_EDID1_RAWDATA" );
  atom = XInternAtom(display, atom_name, 1);

  DBG_PROG1_S("atom: %ld", atom)

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
      oySnprintf1_( txt, 1024, "OYRANOS_DEBUG=%d ", oy_debug);
    oySnprintf1_( &txt[strlen(txt)], 1024, "%s",
              "PATH=" OY_BINDIR ":$PATH; oyranos-monitor-nvidia -p" );

    error = system( txt );
    if(txt) { oyDeAllocateFunc_(txt); txt = 0; }

    /*if(!error)*/
    {
      atom = XInternAtom(display, atom_name, 1);
      DBG_PROG1_S("atom: %ld", atom)

      if(atom)
        XGetWindowProperty(display, w, atom, 0, 32, 0, AnyPropertyType, &a,
                     &actual_format_return, &nitems_return, &bytes_after_return,
                     &prop_return );
    }
  }

  if( nitems_return != 128 ) {
    WARNc4_S("\n\t  %s %d; %s %s",_("unexpected EDID lenght"),
               (int)nitems_return,
               atom_name,
               _("Cant read hardware information from device."))
    
    goto CleanUp;
  }

  /* convert to an deployable struct */
  edi = (struct oyDDC_EDID1_s_*) prop_return;

  oyUnrollEdid1_( edi, manufacturer, model, serial, allocate_func );

  CleanUp:

  oyMonitor_release_( &disp );
  oyFree_m_( atom_name )

  if(prop_return && nitems_return) {
    oyDeAllocateFunc_ (prop_return);
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
oyMonitor_nameToOsxID( const char* display_name )
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
                               oyAlloc_f     allocate_func)
{
  char       *moni_profile=0;

#ifdef __APPLE__

  CMProfileRef prof=NULL;
  DisplayIDType screenID=0;
  CMProfileLocation loc;
  int err = 0;
  char * block = NULL;

  DBG_PROG_START

  screenID = oyMonitor_nameToOsxID( display_name );

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
  oyMonitor_s * disp = 0;

  DBG_PROG_START

  if(display_name)
    DBG_PROG1_S("display_name %s",display_name);

  disp = oyMonitor_newFrom_( display_name );
  if(!disp)
    return 0;

  display = oyMonitor_device_( disp );
  oyPostAllocHelper_m_( display, 1,
    WARNc_S(_("open X Display failed")); *size = 0; DBG_PROG_ENDE return 0; )

  screen = oyMonitor_deviceScreen_( disp ); DBG_PROG_V((screen))
  w = RootWindow(display, screen); DBG_PROG1_S("w: %ld", w)
  DBG_PROG 

  {
    char *atom_name = 0;

    atom_name = oyMonitor_getAtomName_( disp, "_ICC_PROFILE" );

    if( atom_name )
    {
      atom = XInternAtom (display, atom_name, True);
      if (atom == None) {
        WARNc2_S("%s %s", _("Could not get Xatom, probably your monitor profile is not set:"), atom_name);
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
  if(*size)
  {
    moni_profile = (char*)oyAllocateWrapFunc_( *size, allocate_func );
    memcpy( moni_profile, prop_return, *size );
  }

  if(prop_return)
    XFree( prop_return );
  oyMonitor_release_( &disp );
#endif /* TODO WIN */

  DBG_PROG_ENDE
  return moni_profile;
}

char*
oyGetMonitorProfileName_          (const char* display_name,
                                   oyAlloc_f     allocate_func)
{
  char       *moni_profile=0;

#ifdef __APPLE__

  CMProfileRef prof=NULL;
  DisplayIDType screenID=0;
  CMProfileLocation loc;

  DBG_PROG_START

  screenID = oyMonitor_nameToOsxID( display_name );

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
  oyMonitor_s * disp = 0;

  DBG_PROG_START

  oyGetMonitorInfo_( display_name,
                     &manufacturer, &model, &serial, &display_geometry,
                     oyAllocateFunc_);

  disp = oyMonitor_newFrom_( display_name );
  if(!disp)
    return 0;

  host_name = oyMonitor_hostName_ ( disp );


  /* search the profile in the local database */
  /* It's not network transparent. */
  /* If working remotely, better fetch the whole profile instead. */
  moni_profile = oyGetDeviceProfile( oyDISPLAY, manufacturer, model, serial,
                                     host_name, display_geometry,
                                     0,0,0, allocate_func);

  if(manufacturer) oyDeAllocateFunc_(manufacturer);
  if(model) oyDeAllocateFunc_(model);
  if(serial) oyDeAllocateFunc_(serial);
  if(display_geometry) oyDeAllocateFunc_ (display_geometry);
  oyMonitor_release_( &disp );

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
    oySnprintf1_( list[i], 24, "%d", ids[i] );
  }

#else /* HAVE_X */
  Display *display = 0;
  int len = 0;
  oyMonitor_s * disp = 0;

  *n_scr = 0;

  disp = oyMonitor_newFrom_( display_name );
  if(!disp)
    return 0;

  display = oyMonitor_device_( disp );

  if( !display || (len = ScreenCount( display )) == 0 )
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
  oyMonitor_release_( &disp );

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
  oyMonitor_s * disp = 0;

  disp = oyMonitor_newFrom_( display_name );
  if(!disp)
    return 1;

  display = oyMonitor_device_( disp );

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
      oyMonitor_s * disp = 0;
      char *screen_name = 0;

      screen_name = oyChangeScreenName_( display_name, i );
      oyPostAllocHelper_m_( screen_name, 1, oyMonitor_release_( &disp ); return 0 )

      disp = oyMonitor_newFrom_( screen_name );
      if(!disp)
        return 0;

      DBG_PROG6_S( "i %d x %d y %d dispxy %d,%d %s", i, x,y, oyMonitor_x_(disp), oyMonitor_y_(disp) ,screen_name )

      if( (x >= oyMonitor_x_(disp) &&
           x < oyMonitor_x_(disp) + oyMonitor_width_(disp) &&
           y >= oyMonitor_y_(disp) &&
           y < oyMonitor_y_(disp) + oyMonitor_height_(disp)) )
      {
        screen = i;
      }
      oyMonitor_release_( &disp );
      oyFree_m_( screens[i] )
      oyFree_m_( screen_name )
    }

  clean_up:
  oyMonitor_release_( &disp );
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
                                   oyAlloc_f     allocate_func)
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
      oySnprintf1_( new_display_name, 24, "%d", (int)screenID );

      return new_display_name;
    }
  }

#else
  int len = 0, i;
  char** screens = 0;
  int screen = -1;
  Display *display = 0;
  oyMonitor_s * disp = 0;

  disp = oyMonitor_newFrom_( display_name );
  if(!disp)
    return 0;

  if(!display_name)
    display_name = oyMonitor_name_( disp );

  display = oyMonitor_device_( disp );

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
      oyMonitor_s * disp = 0;
      char *screen_name = 0;

      screen_name = oyChangeScreenName_( display_name, i );
      oyPostAllocHelper_m_( screen_name, 1, return 0 )

      disp = oyMonitor_newFrom_( screen_name );
      if(!disp)
         return 0;

      DBG_PROG6_S( "i %d x %d y %d dispxy %d,%d %s", i, x,y, oyMonitor_x_(disp), oyMonitor_y_(disp) ,screen_name )

      if( (x >= oyMonitor_x_(disp) &&
           x < oyMonitor_x_(disp) + oyMonitor_width_(disp) &&
           y >= oyMonitor_y_(disp) &&
           y < oyMonitor_y_(disp) + oyMonitor_height_(disp)) )
      {
        screen = i;
      }
      oyMonitor_release_( &disp );
      oyFree_m_( screen_name )
    }

  if (screen >= 0)
  {
    int len = sizeof(char) * strlen(screens[screen]) + 1;
    new_display_name = oyAllocateWrapFunc_( len, allocate_func );
    oySnprintf_( new_display_name, len, screens[screen] ) ;
  }

  clean_up:
  oyMonitor_release_( &disp );
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
oyMonitor_getGeometryIdentifier_         (oyMonitor_s  *disp)
{
  int len = 64;

  oyAllocHelper_m_( disp->identifier, char, len, 0, return 1 )

  oySnprintf5_( disp->identifier, len, "%d_%d+%d+%dx%d", 
            oyMonitor_screen_(disp), oyMonitor_x_(disp), oyMonitor_y_(disp),
            oyMonitor_width_(disp), oyMonitor_height_(disp) );

  return 0;
}

char* oyMonitor_getAtomName_         ( oyMonitor_s       * disp,
                                       const char        * base )
{
  int len = 64;
  char *atom_name = 0;
  char *screen_number = oyMonitor_screenIdentifier_( disp );

  oyPostAllocHelper_m_( screen_number, 1, return 0 )
  oyAllocHelper_m_( atom_name, char, len, 0, return 0 )

  oySnprintf2_( atom_name, len, "%s%s", base, screen_number );

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
    DBG_PROG1_S("display_name %s",display_name);


  screen_names = oyGetAllScreenNames_( display_name, &n_scr );

  for( i = 0; i < n_scr; ++i )
  {
    error = oyActivateMonitorProfile_( screen_names[i], 0 );
    oyDeAllocateFunc_( screen_names[i] ); screen_names[i] = 0;
  }

  if (screen_names) oyDeAllocateFunc_ (screen_names);

  DBG_PROG_ENDE
  return error;
}

int
oyActivateMonitorProfile_         (const char* display_name,
                                   const char* profil_name )
{
  int error = 0;
  const char * profile_fullname = 0;
  const char * profil_basename = 0;
  char* profile_name_ = 0;
  oyProfile_s * prof = 0;
  size_t size = 0;
#if defined( HAVE_X ) && !defined(__APPLE__)
  oyMonitor_s * disp = 0;
  char       *dpy_name = NULL;
  char *text = 0;
#endif

  char * moni_profile = oyGetMonitorProfile_( display_name,
                                              &size, oyAllocateFunc_ );

  if(moni_profile && size)
    oyDeAllocateFunc_(moni_profile);

  if(size)
    return 0;

  DBG_PROG_START
#if defined( HAVE_X ) && !defined(__APPLE__)
  disp = oyMonitor_newFrom_( display_name );
  if(!disp)
    return 1;

  dpy_name = calloc( sizeof(char), MAX_PATH );
  if( display_name && !strstr( disp->host, display_name ) )
    oySnprintf1_( dpy_name, MAX_PATH, ":%d", disp->geo[0] );
  else
    oySnprintf2_( dpy_name, MAX_PATH, "%s:%d", disp->host, disp->geo[0] );
#endif

  if(profil_name)
  {
    DBG_PROG1_S( "profil_name = %s", profil_name );
    prof = oyProfile_FromFile( profile_name_, 0, 0 );
    profile_fullname = oyProfile_GetFileName( prof, -1 );
  } else
  {
    profile_name_ = oyGetMonitorProfileName_( display_name, oyAllocateFunc_);
    prof = oyProfile_FromFile( profile_name_, 0, 0 );
    profile_fullname = oyProfile_GetFileName( prof, -1 );
    profil_name = profile_name_;
  }

  if( profile_fullname && strlen(profile_fullname) )
  {

    if(profil_name && strrchr(profil_name,OY_SLASH_C))
      profil_basename = strrchr(profil_name,OY_SLASH_C)+1;
    else
    {
      if(profil_name)
        profil_basename = profil_name;
      else if( profile_fullname && strrchr(profile_fullname,OY_SLASH_C))
        profil_basename = strrchr( profile_fullname, OY_SLASH_C)+1;
    }

#ifdef __APPLE__
    {
      CMProfileLocation loc;
      CMError err = 0;
      CMProfileRef prof=NULL;
      DisplayIDType screenID = 0;

      loc.locType = cmPathBasedProfile;
      oySnprintf1_( loc.u.pathLoc.path, 255, "%s", profile_fullname);

      err = CMOpenProfile ( &prof, &loc );
      screenID = oyMonitor_nameToOsxID( display_name );

      if( screenID && !err )
        err = CMSetProfileByAVID ( screenID, prof );

      CMCloseProfile( prof );
    }
#else /* HAVE_X */

    oyAllocHelper_m_( text, char, MAX_PATH, 0, error = 1; goto Clean )
    DBG_PROG1_S( "profile_fullname %s", profile_fullname );

    /** set vcgt tag with xcalib
       not useable with multihead Xinerama at one screen

       @todo TODO xcalib should be configurable as a module
     */
    sprintf(text,"xcalib -d %s -s %d \'%s\'", dpy_name, disp->geo[1],
                               profile_fullname);
    {
      Display * display = oyMonitor_device_( disp );
      int effective_screen = oyMonitor_screen_( disp );
      int screen = oyMonitor_deviceScreen_( disp );
      XF86VidModeGamma gamma;
      int size = 0,
          can_gamma = 0;

      if(!display)
      {
        WARNc3_S("%s %s %s", _("open X Display failed"), dpy_name, display_name)
        return 1;
      }

#ifdef HAVE_XF86VMODE
      if(effective_screen == screen)
      {
        /* check for gamma capabiliteis on the given screen */
        if (XF86VidModeGetGamma(display, effective_screen, &gamma))
          can_gamma = 1;
        else
        if (XF86VidModeGetGammaRampSize(display, effective_screen, &size))
        {
          if(size)
            can_gamma = 1;
        }
      }
#endif

      /* Check for incapabilities of X gamma table access */
      if(can_gamma || oyMonitor_screen_( disp ) == 0)
        error = system(text);
      if(error &&
         error != 65280) { /* hack */
        WARNc2_S("%s %s", _("No monitor gamma curves by profile:"),
                oyNoEmptyName_m_(profil_basename) )
      }
    }

    DBG_PROG1_S( "system: %s", text )

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
        DBG_PROG1_S("display_name %s",display_name);

      display = oyMonitor_device_( disp );

      screen = oyMonitor_deviceScreen_( disp );
      DBG_PROG_V((screen))
      w = RootWindow(display, screen); DBG_PROG1_S("w: %ld", w)

      if(profile_fullname)
        moni_profile = oyGetProfileBlock( profile_fullname, &size, oyAllocateFunc_ );
      else if(profil_name)
        moni_profile = oyGetProfileBlock( profil_name, &size, oyAllocateFunc_ );

      if(!size || !moni_profile)
        WARNc_S(_("Error obtaining profile"));

      atom_name = oyMonitor_getAtomName_( disp, "_ICC_PROFILE" );
      if( atom_name )
      {
        atom = XInternAtom (display, atom_name, False);
        if (atom == None) {
          WARNc2_S("%s \"%s\"", _("Error setting up atom"), atom_name);
        }
      } else WARNc_S(_("Error setting up atom"));

      if( atom && moni_profile)
      result = XChangeProperty( display, w, atom, XA_CARDINAL,
                       8, PropModeReplace, moni_profile, (int)size );

      if(moni_profile)
        oyFree_m_( moni_profile )
      oyFree_m_( atom_name )
    }

    oyFree_m_( text );
#endif
  }

#if defined( HAVE_X ) && !defined(__APPLE__)
  Clean:
  oyMonitor_release_( &disp );
#endif
  oyProfile_Release( &prof );
  if(profile_name_) oyFree_m_( profile_name_ );
#if defined( HAVE_X ) && !defined(__APPLE__)
  if(dpy_name) oyFree_m_( dpy_name );
#endif

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
  const char *profile_fullname = 0;
  oyMonitor_s * disp = 0;
  oyProfile_s * prof = 0;

  DBG_PROG_START

  error  =
    oyGetMonitorInfo_ (display_name,
                       &manufacturer, &model, &serial, &display_geometry,
                       oyAllocateFunc_);

  disp = oyMonitor_newFrom_( display_name );
  if(!disp)
  {
    DBG_PROG_ENDE
    return 1;
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
        DBG_PROG1_S("display_name %s",display_name);

      display = oyMonitor_device_( disp );

      screen = oyMonitor_deviceScreen_( disp );
      DBG_PROG_V((screen))
      w = RootWindow(display, screen); DBG_PROG1_S("w: %ld", w)

      DBG_PROG

      atom_name = oyMonitor_getAtomName_( disp, "_ICC_PROFILE" );
      atom = XInternAtom (display, atom_name, True);
      if (atom == None) {
        WARNc2_S("%s \"%s\"", _("Error getting atom"), atom_name);
      }

      XDeleteProperty( display, w, atom );

      {
        char *dpy_name = strdup( oyNoEmptyString_m_(display_name) );
        char  command[1024];
        char *ptr = NULL;
        int r;

        if( (ptr = strchr(dpy_name,':')) != 0 )
          if( (ptr = strchr(ptr,'.')) != 0 )
            ptr[0] = '\000';

        oySnprintf2_(command, 1024, "xgamma -gamma 1.0 -screen %d -display %s",
                 disp->geo[1], dpy_name);
        r = system( command );
      }

      oyFree_m_( atom_name )
      DBG_PROG
    goto finish;
  }

  DBG_PROG1_S( "profil_name = %s", profil_name )

  error =  oySetDeviceProfile(oyDISPLAY, manufacturer, model, serial,
                              oyMonitor_hostName_(disp), display_geometry,
                              0,0,0, profil_name, 0,0);

  prof = oyProfile_FromFile( profil_name, 0, 0 );
  profile_fullname = oyProfile_GetFileName( prof, -1 );
  DBG_PROG1_S( "profile_fullname %s", profile_fullname )

  if( profile_fullname )
    error = oyActivateMonitorProfile_(display_name, profil_name);

  finish:
  DBG_PROG
  if (manufacturer) oyDeAllocateFunc_ (manufacturer);
  if (model) oyDeAllocateFunc_ (model);
  if (serial) oyDeAllocateFunc_ (serial);
  if (display_geometry) oyDeAllocateFunc_ (display_geometry);
  oyProfile_Release( &prof );
  oyMonitor_release_( &disp );
#endif

  DBG_PROG_ENDE
  return error;
}

#if defined( HAVE_X ) && !defined(__APPLE)
int
oyGetDisplayNumber_        (oyMonitor_s *disp)
{
  int dpy_nummer = 0;
  const char *display_name = oyMonitor_name_(disp);

  DBG_PROG_START

  if( display_name )
  {
    char ds[8];             /* display.screen*/
    const char *txt = strchr( display_name, ':' );
    
    if( !txt )
    { WARNc_S( "invalid display name" )
      return -1;
    }

    ++txt;
    oySnprintf_( ds, 8, txt );
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

int   oyMonitor_getScreenFromDisplayName_( oyMonitor_s   * disp )
{
  int scr_nummer = 0;
  const char *display_name = oyMonitor_name_(disp);

  DBG_PROG_START

  if( display_name )
  {
    char ds[8];             /* display.screen*/
    const char *txt = strchr( display_name, ':' );
    
    if( !txt )
    { WARNc_S( "invalid display name" )
      return -1;
    }

    oySnprintf_( ds, 8, txt );
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

  DBG_PROG1_S( "host_name = %s", host_name )

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
    { WARNc_S( "invalid display name" )
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

  DBG_PROG1_S( "host_name = %s", host_name )

  DBG_PROG_ENDE
  return host_name;
}

/** @internal get the geometry of a screen 
 *
 *  @param          disp      display info structure
 *  @return         error
 */
int
oyMonitor_getScreenGeometry_            (oyMonitor_s *disp)
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
                            WARNc_S(_("open X Display failed")); return 1 )

    disp->screen = 0;
  }

  disp->geo[0] = oyGetDisplayNumber_( disp );
  disp->geo[1] = screen = oyMonitor_getScreenFromDisplayName_( disp );

# if HAVE_XIN
  if( XineramaIsActive( disp->display ) )
  {
    int n_scr_info = 0;

    XineramaScreenInfo *scr_info = XineramaQueryScreens( disp->display,
                                                         &n_scr_info );
    oyPostAllocHelper_m_(scr_info, n_scr_info, return 1 )

    if( !scr_info )
    {
      WARNc_S(_("Xinerama request failed"))
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
    oyPostAllocHelper_m_(scr, 1, WARNc_S(_("open X Screen failed")); return 1;)
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

/** @internal
 *  @brief create a monitor information stuct for a given display name
 *
 *  @since Oyranos: version 0.x.x
 *  @date  17 december 2007 (API 0.1.8)
 */
oyMonitor_s* oyMonitor_newFrom_      ( const char        * display_name )
{
  int error = 0;
  int i = 0;
  oyMonitor_s * disp = 0;

  DBG_PROG_START

  disp = oyAllocateFunc_( sizeof(oyMonitor_s) );
  error = !disp;
  if(!error)
    error = !memset( disp, 0, sizeof(oyMonitor_s) );

  disp->type_ = oyOBJECT_MONITOR_S;

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
      oyMonitor_getScreenGeometry_( disp ) )
    error = 1;

  if( !error &&
      oyMonitor_getGeometryIdentifier_( disp ) ) 
    error = 1;


  DBG_PROG_ENDE
  return disp;
}

/** @internal
 *  @brief release a monitor information stuct
 *
 *  @since Oyranos: version 0.1.8
 *  @date  17 december 2007 (API 0.1.8)
 */
int          oyMonitor_release_      ( oyMonitor_s      ** obj )
{
  int error = 0;
  /* ---- start of common object destructor ----- */
  oyMonitor_s * s = 0;
  
  if(!obj || !*obj)
    return 0;
  
  s = *obj;
  
  if( s->type_ != oyOBJECT_MONITOR_S)
  { 
    WARNc_S("Attempt to release a non oyMonitor_s object.")
    return 1;
  }
  /* ---- end of common object destructor ------- */

  oyDeAllocateFunc_( s->name );
  oyDeAllocateFunc_( s->host );
  oyDeAllocateFunc_( s->identifier );

  s->geo[0] = s->geo[1] = -1;

  if( s->display )
  {
    XCloseDisplay( s->display );
    s->display=0;
  }

  oyDeAllocateFunc_( s );
  s = 0;

  *obj = 0;

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
oyGetMonitorInfo_lib              (const char* display,
                                   char**      manufacturer,
                                   char**      model,
                                   char**      serial,
                                   oyAlloc_f     allocate_func)
{
  int err = 0;

  DBG_PROG_START

#if (defined(HAVE_X) && !defined(__APPLE__))
  err = oyGetMonitorInfo_( display, manufacturer, model, serial, 0,
                           allocate_func);
#else
  err = 1;
#endif

  if(*manufacturer)
    DBG_PROG_S( *manufacturer );
  if(*model)
    DBG_PROG_S( *model );
  if(*serial)
    DBG_PROG_S( *serial );

  DBG_PROG_ENDE
  return err;
}

/** @brief get the monitor profile from the server

 *  @param      display       the display string
 *  @param[out] size          the size of profile
 *  @param      allocate_func function used to allocate memory for the profile
 *  @return                   the memory block containing the profile
 */
char*
oyGetMonitorProfile_lib       (const char* display,
                               size_t *size,
                               oyAlloc_f     allocate_func)
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

/** @brief get the monitor profile filename from the device profile database

 *  @param      display       the display string
 *  @param      allocate_func function used to allocate memory for the string
 *  @return                   the profiles filename (if localy available)
 */
char*  oyGetMonitorProfileNameFromDB_lib ( const char        * display,
                                       oyAlloc_f           allocate_func )
{
  char* moni_profile = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  moni_profile = oyGetMonitorProfileName_( display, allocate_func );

  oyExportEnd_();
  DBG_PROG_ENDE
  return moni_profile;
}

/** @brief set the monitor profile by filename

 *  @param      display_name  the display string
 *  @param      profil_name   the file to use as monitor profile or 0 to unset
 *  @return                   error
 */
int
oySetMonitorProfile_lib           (const char* display_name,
                                   const char* profil_name )
{
  int error = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING | EXPORT_MONITOR);

  error = oySetMonitorProfile_( display_name, profil_name );

  oyExportEnd_();
  DBG_PROG_ENDE
  return error;
}

/** @func    oyActivateMonitorProfiles
 *  @brief   activate the monitor using the stored configuration
 *
 *  Activate in case the appropriate profile is not yet setup in the server.
 *
 *  @see oySetMonitorProfile for permanently configuring a monitor
 *
 *  @param   display_name              the display string
 *  @return                            error
 *
 *  @version Oyranos: 0.1.8
 *  @since   2005/00/00 (Oyranos: 0.1.8)
 *  @date    2008/10/16
 */
int
oyActivateMonitorProfiles_lib         (const char* display_name)
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
oyGetScreenFromPosition_lib     (const char *display_name,
                                 int x,
                                 int y)
{
  int screen = 0;

  DBG_PROG_START

#if (defined(HAVE_X) && !defined(__APPLE__))
  screen = oyGetScreenFromPosition_( display_name, x,y );
#endif

  DBG_PROG3_S( "x %d y %d screen %d", x,y,screen );

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
oyGetDisplayNameFromPosition_lib      (const char *display_name,
                                   int x,
                                   int y,
                                   oyAlloc_f     allocate_func)
{
  char *new_display_name = 0;

  DBG_PROG_START

  new_display_name = oyGetDisplayNameFromPosition_( display_name, x,y,
                                                    allocate_func );
  DBG_PROG3_S( "x %d y %d new_display_name %s", x,y,new_display_name );

  DBG_PROG_ENDE
  return new_display_name;
}

