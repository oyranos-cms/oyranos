{% include "source_file_header.txt" %}
#include <string.h>

#include "oyCMMapi6_s.h"
#include "oyCMMapiFilters_s.h"
#include "oyCMMapi9_s_.h"
#include "oyFilterNode_s_.h"
#include "oyFilterCore_s_.h"
#include "oyFilterNodes_s.h"

#include "oyranos_helper.h"
#include "oyranos_icc.h"
#include "oyranos_texts.h"

#include "oyranos_db.h"
#include "oyranos_devices.h"
#include "oyranos_devices_internal.h"
#include "oyranos_object_internal.h"
#include "oyranos_sentinel.h"
#include "oyConfig_s_.h"
#include "oyOption_s_.h"
#include "oyOptions_s_.h"
#include "oyProfiles_s.h"


/** \addtogroup devices_handling
 *  @brief Color device meta data and profile handling
 *
 *  Devices are a special form of configurations. Their access is grouped
 *  for effective performance. Known devices are queried with
 *  oyDevicesGet(). oyConfigDomainList() provides a list of known device
 *  modules.
 *  A single device can be obtained by oyDeviceGet(). The \a
 *  device_type argument defaults to OY_TYPE_STD and can be omitted for this
 *  group. The \a device_class argument specifies a subgroup, e.g. 
 *  "monitor".
 *
 *  All other functions return a handle to the device. With this handle it is
 *  possible to get informations (oyDeviceGetInfo()), query it's current,
 *  possibly remote profile (oyDeviceAskProfile2()) or typical used get a 
 *  profile with fallbacks including the DB through (oyDeviceGetProfile()), 
 *  set the profile persistent (oyDeviceSetProfile()) or query the persistent
 *  stored profile (oyDeviceProfileFromDB()).
 *
 *  @{ *//* devices_handling */

char *       oyGetFilterNodeKey      ( const char        * base_key,
                                       int                 select_core );


/** Function oyDevicesGet
 *  @brief   get all devices matching to a device class and type
 *
 *  @code
    // "list" all monitors
    oyConfigs_s * monitors = 0;
    int error = oyDevicesGet( 0, "monitor", 0, &monitors );
    // see how many are included
    int n = oyConfigs_Count( monitors );
    // release them
    oyConfigs_Release( &monitors );
    @endcode
 *
 *  For obtaining expensive "properties" informations at once, add the according
 *  option.
 *  @code
    // get all monitors the expensive way
    oyConfigs_s * monitors = 0;
    oyOptions_s * options = oyOptions_New( 0 );
    int error = 0;

    error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/command",
                                   "properties", OY_CREATE_NEW );
    error = oyDevicesGet( 0, "monitor", 0, &monitors );
    oyOptions_Release( &options );

    // see how many are included
    int n = oyConfigs_Count( monitors );

    // release them
    oyConfigs_Release( &monitors );
    @endcode
 *
 *  @param[in]     device_type         the device type ::oyFILTER_REG_TYPE,
 *                                     defaults to OY_TYPE_STD (optional)
 *  @param[in]     device_class        the device class, e.g. "monitor",
 *                                     ::oyFILTER_REG_APPLICATION
 *  @param[in]     options             options for the device
 *  @param[out]    devices             the found devices
 *  @return                            0 - good, >= 1 - error, <= -1 unknown
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/02 (Oyranos: 0.1.10)
 *  @date    2009/02/02
 */
OYAPI int  OYEXPORT
           oyDevicesGet              ( const char        * device_type,
                                       const char        * device_class,
                                       oyOptions_s       * options,
                                       oyConfigs_s      ** devices )
{
  int error = !device_class || !device_class[0];
  oyOptions_s * tmp = NULL;

  if(error > 0)
  {
    WARNc_S( "Argument(s) incorrect. Giving up" );
    return error;
  }

  /** 1. obtain detailed and expensive device informations */
  if(!options)
  {
    /** 1.1 add "list" call to module arguments */
    error = oyOptions_SetDeviceTextKey_( &options, device_type,
                                             device_class,
                                             "command", "list" );
    tmp = options;
  }

  /** 1.2 ask each module */
  if(error <= 0)
    error = oyConfigs_FromDeviceClass( device_type, device_class,
                                           options, devices, 0 );

  oyOptions_Release( &tmp );

  return error;
}

/** Function oyDeviceGet
 *  @brief   ask a module for device informations or other direct calls
 *
 *  @code
    oyConfig_s * device = 0;
    int error = oyDeviceGet( 0, "monitor", ":0.0", 0, &device );
    oyConfig_Release( &device );
    @endcode
 *
 *  @code
    // pass empty options to the module to get a usage message
    oyOptions_s * options = oyOptions_New( 0 );
    oyDeviceGet( OY_TYPE_STD, "monitor", ":0.0", options, 0 );
    @endcode
 *
 *  @param[in]     device_type         the device type, e.g. OY_TYPE_STD,
 *                                     defaults to OY_TYPE_STD (optional)
 *  @param[in]     device_class        registration ::oyFILTER_REG_APPLICATION
 *                                     part, e.g. "monitor", mandatory
 *  @param[in]     device_name         the device name as returned by
 *                                     oyConfigs_FromPattern_f, mandatory,
                                       ::oyFILTER_REG_OPTION
 *  @param[in]     options             options to pass to the module, for zero
 *                                     the verbose and expensive "properties"
 *                                     call is assumed
 *  @param[out]    device              the returned device
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/28 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
OYAPI int  OYEXPORT
           oyDeviceGet               ( const char        * device_type,
                                       const char        * device_class,
                                       const char        * device_name,
                                       oyOptions_s       * options,
                                       oyConfig_s       ** device )
{
  int error = !device_name || !device_name[0] ||
              !device_class || !device_class[0];
  oyConfigs_s * devices = 0;
  oyConfig_s * s = 0;
  oyOptions_s * tmp = NULL;

  if(error > 0)
  {
    WARNc2_S( "No device_name/device_class argument provided. Give up: %s/%s",
              oyNoEmptyString_m_(device_name),
              oyNoEmptyString_m_(device_class) );
    return 0;
  }

  /** 1. obtain basic device informations */

  if(!options)
  {
    /** 1.1 add "list" call to module arguments */
    if(error <= 0)
    error = oyOptions_SetDeviceTextKey_( &options, device_type,
                                             device_class,
                                             "command", "list" );
    tmp = options;
  }

  /** 1.1.2 set device filter */
  if(error <= 0)
    error = oyOptions_SetDeviceTextKey_( &options, device_type,
                                             device_class,
                                             "device_name",device_name);

  /** 2. get the device */
  error = oyConfigs_FromDeviceClass( device_type, device_class,
                                         options, &devices, 0 );

  s = oyConfigs_Get( devices, 0 );

  oyConfigs_Release( &devices );


  /** 3. check for success of device detection */
  error = !s;
  if(error)
    DBG_NUM2_S( "%s: \"%s\"", _("Could not open device"), device_name );

  if(device)
    *device = s;
  else
    oyConfig_Release( &s );

  oyOptions_Release( &tmp );

  return error;
}

/** Function oyDeviceBackendCall
 *  @brief   get device answere from options
 *
 *  @param[in]     device          the device
 *  @param[in]     options             options for the device
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/02 (Oyranos: 0.1.10)
 *  @date    2009/08/25
 */
OYAPI int  OYEXPORT
               oyDeviceBackendCall   ( oyConfig_s        * device,
                                       oyOptions_s       * options )
{
  int error = !device,
      l_error = 0;
  oyConfigs_s * devices = 0;
  oyConfig_s * s = device;
  int new_options = 0;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 1 )

  if(!options)
  {
    /** 1. obtain detailed and expensive device informations for a
     *     zero options argument through the "properties" command. */
    /** 1.1 add "properties" call to module arguments */
    if(error <= 0)
    l_error = oyOptions_SetRegistrationTextKey_( &options,
                                                 oyConfigPriv_m(device)->registration,
                                                 "command", "properties" ); OY_ERR
    new_options = 1;
  }

  devices = oyConfigs_New( 0 );
  error = !devices;
  
  if(error <= 0)
  {
    /* Keep a reference to config as devices will later be released. */
    s = oyConfig_Copy( device, 0 );

    oyConfigs_MoveIn( devices, &device, -1 );
  }

  /** 3. talk to the module */
  l_error = oyConfigs_Modify( devices, options ); OY_ERR

  oyConfigs_Release( &devices );
  if(new_options)
    oyOptions_Release( &options );

  return error;
}

icProfileClassSignature oyDeviceSigGet(oyConfig_s        * device )
{
  icProfileClassSignature deviceSignature = 0;
  if(oyFilterRegistrationMatch( oyConfigPriv_m(device)->registration, "monitor", 0 ))
    deviceSignature = icSigDisplayClass;
  else if(oyFilterRegistrationMatch( oyConfigPriv_m(device)->registration, "scanner", 0 ))
    deviceSignature = icSigInputClass;
  else if(oyFilterRegistrationMatch( oyConfigPriv_m(device)->registration, "camera", 0 ))
    deviceSignature = icSigInputClass;
  else if(oyFilterRegistrationMatch( oyConfigPriv_m(device)->registration, "printer", 0 ))
    deviceSignature = icSigOutputClass;

  return deviceSignature;
}

/** Function oyDeviceSetup
 *  @brief   activate the device using the stored configuration
 *  @deprecated
 *
 *  Please move on to oyDeviceSetup2() call with extended
 *  functionality.
 *
 *  @param[in]     device              the device
 *  @param[in]     options             additional options,
 *                                     - "skip_ask_for_profile == "yes" - skips oyDeviceAskProfile2() call; useful for systems, where no empty profile is possible like CS
 *  @return                            error
 *
 *  @version Oyranos: 0.9.6
 *  @date    2015/02/04
 *  @since   2009/01/30 (Oyranos: 0.1.10)
 */
OYAPI int  OYEXPORT
         oyDeviceSetup               ( oyConfig_s        * device,
                                       oyOptions_s       * options )
{
  int error = !device;
  oyProfile_s * p = 0;
  char * profile_name = 0,
       * profile_name_temp = 0;
  const char * device_name = 0;
  oyConfig_s * s = device;
  oyOption_s * o;
  int32_t icc_profile_flags = 0;

  oyOptions_FindInt( options, "icc_profile_flags", 0, &icc_profile_flags );

  oyCheckType__m( oyOBJECT_CONFIG_S, return 1 )

  {
    /* 1. ask for the profile the device is setup with */
    if(!oyOptions_FindString( options, "skip_ask_for_profile", "yes" ))
      error = oyDeviceAskProfile2( device, options, &p );
    if(p)
    {
      oyProfile_Release( &p );
      /** We ignore a device, which already has its profile setup. */
      return error;
    }

    /* 2. query the full device information */
    error = oyDeviceProfileFromDB( device, &profile_name, 0 );

    /* 2.1 select best match to device from installed profiles */
    if(!profile_name)
    {
      int size;
      oyProfile_s * profile = 0;
      oyProfiles_s * patterns = 0, * iccs = 0;
      icProfileClassSignature device_signature = oyDeviceSigGet(device);
      int32_t * rank_list = 0;
      double clck;

      profile = oyProfile_FromSignature( device_signature, oySIGNATURE_CLASS, 0 );
      patterns = oyProfiles_New( 0 );
      oyProfiles_MoveIn( patterns, &profile, -1 );

      clck = oyClock();
      iccs = oyProfiles_Create( patterns, icc_profile_flags, 0 );
      clck = oyClock() - clck;
      DBG_NUM4_S("oyProfiles_Create(icc_profile_flags: %d, %s) = %d: %g", icc_profile_flags,
                 oyICCDeviceClassDescription(device_signature), oyProfiles_Count(iccs), clck/1000000.0 );
      oyProfiles_Release( &patterns );

      size = oyProfiles_Count(iccs);
      oyAllocHelper_m_( rank_list, int32_t, oyProfiles_Count(iccs), 0, error = 1; return error );
      if(error <= 0)
      {
        clck = oyClock();
        oyProfiles_DeviceRank( iccs, device, rank_list );
        clck = oyClock() - clck;
        DBG_NUM1_S("oyProfiles_DeviceRank(): %g", clck/1000000.0 );
      }
      if(error <= 0 && size && rank_list[0] > 0)
      {
        p = oyProfiles_Get( iccs, 0 );
        profile_name = oyStringCopy_( oyProfile_GetFileName(p, -1),
                                      oyAllocateFunc_ );
        WARNc1_S( "implicitely selected %s", oyNoEmptyString_m_(profile_name) );
        if(oy_debug > 1)
        {
          int i, n = oyProfiles_Count( iccs );
#if DEBUG
          const char * fn;
#endif
          oyProfile_Release( &p );
          for(i = 0; i < n; ++i)
          {
            p = oyProfiles_Get( iccs, i );
#if DEBUG
            fn = oyProfile_GetFileName(p, -1);
            if(rank_list[i])
              DBG_NUM2_S("%d: %s", rank_list[i], fn);
#endif
          }
        }
        oyFree_m_( rank_list );
      }

      oyProfile_Release( &p );
      oyProfiles_Release( &iccs );
    }


    if(!profile_name)
    {
      error = oyOptions_SetRegistrationTextKey_( &options,
                                                 oyConfigPriv_m(device)->registration,
                                                 "icc_profile.fallback","true");
      /* 2.2.1 try fallback for rescue */
      error = oyDeviceAskProfile2( device, options, &p );
      if(p)
      {
        profile_name = oyStringCopy_( oyProfile_GetFileName(p, -1),
                                      oyAllocateFunc_ );
        if(!profile_name)
        {
          oyOptions_s * opts = 0;
          oyOptions_SetFromString( &opts, "////device", "1", OY_CREATE_NEW );
          error = oyProfile_Install( p, oySCOPE_USER, opts );
          oyOptions_Release( &opts );
          if(!error)
            profile_name = oyStringCopy_( oyProfile_GetFileName(p, -1),
                                      oyAllocateFunc_ );
          else
          {
            char * data = 0;
            size_t size = 0;
            data = oyProfile_GetMem( p, &size, 0, oyAllocateFunc_ );
            error = oyWriteMemToFile2_( "oyranos_tmp.icc", data, size,
                                        OY_FILE_NAME_SEARCH | OY_FILE_TEMP_DIR,
                                        &profile_name_temp, oyAllocateFunc_ );
          }

          if(profile_name_temp)
            profile_name = profile_name_temp;
          else if( !profile_name )
          {
            error = 1;
            WARNc2_S("%s: \"%s\"(oyranos_tmp.icc)",_("Could not write to file"),
                      oyNoEmptyString_m_(profile_name_temp) );
          }
        }
        oyProfile_Release( &p );
      }

      if(!profile_name)
        return error;
    }

    /* 2.3 get device_name */
    device_name = oyConfig_FindString( device, "device_name", 0);

    /* 3. setup the device through the module */
    error = oyOptions_SetRegistrationTextKey_( &options,
                                               oyConfigPriv_m(device)->registration,
                                               "command", "setup" );
    error = oyOptions_SetRegistrationTextKey_( &options,
                                               oyConfigPriv_m(device)->registration,
                                               "device_name", device_name );
    error = oyOptions_SetRegistrationTextKey_( &options,
                                               oyConfigPriv_m(device)->registration,
                                               "profile_name", profile_name );
    /* 3.1 send the query to a module */
    error = oyDeviceBackendCall( device, options );

    o = oyConfig_Find( device, "icc_profile" );
    p = (oyProfile_s*) oyOption_GetStruct( o, oyOBJECT_PROFILE_S );
    oyOption_Release( &o );

    /* 3.2 check if the module has used that profile and complete do that if needed */
    if(!p)
    {
      int has = 0;
#define OY_DOMAIN OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD
      o = oyConfig_Find( device, "icc_profile" );
      if(!o)
        o = oyOption_FromRegistration( OY_DOMAIN OY_SLASH "icc_profile", 0 );

      p = oyProfile_FromName( profile_name, icc_profile_flags, 0 );

      if(p)
      {
        has = 1;
        error = oyOption_MoveInStruct( o, (oyStruct_s**) &p );
      }
      else
      /** Warn on not found profile. */
      {
        oyMessageFunc_p( oyMSG_ERROR,(oyStruct_s*)device,
                       OY_DBG_FORMAT_ "\n\t%s: \"%s\"\n\t%s\n", OY_DBG_ARGS_,
                _("Could not open ICC profile"), profile_name,
                _("install in the OpenIccDirectory icc path") );
      }

      if(has)
        oyOptions_Set( oyConfigPriv_m(device)->data, o, -1, 0 );
      oyOption_Release( &o );
      oyProfile_Release( &p );
    }

    /* 4. ping the DB */
    {
      char * v = oyGetPersistentString( OY_STD OY_SLASH "ping", oySOURCE_DATA,
                                        oySCOPE_USER, oyAllocateFunc_ );
      oySetPersistentString( OY_STD OY_SLASH "ping", oySCOPE_USER, 
                             (v && strcmp( v, "1" ) == 0) ? "0" : "1", 0 );
      if(v) oyFree_m_(v);
    }

    if(profile_name_temp)
      oyRemoveFile_( profile_name_temp );
    profile_name_temp = 0;
    if(profile_name)
      oyFree_m_( profile_name );
  }

  return error;
}

