/** XcmEvents.h
 *
 *  A small X11 colour management event observer.
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *             MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @par Copyright:
 *             2009-2013 - Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 */

#ifndef XCM_EVENTS_H
#define XCM_EVENTS_H

#include "XcmVersion.h"
#ifdef XCM_HAVE_X11

#ifdef __cplusplus
extern "C" {
#endif
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#ifdef __cplusplus
}
#endif

#include <stdint.h> /* size_t */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** \addtogroup XcmEvents X Color Management Event API's

 *  @{
 */

/** @brief X Color Management spec atoms */
#define XCM_ICC_COLOUR_SERVER_TARGET_PROFILE_IN_X_BASE "_ICC_DEVICE_PROFILE"
#define XCM_COLOUR_DESKTOP_ADVANCED "_ICC_COLOR_DISPLAY_ADVANCED"
/** @brief _ICC_PROFILE_(xxx) in X spec atoms */
#define XCM_ICC_V0_3_TARGET_PROFILE_IN_X_BASE          "_ICC_PROFILE"

const char * XcmePrintWindowName     ( Display           * display,
                                       Window              w );
const char * XcmePrintWindowRegions  ( Display           * display,
                                       Window              w,
                                       int                 always );
/** @brief context for parsing events */
typedef struct XcmeContext_s_ XcmeContext_s;

XcmeContext_s *
         XcmeContext_New             ( );
XcmeContext_s *
         XcmeContext_Create          ( const char        * display_name );
int      XcmeContext_Setup           ( XcmeContext_s     * c,
                                       const char        * display_name );
int      XcmeContext_Setup2          ( XcmeContext_s     * c,
                                       const char        * display_name,
                                       int                 flags );
int      XcmeContext_Release         ( XcmeContext_s    ** c );
int      XcmeContext_InLoop          ( XcmeContext_s     * c,
                                       XEvent            * event );
Display *XcmeContext_DisplayGet      ( XcmeContext_s     * c );
int      XcmeContext_DisplaySet      ( XcmeContext_s     * c,
                                       Display           * display );
int      XcmeContext_WindowSet       ( XcmeContext_s     * c,
                                       Window              window );

/** @brief customisable signals for a observer */
typedef enum {
  XCME_MSG_TITLE = 400,                /**< @brief basic library title */
  XCME_MSG_COPYRIGHT,                  /**< @brief Copyright infos */
  XCME_MSG_INFO,                       /**< @brief normaly ignored */
  XCME_MSG_SYSTEM,                     /**< @brief forwarded system message */
  XCME_MSG_DISPLAY_ERROR,              /**< @brief errors during run */
  XCME_MSG_DISPLAY_EVENT,              /**< @brief event smessage */
  XCME_MSG_DISPLAY_STATUS              /**< @brief initial status infos */
} XCME_MSG_e;

typedef int  (*XcmMessage_f)         ( XCME_MSG_e            error_code,
                                       const void        * context,
                                       const char        * format,
                                       ... );
int            XcmMessageFuncSet     ( XcmMessage_f        message_func );
void           XcmDebugVariableSet   ( int               * debug );

typedef void*(*XcmICCprofileGetFromMD5_f) (
                                       const void        * md5_hash,
                                       size_t            * size,
                                       void              *(allocate_func)(size_t) );
typedef char*(*XcmICCprofileGetName_f)(const void        * data,
                                       size_t              size,
                                       void              *(allocate_func)(size_t),
                                       int                 file_name );

void           XcmICCprofileGetNameFuncSet
                                     ( XcmICCprofileGetName_f getName );
void           XcmICCprofileFromMD5FuncSet
                                     ( XcmICCprofileGetFromMD5_f fromMD5 );

/** @} XcmEvents */

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* XCM_HAVE_X11 */

#endif /* XCM_EVENTS_H */

