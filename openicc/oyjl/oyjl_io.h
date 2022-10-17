/** @file oyjl_io.h
 *
 *  oyjl - Basic i/o helper C API's
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


#ifndef OYJL_IO_H
#define OYJL_IO_H 1

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

/* --- I/O helpers --- */
/** \addtogroup oyjl_io OyjlCore File Input/Output
 *  @{ *//* oyjl_io */
FILE *     oyjlFopen                 ( const char        * file_name,
                                       const char        * mode );
char *     oyjlReadFileStreamToMem   ( FILE              * fp,
                                       int               * size );
#define    OYJL_IO_STREAM              0x100
char *     oyjlReadFile              ( const char        * file_name,
                                       int                 flags,
                                       int               * size_ptr );
char *     oyjlReadCommandF          ( int               * size,
                                       const char        * mode,
                                       void*            (* alloc)(size_t),
                                       const char        * format,
                                                           ... );
char *     oyjlReadFileP             ( FILE              * fp,
                                       int               * size_ptr,
                                       void*            (* alloc)(size_t),
                                       const char        * file_name );
int        oyjlReadFunction          ( int                 argc,
                                       const char       ** argv,
                                       int              (* callback)(int argc, const char ** argv),
                                       void*            (* alloc)(size_t),
                                       int               * size_stdout,
                                       char             ** data_stdout,
                                       int               * size_stderr,
                                       char             ** data_stderr );
int        oyjlWriteFile             ( const char        * filename,
                                       const void        * mem,
                                       int                 size );
#define    OYJL_NO_CHECK               0x1000
int        oyjlIsFile                ( const char        * fullname,
                                       const char        * mode,
                                       int                 flags,
                                       char              * info,
                                       int                 info_len );
int        oyjlHasApplication        ( const char        * app_name);

/** @brief custom file name checker function type
 *
 *  This function allows to implement file i/o access policies for the
 *  oyjlReadFile(), oyjlWriteFile() and oyjlIsFile() funtions.
 *
 *  @param[in,out] filename            filename can be replaced
 *  @param[in]     size                is write file size otherwise read from
 *  @return                            < 0: issue, 0: fine, > 0: error
 */
typedef int (* oyjlFileNameCheck_f)  ( const char       ** full_filename,
                                       int                 write_size );
#define    OYJL_IO_READ                0x2000
#define    OYJL_IO_WRITE               0x4000
int            oyjlFileNameCheckFuncSet (
                                       oyjlFileNameCheck_f check_func,
                                       int                 flags );

/** @} *//* oyjl_io */

#ifdef __cplusplus
}
#endif


#endif /* OYJL_IO_H */