/** Function oyDeviceUnset
 *  @brief   unset the device profile
 *
 *  The function solely calls \a unset in the module, e.g. unset graphic card
 *  luts and server stored profile. So pretty all device/server side 
 *  informatin should go away. \n
 *
 *  @param         device          the device
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/12 (Oyranos: 0.1.10)
 *  @date    2009/02/12
 */
int      oyDeviceUnset               ( oyConfig_s        * device )
{
  int error = !device;
  oyOptions_s * options = 0;
  char * profile_name = 0;
  const char * device_name = 0;
  oyConfig_s * s = device;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 1 )

  {
    /** 1. query the full device information */
    error = oyDeviceProfileFromDB( device, &profile_name, 0 );

    /** 1.1 get device_name */
    device_name = oyConfig_FindString( device, "device_name", 0);

    /** 2. unset the device through the module */
    /** 2.1 set a general request */
    error = oyOptions_SetRegistrationTextKey_( &options,
                                               oyConfigPriv_m(device)->registration,
                                               "command", "unset" );
    error = oyOptions_SetRegistrationTextKey_( &options,
                                               oyConfigPriv_m(device)->registration,
                                               "device_name", device_name );

    /** 2.2 send the query to a module */
    error = oyConfigs_FromDomain( oyConfigPriv_m(device)->registration, options, 0, 0 );

    /** 3. ping the DB */
    {
      char * v = oyGetPersistentString( OY_STD OY_SLASH "ping", oySOURCE_DATA,
                                        oySCOPE_USER, oyAllocateFunc_ );
      oySetPersistentString( OY_STD OY_SLASH "ping", oySCOPE_USER, 
                             (v && strcmp( v, "1" ) == 0) ? "0" : "1", 0 );
      if(v) oyFree_m_(v);
    }

    oyOptions_Release( &options );
    if(profile_name)
      oyFree_m_( profile_name );
  }

  return error;
}

/** Function oyDeviceGetInfo
 *  @brief   get all devices matching to a device class and type
 *
 *  @code
    // print all properties
    int error = oyDeviceGetInfo( device, oyNAME_DESCRIPTION, 0, &text,
                                     malloc );
    char * list = text, * tmp = 0, * line = malloc(128);
    int even = 1;

        tmp = list;
        while(list && list[0])
        {
          snprintf( line, 128, "%s", list );
          if(strchr( line, '\n' ))
          {
            tmp = strchr( line, '\n' );
            tmp[0] = 0;
          }
          if(even)
            printf( "%s\n", line );
          else
            printf( "  %s\n", line );
          list = strchr( list, '\n' );
          if(list) ++list;
          even = !even;
        }

    if(line) free(line);
    if(text) free(text);
    @endcode
 *
 *  To obtain a certain single pice of information you do not need 
 *  oyDeviceGetInfo. See the following example:
 *  @code
    char * device_name = ":0.0"; // a typical device
    char * text = 0;
    oyConfig_s * device = 0;
    oyOptions_s * options = 0;
    int error = 0;

    // tell the module with the "properties" call to add all informations
    error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD
                                   "/config/command",
                                   "properties", OY_CREATE_NEW );

    oyDeviceGet( OY_TYPE_STD, "monitor", device_name, options, &device );
    text = oyConfig_FindString( device, "manufacturer", 0 );
    @endcode
 *
 *  @param[in]     device          the device
 *  @param[in]     type                influences the info_text output
 *                                     - oyNAME_NAME - a short one line text,
 *                                     - oyNAME_NICK - one word,
 *                                     - oyNAME_DESCRIPTION - expensive text,
 *                                     even lines contain the property key name,
 *                                     odd lines contain the value,
 *                                     lines are separated by newline '\\n'
 *  @param[in]     options             defaults to command=properties
 *  @param[out]    info_text           the text
 *  @param[in]     allocateFunc        the user allocator for info_text
 *  @return                            0 - good, 1 >= error, -1 <= issue(s)
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/02 (Oyranos: 0.1.10)
 *  @date    2009/03/27
 */
OYAPI int  OYEXPORT oyDeviceGetInfo  ( oyConfig_s        * device,
                                       oyNAME_e            type,
                                       oyOptions_s       * options,
                                       char             ** info_text,
                                       oyAlloc_f           allocateFunc )
{
  int error = !device || !info_text;
  oyConfig_s_ * device_ = (oyConfig_s_*)device;
  oyOption_s * o = 0;
  oyConfig_s * config = 0;
  const char * tmp = 0;
  char * text = 0, * t = 0;
  int i, n,
      own_options = 0;
  oyConfig_s * s = device;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 1 )

  if(error > 0)
  {
    WARNc_S( "Argument(s) incorrect. Giving up" );
    return error;
  }

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  if(!options)
    own_options = 1;

  if(type == oyNAME_NICK)
  {
    tmp = oyOptions_FindString( device_->backend_core,"device_name", 0 );
    *info_text = oyStringCopy_( tmp, allocateFunc );
    return error;
  }

  if(type == oyNAME_DESCRIPTION)
  {
    /* get expensive infos */
    if(oyOptions_Count( device_->backend_core ) < 2)
    {
      error = oyOptions_SetRegistrationTextKey_( &options,
                                               oyConfigPriv_m(device)->registration,
                                               "command", "properties" );

      if(error <= 0)
        error = oyDeviceBackendCall( device, options );
    }

    if(error <= 0)
    {
      n = oyOptions_Count( device_->backend_core );
      for( i = 0; i < n; ++i )
      {
        o = oyOptions_Get( device_->backend_core, i );
        
        STRING_ADD( text, oyStrrchr_( oyOption_GetRegistration(o),
                          OY_SLASH_C ) + 1 );
        STRING_ADD( text, ":" );
        t = oyOption_GetValueText(o,oyAllocateFunc_);
        if(t)
        {
          STRING_ADD( text, t );
          oyDeAllocateFunc_(t); t = 0;
        }
        STRING_ADD( text, "\n" );

        oyOption_Release( &o );
      }
      n = oyOptions_Count( device_->data );
      if(n)
        STRING_ADD( text, "data:\n" );
      for( i = 0; i < n; ++i )
      {
        o = oyOptions_Get( device_->data, i );
        
        STRING_ADD( text, oyStrrchr_( oyOption_GetRegistration(o),
                          OY_SLASH_C ) + 1 );
        STRING_ADD( text, ":" );
        t = oyOption_GetValueText(o,oyAllocateFunc_);
        if(t)
        {
          if(strchr(t,'\n'))
            STRING_ADD( text, "\n" );
          STRING_ADD( text, t );
          oyDeAllocateFunc_(t); t = 0;
        }
        STRING_ADD( text, "\n" );

        oyOption_Release( &o );
      }
    }
    *info_text = oyStringCopy_( text, allocateFunc );
    oyFree_m_(text);
    return error;
  }


  if(error <= 0)
  {
    /* add "list" call to module arguments */
    error = oyOptions_SetRegistrationTextKey_( &options,
                                               device_->registration,
                                               "command", "list" );
  }

  if(error <= 0)
  {
    if(type == oyNAME_NAME)
    error = oyOptions_SetRegistrationTextKey_( &options,
                                               device_->registration,
                                               "oyNAME_NAME", "true" );
  }


  /** 1.2 ask each module */
  if(error <= 0)
    error = oyDeviceBackendCall( device, options );

  if(error <= 0 && device_->backend_core)
  {
    /** 1.2.1 add device_name to the string list */
    if(type == oyNAME_NAME)
      tmp = oyOptions_FindString( device_->data, "oyNAME_NAME", 0 );
  }

  *info_text = oyStringCopy_( tmp, allocateFunc );

  if(own_options)
    oyOptions_Release( &options );
  oyConfig_Release( &config );

  return error;
}

/** Function oyDeviceAskProfile2
 *  @brief   ask for the device profile
 *
 *  Ask for a profile associated with the device. A device capable to
 *  hold a profile. Only the held profile will be checked and returned.
 *  In case this profile is not found a "icc_profile" of oyVAL_STRUCT should be
 *  included.
 *
 *  The device might not be able to hold a profile, then just the DB profile
 *  will be returned from here without an issue. For interessted users, the
 *  source of the profile keeps transparent, as it can be checked if the
 *  device contains a "icc_profile" option which contains a oyProfile_s object.
 *
 *  @param[in]     device              the device
 *  @param[in]     options             additional options
 *  @param[out]    profile             the device's ICC profile
 *  @return                            0 - good, 1 >= error, -1 <= issue(s)
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/10 (Oyranos: 0.1.10)
 *  @date    2009/12/10
 */
OYAPI int  OYEXPORT
           oyDeviceAskProfile2       ( oyConfig_s        * device,
                                       oyOptions_s       * options,
                                       oyProfile_s      ** profile )
{
  int error = !device;
  oyOption_s * o = 0;
  oyConfig_s * s = device;
  int own_options = 0;
  oyProfile_s * p = 0;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 1 )


  if(!options)
  {
    options = oyOptions_New( 0 );
    own_options = 1;
    error = !options;
  }

  if(error <= 0)
  {
    /* add "list" call to module arguments */
    error = oyOptions_SetRegistrationTextKey_( &options,
                                               oyConfigPriv_m(device)->registration,
                                               "command", "list" );
  }

  if(error <= 0)
  {
    error = oyOptions_SetRegistrationTextKey_( &options,
                                                 oyConfigPriv_m(device)->registration,
                                                 "icc_profile", "true" );
  }

  if(error <= 0)
    error = oyDeviceBackendCall( device, options );

  /* The backend shows with the existence of the "icc_profile" response that it
   * can handle device profiles through the driver. */
  if(error <= 0)
    o = oyConfig_Find( device, "icc_profile" );

  p = (oyProfile_s*) oyOption_GetStruct( o, oyOBJECT_PROFILE_S );
  if(oyProfile_GetSignature( p, oySIGNATURE_MAGIC ) == icMagicNumber)
    *profile = p;
  else if(!error)
    error = -1;
  p = 0;

  /* The backend can not handle device driver profiles. Switch back to DB. */
  if(error <= 0 && !(*profile) && !o)
  {
    char * profile_name = 0;
    oyDeviceProfileFromDB( device, &profile_name, 0 );
    if(profile_name)
    {
      int32_t icc_profile_flags = 0;

      oyOptions_FindInt( options, "icc_profile_flags", 0, &icc_profile_flags );
      *profile = oyProfile_FromName( profile_name, icc_profile_flags, 0 );
      oyDeAllocateFunc_( profile_name );
    }
  }

  if(own_options)
    oyOptions_Release( &options );
  oyOption_Release( &o );

  return error;
}

/** Function oyDeviceSetProfile
 *  @brief   set the device profile
 *
 *  The function will lookup the device in the Oyranos device database
 *  and stores the given profile there.
 *
 *  To set a new profile und update the device please call the following
 *  sequence:
 *  @code
    // store new settings in the Oyranos data base
    oyDeviceSetProfile( device, profile );
    // remove any device entries
    oyDeviceUnset( device );
    // update the device from the newly added Oyranos data base settings
    oyDeviceSetup2( device, options );
    @endcode
 *
 *  @param         device              the device
 *  @param         scope               oySCOPE_USER and oySCOPE_SYS are possible
 *  @param         profile_name        the device's ICC profile or zero to
 *                                     unset
 *  @return                            error
 *
 *  @version Oyranos: 0.9.6
 *  @date    2015/08/03
 *  @since   2009/02/07 (Oyranos: 0.1.10)
 */
