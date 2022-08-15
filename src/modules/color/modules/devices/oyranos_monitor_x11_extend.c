/** @file oyranos_monitor_x11_extend.c
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

/* handler: color_server_active */
/* handler: clean_profiles */
/* handler: move_color_server_profiles */
/* handler: send_native_update_event */
/* handler: set_xcm_region */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>
#include <unistd.h>  /* intptr_t */
#include <locale.h>
#include <errno.h>

#include "oyranos_config_internal.h"

# include <X11/Xlib.h>
# include <X11/Xutil.h>
# include <X11/Xatom.h>
# include <X11/Xcm/XcmEdidParse.h>
# include <X11/Xcm/XcmEvents.h>
# include <X11/Xcm/Xcm.h>
# if defined(HAVE_XINERAMA)
#  include <X11/extensions/Xinerama.h>
# endif
# ifdef HAVE_XXF86VM
#  include <X11/extensions/xf86vmode.h>
# endif
# ifdef HAVE_XRANDR
#  include <X11/extensions/Xrandr.h>
# endif
#include <X11/extensions/Xfixes.h>

#include "oyranos.h"
#include "oyranos_devices.h"
#include "oyranos_cmm.h"
#include "oyranos_internal.h"
#include "oyranos_io.h"
#include "oyranos_monitor.h"
#include "oyranos_monitor_effect.h"
#include "oyranos_monitor_internal_x11.h"
#include "oyranos_monitor_internal.h"
#include "oyranos_monitor_hooks_x11.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyCMMapi10_s_.h"

/* ---  Helpers  --- */

/* --- internal API definition --- */

extern oyMessage_f oyX1_msg;

int      oyX1ColorServerActive       ( int                 flags )
{
  static int active = 0;
#if defined(XCM_HAVE_X11)
  static double z = 0;
  if( z + 1.0 < oySeconds() ||
      flags & oySOURCE_DATA )
  {
    Display * display = XOpenDisplay(NULL);
    if(!display) return active;

    active = XcmColorServerCapabilities( display );
    z = oySeconds();
    DBG_NUM2_S("color server active: %d %g\n", active, z);
    XCloseDisplay(display);
  }
#endif
  return active;
}
/**
 *  This function implements oyMOptions_Handle_f.
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/02/22
 *  @since   2018/02/22 (Oyranos: 0.9.7)
 */
int          oyX1ColorServer_Handle  ( oyOptions_s       * options OY_UNUSED,
                                       const char        * command,
                                       oyOptions_s      ** result )
{
  if(oyFilterRegistrationMatch(command,"can_handle", 0))
  {
    if(oyFilterRegistrationMatch(command,"color_server_active", 0))
    {
    }
    else
      return 1;
  }
  else if(oyFilterRegistrationMatch(command,"color_server_active", 0))
  {
    int flags = 0;
    int active;
    oyOptions_FindInt( options, "flags", 0, &flags );
    if(oyFilterRegistrationMatch(command,"source_data", 0))
      flags |= oySOURCE_DATA;
    active = oyX1ColorServerActive( flags );

    oyOptions_SetFromString( result,
              "//"OY_TYPE_STD"/config/color_server_active",
              active?"1":"0", OY_CREATE_NEW );
    fprintf(stderr, "%s ", oyjlPrintTime(OYJL_BRACKETS, oyjlGREEN) );
    _msg( oyMSG_WARN, (oyStruct_s*)options,
          OY_DBG_FORMAT_ "color_server_active%s: %d", OY_DBG_ARGS_,
          flags & oySOURCE_DATA?".source_data":"", active );
  }

  return 0;
}

/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/02/22
 *  @since   2018/02/22 (Oyranos: 0.9.7)
 */
const char * oyX1InfoGetTextMyHandlerA(const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
         if(strcmp(select, "can_handle")==0)
  {
         if(type == oyNAME_NICK)
      return "check";
    else if(type == oyNAME_NAME)
      return _("check");
    else
      return _("Check if this module can handle a certain command.");
  } else if(strcmp(select, "color_server_active")==0)
  {
         if(type == oyNAME_NICK)
      return "color_server_active";
    else if(type == oyNAME_NAME)
      return _("Get X Color Management activity state.");
    else
      return _("Ask about _ICC_COLOR_DESKTOP activity.");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return _("help");
    else if(type == oyNAME_NAME)
      return _("Help");
    else
      return _("The oyX1 modules \"color_server_active\" handler lets you ask "
               "for the X Color Management _ICC_COLOR_DESKTOP X11 atom. "
               "Use \"color_server_active.source_data\" to avoid the default caching. "
               "The implementation uses Xlib.");
  }
  return 0;
}
const char *oyX1_texts_color_server_active[4] = {"can_handle","color_server_active","help",0};

