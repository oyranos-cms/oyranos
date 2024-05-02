/** @file oyjl_macros_compile.h
 *
 *  oyjl - C compile macros
 *
 *  @par Copyright:
 *            2010-2022 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl API provides a platformindependent C interface for JSON I/O, conversion to and from
 *            XML + YAML, string helpers, file reading, testing and argument handling.
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de> and Florian Forster  <ff at octo.it>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2010/09/15
 */


#ifndef OYJL_MACROS_COMPILE_H
#define OYJL_MACROS_COMPILE_H 1

#ifndef OYJL_API
#define OYJL_API
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* --- compile helpers --- */
#if   __GNUC__ >= 7 
#define OYJL_FALLTHROUGH              __attribute__ ((fallthrough));
#elif defined(__clang__)
#define OYJL_FALLTHROUGH
#else
#define OYJL_FALLTHROUGH
#endif

#if   __GNUC__ >= 7
#define OYJL_DEPRECATED                __attribute__ ((deprecated))
#elif defined(__clang__)
#define OYJL_DEPRECATED                __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define OYJL_DEPRECATED                __declspec(deprecated)
#else
#define OYJL_DEPRECATED
#endif

#if   (__GNUC__*100 + __GNUC_MINOR__) >= 406
#define OYJL_UNUSED                    __attribute__ ((unused))
#elif defined(__clang__)
#define OYJL_UNUSED                    __attribute__ ((unused))
#elif defined(_MSC_VER)
#define OYJL_UNUSED                    __declspec(unused)
#else
#define OYJL_UNUSED
#endif


#ifdef __cplusplus
}
#endif


#endif /* OYJL_MACROS_COMPILE_H */
