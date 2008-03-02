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

#include "oyranos.h"
#include "oyranos_internal.h"
#include "oyranos_helper.h"
#include "oyranos_debug.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

/* ---  Helpers  --- */

/* --- internal API definition --- */

int   oyGetMonitorInfo_           (const char* display,
                                   int         x,
                                   int         y,
                                   char**      manufacturer,
                                   char**      model,
                                   char**      serial,
                                   oyAllocFunc_t allocate_func);
char* oyGetMonitorProfileName_    (const char *display_name,
                                   int         x,
                                   int         y,
                                   oyAllocFunc_t allocate_func);
char* oyGetMonitorProfile_        (const char *display_name,
                                   int         x,
                                   int         y,
                                   size_t     *size,
                                   oyAllocFunc_t allocate_func);

int   oyActivateMonitorProfile_   (const char* display_name,
                                   int         x,
                                   int         y,
                                   const char* profile_name);
int   oyActivateMonitorProfiles_  (const char* display_name);
int   oySetMonitorProfile_        (const char* display_name,
                                   int         x,
                                   int         y,
                                   const char* profile_name);
char* oyCheckDisplayName_          (const char* display_name);
int   oyGetScreenFromDisplayName_ (const char* display_name);
int*  oyGetScreenGeometry_        (const char *display_name,
                                   int         screen,
                                   int         x,
                                   int         y,
                                   int        *len,
                                   int         all_b);


  /* an incomplete DDC struct */
struct DDC_EDID1 {
 char dummy[18];
 char major_version;
 char minor_version;
 char dummy1[58];
 char HW_ID[10];
 char dummy2[7];
 char Mnf_Model[16];
};



