/** @file oyranos_sentinel.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2006-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    library sentinels
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2006/07/29
 *
 *  Controls the API functions for triggering the internal state.
 * 
 */


#ifndef OYRANOS_SENTINEL_H
#define OYRANOS_SENTINEL_H

/*/#include <oyranos/oyranos_internal.h>*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* library sentinels */
void  oyInit_               (void);
int   oyExportStart_        (int export_check);
int   oyExportReset_        (int export_check);
void  oyExportResetSentinel_(void);
int   oyExportEnd_          (void);
void  oyFinish_             (int unused);

#define EXPORT_CHECK_NO 0x000000
#define EXPORT_SETTING  0x000001
#define EXPORT_PATH     0x000002
#define EXPORT_MONITOR  0x000004
#define EXPORT_CMM      0x000008
#define EXPORT_CMMS     0x00000a
#define EXPORT_I18N     0x000010

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_SENTINEL_H */
