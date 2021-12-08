/** @file oyjl.h
 *
 *  oyjl - Basic helper C API's
 *
 *  @par Copyright:
 *            2010-2021 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl API provides a platformindependent C interface for JSON I/O, conversion to and from
 *            XML + YAML, string helpers, file reading, testing and argument handling.
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de> and Florian Forster  <ff at octo.it>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2010/09/15
 */


#ifndef OYJL_H
#define OYJL_H 1

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

/** \addtogroup oyjl_tree
 *  @{ *//* oyjl_tree */

#define Florian_Forster_SOURCE_GUARD
/*
 * Copyright (c) 2010-2011  Florian Forster  <ff at octo.it>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * The original yajl_val_s structure was renamed to oyjl_val_s by
 * Kai-Uwe Behrmann for libOyjl.
 */

/**
 *  possible data types that a oyjl_val_s can hold */
typedef enum {
    oyjl_t_string = 1,                 /**< @brief a text in UTF-8 */
    oyjl_t_number = 2,                 /**< @brief floating or integer number */
    oyjl_t_object = 3,                 /**< @brief a JSON object */
    oyjl_t_array = 4,                  /**< @brief a JSON array */
    oyjl_t_true = 5,                   /**< @brief boolean true or 1 */
    oyjl_t_false = 6,                  /**< @brief boolean false or 0 */
    oyjl_t_null = 7,                   /**< @brief empty value */
    /** @internal
     *  The any type isn't valid for oyjl_val_s.type, but can be
     *  used as an argument to routines like oyjlTreeGet().
     */
    oyjl_t_any = 8
} oyjl_type;

#define OYJL_NUMBER_INT_VALID          0x01
#define OYJL_NUMBER_DOUBLE_VALID       0x02

/** A pointer to a node in the parse tree */
typedef struct oyjl_val_s * oyjl_val;

/**
 * A JSON value representation capable of holding one of the seven
 * types above. For "string", "number", "object", and "array"
 * additional data is available in the union.  The "OYJL_IS_*"
 * and "OYJL_GET_*" macros below allow type checking and convenient
 * value extraction.
 */
typedef struct oyjl_val_s
{
    /**
     *  Type of the value contained. Use the "OYJL_IS_*" macros to check for a
     *  specific type. */
    oyjl_type type;
    /**
     *  Type-specific data. You may use the "OYJL_GET_*" macros to access these
     *  members. */
    union
    {
        char * string;   /**< @brief UTF-8 text */
        struct {
            long long i; /**< @brief integer value, if representable. */
            double  d;   /**< @brief double value, if representable. */
            char   *r;   /**< @brief unparsed number in string form. */
            /** Signals whether the \em i and \em d members are
             * valid. See \c OYJL_NUMBER_INT_VALID and
             * \c OYJL_NUMBER_DOUBLE_VALID. */
            unsigned int flags;
        } number;        /**< @brief raw and parsed number types */
        struct {
            char **keys; /**< @brief Array of keys */
            oyjl_val *values; /**< @brief Array of values. */
            size_t len; /**< @brief Number of key-value-pairs. */
        } object;        /**< @brief objects with key/value pairs */
        struct {
            oyjl_val *values; /**< @brief Array of elements. */
            size_t len; /**< @brief Number of elements. */
        } array;         /**< @brief series of values */
    } u;
} oyjl_val_s;

/**
 * @brief Parse a string. (libOyjl)
 *
 * Parses an null-terminated string containing JSON data and returns a pointer
 * to the top-level value (root of the parse tree).
 *
 *  This function needs linking to libOyjl.
 *
 *  @see oyjlTreeToJson()
 *
 * \param input              Pointer to a null-terminated utf8 string containing
 *                           JSON data.
 * \param error_buffer       Pointer to a buffer in which an error message will
 *                           be stored if \em oyjlTreeParse() fails, or
 *                           \c NULL. The buffer will be initialized before
 *                           parsing, so its content will be destroyed even if
 *                           \em oyjlTreeParse() succeeds.
 * \param error_buffer_size  Size of the memory area pointed to by
 *                           \em error_buffer_size. If \em error_buffer_size is
 *                           \c NULL, this argument is ignored.
 *
 * \returns Pointer to the top-level value or \c NULL on error. The memory
 * pointed to must be freed using \em oyjlTreeFree(). In case of an error, a
 * null terminated message describing the error in more detail is stored in
 * \em error_buffer if it is not \c NULL.
 */
OYJL_API oyjl_val oyjlTreeParse  ( const char *input,
                                   char *error_buffer, size_t error_buffer_size);

/**
 * Free a parse tree returned by oyjlTreeParse().
 *
 * \param v Pointer to a JSON value returned by oyjlTreeParse(). Passing NULL
 * is valid and results in a no-op.
 */
OYJL_API void oyjlTreeFree ( oyjl_val v );

