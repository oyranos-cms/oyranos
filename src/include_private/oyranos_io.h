/** @file oyranos_io.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2004-2014 (C) Kai-Uwe Behrmann
 *
 *  @brief    input / output  methods
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2004/11/25
 *
 *  This file contains functions for handling and writing files and directories.
 */

#ifndef OYRANOS_IO_H
#define OYRANOS_IO_H

#include "oyranos_types.h" /* define HAVE_POSIX */

#include <sys/stat.h>
#ifdef HAVE_DIRENT_H
#include <dirent.h>
#else
#include "dirent_custom.h"  /* Posix API must be provided */
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "oyranos_internal.h"
#include "oyranos_helper.h"
#include "oyranos_debug.h"

#if defined(_WIN32)
# define oyPOPEN_m    _popen
# define oyPCLOSE_m   _pclose
#else
# define oyPOPEN_m    popen
# define oyPCLOSE_m   pclose
#endif 

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* --- Helpers  --- */

/* --- static variables   --- */

#define OY_WEB_RGB "sRGB.icc"
#define OY_COLOR_DL_CACHE_DIR "icc_device_link"

#if defined(__APPLE__)
  /* Apples ColorSync default paths */

# define OS_USER_DIR    "~/Library"
# define OS_GLOBAL_DIR  "/Library"
# define OS_MACHINE_DIR "/System/Library"

# define OS_ICC_PATH         "/ColorSync/Profiles"
# define OS_ICC_USER_DIR               OS_USER_DIR         OS_ICC_PATH
# define OS_ICC_SYSTEM_DIR             OS_GLOBAL_DIR       OS_ICC_PATH
# define OS_ICC_MACHINE_DIR            OS_MACHINE_DIR      OS_ICC_PATH
# define CSNetworkPath                 "/Network/Library"  OS_ICC_PATH

# define OS_CACHE_PATH       "/Caches/org.freedesktop.openicc/" OY_COLOR_DL_CACHE_DIR
# define OS_SETTING_PATH     "/Preferences/org.freedesktop.openicc/" OY_SETTINGSDIRNAME
# define OS_SETTINGS_USER_DIR          OS_USER_DIR           OS_SETTING_PATH
# define OS_SETTINGS_SYSTEM_DIR        OS_GLOBAL_DIR         OS_SETTING_PATH
# define OS_SETTINGS_MACHINE_DIR       OS_MACHINE_DIR        OS_SETTING_PATH

#else

# define OS_USER_DIR    "~/."
# define OS_GLOBAL_DIR  "/usr/share/"
# define OS_MACHINE_DIR "/var/"

# define OS_ICC_PATH         "color/" OY_ICCDIRNAME
# define OS_ICC_USER_DIR               OS_USER_DIR "local/"  OS_CACHE_PATH
# define OS_ICC_SYSTEM_DIR             OS_MACHINE_DIR        OS_CACHE_PATH
# define OS_ICC_MACHINE_DIR            OS_MACHINE_DIR "lib/" OS_CACHE_PATH

# define OS_CACHE_PATH       "cache/" OY_COLOR_DL_CACHE_DIR
# define OS_SETTING_PATH     "color/" OY_SETTINGSDIRNAME
# define OS_SETTINGS_USER_DIR          OS_USER_DIR "config/" OS_SETTING_PATH
# define OS_SETTINGS_SYSTEM_DIR        OS_GLOBAL_DIR         OS_SETTING_PATH
# define OS_SETTINGS_MACHINE_DIR       OS_MACHINE_DIR "lib/" OS_SETTING_PATH
#endif

# define OS_ICC_USER_DIR               OS_USER_DIR           OS_ICC_PATH

# define OS_DL_CACHE_USER_DIR          OS_USER_DIR           OS_CACHE_PATH
# define OS_DL_CACHE_SYSTEM_DIR        OS_MACHINE_DIR        OS_CACHE_PATH
# define OS_DL_CACHE_MACHINE_DIR       OS_MACHINE_DIR        OS_CACHE_PATH


extern int oy_warn_;


/* --- internal API definition --- */

/* separate from the external functions */
char *   oyGetPathFromProfileName_   ( const char        * profilename,
                                       int                 flags,
                                       oyAlloc_f           allocate_func );


char **  oyProfileListGet_           ( const char        * colorsig,
                                       uint32_t            flags,
                                       uint32_t          * size );

size_t	 oyGetProfileSize_           ( const char        * fullFileName );
void *   oyGetProfileBlock_          ( const char        * fullFileName,
                                       size_t            * size,
                                       oyAlloc_f           allocate_func );

char**  oyPolicyListGet_                  (int         * size);
char**  oyFileListGet_                    (const char * subpath,
                                           int        * size,
                                           int          data,
                                           int          owner);
char**  oyLibListGet_                     (const char * subpath,
                                           int        * size,
                                           int          owner);
char*   oyLibNameCreate_                 ( const char * lib_base_name,
                                           int          version );


/* oyranos part */
/* check for the global and the users directory */
void oyCheckDefaultDirectories_ ();

/* search in profile path and in current path */
char *   oyFindProfile_              ( const char        * name,
                                       int                 flags);

char * oyFindApplication(const char * app_name);


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_IO_H */
