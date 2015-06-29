/** @file oyranos_string.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2004-2010 (C) Kai-Uwe Behrmann
 *
 *  @brief    string handling functions
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2010/11/16
 */

#ifndef OYRANOS_STRING_H
#define OYRANOS_STRING_H

#include "oyranos_helper.h"
#include "oyranos_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* oyNoEmptyName_( name ) */
#define oyNoEmptyName_m_( text_nem ) text_nem?text_nem:"---"
#define oyNoEmptyString_m_ oyNoEmptyName_m_

int          oyStringToLong          ( const char        * text,
                                       long              * value );
int          oyStringToDouble        ( const char        * text,
                                       double            * value );
size_t             oyStrblen_        ( const char        * s );
#define oyStringCopy_ oyStringCopy
char*              oyStringCopy      ( const char        * text,
                                       oyAlloc_f           allocateFunc );
void               oyStringFree_     ( char             ** text,
                                       oyDeAlloc_f         deallocFunc );
char*              oyStringAppend_   ( const char        * text,
                                       const char        * append,
                                       oyAlloc_f           allocateFunc );
int                oyStringFromData_ ( const oyPointer     ptr,
                                       size_t              size,
                                       char             ** text,
                                       size_t            * text_size,
                                       oyAlloc_f           allocateFunc );
#define STRING_ADD(t, txt) oyStringAdd_( &t, txt, \
                                         oyAllocateFunc_, oyDeAllocateFunc_ )
void               oyStringAdd_      ( char             ** text,
                                       const char        * append,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocFunc );
#define oyStringAddPrintf_ oyStringAddPrintf
int                oyStringAddPrintf ( char             ** text,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocFunc,
                                       const char        * format,
                                                           ... );

char**             oyStringSplit_    ( const char        * text,
                                       const char          delimiter,
                                       int               * count,
                                       oyAlloc_f           allocateFunc );
char *             oyStrnchr_        ( char              * text,
                                       char                delimiter,
                                       int                 len );
char *             oyStrnchrN_       ( char              * text,
                                       char                delimiter,
                                       int                 len,
                                       int               * end );
int                oyStringCaseCmp_  ( const char        * a,
                                       const char        * b );
int                oyStringSegments_ ( const char        * text,
                                       char                delimiter );
int                oyStringSegmentsN_( const char        * text,
                                       int                 len,
                                       char                delimiter );
char *             oyStringSegment_  ( char              * text,
                                       char                delimiter,
                                       int                 segment,
                                       int               * end );
char *             oyStringSegmentN_ ( char              * text,
                                       int                 len,
                                       char                delimiter,
                                       int                 segment,
                                       int               * end );
char*              oyStringReplace_  ( const char        * text,
                                       const char        * search,
                                       const char        * replacement,
                                       oyAlloc_f           allocateFunc );
char**             oyStringListAppend_(const char       ** list,
                                       int                 n_alt,
                                       const char       ** append,
                                       int                 n_app,
                                       int               * count,
                                       oyAlloc_f           allocateFunc );
void               oyStringListAdd_  ( char            *** list,
                                       int               * n,
                                       const char       ** append,
                                       int                 n_app,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocateFunc );
void               oyStringListAddString_ ( char       *** list,
                                       int               * n,
                                       char             ** string,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocateFunc );
void               oyStringListAddStaticString_ ( char *** list,
                                       int               * n,
                                       const char        * string,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocateFunc );
void               oyStringListRelease_(char           *** l,
                                       int                 size,
                                       oyDeAlloc_f         deallocFunc );
int                oyStringListHas_  ( const char       ** list,
                                       int                 list_n,
                                       const char        * string );
char**             oyStringListFilter_(const char       ** list,
                                       int                 n_alt,
                                       const char        * dir_string,
                                       const char        * string,
                                       const char        * suffix,
                                       int               * count,
                                       oyAlloc_f           allocateFunc );

int                oyIconv           ( const char        * input,
                                       size_t              len_in,
                                       size_t              len_out,
                                       char              * output,
                                       const char        * from_codeset,
                                       const char        * to_codeset );
int                oyIconvGet        ( const char        * text,
                                       void             ** string,
                                       int               * len,
                                       const char        * encoding_from,
                                       const char        * encoding_to,
                                       oyAlloc_f           alloc );



#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_STRING_H */