int      oyDeviceSetProfile          ( oyConfig_s        * device,
                                       oySCOPE_e           scope,
                                       const char        * profile_name )
{
  int error = !device || !profile_name || !profile_name[0];
  oyOption_s * od = 0;
  oyOptions_s * options = 0;
  oyConfigs_s * configs = 0;
  oyConfig_s * config = 0,
             * device_tmp = 0;
  oyProfile_s * p = 0;
  int i, j, n, j_n, equal;
  char * d_opt = 0;
  const char * device_name = 0,
             * o_val = 0,
             * d_val = 0;
  oyConfig_s * s = device;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 1 )

  if(error > 0)
  {
    WARNc1_S( "No profile argument provided. Give up. %s",
              oyNoEmptyString_m_(profile_name) );
    return error;
  }


  /** 1. obtain detailed and expensive device informations */
  if(oyOptions_Count( oyConfigPriv_m(device)->backend_core ) < 2)
  { 
    /** 1.1 add "properties" call to module arguments */
    error = oyOptions_SetRegistrationTextKey_( &options,
                                               oyConfigPriv_m(device)->registration,
                                               "command", "properties" );

    /** 1.2 call the device module */
    if(error <= 0)
      error = oyDeviceBackendCall( device, options );

    oyOptions_Release( &options );
  }

  if(error <= 0)
    error = (!oyOptions_Count( oyConfigPriv_m(device)->backend_core )) * -1;

  if(error <= 0)
  {
    device_name = oyConfig_FindString( device, "device_name", 0);
    error = (!device_name) * -1;
  }

  /** 2. check for success of device detection */
  if(error >= 1)
  {
    WARNc2_S( "%s: \"%s\"", _("Could not open device"), device_name );
    goto cleanup;
  }

  /** 3. load profile from file name argument */
  p = oyProfile_FromName( profile_name, 0, 0 );

  /** 3.1 check for success of profile loading */
  error = !p;
  if(error)
  {
    WARNc2_S( "%s: \"%s\"", _("Could not open profile"), profile_name );
    goto cleanup;
  }

  /** 4. Now remove all those DB configurations fully matching the selected
   *     device.  */
  if(error <= 0)
  {
    /** 4.1 get stored DB's configurations */
    error = oyConfigs_FromDB( oyConfigPriv_m(device)->registration, NULL, &configs, 0 );

    n = oyConfigs_Count( configs );
    for( i = 0; i < n; ++i )
    {
      config = oyConfigs_Get( configs, i );

      equal = 0;

      j_n = oyOptions_Count( oyConfigPriv_m(device)->backend_core );
      if(j_n)
        options = oyConfigPriv_m(device)->backend_core;
      else
      {
        j_n = oyOptions_Count( oyConfigPriv_m(device)->db );
        if(j_n)
          options = oyConfigPriv_m(device)->db;
      }

      for(j = 0; j < j_n; ++j)
      {
        od = oyOptions_Get( options, j );
        d_opt = oyFilterRegistrationToText( oyOption_GetRegistration(od),
                                            oyFILTER_REG_MAX, 0 );
        d_val = oyConfig_FindString( device, d_opt, 0 );

        o_val = oyConfig_FindString( config, d_opt, 0 );

        /** 4.1.1 compare if each device key matches to one configuration
         *          key */
        if( (d_val && o_val &&
             oyStrcmp_( d_val, o_val ) == 0 ) ||
            (!d_val && !o_val) )
          ++equal;
        else
          if(oyStrcmp_( d_opt, "profile_name") == 0)
            ++equal;

        oyOption_Release( &od );
        oyFree_m_( d_opt );
      }

      /** 4.1.2 if the 4.1.1 condition is true remove the configuration */
      if(equal == j_n)
        oyConfig_EraseFromDB( config, scope );

      oyConfig_Release( &config );
    }
    oyConfigs_Release( &configs );
  }

  /** 5. save the new configuration with a associated profile \n
   *  5.1 add the profile simply to the device configuration */
  if(error <= 0)
  {
    if( oyOptions_Count( *oyConfig_GetOptions( device, "data" ) ) ||
        oyOptions_Count( *oyConfig_GetOptions( device, "backend_core" ) ) )
      error = oyConfig_ClearDBData( device );
    error = oyConfig_AddDBData( device, "profile_name", profile_name,
                                OY_CREATE_NEW );
  }

  /** 5.2 save the configuration to DB (Elektra) */
  if(error <= 0)
  {
    char * json_text = NULL;
    oyConfig_s * json_device = NULL;
    error = oyDeviceToJSON( device, 0, &json_text, oyAllocateFunc_ );
    error = oyDeviceFromJSON( json_text, NULL, &json_device );
    error = oyConfig_SaveToDB( json_device, scope );
    oyFree_m_( json_text );
    oyConfig_Release( &json_device );
  }
  /** 5.3 reload the DB part */
  if(error <= 0)
    error = oyConfig_GetDB( device, NULL, 0 );

  cleanup:
  oyConfig_Release( &device_tmp );

  return error;
}

/** Function oyDeviceProfileFromDB
 *  @brief   look up a profile of a device from DB
 *
 *  The function asks the module for a detailed and possible expensive list
 *  of device information and tries to find a matching configuration in the
 *  DB. The device informations are the same as for saving to DB.
 *
 *  @param[in]     device          a device
 *  @param[in]     profile_name        profile's name in DB
 *  @param[in]     allocateFunc        user allocator
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/21 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
OYAPI int OYEXPORT oyDeviceProfileFromDB
                                     ( oyConfig_s        * device,
                                       char             ** profile_name,
                                       oyAlloc_f           allocateFunc )
{
  oyOption_s * o = 0;
  oyOptions_s * options = 0;
  int error = !device || !profile_name;
  const char * device_name = 0;
  char * tmp = 0, * tmp2 = 0;
  int32_t rank_value = 0;
  oyConfig_s * s = device;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 1 )

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  if(error <= 0)
  {
    o = oyConfig_Find( device, "profile_name" );
    device_name = oyConfig_FindString( device, "device_name", 0);

    /* 1. obtain detailed and expensive device informations */
    if( !oyConfig_FindString(s,"manufacturer",0) ||
        !oyConfig_FindString(s,"model",0) )
    { 
      /* 1.1 add "properties" call to module arguments */
      error = oyOptions_SetRegistrationTextKey_( &options,
                                          oyConfigPriv_m(device)->registration,
                                          "command", "properties" );
      error = oyOptions_SetRegistrationTextKey_( &options,
                                          oyConfigPriv_m(device)->registration,
                                          "device_name", device_name );

      device_name = 0;

      /* 1.2 get device */
      if(error <= 0)
        error = oyDeviceBackendCall( device, options );

      oyOptions_Release( &options );

      /* renew outdated string */
      o = oyConfig_Find( device, "profile_name" );
      device_name = oyConfig_FindString( device, "device_name", 0);
      oyOption_Release( &o );
    }

    if(!o)
    {
      error = oyConfig_GetDB( device, NULL, &rank_value );
      o = oyConfig_Find( device, "profile_name" );
    }

    if(!o)
    {
      o = oyOptions_Get( oyConfigPriv_m(device)->db, 0 );
      if(o)
        tmp = oyStringCopy_(oyOption_GetRegistration(o), oyAllocateFunc_);
      if(tmp && oyStrrchr_( tmp, OY_SLASH_C))
      {
        tmp2 = oyStrrchr_( tmp, OY_SLASH_C);
        tmp2[0] = 0;
      }
      WARNc3_S( "\n Could not get a \"profile_name\" from %s\n"
                " registration: \"%s\" rank: %d", 
                oyNoEmptyString_m_(device_name), oyNoEmptyString_m_(tmp),
                (int)rank_value )
      if(tmp)
        oyFree_m_(tmp); tmp2 = 0;
      oyOption_Release( &o );
      error = -1;
    } else if(!oyOption_GetValueString(o,0))
    {
      WARNc1_S( "Could not get \"profile_name\" data from %s", 
                oyNoEmptyString_m_(device_name) )
      error = -1;
    } else
      *profile_name = oyOption_GetValueText( o, allocateFunc );

  } else
    WARNc_S( "missed argument(s)" );

  oyOption_Release( &o );

  return error;
}

/** Function oyDeviceSelectSimiliar
 *  @brief   get similiar devices by a pattern from a list
 *
 *  The function takes a device and tries to find exact matches, which can be
 *  considered as belonging to the same device. The comparision can be 
 *  influenced by the flags.
 *  The option "profile_name" is ignored during the comparision.
 *
 *  @param[in]     pattern             Pass a device used as reference. String
 *                                     options of this object are compared to
 *                                     the objects in the heap argument
 *                                     depending on the flags argument.
 *                                     "profile_name" and other
 *                                     options from heap objects are ignored.
 *  @param[in]     heap                a list of device objects
 *  @param[in]     flags               - 0 yields exact match
 *                                     - 1 compare manufacturer model and serial
 *                                     - 2 compare only manufacturer and model
 *                                     - 4 compare only device_name
 *  @param[out]    matched_devices     the devices selected from heap
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/08/27 (Oyranos: 0.1.10)
 *  @date    2009/08/27
 */
OYAPI int OYEXPORT oyDeviceSelectSimiliar
                                     ( oyConfig_s        * pattern,
                                       oyConfigs_s       * heap,
                                       uint32_t            flags,
                                       oyConfigs_s      ** matched_devices )
{
  oyOption_s * odh = 0,
             * od = 0;
  int error  = !pattern || !matched_devices;
  char * od_key = 0;
  const char * od_val = 0,
             * odh_val = 0;
  oyConfig_s * s = pattern,
             * dh = 0;
  oyConfigs_s * matched = 0;
  int i,j,n,j_n;
  int match = 1;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 1 )

  if(error <= 0)
  {
    n = oyConfigs_Count( heap );

    /* Make shure the pattern has as well manufacturer, model included.
     * If not try a "properties" command. */
    if((flags == 0 || flags == 1 || flags == 2) &&
       (!oyConfig_FindString(s,"manufacturer",0) ||
        !oyConfig_FindString(s,"model",0)))
    {
      oyOptions_s * options = 0;
      error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/command",
                                   "properties", OY_CREATE_NEW );
      oyDeviceBackendCall( s, options );
      oyOptions_Release( &options );
    }

    if((flags == 1 || flags == 2) &&
       (!oyConfig_FindString(s,"manufacturer",0) ||
        !oyConfig_FindString(s,"model",0)))
    {
      return 0;
    }

    matched = oyConfigs_New( 0 );

    for(i = 0; i < n; ++i)
    {
      match = 0;
      dh = oyConfigs_Get( heap, i );

      j_n = oyConfig_Count( pattern );
      for(j = 0; j < j_n; ++j)
      {
        match = 1;
        od = oyConfig_Get( pattern, j );
        od_key = oyFilterRegistrationToText( oyOption_GetRegistration(od),
                                             oyFILTER_REG_MAX, 0);

        od_val = oyOption_GetValueString( od, 0 );
        if(!od_val)
          /* ignore non text options */
          continue;

        /* handle selective flags */
        if(flags == 4 &&
           oyStrcmp_(od_key,"device_name") != 0
          )
          continue;
        else
        if(flags == 2 &&
           oyStrcmp_(od_key,"manufacturer") != 0 &&
           oyStrcmp_(od_key,"model") != 0
          )
          continue;
        else
        if(flags == 1 &&
           oyStrcmp_(od_key,"manufacturer") != 0 &&
           oyStrcmp_(od_key,"model") != 0 &&
           oyStrcmp_(od_key,"serial") != 0
          )
          continue;

        /* ignore a "profile_name" option */
        if(oyStrcmp_(od_key,"profile_name") == 0)
          continue;

        odh = oyOptions_Find( oyConfigPriv_m(dh)->db, od_key, oyNAME_PATTERN );

        odh_val = oyOption_GetValueString( odh, 0 );
        if( !odh_val )
          /* ignore non text options */
          match = 0;

        if(match && oyStrcmp_( od_val, odh_val ) != 0)
          match = 0;

        /*printf("pruefe: %s=%s match = %d flags=%d\n", od_key, od_val, match, flags);*/


        oyOption_Release( &od );

        oyOption_Release( &odh );

        if(match == 0)
          break;
      }

      if(match)
        oyConfigs_MoveIn( matched, &dh, -1 );
      else
        oyConfig_Release( &dh );
    }

    if(oyConfigs_Count( matched ))
      *matched_devices = matched;
    else
      oyConfigs_Release( &matched );

  } else
    WARNc_S( "missed argument(s)" );

  return error;
}

/** Function oyDeviceFromJSON
 *  @brief   generate a device from a JSON device calibration
 *
 *  @param[in]    json_text            the device calibration
 *  @param[in]    options              optional
 *                                     - "underline_key_suffix" will be used as
 *                                       suffix for keys starting with underline
 *                                       '_'
 *                                     - "pos" integer selects position in array
 *  @param[out]   device               the device
 *  @return                            error
 *
 *  @version Oyranos: 0.9.6
 *  @date    2015/02/10
 *  @since   2011/08/21 (Oyranos: 0.3.2)
 */
OYAPI int  OYEXPORT oyDeviceFromJSON ( const char        * json_text,
                                       oyOptions_s       * options,
                                       oyConfig_s       ** device )
{
  int error = !json_text || !device;
  oyConfig_s * device_ = NULL;
  oyjl_val json = 0,
           json_device,
           json_class;
  char * val, * key = NULL, * t = NULL;
  const char * xpath = OY_STD "/device/%s/[%d]";
  int count, i;
  int32_t pos = 0;
  const char * underline_key_suffix = oyOptions_FindString( options,
                                                    "underline_key_suffix", 0 );
  const char * device_class = 0;
  oyjl_val v;

  if(error) return error;

  oyAllocHelper_m_(t, char, 256, 0, error = 1; return error );
  json = oyjlTreeParse( json_text, t, 256 );
  if(t[0])
    WARNc3_S( "%s: %s\n%s", _("found issues parsing JSON"), t, json_text );
  oyFree_m_(t);

  oyOptions_FindInt( options, "pos", 0, &pos );

  json_class = oyjlTreeGetValue( json, 0, "org/freedesktop/openicc/device" );
  if(json_class && json_class->type == oyjl_t_object)
    device_class = json_class->u.object.keys[0];

  if(device_class)
  /* set the registration string */
  {
    oyStringAddPrintf_( &t, oyAllocateFunc_, oyDeAllocateFunc_,
                        OY_STD "/device/%s", device_class );
    device_ = oyConfig_FromRegistration( t, 0 );
    oyConfig_AddDBData( device_, "device_class", device_class, OY_CREATE_NEW );
  } else
    WARNc1_S( "%s\n", _("device_class not found:") );

  if(t) oyFree_m_(t);

  oyStringAddPrintf_( &t, oyAllocateFunc_, oyDeAllocateFunc_,
                      xpath, device_class, pos );
  json_device = oyjlTreeGetValue( json, 0, t );

  if(!json_device)
    WARNc2_S( "\"%s\" %s\n", t,_("not found:") );
  oyFree_m_( t );
      
  count = oyjlValueCount(json_device);
  if(device_)
  for(i = 0; i < count; ++i)
  {
    if(json_device->type == oyjl_t_object)
      key = oyStringCopy_(json_device->u.object.keys[i], oyAllocateFunc_ );
    v = oyjlValuePosGet( json_device, i );
    val = oyjlValueText( v, oyAllocateFunc_ );

    if(key && key[0] && key[0] == '_' && underline_key_suffix)
    {
      t = 0;
      STRING_ADD( t, underline_key_suffix );
      STRING_ADD( t, key );
      oyFree_m_( key );
      key = t; t = 0;
    }

    /* ignore empty keys or values */
    if(key && val)
      oyConfig_AddDBData( device_, key, val, OY_CREATE_NEW );

    if(key) oyDeAllocateFunc_(key);
    if(val) oyDeAllocateFunc_(val);
  }

  *device = device_;
  device_ = NULL;
  oyjlTreeFree( json );

  return error;
}

#define OPENICC_DEVICE_JSON_HEADER \
  "{\n" \
  "  \"org\": {\n" \
  "    \"freedesktop\": {\n" \
  "      \"openicc\": {\n" \
  "        \"device\": {\n" \
  "          \"%s\": [{\n"
#define OPENICC_DEVICE_JSON_FOOTER \
  "            }\n" \
  "          ]\n" \
  "        }\n" \
  "      }\n" \
  "    }\n" \
  "  }\n" \
  "}\n"

/** Function oyDeviceToJSON
 *  @brief   get JSON format device calibration text from a device
 *
 *  @param[in]     device              the device
 *  @param[in]     options             "source" value is used for
 *                                     oyConfig_GetOptions()
 *  @param[out]    json_text           the device calibration
 *  @param[in]     allocateFunc        user allocator
 *  @return                            error
 *
 *  @version Oyranos: 0.3.2
 *  @since   2011/08/21 (Oyranos: 0.3.2)
 *  @date    2011/08/21
 */
