/*
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2005  Kai-Uwe Behrmann
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

/* ---  Helpers  --- */

/* --- internal API definition --- */


int   oyGetMonitorInfo_           (const char* display,
                                   char**      manufacturer,
                                   char**      model,
                                   char**      serial,
                                   char**      display_geometry,
                                   oyAllocFunc_t allocate_func);
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
int   oyGetScreenFromDisplayName_ (oy_display_s  *disp );
int   oyGetScreenFromPosition_    (const char *display_name,
                                   int         x,
                                   int         y);
int   oyGetScreenGeometry_        (oy_display_s  *disp );
char* oyChangeScreenName_         (const char *display_name, int screen );
char**oyGetAllScreenNames_        (const char *display_name, int *n_scr );
int   oyGetGeometryIdentifier_    (oy_display_s  *disp );
char* oyGetAtomScreen_            (oy_display_s  *disp, const char *base );
char* oyChangeXProperty           (oy_display_s  *disp,
                                   const char *atom_name,
                                   int         delete_property,
                                   char       *block,
                                   size_t     *size );

/** Display functions */
const char* oyDisplayName_( oy_display_s *disp ) { return disp->name; }
const char* oyDisplayHostName_( oy_display_s *disp ) { return disp->host; }
const char* oyDisplayIdentifier_( oy_display_s *disp ) { return disp->identifier; }
/** the screen appendment for the root window properties */
char*       oyDisplayScreenIdentifier_( oy_display_s *disp )
{ char *number = 0;

  oyAllocHelper_m_( number, char, 24, 0, return "");
  number[0] = 0;
  if( disp->geo[1] >= 1 ) sprintf( number,"_%d", disp->geo[1] );
  return number;
}
Display* oyDisplayDevice_( oy_display_s *disp ) { return disp->display; }
int oyDisplayDeviceScreen_( oy_display_s *disp ) { return disp->screen; }
int oyDisplayNumber_( oy_display_s *disp ) { return disp->geo[0]; }
int oyDisplayScreen_( oy_display_s *disp ) { return disp->geo[1]; }
int oyDisplayX_( oy_display_s *disp ) { return disp->geo[2]; }
int oyDisplayY_( oy_display_s *disp ) { return disp->geo[3]; }
int oyDisplayWidth_( oy_display_s *disp ) { return disp->geo[4]; }
int oyDisplayHeight_( oy_display_s *disp ) { return disp->geo[5]; }

int
oyFree_( void *oy_structure )
{ int error = 0;
  oyOBJECT_TYPE *type = (oyOBJECT_TYPE*) oy_structure;

  if( type )
    switch( *type )
    {
    case oyDISPLAY_T:
      {
        oy_display_s *disp = (oy_display_s*)oy_structure;

        oyFree_m_( disp->name )
        oyFree_m_( disp->host )
        oyFree_m_( disp->identifier )
        disp->geo[0] = disp->geo[1] = -1;
        if( disp->display ) XCloseDisplay( disp->display );
      }
      break;
    default: WARN_S (("unknown type")); error = 1;
    }
  else
    error = 1;

  return error;
}

/** this function isnt yet ready to export */
void
oyUnrollEdid1_                    (struct DDC_EDID1 *edi,
                                   char**      manufacturer,
                                   char**      model,
                                   char**      serial,
                                   oyAllocFunc_t allocate_func)
{
  DBG_PROG_START
  char *t = 0;
  int len;

  *manufacturer = edi->Mnf_Model;
  if((t = strchr(*manufacturer,'\n')) != 0)
    *t = 32;
  /*model = 0;*/
  *serial = edi->HW_ID;
  if((t = strchr(*serial,'\n')) != 0)
    *t = 32;
  if(*manufacturer)
    DBG_PROG_S(( *manufacturer ));
  if(*model)
    DBG_PROG_S(( *model ));
  if(*serial)
    DBG_PROG_S(( *serial ));
  /* allocate new memory to release the supplied ID block */
  len = strlen(edi->Mnf_Model); DBG_PROG_V((len))
  if(len) { DBG_PROG
    ++len;
    oyAllocHelper_m_( t, char, len, allocate_func, return )
    sprintf(t, edi->Mnf_Model);
    *manufacturer = t; DBG_PROG_S(( *manufacturer ))
  }
  if(edi->major_version == 1 && edi->minor_version >= 3)
  {
    len = strlen(edi->HW_ID); DBG_PROG_V((len))
    if(len) { DBG_PROG
      ++len;
      t = (char*)allocate_func( len );
      sprintf(t, edi->HW_ID);
      *serial = t; DBG_PROG_S(( *serial ))
    }
  }
  else
  {
    len = strlen(edi->Serial); DBG_PROG_V((len))
    if(len) { DBG_PROG
      ++len;
      t = (char*)allocate_func( len );
      sprintf(t, edi->Serial);
      *serial = t; DBG_PROG_S(( *serial ))
    }
  }
  DBG_PROG

  DBG_PROG_ENDE
}



