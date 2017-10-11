/** @file oyranos_monitor.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2005-2016 (C) Kai-Uwe Behrmann
 *
 *  @brief    monitor device detection
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2005/01/31
 */

#ifndef OY_UNUSED
#ifdef __GNUC__
#define OY_UNUSED                      __attribute__ ((unused))
#elif defined(_MSC_VER)
#define OY_UNUSED                      __declspec(unused)
#else
#define OY_UNUSED                      __attribute__ ((unused))
#endif
#endif

#include "oyranos_monitor_hooks.h"
#include "oyranos_monitor_hooks_x11.h"

#include <ctype.h> /* isdigit() */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <locale.h>
#include <errno.h>

/* ---  Helpers  --- */

#define noE( string ) ((string)?(string):"")
extern int oy_debug; /* print debug infos on stderr */
enum {
  oyOBJECT_MONITOR_S = 2,
  oyOBJECT_MONITOR_HOOKS_S = 120
};

#ifndef MAX_PATH
#define MAX_PATH 2048
#endif

/* --- internal API definition --- */

/** @internal Display functions */
const char* oyX1Monitor_name_( oyX1Monitor_s *disp ) { return disp->name; }
const char* oyX1Monitor_hostName_( oyX1Monitor_s *disp ) { return disp->host; }
const char* oyX1Monitor_identifier_( oyX1Monitor_s *disp ) { return disp->identifier; }
/** @internal the screen appendment for the root window properties */
char*       oyX1Monitor_screenIdentifier_( oyX1Monitor_s *disp )
{ char *number = 0;

  number = malloc( 24 ); if(!number) return "";
  number[0] = 0;
  if( disp->geo[1] >= 1 && !disp->screen ) sprintf( number,"_%d", disp->geo[1]);
  return number;
}
int oyX1Monitor_deviceScreen_( oyX1Monitor_s *disp ) { return disp->screen; }
int oyX1Monitor_number_( oyX1Monitor_s *disp ) { return disp->geo[0]; }
int oyX1Monitor_screen_( oyX1Monitor_s *disp ) { return disp->geo[1]; }
int oyX1Monitor_x_( oyX1Monitor_s *disp ) { return disp->geo[2]; }
int oyX1Monitor_y_( oyX1Monitor_s *disp ) { return disp->geo[3]; }
int oyX1Monitor_width_( oyX1Monitor_s *disp ) { return disp->geo[4]; }
int oyX1Monitor_height_( oyX1Monitor_s *disp ) { return disp->geo[5]; }
int   oyX1Monitor_getGeometryIdentifier_(oyX1Monitor_s       * disp );
Display* oyX1Monitor_device_( oyX1Monitor_s *disp ) { return disp->display; }
const char* oyX1Monitor_systemPort_( oyX1Monitor_s *disp ) { return disp->system_port; }

oyX11INFO_SOURCE_e
    oyX1Monitor_infoSource_( oyX1Monitor_s *disp ) { return disp->info_source; }
# if defined(HAVE_XRANDR)
XRRScreenResources *
    oyX1Monitor_xrrResource_( oyX1Monitor_s * disp ) { return disp->res; }
RROutput
    oyX1Monitor_xrrOutput_( oyX1Monitor_s * disp ) { return disp->output; }
XRROutputInfo *
    oyX1Monitor_xrrOutputInfo_( oyX1Monitor_s * disp ) { return disp->output_info; }
int oyX1Monitor_activeOutputs_( oyX1Monitor_s * disp ) { return disp->active_outputs; }
int oyX1Monitor_rrVersion_    ( oyX1Monitor_s * disp ) { return disp->rr_version; }
int oyX1Monitor_rrScreen_     ( oyX1Monitor_s * disp ) { return disp->rr_screen; }


#endif

char* oyX1Monitor_getAtomName_         ( oyX1Monitor_s       * disp,
                                       const char        * base );
const char *xrandr_edids[] = {"EDID","EDID_DATA",0};






/** @internal
 *  Function oyX1Monitor_getProperty_
 *  @brief   obtain X property
 *
 *  The returned property is owned by the caller.
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/11/25
 *  @since   2009/01/17 (Oyranos: 0.1.10)
 */
char *   oyX1Monitor_getProperty_    ( oyX1Monitor_s     * disp,
                                       const char        * prop_name,
                                       const char       ** prop_name_xrandr,
                                       size_t            * prop_size )
{
  char * prop = 0;
  Display *display = 0;
  Window w = 0;
  Atom atom = 0, a;
  char *atom_name = 0;
  int actual_format_return;
  unsigned long nitems_return=0, bytes_after_return=0;
  unsigned char* prop_return=0;
  int error = !disp;

  if(!error)
  {
    display = oyX1Monitor_device_( disp );
# if defined(HAVE_XRANDR)
    if( oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR )
    {
      int i = 0;
      if(prop_name_xrandr)
        while(!atom && prop_name_xrandr[i])
          atom = XInternAtom( display,
                              prop_name_xrandr[i++],
                              True );
      else
        atom = XInternAtom( display, prop_name, True );

      if(atom)
      {
        error =
        XRRGetOutputProperty ( display, oyX1Monitor_xrrOutput_( disp ),
                      atom, 0, INT_MAX,
                      False, False, AnyPropertyType, &a,
                      &actual_format_return, &nitems_return,
                      &bytes_after_return, &prop_return );

        if(error != Success)
          fprintf( stderr,"%s nitems_return: %lu, bytes_after_return: %lu %d\n",
                   "found issues", nitems_return, bytes_after_return,
                   error );
        if(oy_debug)
          atom_name = XGetAtomName(display, atom);
        if(oy_debug) fprintf( stderr, "root: %d atom: %ld atom_name: %s prop_name: %s %lu %lu\n",
                  (int)w, atom, atom_name, prop_name, nitems_return,bytes_after_return );
      }
    }
#else
    if(oy_debug) fprintf( stderr,"!HAVE_XRANDR\n");
# endif
    atom = 0;

    if( oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XINERAMA ||
        oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_SCREEN ||
        (oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR &&
          !nitems_return) )
    {
      atom_name = oyX1Monitor_getAtomName_( disp, prop_name );
      if(atom_name)
        atom = XInternAtom(display, atom_name, True);
      if(atom)
        w = RootWindow( display, oyX1Monitor_deviceScreen_( disp ) );
      if(w)
        /* AnyPropertyType does not work for XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE ---vvvvvvvvvv */
        XGetWindowProperty( display, w, atom, 0, INT_MAX, False,
                     AnyPropertyType,
                     &a, &actual_format_return, &nitems_return, 
                     &bytes_after_return, &prop_return );
      if(bytes_after_return != 0) fprintf( stderr,"%s bytes_after_return: %lu\n",
                                          "found issues",bytes_after_return);
      if(oy_debug) fprintf( stderr, "root: %d atom: %ld atom_name: %s prop_name: %s %lu %lu\n",
                  (int)w, atom, atom_name, prop_name, nitems_return,bytes_after_return );
      if(atom_name)
        free( atom_name );
    }
  }

  if(nitems_return && prop_return)
  {
    prop = malloc( nitems_return );
    if(!prop)
      return prop;
    memcpy( prop, prop_return, nitems_return );
    *prop_size = nitems_return;
    XFree( prop_return ); prop_return = 0;
  }

  return prop;
}