OYAPI int OYEXPORT oyDeviceToJSON    ( oyConfig_s        * device,
                                       oyOptions_s       * options,
                                       char             ** json_text,
                                       oyAlloc_f           allocateFunc )
{
  int error = 0;
  oyOptions_s * opts = 0;
  int count, j, k;
  char * t = NULL; 
  char * value, * key;
  const char * device_class = NULL;
  oyConfDomain_s * domain;

  if(!error)
  {
      if(oyOptions_FindString( options, "source", 0 ))
        opts = *oyConfig_GetOptions( device,
                                 oyOptions_FindString( options, "source", 0 ));

      if(opts)
        count = oyOptions_Count( opts );
      else
        count = oyConfig_Count( device );

      if(count)
      {
        oyOption_s * opt;

        device_class = oyConfig_FindString( device, "device_class", 0 );
        if(!device_class)
        {
          if(opts)
            opt = oyOptions_Get( opts, 0 );
          else
            opt = oyConfig_Get( device, 0 );

          domain = oyConfDomain_FromReg( oyOption_GetRegistration( opt ), 0 );
          device_class = oyConfDomain_GetText( domain, "device_class", oyNAME_NICK );
          oyOption_Release( &opt );
          oyConfDomain_Release( &domain );
        }


        /* add device class */
        oyStringAddPrintf_( &t, oyAllocateFunc_, oyDeAllocateFunc_,
                            OPENICC_DEVICE_JSON_HEADER, device_class );

        /* add device and driver calibration properties */
        for(j = 0; j < count; ++j)
        {
          int vals_n = 0;
          char ** vals = 0, * val = 0;

          if(opts)
            opt = oyOptions_Get( opts, j );
          else
            opt = oyConfig_Get( device, j );

          key = oyFilterRegistrationToText( oyOption_GetRegistration( opt ),
                                            oyFILTER_REG_MAX, 0 );
          value = oyOption_GetValueText( opt, oyAllocateFunc_ );

          if(value && count > j)
          {
            char * escaped = oyjlJsonEscape( value, OYJL_QUOTE | OYJL_NO_BACKSLASH );
            if(j)
              oyStringAddPrintf_( &t, oyAllocateFunc_, oyDeAllocateFunc_,
                                  ",\n" );
            if(value[0] == '<')
               oyStringAddPrintf_( &t, oyAllocateFunc_, oyDeAllocateFunc_,
               "              \"%s\": \"%s\"",
                     key, escaped );
            else
            {
              /* split into a array with a useful delimiter */
              vals = oyStringSplit_( value, '?', &vals_n, malloc );
              if(vals_n > 1)
              {
                STRING_ADD( val, "              \"");
                STRING_ADD( val, key );
                STRING_ADD( val, ": [" );
                for(k = 0; k < vals_n; ++k)
                {
                  if(k != 0)
                  STRING_ADD( val, "," );
                  STRING_ADD( val, "\"" );
                  STRING_ADD( val, vals[k] );
                  STRING_ADD( val, "\"" );
                }
                STRING_ADD( val, "]");
                oyStringAddPrintf_( &t, oyAllocateFunc_, oyDeAllocateFunc_,
                     "%s", val );
                if(val) oyDeAllocateFunc_( val );
              } else
                oyStringAddPrintf_( &t, oyAllocateFunc_, oyDeAllocateFunc_,
                     "              \"%s\": \"%s\"",
                     key, escaped );

              oyStringListRelease_( &vals, vals_n, free );
            }
            if(escaped)
              oyFree_m_( escaped );
          }
          oyOption_Release( &opt );
          if(key)
            oyFree_m_( key );
          if(value)
            oyFree_m_( value );
        }

        oyStringAddPrintf_( &t, oyAllocateFunc_, oyDeAllocateFunc_,
                            "\n"OPENICC_DEVICE_JSON_FOOTER );
      }
      else
        error = -1;

      if(json_text && t)
      {
        *json_text = oyStringCopy_( t, allocateFunc );
        oyFree_m_( t );
      }
  }

  return error;
}

/**
 *  @brief    Check for matching to a given pattern
 *
 *  @param[in]     module_device       the to be checked configuration from
 *                                     oyConfigs_FromPattern_f;
 *                                     Additional allowed are DB configs.
 *  @param[in]     db_pattern          the to be compared configuration from
 *                                     elsewhere, e.g. ICC dict tag
 *  @param[out]    rank_value          the number of matches between config and
 *                                     pattern, -1 means invalid
 *  @return                            0 - good, >= 1 - error + a message should
 *                                     be sent
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/01/05
 *  @since   2009/01/26 (Oyranos: 0.1.10)
 */
OYAPI int OYEXPORT oyDeviceCompare   ( oyConfig_s        * module_device,
                                       oyConfig_s        * db_pattern,
                                       int32_t           * rank_value )
{
  return oyConfig_Match( module_device, db_pattern, '/', ',', 0, rank_value );
}


#define OPENICC_DEVICE_JSON_HEADER_BASE \
  "{\n" \
  "  \"org\": {\n" \
  "    \"freedesktop\": {\n" \
  "      \"openicc\": {\n" \
  "        \"device\": {\n" \
  "          \"%s\":\n"
#define OPENICC_DEVICE_JSON_FOOTER_BASE \
  "        }\n" \
  "      }\n" \
  "    }\n" \
  "  }\n" \
  "}\n"

int   oyCompareRanks_                ( const void       * rank1,
                                       const void       * rank2 )
{const int32_t *r1=(int32_t*)rank1, *r2=(int32_t*)rank2; if(r1[1] < r2[1]) return 1; else return 0;}

/** Function  oyDevicesFromTaxiDB
 *  @brief    search a calibration state in the taxi DB for a device
 *
 *  oyDevicesFromTaxiDB() needs a device containing the calibration state and
 *  gives you a list of found devices in Taxi DB. You can extract the
 *  device ID inside the "TAXI_id" string from the returned devices "db"
 *  options sets.
 *  Typical you want one entry in the profiles list assigned with that
 *  device. oyProfile_FromTaxiDB() can be used to download that and
 *  oyProfile_Install() helps installing it, while doing some consistency
 *  checks.
 *
 *  @include tutorial_taxi.c
 *
 *  @param[in]     device              the device
 *  @param[in]     options             not used
 *  @param[out]    devices             the obtained device calibrations
 *  @param         obj                 the optional object
 *  @return                            0 - good, >= 1 - error + a message should
 *                                     be sent
 *
 *  @version Oyranos: 0.9.1
 *  @since   2011/09/14 (Oyranos: 0.3.2)
 *  @date    2012/11/15
 */
OYAPI int  OYEXPORT
             oyDevicesFromTaxiDB     ( oyConfig_s        * device,
                                       oyOptions_s       * options OY_UNUSED,
                                       oyConfigs_s      ** devices,
                                       oyObject_s          obj )
{
  int error = 0;
  oyConfigs_s * configs_ = 0,
              * taxi_devices = 0;
  oyConfig_s_ * s = (oyConfig_s_*)device;
  char * manufacturers;
  size_t size = 0;
  int n;
  const char * short_name = NULL,
             * long_name = NULL,
             * name = NULL;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  error = oyDeviceCheckProperties ( device );


  manufacturers = oyReadUrlToMem_( "https://icc.opensuse.org/manufacturers",
                                   &size, "r", oyAllocateFunc_ );

  
  if(manufacturers)
  {
    oyjl_val root = 0;
    int count = 0, i;
    char * val = NULL,
         * key = NULL;
    char * json_text = NULL;
    const char * prefix = oyConfig_FindString( device, "prefix", 0 );

    char * t = oyAllocateFunc_(256);
    root = oyjlTreeParse( manufacturers, t, 256 );
    if(t[0])
      WARNc2_S( "%s: %s\n", _("found issues parsing JSON"), t );
    oyFree_m_(t);

    if(prefix)
      oyStringAddPrintf_( &key, oyAllocateFunc_, oyDeAllocateFunc_,
                          "%smnft", prefix );
    else
      oyStringAddPrintf_( &key, oyAllocateFunc_, oyDeAllocateFunc_,
                          "mnft" );
    name = short_name = oyConfig_FindString( device, key, 0 );
    oyFree_m_(key);
    if(prefix)
      oyStringAddPrintf_( &key, oyAllocateFunc_, oyDeAllocateFunc_,
                          "%smanufacturer", prefix );
    else
      oyStringAddPrintf_( &key, oyAllocateFunc_, oyDeAllocateFunc_,
                          "manufacturer" );
    if(!short_name)
      name = long_name = oyConfig_FindString( device, key, 0 );
    oyFree_m_(key);

    if(oy_debug)
      WARNc1_S("manufacturers:\n%s", manufacturers);

    if(root)
    {
      int done = 0;
      oyjl_val v = 0, tv = 0;

      count = oyjlValueCount(root);
      for(i = 0; i < count; ++i)
      {
        if(short_name)
          v = oyjlTreeGetValueF( root, 0,
                              "[%d]/short_name", i );
        else if(long_name)
          v = oyjlTreeGetValueF( root, 0, 
                              "[%d]/longName", i );

        val = oyjlValueText( v, oyAllocateFunc_ );
        if( val && name && strcmp( val, name) == 0 )
          done = 1;
        else
          DBG_NUM2_S("could not find device:\n%s %s",
                   oyNoEmptyString_m_(name), oyNoEmptyString_m_(val));

        if(done) break;
        if(val) { oyDeAllocateFunc_(val); val = 0; }
      }

      oyjlTreeFree( root ); root = 0;

      /* get the devices */
      if(done)
      {
        char * device_db, * t = NULL;
        oyOptions_s * opts = NULL;
        oyConfig_s * dev;

        /* put a cloak around the bare meta data, so it behaves like OpenICC
         * JSON */
        oyStringAddPrintf_( &t, oyAllocateFunc_, oyDeAllocateFunc_,
                            OPENICC_DEVICE_JSON_HEADER_BASE, "dummy" );

        /* the device DB JSON contains all device meta data for one
         * mnft / manufacturer */
        device_db = oyReadUrlToMemf_( &size, "r", oyAllocateFunc_,
                            "https://icc.opensuse.org/devices/%s", val );
        STRING_ADD( t, device_db );
        oyFree_m_( device_db );

        oyStringAddPrintf_( &t, oyAllocateFunc_, oyDeAllocateFunc_,
                            "\n"OPENICC_DEVICE_JSON_FOOTER_BASE );
        device_db = t; t = NULL;

        if(oy_debug)
          oyMessageFunc_p( oyMSG_DBG,(oyStruct_s*)device,
                       OY_DBG_FORMAT_
                       "https://icc.opensuse.org/devices/%s with header:\n%s",
                       OY_DBG_ARGS_,
                       val, oyNoEmptyString_m_(device_db) );
        t = oyAllocateFunc_(256);
        root = oyjlTreeParse( device_db, t, 256 );
        if(t[0])
          WARNc2_S( "%s: %s\n", _("found issues parsing JSON"), t );
        oyFree_m_(t);

        error = oyOptions_SetFromString( &opts,
                                 "//" OY_TYPE_STD "/argv/underline_key_suffix",
                                 "TAXI", OY_CREATE_NEW );

        tv = oyjlTreeGetValueF( root, 0, "org/freedesktop/openicc/device/[0]" );
        count = oyjlValueCount(tv);
        for(i = 0; i < count; ++i)
        {
          error = oyOptions_SetFromInt( &opts,
                                 "//" OY_TYPE_STD "/argv/pos",
                                 i, 0, OY_CREATE_NEW );

          v = oyjlTreeGetValueF( root, 0, "org/freedesktop/openicc/device/[0]/[%d]/_id/$oid", i );
          val = oyjlValueText( v, oyAllocateFunc_ );
          error = oyDeviceFromJSON( device_db, opts, &dev );

          if(dev)
          {
            int j,n;
            oyConfig_AddDBData( dev, "TAXI_id", val, OY_CREATE_NEW );
            if(val) { oyDeAllocateFunc_(val); val = 0; }

            v = oyjlTreeGetValueF( root, 0, "org/freedesktop/openicc/device/[0]/[%d]/profile_description", i );
            n = oyjlValueCount(v);
            for(j = 0; j < n; ++j)
            {
              v = oyjlTreeGetValueF( root, 0, "org/freedesktop/openicc/device/[0]/[%d]/profile_description/[%d]", i, j );
              val = oyjlValueText( v, oyAllocateFunc_ );
              oyConfig_AddDBData( dev, "TAXI_profile_description", val, OY_CREATE_NEW );
              if(val) { oyDeAllocateFunc_(val); val = 0; }
              /* TODO store all profile descriptions */
              break;
            }

            if(!configs_)
              configs_ = oyConfigs_New(obj);
            oyConfigs_MoveIn( configs_, &dev, -1 );
          }

          if(val) { oyDeAllocateFunc_(val); val = 0; }
          if(json_text) oyFree_m_( json_text );
        }
        oyOptions_Release( &opts );
        if(device_db) { oyDeAllocateFunc_(device_db); device_db = 0; }
      }
      oyjlTreeFree( root ); root = 0;
    }

    oyFree_m_( manufacturers );

    if(!configs_)
    {
      oyDeviceToJSON( device, 0, &json_text, oyAllocateFunc_ );
      WARNc1_S("no profile found for\n%s", json_text);
      oyFree_m_(json_text);
    } else if(oy_debug)
    {
      count = oyConfigs_Count( configs_ );
      for( i = 0; i < count; ++i)
        WARNc1_S("%d", i);
    }

    n = oyConfigs_Count( configs_ );

    /* sort the devices by rank value */
    if(n)
    {
      oyConfig_s * taxi_dev;
      int32_t * ranks = 0;

      taxi_devices = oyConfigs_New(0);

      oyAllocHelper_m_(ranks, int32_t, n*2+1, 0, error = 1; return error );
      for(i = 0; i < n; ++i)
      {
        taxi_dev = oyConfigs_Get( configs_, i );
        ranks[2*i+0] = i;
        error = oyDeviceCompare( device, taxi_dev, &ranks[2*i+1] );

        oyConfig_Release( &taxi_dev );
      }
      qsort( ranks, n, sizeof(int32_t)*2, oyCompareRanks_ );
      for(i = 0; i < n; ++i)
      {
        taxi_dev = oyConfigs_Get( configs_, ranks[2*i+0] );

        if(oy_debug > 2)
        {
          char * json_text = 0;
          oyDeviceToJSON( taxi_dev, 0, &json_text, oyAllocateFunc_ );
          DBG_NUM1_S("%s\n", json_text );
          oyFree_m_(json_text);
        }

        oyConfigs_MoveIn( taxi_devices, &taxi_dev, -1 );
      }
      oyConfigs_Release( &configs_ );
      oyFree_m_(ranks);
    }

    if(devices)
      *devices = taxi_devices;
  }

  return error;
}


/** @} *//* devices_handling */


/** Function oyOptions_ForFilter
 *  @memberof oyOptions_s
 *  @brief   provide Oyranos behaviour settings
 *
 *  The returned options are read in from the Elektra settings and if thats not
 *  available from the inbuild defaults. The later can explicitely selected with
 *  oyOPTIONSOURCE_FILTER passed as flags argument.
 *  The key names map to the registration and XML syntax.
 *
 *  To obtain all advanced front end options from a meta module use:@code
 *  flags = oyOPTIONATTRIBUTE_ADVANCED |
 *          oyOPTIONATTRIBUTE_FRONT |
 *          OY_SELECT_COMMON @endcode
 *
 *  @see OY_SELECT_FILTER OY_SELECT_COMMON oyOPTIONATTRIBUTE_e
 *
 *  @param[in]     registration        the filter registration to search for
 *  @param[in]     flags               select particular options:
 *                                     - for inbuild defaults
 *                                       @ref oyOPTIONSOURCE_FILTER
 *                                     - for options marked as advanced
 *                                       @ref oyOPTIONATTRIBUTE_ADVANCED
 *                                     - use inbuild options from filter type
 *                                       @ref OY_SELECT_FILTER
 *                                     - cover policy options
 *                                       @ref OY_SELECT_COMMON
 *  @param         object              the optional object
 *  @return                            the options
 *
 *  @version Oyranos: 0.9.6
 *  @date    2015/01/26
 *  @since   2008/10/08 (Oyranos: 0.1.8)
 */
