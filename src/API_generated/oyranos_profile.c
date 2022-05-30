/** @file oyranos_profile.c

   [Template file inheritance graph]
   +-- oyranos_profile.template.c

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
#include "oyranos_profile.h"

/** \addtogroup icc_low ICC helpers

 *  @{ *//* icc_low */

/** Function oyProfileSignatureName
 *  @brief   provide UI name for oySIGNATURE_TYPE_e
 *
 *  @version Oyranos: 0.9.4
 *  @date    2013/01/14
 *  @since   2013/01/14 (Oyranos: 0.9.4)
 */
const char   * oyProfileSignatureName( oySIGNATURE_TYPE_e  type )
{
  switch(type) {
    case oySIGNATURE_COLOR_SPACE: return _( "Color Space" );
    case oySIGNATURE_PCS: return _( "PCS" );
    case oySIGNATURE_SIZE: return _( "Size" );
    case oySIGNATURE_CMM: return _( "CMM" );
    case oySIGNATURE_VERSION: return _( "Version" );
    case oySIGNATURE_CLASS: return _( "Class" );
    case oySIGNATURE_MAGIC: return _( "Magic" );
    case oySIGNATURE_PLATFORM: return _( "Platform" );
    case oySIGNATURE_OPTIONS: return _( "Attributes" );
    case oySIGNATURE_MANUFACTURER: return _( "Manufacturer" );
    case oySIGNATURE_MODEL: return _( "Model" );
    case oySIGNATURE_INTENT: return _( "Intent" );
    case oySIGNATURE_CREATOR: return _( "Creator" );
    case oySIGNATURE_ILLUMINANT: return _( "Illuminant" );
    case oySIGNATURE_DATETIME_YEAR: return _( "Date" );
    case oySIGNATURE_DATETIME_MONTH: return _( "Date" );
    case oySIGNATURE_DATETIME_DAY: return _( "Date" );
    case oySIGNATURE_DATETIME_HOURS: return _( "Date" );
    case oySIGNATURE_DATETIME_MINUTES: return _( "Date" );
    case oySIGNATURE_DATETIME_SECONDS: return _( "Date" );
    case oySIGNATURE_MAX: return _("----");
    default: return _("???");
  }
}

/** Function oyProfileSignatureDescription
 *  @brief   provide UI description for oySIGNATURE_TYPE_e
 *
 *  @version Oyranos: 0.9.4
 *  @date    2013/01/14
 *  @since   2013/01/14 (Oyranos: 0.9.4)
 */
const char   * oyProfileSignatureDescription (
                                       oySIGNATURE_TYPE_e  type )
{
  switch(type) {
    case oySIGNATURE_COLOR_SPACE: return _( "Color Space" );
    case oySIGNATURE_PCS: return _( "Profile Connection Space" );
    case oySIGNATURE_SIZE: return _( "Internal stored Size" );
    case oySIGNATURE_CMM: return _( "Prefered CMM" );
    case oySIGNATURE_VERSION: return _( "ICC Version" );
    case oySIGNATURE_CLASS: return _( "ICC Class" );
    case oySIGNATURE_MAGIC: return _( "File Magic" );
    case oySIGNATURE_PLATFORM: return _( "Operating System" );
    case oySIGNATURE_OPTIONS: return _( "ICC header Attributes" );
    case oySIGNATURE_MANUFACTURER: return _( "Device Manufacturer" );
    case oySIGNATURE_MODEL: return _( "Device Model" );
    case oySIGNATURE_INTENT: return _( "Seldom used Profile claimed Intent" );
    case oySIGNATURE_CREATOR: return _( "Profile Creator ID" );
    case oySIGNATURE_ILLUMINANT: return _( "Profile Illuminant" );
    case oySIGNATURE_DATETIME_YEAR: return _( "Creation Time in UTC" );
    case oySIGNATURE_DATETIME_MONTH: return _( "Creation Time in UTC" );
    case oySIGNATURE_DATETIME_DAY: return _( "Creation Time in UTC" );
    case oySIGNATURE_DATETIME_HOURS: return _( "Creation Time in UTC" );
    case oySIGNATURE_DATETIME_MINUTES: return _( "Creation Time in UTC" );
    case oySIGNATURE_DATETIME_SECONDS: return _( "Creation Time in UTC" );
    case oySIGNATURE_MAX: return _("----");
    default: return _("???");
  }
}


/** @} *//* icc_low */