int
oyGetMonitorInfo_                 (const char* display_name,
                                   int     x,
                                   int     y,
                                   char**      manufacturer,
                                   char**      model,
                                   char**      serial,
                                   oyAllocFunc_t allocate_func)
{
  DBG_PROG_START

  Display *display; DBG_PROG
  int screen = 0;
  Window w;
  Atom atom, a;
  int actual_format_return, len;
  unsigned long nitems_return=0, bytes_after_return=0;
  unsigned char* prop_return=0;
  struct DDC_EDID1 *edi=0;
  char *t;

  if(display_name)
    DBG_PROG_S(("display_name %s",display_name));

  if( !(display = XOpenDisplay (display_name))) {
    WARN_S((_("open X Display failed")))
    DBG_PROG_ENDE
    return 1;
  }

  screen = XScreenOfDisplay( display,
                             oyGetScreenFromDisplayName_( display_name ));
  DBG_PROG_V((screen))
  w = RootWindow(display, screen); DBG_PROG_S(("w: %ld", w))
  DBG_PROG 
  atom = XInternAtom(display, "XFree86_DDC_EDID1_RAWDATA", 1);
  DBG_PROG_S(("atom: %ld", atom))

  DBG_PROG

  if(atom)
    XGetWindowProperty(display, w, atom, 0, 32, 0, AnyPropertyType, &a,
                     &actual_format_return, &nitems_return, &bytes_after_return,
                     &prop_return );

  XCloseDisplay(display);

  if( nitems_return != 128 ) {
    WARN_S((_("unexpected EDID lenght")))
    DBG_PROG_ENDE
    return 1;
  }

  /* convert to an deployable struct */
  edi = (struct DDC_EDID1*) prop_return;

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
oyGetMonitorProfile_          (const char* display_name, int x, int y,
                               size_t *size,
                               oyAllocFunc_t allocate_func)
{ DBG_PROG_START

  Display *display;
  int screen = 0;
  Window w;
  Atom atom, a;
  int actual_format_return;
  unsigned long nitems_return=0, bytes_after_return=0;
  unsigned char* prop_return=0;

  char       *moni_profile=0;

  if(display_name)
    DBG_PROG_S(("display_name %s",display_name));

  if( !(display = XOpenDisplay (display_name))) {
    WARN_S((_("open X Display failed")))
    *size = 0;
    DBG_PROG_ENDE
    return 0;
  }

  screen = DefaultScreen(display); DBG_PROG_V((screen))
  w = RootWindow(display, screen); DBG_PROG_S(("w: %ld", w))
  DBG_PROG 
  atom = XInternAtom(display, "_ICC_PROFILE", 1);
  DBG_PROG_S(("atom: %ld", atom))

  DBG_PROG

  if(atom)
    XGetWindowProperty(display, w, atom, 0, INT_MAX, 0, XA_CARDINAL, &a,
                     &actual_format_return, &nitems_return, &bytes_after_return,
                     &prop_return );

  XCloseDisplay(display);

  *size = nitems_return + bytes_after_return;
  moni_profile = prop_return;

  DBG_PROG_ENDE
  return moni_profile;
}

char*
oyGetMonitorProfileName_          (const char* display_name,
                                   int     x,
                                   int     y,
                                   oyAllocFunc_t allocate_func)
{ DBG_PROG_START

  char       *manufacturer=0,
             *model=0,
             *serial=0;
  char       *moni_profile=0,
             *host_name = 0;

  oyGetMonitorInfo_( display_name, x, y, &manufacturer, &model, &serial,
                     oyAllocateFunc_);

  host_name = oyCheckDisplayName_ (display_name);


  /* search the profile in the local database */
  /* It's not network transparent. */
  /* If working remotely, better fetch the whole profile instead. */
  moni_profile = oyGetDeviceProfile( oyDISPLAY, manufacturer, model, serial,
                                     host_name, 0,0,0,0, allocate_func);

  if(manufacturer) free(manufacturer);
  if(model) free(model);
  if(serial) free(serial);
  if(host_name) free(host_name);

  DBG_PROG_ENDE
  return moni_profile;
}

int*
oyAddToIntList_                (int* list, int* list_len, int num)
{
  int *t_list;

  t_list = (int*) calloc( sizeof(int), *list_len * num + num );
  memcpy( t_list, list, *list_len );
  free( list );
  *list_len += 1;

  return t_list;
}

int*
oyGetScreenGeometry_            (const char *display_name,
                                 int x, int y,
                                 int *len,
                                 int all_b)
{
  int* list = 0, list_len = 0, i, comp = 5;
  Display *display = 0;
  int screen = 0;
  char* identifier = 0;

  if(display_name)
    DBG_PROG_S(("display_name %s",display_name));

  if( !(display = XOpenDisplay (display_name))) {
    WARN_S((_("open X Display failed")))
    *len = 0;
    return 0;
  }

  screen = oyGetScreenFromDisplayName_( display_name );


  if( XineramaIsActive( display ) )
  {
    XineramaScreenInf *scr_info = XineramaQueryScreens( display, &n_scr_info );
    for (i = 0; i < n_scr_info; ++i)
    {
      if(all_b ||
         (x >= scr_info[i].x_org && x < scr_info[i].x_org + scr_info[i].width &&
          y >= scr_info[i].y_org && y < scr_info[i].y_org + scr_info[i].height))
      {
        list = oyAddToIntList_( list, &list_len, comp );
        list[ list_len*comp - 5 ] = i;
        list[ list_len*comp - 4 ] = scr_info[i].x_org;
        list[ list_len*comp - 3 ] = scr_info[i].y_org;
        list[ list_len*comp - 2 ] = scr_info[i].width;
        list[ list_len*comp - 1 ] = scr_info[i].height;
      }
    }
  }
  else
  {
    for (i = 0; i < ScreenCount( display ); ++i)
    {
      Screen *scr = XScreenOfDisplay( display, i );
      if( all_b ||
          ( scr &&
            i == screen ) )
      {
        Screen *scr = XScreenOfDisplay( display, i );
        list = oyAddToIntList_( list, &list_len, comp );
        list[ list_len*comp - 5 ] = -1;
        list[ list_len*comp - 4 ] = 0;
        list[ list_len*comp - 3 ] = 0;
        list[ list_len*comp - 2 ] = XWidthOfScreen( scr );
        list[ list_len*comp - 1 ] = XHeightOfScreen( scr );
      }
    }
  }

  *len = list_len;

  if( !*len )
    WARN_S(("no matching screen found"))

  return list;
}

/**
    takes a chaked display name and point as argument and
    gives a string back for search in the db
 */
char*
oyIdentifierFromGeometry_         (const char* display_name
                                   int         x,
                                   int         y )
{
  int all_b;
  int* scr_geo_list = 0;
  int n_scr = 0;
  char* identifier = 0;

  if(display_name)
    DBG_PROG_S(("display_name %s",display_name));

  screen = oyGetScreenFromDisplayName_( display_name );

  all_b = FALSE;
  scr_geo_list = oyGetScreenGeometry_( display_name, x,y, &n_scr, all_b );
  if( n_scr )
  {
    int len = strlen( display_name ) + 48;
    
    identifier = malloc( len );

    snprintf( identifier, len, "%s_%d:%d+%d+%dx%d", 
              display_name, scr_geo_list[0],
         scr_geo_list[1], scr_geo_list[2], scr_geo_list[3], scr_geo_list[4] );
  }

  return identifier;
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
  int *scr_geo_list = 0;
  int all_b = TRUE;  /* detect all screens */
  int ll = 5;        /* list entry length */

  if(display_name)
    DBG_PROG_S(("display_name %s",display_name));

  scr_geo_list = oyGetScreenGeometry_( display_name, 0,0, &n_scr, all_b );
  for( i = 0; i < n_scr; ++i )
  {
    error = oyActivateMonitorProfile_( display_name,
                           scr_geo_list[i*ll+1] + scr_geo_list[i*ll+3] / 2,
                           scr_geo_list[i*ll+2] + scr_geo_list[i*ll+4] / 2,
                                       0 );
  }

  DBG_PROG_ENDE
  return error;
}


int
oyActivateMonitorProfile_         (const char* display_name,
                                   int         x,
                                   int         y,
                                   const char* profil_name )
{ DBG_PROG_START
  int error = 0;

  char       *profil_pathname;
  const char *profil_basename;
  char* profile_name_ = 0;

  if(profil_name && x >= 0 && y >= 0)
  {
    DBG_PROG_S(( "profil_name = %s", profil_name ));
    profil_pathname = oyGetPathFromProfileName( profil_name, oyAllocateFunc_ );
  } else
  {
    char* profile_name_ = 0;
    
      oyGetMonitorProfileName_          ( display_name, x, y,
                                          oyAllocateFunc_);
    profil_pathname = oyGetPathFromProfileName( profile_name_, oyAllocateFunc_);
    profil_name = profile_name_;
  }

  if( profil_pathname && strlen(profil_pathname) )
  {
    char *text = (char*) calloc (MAX_PATH, sizeof(char));
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
      Atom atom;
      int screen = 0;
      Window w;

      char       *moni_profile=0;
      size_t      size=0;

      if(display_name)
        DBG_PROG_S(("display_name %s",display_name));

      if( !(display = XOpenDisplay (display_name))) {
        WARN_S((_("open X Display failed")))
      }

      /* TODO: multi screen */
      screen = XScreenOfDisplay( display,
                                 oyGetScreenFromDisplayName_( display_name ));
      DBG_PROG_V((screen))
      w = RootWindow(display, screen); DBG_PROG_S(("w: %ld", w))

      moni_profile = oyGetProfileBlock( profil_name, &size, oyAllocateFunc_ );
      if(!size || !moni_profile)
        WARN_S((_("Error obtaining profile")));

      atom = XInternAtom (display, "_ICC_PROFILE", False);
      if (atom == None) {
        WARN_S((_("Error setting up atom \"_ICC_PROFILE\"")));
      }

      XChangeProperty( display, w, atom, XA_CARDINAL,
                       8, PropModeReplace, moni_profile, (int)size );

      XCloseDisplay(display);

      if(moni_profile) free(moni_profile);
      moni_profile = 0;

      if(error) {
        WARN_S((_("Error while setting X monitor property")))
      }
    }

    DBG_PROG_S(( "system: %s", text ))
    if(text) free(text);
  }

  if (profil_pathname) free (profil_pathname);
  if (profile_name_) free (profile_name_);
  DBG_PROG_ENDE
  return error;
}

