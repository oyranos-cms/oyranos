/** @file oyranos_types.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2004-2010 (C) Kai-Uwe Behrmann
 *
 *  @brief    public types of Oyranos APIs
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2010/11/16
 */


#ifndef OYRANOS_TYPES_H
#define OYRANOS_TYPES_H

#include <stdlib.h> /* for size_t */

#if (defined(__APPLE__) && defined(__MACH__)) || defined(__unix__) || (!defined(_MSC_VER) && (defined(__CYGWIN__) || defined(__MINGW32__) || defined(__MINGW32)))
#define HAVE_POSIX 1
#include <inttypes.h> /* uint32_t */
#else
 #if !defined(_MSC_VER)
  #include <stdint.h>
 #endif

#define uint32_t unsigned __int32
#define uint16_t unsigned __int16
#define uint8_t unsigned char
#define int32_t __int32
#define int16_t __int16
#define intptr_t long int
#define uintptr_t unsigned long int

#define uid_t int
#define gid_t int
#ifndef INT32_MAX
#define INT32_MAX  (2147483647)
#endif
#ifndef INT32_MIN
#define INT32_MIN  (-2147483647-1)
#endif
#ifndef UINT16_MAX
#define UINT16_MAX  65535
#endif
#endif

#define OY_IEEE16BIT_FLOAT_ZERO 0
#define OY_IEEE16BIT_FLOAT_HALF 14336
#define OY_IEEE16BIT_FLOAT_ONE 15360
#define OY_F2I(flt_number_) (*((uint32_t*)&flt_number_))
#define OY_FLOAT2HALF(int_number_) (((OY_F2I(int_number_)>>16)&0x8000)|((((OY_F2I(int_number_)&0x7f800000)-0x38000000)>>13)&0x7c00)|((OY_F2I(int_number_)>>13)&0x03ff))

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void* oyPointer;

/**
 * @param[in] size the size to allocate
 *
 * Place here your allocator, like: \code
   void* myAllocFunc (size_t size)
   { return malloc (size);
   }
 * \endcode<br>
 */
typedef void * (*oyAlloc_f)          ( size_t              size );

/**
 * @param[in] data the pointer to free
 *
 * Place here your deallocator, like: \code
   void* myDeAllocFunc (void *data)
   { return free (data);
   }
 * \endcode<br>
 */
typedef void (*oyDeAlloc_f)          ( void              * data );


#if   __GNUC__ >= 7 
#define OY_FALLTHROUGH                 __attribute__ ((fallthrough));
#elif defined(__clang__)
#define OY_FALLTHROUGH
#else
#define OY_FALLTHROUGH
#endif

#if   __GNUC__ >= 7
#define OY_DEPRECATED                  __attribute__ ((deprecated))
#elif defined(__clang__)
#define OY_DEPRECATED                  __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define OY_DEPRECATED                  __declspec(deprecated)
#else
#define OY_DEPRECATED
#endif

#if   (__GNUC__*100 + __GNUC_MINOR__) >= 406
#define OY_UNUSED                      __attribute__ ((unused))
#elif defined(__clang__)
#define OY_UNUSED                      __attribute__ ((unused))
#elif defined(_MSC_VER)
#define OY_UNUSED                      __declspec(unused)
#else
#define OY_UNUSED
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_TYPES_H */
