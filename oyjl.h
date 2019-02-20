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
 *   * \file oyjl.h
 *
 * Parses JSON data and returns the data in tree form.
 *
 * \author Florian Forster
 * \date August 2010
 *
 * This interface makes quick parsing and extraction of
 * smallish JSON docs trivial.
 *
 * The original yajl_val_s structure was renamed to oyjl_val_s by
 * Kai-Uwe Behrmann for libOyjl.
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

#define OYJL_NUMBER_INT_VALID    0x01
#define OYJL_NUMBER_DOUBLE_VALID 0x02

/** A pointer to a node in the parse tree */
typedef struct oyjl_val_s * oyjl_val;

/**
 * A JSON value representation capable of holding one of the seven
 * types above. For "string", "number", "object", and "array"
 * additional data is available in the union.  The "OYJL_IS_*"
 * and "OYJL_GET_*" macros below allow type checking and convenient
 * value extraction.
 */
struct oyjl_val_s
{
    /**
     *  Type of the value contained. Use the "OYJL_IS_*" macros to check for a
     * specific type. */
    oyjl_type type;
    /**
     *  Type-specific data. You may use the "OYJL_GET_*" macros to access these
     * members. */
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
};

/**
 * Parse a string.
 *
 * Parses an null-terminated string containing JSON data and returns a pointer
 * to the top-level value (root of the parse tree).
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
void       oyjlTreeToPaths           ( oyjl_val            v,
                                       int                 child_levels,
                                       const char        * xpath,
                                       int                 flags,
                                       char            *** paths );
#define    OYJL_CREATE_NEW             0x02   /**< @brief  flag to allocate a new tree node, in case it is not inside */
oyjl_val   oyjlTreeGetValue          ( oyjl_val            v,
                                       int                 flags,
                                       const char        * path );
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
char *     oyjlValueText             ( oyjl_val            v,
                                       void*             (*alloc)(size_t));
int        oyjlValueCount            ( oyjl_val            v );
oyjl_val   oyjlValuePosGet           ( oyjl_val            v,
                                       int                 pos );
int        oyjlValueSetString        ( oyjl_val            v,
                                       const char        * string );
void       oyjlValueClear            ( oyjl_val            v );
#define    OYJL_PATH_MATCH_LEN         0x20   /**< @brief  flag to test if the specified path match with the full length. */
#define    OYJL_PATH_MATCH_LAST_ITEMS  0x40   /**< @brief  flag to test only the last path segments, which are separated by slash '/'. */
int        oyjlPathMatch             ( const char        * path,
                                       const char        * xpath,
                                       int                 flags );

int        oyjlDataFormat            ( const char        * text );
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
int            oyjlMessageFuncSet    ( oyjlMessage_f      message_func );

/* --- i18n helpers --- */
int oyjlInitLanguageDebug            ( const char        * project_name,
                                       const char        * env_var_debug,
                                       int               * debug_variable,
                                       int                 use_gettext,
                                       const char        * env_var_locdir,
                                       const char        * default_locdir,
                                       const char        * loc_domain,
                                       oyjlMessage_f       msg );

void       oyjlDebugVariableSet      ( int               * debug );

/** @} *//* oyjl */

/* --- string helpers --- */
/** \addtogroup oyjl_core
 *  @{ *//* oyjl_core */
char **    oyjlStringSplit           ( const char        * text,
                                       const char          delimiter,
                                       int               * count,
                                       void*            (* alloc)(size_t));
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
void       oyjlStringListAddStaticString (
                                       char            *** list,
                                       int               * n,
                                       const char        * string,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) );
int        oyjlStringToLong          ( const char        * text,
                                       long              * value );
int        oyjlStringToDouble        ( const char        * text,
                                       double            * value );
int        oyjlStringsToDoubles      ( const char        * text,
                                       char                delimiter,
                                       int               * count,
                                       void*            (* alloc)(size_t),
                                       double           ** value );
typedef struct oyjl_string_s * oyjl_str;
oyjl_str   oyjlStrNew                ( size_t              length,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) );
oyjl_str   oyjlStrNewFrom            ( char             ** text,
                                       size_t              length,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) );
