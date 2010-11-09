/** @file oyranos_monitor.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2005-2010 (C) Kai-Uwe Behrmann
 *
 *  @brief    monitor osX device detection
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
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

#include "config.h"

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
#include "oyranos_alpha.h"
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



char**oyGetAllScreenNames_        (const char *display_name, int *n_scr );
int   oyMonitor_getScreenGeometry_   ( oyMonitor_s       * disp );
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
int oyMonitor_deviceScreen_( oyMonitor_s *disp ) { return disp->screen; }
int oyMonitor_number_( oyMonitor_s *disp ) { return disp->geo[0]; }
int oyMonitor_screen_( oyMonitor_s *disp ) { return disp->geo[1]; }
int oyMonitor_x_( oyMonitor_s *disp ) { return disp->geo[2]; }
int oyMonitor_y_( oyMonitor_s *disp ) { return disp->geo[3]; }
int oyMonitor_width_( oyMonitor_s *disp ) { return disp->geo[4]; }
int oyMonitor_height_( oyMonitor_s *disp ) { return disp->geo[5]; }
int   oyMonitor_getGeometryIdentifier_(oyMonitor_s       * disp );
CGDirectDisplayID oyMonitor_device_( oyMonitor_s *disp ) { return disp->id; }


/** @internal oyUnrollEdid1_ */
void         oyUnrollEdid1_          ( void              * edid,
                                       char             ** manufacturer,
                                       char             ** mnft,
                                       char             ** model,
                                       char             ** serial,
                                       char             ** vendor,
                                       uint32_t          * week,
                                       uint32_t          * year,
                                       uint32_t          * mnft_id,
                                       uint32_t          * model_id,
                                       double            * c,
                                       oyAlloc_f           allocate_func)
{
  int i, count = 0;
  XcmEdidKeyValue_s * list = 0;
  XCM_EDID_ERROR_e err = 0;

  DBG_PROG_START

  err = XcmEdidParse( edid, &list, &count );
  if(err)
    WARNc_S(XcmEdidErrorToString(err));

  if(list)
  for(i = 0; i < count; ++i)
  {
         if(manufacturer && oyStrcmp_( list[i].key, "manufacturer") == 0)
      *manufacturer = oyStringCopy_(list[i].value.text, allocate_func);
    else if(mnft && oyStrcmp_( list[i].key, "mnft") == 0)
      *mnft = oyStringCopy_(list[i].value.text, allocate_func);
    else if(model && oyStrcmp_( list[i].key, "model") == 0)
      *model = oyStringCopy_(list[i].value.text, allocate_func);
    else if(serial && oyStrcmp_( list[i].key, "serial") == 0)
      *serial = oyStringCopy_(list[i].value.text, allocate_func);
    else if(vendor && oyStrcmp_( list[i].key, "vendor") == 0)
      *vendor = oyStringCopy_(list[i].value.text, allocate_func);
    else if(week && oyStrcmp_( list[i].key, "week") == 0)
      *week = list[i].value.integer;
    else if(year && oyStrcmp_( list[i].key, "year") == 0)
      *year = list[i].value.integer;
    else if(mnft_id && oyStrcmp_( list[i].key, "mnft_id") == 0)
      *mnft_id = list[i].value.integer;
    else if(model_id && oyStrcmp_( list[i].key, "model_id") == 0)
      *model_id = list[i].value.integer;
    else if(c && oyStrcmp_( list[i].key, "redx") == 0)
      c[0] = list[i].value.dbl;
    else if(c && oyStrcmp_( list[i].key, "redy") == 0)
      c[1] = list[i].value.dbl;
    else if(c && oyStrcmp_( list[i].key, "greenx") == 0)
      c[2] = list[i].value.dbl;
    else if(c && oyStrcmp_( list[i].key, "greeny") == 0)
      c[3] = list[i].value.dbl;
    else if(c && oyStrcmp_( list[i].key, "bluex") == 0)
      c[4] = list[i].value.dbl;
    else if(c && oyStrcmp_( list[i].key, "bluey") == 0)
      c[5] = list[i].value.dbl;
    else if(c && oyStrcmp_( list[i].key, "whitex") == 0)
      c[6] = list[i].value.dbl;
    else if(c && oyStrcmp_( list[i].key, "whitey") == 0)
      c[7] = list[i].value.dbl;
    else if(c && oyStrcmp_( list[i].key, "gamma") == 0)
      c[8] = list[i].value.dbl;
  }

  XcmEdidFree( &list );

  DBG_PROG_ENDE
}




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