/**
 * @internal
 * Access a nested value inside a tree.
 *
 * \param parent the node under which you'd like to extract values.
 * \param path A null terminated array of strings, each the name of an object key
 * \param type the oyjl_type of the object you seek, or oyjl_t_any if any will do.
 *
 * \returns a pointer to the found value, or NULL if we came up empty.
 *
 * Future Ideas:  it'd be nice to move path to a string and implement support for
 * a teeny tiny micro language here, so you can extract array elements, do things
 * like .first and .last, even .length.  Inspiration from JSONPath and css selectors?
 * No it wouldn't be fast, but that's not what this API is about.
 */
OYJL_API oyjl_val oyjlTreeGet ( oyjl_val parent, const char ** path, oyjl_type type );

/* Various convenience macros to check the type of a `oyjl_val` */
#define OYJL_IS_STRING(v) (((v) != NULL) && ((v)->type == oyjl_t_string))
#define OYJL_IS_NUMBER(v) (((v) != NULL) && ((v)->type == oyjl_t_number))
#define OYJL_IS_INTEGER(v) (OYJL_IS_NUMBER(v) && ((v)->u.number.flags & OYJL_NUMBER_INT_VALID))
#define OYJL_IS_DOUBLE(v) (OYJL_IS_NUMBER(v) && ((v)->u.number.flags & OYJL_NUMBER_DOUBLE_VALID))
#define OYJL_IS_OBJECT(v) (((v) != NULL) && ((v)->type == oyjl_t_object))
#define OYJL_IS_ARRAY(v)  (((v) != NULL) && ((v)->type == oyjl_t_array ))
#define OYJL_IS_TRUE(v)   (((v) != NULL) && ((v)->type == oyjl_t_true  ))
#define OYJL_IS_FALSE(v)  (((v) != NULL) && ((v)->type == oyjl_t_false ))
#define OYJL_IS_NULL(v)   (((v) != NULL) && ((v)->type == oyjl_t_null  ))

/** @internal
 *  Given a oyjl_val_string return a ptr to the bare string it contains,
 *  or NULL if the value is not a string. */
#define OYJL_GET_STRING(v) (OYJL_IS_STRING(v) ? (v)->u.string : NULL)

/** @internal
 *  Get the string representation of a number.  You should check type first,
 *  perhaps using OYJL_IS_NUMBER */
#define OYJL_GET_NUMBER(v) ((v)->u.number.r)

/** @internal
 *  Get the double representation of a number.  You should check type first,
 *  perhaps using OYJL_IS_DOUBLE */
#define OYJL_GET_DOUBLE(v) ((v)->u.number.d)

/** @internal
 *  Get the 64bit (long long) integer representation of a number.  You should
 *  check type first, perhaps using OYJL_IS_INTEGER */
#define OYJL_GET_INTEGER(v) ((v)->u.number.i)

/** @internal
 *  Get a pointer to a oyjl_val_object or NULL if the value is not an object. */
#define OYJL_GET_OBJECT(v) (OYJL_IS_OBJECT(v) ? &(v)->u.object : NULL)

/** @internal
 *  Get a pointer to a oyjl_val_array or NULL if the value is not an object. */
#define OYJL_GET_ARRAY(v)  (OYJL_IS_ARRAY(v)  ? &(v)->u.array  : NULL)

#undef Florian_Forster_SOURCE_GUARD

#define OYJL_NUMBER_DETECTION 0x01     /**< @brief try to parse values as number */
#if defined(OYJL_HAVE_LIBXML2)
oyjl_val   oyjlTreeParseXml          ( const char        * xml,
                                       int                 flags,
                                       char              * error_buffer,
                                       size_t              error_buffer_size);
#endif
#if defined(OYJL_HAVE_YAML)
oyjl_val   oyjlTreeParseYaml         ( const char        * yaml,
                                       int                 flags,
                                       char              * error_buffer,
                                       size_t              error_buffer_size);
#endif

oyjl_val   oyjlTreeNew               ( const char        * path );
void       oyjlTreeClearValue        ( oyjl_val            root,
                                       const char        * xpath );
void       oyjlTreeToJson            ( oyjl_val            v,
                                       int               * level,
                                       char             ** json );
void       oyjlTreeToYaml            ( oyjl_val            v,
                                       int               * level,
                                       char             ** yaml );
void       oyjlTreeToXml             ( oyjl_val            v,
                                       int               * level,
                                       char             ** xml );
#define    OYJL_PATH                   0x08   /**< @brief  flag to obtain only path */
#define    OYJL_KEY                    0x10   /**< @brief  flat to obtain only keys */
#define    OYJL_NO_ALLOC               0x800  /**< @brief  avoid malloc for oyjlOBJECT_JSON */
char **    oyjlTreeToPaths           ( oyjl_val            v,
                                       int                 child_levels,
                                       const char        * xpath,
                                       int                 flags,
                                       int               * count );
char *     oyjlTreeGetPath           ( oyjl_val            v,
                                       oyjl_val            node );