int      oyX1Monitor_setProperty_    ( oyX1Monitor_s     * disp,
                                       const char        * prop_name,
                                       char              * prop,
                                       size_t              prop_size )
{
  Display *display = 0;
  Window w = 0;
  Atom atom = 0;
  char *atom_name = 0;
  int error = !disp;

  if(!error)
  {
    display = oyX1Monitor_device_( disp );
# if defined(HAVE_XRANDR)
    if( oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR )
    {
      atom = XInternAtom( display, prop_name, True );

      if(atom)
      {
        if( oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR )
        {
          XRRChangeOutputProperty( display, oyX1Monitor_xrrOutput_( disp ),
			           atom, XA_CARDINAL, 8, PropModeReplace,
			           (unsigned char *)prop, (int)prop_size );
          if(oy_debug) fprintf( stderr,"XRRChangeOutputProperty[%s] = %lu\n", prop_name, prop_size);
          if(prop_size == 0)
          XRRDeleteOutputProperty( display, oyX1Monitor_xrrOutput_( disp ), atom );
        }
      }
    }
#else
    if(oy_debug) fprintf( stderr,"!HAVE_XRANDR\n");
# endif
    atom = 0;

    if( oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XINERAMA ||
        oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_SCREEN ||
        oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR )
    {
      atom_name = oyX1Monitor_getAtomName_( disp, prop_name );
      if(atom_name)
        atom = XInternAtom(display, atom_name, True);
      if(atom)
        w = RootWindow( display, oyX1Monitor_deviceScreen_( disp ) );
      if(w)
        /* AnyPropertyType does not work for XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE ---vvvvvvvvvv */
        error = XChangeProperty( display, w, atom, XA_CARDINAL,
                       8, PropModeReplace, (unsigned char*)prop, (int)prop_size );
      if(oy_debug) fprintf( stderr,"XChangeProperty[%s] = %lu\n", atom_name, prop_size);
      if(atom_name)
        free( atom_name );
    }
  }

  return error;
}

/*#define IGNORE_EDID 1*/

int      oyX1GetMonitorEdid          ( oyX1Monitor_s     * disp,
                                       char             ** edid,
                                       size_t            * edid_size,
                                       int                 refresh_edid )
{
  char * prop = 0;
  size_t prop_size = 0;
  int error = 0;
 
#if !defined(IGNORE_EDID)
  prop = oyX1Monitor_getProperty_( disp, "XFree86_DDC_EDID1_RAWDATA",
                                   xrandr_edids, &prop_size );
#else
    if(oy_debug) fprintf( stderr,"IGNORE_EDID\n");
#endif

  if( oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XINERAMA &&
      ((!prop || (prop && prop_size%128)) ||
       refresh_edid ) )
  {
#if !defined(IGNORE_EDID)
    prop = oyX1Monitor_getProperty_( disp, "XFree86_DDC_EDID1_RAWDATA",
                                         xrandr_edids, &prop_size );
#else
    if(oy_debug) fprintf( stderr,"IGNORE_EDID\n");
#endif
  }

  if( prop )
  {
    if( prop_size%128 )
    {
      fprintf( stderr, "\n\t  %s %d; %s %s\n","unexpected EDID lenght",
               (int)prop_size,
               "\"XFree86_DDC_EDID1_RAWDATA\"/\"EDID_DATA\"",
               "Cant read hardware information from device.");
      error = -1;
    }
  }

  if(edid)
  {
    *edid = prop;
    *edid_size = prop_size;
    prop = 0;
  }

  if(prop_size && prop) free( prop );

  return error;
}

char * oyX1OpenFile( const char * file_name,
                        size_t   * size_ptr )
{
  FILE * fp = NULL;
  size_t size = 0, s = 0;
  char * text = NULL;

  if(file_name)
  {
    fp = fopen(file_name,"rb");
    if(fp)
    {
      fseek(fp,0L,SEEK_END); 
      size = ftell (fp);
      if(size == (size_t)-1)
      {
        switch(errno)
        {
          case EBADF:        fprintf(stderr, "Not a seekable stream"); break;
          case EINVAL:       fprintf(stderr, "Wrong argument"); break;
          default:           fprintf(stderr, "%s", strerror(errno)); break;
        }
        if(size_ptr)
          *size_ptr = size;
        return NULL;
      }
      rewind(fp);
      text = malloc(size+1);
      if(text == NULL)
      {
        fprintf( stderr, "Error: Could allocate memory: %lu", (long unsigned int)size);
        fclose( fp );
        return NULL;
      }
      s = fread(text, sizeof(char), size, fp);
      text[size] = '\000';
      if(s != size)
        fprintf( stderr, "Error: fread %lu but should read %lu",
                (long unsigned int) s, (long unsigned int)size);
      fclose( fp );
    } else
    {
      fprintf( stderr, "Error: Could not open file - \"%s\"", file_name);
    }
  }

  if(size_ptr)
    *size_ptr = size;

  return text;
}


/** @brief pick up monitor information with Xlib
 *  @deprecated because sometimes is no ddc information available
 *  @todo include connection information - grafic cart
 *
 *  @param      display_name  the display string
 *  @param[out] manufacturer  the manufacturer of the monitor device
 *  @param[out] model         the model of the monitor device
 *  @param[out] serial        the serial number of the monitor device
 *  @return     error
 *
 */