void       oyjlStrRelease            ( oyjl_str          * string_ptr );
const char*oyjlStr                   ( oyjl_str            string );
char *     oyjlStrPull               ( oyjl_str            str );
int        oyjlStrAppendN            ( oyjl_str            string,
                                       const char        * append,
                                       int                 append_len );
int        oyjlStrReplace            ( oyjl_str            text,
                                       const char        * search,
                                       const char        * replacement );

/* --- I/O helpers --- */
char *     oyjlReadFileStreamToMem   ( FILE              * fp,
                                       int               * size );
char *     oyjlReadFile              ( const char        * file_name,
                                       int               * size_ptr );
int        oyjlWriteFile             ( const char        * filename,
                                       void              * mem,
                                       int                 size );
/** @} *//* oyjl_core */

/** \addtogroup oyjl_args

 *  @{ */

typedef enum {
  oyjlOBJECT_NONE,
  oyjlOBJECT_OPTION = 1769433455,      /**< @brief oyjlOption_s */
  oyjlOBJECT_OPTION_GROUP = 1735879023,/**< @brief oyjlOptionGroup_s */
  oyjlOBJECT_OPTIONS = 1937205615,     /**< @brief oyjlOptions_s */
  oyjlOBJECT_UI_HEADER_SECTION = 1936222575, /**< @brief oyjlUiHeaderSection_s */
  oyjlOBJECT_UI = 1769302383           /**< @brief oyjlUi_s */
} oyjlOBJECT_e;

/** @brief Type of option */
typedef enum oyjlOPTIONTYPE_e {
    oyjlOPTIONTYPE_START,              /**< */
    oyjlOPTIONTYPE_CHOICE,             /**< list of choices */
    oyjlOPTIONTYPE_FUNCTION,           /**< computed list of choices */
    oyjlOPTIONTYPE_DOUBLE,             /**< IEEE floating point number with double precission */
    oyjlOPTIONTYPE_STRING,             /**< text string */
    oyjlOPTIONTYPE_NONE,               /**< no value possible - the option is a flag like -v/--verbose */
    oyjlOPTIONTYPE_END                 /**< */
} oyjlOPTIONTYPE_e;

typedef enum oyjlVARIABLETYPE_e {
    oyjlNONE,                          /**< no variable given, will be asked later with oyjlOptions_GetResult() */
    oyjlSTRING,                        /**< pointer to a array of char */
    oyjlDOUBLE,                        /**< IEEE floating point number with double precission */
    oyjlINT                            /**< integer number declared as int */
} oyjlVARIABLE_e;                      /**< @brief type of result */

/** @brief Choice item */
typedef struct oyjlOptionChoice_s {
  char * nick;                         /**< nick / ID as argument for a option */
  char * name;                         /**< i18n short name for labels */
  char * description;                  /**< i18n description sentence; can be "" */
  char * help;                         /**< i18n longer help text; can be "" */
} oyjlOptionChoice_s;
void oyjlOptionChoice_Release        ( oyjlOptionChoice_s**choices );

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
  struct {
    oyjlOptionChoice_s * list;         /**< used for oyjlOPTIONTYPE_CHOICE */
    int selected;                      /**< the currently selected choice */
  } choices;                           /**< @brief oyjlOPTIONTYPE_CHOICE */
  /** @brief oyjlOPTIONTYPE_FUNCTION
   *  @param[in]   opt                 the option context
   *  @param[out]  selected            show the default; optional
   *  @param[in]   context             for more information
   *  @result                          the same as for the *choices::list* member; caller owns the memory
   */
  oyjlOptionChoice_s * (*getChoices)( oyjlOption_s * opt, int * selected, oyjlOptions_s * context );
  struct {
    double d;                          /**< default / recommendation */
    double start;
    double end;
    double tick;
  } dbl;                               /**< @brief oyjlOPTIONTYPE_DOUBLE */
  char * suggest;                      /**< @brief oyjlOPTIONTYPE_STRING initial suggested string of a text field */
} oyjlOption_u;