/** @brief    oyX1 oyCMMapi10_s implementation
 *
 *  X Color Management desktop advanced toogle
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/02/22
 *  @since   2018/02/22 (Oyranos: 0.9.7)
 */
oyCMMapi10_s_    oyX1_api10_color_server_active_handler = {

  oyOBJECT_CMM_API10_S,
  0,0,0,
  NULL,

  CMMapiInit,
  CMMapiReset,
  CMMMessageFuncSet,

  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
  "color_server_active._source_data._" CMM_NICK,

  {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C},/**< version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,                         /**< oyPointer_s * runtime_context */
 
  oyX1InfoGetTextMyHandlerA,             /**< getText */
  (char**)oyX1_texts_color_server_active, /**<texts; list of arguments to getText*/
 
  oyX1ColorServer_Handle               /**< oyMOptions_Handle_f oyMOptions_Handle */
};


#ifdef HAVE_XRANDR
void oyCleanDisplayXRR                 ( Display           * display )
{
  int n = 0, i;
  Window root = RootWindow( display, DefaultScreen( display ) );
  XRRScreenResources * res = XRRGetScreenResources(display, root);
  Atom atom = XInternAtom( display, "_ICC_PROFILE", True );
  Atom atom2 = XInternAtom( display, "_ICC_DEVICE_PROFILE", True );

  if(res)
    n = res->noutput;
  if(atom)
  for(i = 0; i < n; ++i)
  {
    XRROutputInfo * output = XRRGetOutputInfo( display, res, res->outputs[i] );
    if(!output) continue;

    XRRChangeOutputProperty( display, res->outputs[i],
                             atom, XA_CARDINAL, 8, PropModeReplace, NULL, 0 );
    XRRDeleteOutputProperty( display, res->outputs[i], atom );

    XRRChangeOutputProperty( display, res->outputs[i],
                             atom2, XA_CARDINAL, 8, PropModeReplace, NULL, 0 );
    XRRDeleteOutputProperty( display, res->outputs[i], atom2 );
    XRRFreeOutputInfo( output );
  }

  if(res)
  { XRRFreeScreenResources(res); res = 0; }
}
#endif

void oyCleanProfiles                 ( Display           * display )
{
  char * atom_name;
  int i;
  Atom atom;
  Window root;

  if(!display)
    return;

  root = RootWindow( display, DefaultScreen( display ) );

#ifdef HAVE_XRANDR
  oyCleanDisplayXRR( display );
#endif

  /* clean up to 20 displays */

  atom_name = malloc( 1024 );

  for(i = 0; i < 20; ++i)
  {
    sprintf( atom_name, "_ICC_PROFILE" );
    if(i)
      sprintf( &atom_name[strlen(atom_name)], "_%d", i );
    atom = XInternAtom (display, atom_name, True);
    if (atom != None)
      XDeleteProperty( display, root, atom );
  }

  free(atom_name); atom_name = 0;
}

/**
 *  This function implements oyMOptions_Handle_f.
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/12/06 (Oyranos: 0.9.6)
 *  @date    2016/12/06
 */
int          oyX1CleanOptions_Handle ( oyOptions_s       * options,
                                       const char        * command,
                                       oyOptions_s      ** result OY_UNUSED )
{
  int error = 0;
  if(oyFilterRegistrationMatch(command,"can_handle", 0))
  {
    if(oyFilterRegistrationMatch(command,"clean_profiles", 0))
    {
    }
    else
      return 1;
  }
  else if(oyFilterRegistrationMatch(command,"clean_profiles", 0))
  {
    const char * display_name = oyOptions_FindString( options, "display_name", 0 );
    Display * display = XOpenDisplay( display_name );
    oyMSG_e mtype = !display ? oyMSG_ERROR:oyMSG_DBG;

    error = !display;
    fprintf(stderr, "%s clean_profiles\n", oyjlPrintTime(OYJL_BRACKETS, oyjlGREEN) );
    _msg( mtype, (oyStruct_s*)options,
          OY_DBG_FORMAT_ "clean_profiles: display_name: %s", OY_DBG_ARGS_,
          display_name?display_name:"----" );
    if(!error)
    {
      oyCleanProfiles( display );
      XCloseDisplay( display );
    }
  }

  return error;
}

