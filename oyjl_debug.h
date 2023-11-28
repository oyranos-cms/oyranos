/** @file oyjl_debug.h
 *
 *  oyjl - Debug helper C API's
 *
 *  @par Copyright:
 *            2010-2022 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl API provides a platformindependent C interface for file reading and i/o.
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2010/09/15
 */


#ifndef OYJL_DEBUG_H
#define OYJL_DEBUG_H 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef OYJL_API
#define OYJL_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* --- Core --- */
/* --- debug helpers --- */
/** \addtogroup oyjl_core
 *  @{ *//* oyjl_core */

#define OYJL_NO_BACKTRACE              0x02                /**< @brief skip oyjlBT() output */
char *       oyjlBT                  ( int                 stack_limit );
void         oyjlDebugVariableSet    ( int               * debug );

/* --- message helpers --- */
/** @brief types of terminal color markers */
typedef enum {
  oyjlNO_MARK,                         /**< @brief plain */
  oyjlRED,                             /**< @brief red color */
  oyjlGREEN,                           /**< @brief green color */
  oyjlBLUE,                            /**< @brief blue color */
  oyjlBOLD,                            /**< @brief <strong>bold</strong> */
  oyjlITALIC,                          /**< @brief <em>italic</em> */
  oyjlUNDERLINE                        /**< @brief <u>underline</u> */
} oyjlTEXTMARK_e;
const char * oyjlTermColor           ( oyjlTEXTMARK_e      mark,
                                       const char        * text );
const char * oyjlTermColorPtr        ( oyjlTEXTMARK_e      mark,
                                       char             ** color_text,
                                       const char        * text );
const char * oyjlTermColorF          ( oyjlTEXTMARK_e      mark,
                                       const char        * format,
                                       ... );
const char * oyjlTermColorFPtr       ( oyjlTEXTMARK_e      mark,
                                       char             ** color_text,
                                       const char        * format,
                                       ... );
const char * oyjlTermColorFromHtml   ( const char        * text,
                                       int                 flags );
#define OYJL_WRAP                      0x1000              /**< @brief wrap into format specifix code */
const char * oyjlTermColorToHtml     ( const char        * text,
                                       int                 flags );
const char * oyjlTermColorToPlain    ( const char        * text,
                                       int                 flags );
#define OYJL_FORCE_COLORTERM           0x01                /**< @brief skip environment checks and mark */
#define OYJL_FORCE_NO_COLORTERM        0x02                /**< @brief skip environment checks and do not mark */
#define OYJL_RESET_COLORTERM           0x04                /**< @brief skip cached checks and init again */
int          oyjlTermColorInit       ( int                 flags );
#define OYJL_DATE           0x01
#define OYJL_TIME           0x02
#define OYJL_TIME_MILLI     0x04
#define OYJL_TIME_ZONE      0x08
#define OYJL_TIME_ZONE_DIFF 0x10
#define OYJL_BRACKETS       0x20
const char * oyjlPrintTime           ( int                 flags,
                                       oyjlTEXTMARK_e      mark );
/** @brief message type */
typedef enum {
  oyjlMSG_INFO = 400,                  /**< @brief informational, for debugging */
  oyjlMSG_CLIENT_CANCELED,             /**< @brief user side requested stop */
  oyjlMSG_INSUFFICIENT_DATA,           /**< @brief missing or insufficient data */
  oyjlMSG_ERROR,                       /**< @brief usage error */
  oyjlMSG_PROGRAM_ERROR,                /**< @brief program error */
  oyjlMSG_SECURITY_ALERT              /**< @brief chanceld by security policy */
} oyjlMSG_e;
/** @brief custom message function type
 *
 *  @param         error_code          ::oyjlMSG_e codes and possibly own defined ones
 *  @param         context             user defined data
 *  @param         format              your text
 *  @return                            error
 */
typedef int (* oyjlMessage_f)        ( int/*oyjlMSG_e*/    error_code,
                                       const void        * context,
                                       const char        * format,
                                       ... );
int            oyjlMessageFuncSet    ( oyjlMessage_f       message_func );

/** @} *//* oyjl_core */

#ifdef __cplusplus
}
#endif


#endif /* OYJL_DEBUG_H */