int      oyX1GetMonitorInfo          ( const char        * display_name,
                                       char             ** manufacturer OY_UNUSED,
                                       char             ** mnft,
                                       char             ** model OY_UNUSED,
                                       char             ** serial OY_UNUSED,
                                       char             ** vendor OY_UNUSED,
                                       char             ** display_geometry,
                                       char             ** system_port,
                                       char             ** host,
                                       int               * week,
                                       int               * year,
                                       int               * mnft_id OY_UNUSED,
                                       int               * model_id,
                                       double            * colors,
                                       char             ** edid,
                                       size_t            * edid_size,
                                       int                 refresh_edid )
{
  int len;
  char *t, * port = 0, * geo = 0;
  oyX1Monitor_s * disp = 0;
  char * prop = 0;
  size_t prop_size = 0;
  int error = 0;

  if(display_name)
    if(oy_debug) fprintf( stderr,"display_name %s\n",display_name);

  disp = oyX1Monitor_newFrom_( display_name, 1 );
  if(!disp)
    return -1;

  {
    t = 0;
    if( oyX1Monitor_systemPort_( disp ) &&
        strlen(oyX1Monitor_systemPort_( disp )) )
    {
      len = strlen(oyX1Monitor_systemPort_( disp ));
      ++len;
      t = (char*)malloc( len );
      strcpy(t, oyX1Monitor_systemPort_( disp ));
    }
    port = t;
    if( system_port ) 
      *system_port = port;
    t = 0;
  }

  if( display_geometry )
    *display_geometry = strdup( oyX1Monitor_identifier_( disp ) );
  else
    geo = strdup( oyX1Monitor_identifier_( disp ) );
  if( host )
    *host = strdup( oyX1Monitor_hostName_( disp ) );

  error = oyX1GetMonitorEdid( disp, &prop, &prop_size, refresh_edid );

  if( !prop )
  /* as a last means try Xorg.log for at least some informations */
  {
    char * log_file = malloc(256);
    char * log_text = 0;
    int screen = oyX1Monitor_screen_( disp ), i;

    if(log_file)
    {
      size_t size = 0;

#define X_LOG_PATH  "/var/log/"
      sprintf( log_file, X_LOG_PATH "Xorg.%d.log", oyX1Monitor_number_(disp) );

      log_text = oyX1OpenFile( log_file, &size );

      if(!log_text)
      {
        free(log_file);
        error = 1;
        goto cleanInfoMem;
      } else
        log_text[size] = '\000';
    }

    if(log_text)
    {
      float rx=0,ry=0,gx=0,gy=0,bx=0,by=0,wx=0,wy=0,g=0;
      int year_ = 0, week_ = 0;
      const char * t;
      char mnft_[80] = {0};
      unsigned int model_id_ = 0;

      char * save_locale = 0;
      /* sensible parsing */
      save_locale = strdup( setlocale( LC_NUMERIC, 0 ) );
      setlocale( LC_NUMERIC, "C" );

      t = strstr( log_text, "Connected Display" );
      if(!t) t = log_text;
      if(port)
        t = strstr( t, port );
      if(!t)
      {
        t = log_text;
        if(t && t[0])
        for(i = 0; i < screen; ++i)
        {
          ++t;
          t = strstr( t, "redX:" );
        }
      }

      if(t && (t = strstr( t, "Manufacturer:" )) != 0)
      {
        sscanf( t,"Manufacturer: %s", mnft_ );
      }

      if(t && (t = strstr( t, "Model:" )) != 0)
      {
        sscanf( t,"Model: %x ", &model_id_ );
      }

      if(t && (t = strstr( t, "Year:" )) != 0)
      {
        sscanf( t,"Year: %d  Week: %d", &year_, &week_ );
      } 

      if(t && (t = strstr( t, "Gamma:" )) != 0)
        sscanf( t,"Gamma: %g", &g );
      if(t && (t = strstr( t, "redX:" )) != 0)
        sscanf( t,"redX: %g redY: %g", &rx,&ry );
      if(t && (t = strstr( t, "greenX:" )) != 0)
        sscanf( t,"greenX: %g greenY: %g", &gx,&gy );
      if(t && (t = strstr( t, "blueX:" )) != 0)
        sscanf( t,"blueX: %g blueY: %g", &bx,&by );
      if(t && (t = strstr( t, "whiteX:" )) != 0)
        sscanf( t,"whiteX: %g whiteY: %g", &wx,&wy );

      if(mnft_[0])
      {
        *mnft = strdup( mnft_ );
        *model_id = model_id_;
        colors[0] = rx;
        colors[1] = ry;
        colors[2] = gx;
        colors[3] = gy;
        colors[4] = bx;
        colors[5] = by;
        colors[6] = wx;
        colors[7] = wy;
        colors[8] = g;
        *year = year_;
        *week = week_;
        fprintf( stderr,  "found %s in \"%s\": %s %d %s\n",
                   log_file, display_name, mnft_, model_id_,
                   display_geometry?noE(*display_geometry):geo);
      }

      setlocale(LC_NUMERIC, save_locale);
      if(save_locale)
        free( save_locale );
      free(log_text);
    }
    free(log_file);
  }

cleanInfoMem:
  if(edid)
  {
    *edid = prop;
    *edid_size = prop_size;
    prop = 0;
  }

  oyX1Monitor_release_( &disp );
  if(geo) free(geo);
  if( port && !system_port ) free( port );

  if(prop || (edid && *edid))
  {
    if(prop_size && prop) free( prop );
    return error;
  } else {
    const char * log = "Can not read hardware information from device.";
    int r = -1;

    if(*mnft && (*mnft)[0])
    {
      log = "using Xorg log fallback.";
      r = 0;
    }

    fprintf( stderr,  "\n  %s:\n  %s\n  %s\n",
               "no EDID available from X properties",
               "\"XFree86_DDC_EDID1_RAWDATA\"/\"EDID_DATA\"",
               noE(log));
    return r;
  }
}



char *       oyX1GetMonitorProfile   ( const char        * device_name,
                                       int                 flags,
                                       size_t            * size )
{
  char * moni_profile = NULL;


  oyX1Monitor_s * disp = 0;
  char * prop = 0;
  size_t prop_size = 0;

  if(device_name)
    if(oy_debug) fprintf( stderr,"device_name %s\n",device_name);

  disp = oyX1Monitor_newFrom_( device_name, flags & 0x02 ? 1 : 0 );
  if(!disp)
    return 0;

#if defined(XCM_HAVE_X11) && defined(HAVE_XCM)
  /* support the color server device profile */
  if(flags & 0x01)
    prop = oyX1Monitor_getProperty_( disp,
                             XCM_ICC_COLOUR_SERVER_TARGET_PROFILE_IN_X_BASE, 0, &prop_size );
#endif

  /* alternatively fall back to the non color server or pre v0.4 atom */
  if(!prop)
#if defined(XCM_HAVE_X11) && defined(HAVE_XCM)
    prop = oyX1Monitor_getProperty_( disp, XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE, 0, &prop_size );
#else
    prop = oyX1Monitor_getProperty_( disp, "_ICC_PROFILE", 0, &prop_size );
#endif

  if(prop)
  {
    moni_profile = prop;
    if(moni_profile)
    {
      *size = prop_size;
    }
  } /*else
    fprintf( stderr,"\n  %s",
         _("Could not get Xatom, probably your monitor profile is not set:"));*/

  oyX1Monitor_release_( &disp );

  return moni_profile;
}


