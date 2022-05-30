/** @file oyranos_devices_internal.c

   [Template file inheritance graph]
   +-- oyranos_devices_internal.template.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


#include "oyranos_i18n.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyranos_devices.h"
#include "oyranos_devices_internal.h"
#include "oyranos_object_internal.h"


/** \addtogroup devices_handling
 *
 *  @{ *//* devices_handling */

/** @internal
 *  Function oyDeviceRegistrationCreate_
 *  @brief   combine a device registration
 *
 *  The base is OY_TOP_SHARED OY_SLASH OY_DOMAIN_STD, followed by slash
 *  device_type, slash, "device", slash, device_class, slash and key.
 *
 *  @param[in]     device_type         the device type ::oyFILTER_REG_TYPE,
 *                                     defaults to OY_TYPE_STD (optional)
 *  @param[in]     device_class        the device class, e.g. "monitor",
 *                                     ::oyFILTER_REG_APPLICATION
 *  @param[in]     key                 key_name to add at ::oyFILTER_REG_OPTION
 *  @param[in]     old_text            string to reuse
 *  @return                            the new registration
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/12/28
 *  @since   2009/01/30 (Oyranos: 0.1.10)
 */
char * oyDeviceRegistrationCreate_   ( const char        * device_type,
                                       const char        * device_class,
                                       const char        * key,
                                       char              * old_text )
{
  char * text = old_text;
  const char * device_type_ = device_type;

  if(!device_type_)
    device_type_ = OY_TYPE_STD;

  STRING_ADD( text, OY_TOP_SHARED OY_SLASH OY_DOMAIN_STD );
  STRING_ADD( text, OY_SLASH );
  STRING_ADD( text, device_type_ );
  STRING_ADD( text, OY_SLASH "device" );
  if(device_class)
  {
    STRING_ADD( text, OY_SLASH );
    STRING_ADD( text, device_class );
  }
  if(key)
  {
    STRING_ADD( text, OY_SLASH );
    STRING_ADD( text, key );
  }

  return text;
}

/** @internal
 *  Function oyOptions_SetDeviceTextKey_
 *  @brief   set a device option
 *
 *  @param[in,out] options             options for the device
 *  @param[in]     device_type         the device type ::oyFILTER_REG_TYPE,
 *                                     defaults to OY_TYPE_STD (optional)
 *  @param[in]     device_class        the device class, e.g. "monitor",
 *                                     ::oyFILTER_REG_APPLICATION
 *  @param[in]     key                 key_name to add at ::oyFILTER_REG_OPTION
 *  @param[in]     value               value of type ::oyVAL_STRING
 *  @return                            the new registration
 *
 *  @version Oyranos: 0.9.6
 *  @date    2015/02/28
 *  @since   2009/01/30 (Oyranos: 0.1.10)
 */
int    oyOptions_SetDeviceTextKey_   ( oyOptions_s      ** options,
                                       const char        * device_type,
                                       const char        * device_class,
                                       const char        * key,
                                       const char        * value )
{
  char * text = 0;
  int error = 0;

  text = oyDeviceRegistrationCreate_( device_type, device_class,
                                          key, text );
  error = oyOptions_SetFromString( options, text, value, OY_CREATE_NEW );

  oyFree_m_( text );

  return error;
}

/** @internal
 *  Function oyOptions_SetRegistrationTextKey_
 *  @brief   set a device option
 *
 *  @param[in,out] options             options for the device
 *  @param[in]     registration        the registration string
 *  @param[in]     key                 key_name to add at ::oyFILTER_REG_OPTION
 *  @param[in]     value               value of type ::oyVAL_STRING
 *  @return                            the new registration
 *
 *  @version Oyranos: 0.9.6
 *  @date    2015/02/28
 *  @since   2009/02/09 (Oyranos: 0.1.10)
 */
int    oyOptions_SetRegistrationTextKey_(
                                       oyOptions_s      ** options,
                                       const char        * registration,
                                       const char        * key,
                                       const char        * value )
{
  char * text = NULL,
       * reg = NULL,
       * old_val = NULL;
  const char * t;
  int error = 0;
  oyOption_s * o = oyOptions_Find( *options, key, oyNAME_PATTERN );

  if(o)
  {
    
    reg = oyStringCopy( oyOption_GetRegistration(o), oyAllocateFunc_ );
    text = strrchr( reg, OY_SLASH_C );
    if(text)
    {
      text[0] = '\000';
      text = NULL;
    }
    registration = reg;
    t = oyOption_GetValueString( o, 0 );
    if(t)
      old_val = oyStringCopy( t, oyAllocateFunc_ );
  }

  STRING_ADD( text, registration );
  STRING_ADD( text, "/" );
  STRING_ADD( text, key );

  if(old_val)
    DBG_PROG4_S( "%s=%s%s%s", text, old_val?old_val:"", old_val?" -> ": "", value );

  error = oyOptions_SetFromString( options, text, value, OY_CREATE_NEW );

  oyFree_m_( text );
  if(reg)
    oyFree_m_( reg );
  if(old_val)
    oyFree_m_( old_val );
  oyOption_Release( &o );

  return error;
}


int          oyDeviceCheckProperties ( oyConfig_s        * device )
{
  oyOption_s * o = 0;
  oyOptions_s * options = 0;
  int error = !device;
  const char * device_name = 0;
  oyConfig_s * s = device;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 1 )

  if(error <= 0)
  {
    device_name = oyConfig_FindString( device, "device_name", 0);

    /* 1. obtain detailed and expensive device informations */
    if( !oyConfig_FindString(s,"manufacturer",0) ||
        !oyConfig_FindString(s,"model",0) )
    { 
      /* 1.1 add "properties" call to module arguments */
      error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/command",
                                     "properties", OY_CREATE_NEW );
      error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/device_name",
                                     device_name, OY_CREATE_NEW );

      device_name = 0;

      /* 1.2 get details about device */
      if(error <= 0)
        error = oyDeviceBackendCall( device, options );

      oyOptions_Release( &options );

      /* renew outdated string */
      o = oyConfig_Find( device, "profile_name" );
      device_name = oyConfig_FindString( device, "device_name", 0);
      oyOption_Release( &o );
    }
  }

  return error;
}

/** @} *//* devices_handling */