/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/12/06 (Oyranos: 0.9.6)
 *  @date    2016/12/06
 */
const char * oyX1InfoGetTextMyHandlerC(const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
         if(strcmp(select, "can_handle")==0)
  {
         if(type == oyNAME_NICK)
      return "check";
    else if(type == oyNAME_NAME)
      return _("check");
    else
      return _("Check if this module can handle a certain command.");
  } else if(strcmp(select, "clean_profiles")==0)
  {
         if(type == oyNAME_NICK)
      return "clean_profiles";
    else if(type == oyNAME_NAME)
      return _("Remove all X Color Management profiles.");
    else
      return _("Remove naive and XCM aware CM apps profiles.");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return _("help");
    else if(type == oyNAME_NAME)
      return _("Help");
    else
      return _("The oyX1 modules \"clean_profiles\" handler removes "
               "X Color Management device profile and screen document profile properties. "
               "The handler expects a \"display_name\" option with a string containing "
               "the X11 display name. "
               "The implementation uses Xlib and the XRandR extension.");
  }
  return 0;
}
const char *oyX1_texts_clean_profiles[4] = {"can_handle","clean_profiles","help",0};

/** @brief    oyX1 oyCMMapi10_s implementation
 *
 *  X Color Management desktop device profile handler
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/12/06 (Oyranos: 0.9.6)
 *  @date    2016/12/06
 */
oyCMMapi10_s_    oyX1_api10_clean_profiles_handler = {

  oyOBJECT_CMM_API10_S,
  0,0,0,
  (oyCMMapi_s*) &oyX1_api10_color_server_active_handler,

  CMMapiInit,
  CMMapiReset,
  CMMMessageFuncSet,

  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
  "clean_profiles._" CMM_NICK,

  {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C},/**< version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,                         /**< oyPointer_s * runtime_context */
 
  oyX1InfoGetTextMyHandlerC,             /**< getText */
  (char**)oyX1_texts_clean_profiles, /**<texts; list of arguments to getText*/
 
  oyX1CleanOptions_Handle               /**< oyMOptions_Handle_f oyMOptions_Handle */
};

