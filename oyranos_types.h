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

/** \namespace oyranos
    @brief The Oyranos namespace.
 */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void* oyPointer;
typedef struct oyStruct_s oyStruct_s;

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
