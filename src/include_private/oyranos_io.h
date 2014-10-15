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
#ifdef HAVE_POSIX
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

# if defined(__APPLE__)
  /* Apples ColorSync default paths */

# define CSSystemPATH        "/System/Library/ColorSync/Profiles"
# define CSGlobalInstallPATH "/Library/ColorSync/Profiles"
# define CSUserPATH          "~/Library/ColorSync/Profiles"
# define CSNetworkPath       "/Network/Library/ColorSync/Profiles"
# endif

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_IO_H */