int  oyMoveColorServerProfiles       ( const char        * display_name,
                                       int                 screen,
                                       int                 setup )
{
  char * screen_name = oyX1ChangeScreenName_( display_name, screen );
  oyMonitor_s * disp = oyX1Monitor_newFrom_( screen_name, 1 );
  char * dev_prof;
  size_t dev_prof_size = 0;
  /* select profiles matching actual capabilities */
  int icc_profile_flags = oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD, "//" OY_TYPE_STD "/icc_color", NULL, 0 );
  oyConfigs_s * devices = NULL;
  oyConfig_s * monitor = NULL;
  oyOptions_s * options = NULL;
  oyProfile_s * monitor_icc = NULL;
  const char * monitor_icc_dscr = NULL;
  int active = oyX1ColorServerActive( oySOURCE_DATA );
  const char * filename = NULL;
  char * fn = NULL;

  if(!disp)
  {
    oyFree_m_(screen_name);
    return -1;
  }

  // get all monitors
  oyDevicesGet( NULL, "monitor", NULL, &devices );
  monitor = oyConfigs_Get( devices, screen );
  oyConfigs_Release( &devices );

  /* detect OpenICC DB profile */
  oyDeviceProfileFromDB( monitor, &fn, 0 );
  if(fn && fn[0])
  {
    monitor_icc = oyProfile_FromName( fn, 0, 0 );
    _msg( oyMSG_DBG, (oyStruct_s*)options,
        OY_DBG_FORMAT_ "monitor[%d] DB profile \"%s\" %s", OY_DBG_ARGS_,
        screen, fn ? fn :"not found", setup?"setup":"unset");
  }
  /* detect the possibly new set ICC profile in X
   * get XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE */
  if(!monitor_icc)
  {
    oyX11INFO_SOURCE_e source;
    char * atom_name = NULL;
    dev_prof = oyX1Monitor_getProperty_( disp,
                                         XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE,
                                         0, &dev_prof_size,
                                         &source, &atom_name );
    monitor_icc = oyProfile_FromMem( dev_prof_size, dev_prof, 0,0 );
    _msg( oyMSG_DBG, (oyStruct_s*)options,
        OY_DBG_FORMAT_ "monitor[%d] %s profile %d %s %s %s", OY_DBG_ARGS_,
        screen, XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE, dev_prof_size, atom_name?atom_name:"not reachable", setup?"setup":"unset");
    if(dev_prof) { free(dev_prof); dev_prof = NULL; }
    if(atom_name) { free( atom_name ); atom_name = NULL; }
  }
  if(!monitor_icc)
  {
    oyOptions_SetFromString( &options,
              "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target", "yes", OY_CREATE_NEW );
    oyDeviceGetProfile( monitor, options, &monitor_icc );
  }
  filename = oyProfile_GetFileName( monitor_icc, -1 );
  dev_prof = oyProfile_GetMem( monitor_icc, &dev_prof_size, 0,0 );
  // get the profiles internal name
  monitor_icc_dscr = oyProfile_GetText( monitor_icc, oyNAME_DESCRIPTION );
  _msg( oyMSG_DBG, (oyStruct_s*)options,
        OY_DBG_FORMAT_ "monitor[%d] has profile: \"%s\" %s %s", OY_DBG_ARGS_,
        screen, monitor_icc_dscr, active ? "color_server_active":"no color server", setup?"setup":"unset");
  oyOptions_Release( &options );

  oyConfig_Release( &monitor );

  fprintf(stderr, "%s oyMoveColorServerProfiles() %s %s ", oyjlPrintTime(OYJL_BRACKETS, oyjlGREEN), active ? "color_server_active":"no color server", setup?"setup":"unset" );
  if(setup)
  {
    size_t size = 0;
    oyProfile_s * screen_document_profile = oyProfile_FromStd( oyASSUMED_WEB,
                                                        icc_profile_flags, 0 );

    char * docp = oyProfile_GetMem( screen_document_profile, &size, 0, oyAllocateFunc_ );
    oyProfile_Release( &screen_document_profile );

    oyX1Monitor_setProperty_( disp, XCM_ICC_COLOUR_SERVER_TARGET_PROFILE_IN_X_BASE, dev_prof, dev_prof_size );
    oyX1Monitor_setProperty_( disp, XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE, docp, size );
    oyFree_m_( docp );

    SetupMonitorCalibration( disp, NULL, NULL, 0 );
    oyX1Monitor_setCompatibility( disp, NULL );
  }
  else
  {
    oyX1Monitor_setProperty_( disp, XCM_ICC_COLOUR_SERVER_TARGET_PROFILE_IN_X_BASE, NULL, 0 );
    oyX1Monitor_setProperty_( disp, XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE, dev_prof, dev_prof_size );
    oyDevicesGet( NULL, "monitor", NULL, &devices );
    monitor = oyConfigs_Get( devices, screen );
    oyConfigs_Release( &devices );
    oyDeviceSetup2( monitor, NULL );
    if(filename)
      oyX1Monitor_setCompatibility( disp, filename );
  }

  oyConfig_Release( &monitor );
  oyProfile_Release( &monitor_icc );
  oyX1Monitor_release_( &disp );
  if(screen_name) free( screen_name );
  return 1;
}

/**
 *  This function implements oyMOptions_Handle_f.
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/11/28 (Oyranos: 0.9.6)
 *  @date    2016/11/28
 */
int          oyX1MoveOptions_Handle  ( oyOptions_s       * options,
                                       const char        * command,
                                       oyOptions_s      ** result OY_UNUSED )
{
  if(oyFilterRegistrationMatch(command,"can_handle", 0))
  {
    if(oyFilterRegistrationMatch(command,"move_color_server_profiles", 0))
    {
    }
    else
      return 1;
  }
  else if(oyFilterRegistrationMatch(command,"move_color_server_profiles", 0))
  {
    const char * display_name = oyOptions_FindString( options, "display_name", 0 );
    int screen = 0;
    int setup = 0;
    char * t;
    oyOptions_FindInt( options, "screen", 0, &screen );
    oyOptions_FindInt( options, "setup", 0, &setup );
    _msg( oyMSG_DBG, (oyStruct_s*)options,
          OY_DBG_FORMAT_ "move_color_server_profiles: display_name: %s screen: %d setup: %d", OY_DBG_ARGS_,
          display_name, screen, setup );
    fprintf(stderr, "%s move_color_server_profiles setup: %d ", oyjlPrintTime(OYJL_BRACKETS, oyjlGREEN), setup );
    t = oyjlBT(0);
    fprintf( stderr, "%s", t );
    free(t);
    if(oy_debug)
      fprintf(stderr, "display_name: %s screen: %d setup: %d\n", display_name, screen, setup );
    oyMoveColorServerProfiles( display_name, screen, setup );
  }

  return 0;
}

