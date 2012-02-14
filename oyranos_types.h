/** @file oyranos_types.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2004-2010 (C) Kai-Uwe Behrmann
 *
 *  @brief    public types of Oyranos APIs
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2010/11/16
 */


#ifndef OYRANOS_TYPES_H
#define OYRANOS_TYPES_H

#include <stdlib.h> /* for size_t */

#if !defined(WIN32) || (defined(WIN32) && (defined(__CYGWIN__) || defined(__MINGW) || defined(__MINGW32) || defined(__MINGW64__)))
#define HAVE_POSIX 1
#include <inttypes.h> /* uint32_t */
#else
#define uint32_t unsigned __int32
#define uint16_t unsigned __int16
#define uint8_t unsigned char
#define int32_t __int32
#define int16_t __int16
#define intptr_t long int
#define uintptr_t unsigned long int
#define INT32_MAX  (2147483647)
#define INT32_MIN  (-2147483647-1)
#define UINT16_MAX  65535
#endif

/** \namespace oyranos
    @brief The Oyranos namespace.
 */
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


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_TYPES_H */