oyOptions_s *  oyOptions_ForFilter   ( const char        * registration,
                                       uint32_t            flags,
                                       oyObject_s          object )
{
  oyOptions_s * s = 0;
  oyFilterCore_s_ * filter = 0;
  oyCMMapi4_s_ * cmm_api4 = 0;
  char * lib_name = 0;
  int error = 0;

  /*  1. get filter */
  filter = oyFilterCore_New_( object );

  error = !filter;

  if(error <= 0)
    cmm_api4 = (oyCMMapi4_s_*) oyCMMsGetFilterApi_( registration,
                                                    oyOBJECT_CMM_API4_S );

  if(cmm_api4)
    lib_name = cmm_api4->id_;

  error = !(cmm_api4 && lib_name);

  if(error <= 0)
    error = oyFilterCore_SetCMMapi4_( filter, cmm_api4 );

  s = oyOptions_ForFilter_( filter, NULL, flags, object);

  oyFilterCore_Release( (oyFilterCore_s**)&filter );

  return s;
}




/** Function  oyOptions_ForFilter_
 *  @memberof oyOptions_s
 *  @brief    Provide Oyranos behaviour settings
 *  @internal
 *
 *  The returned options are read in from the Elektra settings and if thats not
 *  available from the inbuild defaults. The later can explicitely selected with
 *  oyOPTIONSOURCE_FILTER passed as flags argument.
 *  The key names map to the registration and XML syntax.
 *
 *  To obtain all front end options from a meta module use: @code
    flags = oyOPTIONATTRIBUTE_ADVANCED |
            oyOPTIONATTRIBUTE_FRONT |
            OY_SELECT_COMMON @endcode
 *
 *  @param[in]     core                the filter core
 *  @param[in]     node                the filter node; optional
 *  @param[in]     flags               select particular options:
 *                                     - for inbuild defaults
 *                                       @ref oyOPTIONSOURCE_FILTER
 *                                     - for options marked as advanced
 *                                       @ref oyOPTIONATTRIBUTE_ADVANCED
 *                                     - use inbuild options from filter type
 *                                       @ref OY_SELECT_FILTER
 *                                     - cover policy options
 *                                       @ref OY_SELECT_COMMON
 *  @param         object              the optional object
 *  @return                            the options
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/10/24
 *  @since   2008/12/08 (Oyranos: 0.1.9)
 */
oyOptions_s *  oyOptions_ForFilter_  ( oyFilterCore_s_   * core,
                                       oyFilterNode_s_   * node,
                                       uint32_t            flags,
                                       oyObject_s          object )
{
  oyOptions_s * s = 0,
              * opts_tmp = 0,
              * opts_tmp2 = 0;
  oyOption_s * o = NULL, * c = NULL, * r = NULL;
  int error = !core || !core->api4_;
  char * type_txt = NULL,
       * renderer = NULL,
       * context = NULL;
  oyCMMapi5_s * api5 = 0;
  int i,n;

  if(!error)
    type_txt = oyFilterRegistrationToText( core->registration_,
                                           oyFILTER_REG_TYPE, 0 );

  /* by default we parse both sources */
  if(!(flags & OY_SELECT_FILTER) && !(flags & OY_SELECT_COMMON))
    flags |= OY_SELECT_FILTER | OY_SELECT_COMMON;

  if(!error)
  {
    /*
        Programm:
        1. get filter and its type
        2. get implementation for filter type
        3. parse static common options from meta module
        4. parse static options from filter
        5. merge both
        6. get stored values from disk
     */

    /*  1. get filter */

    /*  2. get implementation for filter type */
    api5 = (oyCMMapi5_s*)core->api4_->api5_;

    /*  3. parse static common options from a policy module */
    if(api5 && flags & OY_SELECT_COMMON)
    {
      oyCMMapiFilters_s * apis;
      int apis_n = 0;
      oyCMMapi9_s_ * cmm_api9_ = 0;
      char * klass, * api_reg;

      klass = oyFilterRegistrationToText( core->registration_,
                                          oyFILTER_REG_TYPE, 0 );
      api_reg = oyStringCopy_("//", oyAllocateFunc_ );
      STRING_ADD( api_reg, klass );
      oyFree_m_( klass );

      s = oyOptions_New( 0 );
      apis = oyCMMsGetFilterApis_( api_reg,
                                   oyOBJECT_CMM_API9_S,
                                   oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR,
                                   0,0);
      apis_n = oyCMMapiFilters_Count( apis );
      for(i = 0; i < apis_n; ++i)
      {
        cmm_api9_ = (oyCMMapi9_s_*) oyCMMapiFilters_Get( apis, i );
        if(oyFilterRegistrationMatch( core->registration_, cmm_api9_->pattern,
                                      oyOBJECT_NONE ))
        {
          char * api_pattern = NULL,
               * key_name = NULL;
          int select_core = 1, is_new = 0;

          opts_tmp = oyOptions_FromText( cmm_api9_->options, 0, object );

          /*  3.1. set the "context" and "renderer" options */
          key_name = oyGetFilterNodeKey( cmm_api9_->key_base, select_core );
          o = oyOptions_Find( opts_tmp, key_name, oyNAME_PATTERN );
          oyFree_m_(key_name);
          if(!o)
          {
            o = oyOption_New( NULL );
            oyOption_SetRegistration( o, OY_DEFAULT_CMM_CONTEXT );
            is_new = 1;
          }
          api_pattern = cmm_api9_->oyCMMRegistrationToName(
                                 core->api4_->registration,
                                 oyNAME_PATTERN, 0, select_core, oyAllocateFunc_ );
          oyOption_SetFromString( o, api_pattern, 0 );
          oyFree_m_( api_pattern );
          if(is_new)
            oyOptions_MoveIn( opts_tmp, &o, -1 );
          else
            oyOption_Release( &o );
          is_new = 0;

          select_core = 0;
          if(node)
            api_pattern = cmm_api9_->oyCMMRegistrationToName(
                                 node->api7_->registration,
                                 oyNAME_PATTERN, 0, select_core, oyAllocateFunc_ );
          else
            if(cmm_api9_->oyCMMGetDefaultPattern)
              api_pattern = cmm_api9_->oyCMMGetDefaultPattern( cmm_api9_->pattern, 0, select_core,
                                                       oyAllocateFunc_ );
          key_name = oyGetFilterNodeKey( cmm_api9_->key_base, select_core );
          o = oyOptions_Find( opts_tmp, key_name, oyNAME_PATTERN );
          oyFree_m_( key_name );
          if(!o)
          {
            o = oyOption_New( NULL );
            oyOption_SetRegistration( o, OY_DEFAULT_CMM_RENDERER );
            is_new = 1;
          }
          oyOption_SetFromString( o, api_pattern, 0 );
          oyFree_m_( api_pattern );
          if(is_new)
            oyOptions_MoveIn( opts_tmp, &o, -1 );
          else
            oyOption_Release( &o );
          is_new = 0;

          oyOptions_AppendOpts( s, opts_tmp );
          oyOptions_Release( &opts_tmp );
        }
        if(cmm_api9_->release)
          cmm_api9_->release( (oyStruct_s**)&cmm_api9_ );
      }
      oyCMMapiFilters_Release( &apis );
      oyFree_m_( api_reg );
      opts_tmp = s; s = NULL;
    }
    /* requires step 2 */

    /*  4. parse static options from filter */
    if(flags & OY_SELECT_FILTER)
      opts_tmp2 = oyOptions_FromText( core->api4_->ui->options, 0, object );

    /*  5. merge */
    s = oyOptions_FromBoolean( opts_tmp, opts_tmp2, oyBOOLEAN_UNION, object );

    oyOptions_Release( &opts_tmp );
    oyOptions_Release( &opts_tmp2 );

    /*  6. get stored values */
    n = oyOptions_Count( s );
    for(i = 0; i < n && error <= 0; ++i)
    {
      o = oyOptions_Get( s, i );
      oyOption_SetSource( o, oyOPTIONSOURCE_FILTER );
      oyOption_SetFlags(o, oyOption_GetFlags(o) & (~oyOPTIONATTRIBUTE_EDIT));
      oyOption_Release( &o );
    }

    c = oyOptions_Find( s, "////context", oyNAME_PATTERN );
    if(c)
      context = oyStringCopy( oyOption_GetValueString( c, 0 ),
                              oyAllocateFunc_ );
    r = oyOptions_Find( s, "////renderer", oyNAME_PATTERN );
    if(r)
      renderer= oyStringCopy( oyOption_GetValueString( r, 0 ),
                              oyAllocateFunc_ );

    error = oyOptions_DoFilter ( s, flags, type_txt );

    if(c && context)
    {
      oyOption_SetFromString( c, context, 0 );
      oyOption_SetFlags(c, oyOption_GetFlags(c) & (~oyOPTIONATTRIBUTE_EDIT));
      oyMessageFunc_p( oyMSG_DBG,(oyStruct_s*)c,
                       OY_DBG_FORMAT_ "set context to %s %d", OY_DBG_ARGS_, context, oyObject_GetId(c->oy_) );
    }
    if(r && renderer)
    {
      oyOption_SetFromString( r, renderer, 0 );
      oyOption_SetFlags(r, oyOption_GetFlags(r) & (~oyOPTIONATTRIBUTE_EDIT));
    }

    if(context)
      oyFree_m_( context );
    if(renderer)
      oyFree_m_( renderer );
  }

  oyOption_Release( &c );
  oyOption_Release( &r );

  if(type_txt)
    oyFree_m_( type_txt );

  return s;
}

/** Function oyOption_FromDB
 *  @memberof oyOption_s
 *  @brief   new option with registration and value filled from DB if available
 *
 *  @param         registration        no or full qualified registration
 *  @param[out]    option              the result
 *  @param         object              the optional object
 *  @return                            0 - good, 1 >= error, -1 <= issue(s)
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/12/28
 *  @since   2009/01/24 (Oyranos: 0.1.10)
 */
OYAPI int  OYEXPORT  oyOption_FromDB ( const char        * registration,
                                       oyOption_s       ** option,
                                       oyObject_s          object )
{
  int error = !registration || !option;
  oyOption_s * o = NULL;

  if(error <= 0)
  {
    /** This is merely a wrapper to oyOption_New() and
     *  oyOption_SetValueFromDB(). */
    o = oyOption_FromRegistration( registration, object );
    error = oyOption_SetFromString( o, 0, 0 );
    error = oyOption_SetValueFromDB( o );
    if(error)
      oyOption_Release( &o );
    else
      oyOption_SetSource( o, oyOPTIONSOURCE_DATA );

    *option = o;
  }

  return o != NULL ? 0 : -1;
}


/** Function oyOptions_SaveToDB
 *  @memberof oyOptions_s
 *  @brief   store a oyOptions_s in DB
 *
 *  @param[in]     options             the options
 *  @param         scope               oySCOPE_USER and oySCOPE_SYS are possible
 *  @param[in]     registration        the registration
 *  @param[out]    new_reg             the new registration; optional
 *  @param[in]     allocateFunc        the user allocator for new_reg; optional
 *  @return                            0 - good, 1 >= error
 *
 *  @version Oyranos: 0.9.6
 *  @date    2015/02/07
 *  @since   2009/02/08 (Oyranos: 0.1.10)
 */
OYAPI int OYEXPORT oyOptions_SaveToDB( oyOptions_s       * options,
                                       oySCOPE_e           scope,
                                       const char        * registration,
                                       char             ** new_reg,
                                       oyAlloc_f           allocateFunc )
{
  int error = !options || !registration;
  oyOption_s * o = 0;
  int n,i;
  char * key_base_name = 0,
       * key_name = 0,
       * key_top = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  if(error <= 0)
  {
    key_base_name = oyDBSearchEmptyKeyname( registration, scope );
    error = !key_base_name;
    if(error <= 0)
    {
      STRING_ADD( key_base_name, OY_SLASH );
    }

    n = oyOptions_Count( options );
    for( i = 0; i < n; ++i )
    {
      o = oyOptions_Get( options, i );
      key_top = oyFilterRegistrationToText( oyOption_GetRegistration(o),
                                            oyFILTER_REG_MAX, 0 );


      STRING_ADD( key_name, key_base_name );
      STRING_ADD( key_name, key_top );
      if(oyOption_GetValueString(o,0))
        error = oySetPersistentString( key_name,
        /* The key_name is already scope'd by oyDBSearchEmptyKeyname().
         * So we skip creating again a scope prefix. */
                                       oySCOPE_USER_SYS, 
                                       oyOption_GetValueString(o,0), 0 );
# if 0
      else if(o->value_type == oyVAL_STRUCT &&
              o->value && o->value->oy_struct->type_ == oyOBJECT_BLOB_S)
        error = 0;/*oyAddKeyBlobComment_();*/
#endif
      else
        WARNcc_S( (oyStruct_s*)o,
                    "Could not save non string / non binary option" );

      oyOption_Release( &o );
      oyFree_m_( key_name );
    }

    if(error <= 0 && new_reg && key_base_name)
    {
      key_base_name[strlen(key_base_name)-1] = '\000';
      *new_reg = oyStringCopy_(key_base_name, allocateFunc);
    }
    oyFree_m_( key_base_name );
  }

  oyExportEnd_();
  DBG_PROG_ENDE
  return error;
}

/** Function  oyOption_SetValueFromDB
 *  @memberof oyOption_s
 *  @brief    Value filled from DB if available
 *
 *  @param         option              the option
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/05/25
 *  @since   2009/01/24 (Oyranos: 0.1.10)
 */
int            oyOption_SetValueFromDB  ( oyOption_s        * option )
{
  int error = !option || !oyOption_GetRegistration(option);
  char * text = 0;
  oyOption_s * s = option;

  if(error)
    return error;

  oyCheckType__m( oyOBJECT_OPTION_S, return 1 )

  oyExportStart_(EXPORT_SETTING);

  if(error <= 0)
    text = oyGetPersistentString( oyOption_GetRegistration( option ),
                                  0, oySCOPE_USER_SYS, 0 );

  if(error <= 0)
  {
    /** Change the option value only if something was found in the DB. */
    if(text && text[0])
    {
      oyOption_SetFromString( option, text, 0 );
      oyOption_SetSource( s, oyOPTIONSOURCE_DATA );
    } else
      error = -1;
  }

  if(text)
    oyFree_m_( text );

  oyExportEnd_();

  return error;
}

/** Function oyOptions_DoFilter
 *  @memberof oyOptions_s
 *  @brief   filter the options
 *
 *  The returned options are read in from the Elektra settings and if thats not
 *  available from the inbuild defaults. The later can explicitely selected with
 *  oyOPTIONSOURCE_FILTER passed as flags argument. advanced options can be 
 *  filtered out by adding oyOPTIONATTRIBUTE_ADVANCED.
 *
 *  Modules should handle the advanced options as well but shall normally
 *  not act upon them. The convention to set them zero, keeps them inactive.
 *  
 *  On the front end side the CMM cache has to include them, as they will 
 *  influence the hash sum generation. The question arrises, whether to include
 *  these options marked as non visible along the path or require the CMM cache
 *  code to check each time for them on cache lookup. The oyOption_s::flags
 *  is already in place. So we use it and do inclusion. Front end options can be
 *  filtered as they do not affect the CMM cache.
 *
 *  @see oyOptions_Add
 *
 *  @param         opts                the options
 *  @param[in]     flags               select particular options:
 *                                     - for inbuild defaults
 *                                       @ref oyOPTIONSOURCE_FILTER
 *                                     - for options marked as advanced
 *                                       @ref oyOPTIONATTRIBUTE_ADVANCED
 *                                     - for front end options
 *                                       @ref oyOPTIONATTRIBUTE_FRONT
 *                                     - for already edited options |
 *                                       @ref oyOPTIONATTRIBUTE_EDIT
 *  @param         filter_type         the type level from a registration
 *  @return                            options
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/06/30
 *  @since   2008/11/27 (Oyranos: 0.1.9)
 */