/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/11/28 (Oyranos: 0.9.6)
 *  @date    2016/11/28
 */
const char * oyX1InfoGetTextMyHandlerM(const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
         if(strcmp(select, "can_handle")==0)
  {
         if(type == oyNAME_NICK)
      return "check";
    else if(type == oyNAME_NAME)
      return _("check");
    else
      return _("Check if this module can handle a certain command.");
  } else if(strcmp(select, "move_color_server_profiles")==0)
  {
         if(type == oyNAME_NICK)
      return "move_color_server_profiles";
    else if(type == oyNAME_NAME)
      return _("Set all X Color Management device profiles.");
    else
      return _("Handle naive and XCM aware CM apps device profiles.");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return _("help");
    else if(type == oyNAME_NAME)
      return _("Help");
    else
      return _("The oyX1 modules \"move_color_server_profiles\" handler sets up "
               "X Color Management device profile and screen document profile properties. "
               "The handler should only be called by desktop XCM compatible color servers. "
               "The handler expects a \"display_name\" option with a string containing "
               "the X11 display name, a \"screen\" option containing the selected screen "
               "as integer (Xlib/Xinerama/XRandR) and a \"setup\" integer option telling "
               "with 0 no/revert setup and with 1 setup profiles. "
               "The implementation uses Xlib.");
  }
  return 0;
}
const char *oyX1_texts_move_color_server_profiles[4] = {"can_handle","move_color_server_profiles","help",0};

/** @brief    oyX1 oyCMMapi10_s implementation
 *
 *  X Color Management desktop device profile handler
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/11/28 (Oyranos: 0.9.6)
 *  @date    2016/11/28
 */
oyCMMapi10_s_    oyX1_api10_move_color_server_profiles_handler = {

  oyOBJECT_CMM_API10_S,
  0,0,0,
  (oyCMMapi_s*) &oyX1_api10_clean_profiles_handler,

  CMMapiInit,
  CMMapiReset,
  CMMMessageFuncSet,

  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
  "move_color_server_profiles._" CMM_NICK,

  {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C},/**< version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,                         /**< oyPointer_s * runtime_context */
 
  oyX1InfoGetTextMyHandlerM,             /**< getText */
  (char**)oyX1_texts_move_color_server_profiles, /**<texts; list of arguments to getText*/
 
  oyX1MoveOptions_Handle               /**< oyMOptions_Handle_f oyMOptions_Handle */
};


#if defined(XCM_HAVE_X11) && defined(HAVE_XCM)
int XcolorRegionFind(XcolorRegion * old_regions, unsigned long old_regions_n, Display * dpy, Window win, XRectangle * rectangle)
{   
  XRectangle * rect = 0;
  int nRect = 0;
  int pos = -1;
  unsigned long i;
  int j;
    
  /* get old regions */ 
  old_regions = XcolorRegionFetch( dpy, win, &old_regions_n );
  /* search region */
  for(i = 0; i < old_regions_n; ++i) 
  {     
                 
    if(!old_regions[i].region || pos >= 0)
      break;                    

    rect = XFixesFetchRegion( dpy, ntohl(old_regions[i].region),
                              &nRect );

    for(j = 0; j < nRect; ++j)
    {
      if(oy_debug) 
        printf( "reg[%lu]: %dx%d+%d+%d %dx%d+%d+%d\n",
                   i,
                   rectangle->width, rectangle->height,
                   rectangle->x, rectangle->y,
                   rect[j].width, rect[j].height, rect[j].x, rect[j].y
                  );
      if(rectangle->x == rect[j].x &&
         rectangle->y == rect[j].y &&
         rectangle->width == rect[j].width &&
         rectangle->height == rect[j].height )
      {
        pos = i;
        break;
      }
    }
  }

  return pos;
}
#endif

/**
 *  This function implements oyMOptions_Handle_f.
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/03/10 (Oyranos: 0.9.6)
 *  @date    2016/03/10
 */