#define    OYJL_CREATE_NEW             0x02   /**< @brief  flag to allocate a new tree node, in case it is not inside */
oyjl_val   oyjlTreeGetValue          ( oyjl_val            v,
                                       int                 flags,
                                       const char        * path );
oyjl_val oyjlTreeGetNewValueFromArray( oyjl_val            root,
                                       const char        * name,
                                       oyjl_val          * array_ret,
                                       int               * pos_ret );
oyjl_val   oyjlTreeGetValueF         ( oyjl_val            v,
                                       int                 flags,
                                       const char        * format,
                                                           ... );
int        oyjlTreeSetStringF        ( oyjl_val            root,
                                       int                 flags,
                                       const char        * value_text,
                                       const char        * format,
                                                           ... );
int        oyjlTreeSetDoubleF        ( oyjl_val            root,
                                       int                 flags,
                                       double              value,
                                       const char        * format,
                                                           ... );
oyjl_val   oyjlTreeSerialise         ( oyjl_val            v,
                                       int                 flags,
                                       int               * size );
oyjl_val   oyjlTreeDeSerialise       ( oyjl_val            v,
                                       int                 flags,
                                       int                 size );
char *     oyjlValueText             ( oyjl_val            v,
                                       void*             (*alloc)(size_t));
int        oyjlValueCount            ( oyjl_val            v );
oyjl_val   oyjlValuePosGet           ( oyjl_val            v,
                                       int                 pos );
int        oyjlValueSetString        ( oyjl_val            v,
                                       const char        * string );
int        oyjlValueSetDouble        ( oyjl_val            v,
                                       double              value );
void       oyjlValueCopy             ( oyjl_val            v,
                                       oyjl_val            src );
void       oyjlValueClear            ( oyjl_val            v );
#define    OYJL_PATH_MATCH_LEN         0x20   /**< @brief  flag to test if the specified path match with the full length. */
#define    OYJL_PATH_MATCH_LAST_ITEMS  0x40   /**< @brief  flag to test only the last path segments, which are separated by slash '/'. */
int        oyjlPathMatch             ( const char        * path,
                                       const char        * xpath,
                                       int                 flags );

int        oyjlDataFormat            ( const char        * text );
const char * oyjlDataFormatToString  ( int                 format );
#define    OYJL_NO_INDEX               0x20 /**< @brief omit index resolving by squared brackets [] */
#define    OYJL_QUOTE                  0x40 /**< @brief quotation marks '"' */
#define    OYJL_NO_BACKSLASH           0x80 /**< @brief skip back slash '\' */
#define    OYJL_REVERSE                0x100/**< @brief undo */
#define    OYJL_REGEXP                 0x200/**< @brief handle regexp sequences */
char *     oyjlJsonEscape            ( const char        * in,
                                       int                 flags );

#define OYJL_OBSERVE                   0x200000 /**< @brief be verbose on change */
#define OYJL_IS_OBSERVED(v) (((v)!= NULL) && ((v)->u.number.flags & OYJL_OBSERVE))
#define OYJL_SET_OBSERVE(v) if((v)!= NULL) (v)->u.number.flags |= OYJL_OBSERVE;
#define OYJL_UNSET_OBSERVE(v) if((v)!= NULL) (v)->u.number.flags &= (~OYJL_OBSERVE);
/** @} *//* oyjl_tree */

/* --- Core --- */
/** \addtogroup oyjl Oyranos JSON Library
 *  Basic low level API's
 *  @{ *//* oyjl */
/** \addtogroup oyjl_core
 *  @{ */
/** @} */
/** \addtogroup oyjl_tree
 *  @{ */
/** @} */
/** \addtogroup oyjl_args
 *  @{ */
/** @} */


int        oyjlVersion               ( int                 type );

/* --- message helpers --- */
/** @brief message type */
typedef enum {
  oyjlMSG_INFO = 400,                  /**< @brief informational, for debugging */
  oyjlMSG_CLIENT_CANCELED,             /**< @brief user side requested stop */
  oyjlMSG_INSUFFICIENT_DATA,           /**< @brief missing or insufficient data */
  oyjlMSG_ERROR                        /**< @brief error */
} oyjlMSG_e;
/** @brief custom message function type */
typedef int (* oyjlMessage_f)        ( int/*oyjlMSG_e*/    error_code,
                                       const void        * context,
                                       const char        * format,
                                       ... );
int            oyjlMessageFuncSet    ( oyjlMessage_f       message_func );

/* --- i18n helpers --- */
typedef struct oyjlTr_s oyjlTr_s;
int            oyjlInitLanguageDebug ( const char        * project_name,
                                       const char        * env_var_debug,
                                       int               * debug_variable,
                                       int                 use_gettext,
                                       const char        * env_var_locdir,
                                       const char        * default_locdir,
                                       oyjlTr_s         ** context,
                                       oyjlMessage_f       msg );
char *         oyjlLanguage          ( const char        * loc );
char *         oyjlCountry           ( const char        * loc );
const char *   oyjlLang              ( const char        * loc );
char *         oyjlTranslate         ( oyjlTr_s          * context,
                                       const char        * string );