/** @brief abstract UI option */
struct oyjlOption_s {
  char type[4];                        /**< must be 'oiwi' */
  unsigned int flags;                  /**< unused */
  char o;                              /**< one letter option name; '-' and ' ' are reserved */
  const char * option;                 /**< string without white space, "my-option"; optional if *o* is present */
  const char * key;                    /**< DB key; optional */
  const char * name;                   /**< i18n label string */
  const char * description;            /**< i18n short sentence about the option */
  const char * help;                   /**< i18n longer text to explain what the option does; optional */
  const char * value_name;             /**< i18n value string; used only for option args; consider using upper case, e.g. FILENAME, NUMBER ... */
  oyjlOPTIONTYPE_e value_type;         /**< type for *values* */
  oyjlOption_u values;                 /**< the selectable values for the option; not used for oyjlOPTIONTYPE_NONE */
  oyjlVARIABLE_e variable_type;        /**< type for *variable* */
  oyjlVariable_u variable;             /**< automatically filled variable depending on *value_type* */
};

/**
    @brief info to compile a Syntax line and check missing arguments
 */
typedef struct oyjlOptionGroup_s {
  char type [4];                       /**< must be 'oiwg' */
  unsigned int flags;                  /**< unused */
  const char * name;                   /**< i18n label string */
  const char * description;            /**< i18n short sentence about the option */
  const char * help;                   /**< i18n longer text to explain what the option does; optional */
  const char * mandatory;              /**< list of mandatory one letter options for this group of associated options */
  const char * optional;               /**< list of non mandatory one letter options for this group of associated options */
  const char * detail;                 /**< list of one letter options for this group of associated options to display */
} oyjlOptionGroup_s;

/**
 *   @brief main command line, options and groups
 */
struct oyjlOptions_s {
  char type [4];                       /**< must be 'oiws' */
  oyjlOption_s * array;                /**< the options; make shure to add -h|--help and -v|--verbose options */
  oyjlOptionGroup_s * groups;          /**< groups of options, which form a command */
  void * user_data;                    /**< will be passed to functions; optional */
  int argc;                            /**< plain reference from main(argc,argv) */
  char ** argv;                        /**< plain reference from main(argc,argv) */
  void * private_data;                 /**< internal state; private, do not use */
};
int    oyjlOptions_Count             ( oyjlOptions_s     * opts );
int    oyjlOptions_CountGroups       ( oyjlOptions_s     * opts );
oyjlOption_s * oyjlOptions_GetOption (
                                       oyjlOptions_s     * opts,
                                       char                oc );
oyjlOption_s * oyjlOptions_GetOptionL( oyjlOptions_s     * opts,
                                       const char        * ostring );
/** @brief option state */
typedef enum {
  oyjlOPTION_NONE,                     /**< untouched */
  oyjlOPTION_USER_CHANGED,             /**< passed in by user */
  oyjlOPTION_MISSING_VALUE,            /**< user error */
  oyjlOPTION_UNEXPECTED_VALUE,         /**< user error */
  oyjlOPTION_NOT_SUPPORTED,            /**< user error */
  oyjlOPTION_DOUBLE_OCCURENCE          /**< user error */
} oyjlOPTIONSTATE_e;
oyjlOptions_s *    oyjlOptions_New   ( int                 argc,
                                       char             ** argv );
oyjlOPTIONSTATE_e  oyjlOptions_Parse ( oyjlOptions_s     * opts );
oyjlOPTIONSTATE_e  oyjlOptions_GetResult (
                                       oyjlOptions_s     * opts,
                                       char                oc,
                                       const char       ** result_string,
                                       double            * result_dbl,
                                       int               * result_int );
char * oyjlOptions_ResultsToJson     ( oyjlOptions_s     * opts );
char * oyjlOptions_ResultsToText     ( oyjlOptions_s     * opts );
typedef struct oyjlUi_s oyjlUi_s;
void   oyjlOptions_PrintHelp         ( oyjlOptions_s     * opts,
                                       oyjlUi_s          * ui,
                                       int                 verbose,
                                       const char        * motto_format,
                                                           ... );