int          oyX1UpdateOptions_Handle( oyOptions_s       * options,
                                       const char        * command,
                                       oyOptions_s      ** result OY_UNUSED )
{
  if(oyFilterRegistrationMatch(command,"can_handle", 0))
  {
    if(oyFilterRegistrationMatch(command,"send_native_update_event", 0))
    {
    }
    else
      return 1;
  }
  else if(oyFilterRegistrationMatch(command,"send_native_update_event", 0))
  {
    // ping X11 observers about option change
    // ... by setting a known property again to its old value
    Display * display = XOpenDisplay(NULL);
    Atom atom;
    Window root;

    if(!display)
    {
       _msg( oyMSG_ERROR, (oyStruct_s*)options,
          OY_DBG_FORMAT_ "can not open Display for %s\n", OY_DBG_ARGS_,
          command );
      return 1;
    }

    atom = XInternAtom(display, XCM_COLOUR_DESKTOP_ADVANCED, False); // "_ICC_COLOR_DISPLAY_ADVANCED"
    root = RootWindow( display, 0 );
  
    XFlush( display );
  
    Atom actual;
    int format;
    int advanced = -1;
    unsigned long left;
    unsigned long size;
    unsigned char *data;
    int result = XGetWindowProperty( display, root, atom, 0, ~0, 0, XA_STRING, &actual,
                                     &format, &size, &left, &data );
    if(data && size && atoi((const char*)data) > 0)
      advanced = atoi((const char*)data);
    fprintf(stderr, "%s send_native_update_event\n", oyjlPrintTime(OYJL_BRACKETS, oyjlGREEN) );
    _msg( oyMSG_DBG, (oyStruct_s*)options,
          OY_DBG_FORMAT_ "desktop uses advanced settings: %d\n", OY_DBG_ARGS_,
          advanced );
    XChangeProperty( display, root,
                       atom, XA_STRING, 8, PropModeReplace,
                       data, size );
    if(result == Success && data)
    { XFree( data ); data = 0; }
    if(display)
      XCloseDisplay(display);
  }

  return 0;
}

/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/03/10 (Oyranos: 0.9.6)
 *  @date    2016/03/10
 */
const char * oyX1InfoGetTextMyHandlerU(const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
         if(strcmp(select, "can_handle")==0)
  {
         if(type == oyNAME_NICK)
      return "check";
    else if(type == oyNAME_NAME)
      return _("check");
    else
      return _("Check if this module can handle a certain command.");
  } else if(strcmp(select, "send_native_update_event")==0)
  {
         if(type == oyNAME_NICK)
      return "send_native_update_event";
    else if(type == oyNAME_NAME)
      return _("Set a X Color Management update toggle.");
    else
      return _("Ping the XCM_COLOUR_DESKTOP_ADVANCED X11 atom.");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return _("help");
    else if(type == oyNAME_NAME)
      return _("Help");
    else
      return _("The oyX1 modules \"send_native_update_event\" handler lets you ping "
               "X Color Management advanced X11 atom. "
               "The implementation uses Xlib.");
  }
  return 0;
}
const char *oyX1_texts_send_native_update_event[4] = {"can_handle","send_native_update_event","help",0};

/** @brief    oyX1 oyCMMapi10_s implementation
 *
 *  X Color Management desktop advanced toogle
 *
 *  @version Oyranos: 0.9.6
 *  @since   2016/03/10 (Oyranos: 0.9.6)
 *  @date    2016/03/10
 */
oyCMMapi10_s_    oyX1_api10_send_native_update_event_handler = {

  oyOBJECT_CMM_API10_S,
  0,0,0,
  (oyCMMapi_s*) & oyX1_api10_move_color_server_profiles_handler,

  CMMapiInit,
  CMMapiReset,
  CMMMessageFuncSet,

  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
  "send_native_update_event._" CMM_NICK,

  {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C},/**< version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,                         /**< oyPointer_s * runtime_context */
 
  oyX1InfoGetTextMyHandlerU,             /**< getText */
  (char**)oyX1_texts_send_native_update_event, /**<texts; list of arguments to getText*/
 
  oyX1UpdateOptions_Handle               /**< oyMOptions_Handle_f oyMOptions_Handle */
};


/**
 *  This function implements oyMOptions_Handle_f.
 *
 *  @version Oyranos: 0.4.0
 *  @since   2012/01/11 (Oyranos: 0.4.0)
 *  @date    2012/01/11
 */
