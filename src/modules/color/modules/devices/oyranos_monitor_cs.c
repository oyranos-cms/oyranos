/** @file oyranos_monitor.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2005-2013 (C) Kai-Uwe Behrmann
 *
 *  @brief    monitor osX device detection
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2005/01/31
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "limits.h"
#include <unistd.h>  /* intptr_t */
#include <X11/Xcm/XcmEdidParse.h>

#include "oyranos_config_internal.h"
#include "oyranos_string.h"

#ifdef __APPLE__
/* old style Quicktime APIs */
#include <Carbon/Carbon.h>
/* newer style CoreGraphics APIs like CGDirectDisplay.h and so on */
#include <ApplicationServices/ApplicationServices.h>
/* use the CFDict C access to probably IOFramebuffer */
#include <IOKit/Graphics/IOGraphicsLib.h> /* IODisplayCreateInfoDictionary() */
char * printCFDictionary( CFDictionaryRef dict );
#endif

#include "oyranos.h"
#include "oyranos_internal.h"
#include "oyranos_cs.h"
#include "oyranos_monitor.h"
#include "oyranos_monitor_internal_cs.h"
#include "oyranos_monitor_internal.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_sentinel.h"

/* ---  Helpers  --- */

/* --- internal API definition --- */



char**qarzGetAllScreenNames_        (const char *display_name, int *n_scr );
int   qarzMonitor_getScreenGeometry_   ( qarzMonitor_s       * disp );
/** @internal Display functions */
const char* qarzMonitor_name_( qarzMonitor_s *disp ) { return disp->name; }
const char* qarzMonitor_hostName_( qarzMonitor_s *disp ) { return disp->host; }
const char* qarzMonitor_identifier_( qarzMonitor_s *disp ) { return disp->identifier; }
/** @internal the screen appendment for the root window properties */
char*       qarzMonitor_screenIdentifier_( qarzMonitor_s *disp )
{ char *number = 0;

  oyAllocHelper_m_( number, char, 24, 0, return "");
  number[0] = 0;
  if( disp->geo[1] >= 1 && !disp->screen ) sprintf( number,"_%d", disp->geo[1]);
  return number;
}
int qarzMonitor_deviceScreen_( qarzMonitor_s *disp ) { return disp->screen; }
int qarzMonitor_number_( qarzMonitor_s *disp ) { return disp->geo[0]; }
int qarzMonitor_screen_( qarzMonitor_s *disp ) { return disp->geo[1]; }
int qarzMonitor_x_( qarzMonitor_s *disp ) { return disp->geo[2]; }
int qarzMonitor_y_( qarzMonitor_s *disp ) { return disp->geo[3]; }
int qarzMonitor_width_( qarzMonitor_s *disp ) { return disp->geo[4]; }
int qarzMonitor_height_( qarzMonitor_s *disp ) { return disp->geo[5]; }
int   qarzMonitor_getGeometryIdentifier_(qarzMonitor_s       * disp );
CGDirectDisplayID qarzMonitor_device_( qarzMonitor_s *disp ) { return disp->id; }





#if 0
OSErr oyCMIterateDeviceInfoProc          ( const CMDeviceInfo* dev_info,
                                           void              * ptr )
{
  /*char ** my_data = (char**) ptr;*/
  char dev_class[8] = {0,0,0,0,0,0,0,0};

  memcpy( dev_class, &dev_info->deviceClass, 4 );
  dev_class[4] = 0;

  /* watch the device */
  /*WARNc2_S( "%s %d", dev_class, dev_info->deviceID );*/

  if(dev_info->deviceClass == cmDisplayDeviceClass)
  {
  }

  return noErr;
}
#endif

CGDirectDisplayID qarzMonitor_nameToOsxID ( const char        * display_name )
{
#if 0
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
#endif

  int pos = 0;
  CGDisplayErr err = kCGErrorSuccess;
  CGDisplayCount alloc_disps = 0;
  CGDirectDisplayID * active_displays = 0,
                    cg_direct_display_id = 0;
  CGDisplayCount count = 0;

  err = CGGetActiveDisplayList( alloc_disps, active_displays, &count );
  if(count <= 0 || err != kCGErrorSuccess)
  {
    WARNc2_S("%s %s", _("open X Display failed"), display_name)
    return 0;
  }
  alloc_disps = count + 1;
  oyAllocHelper_m_( active_displays, CGDirectDisplayID, alloc_disps,0,return 0);
  if(active_displays)
    err = CGGetActiveDisplayList( alloc_disps, active_displays, &count);

  if(display_name && display_name[0])
    pos = atoi( display_name );

  if(err)
  {
    WARNc1_S( "CGGetActiveDisplayList call with error %d", err );
  } else
  {
    cg_direct_display_id = active_displays[pos];

    oyFree_m_( active_displays );
  }

  return cg_direct_display_id;
}