typedef char*(*oyjlTranslate_f)      ( oyjlTr_s          * context,
                                       const char        * string );
#define OYJL_GETTEXT                   0x400000 /**< @brief use gettext */
/* Workaround for chaotic key order. Slow. */
#define OYJL_NO_OPTIMISE               0x800000 /**< @brief skip binary search */
oyjlTr_s *     oyjlTr_New            ( const char        * loc,
                                       const char        * domain,
                                       oyjl_val          * catalog,
                                       oyjlTranslate_f     translator,
                                       void              * user_data,
                                       void             (* deAlloc)(void*),
                                       int                 flags );
oyjlTranslate_f oyjlTr_GetTranslator ( oyjlTr_s          * context );
const char *   oyjlTr_GetLang        ( oyjlTr_s          * context );
const char *   oyjlTr_GetDomain      ( oyjlTr_s          * context );
oyjl_val       oyjlTr_GetCatalog     ( oyjlTr_s          * context );
void *         oyjlTr_GetUserData    ( oyjlTr_s          * context );
int            oyjlTr_GetFlags       ( oyjlTr_s          * context );
void           oyjlTr_SetFlags       ( oyjlTr_s          * context,
                                       int                 flags );
void           oyjlTr_SetLocale      ( oyjlTr_s          * context,
                                       const char        * loc );
void           oyjlTr_Release        ( oyjlTr_s         ** context );
oyjlTr_s *     oyjlTr_Get            ( const char        * domain );
int            oyjlTr_Set            ( oyjlTr_s         ** context );
int            oyjlTr_Unset          ( const char        * domain );
void           oyjlTranslateJson     ( oyjl_val            root,
                                       oyjlTr_s          * context,
                                       const char        * key_list );

void       oyjlDebugVariableSet      ( int               * debug );

/** @} *//* oyjl */

/* --- string helpers --- */
/** \addtogroup oyjl_core
 *  @{ *//* oyjl_core */
char **    oyjlStringSplit           ( const char        * text,
                                       const char          delimiter,
                                       int               * count,
                                       void*            (* alloc)(size_t));
char **    oyjlStringSplit2          ( const char        * text,
                                       const char        * delimiter,
                                       int               * count,
                                       int              ** index,
                                       void*            (* alloc)(size_t));
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
int        oyjlStringReplace         ( char             ** text,
                                       const char        * search,
                                       const char        * replacement,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) );
void       oyjlStringListRelease     ( char            *** l,
                                       int                 size,
                                       void             (* deAlloc)(void*) );
void       oyjlStringListFreeDoubles ( char             ** list,
                                       int               * list_n,
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
void       oyjlStringListAddString   ( char            *** list,
                                       int               * n,
                                       const char        * string,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) );
int        oyjlStringToLong          ( const char        * text,
                                       long              * value );
int        oyjlStringToDouble        ( const char        * text,
                                       double            * value );
int        oyjlStringsToDoubles      ( const char        * text,
                                       const char        * delimiter,
                                       int               * count,
                                       void*            (* alloc)(size_t),
                                       double           ** value );
char *     oyjlRegExpFind            ( char              * text,
                                       const char        * regex );
char *     oyjlRegExpEscape          ( const char        * text );
int        oyjlRegExpReplace         ( char             ** text,
                                       const char        * regex,
                                       const char        * replacement );
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
                                                              void * user_data),
                                       void              * user_data );
typedef enum {
  oyjlNO_MARK,
  oyjlRED,
  oyjlGREEN,
  oyjlBLUE,
  oyjlBOLD,
  oyjlITALIC,
  oyjlUNDERLINE
} oyjlTEXTMARK_e;
const char * oyjlTermColor           ( oyjlTEXTMARK_e      mark,
                                       const char        * text );
const char * oyjlTermColorFromHtml   ( const char        * text,
                                       int                 flags );
char *       oyjlBT                  ( int                 stack_limit );
#define OYJL_DATE           0x01
#define OYJL_TIME           0x02
#define OYJL_TIME_ZONE      0x04
#define OYJL_TIME_ZONE_DIFF 0x08
#define OYJL_BRACKETS       0x10
const char * oyjlPrintTime           ( int                 flags,
                                       oyjlTEXTMARK_e      mark );

/* --- I/O helpers --- */
char *     oyjlReadFileStreamToMem   ( FILE              * fp,
                                       int               * size );
char *     oyjlReadFile              ( const char        * file_name,
                                       int               * size_ptr );
char *     oyjlReadCommandF          ( int               * size,
                                       const char        * mode,
                                       void*            (* alloc)(size_t),
                                       const char        * format,
                                                           ... );
int        oyjlWriteFile             ( const char        * filename,
                                       const void        * mem,
                                       int                 size );
int        oyjlIsFile                ( const char        * fullname,
                                       const char        * mode,
                                       char              * info,
                                       int                 info_len );
void       oyjlLibRelease            ( );
/** @} *//* oyjl_core */

