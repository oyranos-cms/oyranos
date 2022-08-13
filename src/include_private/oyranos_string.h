/** @file oyranos_string.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2004-2017 (C) Kai-Uwe Behrmann
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
#include "oyranos_json.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* oyNoEmptyName_( name ) */
#define oyNoEmptyName_m_( text_nem ) text_nem?text_nem:"---"
#define oyNoEmptyString_m_ oyNoEmptyName_m_



size_t             oyStrblen_        ( const char        * s );
#define oyStringCopy_ oyStringCopy
char*              oyStringCopy      ( const char        * text,
                                       oyAlloc_f           allocateFunc );
void               oyStringFree_     ( char             ** text,
                                       oyDeAlloc_f         deallocFunc );
char*              oyStringAppend_   ( const char        * text,
                                       const char        * append,
                                       oyAlloc_f           allocateFunc );
int                oyStringFromData_ ( const void        * ptr,
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

#ifdef HAVE_LOCALE_H
#define oyStringAddPrintfC(value, alloc, dealloc, format, ...) \
{ char * save_locale = oyStringCopy( setlocale(LC_NUMERIC, 0 ), oyAllocateFunc_ ); \
  setlocale(LC_NUMERIC, "C"); \
  oyStringAddPrintf (value, alloc,dealloc, format, __VA_ARGS__); \
  setlocale(LC_NUMERIC, save_locale); \
  oyFree_m_( save_locale ); \
}
#else
#define oyStringAddPrintfC(value, alloc, dealloc, format, ...) \
  oyStringAddPrintf(&value, alloc,dealloc, format, __VA_ARGS__);
#endif

#define oyStringSplit_ oyStringSplit
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
void *             oyMemMem          ( const void        * haystack,
                                       size_t              haystacklen,
                                       const void        * needle,
                                       size_t              needlelen );
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
#define oyStringListAppend_ oyStringListCat
char**             oyStringListCat   ( const char       ** list,
                                       int                 n_alt,
                                       const char       ** append,
                                       int                 n_app,
                                       int               * count,
                                       oyAlloc_f           allocateFunc );
#define oyStringListAdd_ oyStringListAdd
void               oyStringListAdd   ( char            *** list,
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
#define oyStringListAddStaticString_ oyStringListAddStaticString
void               oyStringListAddStaticString ( char *** list,
                                       int               * n,
                                       const char        * string,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocateFunc );
#define oyStringListRelease_ oyStringListRelease
void               oyStringListRelease(char            *** l,
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
void               oyStringListReplaceBy (
                                       char             ** list,
                                       int                 list_n,
                                       char *           (* replacer)(const char*, oyAlloc_f),
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deAllocateFunc );
void oyStringListSetHeadingWhiteSpace( char             ** list,
                                       int                 n,
                                       int                 count,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocateFunc );
#define oyStringListFreeDoubles_ oyStringListFreeDoubles
void     oyStringListFreeDoubles     ( char         ** list,
                                       int           * list_n,
                                       oyDeAlloc_f     deallocateFunc );

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

/* oyjl compatible definitions */
#define oyStringAppendN             oyjlStringAppendN
#define oyStringAddN                oyjlStringAddN
#define oyStringListAdd             oyjlStringListAddList
#define oyStringListCat             oyjlStringListCatList
#define oyStringListFreeDoubles     oyjlStringListFreeDoubles
#define oyStringListAddStaticString oyjlStringListPush


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_STRING_H */
