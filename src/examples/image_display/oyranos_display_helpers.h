/**
 *  Oyranos is an open source Color Management System 
 * 
 *  @par Copyright:
 *            2009-2012 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2010/09/05
 *
 *  Oyranos helpers for handling on screen display of images.
 */

#ifndef oyranos_display_helpers_h
#define oyranos_display_helpers_h

#include <oyConversion_s.h>
#include <oyranos_image.h>
#include "oyranos_config_internal.h" /* I18N */
#if defined(HAVE_XCM)
#include <X11/Xcm/Xcm.h>             /* check for proper X11/XCM support */
#endif

extern int oy_display_verbose;

oyConversion_s * oyConversion_FromImageForDisplay  (
                                       oyImage_s         * image_in,
                                       oyImage_s         * image_out,
                                       oyFilterNode_s   ** icc_node,
                                       uint32_t            flags,
                                       oyDATATYPE_e        data_type,
                                       const char        * cc_name,
                                       oyOptions_s       * cc_options,
                                       oyObject_s          obj );
oyConversion_s * oyConversion_FromImageFileNameForDisplay  (
                                       const char        * file_name,
                                       oyFilterNode_s   ** icc_node,
                                       uint32_t            flags,
                                       oyDATATYPE_e        data_type,
                                       const char        * cc_name,
                                       oyOptions_s       * cc_options,
                                       oyObject_s          obj );
int  oyDrawScreenImage               ( oyConversion_s    * context,
                                       oyPixelAccess_s   * ticket,
                                       oyRectangle_s     * display_rectangle,
                                       oyRectangle_s     * old_display_rectangle,
                                       oyRectangle_s     * old_roi_rectangle,
                                       const char        * system_type,
                                       oyDATATYPE_e        data_type_request,
                                       void              * display,
                                       void              * window,
                                       int                 dirty,
                                       oyImage_s         * image );
oyProfile_s * getEditingProfile      ( );

#ifdef XCM_HAVE_X11
#include <X11/Xlib.h>
#endif

#ifndef OY_MIN
#define OY_MIN(a,b)    (((a) <= (b)) ? (a) : (b))
#define OY_MAX(a,b)    (((a) > (b)) ? (a) : (b))
#endif


#endif /* oyranos_display_helpers_h */
