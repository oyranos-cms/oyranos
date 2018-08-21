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
 *   * \file oyjl_tree.h
 *
 * Parses JSON data and returns the data in tree form.
 *
 * \author Florian Forster
 * \date August 2010
 *
 * This interface makes quick parsing and extraction of
 * smallish JSON docs trivial:
 *
 * \include example/parse_config.c
 */

#ifndef OYJL_TREE_H
#define OYJL_TREE_H 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifndef OYJL_API
#define OYJL_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  possible data types that a oyjl_val_s can hold */
typedef enum {
    oyjl_t_string = 1,
    oyjl_t_number = 2,
    oyjl_t_object = 3,
    oyjl_t_array = 4,
    oyjl_t_true = 5,
    oyjl_t_false = 6,
    oyjl_t_null = 7,
    /** @internal
     *  The any type isn't valid for oyjl_val_s.type, but can be
     *  used as an argument to routines like oyjl_tree_get().
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
    /** @internal
     *  Type of the value contained. Use the "OYJL_IS_*" macros to check for a
     * specific type. */
    oyjl_type type;
    /** @internal
     *  Type-specific data. You may use the "OYJL_GET_*" macros to access these
     * members. */
    union
    {
        char * string;
        struct {
            long long i; /*< integer value, if representable. */
            double  d;   /*< double value, if representable. */
            char   *r;   /*< unparsed number in string form. */
            /** Signals whether the \em i and \em d members are
             * valid. See \c OYJL_NUMBER_INT_VALID and
             * \c OYJL_NUMBER_DOUBLE_VALID. */
            unsigned int flags;
        } number;
        struct {
            char **keys; /*< Array of keys */
            oyjl_val *values; /*< Array of values. */
            size_t len; /*< Number of key-value-pairs. */
        } object;
        struct {
            oyjl_val *values; /*< Array of elements. */
            size_t len; /*< Number of elements. */
        } array;
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
 *                           be stored if \em oyjl_tree_parse fails, or
 *                           \c NULL. The buffer will be initialized before
 *                           parsing, so its content will be destroyed even if
 *                           \em oyjl_tree_parse succeeds.
 * \param error_buffer_size  Size of the memory area pointed to by
 *                           \em error_buffer_size. If \em error_buffer_size is
 *                           \c NULL, this argument is ignored.
 *
 * \returns Pointer to the top-level value or \c NULL on error. The memory
 * pointed to must be freed using \em oyjl_tree_free. In case of an error, a
 * null terminated message describing the error in more detail is stored in
 * \em error_buffer if it is not \c NULL.
 */
OYJL_API oyjl_val oyjlTreeParse  ( const char *input,
                                   char *error_buffer, size_t error_buffer_size);

/**
 * Free a parse tree returned by "oyjl_tree_parse".
 *
 * \param v Pointer to a JSON value returned by "oyjl_tree_parse". Passing NULL
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

oyjl_val   oyjlTreeNew               ( const char        * path );
void       oyjlTreeClearValue        ( oyjl_val            root,
                                       const char        * xpath );
void       oyjlTreeToJson            ( oyjl_val            v,
                                       int               * level,
                                       char             ** json );
void       oyjlTreeToYaml            ( oyjl_val            v,
                                       int               * level,
                                       char             ** yaml );
#define    OYJL_PATH                   0x08
#define    OYJL_KEY                    0x10
void       oyjlTreeToPaths           ( oyjl_val            v,
                                       int                 child_levels,
                                       const char        * xpath,
                                       int                 flags,
                                       char            *** paths );
#define    OYJL_CREATE_NEW             0x02
oyjl_val   oyjlTreeGetValue          ( oyjl_val            v,
                                       int                 flags,
                                       const char        * path );
oyjl_val   oyjlTreeGetValuef         ( oyjl_val            v,
                                       int                 flags,
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
#define    OYJL_PATH_MATCH_LEN         0x20
#define    OYJL_PATH_MATCH_LAST_ITEMS  0x40
int        oyjlPathMatch             ( const char        * path,
                                       const char        * xpath,
                                       int                 flags );

/* --- Core --- */
int        oyjlVersion               ( int                 type );

/* --- string helpers --- */
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
char*      oyjlStringReplace         ( const char        * text,
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
char **    oyjlStringListCatList ( const char       ** list,
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

/* --- I/O helpers --- */
char *     oyjlReadFileStreamToMem   ( FILE              * fp,
                                       int               * size );

/* --- message helpers --- */
typedef enum {
  oyjlMSG_INFO = 400,
  oyjlMSG_CLIENT_CANCELED,
  oyjlMSG_INSUFFICIENT_DATA,
  oyjlMSG_ERROR
} oyjlMSG_e;
typedef int (* oyjlMessage_f)        ( int/*oyjlMSG_e*/    error_code,
                                       const void        * context,
                                       const char        * format,
                                       ... );
int            oyjlMessageFuncSet    ( oyjlMessage_f      message_func );


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

#endif /* OYJL_TREE_H */
