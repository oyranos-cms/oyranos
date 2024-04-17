/** @file oyjl_string.h
 *
 *  oyjl - Basic string C API's
 *
 *  @par Copyright:
 *            2010-2023 (C) Kai-Uwe Behrmann
 *
 *  @brief    OyjlCore API provides a platformindependent C interface for string helpers.
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2010/09/15
 */


#ifndef OYJL_STRING_H
#define OYJL_STRING_H 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef OYJL_API
#define OYJL_API
#endif

#include "oyjl_version.h"

#ifdef __cplusplus
extern "C" {
#endif

/* --- Core --- */
/* --- string helpers --- */
/** \addtogroup oyjl_string
 *  @{ *//* oyjl_string */
char **    oyjlStringSplit           ( const char        * text,
                                       const char          delimiter,
                                       int               * count,
                                       void*            (* alloc)(size_t));
char **    oyjlStringSplit2          ( const char        * text,
                                       const char        * delimiter,
                                       const char        *(splitFunc)( const char * text, const char * delimiter, int * length ),
                                       int               * count,
                                       int              ** index,
                                       void*            (* alloc)(size_t));
#define OYJL_COMPARE_EXACT             0x01           /**< like with strcmp() */
#define OYJL_COMPARE_CASE              0x02           /**< case insensitive like with strcasecmp() */
#define OYJL_COMPARE_LAZY              0x04
#define OYJL_COMPARE_FIND_NEEDLE       0x08
#define OYJL_COMPARE_STARTS_WITH       0x10
int        oyjlStringFind            ( const char        * text,
                                       const char        * pattern,
                                       int                 flags );
#define OYJL_REMOVE                    0x20
#define OYJL_TO_JSON                   0x40
#define OYJL_TO_TEXT                   0x80
int        oyjlStringSplitFind       ( const char        * set,
                                       const char        * delimiters,
                                       const char        * pattern,
                                       int                 flags,
                                       char             ** result,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) );
const char * oyjlStringDelimiter     ( const char        * text,
                                       const char        * delimiter,
                                       int               * length );
int        oyjlStringSplitUTF8       ( const char        * text,
                                       char            *** mbchars,
                                       void*            (* alloc)(size_t) );
char *     oyjlStringCopy            ( const char        * string,
                                       void*            (* alloc)(size_t));
int        oyjlStringAdd             ( char             ** string,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*),
                                       const char        * format,
                                                           ... );
char*      oyjlStringAppendN         ( const char        * text,
                                       const char        * append,
                                       int                 append_len,
                                       void*            (* alloc)(size_t size) );
void       oyjlStringAddN            ( char             ** text,
                                       const char        * append,
                                       int                 append_len,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) );
void       oyjlStringPush            ( char             ** text,
                                       const char        * append,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) );
void       oyjlStringPrepend         ( char             ** text,
                                       const char        * prepend,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) );
int        oyjlStringReplace         ( char             ** text,
                                       const char        * search,
                                       const char        * replacement,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) );
const char * oyjlStringColor         ( oyjlTEXTMARK_e      mark,
                                       int                 flags,
                                       const char        * format,
                                                           ... );
void       oyjlStringListRelease     ( char            *** l,
                                       int                 size,
                                       void             (* deAlloc)(void*) );
void       oyjlStringListFreeDoubles ( char             ** list,
                                       int               * list_n,
                                       void             (* deAlloc)(void*) );
void       oyjlStringListFree        ( char             ** list,
                                       int               * list_n,
                                       int                 start,
                                       int                 count,
                                       void             (* deAlloc)(void*) );
void       oyjlStringListAddList     ( char            *** list,
                                       int               * n,
                                       const char       ** append,
                                       int                 n_app,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) );
char **    oyjlStringListCatList     ( const char       ** list,
                                       int                 n_alt,
                                       const char       ** append,
                                       int                 n_app,
                                       int               * count,
                                       void*            (* alloc)(size_t) );
void       oyjlStringListPush        ( char            *** list,
                                       int               * n,
                                       const char        * string,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) );
int        oyjlStringListAdd         ( char            *** list,
                                       int               * n,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*),
                                       const char        * format,
                                                           ... );
void       oyjlStringListDo          ( char             ** list,
                                       int                 n,
                                       const char        * string,
                                       void             (* listDo)(char**,const char*, void*(*)(size_t),void(*)(void*)),
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) );
int        oyjlStringListFind        ( char             ** list,
                                       int               * list_n,
                                       const char        * pattern,
                                       int                 flags,
                                       void             (* deAlloc)(void*) );
int        oyjlStringToLong          ( const char        * text,
                                       long              * value,
                                       const char       ** end );
#define    OYJL_KEEP_LOCALE            0x01
int        oyjlStringToDouble        ( const char        * text,
                                       double            * value,
                                       const char       ** end,
                                       int                 flags );
int        oyjlStringsToDoubles      ( const char        * text,
                                       const char        * delimiter,
                                       int               * count,
                                       void*            (* alloc)(size_t),
                                       double           ** value );
char *     oyjlRegExpFind            ( char              * text,
                                       const char        * regex,
                                       int               * end_pos );
const char*oyjlRegExpDelimiter       ( const char        * text,
                                       const char        * delimiter,
                                       int               * length );
char *     oyjlRegExpEscape          ( const char        * text );
int        oyjlRegExpReplace         ( char             ** text,
                                       const char        * regex,
                                       const char        * replacement );
int        oyjlStringStartsWith      ( const char        * text,
                                       const char        * pattern,
                                       int                 flags );
typedef struct oyjl_string_s * oyjl_str;
oyjl_str   oyjlStr_New               ( size_t              length,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) );
oyjl_str   oyjlStr_NewFrom           ( char             ** text,
                                       size_t              length,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) );
void       oyjlStr_Release           ( oyjl_str          * string_ptr );
const char*oyjlStr                   ( oyjl_str            string );
char *     oyjlStr_Pull              ( oyjl_str            str );
void       oyjlStr_Clear             ( oyjl_str            string );
int        oyjlStr_AppendN           ( oyjl_str            string,
                                       const char        * append,
                                       int                 append_len );
int        oyjlStr_Push              ( oyjl_str            string,
                                       const char        * append );
int        oyjlStr_Add               ( oyjl_str            string,
                                       const char        * format,
                                                           ... );
int        oyjlStr_Replace           ( oyjl_str            text,
                                       const char        * search,
                                       const char        * replacement,
                                       void             (* modifyReplacement)
                                                             (const char * text,
                                                              const char * start,
                                                              const char * end,
                                                              const char * search,
                                                              const char ** replace,
                                                              int * replace_len,
                                                              void * user_data),
                                       void              * user_data );

/** @} *//* oyjl_string */



#ifdef __cplusplus
}
#endif


#endif /* OYJL_STRING_H */