int      oyX1GetAllScreenNames       ( const char        * display_name,
                                       char            *** display_names )
{
  int i = 0;

  *display_names = oyX1GetAllScreenNames_( display_name, &i );

  return i; 
}

char**
oyX1GetAllScreenNames_          (const char *display_name,
                                 int *n_scr)
{
  int i = 0;
  char** list = 0;

  Display *display = 0;
  int len = 0;
  oyX1Monitor_s * disp = 0;

  *n_scr = 0;

  if(!display_name || !display_name[0])
  {
    fprintf( stderr, "No display_name\n" );
    return 0;
  }

  disp = oyX1Monitor_newFrom_( display_name, 0 );
  if(!disp)
  {
#ifdef __APPLE__
    if(oy_debug)
#endif
    fprintf( stderr, "No disp object\n" );
    return 0;
  }

  display = oyX1Monitor_device_( disp );

  if( !display || (len = ScreenCount( display )) == 0 )
  {
    if(!display)
      fprintf( stderr, "No display struct\n" );
    else
      fprintf( stderr, "No ScreenCount %d\n", len );
    return 0;
  }

# if defined(HAVE_XRANDR)
  if( oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR)
    len = disp->active_outputs;
# endif

# if defined(HAVE_XINERAMA)
  /* test for Xinerama screens */
  if( oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XINERAMA)
    {
      int n_scr_info = 0;
      XineramaScreenInfo *scr_info = XineramaQueryScreens( display, &n_scr_info );
      if(!scr_info || !n_scr_info) return 0;

      if( n_scr_info >= 1 )
        len = n_scr_info;

      XFree( scr_info );
    }
# endif

  list = malloc( sizeof(char*) * len );
  if(!list) return NULL;

  for (i = 0; i < len; ++i)
    if( (list[i] = oyX1ChangeScreenName_( display_name, i )) == 0 )
    {
      fprintf( stderr, "oyChangeScreenName_failed %s %d\n",
               noE(display_name), i );
      free( list );
      return NULL;
    }

  *n_scr = len;
  oyX1Monitor_release_( &disp );


  return list;
}


/** @internal
 *  Function oyX1Rectangle_FromDevice
 *  @memberof monitor_api
 *  @brief   value filled a oyStruct_s object
 *
 *  @param         device_name         the Oyranos specific device name
 *  @return                            the rectangle the device projects to
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/28 (Oyranos: 0.1.10)
 *  @date    2009/01/28
 */
int          oyX1Rectangle_FromDevice( const char        * device_name,
                                       double            * x,
                                       double            * y,
                                       double            * width,
                                       double            * height )
{
  int error = !device_name;

  if(!error)
  {
    oyX1Monitor_s * disp = 0;

    disp = oyX1Monitor_newFrom_( device_name, 0 );
    if(!disp)
      return 1;

    *x = oyX1Monitor_x_(disp);
    *y = oyX1Monitor_y_(disp);
    *width = oyX1Monitor_width_(disp);
    *height = oyX1Monitor_height_(disp);

    oyX1Monitor_release_( &disp );
  }

  return 0;
}


/** @internal
    takes a chaked display name and point as argument and
    gives a string back for search in the db
 */
int
oyX1Monitor_getGeometryIdentifier_         (oyX1Monitor_s  *disp)
{
  int len = 64;

  disp->identifier = malloc(len);
  if(!disp->identifier) return 1;

  snprintf( disp->identifier, len, "%dx%d+%d+%d", 
            oyX1Monitor_width_(disp), oyX1Monitor_height_(disp),
            oyX1Monitor_x_(disp), oyX1Monitor_y_(disp) );

  return 0;
}

char* oyX1Monitor_getAtomName_         ( oyX1Monitor_s       * disp,
                                       const char        * base )
{
  int len = 64;
  char *atom_name = malloc(len);
  char *screen_number = oyX1Monitor_screenIdentifier_( disp );

  if(!screen_number) return 0;
  if(!atom_name) { free( screen_number ); return 0; }

  snprintf( atom_name, len, "%s%s", base, screen_number );

  free( screen_number );

  return atom_name;
}

void  oyX1Monitor_setCompatibility   ( oyX1Monitor_s     * disp,
                                       const char        * profile_name )
{
  char * prop = 0;
  size_t prop_size = 0;
  int refresh_edid = 1;
  char * command = malloc(4096);

  if(!command) return;

  oyX1GetMonitorEdid( disp, &prop, &prop_size, refresh_edid );

  sprintf( command, "oyranos-compat-gnome -q %s -i -", profile_name?"-a":"-e" );
  if(profile_name)
    sprintf( &command[strlen(command)], " -p \"%s\"", profile_name );

  if(oy_debug)
    fprintf( stderr, "%s\n", command );
  if(prop && prop_size)
  {
    FILE * s = popen( command, "w" );

    if(s)
    {
      fwrite( prop, sizeof(char), prop_size, s );

      pclose(s); s = 0;
    } else
      fprintf( stderr, "fwrite(%s) : %s\n", command, strerror(errno));

    free(prop);
  }
  free( command );
}

