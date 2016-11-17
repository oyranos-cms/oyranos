/** XcmDDC.h
 *
 *  Xcm Xorg Colour Management
 *
 *  DDC communication with monitor devices 
 *
 *  @par Copyright:
 *            2010 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2010/12/28
 */

#ifndef XCM_DDC_H
#define XCM_DDC_H
#include <stddef.h> /* size_t */

#include "XcmVersion.h"

#if XCM_HAVE_LINUX

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** \addtogroup XcmDDC
 *
 *  @{
 */


/**
 *  @brief   errors specific to XcmDDC
 *
 *  @version libXcm: 0.4.0
 *  @since   2010/12/28 (libXcm: 0.4.0)
 *  @date    2010/12/28
 */
typedef enum {
  XCM_DDC_OK,                          /**< @brief success */
  XCM_DDC_NO_FILE,                     /**< @brief fopen failed, permissions? */
  XCM_DDC_PREPARE_FAIL,            /**< @brief setting EEDID adress failed*/
  XCM_DDC_EDID_READ_FAIL,              /**< @brief could not obtain EEDID */
  XCM_DDC_WRONG_EDID                   /**< @brief wrong EDID header */
} XCM_DDC_ERROR_e;

/* basic access functions */
XCM_DDC_ERROR_e    XcmDDClist        ( char            *** devices,
                                       int               * count );
XCM_DDC_ERROR_e    XcmDDCgetEDID     ( const char        * device,
                                       char             ** data,
                                       size_t            * size );

const char *   XcmDDCErrorToString   ( XCM_DDC_ERROR_e     error );


/** @} XcmDDC */

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* XCM_HAVE_LINUX */

#endif /* XCM_DDC_H */
