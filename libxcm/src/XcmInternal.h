/*  @file XcmInternal.h
 *
 *  libXcm  Xorg Colour Management
 *
 *  @par Copyright:
 *            2017-2022 (C) Kai-Uwe Behrmann
 *
 *  @brief    X Color Management library
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2017/11/20
 */

#ifndef __XCM_INTERNAL_H__
#define __XCM_INTERNAL_H__

#if   defined(__clang__)
#define XCM_FALLTHROUGH
#elif __GNUC__ >= 7 
#define XCM_FALLTHROUGH                 __attribute__ ((fallthrough));
#else
#define XCM_FALLTHROUGH
#endif

#if   __GNUC__ >= 7
#define XCM_DEPRECATED                  __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define XCM_DEPRECATED                  __declspec(deprecated)
#else
#define XCM_DEPRECATED
#endif

#if   (__GNUC__*100 + __GNUC_MINOR__) >= 406
#define XCM_UNUSED                      __attribute__ ((unused))
#elif defined(_MSC_VER)
#define XCM_UNUSED                      __declspec(unused)
#else
#define XCM_UNUSED
#endif

#endif /* __XCM_INTERNAL_H__ */
