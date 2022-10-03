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
char *     oyjlReadFileStreamToMem   ( FILE              * fp,
                                       int               * size );
char *     oyjlReadFile              ( const char        * file_name,
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
int        oyjlIsFile                ( const char        * fullname,
                                       const char        * mode,
                                       char              * info,
                                       int                 info_len );
int        oyjlHasApplication        ( const char        * app_name);
/** @} *//* oyjl_io */

#ifdef __cplusplus
}
#endif


#endif /* OYJL_IO_H */
