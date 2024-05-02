/** @file oyjl_io_internal.h
 *
 *  oyjl - Extented i/o helper C API's
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


#ifndef OYJL_IO_INTERNAL_H
#define OYJL_IO_INTERNAL_H 1

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
#ifdef _WIN32
#define OYJL_SLASH_C '\\'
#else
#define OYJL_SLASH_C '/'
#endif
const char * oyjlGetHomeDir_ ();
/** @} *//* oyjl_io */

#ifdef __cplusplus
}
#endif


#endif /* OYJL_IO_INTERNAL_H */