CGDirectDisplayID oyMonitor_nameToOsxID ( const char        * display_name )
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



char *       oyX1GetMonitorProfile   ( const char        * device_name,
                                       uint32_t            flags,
                                       size_t            * size,
                                       oyAlloc_f           allocate_func )
{
  char       *moni_profile=0;
  int error = 0;


  CMProfileRef prof = NULL;
  CGDirectDisplayID screenID = 0;
  CMProfileLocation loc;
  int err = 0;
  char * block = NULL;
  UInt32 locationSize = sizeof(CMProfileLocation);

  DBG_PROG_START

  screenID = oyMonitor_nameToOsxID( device_name );

  CMGetProfileByAVID( (CMDisplayIDType)screenID, &prof );
  NCMGetProfileLocation(prof, &loc, &locationSize );

  err = oyGetProfileBlockOSX (prof, &block, size, allocate_func);
  moni_profile = block;
  if (prof) CMCloseProfile(prof);

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
  if(!error)
    return moni_profile;
  else
    return NULL;
}


int      oyGetAllScreenNames         ( const char        * display_name,
                                       char            *** display_names,
                                       oyAlloc_f           allocateFunc )
{
  int i = 0;
  char** list = 0;

  list = oyGetAllScreenNames_( display_name, &i );

  *display_names = 0;

  if(list && i)
  {
    *display_names = oyStringListAppend_( 0, 0, (const char**)list, i, &i,
                                          allocateFunc );
    oyStringListRelease_( &list, i, oyDeAllocateFunc_ );
  }

  return i; 
}

char**
oyGetAllScreenNames_            (const char *display_name,
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
oyRectangle_s* oyX1Rectangle_FromDevice ( const char        * device_name )
{
  oyRectangle_s * rectangle = 0;
  int error = !device_name;

  if(!error)
  {
    oyMonitor_s * disp = 0;

    disp = oyMonitor_newFrom_( device_name, 0 );
    if(!disp)
      return 0;

    rectangle = oyRectangle_NewWith( oyMonitor_x_(disp), oyMonitor_y_(disp),
                           oyMonitor_width_(disp), oyMonitor_height_(disp), 0 );

    oyMonitor_release_( &disp );
  }

  return rectangle;
}


/** @internal
    takes a chaked display name and point as argument and
    gives a string back for search in the db
 */
int
oyMonitor_getGeometryIdentifier_         (oyMonitor_s  *disp)
{
  int len = 64;

  oyAllocHelper_m_( disp->identifier, char, len, 0, return 1 )

  oySnprintf4_( disp->identifier, len, "%dx%d+%d+%d", 
            oyMonitor_width_(disp), oyMonitor_height_(disp),
            oyMonitor_x_(disp), oyMonitor_y_(disp) );

  return 0;
}



int      oyX1MonitorProfileSetup     ( const char        * display_name,
                                       const char        * profil_name )
{
  int error = 0;
  const char * profile_fullname = 0;
  const char * profil_basename = 0;
  char* profile_name_ = 0;
  oyProfile_s * prof = 0;

  DBG_PROG_START

  if(profil_name)
  {
    DBG_PROG1_S( "profil_name = %s", profil_name );
    prof = oyProfile_FromFile( profil_name, 0, 0 );
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
      screenID = oyMonitor_nameToOsxID( display_name );

      if( screenID && !err )
        err = CMSetProfileByAVID ( (CMDisplayIDType)screenID, prof );

      CMCloseProfile( prof );
    }
  }

  oyProfile_Release( &prof );
  if(profile_name_) oyFree_m_( profile_name_ );

  DBG_PROG_ENDE
  return error;
}


