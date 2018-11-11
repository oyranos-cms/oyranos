/*  @file openicc_config.h
 *
 *  libOpenICC - OpenICC Colour Management Configuration
 *
 *  @par Copyright:
 *            2011-2017 (C) Kai-Uwe Behrmann
 *
 *  @brief    OpenICC Colour Management configuration helpers
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2011/06/27
 */

#ifndef __OPENICC_CONFIG_H__
#define __OPENICC_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "openicc_core.h"

/** \addtogroup OpenICC_config

 *  @{
 */

#define OPENICC_DB_PREFIX "color/settings"
#define OPENICC_DB "openicc.json"

#define OPENICC_BASE_PATH "org/freedesktop/openicc"
#define OPENICC_DEVICE_PATH OPENICC_BASE_PATH "/device"
#define OPENICC_DEVICE_MONITOR "monitor"
#define OPENICC_DEVICE_SCANNER "scanner"
#define OPENICC_DEVICE_PRINTER "printer"
#define OPENICC_DEVICE_CAMERA  "camera"
#define OPENICC_DEVICE_JSON_HEADER \
  "{\n" \
  "  \"org\": {\n" \
  "    \"freedesktop\": {\n" \
  "      \"openicc\": {\n" \
  "        \"device\": {\n" \
  "          \"%s\": [{\n"
#define OPENICC_DEVICE_JSON_FOOTER \
  "          ]\n" \
  "        }\n" \
  "      }\n" \
  "    }\n" \
  "  }\n" \
  "}\n"


typedef enum {
  openiccOBJECT_NONE,
  openiccOBJECT_CONFIG = 312,
  openiccOBJECT_DB,
  openiccOBJECT_OPTION = 1769433455,   /**< @brief openiccOption_s */
  openiccOBJECT_OPTION_GROUP = 1735879023, /**< @brief openiccOptionGroup_s */
  openiccOBJECT_OPTIONS = 1937205615,  /**< @brief openiccOptions_s */
  openiccOBJECT_UI_HEADER_SECTION = 1936222575, /**< @brief openiccUiHeaderSection_s */
  openiccOBJECT_UI = 1769302383        /**< @brief openiccUi_s */
} openiccOBJECT_e;

typedef struct openiccConfig_s openiccConfig_s;

openiccConfig_s  * openiccConfig_FromMem (
                                       const char        * data );
void               openiccConfig_Release (
                                       openiccConfig_s  ** config );
void               openiccConfig_SetInfo (
                                       openiccConfig_s   * config,
                                       const char        * debug_info );

/* Device related APIs */
int                openiccConfig_DevicesCount (
                                       openiccConfig_s   * config,
                                       const char       ** device_classes );
const char *       openiccConfig_DeviceGet (
                                       openiccConfig_s   * config,
                                       const char       ** device_classes,
                                       int                 pos,
                                       char            *** keys,
                                       char            *** values,
                                       openiccAlloc_f      alloc,
                                       openiccDeAlloc_f    dealloc );
#define OPENICC_CONFIGS_SKIP_HEADER 0x01
#define OPENICC_CONFIGS_SKIP_FOOTER 0x02
const char *       openiccConfig_DeviceGetJSON (
                                       openiccConfig_s   * config,
                                       const char       ** device_classes,
                                       int                 pos,
                                       int                 flags,
                                       const char        * device_class,
                                       char             ** json,
                                       openiccAlloc_f      alloc,
                                       openiccDeAlloc_f    dealloc );
char *             openiccConfig_DeviceClassGet (
                                       openiccConfig_s   * config,
                                       openiccAlloc_f      alloc );
const char**       openiccConfigGetDeviceClasses (
                                       const char       ** device_classes,
                                       int               * count );

/* basic key/value APIs */
int                openiccConfig_GetString (
                                       openiccConfig_s   * config,
                                       const char        * xpath,
                                       const char       ** value );
int                openiccConfig_GetStringf (
                                       openiccConfig_s   * config,
                                       const char       ** value,
                                       const char        * xpath_format,
                                       ... );
int                openiccConfig_GetStrings (
                                       openiccConfig_s   * config,
                                       const char       ** xpaths,
                                       openiccAlloc_f      alloc,
                                       char            *** values,
                                       int               * n );
int                openiccConfig_GetKeyNames (
                                       openiccConfig_s   * config,
                                       const char        * xpath,
                                       int                 child_levels,
                                       openiccAlloc_f      alloc,
                                       char            *** key_names,
                                       int               * n );
/** 
 *  @} *//* OpenICC_config
 */

/** \addtogroup path_names Path Names
 *  @brief   Access to data path names for Installation
 *
 *  The obtained informations can be used outside OpenICC, to do system
 *  specific operations, which do not easily fit into the library.
 *  @{ */
/**
 *  @brief   data type selectors
 */
typedef enum {
  openiccPATH_NONE      = 0x00,        /**< */
  openiccPATH_ICC       = 0x01,        /**< ICC profile path */
  openiccPATH_POLICY    = 0x02,        /**< policy data */
  openiccPATH_MODULE    = 0x04,        /**< module data */
  openiccPATH_SCRIPT    = 0x08,        /**< JIT code */
  openiccPATH_CACHE     = 0xf0,        /**< cached files */
  openiccPATH_LOGO      = 0xf1         /**< logo images */
} openiccPATH_TYPE_e;

/** @brief Select Scope for Installation or Configurations
 */
typedef enum {
  openiccSCOPE_USER_SYS = 0x0,         /**< do not limit search to one scope */
  openiccSCOPE_USER = 0x01,            /**< use user data scope */
  openiccSCOPE_SYSTEM = 0x02           /**< use system data scope */
} openiccSCOPE_e;
/** use OpenICC installation data scope */
#define openiccSCOPE_OPENICC  0x04
/** use machine data scope */
#define openiccSCOPE_MACHINE  0x08

char *       openiccGetInstallPath   ( openiccPATH_TYPE_e  type,
                                       openiccSCOPE_e      scope,
                                       openiccAlloc_f      allocFunc );

/** 
 *  @} *//* path_names
 */

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */


#endif /* __OPENICC_CONFIG_H__ */