int
oySetMonitorProfile_              (const char* display_name,
                                   int     x,
                                   int     y,
                                   const char* profil_name )
{ DBG_PROG_START
  int error = 0;

  char       *manufacturer=0,
             *model=0,
             *serial=0;
  char       *host_name = oyCheckDisplayName_(display_name);
  char       *profil_pathname = 0;

  error  =
    oyGetMonitorInfo_ (display_name, x, y,, &manufacturer, &model, &serial,
                       oyAllocateFunc_);

  DBG_PROG
  if(error) {
    WARN_S((_("Error while requesting monitor information")))
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

      if(display_name)
        DBG_PROG_S(("display_name %s",display_name));

      if( !(display = XOpenDisplay (display_name))) {
        WARN_S((_("open X Display failed")))
      }

      /* TODO: multi screen */
      screen = XScreenOfDisplay( display,
                                 oyGetScreenFromDisplayName_( host_name ));
      DBG_PROG_V((screen))
      w = RootWindow(display, screen); DBG_PROG_S(("w: %ld", w))

      DBG_PROG

      atom = XInternAtom (display, "_ICC_PROFILE", False);
      if (atom == None) {
        WARN_S((_("Error setting up atom \"_ICC_PROFILE\"")));
      }

      XDeleteProperty( display, w, atom );
      XCloseDisplay(display);

      DBG_PROG
    goto finish;
  }

  DBG_PROG_S(( "profil_name = %s", profil_name ))

  error =  oySetDeviceProfile(oyDISPLAY, manufacturer, model, serial,
                              host_name,0,0,0,0,profil_name,0,0);

  profil_pathname = oyGetPathFromProfileName( profil_name, oyAllocateFunc_ );
  DBG_PROG_S(( "profil_pathname %s", profil_pathname ))

  if( profil_pathname ) {
    error = oyActivateMonitorProfile_(display_name, x, y, profil_name);
  }

  finish:
  DBG_PROG
  if (manufacturer) free (manufacturer);
  if (model) free (model);
  if (serial) free (serial);
  if (profil_pathname) free (profil_pathname);
  if (host_name) free (host_name);

  DBG_PROG_ENDE
  return error;
}

