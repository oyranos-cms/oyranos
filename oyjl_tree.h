/** @file oyjl_tree.h
 *
 *  oyjl - tree C API's
 *
 *  @par Copyright:
 *            2010-2023 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl API provides a platformindependent C interface for JSON I/O, conversion to and from
 *            XML + YAML.
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de> and Florian Forster  <ff at octo.it>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2010/09/15
 */


#ifndef OYJL_TREE_H
#define OYJL_TREE_H 1

#ifndef OYJL_API
#define OYJL_API
#endif

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

#define OYJL_NUMBER_INT_VALID          0x200
#define OYJL_NUMBER_DOUBLE_VALID       0x400

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

/** @internal
 *  Get a pointer from a array at pos or NULL if the value is out of array len. */
#define OYJL_GET_ARRAY_POS(v, pos)  (OYJL_IS_ARRAY(v) ? (&(v)->u.array)->len > pos ? (&(v)->u.array)->values[pos] : NULL : NULL)

/** @internal
 *  Get a pointer from a 2D array at pos i/j or NULL if the value is out of both array len. */
#define OYJL_GET_ARRAY_2D_POS(v, pos_i, pos_j) ( OYJL_GET_ARRAY_POS(OYJL_GET_ARRAY_POS(v, pos_i), pos_j) )

/** @internal
 *  Get a integer from a 2D array. */
#define OYJL_GET_ARRAY_2D_STRING(v, pos_i, pos_j)  (OYJL_IS_STRING(OYJL_GET_ARRAY_2D_POS(v, pos_i, pos_j)) ? OYJL_GET_STRING(OYJL_GET_ARRAY_2D_POS(v, pos_i, pos_j)) : NULL)

#ifndef INT32_MIN
# define INT32_MIN		(-2147483647-1)
#endif
/** @internal
 *  Get a integer from a 2D array. Return INT32_MIN if not available. */
#define OYJL_GET_ARRAY_2D_INTEGER(v, pos_i, pos_j)  (OYJL_IS_INTEGER(OYJL_GET_ARRAY_2D_POS(v, pos_i, pos_j)) ? OYJL_GET_INTEGER(OYJL_GET_ARRAY_2D_POS(v, pos_i, pos_j)) : INT32_MIN)

/** @internal
 *  Get a double from a 2D array. Return NAN if not available. NAN is defined in math.h . */
#define OYJL_GET_ARRAY_2D_DOUBLE(v, pos_i, pos_j)  (OYJL_IS_DOUBLE(OYJL_GET_ARRAY_2D_POS(v, pos_i, pos_j)) ? OYJL_GET_DOUBLE(OYJL_GET_ARRAY_2D_POS(v, pos_i, pos_j)) : NAN)

#define OYJL_NUMBER_DETECTION 0x1000     /**< @brief try to parse values as number */
#define OYJL_DECIMAL_SEPARATOR_COMMA 0x2000 /**< @brief use comma ',' as decimal separator */
#define OYJL_ALLOW_STATIC     0x10     /**< @brief allow to read static format */
typedef enum {
  oyjlPARSE_STATE_NONE,                /**< @brief nothing to report */
  oyjlPARSE_STATE_NOT_COMPILED,        /**< @brief the format is currently not compiled */
  oyjlPARSE_STATE_FORMAT_ERROR,        /**< @brief check with oyjlDataFormat() */
  oyjlPARSE_STATE_PARSER_ERROR,        /**< @brief message is sent to oyjlMessage_p */
  oyjlPARSE_STATE_RETURN_STATIC,       /**< @brief oyjl_val is static and must not be freed; needs OYJL_ALLOW_STATIC flag */
} oyjlPARSE_STATE_e;
const char*oyjlPARSE_STATE_eToString ( int                 state );
#define OYJL_DELIMITER_COMMA      0x20 /**< @brief ',' comma */
#define OYJL_DELIMITER_SEMICOLON  0x40 /**< @brief ';' semicolon */
oyjl_val   oyjlTreeParse2            ( const char        * text,
                                       int                 flags,
                                       const char        * error_name,
                                       int               * status );
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
oyjl_val   oyjlTreeParseCsv          ( const char        * csv,
                                       const char        * delimiter,
                                       int                 flags,
                                       char              * error_buffer,
                                       size_t              error_buffer_size);

oyjl_val   oyjlTreeNew               ( const char        * path );
void       oyjlTreeClearValue        ( oyjl_val            root,
                                       const char        * xpath );
#define    OYJL_JSON                   0x01   /**< @brief  JSON format; default */
#define    OYJL_YAML                   0x04   /**< @brief  YAML format */
#define    OYJL_XML                    0x08   /**< @brief  XML format */
#define    OYJL_CSV                    0x20   /**< @brief  CSV format - needs 2D array */
#define    OYJL_CSV_SEMICOLON          0x40   /**< @brief  CSV format - needs 2D array */
#define    OYJL_HTML                   0x100  /**< @brief  Formatting flag for markup on output */
#define    OYJL_NO_MARKUP              0x10000/**< @brief  expect plain text */
char *     oyjlTreeToText            ( oyjl_val            v,
                                       int                 flags );
void       oyjlTreeToJson            ( oyjl_val            v,
                                       int               * level,
                                       char             ** json );
void       oyjlTreeToYaml            ( oyjl_val            v,
                                       int               * level,
                                       char             ** yaml );
void       oyjlTreeToXml             ( oyjl_val            v,
                                       int               * level,
                                       char             ** xml );
void       oyjlTreeToCsv             ( oyjl_val            table,
                                       int                 flags,
                                       char             ** csv);
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
int        oyjlTreeSetIntF           ( oyjl_val            root,
                                       int                 flags,
                                       long long           value,
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
int        oyjlValueSetInt           ( oyjl_val            v,
                                       long long           value );
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

#ifdef __cplusplus
}
#endif


#endif /* OYJL_TREE_H */
