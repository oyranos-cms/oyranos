/** @file oyCMM_s.h
 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2014 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @date     2014/03/26
 */


#ifndef OY_CMM_S__H
#define OY_CMM_S__H

#ifdef __cplusplus
  extern "C" {
#endif /* __cplusplus */


typedef struct oyCMM_s oyCMM_s;

  
#include "oyranos_object.h"
#include "oyranos_module.h"
#include "oyCMMapi_s.h"
  


/**
 *  typedef   oyCMMinfoInit_f
 *  @brief    optional module init function
 *  @ingroup  module_api
 *  @memberof oyCMM_s
 */
typedef int      (*oyCMMinfoInit_f)  ( oyStruct_s        * library );

/** @struct   oyCMM_s
 *  @brief    The CMM API resources struct to implement and set by a CMM
 *  @ingroup  module_api
 *  @extends  oyStruct_s
 *  @see @ref extending_oyranos
 */
struct oyCMM_s {

  const  oyOBJECT_e    type_;          /**< oyOBJECT_CMM_INFO_S The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
  oyStruct_Copy_f      copy;           /**< @brief Copy function */
  oyStruct_Release_f   release;        /**< @brief Release function */
  oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */


  char             cmm[8];             /*!< ICC signature, eg 'lcms' */
  char           * backend_version;    /*!< non translatable, eg "v1.17" */
  /** translated, e.g. "name": "lcms" "little cms" "A CMM with 100k ..."
   *  supported should be "name", "copyright" and "manufacturer".
   *  Optional is "help".
   */
  oyCMMGetText_f   getText;
  char          ** texts;              /**< zero terminated list for getText */

  int              oy_compatibility;   /*!< last supported Oyranos CMM API : OYRANOS_VERSION */
  oyCMMapi_s     * api;                /**< must be casted to a according API, zero terminated list */

  oyIcon_s       * icon;               /*!< module icon */

  oyCMMinfoInit_f  init;      /**< should be called before any access to members of oyCMMinfo_s */

};

/** @brief   Icon data
 *  @ingroup module_api
 *  @extends oyStruct_s
 *
 *  Since: 0.1.8
 */
struct oyIcon_s {
  oyOBJECT_e       type;               /*!< struct type oyOBJECT_ICON_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyPointer        dummy;              /**< keep to zero */
  int              width;              /**< width of data */
  int              height;             /**< height of data */
  const uint8_t  * data;               /*!< RGBA should be sRGB matched */
  const char     * file_list;          /*!< colon ':' delimited list of icon file names, SVG, PNG */
};


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OY_CMM_S__H */