int
oyGetScreenFromDisplayName_        (const char* display_name)
{ DBG_PROG_START
  int scr_nummer = 0;

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

oyCheckDisplayName_                (const char* display_name)
{ DBG_PROG_START
  char* host_name = 0;
  host_name = (char*) calloc (128, sizeof(char));

  /* Is this X server identifyable? */
  if(!display_name)
  {
    char *host = getenv ("HOSTNAME");
    if (host) {
        sprintf( host_name, host );
        sprintf( host_name+strlen(host_name), ":0.0" );
    }
  } else if (strchr(display_name,':') == display_name) {
    char *host = getenv ("HOSTNAME");
    /* .. if not add host information */
    if (host) {
        sprintf( host_name, host );
        sprintf( host_name+strlen(host_name), display_name );
    }
  } else {
    sprintf( host_name, display_name );
  }

  /* add missing screen */
  {
    char ds[8];
    const char *txt = strchr( host_name, ':' );

    ++txt;

    /* fail if no display was given */
    if( !txt )
    { WARN_S(( "invalid display name" ))
      host_name[0] = 0;
      return host_name;
    }

    snprintf( ds, 8, txt );
    if( !strchr( display_name, '.' ) )
    {
      sprintf( &host_name[ strlen(host_name) ], ".0" );
    }
  }

  DBG_PROG_S(( "host_name = %s", host_name ))

  DBG_PROG_ENDE
  return host_name;
}


/* separate from the internal functions */

/** @brief pick up monitor information with Xlib
 *  @deprecated because sometimes is no ddc information available
 *  @todo include connection information - grafic cart
 *
 *  @param      display       the display string
 *  @param      x             position
 *  @param      y             position
 *  @param[out] manufacturer  the manufacturer of the monitor device
 *  @param[out] model         the model of the monitor device
 *  @param[out] serial        the serial number of the monitor device
 *  @param      allocate_func the allocator for the above strings
 *  @return success
 *
 */
int
oyGetMonitorInfo                  (const char* display,
                                   int     x,
                                   int     y,
                                   char**      manufacturer,
                                   char**      model,
                                   char**      serial,
                                   oyAllocFunc_t allocate_func)
{ DBG_PROG_START
  int err = 0;

  err = oyGetMonitorInfo_( display, x, y, manufacturer, model, serial,
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
 *  @param      x             position
 *  @param      y             position
 *  @param[out] size          the size of profile
 *  @param      allocate_func function used to allocate memory for the profile
 *  @return                   the memory block containing the profile
 */
char*
oyGetMonitorProfile           (const char* display,
                               int     x,
                               int     y,
                               size_t *size,
                               oyAllocFunc_t allocate_func)
{ DBG_PROG_START
  char* moni_profile = 0;

  moni_profile = oyGetMonitorProfile_( display, x, y, size, allocate_func );

  DBG_PROG_ENDE
  return moni_profile;
}

/** @brief get the monitor profile filename

 *  @param      display       the display string
 *  @param      x             position
 *  @param      y             position
 *  @param      allocate_func function used to allocate memory for the string
 *  @return                   the profiles filename (if localy available)
 */
char*
oyGetMonitorProfileName           (const char* display,
                                   int     x,
                                   int     y,
                                   oyAllocFunc_t allocate_func)
{ DBG_PROG_START
  char* moni_profile = 0;

  moni_profile = oyGetMonitorProfileName_( display, x, y, allocate_func );

  DBG_PROG_ENDE
  return moni_profile;
}

/** @brief set the monitor profile filename

 *  @param      display_name  the display string
 *  @param      x             position
 *  @param      y             position
 *  @param      profil_name   the file to use as monitor profile
 *  @return                   success
 */
int
oySetMonitorProfile               (const char* display_name,
                                   int     x,
                                   int     y,
                                   const char* profil_name )
{ DBG_PROG_START
  int error = 0;

  error = oySetMonitorProfile_( display_name, x, y, profil_name );

  DBG_PROG_ENDE
  return error;
}

/** @brief activate the monitor using the stored configuration

 *  @param      display_name  the display string
 *  @return                   success
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