/** \addtogroup oyjl_args

 *  @{ */

/** @brief Types of known objects */
typedef enum {
  oyjlOBJECT_NONE,
  oyjlOBJECT_OPTION = 1769433455,      /**< @brief oyjlOption_s */
  oyjlOBJECT_OPTION_GROUP = 1735879023,/**< @brief oyjlOptionGroup_s */
  oyjlOBJECT_OPTIONS = 1937205615,     /**< @brief oyjlOptions_s */
  oyjlOBJECT_UI_HEADER_SECTION = 1936222575, /**< @brief oyjlUiHeaderSection_s */
  oyjlOBJECT_UI = 1769302383,          /**< @brief oyjlUi_s */
  oyjlOBJECT_TR = 1920231791,          /**< @brief oyjlTr_s */
  oyjlOBJECT_JSON = 1397385583         /**< @brief oyjlNodes_s */
} oyjlOBJECT_e;

/** @brief Type of option */
typedef enum oyjlOPTIONTYPE_e {
    oyjlOPTIONTYPE_START,              /**< */
    oyjlOPTIONTYPE_CHOICE,             /**< list of choices */
    oyjlOPTIONTYPE_FUNCTION,           /**< computed list of choices */
    oyjlOPTIONTYPE_DOUBLE,             /**< IEEE floating point number with double precission */
    oyjlOPTIONTYPE_NONE,               /**< no value possible - the option is a flag like -v/--verbose */
    oyjlOPTIONTYPE_END                 /**< */
} oyjlOPTIONTYPE_e;

/** @brief Types for oyjlVariable_u */
typedef enum oyjlVARIABLETYPE_e {
    oyjlNONE,                          /**< no variable given, will be asked later with oyjlOptions_GetResult() */
    oyjlSTRING,                        /**< pointer to a array of char */
    oyjlDOUBLE,                        /**< IEEE floating point number with double precission */
    oyjlINT                            /**< integer number declared as int */
} oyjlVARIABLE_e;                      /**< @brief type of result */

/** @brief Choice item */
typedef struct oyjlOptionChoice_s {
  char * nick;                         /**< @brief nick / ID as argument for a option */
  char * name;                         /**< @brief i18n short name for labels; In a MAN section and when starting with "http" this string is marked as link. */
  char * description;                  /**< @brief i18n description sentence; can be "" */
  char * help;                         /**< @brief i18n longer help text; can be "" */
} oyjlOptionChoice_s;
void oyjlOptionChoice_Release        ( oyjlOptionChoice_s**choices );
int  oyjlOptionChoice_Count          ( oyjlOptionChoice_s* list );

typedef struct oyjlOptions_s oyjlOptions_s;
typedef struct oyjlOption_s oyjlOption_s;

/** @brief abstract value
 *
 *  The type is declared inside the ::oyjlVARIABLE_e enum range. */
typedef union oyjlVariable_u {
  const char ** s;                     /**< @brief oyjlVARIABLE_STRING */
  double * d;                          /**< @brief oyjlVARIABLE_DOUBLE */
  int * i;                             /**< @brief oyjlVARIABLE_INT */
} oyjlVariable_u;

/** @brief abstract option
 *
 *  The type is declared inside the ::oyjlOPTIONTYPE_e enum range. */
typedef union oyjlOption_u {
  /** @brief oyjlOPTIONTYPE_FUNCTION
   *  The function is well called inside -h for generating help texts. Use OYJL_OPTION_FLAG_EDITABLE to supress this callback with -h.
   *  @param[in]   opt                 the option context
   *  @param[out]  selected            show the default; optional
   *  @param[in]   context             for more information
   *  @result                          the same as for the *choices::list* member; caller owns the memory
   */
  oyjlOptionChoice_s * (*getChoices)( oyjlOption_s * opt, int * selected, oyjlOptions_s * context );
  struct {
    oyjlOptionChoice_s * list;         /**< used for oyjlOPTIONTYPE_CHOICE | oyjlOPTIONTYPE_EDIT */
    int selected;                      /**< the currently selected choice */
  } choices;                           /**< @brief oyjlOPTIONTYPE_CHOICE | oyjlOPTIONTYPE_EDIT */
  struct {
    double d;                          /**< default / recommendation */
    double start;
    double end;
    double tick;
  } dbl;                               /**< @brief oyjlOPTIONTYPE_DOUBLE */
} oyjlOption_u;

