/** @file oyranos_profile.h

   [Template file inheritance graph]
   +-- oyranos_profile.template.h

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


#ifndef OYRANOS_PROFILE_H
#define OYRANOS_PROFILE_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


/** \addtogroup objects_profile ICC Profile APIs
    @brief Select, read, manipulate and write ICC Profiles in memory and on disk.
 *
 *  To open a profile exist several methods in the oyProfile_Fromxxx APIs.
 *  The highest level call is oyProfile_FromName() and supports several names.
 *  oyProfile_FromStd(), oyProfile_FromFile() and oyProfile_FromMem() are specialised
 *  profile open functions. oyProfile_FromSignature() is used for creating a
 *  dummy profile during profile filtering.
 *  oyProfile_FromMD5() is a lookup function from a profile hash, e.g. as
 *  provided by a 'psid' tag. \n
 *  Profile properties can be obtained from oyProfile_GetSignature(), like
 *  color spaces, date, magic number and so on.
 *  oyProfile_GetChannelNames(), oyProfile_GetText() and oyProfile_GetID()
 *  provide additional informations. \n
 *  The profile element functions have Tag in their names. They work together
 *  with the oyProfileTag_s APIs. \n
 *  oyProfile_GetFileName() is a reverse lookup to obtain the name of a installed
 *  file from a profile, e.g. find the name of a reachable display profile.
 *
 *  The oyProfiles_Create() allows to create profile lists with fine grained 
 *  control. oyProfiles_ForStd() creates a list of profiles suitable to be set
 *  as default profiles according to the oyPROFILE_e categories.
 *  oyProfiles_DeviceRank() can be used together with a given device so sort
 *  for a the best match in descenting order.

 *  Low level APIs to obtain pure filenames and handle raw bits are in place:

 *  - @ref profile_lists - provides the ICC profile search path content as string list
 *  - @ref profile_handling - low level ICC profile I/O and more
 *  @{ *//* objects_profile */




/** @brief general profile infos
 *  @ingroup objects_profile
 *
 *  use for oyProfile_GetSignature
 *
 *  @since Oyranos: version 0.1.8
 *  @date  10 december 2007 (API 0.1.8)
 */
typedef enum {
  oySIGNATURE_COLOR_SPACE,            /**< color space */
  oySIGNATURE_PCS,                     /**< profile connection space */
  oySIGNATURE_SIZE,                    /**< internal stored size */
  oySIGNATURE_CMM,                     /**< prefered CMM */
  oySIGNATURE_VERSION,                 /**< version */
  oySIGNATURE_CLASS,                   /**< usage class, e.g. 'mntr' ... */
  oySIGNATURE_MAGIC,                   /**< magic; ICC: 'acsp' */
  oySIGNATURE_PLATFORM,                /**< operating system */
  oySIGNATURE_OPTIONS,                 /**< various ICC header flags */
  oySIGNATURE_MANUFACTURER,            /**< device manufacturer */
  oySIGNATURE_MODEL,                   /**< device modell */
  oySIGNATURE_INTENT,                  /**< seldom used profile claimed intent*/
  oySIGNATURE_CREATOR,                 /**< profile creator ID */
  oySIGNATURE_DATETIME_YEAR,           /**< creation time in UTC */
  oySIGNATURE_DATETIME_MONTH,          /**< creation time in UTC */
  oySIGNATURE_DATETIME_DAY,            /**< creation time in UTC */
  oySIGNATURE_DATETIME_HOURS,          /**< creation time in UTC */
  oySIGNATURE_DATETIME_MINUTES,        /**< creation time in UTC */
  oySIGNATURE_DATETIME_SECONDS,        /**< creation time in UTC */
  oySIGNATURE_ILLUMINANT,              /**< profile illuminant */
  oySIGNATURE_ILLUMINANT_Y,            /**< profile illuminant */
  oySIGNATURE_ILLUMINANT_Z,            /**< profile illuminant */
  oySIGNATURE_ATTRIBUTES,              /**< various ICC header attributes */
  oySIGNATURE_ATTRIBUTES2,             /**< various ICC header attributes */
  oySIGNATURE_MAX
} oySIGNATURE_TYPE_e;

const char   * oyProfileSignatureName( oySIGNATURE_TYPE_e  type );
const char   * oyProfileSignatureDescription (
                                       oySIGNATURE_TYPE_e  type );

/** @} *//* objects_profile */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OYRANOS_PROFILE_H */