int      oyX1MonitorProfileSetup     ( const char        * display_name,
                                       const char        * profile_name,
                                       const char        * profile_data,
                                       size_t              profile_data_size )
{
  int error = 0;
  oyX1Monitor_s * disp = 0;
  char       *dpy_name = NULL;
  char *text = 0;

  /* XRandR needs a expensive initialisation */
  disp = oyX1Monitor_newFrom_( display_name, 1 );
  if(!disp)
    return -1;

  dpy_name = calloc( sizeof(char), MAX_PATH );
  if(!dpy_name) goto Clean;
  if( display_name && !strstr( disp->host, display_name ) )
    snprintf( dpy_name, MAX_PATH, ":%d", disp->geo[0] );
  else
    snprintf( dpy_name, MAX_PATH, "%s:%d", disp->host, disp->geo[0] );

  if(oy_debug) fprintf( stderr, "profile_name = %s\n", profile_name?profile_name:"" );

  if( profile_name && profile_name[0] )
  {
    text = malloc(MAX_PATH);
    if(!text) goto Clean;

    /** set vcgt tag with xcalib
       not useable with multihead Xinerama at one screen

       @todo TODO xcalib should be configurable as a module
     */
    sprintf(text,"xcalib -d %s -s %d %s \'%s\'", dpy_name, disp->geo[1],
                 oy_debug?"-v":"", profile_name);
    if(oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR)
#if defined(HAVE_XRANDR)
      sprintf(text,"xcalib -d %s -s %d %s \'%s\'", dpy_name, oyX1Monitor_rrScreen_(disp),
              oy_debug?"-v":"", profile_name);
#else
      sprintf(text,"xcalib -d %s -s %d %s \'%s\'", dpy_name, 0,
              oy_debug?"-v":"", profile_name);
#endif
    else
      sprintf(text,"xcalib -d %s -s %d %s \'%s\'", dpy_name, disp->geo[1],
              oy_debug?"-v":"", profile_name);

    {
      Display * display = oyX1Monitor_device_( disp );
      int effective_screen = oyX1Monitor_screen_( disp );
      int screen = oyX1Monitor_deviceScreen_( disp );
      int size = 0,
          can_gamma = 0;

      if(!display)
      {
        fprintf( stderr,"%s %s %s\n", "open X Display failed", dpy_name, display_name);
        free( text );
	goto Clean;
      }

#ifdef HAVE_XXF86VM
      if(effective_screen == screen)
      {
        XF86VidModeGamma gamma;
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
      if(can_gamma || oyX1Monitor_screen_( disp ) == 0 || oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR )
      {
      /* OS X handles VGCT fine, no need for xcalib */
#if !defined(__APPLE__)
        error = system(text);
#endif
      }
      if(error &&
         error != 65280)
      { /* hack */
        fprintf( stderr,"%s %s %d\n", "No monitor gamma curves by profile:",
                noE(profile_name), error );
        error = -1;
      } else
      {
        /* take xcalib error not serious, turn into a issue */
        if(oy_debug)
          if(oy_debug) fprintf( stderr,"xcalib returned %d\n", error);
        error = -1;
      }
    }

    if(oy_debug)
      if(oy_debug) fprintf( stderr, "system: %s\n", text );

    /* set XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE atom in X */
    {
      Display *display;
      Atom atom = 0;
      int screen = 0;
      Window w;

      char       *atom_name=0;
      int         result = 0;

      if(display_name)
        if(oy_debug) fprintf( stderr,"display_name %s\n",display_name);

      display = oyX1Monitor_device_( disp );

      screen = oyX1Monitor_deviceScreen_( disp );
      if(oy_debug) fprintf( stderr, "screen: %d\n", screen);
      w = RootWindow(display, screen); if(oy_debug) fprintf( stderr,"w: %ld\n", w);

      if(!profile_data_size || !profile_data)
        fprintf( stderr,"Error obtaining profile\n");

#if defined(XCM_HAVE_X11) && defined(HAVE_XCM)
      atom_name = oyX1Monitor_getAtomName_( disp, XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE );
#else
      atom_name = oyX1Monitor_getAtomName_( disp, "_ICC_PROFILE" );
#endif
      if( atom_name )
      {
        atom = XInternAtom (display, atom_name, False);
        if (atom == None) {
          fprintf( stderr,"%s \"%s\"\n", "Error setting up atom", atom_name);
        }
      } else fprintf( stderr,"Error setting up atom\n");

      if( atom && profile_data)
      result = XChangeProperty( display, w, atom, XA_CARDINAL,
                       8, PropModeReplace, (unsigned char*)profile_data, (int)profile_data_size );
      if(result == 0) fprintf( stderr,"%s %d\n", "found issues",result);

# if defined(HAVE_XRANDR)
      if( oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR )
      {
#if defined(XCM_HAVE_X11) && defined(HAVE_XCM)
        atom = XInternAtom( display, XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE, True );
#else   
        atom = XInternAtom( display, "_ICC_PROFILE", True );
#endif

        if(atom)
        {
          XRRChangeOutputProperty( display, oyX1Monitor_xrrOutput_( disp ),
			           atom, XA_CARDINAL, 8, PropModeReplace,
			           (unsigned char *)profile_data, (int)profile_data_size );

          if(oy_debug)
            atom_name = XGetAtomName(display, atom);
          if(oy_debug) fprintf( stderr, "output: \"%s\" crtc: %d atom_name: %s\n",
               noE(oyX1Monitor_xrrOutputInfo_(disp)->name),
               (int)oyX1Monitor_xrrOutputInfo_(disp)->crtc, atom_name );
        }
      }
#else
      if(oy_debug) fprintf( stderr,"!HAVE_XRANDR\n");
# endif

      /* claim to be compatible with 0.4 
       * http://www.freedesktop.org/wiki/OpenIcc/ICC_Profiles_in_X_Specification_0.4
       */
      atom = XInternAtom( display, "_ICC_PROFILE_IN_X_VERSION", False );
      if(atom)
      {
        Atom a;
        /* 0.4 == 100*0 + 1*4 = 4 */
        const unsigned char * value = (const unsigned char*)"4";
        int actual_format_return;
        unsigned long nitems_return=0, bytes_after_return=0;
        unsigned char* prop_return=0;

        XGetWindowProperty( display, w, atom, 0, INT_MAX, False, XA_STRING,
                     &a, &actual_format_return, &nitems_return, 
                     &bytes_after_return, &prop_return );
        if(bytes_after_return != 0) fprintf( stderr,"%s bytes_after_return: %lu\n",
                                          "found issues",bytes_after_return);
        /* check if the old value is the same as our intented */
        if(actual_format_return != XA_STRING ||
           nitems_return == 0)
        {
          if(!prop_return || strcmp( (char*)prop_return, (char*)value ) != 0)
          result = XChangeProperty( display, w, atom, XA_STRING,
                                    8, PropModeReplace,
                                    value, 4 );
          if(result == 0) fprintf( stderr,"%s %d\n", "found issues",result);
        }
      }

      free( atom_name );

      oyX1Monitor_setCompatibility( disp, profile_name );
    }

    free( text );
  }

  Clean:
  oyX1Monitor_release_( &disp );
  if(dpy_name) free( dpy_name );

  return error;
}


int      oyX1MonitorProfileUnset     ( const char        * display_name )
{
  int error = 0;


  oyX1Monitor_s * disp = 0;

  /* XRandR needs a expensive initialisation */
  disp = oyX1Monitor_newFrom_( display_name, 1 );
  if(!disp)
  {
    return -1;
  }


  {
    /* unset the XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE atom in X */
      Display *display;
      Atom atom;
      int screen = 0;
      Window w;
      char *atom_name = 0;
      char * command = 0;

      if(display_name && oy_debug)
        if(oy_debug) fprintf( stderr,"display_name %s\n",display_name);

      display = oyX1Monitor_device_( disp );

      screen = oyX1Monitor_deviceScreen_( disp );
      if(oy_debug) fprintf( stderr, "screen: %d\n", screen);
      w = RootWindow(display, screen); if(oy_debug) fprintf( stderr,"w: %ld\n", w);


#if defined(XCM_HAVE_X11) && defined(HAVE_XCM)
      atom_name = oyX1Monitor_getAtomName_( disp, XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE );
#else
      atom_name = oyX1Monitor_getAtomName_( disp, "_ICC_PROFILE" );
#endif
      atom = XInternAtom (display, atom_name, True);
      if (atom != None)
        XDeleteProperty( display, w, atom );
      else
      {
        fprintf( stderr,"%s \"%s\"\n", "Error getting atom", atom_name);
        error = -1;
      }
# if defined(HAVE_XRANDR)
      if( oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR )
      {
#if defined(XCM_HAVE_X11) && defined(HAVE_XCM)
        atom = XInternAtom( display, XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE, True );
#else   
        atom = XInternAtom( display, "_ICC_PROFILE", True );
#endif
        if(atom != None)
        {
          /* need a existing property to remove; sorry for the noice */
          XRRChangeOutputProperty( display, oyX1Monitor_xrrOutput_( disp ),
			           atom, XA_CARDINAL, 8, PropModeReplace,
			           (unsigned char *)NULL, (int)0 );
          XRRDeleteOutputProperty( display, oyX1Monitor_xrrOutput_( disp ), atom );
          if(oy_debug)
            atom_name = XGetAtomName(display, atom);
          if(oy_debug) fprintf( stderr, "output: \"%s\" crtc: %d atom_name: %s\n",
               noE(oyX1Monitor_xrrOutputInfo_(disp)->name),
               (int)oyX1Monitor_xrrOutputInfo_(disp)->crtc, atom_name );
        }
      }
#else
      if(oy_debug) fprintf(stderr,"!HAVE_XRANDR\n");
# endif

      {
        char *dpy_name = strdup( noE(display_name) );
        char *ptr = NULL;
        int r = 0;

        if(!dpy_name) goto finish;
        command = malloc(1048);
        if(!command) { free(dpy_name); goto finish; }

        if( (ptr = strchr(dpy_name,':')) != 0 )
          if( (ptr = strchr(ptr,'.')) != 0 )
            ptr[0] = '\000';

        if(oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR)
          snprintf(command, 1024, "xrandr -display %s --output %s --gamma .999999:.999999:.999999",
                 dpy_name, oyX1Monitor_systemPort_(disp));
        else
          snprintf(command, 1024, "xgamma -gamma 1.0 -screen %d -display %s",
                 disp->geo[1], dpy_name);

        if(oy_debug)
          if(oy_debug) fprintf( stderr, "%d %d system: %s\n", screen, disp->geo[1], command );
        if(screen == disp->geo[1] || oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR)
          r = system( command );
        if(r) fprintf( stderr,"%s %d\n", "found issues",r);

        free( dpy_name );
        free( command );
      }

      oyX1Monitor_setCompatibility( disp, NULL );

      free( atom_name );
    goto finish;
  }


  finish:
  oyX1Monitor_release_( &disp );

  return error;
}

int
oyGetDisplayNumber_        (oyX1Monitor_s *disp)
{
  int dpy_nummer = 0;
  const char *display_name = oyX1Monitor_name_(disp);


  if( display_name )
  {
    char ds[8];             /* display.screen*/
    const char *txt = strchr( display_name, ':' );
    
    if( !txt )
    { fprintf( stderr, "invalid display name: %s\n", display_name );
      return -1;
    }

    if(txt[0])
      ++txt;
    strncpy( ds, txt, strlen(txt) > 8 ? 8 : strlen(txt) );
    ds[7] = '\000';
    if( strrchr( ds, '.' ) )
    {
      char *end = strchr( ds, '.' );
      if( end )
        *end = 0;
    }
    dpy_nummer = atoi( ds );
  }

  return dpy_nummer;
}

int   oyX1Monitor_getScreenFromDisplayName_( oyX1Monitor_s   * disp )
{
  int scr_nummer = 0;
  const char *display_name = oyX1Monitor_name_(disp);

  if( display_name )
  {
    char ds[8];             /* display.screen*/
    const char *txt = strchr( display_name, ':' );
    
    if( !txt )
    { fprintf( stderr, "invalid display name: %s\n", display_name );
      return -1;
    }

    strncpy( ds, txt, strlen(txt) > 8 ? 8 : strlen(txt) );
    ds[7] = '\000';
    if( strrchr( display_name, '.' ) )
    {
      char *nummer_text = strchr( ds, '.' );
      if( nummer_text )
        scr_nummer = atoi( &nummer_text[1] );
    }
  }

  return scr_nummer;
}


/**  @internal
 *  extract the host name or get from environment
 */
char*
oyExtractHostName_           (const char* display_name)
{
  char* host_name = NULL;

  /* Is this X server identifyable? */
  if(!display_name)
  {
    char *host = getenv ("HOSTNAME");
    if (host) {
      host_name = strdup( host );
    }
  } else if (strchr(display_name,':') == display_name ||
             !strchr( display_name, ':' ) )
  {
    char *host = getenv ("HOSTNAME");
    /* good */
    if (host) {
      host_name = strdup( host );
    }
  } else if ( strchr( display_name, ':' ) )
  {
    char* ptr = 0;
    host_name = malloc(strlen( display_name ) + 48);
    if(!host_name) return NULL;
    strcpy( host_name, display_name );
    ptr = strchr( host_name, ':' );
    ptr[0] = 0;
  } else
    host_name = strdup( "" );

  if(oy_debug) fprintf( stderr, "host_name = %s\n", host_name );

  return host_name;
}

/** @internal Do a full check and change the screen name,
 *  if the screen arg is appropriate. Dont care about the host part
 */
char * oyX1ChangeScreenName_         ( const char        * display_name,
                                       int                 screen )
{
  char* host_name;

  /* Is this X server identifyable? */
  if(!display_name)
    display_name = ":0.0";


  host_name = malloc(strlen( display_name ) + 48);
  if(!host_name) return NULL;

  strcpy( host_name, display_name );

  /* add screen */
  {
    const char *txt = strchr( host_name, ':' );

    /* fail if no display was given */
    if( !txt )
    { fprintf( stderr, "invalid display name: %s\n", display_name );
      host_name[0] = 0;
      return host_name;
    }

    if( !strchr( txt, '.' ) )
    {
      sprintf( &host_name[ strlen(host_name) ], ".%d", screen );
    } else
    if( screen >= 0 )
    {
      char *txt_scr = strchr( txt, '.' );
      sprintf( txt_scr, ".%d", screen );
    }
  }

  if(oy_debug) fprintf( stderr, "host_name = %s\n", host_name );

  return host_name;
}

/** @internal get the geometry of a screen 
 *
 *  @param          disp      display info structure
 *  @return         error
 */
int
oyX1Monitor_getScreenGeometry_            (oyX1Monitor_s *disp)
{
  int error = 0;
  int screen = 0;


  disp->geo[0] = oyGetDisplayNumber_( disp );
  disp->geo[1] = screen = oyX1Monitor_getScreenFromDisplayName_( disp );

  if(screen < 0)
    return screen;

# if defined(HAVE_XRANDR)
  if( oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR )
  {
    XRRCrtcInfo * crtc_info = 0;

    crtc_info = XRRGetCrtcInfo( disp->display, disp->res,
                                disp->output_info->crtc );
    if(crtc_info)
    {
      disp->geo[2] = crtc_info->x;
      disp->geo[3] = crtc_info->y;
      disp->geo[4] = crtc_info->width;
      disp->geo[5] = crtc_info->height;

      XRRFreeCrtcInfo( crtc_info );
    } else
    {
      fprintf( stderr, "%s output: \"%s\" crtc: %d\n",
               "XRandR CrtcInfo request failed", 
               noE(disp->output_info->name),
               (int)disp->output_info->crtc);
    }
  }
# endif /* HAVE_XRANDR */

# if defined(HAVE_XINERAMA)
  if( oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XINERAMA )
  {
    int n_scr_info = 0;

    XineramaScreenInfo *scr_info = XineramaQueryScreens( disp->display,
                                                         &n_scr_info );
    if( !scr_info || !n_scr_info )
    {
      fprintf( stderr,"Xinerama request failed\n");
      return 1;
    }
    if( n_scr_info <= screen )
    {
      fprintf( stderr,"Xinerama request failed\n");
      return -1;
    }
    {
        disp->geo[2] = scr_info[screen].x_org;
        disp->geo[3] = scr_info[screen].y_org;
        disp->geo[4] = scr_info[screen].width;
        disp->geo[5] = scr_info[screen].height;
    }
    XFree( scr_info );
  }
# endif /* HAVE_XINERAMA */

  if( oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_SCREEN )
  {
    Screen *scr = XScreenOfDisplay( disp->display, screen );
    if(scr == NULL)
    {
      fprintf( stderr,"open X Screen failed\n");
      error = 1;

    } else
    {
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
 *  @brief create a monitor information struct for a given display name
 *
 *  @param   display_name              Oyranos display name
 *  @param   expensive                 probe XRandR even if it causes flickering
 *  @return                            a monitor information structure
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/17 (Oyranos: 0.1.8)
 *  @date    2009/01/13
 */
oyX1Monitor_s* oyX1Monitor_newFrom_      ( const char        * display_name,
                                       int                 expensive )
{
  int error = 0, t_err = 0;
  int i = 0;
  oyX1Monitor_s * disp = 0;

  if(display_name && 
     (isdigit(display_name[0]) || strchr(display_name, ':') == NULL))
  {
      return disp;
  }
	  
  disp = malloc( sizeof(oyX1Monitor_s) );
  if(!disp) return disp;

  t_err = !memset( disp, 0, sizeof(oyX1Monitor_s) );
  if(t_err) error = t_err;

  disp->type_ = oyOBJECT_MONITOR_S;

  if( display_name )
  {
    if( display_name[0] )
      disp->name = strdup( display_name );
  } else
  {
    const char * dpyn = getenv("DISPLAY");
    if(dpyn && dpyn[0])
      disp->name = strdup( dpyn );
    else
      disp->name = strdup( ":0" );
  }

  if( error <= 0 &&
      (disp->host = oyExtractHostName_( disp->name )) == 0 )
    error = 1;

  {
  Display *display = 0;
  int len = 0;
  int monitors = 0;

  disp->display = XOpenDisplay (disp->name);

  /* switch to Xinerama mode */
  if( !disp->display ) {
    char *text = oyX1ChangeScreenName_( disp->name, 0 );
    if(!text) goto dispFailed;

    disp->display = XOpenDisplay( text );

    if( !disp->display )
    {
      fprintf( stderr,"%s: %s %s %s\n", "open X Display failed",
                            noE(display_name),
                            noE(disp->name),
                            noE(text));
      free( text );
      goto dispFailed;
    }
    free( text );

    disp->screen = 0;
  }
  display = oyX1Monitor_device_( disp );

  if( !display || (len = ScreenCount( display )) <= 0 )
  {
    fprintf( stderr,"%s: \"%s\"\n", "no Screen found",
                           noE(disp->name));
    goto dispFailed;
  }

  disp->info_source = oyX11INFO_SOURCE_SCREEN;

  if(len == 1)
  {
# if defined(HAVE_XRANDR)
    int major_versionp = 0;
    int minor_versionp = 0;
    int i, n = 0;
    XRRQueryVersion( display, &major_versionp, &minor_versionp );

    if((major_versionp*100 + minor_versionp) >= 102)
    {
      /* too expensive
      Time xrr_config_time0 = 0,
           xrr_config_time = XRRTimes( display, oyX1Monitor_screen_(disp),
                                       &xrr_config_time0 );

      // expensive too: XRRConfigTimes()
      */
    }

    if((major_versionp*100 + minor_versionp) >= 102)
    {
      Window w = RootWindow(display, oyX1Monitor_screen_(disp));
      XRRScreenResources * res = 0;
      int selected_screen = oyX1Monitor_getScreenFromDisplayName_( disp );
      int xrand_screen = -1;
      int geo[4] = {-1,-1,-1,-1};
      int geo_monitors = 0;

      if(selected_screen < 0)
        goto dispFailed;

# if defined(HAVE_XINERAMA)
      /* sync numbering with Xinerama screens */
      if( XineramaIsActive( display ) )
      {
        int n_scr_info = 0;
        XineramaScreenInfo *scr_info = XineramaQueryScreens( display,
                                                             &n_scr_info );
        if(n_scr_info <= selected_screen)
        {
          XFree( scr_info );
          oyX1Monitor_release_( &disp );
          return 0;
        }

        geo[0] = scr_info[selected_screen].x_org;
        geo[1] = scr_info[selected_screen].y_org;
        geo[2] = scr_info[selected_screen].width;
        geo[3] = scr_info[selected_screen].height;
        if(!scr_info || !n_scr_info)
          goto dispFailed;

        XFree( scr_info );

        if(n_scr_info > 0)
          disp->info_source = oyX11INFO_SOURCE_XINERAMA;
      }
# endif /* HAVE_XINERAMA */

      if(disp->info_source != oyX11INFO_SOURCE_XINERAMA)
        expensive = 1;


      if(expensive)
      {
        /* a havily expensive call */
        if(oy_debug) fprintf( stderr,"going to call XRRGetScreenResources()\n");
        res = XRRGetScreenResources(display, w);
        if(oy_debug) fprintf( stderr,"end of call XRRGetScreenResources()\n");
        if(res)
          n = res->noutput;
      }
      for(i = 0; i < n; ++i)
      {
        XRRScreenResources * res_temp = res ? res : disp->res;
        XRROutputInfo * output_info = XRRGetOutputInfo( display, res_temp,
                                                        res_temp->outputs[i]);
 
        /* we work on connected outputs */
        if( output_info && output_info->crtc )
        {
          XRRCrtcInfo * crtc_info = 0;

          if(monitors == 0)
          {
            if(!XRRGetCrtcGammaSize( display, output_info->crtc ))
            {
              XRRFreeOutputInfo( output_info );
              break;
            }
          }

          if(XRRGetCrtcGammaSize( display, output_info->crtc ))
            disp->info_source = oyX11INFO_SOURCE_XRANDR;

          crtc_info = XRRGetCrtcInfo( disp->display, res_temp,
                                      output_info->crtc );
          if(crtc_info)
          {
            /* compare with Xinerama geometry */
            if(!(geo[0] != -1 && geo[1] != -1 && geo[2] != -1 && geo[3] != -1)||
               (geo[0] == crtc_info->x &&
                geo[1] == crtc_info->y &&
                geo[2] == (int)crtc_info->width &&
                geo[3] == (int)crtc_info->height ) )
            {
              xrand_screen = monitors;
              ++geo_monitors;
            }

            XRRFreeCrtcInfo( crtc_info );
          }

          if(xrand_screen == monitors &&
             oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR &&
             (geo_monitors == 1 || geo_monitors-1 == selected_screen))
          {
            if(output_info)
              disp->output_info = output_info;
            disp->output = res_temp->outputs[i];
            output_info = 0;
            if(res) /* only needed for a second geo matching monitor */
              disp->res = res;
            res = 0;
            if(disp->output_info->name && strlen(disp->output_info->name))
              disp->system_port = strdup( disp->output_info->name );
            disp->rr_version = major_versionp*100 + minor_versionp;
            disp->rr_screen = xrand_screen;
            disp->mm_width = disp->output_info->mm_width;
            disp->mm_height = disp->output_info->mm_height;
          }

          ++ monitors;
        }

        if(output_info)
          XRRFreeOutputInfo( output_info );
      }

      if(res)
      { XRRFreeScreenResources(res); res = 0; }

      if(oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_XRANDR)
      {
        if(monitors >= len && disp->output_info)
          disp->active_outputs = monitors;
        else
          disp->info_source = oyX11INFO_SOURCE_SCREEN;
      }
    }
# endif /* HAVE_XRANDR */

    if(oyX1Monitor_infoSource_( disp ) == oyX11INFO_SOURCE_SCREEN)
    {
# if defined(HAVE_XINERAMA)
      /* test for Xinerama screens */
      if( XineramaIsActive( display ) )
      {
        int n_scr_info = 0;
        XineramaScreenInfo *scr_info = XineramaQueryScreens( display,
                                                             &n_scr_info );
        if(!scr_info || !n_scr_info)
          goto dispFailed;

        if( n_scr_info >= 1 )
          len = n_scr_info;

        if(n_scr_info < monitors)
        {
          fprintf( stderr, "%s: %d < %d\n", "less Xinerama monitors than XRandR ones",
                    n_scr_info, monitors );
        } else
          disp->info_source = oyX11INFO_SOURCE_XINERAMA;

        XFree( scr_info );
      }
# endif /* HAVE_XINERAMA */
    }
  }
  }

  for( i = 0; i < 6; ++i )
    disp->geo[i] = -1;

  if( error <= 0 &&
      (t_err = oyX1Monitor_getScreenGeometry_( disp )) != 0 )
    error = t_err;

  if( error <= 0 )
  {
    t_err = oyX1Monitor_getGeometryIdentifier_( disp );
    if(t_err) error = t_err;
  }

  if( !disp->system_port || !strlen( disp->system_port ) )
  if( 0 <= oyX1Monitor_screen_( disp ) && oyX1Monitor_screen_( disp ) < 10000 )
  {
    disp->system_port = (char*)malloc( 12 );
    sprintf( disp->system_port, "%d", oyX1Monitor_screen_( disp ) );
  }

  if(error > 0)
  {
    dispFailed:
    fprintf( stderr,"%s: %s %d\n", "no oyX1Monitor_s created", display_name, error);
    oyX1Monitor_release_( &disp );
  }

  return disp;
}


/** @internal
 *  @brief release a monitor information stuct
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/17 (Oyranos: 0.1.8)
 *  @date    2009/04/01
 */
int          oyX1Monitor_release_      ( oyX1Monitor_s      ** obj )
{
  int error = 0;
  /* ---- start of common object destructor ----- */
  oyX1Monitor_s * s = 0;
  
  if(!obj || !*obj)
    return 0;
  
  s = *obj;
  
  if( s->type_ != oyOBJECT_MONITOR_S)
  { 
    fprintf( stderr,"Attempt to release a non oyX1Monitor_s object.\n");
    return 1;
  }
  /* ---- end of common object destructor ------- */

  if(s->name) free( s->name );
  if(s->host) free( s->host );
  if(s->identifier) free( s->identifier );


  s->geo[0] = s->geo[1] = -1;

  if( s->display )
  {
#  if defined(HAVE_XRANDR)
    if(s->output_info)
    { XRRFreeOutputInfo( s->output_info ); s->output_info = 0; }
    if(s->res)
    { XRRFreeScreenResources( s->res ); s->res = 0; }
#  endif
    XCloseDisplay( s->display );
    s->display=0;
  }

  free( s );
  s = 0;

  *obj = 0;

  return error; 
}


/* separate from the internal functions */


#define oyX1_help_system_specific \
      " One option \"device_name\" will select the according X display.\n" \
      " If not the module will try to get this information from \n" \
      " your \"DISPLAY\" environment variable or uses what the system\n" \
      " provides. The \"device_name\" should be identical with the one\n" \
      " returned from a \"list\" request.\n" \
      " The \"properties\" call might be a expensive one.\n" \
      " Informations are stored in the returned oyConfig_s::backend_core member."
#include "config.icc_profile.monitor.oyX1.qarz.json.h"
oyMonitorHooks_s oyX1MonitorHooks_ = {
  oyOBJECT_MONITOR_HOOKS_S,
  {"oyX1"},
  10000, /* 1.0.0 */
  oyX1_help_system_specific,
  (const char*)config_icc_profile_monitor_oyX1_qarz_json,
  oyX1MonitorProfileSetup,
  oyX1MonitorProfileUnset,
  oyX1Rectangle_FromDevice,
  oyX1GetMonitorProfile,
  oyX1GetAllScreenNames,
  oyX1GetMonitorInfo
};

oyMonitorHooks_s * oyX1MonitorHooks = &oyX1MonitorHooks_;