int          oyOptions_DoFilter      ( oyOptions_s       * opts,
                                       uint32_t            flags,
                                       const char        * filter_type )
{
  oyOptions_s * opts_tmp = 0;
  oyOption_s * o = 0, * db_opt = 0;
  int error = !opts;
  char * text;
  int i,n;
  char ** db_keys = NULL;
  int db_keys_n = 0;

  oyExportStart_(EXPORT_SETTING);
  oyExportEnd_();

  if(error <= 0 && (flags || filter_type))
  {
    /*  6. get stored values */
    n = oyOptions_Count( opts );
    opts_tmp = oyOptions_New(0);
    for(i = 0; i < n; ++i)
    {
      int skip = 0;
      const char * reg;

      o = oyOptions_Get( opts, i );
      reg = oyOption_GetRegistration(o);

      /* usage/type range filter */
      if(filter_type)
      {
        text = oyFilterRegistrationToText( reg, oyFILTER_REG_TYPE, 0);
        if(oyStrcmp_( filter_type, text ) != 0)
          skip = 1;

        oyFree_m_( text );
      }

      /* front end options filter */
      if(!skip && !(flags & oyOPTIONATTRIBUTE_FRONT))
      {
        text = oyStrrchr_( reg, '/' );

        if(text)
           text = oyStrchr_( text, '.' );
        if(text)
          if(oyStrstr_( text, "front" ))
            skip = 1;
      }

      /* advanced options filter */
      if(!skip && !(flags & oyOPTIONATTRIBUTE_ADVANCED))
      {
        text = oyStrrchr_( reg, '/' );
        if(text)
           text = oyStrchr_( text, '.' );
        if(text)
          if(oyStrstr_( text, "advanced" ))
            skip = 1;
      }

      /* Elektra settings, modify value */
      if(!skip && !(flags & oyOPTIONSOURCE_FILTER))
      {
        int oflags = oyOption_GetFlags(o);
        if((flags & oyOPTIONATTRIBUTE_EDIT) ||
           /* skip already edited options by default */
           !(oflags & oyOPTIONATTRIBUTE_EDIT))
          /* remember the DB requests */
          oyStringListAddStaticString( &db_keys,
                                         &db_keys_n,
                                         oyOption_GetText( o,oyNAME_DESCRIPTION),
                                         oyAllocateFunc_,
                                         oyDeAllocateFunc_ );
      }

      if(!skip)
        oyOptions_Add( opts_tmp, o, -1, opts->oy_ );

      oyOption_Release( &o );
    }

    n = oyOptions_Count( opts_tmp );
    error = oyOptions_Clear(opts);
    for( i = 0; i < n && !error; ++i )
    {
      o = oyOptions_Get( opts_tmp, i );
      error = oyOptions_MoveIn( opts, &o, -1 );
    }
    oyOptions_Release( &opts_tmp );

          /* ask the DB */
    for( i = 0; i < db_keys_n && !error; ++i )
    {
      uint32_t flags;
      const char * db_key = db_keys[i], * db_opt_desc, * db_opt_val;
      oyOption_FromDB( db_key, &db_opt, NULL );
      db_opt_desc = oyOption_GetText( db_opt, oyNAME_DESCRIPTION );
      o = oyOptions_Find( opts, db_opt_desc, oyNAME_PATTERN);
      oyOption_SetSource( o, oyOPTIONSOURCE_DATA );
      flags = oyOption_GetFlags(o);
      db_opt_val = oyOption_GetValueString( db_opt,0 );
      oyOption_SetFromString( o, db_opt_val, 0 );
      if(!(flags & oyOPTIONATTRIBUTE_EDIT))
        oyOption_SetFlags(o, flags & (~oyOPTIONATTRIBUTE_EDIT));
      oyOption_Release( &o );
      oyOption_Release( &db_opt );
    }
    oyStringListRelease_( &db_keys, db_keys_n, oyDeAllocateFunc_ );
  }

  return error;
}


/** Function  oyFilterNode_GetOptions
 *  @memberof oyFilterNode_s
 *  @brief    Get filter options
 *
 *  Options are typical user visible settings, which can be presistently stored
 *  as user preferences and can be displayed in filter dialogs.
 *
 *  @param[in,out] node                filter object
 *  @param         flags               see oyOptions_s::oyOptions_ForFilter()
 *  @return                            the options
 *
 *  @version Oyranos: 0.5.0
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 *  @date    2012/06/12
 */
oyOptions_s* oyFilterNode_GetOptions ( oyFilterNode_s    * node,
                                       int                 flags )
{
  oyOptions_s * options = 0;
  oyFilterNode_s * s = node;
  int error = 0;

  oyFilterNode_s_ * node_ = (oyFilterNode_s_*)node;

  if(!node)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 0 )

  if(flags || !node_->core->options_)
  {
    options = oyOptions_ForFilter_( node_->core, node_, flags, node_->core->oy_ );
    if(!node_->core->options_)
    {
      node_->core->options_ = options;
      options = NULL;
    } else if(options)
      error = oyOptions_Filter( &node_->core->options_, 0, 0,
                                oyBOOLEAN_UNION,
                                0, options );
    if(error > 0)
      WARNc2_S("%s %d", _("found issues"),error);
    if(!node_->core->options_)
      node_->core->options_ = oyOptions_New( 0 );
    oyOptions_Release( &options );
  }

  options = oyOptions_Copy( node_->core->options_, 0 );

  /** Observe exported options for changes and propagate to a existing graph. */
  error = oyOptions_ObserverAdd( options, (oyStruct_s*)node,
                                 0, oyFilterNode_Observe_ );
  if(error)
    WARNc2_S("%s %d", _("found issues"),error);

  return options;
}

/** Function  oyFilterNode_GetUi
 *  @memberof oyFilterNode_s
 *  @brief    Get filter options XFORMS
 *
 *  @param[in,out] node                filter object
 *  @param[in]     flags               modificators
 *                                     - oyNAME_JSON : order JSON flavour
 *  @param[out]    ui_text             XFORMS fitting to the node Options
 *  @param[out]    namespaces          additional XML namespaces
 *  @param         allocateFunc        optional user allocator
 *  @return                            the options
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/01/17
 *  @since   2009/07/29 (Oyranos: 0.1.10)
 */
int            oyFilterNode_GetUi    ( oyFilterNode_s     * node,
                                       int                  flags,
                                       char              ** ui_text,
                                       char             *** namespaces,
                                       oyAlloc_f            allocateFunc )
{
  int error = 0;
  oyFilterNode_s * s = node;
  oyOptions_s * options = 0;
  char * text = NULL,
       * tmp = NULL,
       * tmp4 = NULL;
  oyjl_val root9 = NULL, root4 = NULL;

  oyFilterNode_s_ ** node_ = (oyFilterNode_s_**)&node;

  if(!node)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 1 )

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  if(!error)
    options = oyFilterNode_GetOptions( node, 0 );

  if(!error && (*node_)->core->api4_->ui->oyCMMuiGet)
    error = (*node_)->core->api4_->ui->oyCMMuiGet( (oyCMMapiFilter_s*) (*node_)->core->api4_, options, flags, &tmp4, oyAllocateFunc_ );
  if(!(flags & oyNAME_JSON) && tmp4 && oyjlDataFormat(tmp4) == oyNAME_JSON)
    flags |= oyNAME_JSON;
  if(flags & oyNAME_JSON && tmp4 && oyjlDataFormat(tmp4) != oyNAME_JSON)
    flags = flags & (~oyNAME_JSON);

  if(!error)
  {
    oyCMMapiFilters_s * apis;
    int apis_n = 0, i,j = 0;
    oyCMMapi9_s * cmm_api9 = 0;
    oyCMMapi9_s_ ** cmm_api9_ = (oyCMMapi9_s_**)&cmm_api9;
    char * class_name, * api_reg;
    const char * reg = (*node_)->core->registration_;

    class_name = oyFilterRegistrationToText( reg, oyFILTER_REG_APPLICATION, 0 );
    api_reg = oyStringCopy_("///", oyAllocateFunc_ );
    STRING_ADD( api_reg, class_name );
    oyFree_m_( class_name );

    apis = oyCMMsGetFilterApis_( api_reg, oyOBJECT_CMM_API9_S,
                                 oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR,
                                 0,0 );
    apis_n = oyCMMapiFilters_Count( apis );
    for(i = 0; i < apis_n; ++i)
    {
      cmm_api9 = (oyCMMapi9_s*) oyCMMapiFilters_Get( apis, i );

      if(oyFilterRegistrationMatch( reg, (*cmm_api9_)->pattern, 0 ))
      {
        if((*cmm_api9_)->oyCMMuiGet)
          error = (*cmm_api9_)->oyCMMuiGet( (oyCMMapiFilter_s*) cmm_api9_, options, flags, &tmp, oyAllocateFunc_ );

        if(error)
        {
          WARNc2_S( "%s %s",_("error in module:"), (*cmm_api9_)->registration );
          return 1;

        } else
        if(tmp)
        {
          STRING_ADD( text, tmp );
          STRING_ADD( text, "\n" );
          if(flags & oyNAME_JSON)
          {
            root9 = oyJsonParse( tmp );
            if(tmp && !root9)
              WARNc3_S( "%s %s\n%s",_("error in module:"), (*cmm_api9_)->registration, tmp );
          }
          oyFree_m_(tmp);

          if(namespaces && (*cmm_api9_)->xml_namespace)
          {
            if(j == 0)
            {
              size_t len = (apis_n - i + 1) * sizeof(char*);
              *namespaces = allocateFunc( len );
              memset(*namespaces, 0, len);
            }
            *namespaces[j] = oyStringCopy_( (*cmm_api9_)->xml_namespace,
                                            allocateFunc );
            ++j;
            namespaces[j] = 0;
          }
        }
      }

      if(cmm_api9->release)
        cmm_api9->release( (oyStruct_s**)&cmm_api9 );
    }
    oyCMMapiFilters_Release( &apis );
  }

  if(!error && tmp4)
  {
    /* @todo and how to mix in the values? */
    if(flags & oyNAME_JSON)
    {
      root4 = oyJsonParse( tmp4 );
      if(!root4)
        WARNc3_S( "%s %s\n%s",_("error in module:"), (*node_)->core->api4_->registration, tmp4 );
    }
    STRING_ADD( text, tmp4 );
    oyFree_m_(tmp4);
  }

  if( flags & oyNAME_JSON &&
      root9 && root4 )
  {
    /* add api9 groups to api4 declaration */
    oyjl_val g9 = oyjlTreeGetValue( root9, 0, "org/freedesktop/oyjl/modules/[0]/groups" ),
             g4 = oyjlTreeGetValue( root4, 0, "org/freedesktop/oyjl/modules/[0]/groups" );
    int n9 = oyjlValueCount( g9 ),
        n4 = oyjlValueCount( g4 ), i;
    for(i = 0; i < n9; ++i)
    {
      oyjl_val group9 = oyjlTreeGetValueF( root9, 0, "org/freedesktop/oyjl/modules/[0]/groups/[%d]", i );
      /* allocate new array member */
      oyjl_val g4new = oyjlTreeGetValueF( root4, OYJL_CREATE_NEW, "org/freedesktop/oyjl/modules/[0]/groups/[%d]", n4 + i );
      /* move the node to the new tree */
      if(g4new && OYJL_IS_ARRAY(g4) && g4->u.array.values)
      {
        int size = sizeof(oyjl_val);
        memmove( &g4->u.array.values[i+1], &g4->u.array.values[i], size*n4 );
        g4->u.array.values[i] = group9;
        g9->u.array.values[i] = NULL;
        if(i == n9 - 1)
          g9->u.array.len -= n9;
      }
    }
    oyFree_m_( text );
    text = oyJsonPrint( root4 );
  }
  oyjlTreeFree(root9);
  oyjlTreeFree(root4);

  oyOptions_Release( &options );

  if(error <= 0 && text)
  {
    if(allocateFunc != malloc)
    {
      *ui_text = oyStringCopy_( text, allocateFunc );
      free(text);
    }
    else
      *ui_text = text;
    text = NULL;
  }

  if(text)
    free(text);

  return error;
}

/** Function  oyFilterGraph_ToText
 *  @memberof oyFilterGraph_s
 *  @brief    Text description of a graph
 *
 *  @todo Should this function generate XFORMS compatible output? How?
 *
 *  @param[in]     graph               graphy object
 *  @param[in]     input               start node of a oyConversion_s
 *  @param[in]     output              end node and if present a switch
 *                                     to interprete input and output as start
 *                                     and end node of a oyConversion_s
 *  @param[in]     head_line           text for inclusion
 *  @param[in]     reserved            future format selector (dot, xml ...)
 *  @param[in]     allocateFunc        allocation function; not implemented
 *  @return                            the graph description
 *
 *  @version Oyranos: 0.9.5
 *  @date    2013/03/12
 *  @since   2008/10/04 (Oyranos: 0.1.8)
 */
