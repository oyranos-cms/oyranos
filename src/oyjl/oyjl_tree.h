/** @file oyjl_tree.h
 *
 *  @par Copyright:
 *            2010-2012 (C) Kai-Uwe Behrmann
 *
 *  @brief    object parser extension to yajl
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2010/09/15
 *
 *  This file contains a JSON to in memory object converter.
 *  The data structures can be easily accessed.
 */

#ifndef OYJL_TREE_H
#define OYJL_TREE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/*#include <unistd.h>*/

#include <yajl/yajl_parse.h>


typedef struct oyjl_text_s_ {
  unsigned char           * text;
  unsigned int              len;
} oyjl_text_s;

typedef union oyjl_value_u_ oyjl_value_u;
typedef struct oyjl_value_s_ oyjl_value_s;

typedef enum  oyjl_type_e_ {
  oyjl_type_none,
  oyjl_type_boolean,
  oyjl_type_integer,
  oyjl_type_double,
  oyjl_type_text,
  oyjl_type_array,
  oyjl_type_object
} oyjl_type_e;

typedef struct yail_object_s_ {
  oyjl_text_s               key;
  oyjl_value_s            * value;
} oyjl_object_s;

union oyjl_value_u_ {
  int                       boolean;
  unsigned long             integer;
  double                    floating;
  oyjl_text_s               text;
  oyjl_object_s           * object;
  oyjl_value_s           ** array;
};

struct oyjl_value_s_ {
  oyjl_type_e               type;
  oyjl_value_u              value;
  void                    * priv_;
};


/* intermediate memory handlers, can be replaced once yajl exports its stuff. */
void * oyjl_malloc( size_t size );
void   oyjl_free_memory( void ** ptr );

oyjl_value_s  *  oyjl_value_create   ( );
oyjl_object_s *  oyjl_object_create  ( const unsigned char * key,
                                       const unsigned int    len );
oyjl_text_s *  oyjl_string_create    ( const unsigned char * text,
                                       unsigned int        len );
yajl_status    oyjl_string_free      ( oyjl_text_s      ** text );
unsigned char * oyjl_string_dup      ( const unsigned char * text,
                                       unsigned int        len );
const char * oyjl_print_text         ( oyjl_text_s       * text );
oyjl_value_s  ** oyjl_array_new      ( int                 count );
yajl_status  oyjl_value_array_move_in( oyjl_value_s    *** array,
                                       oyjl_value_s     ** value );
yajl_status  oyjl_map_array_move_in  ( oyjl_object_s   *** array,
                                       oyjl_object_s    ** map );

/** oyjl object parser context */
typedef struct oyjl_object_parse_context_s_ {
  char type[4];            /**< "yopc" */
  oyjl_value_s  ** stack;  /**< the parent levels */
} oyjl_tree_parse_context_s;

oyjl_tree_parse_context_s * oyjl_tree_parse_context_new( void );
int  oyjl_tree_parse_context_free  ( oyjl_tree_parse_context_s ** context );
oyjl_value_s * oyjl_tree_parse_get_current 
                                     ( oyjl_tree_parse_context_s * s );

/* the parser callbacks */
int  oyjl_tree_parse_null            ( void              * context);
int  oyjl_tree_parse_boolean         ( void              * context,
                                       int                 b );
int  oyjl_tree_parse_integer         ( void              * context,
                                       long                i );
int  oyjl_tree_parse_double          ( void              * context,
                                       double              g );
int  oyjl_tree_parse_string          ( void              * context,
                                       const unsigned char * text,
                                       unsigned int        len );
int  oyjl_tree_parse_start_map       ( void              * context );
int  oyjl_tree_parse_map_key         ( void              * context,
                                       const unsigned char * text,
                                       unsigned int        len );
int  oyjl_tree_parse_end_map         ( void              * context );
int  oyjl_tree_parse_start_array     ( void              * context );
int  oyjl_tree_parse_end_array       ( void              * context );


yajl_status    oyjl_tree_from_json   ( const char        * text,
                                       oyjl_value_s     ** object,
                                       void              * user_data );
yajl_status    oyjl_tree_free        ( oyjl_value_s     ** object );
yajl_status    oyjl_tree_print       ( oyjl_value_s      * root,
                                       int               * level,
                                       FILE              * fp );
yajl_status    oyjl_tree_to_json     ( oyjl_value_s      * v,
                                       int               * level,
                                       char             ** json );

oyjl_value_s * oyjl_tree_get_value   ( oyjl_value_s      * root,
                                       const char        * xpath );
oyjl_value_s * oyjl_tree_get_valuef  ( oyjl_value_s      * root,
                                       const char        * xpath_format,
                                                           ... );
int            oyjl_value_count      ( oyjl_value_s      * value );
oyjl_value_s * oyjl_value_pos_get    ( oyjl_value_s      * value,
                                       int                 pos );
char *         oyjl_value_text       ( oyjl_value_s      * value,
                                       void              * (*my_alloc)(size_t len) );

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


#endif /* OYJL_TREE_H */

