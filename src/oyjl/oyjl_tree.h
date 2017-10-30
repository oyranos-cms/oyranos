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
 *  @internal
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
#include <yajl/yajl_parse.h>
#ifndef OYJL_API
#define OYJL_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @internal
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
 * @internal
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
 * @internal
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
OYJL_API oyjl_val oyjl_tree_parse (const char *input,
                                   char *error_buffer, size_t error_buffer_size);

/**
 * @internal
 * Free a parse tree returned by "oyjl_tree_parse".
 *
 * \param v Pointer to a JSON value returned by "oyjl_tree_parse". Passing NULL
 * is valid and results in a no-op.
 */
OYJL_API void oyjl_tree_free (oyjl_val v);

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
OYJL_API oyjl_val oyjl_tree_get(oyjl_val parent, const char ** path, oyjl_type type);

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

oyjl_val   oyjl_tree_new             ( const char        * path );
void       oyjl_tree_clear_value     ( oyjl_val            root,
                                       const char        * xpath );
void       oyjl_tree_to_json         ( oyjl_val            v,
                                       int               * level,
                                       char             ** json );
#define    OYJL_PATH                   0x08
#define    OYJL_KEY                    0x10
void       oyjl_tree_to_paths        ( oyjl_val            v,
                                       int                 child_levels,
                                       int                 flags,
                                       char            *** paths );
#define    OYJL_CREATE_NEW             0x02
oyjl_val   oyjl_tree_get_value       ( oyjl_val            v,
                                       int                 flags,
                                       const char        * path );
oyjl_val   oyjl_tree_get_valuef      ( oyjl_val            v,
                                       int                 flags,
                                       const char        * format,
                                                           ... );
char *     oyjl_value_text           ( oyjl_val            v,
                                       void*             (*alloc)(size_t));
int        oyjl_value_count          ( oyjl_val            v );
oyjl_val   oyjl_value_pos_get        ( oyjl_val            v,
                                       int                 pos );
int        oyjl_value_set_string     ( oyjl_val            v,
                                       const char        * string );
void       oyjl_value_clear          ( oyjl_val            v );
int        oyjl_path_match           ( const char        * path,
                                       const char        * xpath );


/* --- string helpers --- */
char **    oyjl_string_split         ( const char        * text,
                                       const char          delimiter,
                                       int               * count,
                                       void*            (* alloc)(size_t));
char *     oyjl_string_copy          ( const char        * string,
                                       void*            (* alloc)(size_t));
int        oyjl_string_add           ( char             ** string,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*),
                                       const char        * format,
                                                           ... );
void       oyjl_string_list_release  ( char            *** l,
                                       int                 size,
                                       void             (* deAlloc)(void*) );
void       oyjl_string_list_free_doubles (
                                       char             ** list,
                                       int               * list_n,
                                       void             (* deAlloc)(void*) );
void       oyjl_string_list_add_list ( char            *** list,
                                       int               * n,
                                       const char       ** append,
                                       int                 n_app,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) );
char **    oyjl_string_list_cat_list ( const char       ** list,
                                       int                 n_alt,
                                       const char       ** append,
                                       int                 n_app,
                                       int               * count,
                                       void*            (* alloc)(size_t) );
void       oyjl_string_list_add_static_string (
                                       char            *** list,
                                       int               * n,
                                       const char        * string,
                                       void*            (* alloc)(size_t),
                                       void             (* deAlloc)(void*) );
int        oyjl_string_to_long       ( const char        * text,
                                       long              * value );

/* --- message helpers --- */
typedef enum {
  oyjl_message_info = 400 + yajl_status_ok,
  oyjl_message_client_canceled,
  oyjl_message_insufficient_data,
  oyjl_message_error
} oyjl_message_e;
typedef yajl_status(*oyjl_message_f) ( oyjl_message_e      error_code,
                                       const void        * context,
                                       const char        * format,
                                       ... );
yajl_status    oyjl_message_func_set ( oyjl_message_f      message_func );


#ifdef __cplusplus
}
#endif

#endif /* OYJL_TREE_H */