#define OYJL_OPTION_FLAG_EDITABLE      0x001 /**< @brief The oyjlOption_s choices are merely a hint. Let users fill other strings too: "prog --opt=arg|..." */
#define OYJL_OPTION_FLAG_ACCEPT_NO_ARG 0x002 /**< @brief Accept as well no arg: "prog --opt[=arg]" */
#define OYJL_OPTION_FLAG_NO_DASH       0x004 /**< @brief No double dash '--' acceptance; single dash can be omitted by not specifying oyjlOption_s::o : "prog opt" */
#define OYJL_OPTION_FLAG_REPETITION    0x008 /**< @brief Accept more than one occurence: "prog --opt=arg ..." */
#define OYJL_OPTION_FLAG_MAINTENANCE   0x100 /**< @brief Maintenance option; can be invisible */
#define OYJL_OPTION_FLAG_IMMEDIATE     0x200 /**< @brief Apply instantly in UI; opposite to ::OYJL_GROUP_FLAG_EXPLICITE */
/** @brief abstract UI option
 *
 *  A oyjlOption_s::o is inside of oyjlOptionGroup_s::detail to be displayed and oyjlOptionGroup_s::mandatory/optional for syntax checking.
 */
struct oyjlOption_s {
  char type[8];                        /**< @brief must be 'oiwi' */
  /** - ::OYJL_OPTION_FLAG_EDITABLE : flag for oyjlOPTIONTYPE_CHOICE and oyjlOPTIONTYPE_FUNCTION. Hints a not closely specified intput. The content is typically not useful for a overview in a help or man page. These can print a overview with oyjlOption_s::value_type. This flag is intented for convinience suggestions or very verbose dictionaries used in scrollable pull down GUI elements.
   *  - ::OYJL_OPTION_FLAG_ACCEPT_NO_ARG : the flagged option can accept as well no argument without error.
   *    - oyjlSTRING will be set to empty string ("")
   *    - oyjlDOUBLE and oyjlINT will be set to 1
   *  - ::OYJL_OPTION_FLAG_NO_DASH can be used for subcommand options in ::OYJL_GROUP_FLAG_SUBCOMMAND flagged groups
   *  - ::OYJL_OPTION_FLAG_REPETITION multi occurence; print trailing ...
   *  - ::OYJL_OPTION_FLAG_IMMEDIATE instant applying; e.g. for cheap status info inside a ::OYJL_GROUP_FLAG_EXPLICITE flagged group
   *  - ::OYJL_OPTION_FLAG_MAINTENANCE accept even without printed visibility
   *  - ::OYJL_NO_OPTIMISE pass values through as is
   */
  unsigned int flags;                  /**< @brief parsing and rendering hints */
  /** '#' is used as default option like a command without any arguments.
   *  '@' together with value_name expects arbitrary arguments as described in oyjlOption_s::value_name.
   *  Reserved letters are ,(comma), \'(quote), \"(double quote), .(dot), :(double point), ;(semikolon), /(slash), \\(backslash)
   *  The letter shall return strlen(o) <= 1.
   *  If zero '\000' terminated, this short :o: option name is not enabled and a long :option: name shall be provided.
   */
  const char * o;                      /**< @brief One letter UTF-8 option name; optional if *option* is present */
  /** The same reserved letters apply as for the oyjlOption_s::o member letter. */
  const char * option;                 /**< @brief String without white space, "my-option"; optional if *o* is present */
  const char * key;                    /**< @brief DB key; optional */
  const char * name;                   /**< @brief i18n label string */
  const char * description;            /**< @brief i18n short sentence about the option */
  const char * help;                   /**< @brief i18n longer text to explain what the option does; optional */
  const char * value_name;             /**< @brief i18n value string; used only for option args; consider using upper case, e.g. FILENAME, NUMBER ... */
  oyjlOPTIONTYPE_e value_type;         /**< @brief type for *values* */
  oyjlOption_u values;                 /**< @brief the selectable values for the option; not used for oyjlOPTIONTYPE_NONE */
  oyjlVARIABLE_e variable_type;        /**< @brief type for *variable* */
  oyjlVariable_u variable;             /**< @brief automatically filled variable depending on *value_type* */
};

/** For a related flag see ::OYJL_OPTION_FLAG_NO_DASH */
#define OYJL_GROUP_FLAG_SUBCOMMAND     0x080 /**< @brief This oyjlOptionGroup_s flag requires one single mandatory option with oyjlOPTIONTYPE_NONE. */
/** For per option exception see ::OYJL_OPTION_FLAG_IMMEDIATE . The flag is intended for e.g. costly processing. */
#define OYJL_GROUP_FLAG_EXPLICITE      0x100 /**< @brief Apply explicitely in UI */
/** Recommend to set a special section name "GENERAL OPTIONS". Usualy it follows the "OPTIONS" section. */
#define OYJL_GROUP_FLAG_GENERAL_OPTS   0x200 /**< @brief Hint for MAN page section */
/**
 *  @brief Info to compile a Syntax line and check missing arguments
 *
 *  Options listed in mandatory, optional and detail are comma(,) separated.
 */