int      oyX1MonitorProfileUnset     ( const char        * display_name )
{
  int error = 0;


    {
      CMError err = 0;
      CMProfileRef prof=NULL;
      CGDirectDisplayID screenID = 0;


      screenID = oyMonitor_nameToOsxID( display_name );

      if( screenID && !err )
        err = CMSetProfileByAVID ( (CMDisplayIDType)screenID, prof );

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
oyMonitor_s* oyMonitor_newFrom_      ( const char        * display_name,
                                       int                 expensive )
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

  disp->id = oyMonitor_nameToOsxID( display_name );

  if( !error &&
      oyMonitor_getScreenGeometry_( disp ) != 0 )
    error = 1;

  if( error <= 0 )
    error = oyMonitor_getGeometryIdentifier_( disp );

  /*if( !disp->system_port || !oyStrlen_( disp->system_port ) )
  if( 0 <= oyMonitor_screen_( disp ) && oyMonitor_screen_( disp ) < 10000 )
  {
    disp->system_port = (char*)oyAllocateWrapFunc_( 12, oyAllocateFunc_ );
    oySprintf_( disp->system_port, "%d", oyMonitor_screen_( disp ) );
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
int  oyMonitor_getScreenGeometry_    ( oyMonitor_s       * disp )
{
  int error = 0;

  {
    CGRect            r = CGRectNull;

    disp->geo[0] = 0; /* @todo how is display management handled in osX? */
    disp->geo[1] = disp->screen;

    r = CGDisplayBounds( oyMonitor_device_( disp ) );

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
 *  @param      allocate_func the allocator for the above strings
 *  @return     error
 *
 */
int
oyGetMonitorInfo_lib              (const char* display_name,
                                   char**      manufacturer,
                                       char             ** mnft,
                                   char**      model,
                                   char**      serial,
                                       char             ** vendor,
                                       char             ** display_geometry,
                                       char             ** system_port,
                                       char             ** host,
                                       uint32_t          * week,
                                       uint32_t          * year,
                                       uint32_t          * mnft_id,
                                       uint32_t          * model_id,
                                       double            * colours,
                                       oyBlob_s         ** edid,
                                   oyAlloc_f     allocate_func,
                                       oyStruct_s        * user_data)
{
  int err = 0;

  DBG_PROG_START

  {
    char *t;
    oyMonitor_s * disp = 0;
    /*oyBlob_s * prop = 0;*/

    disp = oyMonitor_newFrom_( display_name, 0 );
    if(!disp)
      return 1;
    if(!allocate_func)
      allocate_func = oyAllocateFunc_;

    if( disp ) 
    {
      CFDictionaryRef dict = 0;
      unsigned char edi_[256] = {0,0,0,0,0,0,0,0}; /* mark the EDID signature */
      CFTypeRef cf_value = 0;
      CFRange cf_range = {0,256};
      int count = 0;
      io_service_t io_service = CGDisplayIOServicePort (
                                                    oyMonitor_device_( disp ) );

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
            WARNcc3_S( user_data, "\n  %s:\n  %s\n  %s",
               _("no EDID available from"),
               "\"CFDictionaryGetValue\"",
               _("Cant read hardware information from device."))

        } else
          WARNcc3_S( user_data, "\n  %s:\n  %s\n  %s",
               _("no EDID available from"),
               "\"CFDictionaryGetCountOfKey\"",
               _("Cant read hardware information from device."))

        if(oy_debug)
          printf("%s", printCFDictionary(dict) );

        CFRelease( dict ); dict = 0;
      }
      else
        WARNcc3_S( user_data, "\n  %s:\n  %s\n  %s",
               _("no EDID available from"),
               "\"IODisplayCreateInfoDictionary\"",
               _("Cant read hardware information from device."))

      if(edi_[0] || edi_[1])
        oyUnrollEdid1_( edi_, manufacturer, mnft, model, serial, vendor,
                        week, year, mnft_id, model_id, colours, allocate_func);

      if(edid)
      {
        *edid = oyBlob_New( 0 );
        oyBlob_SetFromData( *edid, edi_, 128, 0 );
      }

      *system_port = t; t = 0;
    }
    else
      WARNcc3_S( user_data, "\n  %s: \"%s\"\n  %s",
               _("no oyMonitor_s from"),
               display_name,
               _("Cant read hardware information from device."))


    if( display_geometry )
      *display_geometry = oyStringCopy_( oyMonitor_identifier_( disp ),
                                         allocate_func );


    err = 0;
    oyMonitor_release_( &disp );
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