OYAPI char * OYEXPORT
           oyFilterGraph_ToText      ( oyFilterGraph_s   * graph,
                                       oyFilterNode_s    * input,
                                       oyFilterNode_s    * output,
                                       const char        * head_line,
                                       int                 reserved OY_UNUSED,
                                       oyAlloc_f           allocateFunc OY_UNUSED )
{
  char * text = NULL, * temp = NULL, * temp2 = NULL,
       * tmp = NULL, * txt = NULL, * t = NULL;
  oyFilterNode_s * node = 0;
#ifdef USE_GETTEXT
  char * save_locale = 0;
#endif
  oyFilterGraph_s_ * s = (oyFilterGraph_s_*)graph;

  oyFilterPlug_s_ * p = 0;
  int i, j, n, len,
      nodes_n = 0;
  oyPointer_s * backend_data;

  oyCheckType__m( oyOBJECT_FILTER_GRAPH_S, return NULL )

  temp = oyAllocateFunc_(4096);
  if(!temp) { return NULL; }
  temp2 = oyAllocateFunc_(4096);
  if(!temp2) { oyFree_m_(temp); return NULL; }

#ifdef USE_GETTEXT
  save_locale = oyStringCopy_( setlocale(LC_NUMERIC, 0 ), oyAllocateFunc_);
#endif

  STRING_ADD( text, "digraph G {\n" );
  STRING_ADD( text, "bgcolor=\"transparent\"\n" );
  STRING_ADD( text, "  rankdir=LR\n" );
  STRING_ADD( text, "  graph [fontname=Helvetica, fontsize=12];\n" );
  STRING_ADD( text, "  node [shape=record, fontname=Helvetica, fontsize=10, style=\"filled,rounded\"];\n" );
  STRING_ADD( text, "  edge [fontname=Helvetica, fontsize=10];\n" );
  STRING_ADD( text, "\n" );
  if(input && output)
  {
  STRING_ADD( text, "  conversion [shape=plaintext, label=<\n" );
  STRING_ADD( text, "<table border=\"0\" cellborder=\"1\" cellspacing=\"0\">\n" );
  STRING_ADD( text, "  <tr><td>oyConversion_s</td></tr>\n" );
  STRING_ADD( text, "  <tr><td>\n" );
  STRING_ADD( text, "     <table border=\"0\" cellborder=\"0\" align=\"left\">\n" );
  STRING_ADD( text, "       <tr><td align=\"left\">...</td></tr>\n" );
  STRING_ADD( text, "       <tr><td align=\"left\" port=\"in\">+input</td></tr>\n" );
  STRING_ADD( text, "       <tr><td align=\"left\" port=\"out\">+out_</td></tr>\n" );
  STRING_ADD( text, "       <tr><td align=\"left\">...</td></tr>\n" );
  STRING_ADD( text, "     </table>\n" );
  STRING_ADD( text, "     </td></tr>\n" );
  STRING_ADD( text, "  <tr><td> </td></tr>\n" );
  STRING_ADD( text, "</table>>,\n" );
  STRING_ADD( text, "                    style=\"\", color=black];\n" );
  }
  STRING_ADD( text, "\n" );

  /* add more node descriptions */
  nodes_n = oyFilterNodes_Count( s->nodes );
  for(i = 0; i < nodes_n; ++i)
  {
    oyFilterCore_s_ * node_core;
    oyOptions_s * node_opts;
    const char * tc;

    node = oyFilterNodes_Get( s->nodes, i );
    n = oyFilterNode_EdgeCount( node, 1, 0 );
    node_core = (oyFilterCore_s_*)oyFilterNode_GetCore( node );

    /** The function is more verbose with the oy_debug variable set. */
    if(!oy_debug &&
       oyStrchr_( ((oyCMMapi4_s_*)node_core->api4_)->id_, OY_SLASH_C ))
    {
      STRING_ADD( tmp, ((oyCMMapi4_s_*)node_core->api4_)->id_ );
      t = oyStrrchr_( tmp, OY_SLASH_C );
      *t = 0;
      STRING_ADD( txt, t+1 );
      oyFree_m_(tmp);
    } else
    {
      STRING_ADD( txt, ((oyCMMapi4_s_*)node_core->api4_)->id_ );
      if(oy_debug)
      {
        int node_sockets_n = oyFilterNode_EdgeCount( node, 0,
                                                     OY_FILTEREDGE_CONNECTED );
      for(j = 0; j < node_sockets_n; ++j)
      {
        oyFilterSocket_s * socket = oyFilterNode_GetSocket( node, j );
        oyStruct_s * socket_data = oyFilterSocket_GetData( socket );
        if(socket && socket_data)
        {
          const char * name = oyObject_GetName( socket_data->oy_, 1 );
          int k, pos = 0;
          len = strlen(name);
          for(k = 0; k < len; ++k)
            if(k && name[k] == '"' && name[k-1] != '\\')
            {
              sprintf( &temp2[pos], "\\\"" );
              pos += 2;
            } else if(name[k] == '<')
            {
              sprintf( &temp2[pos], "\\<" );
              pos += 2;
            } else if(name[k] == '>')
            {
              sprintf( &temp2[pos], "\\>" );
              pos += 2;
            } else if(name[k] == '[')
            {
              sprintf( &temp2[pos], "\\[" );
              pos += 2;
            } else if(name[k] == ']')
            {
              sprintf( &temp2[pos], "\\]" );
              pos += 2;
            } else if(name[k] == '\n')
            {
              sprintf( &temp2[pos], "\\n" );
              pos += 2;
            } else
              temp2[pos++] = name[k];
          temp2[pos] = 0;
          if(oy_debug >= 4)
          oyMessageFunc_p( oyMSG_DBG,(oyStruct_s*)node,
                           OY_DBG_FORMAT_ "%s", OY_DBG_ARGS_, name );
          if(oy_debug >= 4)
          oyMessageFunc_p( oyMSG_DBG,(oyStruct_s*)node,
                           OY_DBG_FORMAT_ "%s", OY_DBG_ARGS_, temp2 );
          oySprintf_(temp, "  %d [ label=\"{<data> | Data %d\\n"
                     " Type: \\\"%s\\\"\\n"
                     " XML: \\\"%s\\\"|<socket>}\"];\n",
                     oyObject_GetId( socket->oy_ ),
                     j,
                     oyStructTypeToText( socket_data->type_ ),
                     temp2);
          if(oy_debug >= 4)
          oyMessageFunc_p( oyMSG_DBG,(oyStruct_s*)node,
                           OY_DBG_FORMAT_ "%s", OY_DBG_ARGS_, temp );
          STRING_ADD( text, temp );
        }
        oyFilterSocket_Release( &socket );
      }
      }
    }

    backend_data = oyFilterNode_GetContext( node );
#if 0
    tc = oyFilterNode_GetText( node, oyNAME_NICK );
#else
    node_opts = oyFilterNode_GetOptions( node, 0 );
    tc = oyOptions_GetText( node_opts, oyNAME_NICK );
    oyOptions_Release( &node_opts );
#endif
    t = oyjlStringCopy(tc, 0);
    oyjlStringReplace( &t, "\n\n", "\n", NULL,NULL );
    oyjlStringReplace( &t, "\n", "\\n", NULL,NULL );
    oyjlStringReplace( &t, "\"", "\\\"", NULL,NULL );
    oyjlStringReplace( &t, "<", "*", NULL,NULL );
    oyjlStringReplace( &t, ">", "*", NULL,NULL );
    oyMessageFunc_p( oyMSG_DBG,(oyStruct_s*)node,
                     OY_DBG_FORMAT_ "%s\n%s", OY_DBG_ARGS_, oyNoEmptyString_m_(t), oyNoEmptyString_m_(tc) );
    sprintf( temp,   "  %d [ label=\"{<plug> %d| Filter Node %d\\n"
                     " Category: \\\"%s\\\"\\n CMM: \\\"%s\\\"\\n"
                     " Type: \\\"%s\\\"\\n"
                     " ResourceType: \\\"%s\\\" %d\\n"
                     " Options: \\\"%s\\\"|<socket>}\"];\n",
                     oyFilterNode_GetId( node ), n,
                     node->oy_->id_,
                     oyFilterCore_GetCategory( (oyFilterCore_s*)node_core, 1 ),
                     txt,
                     oyFilterNode_GetRegistration( node ),
                     oyNoEmptyString_m_(oyPointer_GetResourceName( backend_data )),
                     oyPointer_GetSize( backend_data ),
                     oyNoEmptyString_m_(t));

    STRING_ADD( text, temp );
    oyFree_m_(txt);
    if(t) oyFree_m_(t);

    oyFilterNode_Release( &node );
    oyFilterCore_Release( (oyFilterCore_s**)&node_core );
  }


  STRING_ADD( text, "\n" );
  STRING_ADD( text, "  subgraph cluster_0 {\n" );
  STRING_ADD( text, "    label=\"" );
  STRING_ADD( text, head_line );
  STRING_ADD( text, "\"\n" );
  STRING_ADD( text, "    color=gray;\n" );
  STRING_ADD( text, "\n" );

  /* add more node placements */
  n = oyFilterPlugs_Count( s->edges );
  for(i = 0; i < n; ++i)
  {
    p = (oyFilterPlug_s_*)oyFilterPlugs_Get( s->edges, i );

    oySprintf_( temp,
                "    %d:socket -> %d:plug [arrowhead=crow, arrowtail=box];\n",
                oyFilterNode_GetId( (oyFilterNode_s*)p->remote_socket_->node ),
                oyFilterNode_GetId( (oyFilterNode_s*)p->node ) );
    STRING_ADD( text, temp );

    oyFilterPlug_Release( (oyFilterPlug_s**)&p );
  }
  
  for(i = 0; i < nodes_n; ++i)
  {
    node = oyFilterNodes_Get( s->nodes, i );
    if(oy_debug)
    {
      int node_sockets_n = oyFilterNode_EdgeCount( node, 0,
                                                   OY_FILTEREDGE_CONNECTED );
      for(j = 0; j < node_sockets_n; ++j)
      {
        oyFilterSocket_s * socket = oyFilterNode_GetSocket( node, j );
        oyStruct_s * socket_data = oyFilterSocket_GetData( socket );
        if(socket && socket_data)
        {
          oySprintf_( temp,
               "    %d:socket -> %d:data [arrowhead=crow, arrowtail=box];\n",
                oyFilterNode_GetId( node ),
                oyObject_GetId( socket->oy_ ));
          STRING_ADD( text, temp );
          if(socket_data->release)
            socket_data->release( &socket_data );
        }
        oyFilterSocket_Release( &socket );
          
      }
    }
    oyFilterNode_Release( &node );
  }

  STRING_ADD( text, "\n" );
  if(input && output)
  {
  oySprintf_( temp, "    conversion:in -> %d [arrowhead=none, arrowtail=normal];\n", oyFilterNode_GetId( input ) );
  STRING_ADD( text, temp );
  oySprintf_( temp, "    conversion:out -> %d;\n",
                    oyFilterNode_GetId( output ) );
  STRING_ADD( text, temp );
  }
  STRING_ADD( text, "  }\n" );
  STRING_ADD( text, "\n" );
  if(input && output)
  STRING_ADD( text, "  conversion\n" );
  STRING_ADD( text, "}\n" );
  STRING_ADD( text, "\n" );

#ifdef USE_GETTEXT
  setlocale(LC_NUMERIC, "C");
  /* sensible printing */
  setlocale(LC_NUMERIC, save_locale);
  oyFree_m_( save_locale );
#endif

  STRING_ADD( text, "" );

  oyFree_m_( temp );
  oyFree_m_( temp2 );

  return text;
}

char *       oyGetFilterNodeKey      ( const char        * base_key,
                                       int                 select_core )
{
  char * key_name = NULL;
  const char * key = "renderer";

  if(base_key)
  {
    if(select_core)
      key = "context";

    oyStringAddPrintf( &key_name, oyAllocateFunc_, oyDeAllocateFunc_,
                       "%s" OY_SLASH "%s", base_key, key );
  }

  return key_name;
}

/** Function oyGetFilterNodeRegFromDB
 *  @brief   Get a default FilterNode from DB
 *  @internal
 *
 *  The stored value will contain a registration pattern string.
 *
 *  @param         db_base_key         the CMM node to look up
 *  @param         select_core         select core oyCMMapi4_s
 *  @param         flags               oySOURCE_DATA for persistent DB only settings, oySOURCE_FILTER for Oyranos inbuild default
 *  @param         allocate_func       user allocator
 *  @return                            a registration pattern to match a CMM registration string
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/07/17
 *  @since   2014/07/17 (Oyranos: 0.9.6)
 */
char *       oyGetFilterNodeRegFromDB( const char        * db_base_key,
                                       int                 select_core,
                                       uint32_t            flags,
                                       oyAlloc_f           allocate_func )
{
  char* key_name = oyGetFilterNodeKey( db_base_key, select_core );
  char * name = NULL;

  DBG_PROG_START

  DBG_PROG3_S( "params db_base_key = %s select_core = %d flags = %d", oyNoEmptyString_m_(db_base_key), select_core, flags )

  if(db_base_key)
  {
    if(key_name &&
       (!flags || flags & oySOURCE_DATA))
    {
      name = oyGetPersistentString( key_name, flags, oySCOPE_USER_SYS,
                                    allocate_func );
    }

  } else
      WARNc_S( "db_base_key arg missed" );

  oyFree_m_( key_name );

  DBG_PROG_ENDE
  return name;
}

/** Function oyGetFilterNodeRegFromOptions
 *  @brief   Get a default FilterNode registration from options
 *  @internal
 *
 *  The returned value will contain a registration pattern string.
 *
 *  @param         base_pattern        the basic pattern to search in the options
 *  @param         select_core         select core 1 - oyCMMapi4_s else 0 - oyCMMapi7_s
 *  @param         options             the options to search in
 *  @param         allocate_func       user allocator
 *  @return                            a registration pattern to match a CMM registration string
 *
 *  @version Oyranos: 0.9.6
 *  @date    2015/02/29
 *  @since   2014/07/17 (Oyranos: 0.9.6)
 */
char *       oyGetFilterNodeRegFromOptions(
                                       const char        * base_pattern,
                                       int                 select_core,
                                       oyOptions_s       * options,
                                       oyAlloc_f           allocate_func )
{
  char * name = NULL;
  const char * module = NULL;
  char * key_name = oyGetFilterNodeKey( base_pattern, select_core );

  if(!base_pattern)
  {
    WARNc_S( "base_pattern arg is missed" );
    goto clean_oyGetFilterNodeRegFromOptions;
  }

  module = oyOptions_FindString( options, key_name, NULL );

  if(!module)
    goto clean_oyGetFilterNodeRegFromOptions;
  else if(strchr(module, '/') == NULL)
  {
    char * t = NULL;
    oyStringAddPrintf( &t, oyAllocateFunc_, oyDeAllocateFunc_,
                       "//" OY_TYPE_STD "/%s", module );
    name = oyStringCopy( t, allocate_func );
    oyFree_m_(t);

  } else
    name = oyStringCopy( module, allocate_func );

clean_oyGetFilterNodeRegFromOptions:
  oyFree_m_( key_name );

  return name;
}

/** Function oyGetFilterNodeDefaultPatternFromPolicy
 *  @brief   Get a default FilterNode registration from a node type module
 *  @internal
 *
 *  The returned value will contain a registration pattern string.
 *
 *  @param         base_pattern        the basic pattern to search in the options
 *  @param         select_core         select core 1 - oyCMMapi4_s else 0 - oyCMMapi7_s
 *  @param         options             the options to search in
 *  @param         allocate_func       user allocator
 *  @return                            a registration pattern to match a CMM registration string
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/07/17
 *  @since   2014/07/17 (Oyranos: 0.9.6)
 */
char *       oyGetFilterNodeDefaultPatternFromPolicy (
                                       const char        * base_pattern,
                                       int                 select_core,
                                       oyAlloc_f           allocate_func )
{
  char * name = NULL;
  char * key_name = oyGetFilterNodeKey( base_pattern, select_core ),
       * pattern = key_name;

  if(!base_pattern)
  {
    WARNc_S( "base_pattern arg is missed" );
    return NULL;
  }

  oyCMMapiFilters_s * apis;
  int apis_n = 0, i;
  oyCMMapi9_s_ * cmm_api9_ = 0;
  char * class_name, * api_reg;

  class_name = oyFilterRegistrationToText( pattern, oyFILTER_REG_APPLICATION,0);
  api_reg = oyStringCopy_("///", oyAllocateFunc_ );
  STRING_ADD( api_reg, class_name );
  oyFree_m_( class_name );

  apis = oyCMMsGetFilterApis_( api_reg, oyOBJECT_CMM_API9_S,
                               oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR,
                               0,0 );
  oyFree_m_( api_reg );
  apis_n = oyCMMapiFilters_Count( apis );
  for(i = 0; i < apis_n; ++i)
  {
    cmm_api9_ = (oyCMMapi9_s_*) oyCMMapiFilters_Get( apis, i );

    if(oyFilterRegistrationMatch( cmm_api9_->pattern, pattern, 0 ))
    {
      if(cmm_api9_->oyCMMGetDefaultPattern)
        name = cmm_api9_->oyCMMGetDefaultPattern( base_pattern, 0,
                                                  select_core, allocate_func );
      if(!name)
        WARNc2_S( "%s %s",_("error in module:"), cmm_api9_->registration );
    }

    if(cmm_api9_->release)
      cmm_api9_->release( (oyStruct_s**)&cmm_api9_ );
  }
  oyCMMapiFilters_Release( &apis );

  oyFree_m_( key_name );

  return name;
}

/** Function  oyFilterNode_FromOptions
 *  @brief    Create a FilterNode from options and fallbacks
 *  @memberof oyFilterNode_s
 *
 *  The returned object will be created from the found registration pattern strings.
 *  The first pattern will be searched in the options argument. If a option is found
 *  and that fails, then NULL will be returned.
 *  Else the db_base_key will be used to ask the Oyranos DB for context and 
 *  renderer patterns. In case that fails a policy module will be asked for
 *  a default through the base_pattern.
 *
 *  @param         db_base_key         the Oyranos DB basic key, which will appended by "context" and "renderer"; optional
 *  @param         base_pattern        the basic pattern to search in the options and to be used as fallback
 *  @param         options             the options to search in for the base_pattern and get "context" and "renderer" keys; optional
 *  @param         object              the optional object
 *  @return                            a registration pattern to match a CMM registration string or NULL
 *
 *  @version Oyranos: 0.9.6
 *  @date    2015/05/13
 *  @since   2014/07/17 (Oyranos: 0.9.6)
 */