/** @brief Header section */
typedef struct oyjlUiHeaderSection_s {
  char type [4];                       /**< must be 'oihs' */
  const char * nick;                   /**< single word well known identifier; *version*, *manufacturer*, *copyright*, *license*, *url*, *support*, *download*, *sources*, *oyjl_modules_author*, *documentation* */
  const char * label;                  /**< i18n short string, in case this section nick is not well known; optional */
  const char * name;                   /**< i18n short content */
  const char * description;            /**< i18n optional second string; might contain a browsable url for further information, e.g. a link to the full text license, home page; optional */
} oyjlUiHeaderSection_s;

/** @brief Info for graphic UI's containing options, additional info sections and other bells and whistles */
struct oyjlUi_s {
  char type [4];                       /**< must be 'oiui' */
  const char * app_type;               /**< "tool" or "module" */
  const char * nick;                   /**< four byte ID for module or plain comand line tool name, e.g. "oyjl-tool" */
  const char * name;                   /**< i18n short name for tool bars, app lists */
  const char * description;            /**< i18n name, version, maybe more for a prominent one liner */
  const char * logo;                   /**< file name body without path, for relocation, nor file type ending; typical a PNG or SVG icon; e.g. "lcms_icon" for lcms_icon.png or lcms_icon.svg; optional */
  /** We describe here a particular tool/module. Each property object contains at least one 'name' key. All values shall be strings. *nick* or *description* keys are optional. If they are not contained, fall back to *name*. Well known objects are *version*, *manufacturer*, *copyright*, *license*, *url*, *support*, *download*, *sources*, *development*, *oyjl_modules_author*, *documentation*, *date* and *logo*. The *modules/[]/nick* shall contain a four byte string in as the CMM identifier. */
  oyjlUiHeaderSection_s * sections;
  oyjlOptions_s * opts;                /**< info for UI logic */
};
oyjlUi_s *         oyjlUi_New        ( int                 argc,
                                       char             ** argv );
enum {
  oyjlUI_STATE_NONE,                   /**< nothing to report */
  oyjlUI_STATE_HELP,                   /**< --help printed */
  oyjlUI_STATE_VERBOSE = 2,            /**< --verbose option detected */
  oyjlUI_STATE_EXPORT = 4,             /**< --export=XXX printed */
  oyjlUI_STATE_OPTION = 24,            /**< bit shift for detected error from option parser */
};
oyjlUi_s *         oyjlUi_Create     ( int                 argc,
                                       char             ** argv,
                                       const char        * nick,
                                       const char        * name,
                                       const char        * description,
                                       const char        * logo,
                                       oyjlUiHeaderSection_s * info,
                                       oyjlOption_s   * options,
                                       oyjlOptionGroup_s * groups,
                                       int               * status );
void               oyjlUi_Release    ( oyjlUi_s         ** ui );
int      oyjlUi_CountHeaderSections  ( oyjlUi_s          * ui );
oyjlUiHeaderSection_s * oyjlUi_GetHeaderSection (
                                       oyjlUi_s          * ui,
                                       const char        * nick );
char *             oyjlUi_ToJson     ( oyjlUi_s          * ui,
                                       int                 flags );
char *             oyjlUi_ToMan      ( oyjlUi_s          * ui,
                                       int                 flags );
char *             oyjlUi_ToMarkdown ( oyjlUi_s          * ui,
                                       int                 flags );

/** 
 *  @} *//* oyjl_args
 */


/* --- compile helpers --- */
#if   defined(__clang__)
#define OYJL_FALLTHROUGH
#elif __GNUC__ >= 7 
#define OYJL_FALLTHROUGH              __attribute__ ((fallthrough));
#else
#define OYJL_FALLTHROUGH
#endif

#if   __GNUC__ >= 7
#define OYJL_DEPRECATED                __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define OYJL_DEPRECATED                __declspec(deprecated)
#else
#define OYJL_DEPRECATED
#endif

#if   (__GNUC__*100 + __GNUC_MINOR__) >= 406
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