char *       qarzGetMonitorProfile   ( const char        * device_name,
                                       int                 flags,
                                       size_t            * size )
{
  char       *moni_profile=0;

  CMProfileRef prof = NULL;
  CGDirectDisplayID screenID = 0;
  CMProfileLocation loc;
  int err = 0;
  UInt32 locationSize = sizeof(CMProfileLocation);
  int version = oyOSxVersionAtRuntime();

  DBG_PROG_START

  /* Since osX SL DeviceRGB is set to sRGB. And we handle the native profile 
   * like the X Color Management one */
  if(flags & 0x01 ||
     version < 100600)
  {
    screenID = qarzMonitor_nameToOsxID( device_name );

    CMGetProfileByAVID( (CMDisplayIDType)screenID, &prof );
    NCMGetProfileLocation(prof, &loc, &locationSize );

    err = oyGetProfileBlockOSX (prof, &moni_profile, size, malloc);
    if (prof) CMCloseProfile(prof);

  } else if(version >= 100600)
  {
    oyOptions_s * options = NULL;
    uint32_t icc_profile_flags = oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                                                        "//" OY_TYPE_STD "/icc_color",
                                                                        options, 0 );
    oyProfile_s * prof = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, NULL );
    moni_profile = oyProfile_GetMem( prof, size, 0, oyAllocateFunc_ );
  }

#if 0
{
    UInt32 seed = 0,
           count = 0;
    char * my_data = 0;
    CMError cm_err = CMIterateColorDevices( oyCMIterateDeviceInfoProc, 
                                            &seed, &count, &my_data );
  }
#endif

  DBG_PROG_ENDE
  return moni_profile;
}


int      qarzGetAllScreenNames       ( const char        * display_name,
                                       char            *** display_names )
{
  int i = 0;
  char** list = 0;

  list = qarzGetAllScreenNames_( display_name, &i );

  *display_names = 0;

  if(list && i)
  {
    *display_names = oyStringListAppend_( 0, 0, (const char**)list, i, &i,
                                          malloc );
    oyStringListRelease_( &list, i, oyDeAllocateFunc_ );
  }

  return i; 
}

char**
qarzGetAllScreenNames_          (const char *display_name,
                                 int *n_scr)
{
  int i = 0;
  char** list = 0;


  int ids[256];
  CGDisplayErr err = kCGErrorSuccess;
  CGDisplayCount alloc_disps = 0;
  CGDirectDisplayID * active_displays = 0;
  CGDisplayCount count = 0;
  io_service_t io_service = 0;

  *n_scr = 0;

  err = CGGetActiveDisplayList( alloc_disps, active_displays, &count );
  if(count <= 0 || err != kCGErrorSuccess)
  {
    WARNc2_S("%s %s", _("open X Display failed"), display_name)
    return 0;
  }
  alloc_disps = count + 1;
  oyAllocHelper_m_( active_displays, CGDirectDisplayID, alloc_disps,0,return 0);
  if(active_displays)
    err = CGGetActiveDisplayList( alloc_disps, active_displays, &count);
  for(i = 0; i < count && i < 256; ++i)
  {
    io_service = CGDisplayIOServicePort ( active_displays[i] );
    ids[i] = i;
  }
  i = count;

  *n_scr = i;
  oyAllocHelper_m_( list, char*, *n_scr, 0, return NULL )

  for ( i = 0; i < *n_scr ; ++i )
  {
    oyAllocHelper_m_( list[i], char, 24, 0, return NULL )
    oySnprintf1_( list[i], 24, "%d", ids[i] );
  }

  return list;
}


/** @internal
 *  Function qarzRectangle_FromDevice
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
int          qarzRectangle_FromDevice (
                                       const char        * device_name,
                                       double            * x,
                                       double            * y,
                                       double            * width,
                                       double            * height )
{
  int error = !device_name;

  if(!error)
  {
    qarzMonitor_s * disp = 0;

    disp = qarzMonitor_newFrom_( device_name, 0 );
    if(!disp)
      return 0;

    *x = qarzMonitor_x_(disp);
    *y =  qarzMonitor_y_(disp);
    *width = qarzMonitor_width_(disp);
    *height = qarzMonitor_height_(disp);

    qarzMonitor_release_( &disp );
  }

  return error;
}


/** @internal
    takes a chaked display name and point as argument and
    gives a string back for search in the db
 */