oyFilterNode_s *   oyFilterNode_FromOptions (
                                       const char        * db_base_key,
                                       const char        * base_pattern,
                                       oyOptions_s       * options,
                                       oyObject_s          object )
{
  oyFilterCore_s * core = NULL;
  oyFilterNode_s * node = NULL;
  char * pattern = NULL;
  int select_core = 1;

  pattern = oyGetFilterNodeRegFromOptions( base_pattern, select_core, options,
                                           oyAllocateFunc_ );
  if(pattern)
  {
    if(!pattern[0])
    {
      oyFree_m_(pattern);
    } else
      core = oyFilterCore_NewWith( pattern, options, object );
  }
  if(!core)
  {
    oyOption_s * ct = oyOptions_Find( options, "////context", oyNAME_PATTERN );
    if(ct)
      if(oyOption_GetFlags(ct) & oyOPTIONATTRIBUTE_EDIT)
      {
        oyOption_Release( &ct );
        WARNc1_S("no explicite context node possible: \"%s\"", oyNoEmptyString_m_(pattern) );
        return node;
      }
  }
  if(!core && db_base_key)
  {
    pattern = oyGetFilterNodeRegFromDB( db_base_key, select_core,
                                        0, oyAllocateFunc_ );
    if(pattern)
    {
      if(!pattern[0])
      {
        oyFree_m_(pattern);
      } else
        core = oyFilterCore_NewWith( pattern, options, object );
    }
  }
  if(!core)
  {
    if(pattern)
      oyFree_m_(pattern);
    pattern = oyGetFilterNodeDefaultPatternFromPolicy ( base_pattern,
                                                        select_core,
                                                        oyAllocateFunc_ );
    if(!pattern)
    {
      WARNc1_S("no pattern from policy module for base_pattern obtained: \"%s\"", oyNoEmptyString_m_(base_pattern) );
    } else
      core = oyFilterCore_NewWith( pattern, options, object );
  }
  if(!core)
    oyMessageFunc_p( oyMSG_WARN, (oyStruct_s*) node,
                     OY_DBG_FORMAT_ "could not create new core: \"%s\" \"%s\"",
                     OY_DBG_ARGS_,
                     oyNoEmptyString_m_(pattern),
                     oyNoEmptyString_m_(base_pattern) );

  oyFree_m_( pattern );

  /* now the same again like above, just for node creation and 
   * with the above created core */
  select_core = 0;
  pattern = oyGetFilterNodeRegFromOptions( base_pattern, select_core, options,
                                           oyAllocateFunc_ );
  if(pattern)
  {
    if(!pattern[0])
    {
      oyFree_m_(pattern);
    } else
      node = oyFilterNode_Create( pattern, core, object );
  }
  if(!node)
  {
    oyOption_s * ct = oyOptions_Find( options, "////renderer", oyNAME_PATTERN );
    if(ct)
      if(oyOption_GetFlags(ct) & oyOPTIONATTRIBUTE_EDIT)
      {
        oyOption_Release( &ct );
        oyFilterCore_Release( &core );
        WARNc1_S("no explicite renderer node possible: \"%s\"", oyNoEmptyString_m_(pattern) );
        return node;
      }
  }
  if(!node && db_base_key)
  {
    pattern = oyGetFilterNodeRegFromDB( db_base_key, select_core,
                                        0, oyAllocateFunc_ );
    if(pattern)
    {
      if(!pattern[0])
      {
        oyFree_m_(pattern);
      } else
        node = oyFilterNode_Create( pattern, core, object );
    }
  }
  if(!node)
  {
    if(pattern)
      oyFree_m_(pattern);
    pattern = oyGetFilterNodeDefaultPatternFromPolicy ( base_pattern,
                                                        select_core,
                                                        oyAllocateFunc_ );
    node = oyFilterNode_Create( pattern, core, object );
  }
  if(!node)
  {
    oyMessageFunc_p( oyMSG_WARN, (oyStruct_s*) node,
                     OY_DBG_FORMAT_ "could not create new node: %s %s",
                     OY_DBG_ARGS_,
                     oyNoEmptyString_m_(pattern),
                     oyNoEmptyString_m_(base_pattern) );
    node = oyFilterNode_Create( base_pattern, core, object );
  }

  oyFree_m_( pattern );
  oyFilterCore_Release( &core );

  return node;
}

/** \addtogroup cmm_handling
 *  @{ *//* cmm_handling */

/** Function  oyICCProfileSelectionFlagsFromOptions
 *  @brief    Get valid profile selection flags from node options and fallbacks
 *
 *  A convenience function for oyFilterNode_FromOptions() .
 *
 *  @param         db_base_key         the Oyranos DB basic key, which will appended by "context" and "renderer"; optional
 *  @param         base_pattern        the basic pattern to search in the options and to be used as fallback
 *  @param         options             the options to search in for the base_pattern and get "context" and "renderer" keys; optional
 *  @param         select_core         0 - select node, 1 - select core
 *  @return                            a registration string
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/07/30
 *  @since   2014/07/30 (Oyranos: 0.9.6)
 */
uint32_t     oyICCProfileSelectionFlagsFromOptions (
                                       const char        * db_base_key,
                                       const char        * base_pattern,
                                       oyOptions_s       * options,
                                       int                 select_core )
{
  oyFilterNode_s * node = NULL;
  const char * reg = NULL;
  uint32_t icc_profile_flags = 0;
  char * db_base_key_ = NULL;

  if(db_base_key)
    STRING_ADD( db_base_key_, db_base_key);
  STRING_ADD( db_base_key_, select_core?"context":"renderer" );

  node = oyFilterNode_FromOptions( db_base_key_, base_pattern, options, NULL );
  oyFree_m_( db_base_key_ );
  reg = oyFilterNode_GetRegistration( node );
  icc_profile_flags = oyICCProfileSelectionFlagsFromRegistration( reg );
  oyFilterNode_Release( &node );
  DBG_NUM2_S("reg: %s icc_profile_flags: %d", reg, icc_profile_flags );

  return icc_profile_flags;
}
/** @} *//* cmm_handling */



/** \addtogroup misc Miscellaneous

 *  @{ *//* misc */


/** \addtogroup objects_generic Generic Objects 
 *  @brief Basic objects to make all higher level object types happy

 *  @{ *//* objects_generic */



/** @} *//* objects_generic */



/** \addtogroup objects_value Values Handling
 *  @brief Manipulate and abstract values

 *  @{ *//* objects_value */


/** @internal
 *  @struct  oyConfDomain_s_
 *  @brief   a ConfDomain object
 *  @extends oyStruct_s
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
typedef struct {
  oyOBJECT_e           type_;          /**< struct type oyOBJECT_CONF_DOMAIN_S */ 
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  oyCMMapi8_s        * api8;
} oyConfDomain_s_;

oyConfDomain_s_ *
           oyConfDomain_New_         ( oyObject_s          object );
oyConfDomain_s_ *
           oyConfDomain_FromReg_     ( const char        * registration,
                                       oyObject_s          object );
oyConfDomain_s_ *
           oyConfDomain_Copy_        ( oyConfDomain_s_   * obj,
                                       oyObject_s          object);
int
           oyConfDomain_Release_     ( oyConfDomain_s_   **obj );

const char * oyConfDomain_GetText_   ( oyConfDomain_s_   * obj,
                                       const char        * name,
                                       oyNAME_e            type );
const char **oyConfDomain_GetTexts_  ( oyConfDomain_s_   * obj );

/* --- Public_API Begin --- */

/** Function oyConfDomain_New
 *  @memberof oyConfDomain_s
 *  @brief   allocate a new ConfDomain object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
OYAPI oyConfDomain_s * OYEXPORT
           oyConfDomain_FromReg      ( const char        * registration_domain,
                                       oyObject_s          object )
{
  oyConfDomain_s_ * obj = 0;

  obj = oyConfDomain_FromReg_( registration_domain, object );

  return (oyConfDomain_s*) obj;
}

/** Function oyConfDomain_Copy
 *  @memberof oyConfDomain_s
 *  @brief   copy or reference a ConfDomain object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
OYAPI oyConfDomain_s * OYEXPORT
           oyConfDomain_Copy         ( oyConfDomain_s    * obj,
                                       oyObject_s          object )
{
  oyConfDomain_s_ * s = (oyConfDomain_s_*) obj;

  if(s)
  { oyCheckType__m( oyOBJECT_CONF_DOMAIN_S, return NULL ); }
  else
    return NULL;

  s = oyConfDomain_Copy_( s, (oyObject_s) object );

  return (oyConfDomain_s*) s;
}
 
/** Function oyConfDomain_Release
 *  @memberof oyConfDomain_s
 *  @brief   release and possibly deallocate a ConfDomain object
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
OYAPI int  OYEXPORT
           oyConfDomain_Release      ( oyConfDomain_s    **obj )
{
  oyConfDomain_s_ * s = 0;

  if(!obj || !*obj)
    return 0;

  s = (oyConfDomain_s_*) *obj;

  oyCheckType__m( oyOBJECT_CONF_DOMAIN_S, return 1 )

  *obj = 0;

  return oyConfDomain_Release_( &s );
}

/** Function oyConfDomain_GetText
 *  @memberof oyConfDomain_s
 *  @brief   obtain a UI text from a ConfDomain object
 *
 *  @param[in,out] obj                 struct object
 *  @param[in]     name                the category to return
 *  @param[in]     type                the type of string
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
OYAPI const char * OYEXPORT
           oyConfDomain_GetText      ( oyConfDomain_s    * obj,
                                       const char        * name,
                                       oyNAME_e            type )
{
  oyConfDomain_s_ * s = (oyConfDomain_s_*) obj;
  const char * text = 0;

  if(s)
  { oyCheckType__m( oyOBJECT_CONF_DOMAIN_S, return NULL ); }
  else
    return NULL;

  text = oyConfDomain_GetText_( s, name, type );

  return text;
}

/** Function oyConfDomain_GetTexts
 *  @memberof oyConfDomain_s
 *  @brief   obtain a list of possible UI text from a ConfDomain object
 *
 *  @return                            zero terminated list of strings,
 *                                     Each string is a "name" option to
 *                                     oyConfDomain_GetText().
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
OYAPI const char ** OYEXPORT
           oyConfDomain_GetTexts     ( oyConfDomain_s    * obj )
{
  oyConfDomain_s_ * s = (oyConfDomain_s_*) obj;
  const char ** texts = 0;

  if(s)
  { oyCheckType__m( oyOBJECT_CONF_DOMAIN_S, return NULL ); }
  else
    return NULL;

  texts = oyConfDomain_GetTexts_( s );

  return texts;
}

/* --- Public_API End --- */


/** @internal
 *  Function oyConfDomain_New_
 *  @memberof oyConfDomain_s_
 *  @brief   allocate a new ConfDomain object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
oyConfDomain_s_ * oyConfDomain_New_  ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CONF_DOMAIN_S;
# define STRUCT_TYPE oyConfDomain_s_
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyConfDomain_s" );
  STRUCT_TYPE * s = NULL;

  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    oyObject_Release( &s_obj );
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyConfDomain_Copy;
  s->release = (oyStruct_Release_f) oyConfDomain_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
  if(error)
    return 0;
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  s->api8 = 0;

  return s;
}

oyConfDomain_s_ * oyConfDomain_FromReg_(
                                       const char        * registration,
                                       oyObject_s          object )
{
  oyConfDomain_s_ * s = oyConfDomain_New_( object );
  int error = !s;
  oyCMMapi8_s * cmm_api8 = 0;

  if(error <= 0)
  {
    cmm_api8 = (oyCMMapi8_s*) oyCMMsGetFilterApi_( registration,
                                                   oyOBJECT_CMM_API8_S );
    error = !cmm_api8;
  }

  if(error <= 0)
    s->api8 = cmm_api8;

  return s;
}

/** @internal
 *  Function oyConfDomain_Copy__
 *  @memberof oyConfDomain_s_
 *  @brief   real copy a ConfDomain object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
oyConfDomain_s_ * oyConfDomain_Copy__ (
                                       oyConfDomain_s_   * obj,
                                       oyObject_s          object )
{
  oyConfDomain_s_ * s = 0;
  int error = 0;

  if(!obj || !object)
    return s;

  s = oyConfDomain_New_( object );
  error = !s;

  if(!error)
  {

    if(obj->api8)
    {
      if(obj->api8->copy)
        s->api8 = (oyCMMapi8_s*) obj->api8->copy( (oyStruct_s*)s->api8,
                                                  object );
      else
        s->api8 = obj->api8;
    }
  }

  if(error)
    oyConfDomain_Release_( &s );

  return s;
}

/** @internal
 *  Function oyConfDomain_Copy_
 *  @memberof oyConfDomain_s_
 *  @brief   copy or reference a ConfDomain object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
oyConfDomain_s_ * oyConfDomain_Copy_ ( oyConfDomain_s_   * obj,
                                       oyObject_s          object )
{
  oyConfDomain_s_ * s = obj;

  if(!obj)
    return 0;

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyConfDomain_Copy__( obj, object );

  return s;
}
 
/** @internal
 *  Function oyConfDomain_Release_
 *  @memberof oyConfDomain_s_
 *  @brief   release and possibly deallocate a ConfDomain object
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
int        oyConfDomain_Release_     ( oyConfDomain_s_   **obj )
{
  /* ---- start of common object destructor ----- */
  oyConfDomain_s_ * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */


  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    if(s->api8)
    {
      if(s->api8->release)
        s->api8->release( (oyStruct_s**) &s->api8 );
      else
        s->api8 = 0;
    }

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}

/** @internal
 *  Function oyConfDomain_GetText_
 *  @memberof oyConfDomain_s
 *  @brief   obtain a UI text from a ConfDomain object
 *
 *  @param[in,out] obj                 struct object
 *  @param[in]     name                the category to return
 *  @param[in]     type                the type of string
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/12/30
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 */
const char * oyConfDomain_GetText_   ( oyConfDomain_s_   * obj,
                                       const char        * name,
                                       oyNAME_e            type )
{
  const char * text = 0;
  oyConfDomain_s_ * s = obj;
  oyCMMui_s * ui = 0;

  if(s->api8)
    ui = oyCMMapi8_GetUi(s->api8);

  if(ui && oyCMMui_GetTextF(ui))
    text = oyCMMui_GetTextF(ui)( name, type, (oyStruct_s*)ui );

  return text;
}

/** @internal
 *  Function oyConfDomain_GetTexts
 *  @memberof oyConfDomain_s
 *  @brief   obtain a list of possible UI text from a ConfDomain object
 *
 *  @return                            zero terminated list of strings,
 *                                     Each string is a "name" option to
 *                                     oyConfDomain_GetText().
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
const char **oyConfDomain_GetTexts_  ( oyConfDomain_s_   * obj )
{
  oyConfDomain_s_ * s = obj;
  const char ** texts = 0;
  oyCMMui_s * ui = 0;

  if(!s) return NULL;

  if(s->api8)
    ui = oyCMMapi8_GetUi(s->api8);

  if(ui)
    texts = oyCMMui_GetTexts(ui);

  return texts;
}


/** @} *//* objects_value */

/** @} *//* misc */
