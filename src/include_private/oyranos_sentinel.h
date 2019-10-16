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
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2006/07/29
 *
 *  Controls the API functions for triggering the internal state.
 * 
 */


#ifndef OYRANOS_SENTINEL_H
#define OYRANOS_SENTINEL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* library sentinels */
void  oyInit_               (void);
int   oyExportStart_        (int export_check);
int   oyExportReset_        (int export_check);
void  oyExportResetSentinel_(void);
int   oyExportEnd_          (void);

#define EXPORT_CHECK_NO 0x000000
#define EXPORT_SETTING  0x000001
#define EXPORT_PATH     0x000002
#define EXPORT_MONITOR  0x000004
#define EXPORT_CMM      0x000008
#define EXPORT_CMMS     0x00000a
#define EXPORT_I18N     0x000010

#define FINISH_IGNORE_I18N 0x01
#define FINISH_IGNORE_CACHES 0x02
#define FINISH_IGNORE_OBJECT_LIST 0x04
#define FINISH_IGNORE_CORE 0x08
void  oyFinish_             (int flags);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_SENTINEL_H */
