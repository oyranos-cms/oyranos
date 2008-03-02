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

#include "oyranos.h"
#include "oyranos_helper.h"
#include "oyranos_definitions.h"
#include "oyranos_debug.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

/* ---  Helpers  --- */

/* --- internal API definition --- */

int   oyGetMonitorInfo_           (const char* display,
                                   char**      manufacturer,
                                   char**      model,
                                   char**      serial);
char* oyGetMonitorProfileName_    (const char* display_name);

int   oyActivateMonitorProfile_   (const char* display_name,
                                   const char* profile_name);
int   oySetMonitorProfile_        (const char* display_name,
                                   const char* profile_name);
char* oyLongDisplayName_          (const char* display_name);

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
                                   char**      manufacturer,
                                   char**      model,
                                   char**      serial)
{ //oy_debug = 1;
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

  screen = DefaultScreen(display); DBG_PROG_V((screen))
  w = RootWindow(display, screen); DBG_PROG_S(("w: %ld", w))
  DBG_PROG 
  atom = XInternAtom(display, "XFree86_DDC_EDID1_RAWDATA", 1);
  DBG_PROG_S(("atom: %ld", atom))

  DBG_PROG

  XGetWindowProperty(display, w, atom, 0, 32, 0, AnyPropertyType, &a,
                     &actual_format_return, &nitems_return, &bytes_after_return,
                     &prop_return );

  XCloseDisplay(display);

  if( nitems_return != 128 ) {
    WARN_S((_("unexpected EDID lenght")))
    DBG_PROG_ENDE
    return 1;
  }

  // convert to an deployable struct
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
  // allocate new memory to release the supplied ID block
  len = strlen(edi->Mnf_Model); DBG_PROG_V((len))
  if(len) { DBG_PROG
    ++len;
    t = (char*)calloc( len, sizeof(char) );
    sprintf(t, edi->Mnf_Model);
    *manufacturer = t; DBG_PROG_S(( *manufacturer ))
  }
  len = strlen(edi->HW_ID); DBG_PROG_V((len))
  if(len) { DBG_PROG
    ++len;
    t = (char*)calloc( len, sizeof(char) );
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
oyGetMonitorProfileName_          (const char* display_name)
{ DBG_PROG_START

  char       *manufacturer=0,
             *model=0,
             *serial=0;
  char       *moni_profile=0,
             *host_name = 0;

  oyGetMonitorInfo_( display_name, &manufacturer, &model, &serial );

  host_name = oyLongDisplayName_ (display_name);


  /* search the profile in the database */
  moni_profile = oyGetDeviceProfile( oyDISPLAY, manufacturer, model, serial,
                                     host_name, 0,0,0,0);

  if(manufacturer) free(manufacturer);
  if(model) free(model);
  if(serial) free(serial);
  if(host_name) free(host_name);

  DBG_PROG_ENDE
  return moni_profile;
}

int
oyActivateMonitorProfile_         (const char* display_name,
                                   const char* profil_name )
{ DBG_PROG_START
  int error = 0;

  char       *profil_pathname;
  const char *profil_basename;

  DBG_PROG_S(( "profil_name = %s", profil_name ))
  
  profil_pathname = oyGetPathFromProfileName( profil_name );
  DBG_PROG_S(( "profil_pathname %s", profil_pathname ))

  if( profil_pathname ) {
    char *text = (char*) calloc (MAX_PATH, sizeof(char));
    if(strrchr(profil_name,OY_SLASH_C))
      profil_basename = strrchr(profil_name,OY_SLASH_C)+1;
    else
      profil_basename = profil_name;

    /* set vcgt tag with xcalib */
    sprintf(text,"xcalib -d %s %s%s%s", display_name,
                               profil_pathname, OY_SLASH, profil_basename);
    error = system(text);
    if(error) {
      WARN_S((_("Error while setting monitor gamma curves")))
    }

    DBG_PROG_S(( "system: %s", text ))

    /* set _ICC_PROFILE atom in X with xicc */
    sprintf(text,"xicc %s %s%s%s", display_name,
                               profil_pathname, OY_SLASH, profil_basename);
    error = system(text);
    if(error) {
      WARN_S((_("Error while setting X monitor property")))
    }

    DBG_PROG_S(( "system: %s", text ))
    if(text) free(text);
  }

  if (profil_pathname) free (profil_pathname);

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
             *serial=0;
  char       *host_name = oyLongDisplayName_(display_name);
  char *profil_pathname;
  error  =
    oyGetMonitorInfo_ (display_name, &manufacturer, &model, &serial);

  DBG_PROG
  if(error) {
    WARN_S((_("Error while requesting monitor information")))
    DBG_PROG_ENDE
    return error;
  }

  DBG_PROG_S(( "profil_name = %s", profil_name ))
  
  error =  oySetDeviceProfile(oyDISPLAY, manufacturer, model, serial,
                              host_name,0,0,0,0,profil_name,0,0);

  profil_pathname = oyGetPathFromProfileName( profil_name );
  DBG_PROG_S(( "profil_pathname %s", profil_pathname ))

  if( profil_pathname ) {
    error = oyActivateMonitorProfile_(display_name, profil_name);
  }

  if (profil_pathname) free (profil_pathname);

  DBG_PROG_ENDE
  return error;
}

char*
oyLongDisplayName_                (const char* display_name)
{ DBG_PROG_START
  char* host_name = 0;
  host_name = (char*) calloc (128, sizeof(char));

  /* Is this X server identifyable? */
  if(!display_name) {
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
  DBG_PROG_S(( "host_name = %s", host_name ))

  DBG_PROG_ENDE
  return host_name;
}


/* separate from the internal functions */

int
oyGetMonitorInfo                  (const char* display,
                                   char**      manufacturer,
                                   char**      model,
                                   char**      serial)
{ DBG_PROG_START
  int err = 0;

  err = oyGetMonitorInfo_( display, manufacturer, model, serial );
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

char*
oyGetMonitorProfileName           (const char* display)
{ DBG_PROG_START
  char* moni_profile = 0;

  moni_profile = oyGetMonitorProfileName_( display );

  DBG_PROG_ENDE
  return moni_profile;
}

int
oyActivateMonitorProfile          (const char* display_name,
                                   const char* profil_name )
{ DBG_PROG_START
  int error = 0;

  error = oyActivateMonitorProfile_( display_name, profil_name );

  DBG_PROG_ENDE
  return error;
}

int
oySetMonitorProfile               (const char* display_name,
                                   const char* profil_name )
{ DBG_PROG_START
  int error = 0;

  error = oySetMonitorProfile_( display_name, profil_name );

  DBG_PROG_ENDE
  return error;
}