int          oyX1MOptions_Handle     ( oyOptions_s       * options,
                                       const char        * command,
                                       oyOptions_s      ** result OY_UNUSED )
{
  oyOption_s * o = 0;
  int error = 0;

  if(oyFilterRegistrationMatch(command,"can_handle", 0))
  {
    if(oyFilterRegistrationMatch(command,"set_xcm_region", 0))
    {
      o = oyOptions_Find( options, "window_rectangle", oyNAME_PATTERN );
      if(!o)
      {
        _msg( oyMSG_WARN, (oyStruct_s*)options,
                 "no option window_rectangle found");
        error = 1;
      }
      oyOption_Release( &o );
      o = oyOptions_Find( options, "window_id", oyNAME_PATTERN );
      if(!o)
      {
        _msg( oyMSG_WARN, (oyStruct_s*)options,
                 "no option window_id found");
        error = 1;
      }
      oyOption_Release( &o );
      o = oyOptions_Find( options, "display_id", oyNAME_PATTERN );
      if(!o)
      {
        _msg( oyMSG_WARN, (oyStruct_s*)options,
                 "no option display_id found");
        error = 1;
      }
      oyOption_Release( &o );

      return error;
    }
    else
      return 1;
  }
  else if(oyFilterRegistrationMatch(command,"set_xcm_region", 0))
  {
#if defined(XCM_HAVE_X11) && defined(HAVE_XCM)
    oyProfile_s * p = NULL;
    oyRectangle_s * win_rect = NULL;
    oyRectangle_s * old_rect = NULL;

    Display * dpy = NULL;
    Window win = 0;
    char * blob = 0;
    size_t size = 0;
    XcolorProfile * profile = 0;
    XserverRegion reg = 0;
    XcolorRegion region;
    int error;
    XRectangle rec[2] = { { 0,0,0,0 }, { 0,0,0,0 } };
    double rect[4];

    oyBlob_s * win_id, * display_id;

    win_id = (oyBlob_s*) oyOptions_GetType( options, -1, "window_id",
                                          oyOBJECT_BLOB_S );
    display_id = (oyBlob_s*) oyOptions_GetType( options, -1, "display_id",
                                          oyOBJECT_BLOB_S );
    win = (Window) oyBlob_GetPointer(win_id);
    dpy = (Display *) oyBlob_GetPointer(display_id);

    oyBlob_Release( &win_id );
    oyBlob_Release( &display_id );

    /* now handle the options */
    win_rect = (oyRectangle_s*) oyOptions_GetType( options, -1, "window_rectangle",
                                          oyOBJECT_RECTANGLE_S );
    old_rect = (oyRectangle_s*) oyOptions_GetType( options, -1,
                                 "old_window_rectangle", oyOBJECT_RECTANGLE_S );
    o = oyOptions_Find( options, "icc_profile", oyNAME_PATTERN );
    p = (oyProfile_s*) oyOptions_GetType( options, -1, "icc_profile",
                                          oyOBJECT_PROFILE_S );
    if(!win || !dpy)
      _msg( oyMSG_WARN, (oyStruct_s*)options,
                "options display_id or window_id not found");
    if(!win_rect)
      _msg( oyMSG_WARN, (oyStruct_s*)options,
                "option window_rectangle not found");


    if(old_rect)
    {
      XcolorRegion *old_regions = 0;
      unsigned long old_regions_n = 0;
      int pos = -1;

      oyRectangle_GetGeo( old_rect, &rect[0], &rect[1], &rect[2], &rect[3] );

      rec[0].x = rect[0];
      rec[0].y = rect[1];
      rec[0].width = rect[2];
      rec[0].height = rect[3];

      /* get old regions */
      old_regions = XcolorRegionFetch( dpy, win, &old_regions_n );
      /* remove specified region */
      pos = XcolorRegionFind( old_regions, old_regions_n, dpy, win, rec );
      XFree( old_regions );
      if(pos >= 0)
      {
        int undeleted_n = old_regions_n;
        XcolorRegionDelete( dpy, win, pos, 1 );
        old_regions = XcolorRegionFetch( dpy, win, &old_regions_n );
        if(undeleted_n - old_regions_n != 1)
          _msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_ "removed %d; have still %d",
             OY_DBG_ARGS_, pos, (int)old_regions_n );
      } else
        _msg( oyMSG_WARN, (oyStruct_s*)options, OY_DBG_FORMAT_
                  "region not found in %lu\n",OY_DBG_ARGS_, old_regions_n );

      XFlush( dpy );

    }

    oyRectangle_GetGeo( win_rect, &rect[0], &rect[1], &rect[2], &rect[3] );

    rec[0].x = rect[0];
    rec[0].y = rect[1];
    rec[0].width = rect[2];
    rec[0].height = rect[3];

    if(p)
    {
        blob = (char*)oyProfile_GetMem( p, &size, 0,0 );

        if(blob && size)
        {
          int result;
        /* Create a XcolorProfile object that will be uploaded to the display.*/
          profile = (XcolorProfile*)malloc(sizeof(XcolorProfile) + size);

          oyProfile_GetMD5(p, 0, (uint32_t*)profile->md5);

          profile->length = htonl(size);
          memcpy(profile + 1, blob, size);

          result = XcolorProfileUpload( dpy, profile );
          if(result)
            _msg( oyMSG_WARN, (oyStruct_s*)options,
                "XcolorProfileUpload: %d\n", result);

          XFlush( dpy );
        }
    }

    if( rect[0] || rect[1] || rect[2] || rect[3] )
    {
      reg = XFixesCreateRegion( dpy, rec, 1);

      region.region = htonl(reg);
      if(blob && size)
        memcpy(region.md5, profile->md5, 16);
      else
        memset( region.md5, 0, 16 );

      /* upload the new or changed region to the X server */
      error = XcolorRegionInsert( dpy, win, 0, &region, 1 );
      if(error)
          _msg( oyMSG_WARN, (oyStruct_s*)options,
                    "XcolorRegionInsert failed %d\n", error );
      XFlush( dpy );
    }
#endif
  }

  return 0;
}