int
qarzMonitor_getGeometryIdentifier_         (qarzMonitor_s  *disp)
{
  int len = 64;

  oyAllocHelper_m_( disp->identifier, char, len, 0, return 1 )

  oySnprintf4_( disp->identifier, len, "%dx%d+%d+%d", 
            qarzMonitor_width_(disp), qarzMonitor_height_(disp),
            qarzMonitor_x_(disp), qarzMonitor_y_(disp) );

  return 0;
}



int      qarzMonitorProfileSetup     ( const char        * display_name,
                                       const char        * profil_name,
                                       const char        * profile_data,
                                       size_t              profile_data_size )
{
  int error = 0;
  const char * profile_fullname = 0;
  const char * profil_basename = 0;
  char* profile_name_ = 0;
  oyProfile_s * prof = 0;

  DBG_PROG_START

  DBG_NUM1_S( "display_name: %s", display_name );

  if(profil_name)
  {
    DBG_PROG1_S( "profil_name = %s", profil_name );
    prof = oyProfile_FromName( profil_name, 0, 0 );
    profile_fullname = oyProfile_GetFileName( prof, -1 );
  }

  if( profile_fullname && profile_fullname[0] )
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

    {
      CMProfileLocation loc;
      CMError err = 0;
      CMProfileRef prof=NULL;
      CGDirectDisplayID screenID = 0;

      loc.locType = cmPathBasedProfile;
      oySnprintf1_( loc.u.pathLoc.path, 255, "%s", profile_fullname);

      err = CMOpenProfile ( &prof, &loc );
      screenID = qarzMonitor_nameToOsxID( display_name );

      if( screenID && !err )
        err = CMSetProfileByAVID ( (CMDisplayIDType)screenID, prof );
      
      if(err)
      {
        WARNc3_S("Could not set profile for monitor: \"%s\" %s %d", display_name, profile_fullname, (int)err );
        error = err;
      } else if(oy_debug)
        DBG3_S("Set profile for monitor: \"%s\" \"%s\" %d", display_name, profile_fullname, (int)err );

      CMCloseProfile( prof );
    }
  }

  oyProfile_Release( &prof );
  if(profile_name_) oyFree_m_( profile_name_ );

  DBG_PROG_ENDE
  return error;
}