int
oyGetMonitorInfo_                 (const char* display_name,
                                   char**      manufacturer,
                                   char**      model,
                                   char**      serial,
                                   char**      display_geometry,
                                   oyAllocFunc_t allocate_func)
{
  DBG_PROG_START

  Display *display = 0;
  int screen = 0;
  Window w;
  Atom atom, a;
  char *atom_name;
  int actual_format_return, len;
  unsigned long nitems_return=0, bytes_after_return=0;
  unsigned char* prop_return=0;
  struct DDC_EDID1 *edi=0;
  char *t;
  oy_display_s disp;

  if(display_name)
    DBG_PROG_S(("display_name %s",display_name));

  if( oyGetDisplay_( display_name , &disp ) )
    return 1;

  display = oyDisplayDevice_( &disp );
  screen = oyDisplayDeviceScreen_( &disp );
  DBG_PROG_V((screen))
  w = RootWindow( display, screen); DBG_PROG_S(("w: %ld", w))
  DBG_PROG 

  atom_name = oyGetAtomScreen_( &disp, "XFree86_DDC_EDID1_RAWDATA" );
  atom = XInternAtom(display, atom_name, 1);

  DBG_PROG_S(("atom: %ld", atom))

  DBG_PROG

  if(atom)
    XGetWindowProperty(display, w, atom, 0, 32, 0, AnyPropertyType, &a,
                     &actual_format_return, &nitems_return, &bytes_after_return,
                     &prop_return );


  if( nitems_return != 128 )
  {
    /* test twinview edid */
    int error = system("oyranos-monitor-nvidia -p");

    int oy_debug_ = oy_debug; oy_debug = 1;

    if(!error)
    {
      atom = XInternAtom(display, atom_name, 1);
      DBG_PROG_S(("atom: %ld", atom))

      if(atom)
        XGetWindowProperty(display, w, atom, 0, 32, 0, AnyPropertyType, &a,
                     &actual_format_return, &nitems_return, &bytes_after_return,
                     &prop_return );
    }

    oy_debug = oy_debug_;
  }

  if( nitems_return != 128 ) {
    WARN_S((_("unexpected EDID lenght")))
    DBG_PROG_ENDE
    return 1;
  }

  /* convert to an deployable struct */
  edi = (struct DDC_EDID1*) prop_return;

  oyUnrollEdid1_( edi, manufacturer, model, serial, allocate_func );
  *manufacturer = edi->Mnf_Model;
  if((t = strchr(*manufacturer,'\n')) != 0)
    *t = 32;
  /*model = 0;*/
  *serial = edi->HW_ID;
  if((t = strchr(*serial,'\n')) != 0)
    *t = 32;
  if(*manufacturer)
    DBG_PROG_S(( *manufacturer ));
  if(*model)
    DBG_PROG_S(( *model ));
  if(*serial)
    DBG_PROG_S(( *serial ));
  /* allocate new memory to release the supplied ID block */
  len = strlen(edi->Mnf_Model); DBG_PROG_V((len))
  if(len) { DBG_PROG
    ++len;
    t = (char*)allocate_func( len );
    sprintf(t, edi->Mnf_Model);
    *manufacturer = t; DBG_PROG_S(( *manufacturer ))
  }
  len = strlen(edi->HW_ID); DBG_PROG_V((len))
  if(len) { DBG_PROG
    ++len;
    t = (char*)allocate_func( len );
    sprintf(t, edi->HW_ID);
    *serial = t; DBG_PROG_S(( *serial ))
  } DBG_PROG

  if( display_geometry )
  {
    const char *identifier = oyDisplayIdentifier_( &disp );

    len = strlen( identifier );
    ++len;
    t = (char*)allocate_func( len );
    sprintf(t, identifier);

    *display_geometry = t; DBG_PROG_S(( *display_geometry ))
  }

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

char*
oyGetMonitorProfile_          (const char* display_name,
                               size_t *size,
                               oyAllocFunc_t allocate_func)
{ DBG_PROG_START

  Display *display;
  int screen = 0;
  Window w;
  Atom atom = 0, a;
  int actual_format_return;
  unsigned long nitems_return=0, bytes_after_return=0;
  unsigned char* prop_return=0;
  oy_display_s disp;
  char       *moni_profile=0;

  if(display_name)
    DBG_PROG_S(("display_name %s",display_name));

  if( oyGetDisplay_( display_name, &disp ) )
    return NULL;

  display = oyDisplayDevice_( &disp );
  oyPostAllocHelper_m_( display, 1,
    WARN_S((_("open X Display failed"))); *size = 0; DBG_PROG_ENDE return 0; )

  screen = oyDisplayDeviceScreen_( &disp ); DBG_PROG_V((screen))
  w = RootWindow(display, screen); DBG_PROG_S(("w: %ld", w))
  DBG_PROG 

  {
    char *atom_name = 0;

    atom_name = oyGetAtomScreen_( &disp, "_ICC_PROFILE" );

    if( atom_name )
    {
      atom = XInternAtom (display, atom_name, True);
      if (atom == None) {
        WARN_S((_("Error setting up atom \"%s\""), atom_name));
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
  moni_profile = (char*)allocate_func( *size );
  memcpy( moni_profile, prop_return, *size );
  XFree( prop_return );
  oyFree_( &disp );

  DBG_PROG_ENDE
  return moni_profile;
}

char*
oyGetMonitorProfileName_          (const char* display_name,
                                   oyAllocFunc_t allocate_func)
{ DBG_PROG_START

  char       *manufacturer=0,
             *model=0,
             *serial=0,
             *display_geometry=0;
  char       *moni_profile=0;
  const char *host_name = 0;
  oy_display_s disp;

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

  DBG_PROG_ENDE
  return moni_profile;
}


char**
oyGetAllScreenNames_            (const char *display_name,
                                 int *n_scr)
{
  int i;
  Display *display = 0;
  int len = 0;
  char** list = 0;
  oy_display_s disp;

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

  return list;
}


/** This function will only with Xineram hit exact results
 */
int
oyGetScreenFromPosition_        (const char *display_name,
                                 int x,
                                 int y)
{
  int len = 0, i;
  char** screens = 0;
  int screen = 0;

  screens = oyGetAllScreenNames_( display_name, &len );
  oyPostAllocHelper_m_( screens, len, return 0 );

  for (i = 0; i < len; ++i)
    {
      oy_display_s disp;
      char *screen_name = 0;

      screen_name = oyChangeScreenName_( display_name, i );
      oyPostAllocHelper_m_( screen_name, 1, oyFree_( &disp ); return 0 )

      if( oyGetDisplay_( screen_name, &disp ) ) { oyFree_( &disp ); return 0; }

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

  oyFree_m_( screens )
  return screen;
}


/**
    takes a chaked display name and point as argument and
    gives a string back for search in the db
 */
int
oyGetGeometryIdentifier_         (oy_display_s  *disp)
{
  int len = 64;

  oyAllocHelper_m_( disp->identifier, char, len, 0, return 1 )

  snprintf( disp->identifier, len, "%d_%d+%d+%dx%d", 
            oyDisplayScreen_(disp), oyDisplayX_(disp), oyDisplayY_(disp),
            oyDisplayWidth_(disp), oyDisplayHeight_(disp) );

  return 0;
}

char*
oyGetAtomScreen_                  (oy_display_s *disp,
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


/**
    1. get all monitors / screens / Xinerama screens
    2. get the profile name for
    3. set the profile data to a Xatom
 */
int
oyActivateMonitorProfiles_        (const char* display_name)
{ DBG_PROG_START
  int error = 0;
  int n_scr = 0;
  int i;
  char **screen_names = 0;

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
{ DBG_PROG_START
  int error = 0;

  char       *profil_pathname;
  const char *profil_basename;
  char* profile_name_ = 0;
  oy_display_s disp;
  const char *host_name;

  if( oyGetDisplay_( display_name, &disp ) ) return 1;
  host_name = oyDisplayName_( &disp );

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
    char *text = 0;

    oyAllocHelper_m_( text, char, MAX_PATH, 0, error = 1; goto Clean )
    DBG_PROG_S(( "profil_pathname %s", profil_pathname ));

    if(profil_name && strrchr(profil_name,OY_SLASH_C))
      profil_basename = strrchr(profil_name,OY_SLASH_C)+1;
    else
      profil_basename = profil_name;

    /* set vcgt tag with xcalib
       not useable with Xinerama
     */
    sprintf(text,"xcalib -d %s %s%s%s", display_name,
                               profil_pathname, OY_SLASH, profil_basename);
    error = system(text);
    if(error) {
      WARN_S((_("Error while setting monitor gamma curves")))
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

      /* TODO: multi screen */
      screen = oyDisplayDeviceScreen_( &disp );
      DBG_PROG_V((screen))
      w = RootWindow(display, screen); DBG_PROG_S(("w: %ld", w))

      moni_profile = oyGetProfileBlock( profil_name, &size, oyAllocateFunc_ );
      if(!size || !moni_profile)
        WARN_S((_("Error obtaining profile")));

      atom_name = oyGetAtomScreen_( &disp, "_ICC_PROFILE" );
      if( atom_name )
      {
        atom = XInternAtom (display, atom_name, False);
        if (atom == None) {
          WARN_S((_("Error setting up atom \"%s\""), atom_name));
        }
      } else WARN_S((_("Error setting up atom")));

      if( atom )
      result = XChangeProperty( display, w, atom, XA_CARDINAL,
                       8, PropModeReplace, moni_profile, (int)size );

      oyFree_m_( moni_profile )
      oyFree_m_( atom_name )
    }

    DBG_PROG_S(( "system: %s", text ))
    oyFree_m_( text );
  }

  Clean:
  oyFree_m_( profil_pathname );
  oyFree_m_( profile_name_ );
  oyFree_( &disp );

  DBG_PROG_ENDE
  return error;
}

int
oySetMonitorProfile_              (const char* display_name,
                                   const char* profil_name )
{ DBG_PROG_START
  int error = 0;

  char       *manufacturer=0,
             *model=0,
             *serial=0,
             *display_geometry=0;
  char       *profil_pathname = 0;
  oy_display_s disp;

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

      /* TODO: multi screen */
      screen = oyDisplayDeviceScreen_( &disp );
      DBG_PROG_V((screen))
      w = RootWindow(display, screen); DBG_PROG_S(("w: %ld", w))

      DBG_PROG

      atom_name = oyGetAtomScreen_( &disp, "_ICC_PROFILE" );
      atom = XInternAtom (display, atom_name, False);
      if (atom == None) {
        WARN_S((_("Error setting up atom \"%s\""), atom_name));
      }

      XDeleteProperty( display, w, atom );

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

  DBG_PROG_ENDE
  return error;
}

int
oyGetDisplayNumber_        (oy_display_s *disp)
{ DBG_PROG_START
  int scr_nummer = 0;
  const char *display_name = oyDisplayName_(disp);

  if( display_name )
  {
    char ds[8];             // display.screen
    const char *txt = strchr( display_name, ':' );
    
    if( !txt )
    { WARN_S(( "invalid display name" ))
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
    scr_nummer = atoi( ds );
  }

  DBG_PROG_ENDE
  return scr_nummer;
}

int
oyGetScreenFromDisplayName_        (oy_display_s *disp)
{ DBG_PROG_START
  int scr_nummer = 0;
  const char *display_name = oyDisplayName_(disp);

  if( display_name )
  {
    char ds[8];             // display.screen
    const char *txt = strchr( display_name, ':' );
    
    if( !txt )
    { WARN_S(( "invalid display name" ))
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


/** 
 *  extract the host name or get from environment
 */
char*
oyExtractHostName_           (const char* display_name)
{ DBG_PROG_START
  char* host_name = 0;

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

/** Do a full check and change the screen name,
 *  if the screen arg is appropriate. Dont care about the host part
 */
char*
oyChangeScreenName_                (const char* display_name,
                                    int         screen)
{ DBG_PROG_START
  char* host_name = 0;

  oyAllocHelper_m_( host_name, char, strlen( display_name ) + 48,0,return NULL);

  /* Is this X server identifyable? */
  if(!display_name)
  {
    sprintf( host_name, ":0.0" );
  } else {
    sprintf( host_name, display_name );
  }

  /* add screen */
  {
    const char *txt = strchr( host_name, ':' );

    /* fail if no display was given */
    if( !txt )
    { WARN_S(( "invalid display name" ))
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

/** get the geometry of a screen 
 *
 *  @param          disp      display info structure
 *  @return         error
 */
int
oyGetScreenGeometry_            (oy_display_s *disp)
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
                            WARN_S((_("open X Display failed"))); return 1 )

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
      WARN_S((_("Xinerama request failed")))
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
    oyPostAllocHelper_m_(scr, 1, WARN_S((_("open X Screen failed"))); return 1;)
    {
        disp->geo[1] = screen;
        disp->geo[2] = 0;
        disp->geo[3] = 0;
        disp->geo[4] = XWidthOfScreen( scr );
        disp->geo[5] = XHeightOfScreen( scr );
    }
  }

  return error;
}

int
oyGetDisplay_                      (const char *display_name,
                                    oy_display_s  *disp)
{ DBG_PROG_START
  int error = 0;
  int i = 0;

  disp->name = 0;
  disp->host = 0;
  disp->identifier = 0;
  disp->display = 0;
  disp->screen = 0;

  disp->type = oyDISPLAY_T;
  if( display_name && strlen( display_name ) )
    disp->name = strdup( display_name );
  else
    disp->name = strdup( ":0" );

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
{ DBG_PROG_START
  int err = 0;

  err = oyGetMonitorInfo_( display, manufacturer, model, serial, 0,
                           allocate_func);
  DBG_PROG_V(( strlen(*manufacturer) ))
  if(*manufacturer)
    DBG_PROG_S(( *manufacturer ));
  if(*model)
    DBG_PROG_S(( *model ));
  if(*serial)
    DBG_PROG_S(( *serial ));

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
{ DBG_PROG_START
  char* moni_profile = 0;

  moni_profile = oyGetMonitorProfile_( display, size, allocate_func );

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
{ DBG_PROG_START
  char* moni_profile = 0;

  moni_profile = oyGetMonitorProfileName_( display, allocate_func );

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
{ DBG_PROG_START
  int error = 0;

  error = oySetMonitorProfile_( display_name, profil_name );

  DBG_PROG_ENDE
  return error;
}

/** @brief activate the monitor using the stored configuration

 *  @param      display_name  the display string
 *  @return                   error
 *  @see oySetMonitorProfile for permanently configuring a monitor
 */
int
oyActivateMonitorProfile          (const char* display_name)
{ DBG_PROG_START
  int error = 0;

  error = oyActivateMonitorProfiles_( display_name );

  DBG_PROG_ENDE
  return error;
}

/** @brief screen number from position
 *
 *  This function will hit exact results only with Xinerama.
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
{ DBG_PROG_START
  int screen = 0;

  screen = oyGetScreenFromPosition_( display_name, x,y );
  DBG_PROG_S(( "x %d y %d screen %d\n", x,y,screen ));

  DBG_PROG_ENDE
  return screen;
}