/**
 *  This function implements oyCMMinfoGetText_f.
 *
 *  @version Oyranos: 0.4.0
 *  @since   2012/01/11 (Oyranos: 0.4.0)
 *  @date    2012/01/11
 */
const char * oyX1InfoGetTextMyHandler( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context OY_UNUSED )
{
         if(strcmp(select, "can_handle")==0)
  {
         if(type == oyNAME_NICK)
      return "check";
    else if(type == oyNAME_NAME)
      return _("check");
    else
      return _("Check if this module can handle a certain command.");
  } else if(strcmp(select, "set_xcm_region")==0)
  {
         if(type == oyNAME_NICK)
      return "set_xcm_region";
    else if(type == oyNAME_NAME)
      return _("Set a X Color Management region.");
    else
      return _("The set_xcm_region takes minimal three options. The key name "
               "\"window_rectangle\" specifies in a oyRectangle_s object the "
               "requested window region in coordinates relative to the window. "
               "If its parameters are all set to zero, then the rectangle is "
               "ignored. The \"old_window_rectangle\" is similiar to the "
               "\"window_rectangle\" "
               "option but optionally specifies to remove a old rectangle. "
               "The \"window_id\" specifies a X11 window id as oyBlob_s. "
               "The \"display_id\" specifies a X11 Display struct as oyBlob_s. "
               "The " "\"icc_profile\" option of type oyProfile_s optionally "
               "provides a ICC profile to upload to the server.");
  } else if(strcmp(select, "help")==0)
  {
         if(type == oyNAME_NICK)
      return _("help");
    else if(type == oyNAME_NAME)
      return _("Help");
    else
      return _("The oyX1 modules \"set_xcm_region\" handler lets you set "
               "X Color Management compatible client side color regions. "
               "The implementation uses libXcm and Oyranos.");
  }
  return 0;
}
const char *oyX1_texts_set_xcm_region[4] = {"can_handle","set_xcm_region","help",0};

/** @brief    oyX1 oyCMMapi10_s implementation
 *
 *  X Color Management server side regions setup
 *
 *  @version Oyranos: 0.4.0
 *  @since   2012/01/11 (Oyranos: 0.4.0)
 *  @date    2012/01/11
 */
oyCMMapi10_s_    oyX1_api10_set_xcm_region_handler = {

  oyOBJECT_CMM_API10_S,
  0,0,0,
  (oyCMMapi_s*) &oyX1_api10_send_native_update_event_handler,

  CMMapiInit,
  CMMapiReset,
  CMMMessageFuncSet,

  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH
  "set_xcm_region._" CMM_NICK,

  {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C},/**< version[3] */
  CMM_API_VERSION,                     /**< int32_t module_api[3] */
  0,   /* id_; keep empty */
  0,   /* api5_; keep empty */
  0,                         /**< oyPointer_s * runtime_context */
 
  oyX1InfoGetTextMyHandler,             /**< getText */
  (char**)oyX1_texts_set_xcm_region,       /**<texts; list of arguments to getText*/
 
  oyX1MOptions_Handle                  /**< oyMOptions_Handle_f oyMOptions_Handle */
};