int      qarzMonitorProfileUnset     ( const char        * display_name )
{
  int error = 0;


    {
      CGDirectDisplayID screenID = 0;


      screenID = qarzMonitor_nameToOsxID( display_name );

      if(oy_debug)
        DBG2_S("Unset profile for monitor makes usually no sense: \"%s\" %d", display_name, (int)screenID );
#ifdef QARZ_UNSET_MAKES_SENSE
      CMError err = 0;
      CMProfileRef prof=NULL;
      if( screenID && !err )
        err = CMSetProfileByAVID ( (CMDisplayIDType)screenID, prof );
#endif
    }

  DBG_PROG_ENDE
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
 *  @date    2009/04/01
 */
qarzMonitor_s* qarzMonitor_newFrom_      ( const char        * display_name,
                                       int                 expensive )
{
  int error = 0;
  int i = 0;
  qarzMonitor_s * disp = 0;

  DBG_PROG_START

  disp = oyAllocateFunc_( sizeof(qarzMonitor_s) );
  error = !disp;
  if(!error)
    error = !memset( disp, 0, sizeof(qarzMonitor_s) );

  disp->type_ = oyOBJECT_MONITOR_S;

  if( display_name )
  {
    if( display_name[0] )
      disp->name = oyStringCopy_( display_name, oyAllocateFunc_ );
  } else
  {
    if(getenv("DISPLAY") && strlen(getenv("DISPLAY")))
      disp->name = oyStringCopy_( getenv("DISPLAY"), oyAllocateFunc_ );
    else
      disp->name = oyStringCopy_( "0", oyAllocateFunc_ );
  }

  if( !error &&
      (disp->host = oyStringCopy_( "0", oyAllocateFunc_ )) == 0 )
    error = 1;

  for( i = 0; i < 6; ++i )
    disp->geo[i] = -1;

  disp->id = qarzMonitor_nameToOsxID( display_name );

  if( !error &&
      qarzMonitor_getScreenGeometry_( disp ) != 0 )
    error = 1;

  if( error <= 0 )
    error = qarzMonitor_getGeometryIdentifier_( disp );

  /*if( !disp->system_port || !oyStrlen_( disp->system_port ) )
  if( 0 <= qarzMonitor_screen_( disp ) && qarzMonitor_screen_( disp ) < 10000 )
  {
    disp->system_port = (char*)oyAllocateWrapFunc_( 12, oyAllocateFunc_ );
    oySprintf_( disp->system_port, "%d", qarzMonitor_screen_( disp ) );
  }*/

  DBG_PROG_ENDE
  return disp;
}

/** @internal get the geometry of a screen 
 *
 *  @param          disp      display info structure
 *  @return         error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/00/00 (Oyranos: 0.1.8)
 *  @date    2009/04/01
 */
int  qarzMonitor_getScreenGeometry_    ( qarzMonitor_s       * disp )
{
  int error = 0;

  {
    CGRect            r = CGRectNull;

    disp->geo[0] = 0; /* @todo how is display management handled in osX? */
    disp->geo[1] = disp->screen;

    r = CGDisplayBounds( qarzMonitor_device_( disp ) );

    disp->geo[2] = r.origin.x;
    disp->geo[3] = r.origin.y;
    disp->geo[4] = r.size.width;
    disp->geo[5] = r.size.height;
  }

  return error;
}

/** @internal
 *  @brief release a monitor information stuct
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/17 (Oyranos: 0.1.8)
 *  @date    2009/04/01
 */
int          qarzMonitor_release_      ( qarzMonitor_s      ** obj )
{
  int error = 0;
  /* ---- start of common object destructor ----- */
  qarzMonitor_s * s = 0;
  
  if(!obj || !*obj)
    return 0;
  
  s = *obj;
  
  if( s->type_ != oyOBJECT_MONITOR_S)
  { 
    WARNc_S("Attempt to release a non qarzMonitor_s object.")
    return 1;
  }
  /* ---- end of common object destructor ------- */

  if(s->name) oyDeAllocateFunc_( s->name );
  if(s->host) oyDeAllocateFunc_( s->host );
  if(s->identifier) oyDeAllocateFunc_( s->identifier );


  s->geo[0] = s->geo[1] = -1;

  oyDeAllocateFunc_( s );
  s = 0;

  *obj = 0;

  return error; 
}


/* separate from the internal functions */

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
int   qarzGetMonitorInfo_lib         ( const char        * display_name,
                                       char             ** manufacturer,
                                       char             ** mnft,
                                       char             ** model,
                                       char             ** serial,
                                       char             ** vendor,
                                       char             ** device_geometry,
                                       char             ** system_port,
                                       char             ** host,
                                       int               * week,
                                       int               * year,
                                       int               * mnft_id,
                                       int               * model_id,
                                       double            * colors,
                                       char             ** edid,
                                       size_t            * edid_size,
                                       int                 refresh_edid )
{
  int err = 0;

  DBG_PROG_START

  {
    char *t;
    qarzMonitor_s * disp = 0;
    /*oyBlob_s * prop = 0;*/

    disp = qarzMonitor_newFrom_( display_name, 0 );
    if(!disp)
      return 1;

    if( disp ) 
    {
      CFDictionaryRef dict = 0;
      unsigned char edi_[256] = {0,0,0,0,0,0,0,0}; /* mark the EDID signature */
      CFTypeRef cf_value = 0;
      CFRange cf_range = {0,256};
      int count = 0;
      io_service_t io_service = CGDisplayIOServicePort (
                                                    qarzMonitor_device_( disp ) );

      t = 0;
      dict = IODisplayCreateInfoDictionary( io_service, 0 );
      if(dict)
      {
        count = CFDictionaryGetCountOfKey( dict, CFSTR( kIODisplayEDIDKey ) );
        if(count)
        {
          cf_value = CFDictionaryGetValue( dict, CFSTR( kIODisplayEDIDKey ));

          if(cf_value)
            CFDataGetBytes( cf_value, cf_range, edi_ );
          else
            WARNc3_S( "\n  %s:\n  %s\n  %s",
               _("no EDID available from"),
               "\"CFDictionaryGetValue\"",
               _("Cant read hardware information from device."))

        } else
          WARNc3_S( "\n  %s:\n  %s\n  %s",
               _("no EDID available from"),
               "\"CFDictionaryGetCountOfKey\"",
               _("Cant read hardware information from device."))

        if(oy_debug)
          printf("%s", printCFDictionary(dict) );

        CFRelease( dict ); dict = 0;
      }
      else
        WARNc3_S( "\n  %s:\n  %s\n  %s",
               _("no EDID available from"),
               "\"IODisplayCreateInfoDictionary\"",
               _("Cant read hardware information from device.") )

      if(edid)
      {
        *edid = malloc(128);
        memcpy( *edid, edi_, 128 );
        *edid_size = 128;
      }

      *system_port = t; t = 0;
    }
    else
      WARNc3_S( "\n  %s: \"%s\"\n  %s",
               _("no qarzMonitor_s from"),
               display_name,
               _("Cant read hardware information from device."))


    if( device_geometry )
      *device_geometry = oyStringCopy_( qarzMonitor_identifier_( disp ),
                                         malloc );


    err = 0;
    qarzMonitor_release_( &disp );
  }

  if(*manufacturer)
    DBG_PROG_S( *manufacturer );
  if(*model)
    DBG_PROG_S( *model );
  if(*serial)
    DBG_PROG_S( *serial );

  DBG_PROG_ENDE
  return err;
}


char * printCFDictionary( CFDictionaryRef dict )
{
  char * text = 0;

  int count = CFDictionaryGetCount( dict );
  if(count)
  {
          char ** keys = malloc(count * sizeof(void*));
          char ** values = malloc(count * sizeof(void*));
          int i;
          char txt[128];
          const char * type = "????";
          CFDictionaryGetKeysAndValues( dict, (CFTypeRef*)keys, (CFTypeRef*)values );
          for (i = 0; i < count; ++i)
          {
            CFTypeID cf_id;
            CFTypeRef cf_element = keys[i];
            const char * key = 0;
            char * value = 0;
            CFStringRef string = (CFStringRef)cf_element;

            key = CFStringGetCStringPtr( string, kCFStringEncodingMacRoman );
            cf_id = CFGetTypeID(values[i]);
            string = CFCopyTypeIDDescription( cf_id );
            type = CFStringGetCStringPtr( string, kCFStringEncodingMacRoman );
            if(cf_id == CFBooleanGetTypeID())
            {
              CFBooleanRef cf_bol = (CFBooleanRef) values[i];
              if(CFBooleanGetValue( cf_bol ))
                STRING_ADD( value, "true" );
              else
                STRING_ADD( value, "false" );
            } else if(cf_id == CFNumberGetTypeID())
            {
              float nv = 0;
              CFNumberRef cf_nm = (CFNumberRef) values[i];
              CFNumberGetValue( cf_nm, kCFNumberFloatType, &nv);
              sprintf( txt, "%g", nv );
              STRING_ADD( value, txt );
            } else if (cf_id == CFStringGetTypeID())
            {
              CFStringGetCString( (CFStringRef)values[i], txt, 128,
                              kCFStringEncodingUTF8 /*kCFStringEncodingASCII*/);
              STRING_ADD( value, txt[0] ? txt : &txt[1] );
            } else if (cf_id == CFDataGetTypeID())
            {
              CFDataRef cf_data = (CFDataRef) values[i];
              CFIndex len = CFDataGetLength( cf_data );
              const unsigned char * ptr = CFDataGetBytePtr( cf_data );
              ptr = 0; len = 0;
            } else if (cf_id == CFDictionaryGetTypeID())
            {
              CFDictionaryRef d = (CFDictionaryRef) values[i];
              value = printCFDictionary( d );
            /*} else if (cf_id == CFGetTypeID())
            {*/
            }

            STRING_ADD(text, key);
            STRING_ADD(text,"[");
            STRING_ADD(text,type);
            STRING_ADD(text,"]:\"");
            STRING_ADD(text,value?value:"????");
            STRING_ADD(text,"\"\n");

            if(value)
              oyDeAllocateFunc_(value); value = 0;
          }
  }

  return text;
}


#define qarz_help_system_specific \
      " One option \"device_name\" will select the according Quarz display.\n" \
      " The \"device_name\" should be identical with the one\n" \
      " returned from a \"list\" request.\n" \
      " The \"properties\" call might be a expensive one.\n" \
      " Informations are stored in the returned oyConfig_s::backend_core member."

oyMonitorHooks_s qarzMonitorHooks_ = {
  oyOBJECT_MONITOR_HOOKS_S,
  {CMM_NICK},
  10000, /* 1.0.0 */
  qarz_help_system_specific,
  NULL,
  qarzMonitorProfileSetup,
  qarzMonitorProfileUnset,
  qarzRectangle_FromDevice,
  qarzGetMonitorProfile,
  qarzGetAllScreenNames,
  qarzGetMonitorInfo_lib
};

oyMonitorHooks_s * qarzMonitorHooks = &qarzMonitorHooks_;