typedef struct oyjlOptionGroup_s {
  char type [8];                       /**< @brief must be 'oiwg' */
  /** - ::OYJL_GROUP_FLAG_SUBCOMMAND : flag inhibits --style mandatory option documentation and uses subcommand style without double dash "--" prefix. */
  unsigned int flags;                  /**< @brief parsing and rendering hints */
  const char * name;                   /**< @brief i18n label string */
  const char * description;            /**< @brief i18n short sentence about the option */
  const char * help;                   /**< @brief i18n longer text to explain what the option does; optional */
  const char * mandatory;              /**< @brief list of mandatory options from a oyjlOption_s::o or oyjlOption_s::option for this group of associated options; one single option here makes a subcommand and is usualy easier to understand */
  const char * optional;               /**< @brief list of non mandatory options from a oyjlOption_s::o or oyjlOption_s::option for this group of associated options */
  const char * detail;                 /**< @brief list of options from a oyjlOption_s::o or oyjlOption_s::option for this group of associated options to display */
} oyjlOptionGroup_s;

/**
 *   @brief Main command line, options and groups
 *
 *  Man sections can be added by using the "man-"man_name identifier/name scheme, e.g. "man-see_also" in oyjlOption_s::option.
 *
 *  @see oyjlUi_ToMan()
 */
struct oyjlOptions_s {
  char type [8];                       /**< @brief must be 'oiws' */
  oyjlOption_s * array;                /**< @brief the options; make shure to add -h|--help and -v|--verbose options */
  oyjlOptionGroup_s * groups;          /**< @brief groups of options, which form a command */
  void * user_data;                    /**< @brief will be passed to functions; optional */
  int argc;                            /**< @brief plain reference from main(argc,argv) */
  const char ** argv;                  /**< @brief plain reference from main(argc,argv) */
  void * private_data;                 /**< internal state; private to OyjlArgs API, do not use */
};
int    oyjlOptions_Count             ( oyjlOptions_s     * opts );
int    oyjlOptions_CountGroups       ( oyjlOptions_s     * opts );
oyjlOption_s * oyjlOptions_GetOption ( oyjlOptions_s     * opts,
                                       const char        * ol );
#define OYJL_QUIET                     0x100000 /**< @brief be silent on error */
oyjlOption_s * oyjlOptions_GetOptionL( oyjlOptions_s     * opts,
                                       const char        * ostring,
                                       int                 flags );
/** @brief option state */
typedef enum {
  oyjlOPTION_NONE,                     /**< untouched */
  oyjlOPTION_USER_CHANGED,             /**< passed in by user */
  oyjlOPTION_MISSING_VALUE,            /**< user error */
  oyjlOPTION_UNEXPECTED_VALUE,         /**< user error */
  oyjlOPTION_NOT_SUPPORTED,            /**< user error */
  oyjlOPTION_DOUBLE_OCCURENCE,         /**< user error; except '@' is specified */
  oyjlOPTIONS_MISSING,                 /**< user error; except '#' is specified */
  oyjlOPTION_NO_GROUP_FOUND,           /**< user error */
  oyjlOPTION_SUBCOMMAND,               /**< category */
  oyjlOPTION_NOT_ALLOWED_AS_SUBCOMMAND /**< user error */
} oyjlOPTIONSTATE_e;
oyjlOptions_s *    oyjlOptions_New   ( int                 argc,
                                       const char       ** argv );
oyjlOPTIONSTATE_e  oyjlOptions_Parse ( oyjlOptions_s     * opts );
oyjlOPTIONSTATE_e  oyjlOptions_GetResult (
                                       oyjlOptions_s     * opts,
                                       const char        * oc,
                                       const char       ** result_string,
                                       double            * result_dbl,
                                       int               * result_int );
char *   oyjlOptions_ResultsToJson   ( oyjlOptions_s     * opts );
char *   oyjlOptions_ResultsToText   ( oyjlOptions_s     * opts );
char **  oyjlOptions_ResultsToList   ( oyjlOptions_s     * opts,
                                       const char        * option,
                                       int               * count );
void     oyjlOptions_SetAttributes   ( oyjlOptions_s     * opts,
                                       oyjl_val          * root );
typedef struct oyjlUi_s oyjlUi_s;
void   oyjlOptions_PrintHelp         ( oyjlOptions_s     * opts,
                                       oyjlUi_s          * ui,
                                       int                 verbose,
                                       const char        * motto_format,
                                                           ... );
/** @brief Header section */
typedef struct oyjlUiHeaderSection_s {
  char type [8];                       /**< @brief must be 'oihs' */
  const char * nick;                   /**< @brief single word well known identifier; *version*, *manufacturer*, *copyright*, *license*, *url*, *support*, *download*, *sources*, *oyjl_modules_author*, *documentation* */
  const char * label;                  /**< @brief i18n short string, in case this section nick is not well known; optional */
  const char * name;                   /**< @brief i18n short content */
  const char * description;            /**< @brief i18n optional second string; might contain a browsable url for further information, e.g. a link to the full text license, home page; optional */
} oyjlUiHeaderSection_s;

/** @brief Info for graphic UI's containing options, additional info sections and other bells and whistles */
struct oyjlUi_s {
  char type [8];                       /**< @brief must be 'oiui' */
  const char * app_type;               /**< @brief "tool" or "module" */
  const char * nick;                   /**< @brief four byte ID for module or plain comand line tool name, e.g. "oyjl-tool" */
  const char * name;                   /**< @brief i18n short name for tool bars, app lists */
  const char * description;            /**< @brief i18n name, version, maybe more for a prominent one liner */
  const char * logo;                   /**< @brief file name body without path, for relocation, nor file type ending; typical a PNG or SVG icon; e.g. "lcms_icon" for lcms_icon.png or lcms_icon.svg; optional */
  /** We describe here a particular tool/module. Each property object contains at least one 'name' key. All values shall be strings. *nick* or *description* keys are optional. If they are not contained, fall back to *name*. Well known objects are *version*, *manufacturer*, *copyright*, *license*, *url*, *support*, *download*, *sources*, *development*, *oyjl_modules_author*, *documentation*, *date* and *logo*. The *modules/[]/nick* shall contain a four byte string in as the CMM identifier. */
  oyjlUiHeaderSection_s * sections;
  oyjlOptions_s * opts;                /**< @brief info for UI logic */
};
oyjlUi_s *         oyjlUi_New        ( int                 argc,
                                       const char       ** argv );
oyjlUi_s *         oyjlUi_Copy       ( oyjlUi_s          * src );
enum {
  oyjlUI_STATE_NONE,                   /**< @brief nothing to report */
  oyjlUI_STATE_HELP,                   /**< @brief --help printed */
  oyjlUI_STATE_VERBOSE = 2,            /**< @brief --verbose option detected */
  oyjlUI_STATE_EXPORT = 4,             /**< @brief --export=XXX or other stuff printed */
  oyjlUI_STATE_OPTION = 24,            /**< @brief bit shift for detected error from option parser */
  oyjlUI_STATE_NO_CHECKS = 0x1000      /**< @brief skip any checks */
};
oyjlUi_s *         oyjlUi_Create     ( int                 argc,
                                       const char       ** argv,
                                       const char        * nick,
                                       const char        * name,
                                       const char        * description,
                                       const char        * logo,
                                       oyjlUiHeaderSection_s * info,
                                       oyjlOption_s   * options,
                                       oyjlOptionGroup_s * groups,
                                       int               * status );
oyjlUi_s *         oyjlUi_FromOptions( const char        * nick,
                                       const char        * name,
                                       const char        * description,
                                       const char        * logo,
                                       oyjlUiHeaderSection_s * info,
                                       oyjlOptions_s     * opts,
                                       int               * status );
void               oyjlUi_ReleaseArgs( oyjlUi_s         ** ui );
void               oyjlUi_Release    ( oyjlUi_s         ** ui );
int      oyjlUiHeaderSection_Count   ( oyjlUiHeaderSection_s * sections );
oyjlUiHeaderSection_s * oyjlUiHeaderSection_Append (
                                       oyjlUiHeaderSection_s * sections,
                                       const char        * nick,
                                       const char        * label,
                                       const char        * name,
                                       const char        * description );
oyjlUiHeaderSection_s * oyjlUi_GetHeaderSection (
                                       oyjlUi_s          * ui,
                                       const char        * nick );
char *             oyjlUi_ToJson     ( oyjlUi_s          * ui,
                                       int                 flags );
char *             oyjlUi_ToMan      ( oyjlUi_s          * ui,
                                       int                 flags );
char *             oyjlUi_ToMarkdown ( oyjlUi_s          * ui,
                                       int                 flags );
char *             oyjlUi_ExportToJson(oyjlUi_s          * ui,
                                       int                 flags );
oyjlUi_s *         oyjlUi_ImportFromJson(
                                       oyjl_val            root,
                                       int                 flags );
#define OYJL_SOURCE_CODE_C             0x01 /**< @brief C programming language source code */
#define OYJL_WITH_OYJL_ARGS_C          0x02 /**< @brief Include oyjl_args.c . Skip libOyjlCode. */
#define OYJL_NO_DEFAULT_OPTIONS        0x04 /**< @brief omit automatic options generation for --help, --X export or --verbose */
#define OYJL_SUGGEST_VARIABLE_NAMES    0x08 /**< @brief automatic suggestion of variable names for missing oyjlOption_s::o and oyjlOption_s::option members */
#define OYJL_COMPLETION_BASH           0x100 /**< @brief bash completion source code */
char *             oyjlUiJsonToCode  ( oyjl_val            root,
                                       int                 flags );
void               oyjlUi_Translate  ( oyjlUi_s          * ui,
                                       oyjlTr_s          * context );

/** link with libOyjlArgsWeb and use microhttps WWW renderer as library \n
 *  link with libOyjlArgsQml and use Qt's QML to render in a GUI
 *  @see oyjlUi_ToJson()
 */
int                oyjlArgsRender    ( int                 argc,
                                       const char       ** argv,
                                       const char        * json,
                                       const char        * commands,
                                       const char        * output,
                                       int                 debug,
                                       oyjlUi_s          * ui,
                                       int               (*callback)(int argc, const char ** argv) );

/** 
 *  @} *//* oyjl_args
 */


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


#endif /* OYJL_H */
