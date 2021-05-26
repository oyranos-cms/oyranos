/*  @file oyjl_tree.c
 *
 *  oyjl - Yajl tree extension
 *
 *  @par Copyright:
 *            2016-2021 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl tree functions
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2016/12/17
 */

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

#include <stdarg.h>  /* va_list */
#include <stddef.h>  /* ptrdiff_t size_t */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>

#include "oyjl_version.h"
#include "oyjl.h"
#include "oyjl_macros.h"
#include "oyjl_tree_internal.h"
#ifdef OYJL_HAVE_LOCALE_H
#include <locale.h>
#endif

/** \addtogroup oyjl_tree
 *  @{ *//* oyjl_tree */

/** @brief   detect data type
 *
 *  @param         text                string
 *  @return                            format
 *                                     - 7 : JSON
 *                                     - 8 : XML
 *                                     - 9 : YAML
 *                                     - 0 : not detected
 *                                     - -1 : no input
 *                                     - -2 : no data
 */
int          oyjlDataFormat          ( const char        * text )
{
  int c,i = 0;
  if(!text)
    return -1;
  /* first simple check */
  while( (c = text[i]) != 0 && (c == ' ' || c == '\t' || c == '\n' || c == '\r' ))
    i++;
  if(strlen(&text[i]) > 5 &&
      ( memcmp( &text[i], "<?xml", 5 ) == 0 ||
        ( text[i] == '<' && text[i+1] != '<' ) ) )
    return 8;
  if(c == '[' || c == '{')
    return 7;
  if((i == 0 || text[i-1] == '\n') && strlen(&text[i]) > 4 && memcmp( &text[i], "---\n", 4 ) == 0)
    return 9;
  if(!c)
    return -2;
  else
    return 0;
}

/** @brief   name data format
 *
 *  @param         format              number from oyjlDataFormat()
 *  @return                            string
 */
const char * oyjlDataFormatToString  ( int                 format )
{
  const char * text = "";
  switch(format)
  {
    case 7: text = "JSON"; break;
    case 8: text = "XML"; break;
    case 9: text = "YAML"; break;
    case 0: text = "unknown"; break;
    case -1: text = "no input"; break;
    case -2: text = "no data"; break;
  }
  return text;
}

#define Florian_Forster_SOURCE_GUARD
static oyjl_val oyjlValueAlloc_(oyjl_type type)
{
    oyjl_val v;

    v = malloc (sizeof (*v));
    if (v == NULL) return (NULL);
    memset (v, 0, sizeof (*v));
    v->type = type;

    return (v);
}

static void oyjlObjectFree_(oyjl_val v)
{
    size_t i;

    if (!OYJL_IS_OBJECT(v)) return;

    for (i = 0; i < v->u.object.len; i++)
    {
        if(v->u.object.keys && v->u.object.keys[i])
        {
          free((char *) v->u.object.keys[i]);
          v->u.object.keys[i] = NULL;
        }
        if(v->u.object.values && v->u.object.values[i])
        {
          oyjlTreeFree (v->u.object.values[i]);
          v->u.object.values[i] = NULL;
        }
    }

    if(v->u.object.keys)
      free((void*) v->u.object.keys);
    if(v->u.object.values)
      free(v->u.object.values);
}

static void oyjlArrayFree_(oyjl_val v)
{
    size_t i;

    if (!OYJL_IS_ARRAY(v)) return;

    for (i = 0; i < v->u.array.len; i++)
    {
        if(v->u.array.values && v->u.array.values[i])
        {
          oyjlTreeFree (v->u.array.values[i]);
          v->u.array.values[i] = NULL;
        }
    }

    if(v->u.array.values)
      free(v->u.array.values);
}

oyjl_val oyjlTreeGet(oyjl_val n, const char ** path, oyjl_type type)
{
    if (!path) return NULL;
    while (n && *path) {
        size_t i;
        size_t len;

        if (n->type != oyjl_t_object) return NULL;
        len = n->u.object.len;
        for (i = 0; i < len; i++) {
            if (!strcmp(*path, n->u.object.keys[i])) {
                n = n->u.object.values[i];
                break;
            }
        }
        if (i == len) return NULL;
        path++;
    }
    if (n && type != oyjl_t_any && type != n->type) n = NULL;
    return n;
}
#undef Florian_Forster_SOURCE_GUARD

int        oyjlPathTermGetIndex_     ( const char        * term,
                                       int               * index );

/** @brief get the value as text string with user allocator */
char * oyjlValueText (oyjl_val v, void*(*alloc)(size_t size))
{
  char * t = 0, * text = 0;

  if(v)
  switch(v->type)
  {
    case oyjl_t_null:
         break;
    case oyjl_t_number:
         oyjlStringAdd (&t, 0,0, "%s", v->u.number.r);
         break;
    case oyjl_t_true:
         oyjlStringAdd (&t, 0,0, "1"); break;
    case oyjl_t_false:
         oyjlStringAdd (&t, 0,0, "0"); break;
    case oyjl_t_string:
         oyjlStringAdd (&t, 0,0, "%s", v->u.string); break;
    case oyjl_t_array:
    case oyjl_t_object:
         break;
    default:
         oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "unknown type: %d", OYJL_DBG_ARGS, v->type );
         break;
  }

  if(t)
  {
    text = oyjlStringCopy (t, alloc);
    free (t); t = 0;
  }

  return text;
}

static void  oyjlTreeFind_           ( oyjl_val            root,
                                       int                 level,
                                       int                 levels,
                                       const char       ** terms,
                                       int                 flags,
                                       const char        * base,
                                       char            *** paths)
{
  int n = 0;
  int pos = -1;
  int i;
  const char * term = NULL;
  int count = oyjlValueCount( root );
  char * xpath = NULL;
  int tn = 0;

  while(terms && terms[tn]) ++tn;
  if(terms && level < tn)
    term = terms[level];

  if(term)
    oyjlPathTermGetIndex_( term, &pos );

  switch(root->type)
  {
    case oyjl_t_null:
    case oyjl_t_number:
    case oyjl_t_true:
    case oyjl_t_false:
    case oyjl_t_string:
         break;
    case oyjl_t_array:
         {
           for(i = 0; i < count; ++i)
           {
             if(pos >= 0 && pos != i) continue;

             if(base)
               oyjlStringAdd( &xpath, 0,0, "%s%s[%d]",base,base[0]?"/":"",i );

             if(flags & OYJL_PATH)
             {
               n = 0; while(paths && *paths && (*paths)[n]) ++n;
               if(tn-1 <= level)
                 oyjlStringListAddStaticString( paths, &n, xpath, malloc,free );
             }

             if(levels != 1)
               oyjlTreeFind_( root->u.array.values[i], level+1, levels-1, terms, flags, xpath, paths );
             if(xpath) { free(xpath); xpath = NULL; }
           }

         } break;
    case oyjl_t_object:
         {
           for(i = 0; i < count; ++i)
           {
             int ocount = oyjlValueCount( root->u.object.values[i]);
             const char * key = root->u.object.keys[i];

             if(pos >= 0 && pos != i)
               continue;
             if(pos <  0 && term && key && !(strcmp(term,key) == 0 ||
                                             term[0] == '\000'))
               continue;

             if(base && key)
               oyjlStringAdd( &xpath, 0,0, "%s%s%s", base,base[0]?"/":"", key );

             if( (flags & OYJL_PATH && ocount) ||
                 (flags & OYJL_KEY && ocount == 0) )
             {
               n = 0; while(paths && *paths && (*paths)[n]) ++n;
               if(tn-1 <= level)
                 oyjlStringListAddStaticString( paths, &n, xpath, malloc,free );
             }

             if(levels != 1)
               oyjlTreeFind_( root->u.object.values[i], level+1, levels-1, terms, flags, xpath, paths );
             if(xpath) { free(xpath); xpath = NULL; }
           }
         }
         break;
    default:
         oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "unknown type: %d", OYJL_DBG_ARGS, root->type );
         break;
  }
}
/** @brief find matching paths
 *
 *  The function works on the whole tree to match a xpath.
 *
 *  @param         root                node
 *  @param         levels              desired level depth
 *  @param         xpath               extented path expression;
 *                                     It accepts even empty terms.
 *  @param         flags               support filters:
 *                                     - ::OYJL_KEY: only keys
 *                                     - ::OYJL_PATH: only paths
 *                                     - 0 for both, paths and keys
 *  @param         paths               the resulting string list
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/11/12
 *  @since   2017/11/10 (Oyranos: 0.9.7)
 */
void       oyjlTreeToPaths           ( oyjl_val            root,
                                       int                 levels,
                                       const char        * xpath,
                                       int                 flags,
                                       char            *** paths )
{
  int pos = 0;
  int n = 0;
  char * base = NULL;
  char ** terms = oyjlStringSplit(xpath, '/', &n, malloc);

  if(!root) return;

  if(!flags) flags = OYJL_PATH | OYJL_KEY;

  while(paths && *paths && (*paths)[pos]) ++pos;
  if(paths && pos)
    base = oyjlStringCopy( (*paths)[pos-1], malloc );
  else
    base = oyjlStringCopy( "",malloc );

  if(base)
  {
    oyjlTreeFind_( root, 0, levels, (const char**) terms, flags, base, paths );

    free(base);
  }

  oyjlStringListRelease( &terms, n, free );
}

static void oyjlJsonIndent_( char ** json, const char * before, int level, const char * after )
{
  char * njson;
  int len;

  if(!json) return;

  len = *json ? strlen(*json) : 0;
  len += before ? strlen( before ) : 0;
  len += level;
  len += after ? strlen( after ) : 0;
  len += 1;

  njson = malloc( len );
  if(!njson) return;
  njson[0] = 0;

  if(*json)
  {
    strcpy( njson, *json );
    free( *json );
  }
  if(before) strcpy( &njson[strlen(njson)], before );
  if(level)
  {
    len = strlen(njson);
    memset( &njson[len], ' ', level );
    njson[len+level] = '\000';
  }
  if(after) strcpy( &njson[strlen(njson)], after );
  *json = njson;
}

char * oyjlJsonEscape( const char * in )
{
  char * out = NULL;
  const char * t = in;
  oyjl_str tmp;
  if(!in) return NULL;

  tmp = oyjlStrNew(10,0,0);
  oyjlStrAppendN( tmp, t, strlen(t) );
  oyjlStrReplace( tmp, "\\", "\\\\", 0, NULL );
  oyjlStrReplace( tmp, "\"", "\\\"", 0, NULL );
  oyjlStrReplace( tmp, "\b", "\\b", 0, NULL );
  oyjlStrReplace( tmp, "\f", "\\f", 0, NULL );
  oyjlStrReplace( tmp, "\n", "\\n", 0, NULL );
  oyjlStrReplace( tmp, "\r", "\\r", 0, NULL );
  oyjlStrReplace( tmp, "\t", "\\t", 0, NULL );
  out = oyjlStrPull(tmp); 
  oyjlStrRelease( &tmp );
  return out;
}

int  oyjlTreeToJson21_(oyjl_val v, int * level, oyjl_str json)
{
  int error = 0;
  if(v)
  switch(v->type)
  {
    case oyjl_t_null:
         oyjlStrAppendN (json, "null", 4); break;
         break;
    case oyjl_t_number:
         oyjlStrAppendN (json, v->u.number.r, strlen(v->u.number.r));
         break;
    case oyjl_t_true:
         oyjlStrAppendN (json, "1", 1); break;
    case oyjl_t_false:
         oyjlStrAppendN (json, "0", 1); break;
    case oyjl_t_string:
         {
          char * escaped = oyjlJsonEscape( v->u.string );
          oyjlStrAppendN( json, "\"", 1 );
          oyjlStrAppendN( json, escaped, strlen(escaped) );
          oyjlStrAppendN( json, "\"", 1 );
          free( escaped );
         }
         break;
    case oyjl_t_array:
         {
           int i,
               count = v->u.array.len;

           oyjlStrAppendN( json, "[", 1 );

           *level += 2;
           for(i = 0; i < count; ++i)
           {
             oyjlTreeToJson21_( v->u.array.values[i], level, json );
             if(count > 1)
             {
               if(i < count - 1)
                 oyjlStrAppendN( json, ",", 1 );
             }
           }
           *level -= 2;

           oyjlStrAppendN( json, "]", 1 );
         } break;
    case oyjl_t_object:
         {
           int i,j,
               count = v->u.object.len;

           oyjlStrAppendN( json, "{", 1 );

           *level += 2;
           for(i = 0; i < count; ++i)
           {
             oyjlStrAppendN( json, "\n", 1 );
             for(j = 0; j < *level; ++j) oyjlStrAppendN( json, " ", 1 );
             if(!v->u.object.keys || !v->u.object.keys[i])
             {
               oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "missing key", OYJL_DBG_ARGS );
               return 1;
             }
             oyjlStrAppendN( json, "\"", 1 );
             {
              char * escaped = oyjlJsonEscape( v->u.object.keys[i] );
              oyjlStrAppendN( json, escaped, strlen(escaped) );
              free( escaped );
             }
             oyjlStrAppendN( json, "\": ", 3 );
             error = oyjlTreeToJson21_( v->u.object.values[i], level, json );
             if(error) return error;
             if(count > 1)
             {
               if(i < count - 1)
                 oyjlStrAppendN( json, ",", 1 );
             }
           }
           *level -= 2;

           oyjlStrAppendN( json, "\n", 1 );
           for(j = 0; j < *level; ++j) oyjlStrAppendN( json, " ", 1 );
           oyjlStrAppendN( json, "}", 1 );
         }
         break;
    default:
         oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "unknown type: %d", OYJL_DBG_ARGS, v->type );
         break;
  }
  return 0;
}
/** @brief convert a C tree into a JSON string
 *
 *  @see oyjlTreeParse()
 */
void oyjlTreeToJson (oyjl_val v, int * level, char ** json)
{
  oyjl_str string = oyjlStrNew(10, 0,0);
  oyjlTreeToJson21_( v, level, string );
  if(oyjlStr(string))
    *json = oyjlStrPull(string);
  else
    *json = NULL;
  oyjlStrRelease( &string );
}

void oyjlTreeToJson2_ (oyjl_val v, int * level, char ** json)
{
  if(v)
  switch(v->type)
  {
    case oyjl_t_null:
         break;
    case oyjl_t_number:
         oyjlStringAdd (json, 0,0, "%s", v->u.number.r);
         break;
    case oyjl_t_true:
         oyjlStringAdd (json, 0,0, "1"); break;
    case oyjl_t_false:
         oyjlStringAdd (json, 0,0, "0"); break;
    case oyjl_t_string:
         {
          const char * t = v->u.string;
          char * tmp = oyjlStringCopy(t,malloc);
          oyjlStringReplace( &tmp, "\\", "\\\\", 0, 0);
          oyjlStringReplace( &tmp, "\"", "\\\"", 0, 0);
          oyjlStringReplace( &tmp, "\b", "\\b", 0, 0);
          oyjlStringReplace( &tmp, "\f", "\\f", 0, 0);
          oyjlStringReplace( &tmp, "\n", "\\n", 0, 0);
          oyjlStringReplace( &tmp, "\r", "\\r", 0, 0);
          oyjlStringReplace( &tmp, "\t", "\\t", 0, 0);
          oyjlStringAdd (json, 0,0, "\"%s\"", tmp);
          if(tmp) free(tmp);
         }
         break;
    case oyjl_t_array:
         {
           int i,
               count = v->u.array.len;

           oyjlStringAdd( json, 0,0, "[" );

           *level += 2;
           for(i = 0; i < count; ++i)
           {
             oyjlTreeToJson2_( v->u.array.values[i], level, json );
             if(count > 1)
             {
               if(i < count - 1)
                 oyjlStringAdd( json, 0,0, "," );
             }
           }
           *level -= 2;

           oyjlStringAdd( json, 0,0, "]");
         } break;
    case oyjl_t_object:
         {
           int i,
               count = v->u.object.len;

           oyjlStringAdd( json, 0,0, "{" );

           *level += 2;
           for(i = 0; i < count; ++i)
           {
             oyjlJsonIndent_( json, "\n", *level, NULL );
             if(!v->u.object.keys || !v->u.object.keys[i])
             {
               oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "missing key", OYJL_DBG_ARGS );
               if(json && *json)
               {
                 free(*json);
                 *json = NULL;
               }
               return;
             }
             oyjlStringAdd( json, 0,0, "\"%s\": ", v->u.object.keys[i] );
             oyjlTreeToJson2_( v->u.object.values[i], level, json );
             if(count > 1)
             {
               if(i < count - 1)
                 oyjlStringAdd( json, 0,0, "," );
             }
           }
           *level -= 2;

           oyjlJsonIndent_( json, "\n", *level, "}" );
         }
         break;
    default:
         oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "unknown type: %d", OYJL_DBG_ARGS, v->type );
         break;
  }
  return;
}

char * oyjlTreePrint (oyjl_val v)
{
  char * json = NULL;
  int level = 0;
  oyjlTreeToJson( v, &level, &json );
  return json;
}

/** @brief convert a C tree into a YAML string
 *
 *  @see oyjlTreeParseYaml()
 *
 *  @param         v                   node
 *  @param         level               desired level depth
 *  @param         text                the resulting string
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/01/01
 *  @since   2019/01/01 (Oyranos: 0.9.7)
 */
void               oyjlTreeToYaml    ( oyjl_val            v,
                                       int               * level,
                                       char             ** text)
{
#define YAML_INDENT " "
  if(*level == 0)
    oyjlStringAdd( text, 0,0, "---" );

  if(v)
  switch(v->type)
  {
    case oyjl_t_null:
         break;
    case oyjl_t_number:
         oyjlStringAdd (text, 0,0, "%s", v->u.number.r);
         break;
    case oyjl_t_true:
         oyjlStringAdd (text, 0,0, "1"); break;
    case oyjl_t_false:
         oyjlStringAdd (text, 0,0, "0"); break;
    case oyjl_t_string:
         {
          const char * t = v->u.string;
          char * tmp = oyjlStringCopy(t,malloc);
          oyjlStringReplace( &tmp, "\"", "\\\"", 0, 0);
          oyjlStringReplace( &tmp, ": ", ":\\ ", 0, 0);
          oyjlStringAdd (text, 0,0, YAML_INDENT "%s", tmp);
          if(tmp) free(tmp);
         }
         break;
    case oyjl_t_array:
         {
           int i,
               count = v->u.array.len;

           for(i = 0; i < count; ++i)
           {
             oyjlJsonIndent_( text, "\n", *level, "-" );
             *level += 2;
             oyjlTreeToYaml( v->u.array.values[i], level, text );
             *level -= 2;
           }

         } break;
    case oyjl_t_object:
         {
           int i,
               count = v->u.object.len;

           for(i = 0; i < count; ++i)
           {
             oyjlJsonIndent_( text, "\n", *level, NULL );
             if(!v->u.object.keys || !v->u.object.keys[i])
             {
               oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "missing key", OYJL_DBG_ARGS );
               if(text && *text)
               {
                 free(*text);
                 *text = NULL;
               }
               return;
             }
             oyjlStringAdd( text, 0,0, "%s:", v->u.object.keys[i] );
             *level += 2;
             oyjlTreeToYaml( v->u.object.values[i], level, text );
             *level -= 2;
           }
         }
         break;
    default:
         oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "unknown type: %d", OYJL_DBG_ARGS, v->type );
         break;
  }
  return;
#undef YAML_INDENT
}

static void oyjlTreeToXml2_(oyjl_val v, const char * parent_key, int * level, oyjl_str text)
{
  if(!v) return;

  switch(v->type)
  {
    case oyjl_t_null:
         break;
    case oyjl_t_number:
         oyjlStrAppendN (text, v->u.number.r, strlen(v->u.number.r));
         break;
    case oyjl_t_true:
         oyjlStrAppendN (text, "1", 1); break;
    case oyjl_t_false:
         oyjlStrAppendN (text, "0", 1); break;
    case oyjl_t_string:
         {
          const char * t = v->u.string;
          oyjlStrAppendN (text, t, strlen(t));
         }
         break;
    case oyjl_t_array:
         {
          int i,j,
              count = v->u.array.len;

          for(i = 0; i < count; ++i)
          {
            if( v->u.array.values[i] && v->u.array.values[i]->type == oyjl_t_object )
              oyjlTreeToXml2_( v->u.array.values[i], parent_key, level, text );
            else
            {
              /* print only values and the closing */
              for(i = 0; i < count; ++i)
              {
                oyjlStrAppendN( text, "\n", 1 );
                for(j = 0; j < *level; ++j) oyjlStrAppendN( text, " ", 1 );
                oyjlStrAdd( text, "<%s>", parent_key );

                oyjlTreeToXml2_( v->u.array.values[i], parent_key, level, text );

                oyjlStrAdd( text, "</%s>", parent_key );
              }
            }
          }
         }
         break;
    case oyjl_t_object:
         {
          int i,j,
              count = v->u.object.len;
          int is_inner_string;
#define XML_TEXT "@text"
#define XML_CDATA "@cdata"
          int is_attribute;
#define XML_ATTR '@'
          int is_array, is_object, last_is_content;
          const char * key = NULL;

          /* check */
          for(i = 0; i < count; ++i)
          {
            if(!v->u.object.keys || !v->u.object.keys[i])
            {
              oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "missing key", OYJL_DBG_ARGS );
              oyjlStrClear( text );
              return;
            }
          }

          /* print key name of parent object */
          if(parent_key)
          {
            oyjlStrAppendN( text, "\n", 1 );
            for(j = 0; j < *level; ++j) oyjlStrAppendN( text, " ", 1 );
            oyjlStrAdd( text, "<%s", parent_key );

            /* insert attributes to key */
            for(i = 0; i < count; ++i)
            {
              is_attribute = is_inner_string = is_array = is_object = 0;

              if( v->u.object.values[i]->type == oyjl_t_object )
                is_object = 1;
              else
              if( v->u.object.values[i]->type == oyjl_t_array )
                is_array = 1;
              else
              if( v->u.object.values[i]->type == oyjl_t_string &&
                  v->u.object.values[i]->u.string &&
                  ( strcmp(v->u.object.keys[i], XML_CDATA) == 0 ||
                    strcmp(v->u.object.keys[i], XML_TEXT) == 0 ) )
                is_inner_string = 1;
              else
              if( ( ( v->u.object.values[i]->type == oyjl_t_string &&
                      v->u.object.values[i]->u.string ) ||
                    v->u.object.values[i]->type == oyjl_t_number
                  ) &&
                  v->u.object.keys[i][0] == XML_ATTR )
                is_attribute = 1;

              if( !is_attribute )
                continue;

              if(v->u.object.values[i]->type == oyjl_t_string)
                oyjlStrAdd( text, " %s=\"%s\"", &v->u.object.keys[i][1], v->u.object.values[i]->u.string );
              else
                oyjlStrAdd( text, " %s=\"%s\"", &v->u.object.keys[i][1], v->u.object.values[i]->u.number.r );
            }

            /* close key name */
            oyjlStrAppendN( text, ">", 1 );
          }

          *level += 2;

          /* print only values and the closing */
          last_is_content = 1;
          for(i = 0; i < count; ++i)
          {
            is_attribute = is_inner_string = is_array = is_object = 0;
            key = v->u.object.keys[i];

            if( v->u.object.values[i]->type == oyjl_t_object )
              is_object = 1;
            else
            if( v->u.object.values[i]->type == oyjl_t_array )
              is_array = 1;
            else
            if( v->u.object.values[i]->type == oyjl_t_string &&
                v->u.object.values[i]->u.string &&
                ( strcmp(key, XML_CDATA) == 0 ||
                  strcmp(key, XML_TEXT) == 0 ) )
              is_inner_string = 1;
            else
            if( ( ( v->u.object.values[i]->type == oyjl_t_string &&
                    v->u.object.values[i]->u.string ) ||
                  v->u.object.values[i]->type == oyjl_t_number
                ) &&
                key[0] == XML_ATTR )
              is_attribute = 1;

            if( is_attribute ) continue;

            if( !is_array &&
                !is_object &&
                !is_inner_string )
            {
              oyjlStrAppendN( text, "\n", 1 );
              for(j = 0; j < *level; ++j) oyjlStrAppendN( text, " ", 1 );
            }

            if( !is_array &&
                !is_object &&
                !is_inner_string )
              oyjlStrAdd( text, "<%s>", key );

            if( strcmp(key, XML_CDATA) == 0 )
              oyjlStrAppendN( text, "<![CDATA[", 9 );

            oyjlTreeToXml2_( v->u.object.values[i], key, level, text );

            if( strcmp(key, XML_CDATA) == 0 )
              oyjlStrAppendN( text, "]]>", 3 );

            if( !is_array &&
                !is_object &&
                !is_inner_string )
            {
              oyjlStrAdd( text, "</%s>", key );
              last_is_content = 0;
            }
            else
            if( is_attribute )
              last_is_content = 0;
          }

          *level -= 2;

          if(parent_key)
          {
            if( !last_is_content )
            {
              oyjlStrAppendN( text, "\n", 1 );
              for(j = 0; j < *level; ++j) oyjlStrAppendN( text, " ", 1 );
            }
            oyjlStrAdd( text, "</%s>", parent_key );
          }
         }
         break;
    default:
          oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "unknown type: %d", OYJL_DBG_ARGS, v->type );
         break;
  }
}

/** @brief convert a C tree into a XML string
 *
 *  The function uses some assumptions for mapping features of JSON to XML.
 *
 *  The JSON tree shall consist of one root object. Object keys starting with
 *  '@' are mapped to a attribute of the parent key. A object key of
 *  "@text" is mapped to the inner XML content of the parent tree object.
 *
 *  @see oyjlTreeParseXml()
 *
 *  @param         v                   node
 *  @param         level               desired level depth
 *  @param         text                the resulting string
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/06/14
 *  @since   2019/01/01 (Oyranos: 0.9.7)
 */
void               oyjlTreeToXml     ( oyjl_val            v,
                                       int               * level,
                                       char             ** text)
{
  oyjlStringAdd( text, 0,0, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" );
  if(v && text && *text)
  switch(v->type)
  {
    case oyjl_t_null:
    case oyjl_t_number:
    case oyjl_t_true:
    case oyjl_t_false:
    case oyjl_t_string:
    case oyjl_t_array:
          oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "Need one key as root element! This object has no key. type: %d", OYJL_DBG_ARGS, v->type );
         break;
    case oyjl_t_object:
         {
          int count = v->u.object.len;

          if(count != 1)
          {
            oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "Need one single object as root element! count: %d", OYJL_DBG_ARGS, count );
            break;
          }
          if(!v->u.object.keys || !v->u.object.keys[0])
          {
            oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "missing key", OYJL_DBG_ARGS );
            free(*text);
            *text = NULL;
            return;
          }
          oyjl_str str = oyjlStrNewFrom(text, 0, 0,0);
          oyjlTreeToXml2_( v->u.object.values[0], v->u.object.keys[0], level, str );
          *text = oyjlStrPull( str );
          oyjlStrRelease( &str );
         }
         break;
    default:
          oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "unknown type: %d", OYJL_DBG_ARGS, v->type );
         break;
  }
  return;
}

/** @brief return the number of members if any at the node level
 *
 *  This function is useful to traverse through objects and arrays of a
 *  unknown JSON tree. */
int            oyjlValueCount        ( oyjl_val            v )
{
  int count = 0;

  if(!v)
    return count;

  if(v->type == oyjl_t_object)
    count = v->u.object.len;
  else if(v->type == oyjl_t_array)
    count = v->u.array.len;

  return count;
}

/** @brief obtain a child node at the nth position from a object or array node */
oyjl_val       oyjlValuePosGet       ( oyjl_val            v,
                                       int                 pos )
{
  if(!v)
    return NULL;

  if(v->type == oyjl_t_object)
    return v->u.object.values[pos];
  else if(v->type == oyjl_t_array)
    return v->u.array.values[pos];

  return NULL;
}

/** @internal 
 *  @brief tell about xpath segment
 *
 *  @param         term                xpath segment
 *  @param         index               resulting array position,
 *                                     - is a index: set index from term
 *                                     - is a wildcard: keeps index untouched
 *                                     - is not an index or wildcard: set position to -1
 *  @return                            status
 *                                     - 0  : index or wildcard
 *                                     - 1  : error
 *                                     - -1 : no suitable term, will set index to -1
 */
int        oyjlPathTermGetIndex_     ( const char        * term,
                                       int               * index )
{
  char * tindex;
  int pos = -1;
  int error = -1;

  if(!term) { *index = pos; return 1; }

  tindex = strrchr(term,'[');

  /* pick wildcards "", "[]" */
  if(term[0] == '\000' ||
     strcmp(term,"[]") == 0)
  {
    pos = *index;
    error = 0;
  }
  else
  if(tindex != NULL)
  {
    ptrdiff_t size;
    ++tindex;
    size = strrchr(term,']') - tindex;
    if(size > 0)
    {
      long signed int num = 0;
      char * ttmp = malloc(size + 1);
      if(!ttmp) return 1;
      memcpy( ttmp, tindex, size );
      ttmp[size] = '\000';

      error = oyjlStringToLong( ttmp, &num );
      if(!error)
        pos = num;

      if(ttmp) free( ttmp );
    }
  }

  *index = pos;

  return error;
}
/** @brief search for xpath pattern matching in a full path
 *
 *  The function tries to match a single path expression level by level.
 *
 *  @code
    // the second xpath expression matches the first path
    int matches = oyjlPathMatch( "org/free/[1]/s2key_d", "org///s2key_d", 0 );
    // "//[1]/s2key_d" or "///s2key_d" would fit as well;  "//[0]/s2key_d" not
    @endcode
 *
 *  @param         path                a path expression
 *  @param         xpath               a extented path expression
 *  @param         flags               optional switches
 *                                     - 0 : match all xpaths from start
 *                                     - OYJL_PATH_MATCH_LEN : match all xpaths of the exact same number of terms
 *                                     - OYJL_PATH_MATCH_LAST_ITEMS : search the last terms(s) from xpath
 *  @return                            0 - fail, 1 - match
 */
int        oyjlPathMatch             ( const char        * path,
                                       const char        * xpath,
                                       int                 flags )
{
  int match = 0, i,pn=0,xn=0,diff=0;
  char ** xlist = oyjlStringSplit(xpath, '/', &xn, malloc);
  char ** plist = oyjlStringSplit(path, '/', &pn, malloc);

  if(flags & OYJL_PATH_MATCH_LAST_ITEMS)
    diff = pn - xn;

  if(!xlist || !plist) return 0;
  if(flags & OYJL_PATH_MATCH_LEN)
  {
    if(pn == xn) match = 1;
  } else {
    if(pn >= xn) match = 1;
  }

  /* follow the search path term */
  for(i = diff; i < (diff + xn) && match; ++i)
  {
    char * xterm = xlist[i - diff],
         * pterm = plist[i];
    int xindex = -2,
        pindex = -2;

    oyjlPathTermGetIndex_( xterm, &xindex );
    oyjlPathTermGetIndex_( pterm, &pindex );

    if(!(strcmp(xterm, pterm) == 0 ||
        (pindex >= 0 && xindex == pindex) ||
        (xindex == -2)))
      match = 0;
  }

  oyjlStringListRelease( &xlist, xn, free );
  oyjlStringListRelease( &plist, pn, free );

  return match;
}

/* split new root allocation from inside root manipulation */
static oyjl_val  oyjlTreeGetValue_   ( oyjl_val            v,
                                       int                 flags,
                                       const char        * xpath )
{
  oyjl_val level = 0, parent = v, root = NULL, result = NULL;
  int n = 0, i, found = 0;
  char ** list = oyjlStringSplit(xpath, '/', &n, malloc);

  if(!oyjl_debug_node_path_)
  {
    const char * t = getenv("OYJL_DEBUG_NODE");
    oyjl_debug_node_path_ = oyjlStringCopy(t?t:"", 0);
    if(*oyjl_debug)
      fprintf(stderr, "allocated oyjl_debug_node_path_: %s", oyjl_debug_node_path_ );
    if(oyjl_debug_node_path_[0])
    {
      char * separate = strchr(oyjl_debug_node_path_, ':');
      if(separate != NULL)
      {
        oyjl_debug_node_value_ = oyjlStringCopy( separate + 1, 0 );
        separate[0] = '\000';
      }
      fprintf(stderr, "observing: %s:\"%s\" - OYJL_DEBUG_NODE=%s\n", oyjl_debug_node_path_, oyjlTermColor(oyjlBOLD, oyjl_debug_node_value_?oyjl_debug_node_value_:""), t);
    }
  }

    /* follow the search path term */
  for(i = 0; i < n; ++i)
  {
    char * term = list[i];
    /* is object or array */
    int count = oyjlValueCount( parent );
    int j;
    int pos = 0;

    found = 0;
    if(count == 0 && !(flags & OYJL_CREATE_NEW)) break;

    oyjlPathTermGetIndex_( term, &pos );

    /* requests index in object or array */
    if(pos != -1)
    {
      if(0 <= pos && pos < count)
        level = oyjlValuePosGet( parent, pos );
      else
        level = NULL;

      /* add new leave */
      if(!level &&
         flags & OYJL_CREATE_NEW)
      {

        if(parent)
        {
          int add = 0;
          int start = parent->u.array.len;
          if(parent->type != oyjl_t_array)
          {
            oyjlValueClear( parent );
            parent->type = oyjl_t_array;
            add = pos + 1;
            oyjlAllocHelper_m( parent->u.array.values, oyjl_val, add, malloc, goto clean );
            parent->u.array.len = add;
          } else
          {
            oyjl_val *tmp;

            add = pos + 1  - count;
            tmp = realloc(parent->u.array.values,
                    sizeof(*(parent->u.array.values)) * (parent->u.array.len + add) + 1);
            if (tmp == NULL)
            {
              oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "could not allocate memory", OYJL_DBG_ARGS );
              goto  clean;
            }
            parent->u.array.values = tmp;
            parent->u.array.len += add;
          }
          for(j = 0; j < add; ++j)
          {
            level = oyjlValueAlloc_( oyjl_t_null );
            parent->u.array.values[start+j] = level;
          }
        }
      }

      found = 1;
    } else
    {
      /* search for name in object */
      for(j = 0; j < count; ++j)
      {
        if((parent->type == oyjl_t_object && strcmp( parent->u.object.keys[j], term ) == 0) ||
            /* a empty term matches to everything */
           term[0] == '\000')
        {
          found = 1;
          level = oyjlValuePosGet( parent, j );
          break;
        }
      }

      /* add new leave */
      if(!level &&
         flags & OYJL_CREATE_NEW)
      {
        level = oyjlValueAlloc_( oyjl_t_null );

        if(parent)
        {
          if(parent->type != oyjl_t_object)
          {
            oyjlValueClear( parent );
            parent->type = oyjl_t_object;
            oyjlAllocHelper_m( parent->u.object.values, oyjl_val, 2, malloc, oyjlTreeFree( level ); goto clean );
            oyjlAllocHelper_m( parent->u.object.keys, char*, 2, malloc, oyjlTreeFree( level ); goto clean );
            parent->u.object.len = 0;
          } else
          {
            oyjl_val *tmp;
            char ** keys;

            tmp = realloc(parent->u.object.values,
                    sizeof(*(parent->u.object.values)) * (parent->u.object.len + 1));
            if (tmp == NULL)
            {
              oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "could not allocate memory", OYJL_DBG_ARGS );
              oyjlTreeFree( level );
              goto clean;
            }
            parent->u.object.values = tmp;

            keys = realloc(parent->u.object.keys,
                    sizeof(*(parent->u.object.keys)) * (parent->u.object.len + 1));
            if (keys == NULL)
            {
              oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "could not allocate memory", OYJL_DBG_ARGS );
              oyjlTreeFree( level );
              goto clean;
            }
            parent->u.object.keys = keys;
          }
          parent->u.object.keys[parent->u.object.len] = oyjlStringCopy( term, malloc );
          parent->u.object.values[parent->u.object.len] = level;
          parent->u.object.len++;
        }
      } 

      found = 1;
    }
    if(!v && !root)
    {
      root = level;
      --i;
    }
    parent = level;
    level = NULL;
  }

  /* clean up temorary memory */
clean:
  for(i = 0; i < n; ++i)
    free(list[i]);
  if(list)
    free(list);

  if(found && root)
    result = root;
  else
  if(found && parent)
    result = parent;
  else
  {
    if(root)
      oyjlTreeFree(root);
    else if(!v && parent)
      oyjlTreeFree(parent);
    return NULL;
  }

  if(oyjl_debug_node_path_[0])
  {
    if(oyjlPathMatch(xpath, oyjl_debug_node_path_, 0))
    {
      if(oyjl_debug_node_value_)
      {
        char * t = oyjlValueText(result, 0);
        if(t && strstr(t, oyjl_debug_node_value_) != NULL)
        {
          OYJL_SET_OBSERVE(result)
          if(!(flags&OYJL_QUIET))
            oyjlValueDebug_( result, xpath, "get", 0 );
        }
        if(t) free(t);
      } else
      {
        OYJL_SET_OBSERVE(result)
        if(!(flags&OYJL_QUIET))
          oyjlValueDebug_( result, xpath, "get", 0 );
      }
    }
  }

  return result;
}
/** @brief create a node by a path expression
 *
 *  A NULL argument allocates just a node of type oyjl_t_null.
 *
 *  @see oyjlTreeGetValueF() */
oyjl_val   oyjlTreeNew               ( const char        * path )
{
  if(path && path[0])
    return oyjlTreeGetValue_( NULL, OYJL_CREATE_NEW, path );
  else
    return oyjlValueAlloc_( oyjl_t_null );
}

/** @brief   get a full path string from a child node
 *
 *  This function is the reverse from oyjlTreeGetValue().
 *  Use this function to get parent names and resolve xpath
 *  expressions to full paths.
 *
 *  @param[in]     v                   the oyjl node
 *  @param[in]     value               the node to search for inside the v tree
 *  @return                            the path or zero
 *
 *  @version Oyranos: 0.9.7
 *  @date    2020/03/27
 *  @since   2020/03/27 (Oyranos: 0.9.7)
 */
char *     oyjlTreeGetPath           ( oyjl_val            v,
                                       oyjl_val            node )
{
  char * path = NULL;
  char ** paths = NULL;
  int count, i;
  oyjl_val test = NULL;

  oyjlTreeToPaths( v, 10000000, NULL, OYJL_KEY, &paths );
  count = 0; while(paths && paths[count]) ++count;

  for(i = 0; i < count; ++i)
  {
    path = paths[i];
    test = oyjlTreeGetValue( v, 0, path );
    if(test == node)
    {
      paths[i] = NULL;
      break;
    }
  }

  if(paths && count)
    oyjlStringListRelease( &paths, count, free );

  if(i == count)
    path = NULL;

  return path;
}

/** @brief obtain a node by a path expression
 *
 *  @see oyjlTreeGetValueF() */
oyjl_val   oyjlTreeGetValue          ( oyjl_val            v,
                                       int                 flags,
                                       const char        * xpath )
{
  if(!v || !xpath)
    return NULL;
  else
    return oyjlTreeGetValue_(v,flags,xpath);
}


/** Function oyjlTreeGetValueF
 *  @brief   get a child node by a path expression
 *
 *  Creating a new node inside a existing tree needs just a root node - v.
 *  The flags should contain ::OYJL_CREATE_NEW.
 *  @code
    oyjl_val new_node = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, "my/new/node" );
    @endcode
 *
 *  Example: "foo/[]/bar" will append a node to the foo array and create
 *  the bar node, which is empty.
 *  @code
    oyjl_val new_node = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, "foo/[]/bar" );
    @endcode
 *
 *
 *  @param[in]     v                   the oyjl node
 *  @param[in]     flags               ::OYJL_CREATE_NEW - returns nodes even
 *                                     if they did not yet exist
 *  @param[in]     format              the format for the slashed xpath string
 *  @param[in]     ...                 the variable argument list; optional
 *  @return                            the requested node or a new tree or zero
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/10/12
 *  @since   2011/09/24 (Oyranos: 0.3.3)
 */
oyjl_val   oyjlTreeGetValueF         ( oyjl_val            v,
                                       int                 flags,
                                       const char        * format,
                                                           ... )
{
  oyjl_val value = NULL;
  char * text = NULL;

  OYJL_CREATE_VA_STRING(format, text, malloc, return value)

  value = oyjlTreeGetValue( v, flags, text );

  if(text) free(text);

  return value;
}

/** Function oyjlTreeSetStringF
 *  @brief   set a child node to a string value
 *
 *  @param[in,out] root                the oyjl node
 *  @param[in]     flags               ::OYJL_CREATE_NEW - allocates nodes even
 *                                     if they did not yet exist
 *  @param[in]     value_text          a string
 *  @param[in]     format              the format for the slashed xpath string
 *  @param[in]     ...                 the variable argument list; optional
 *  @return                            error
 *                                     - -1 - if not found
 *                                     - 0 on success
 *                                     - else error
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/01/10
 *  @since   2019/01/10 (Oyranos: 0.9.7)
 */
int        oyjlTreeSetStringF        ( oyjl_val            root,
                                       int                 flags,
                                       const char        * value_text,
                                       const char        * format,
                                                           ... )
{
  oyjl_val value_node = NULL;

  char * text = NULL;
  int error = 0, is_observed = 0;

  OYJL_CREATE_VA_STRING(format, text, malloc, return 1)

  if(OYJL_DEBUG_NODE_IS_VALUE( text, value_text ))
  {
    oyjlValueDebug_( root, text, "setString", 1 );
    ++is_observed;
  }

  value_node = oyjlTreeGetValue( root, is_observed ? flags|OYJL_QUIET : flags, text );

  if(text) free(text);

  if(value_node)
  {
    int is_observed_vnode = OYJL_IS_OBSERVED(value_node);
    if(is_observed_vnode && is_observed)
      OYJL_UNSET_OBSERVE(value_node);

    error = oyjlValueSetString( value_node, value_text );

    if(is_observed_vnode && is_observed)
      OYJL_SET_OBSERVE(value_node);
  }
  else
    error = -1;

  return error;
}

/** Function oyjlTreeSetDoubleF
 *  @brief   set a child node to a string value
 *
 *  @param[in,out] root                the oyjl node
 *  @param[in]     flags               ::OYJL_CREATE_NEW - allocates nodes even
 *                                     if they did not yet exist
 *  @param[in]     value               IEEE floating point number with double precission
 *  @param[in]     format              the format for the slashed xpath string
 *  @param[in]     ...                 the variable argument list; optional
 *  @return                            error
 *                                     - -1 - if not found
 *                                     - 0 on success
 *                                     - else error
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/01/26
 *  @since   2019/01/26 (Oyranos: 0.9.7)
 */
int        oyjlTreeSetDoubleF        ( oyjl_val            root,
                                       int                 flags,
                                       double              value,
                                       const char        * format,
                                                           ... )
{
  oyjl_val value_node = NULL;

  char * text = NULL;
  int error = 0;

  OYJL_CREATE_VA_STRING(format, text, malloc, return 1)

  value_node = oyjlTreeGetValue( root, flags, text );

  if(text) { free(text); text = NULL; }

  if(value_node)
    error = oyjlValueSetDouble( value_node, value );
  else
    error = -1;

  return error;
}

/** Function oyjlValueSetDouble
 *  @brief   set a child node to a string value
 *
 *  @param[in,out] v                   the oyjl node
 *  @param[in]     value               IEEE floating point number with double precission
 *  @return                            error
 *                                     - -1 - if not found
 *                                     - 0 on success
 *                                     - else error
 *
 *  @version Oyjl: 1.0.0
 *  @date    2020/10/12
 *  @since   2020/10/12 (Oyjl: 1.0.0)
 */
int        oyjlValueSetDouble        ( oyjl_val            v,
                                       double              value )
{
  int error = 0;

  if(v)
  {
    oyjlValueClear( v );
    v->type = oyjl_t_number;
    v->u.number.d = value;
#ifdef OYJL_HAVE_LOCALE_H
    char * save_locale = oyjlStringCopy( setlocale(LC_NUMERIC, 0 ), malloc );
    setlocale(LC_NUMERIC, "C");
#endif
    error = oyjlStringAdd( &v->u.number.r, 0,0, "%g", value );
#ifdef OYJL_HAVE_LOCALE_H
    setlocale(LC_NUMERIC, save_locale);
    if(save_locale)
      free( save_locale );
#endif
    v->u.number.flags |= OYJL_NUMBER_DOUBLE_VALID;
    errno = 0;
    v->u.number.i = strtol(v->u.number.r, 0, 10);
    if(errno == 0)
      v->u.number.flags |= OYJL_NUMBER_INT_VALID;
  }
  else
    error = -1;

  return error;
}

/** @brief set the node value to a string */
int        oyjlValueSetString        ( oyjl_val            v,
                                       const char        * string )
{
  int error = -1;
  if(v && string)
  {
    oyjlValueClear( v );
    v->type = oyjl_t_string;
    v->u.string = NULL;
    error = oyjlStringAdd( &v->u.string, 0,0, "%s", string );
  }
  return error;
}

/* flags - 0x01 : print always */
void       oyjlValueDebug_           ( oyjl_val            val,
                                       const char        * xpath,
                                       const char        * desc,
                                       int                 flags )
{
  if(OYJL_IS_OBSERVED(val) || flags & 0x01)
  {
    char * t = oyjlBT(0);
    int level = 0;
    fprintf( stderr, "%s", t );
    free(t);
    t = oyjlValueText(val, 0);
    if(!t)
      oyjlTreeToJson( val, &level, &t );
    /* improve object print for one level xpath (without slash '/'): OYJL_DEBUG_NODE="value:key" */
    if(val->type == oyjl_t_object)
    {
      size_t l = val->u.object.len, i;
      for( i = 0; i < l; ++i)
      {
        const char * key = val->u.object.keys[i];
        if(strcmp(key, xpath) == 0)
        {
          val = val->u.object.values[i];
          break;
        }
      }
    }
    oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT OYJL_PRINT_POINTER " %s:%s - %s", OYJL_DBG_ARGS, val, xpath, t, desc );
    if(t) free(t);
  }
}

#define Florian_Forster_SOURCE_GUARD
/** @brief release all childs recursively */
void oyjlValueClear          (oyjl_val v)
{
    if (v == NULL) return;
    oyjlValueDebug_(v, "", "clear", 0);

    if (OYJL_IS_STRING(v)) {
        if(v->u.string) free(v->u.string);
        v->u.string = NULL;
    } else if (OYJL_IS_NUMBER(v)) {
        if(v->u.number.r) free(v->u.number.r);
        v->u.number.r = NULL;
    } else if (OYJL_GET_OBJECT(v))
        oyjlObjectFree_(v);
    else if (OYJL_GET_ARRAY(v))
        oyjlArrayFree_(v);

    v->type = oyjl_t_null;
}
#undef Florian_Forster_SOURCE_GUARD

/** @brief release a specific node and all its childs
 *
 *  In case parents have no children, release them or clear root.
 */
void oyjlTreeClearValue              ( oyjl_val            root,
                                       const char        * xpath )
{
  int n = 0, i, pos, count;
  char ** list;
  char * path;
  int delete_parent = 0;

  if(!root) return;

  list = oyjlStringSplit(xpath, '/', &n, malloc);
  path = oyjlStringCopy( xpath, malloc );

  for(pos = 0; pos < n; ++pos)
  {
    oyjl_val p; /* parent */
    oyjl_val o = oyjlTreeGetValue( root, 0, path );

    char * parent_path = oyjlStringCopy( path, malloc ),
         * t = strrchr(parent_path, '/');
    if(t)
    {
      t[0] = '\000';
      p = oyjlTreeGetValue( root, 0, parent_path );
    }
    else
      p = root;

    delete_parent = 0;
    if(p)
    {
      switch(p->type)
      {
      case oyjl_t_array:
         {
           count = p->u.array.len;

           for(i = 0; i < count; ++i)
           {
             if( p->u.array.values[i] == o )
             {
               oyjlTreeFree( o );
               p->u.array.values[i] = o = NULL;

               if(count > 1)
                 memmove( &p->u.array.values[i], &p->u.array.values[i+1],
                          sizeof(oyjl_val *) * (count - i - 1) );
               else
                 delete_parent = 1;

               --p->u.array.len;
               break;
             }
           }
         }
         break;
      case oyjl_t_object:
         {
           count = p->u.object.len;

           if(count == 0)
             delete_parent = 1;

           for(i = 0; i < count; ++i)
           {
             if( p->u.object.values[i] == o )
             {
               if(p->u.object.keys[i])
                 free(p->u.object.keys[i]);
               p->u.object.keys[i] = NULL;

	       oyjlTreeFree( o );
               p->u.object.values[i] = o = NULL;

               if(count > 1)
               {
                 memmove( &p->u.object.keys[i], &p->u.object.keys[i+1],
                          sizeof(char *) * (count - i - 1) );
                 memmove( &p->u.object.values[i], &p->u.object.values[i+1],
                          sizeof(oyjl_val *) * (count - i - 1) );
               }
               else
                 delete_parent = 1;

               --p->u.object.len;
               break;
             }
           }
         }
         break;
      default: break; /* ok */
      }
    }

    if(path) free(path);
    path = parent_path;
    parent_path = NULL;
    oyjlTreeFree( o );
    o = NULL;

    if(delete_parent == 0)
      break;
  }

  /* The root node has no name here. So we need to detect that case.
   * Keep the node itself, as it is still referenced by the caller. */
  if(path && delete_parent && strchr(path,'/') == NULL)
    oyjlValueClear(root);

  for(i = 0; i < n; ++i) free(list[i]);
  if(list) free(list);
  if(path) free(path);
}

/** @brief release a node and all its childs recursively */
void oyjlTreeFree (oyjl_val v)
{
    if (v == NULL) return;

    oyjlValueClear (v);
    free(v);
}

/** @} *//* oyjl_tree */


/** \addtogroup oyjl
 *  @{ *//* oyjl */

/** @brief   translate string from catalog
 *
 *  The passed in catalog shall contain its translations in the
 *  "org/freedesktop/oyjl/translations/loc" path.
 *
 *  @param         loc                 locale name as from setlocale(""), the special locale "back" will inverse the translation; optional, will return without
 *  @param         catalog             the parsed catalog as tree; optional, will return without
 *  @param         text                the to be translated text; optional, will return without
 *  @return                            translated item; must not be freed
 *
 *  @version Oyjl: 1.0.0
 *  @date    2020/07/27
 *  @since   2020/07/27 (Oyjl: 1.0.0)
 */
char *         oyjlTranslate         ( const char        * loc,
                                       oyjl_val            catalog,
                                       const char        * text )
{
  const char * translated = NULL;
  oyjl_val v;

  if(!loc || strcmp(loc,"C") == 0 || !catalog || !text || (text && !text[0]))
    return (char*)text;

  if(strcmp(loc,"back") == 0 && text[0])
  {
    char * path = NULL;
    char ** paths = NULL;
    int count, i;
    const char * current;

    v = oyjlTreeGetValueF( catalog, 0, "org/freedesktop/oyjl/translations/back/%s", text );
    if(v)
    {
      translated = OYJL_GET_STRING(v);
      return translated ? (char*)translated : (char*)text;
    }

    oyjlTreeToPaths( catalog, 10000000, NULL, OYJL_KEY, &paths );
    count = 0; while(paths && paths[count]) ++count;

    for(i = 0; i < count; ++i)
    {
      path = paths[i];
      if(strstr(path, "org/freedesktop/oyjl/translations/back") != NULL)
        continue;
      v = oyjlTreeGetValueF( catalog, 0, "%s", path );
      current = OYJL_GET_STRING(v);
      if(strcmp(current, text) == 0)
      {
        oyjlTreeSetStringF( catalog, OYJL_CREATE_NEW, strrchr(path,'/')+1, "org/freedesktop/oyjl/translations/back/%s", text );
        v = oyjlTreeGetValueF( catalog, OYJL_CREATE_NEW, "org/freedesktop/oyjl/translations/back/%s", text );
        translated = OYJL_GET_STRING(v);
        break;
      }
    }

    if(paths && count)
      oyjlStringListRelease( &paths, count, free );

    return translated ? (char*)translated : (char*)text;
  }

  v = oyjlTreeGetValueF( catalog, 0, "org/freedesktop/oyjl/translations/%s/%s", loc, text );
  if(v)
    translated = OYJL_GET_STRING(v);
  if(!translated)
  {
    char * language = oyjlLanguage(loc),
         * country = oyjlCountry(loc);
    if(language && country)
      v = oyjlTreeGetValueF( catalog, 0, "org/freedesktop/oyjl/translations/%s_%s/%s", language, country, text );
    if(v)
      translated = OYJL_GET_STRING(v);
    if(language) free(language);
    if(country) free(country);
  }
  if(!translated)
  {
    char * language = oyjlLanguage(loc);
    v = oyjlTreeGetValueF( catalog, 0, "org/freedesktop/oyjl/translations/%s/%s", language, text );
    if(v)
      translated = OYJL_GET_STRING(v);
    if(language) free(language);
  }
  if(!translated && text[0])
  {
    char * language = oyjlLanguage(loc);
    char * path = NULL;
    char ** paths = NULL;
    int count, i;
    char * regex = NULL,
         * json = oyjlJsonEscape( text ),
         * escape = oyjlRegExpEscape( json );
    free(json); json = NULL;
    oyjlStringAdd( &regex, 0,0, "org/freedesktop/oyjl/translations/%s.*/%s", language, escape );
    free(escape); escape = NULL;

    oyjlTreeToPaths( catalog, 10000000, NULL, OYJL_KEY, &paths );
    count = 0; while(paths && paths[count]) ++count;

    for(i = 0; i < count; ++i)
    {
      path = paths[i];
      if(oyjlRegExpFind(path, regex))
      {
        const char * p = strrchr(path, '/');
        if(p) ++p;
        if(strcmp(p, text) == 0)
        {
          paths[i] = NULL;
          break;
        }
      }
    }

    if(paths && count)
      oyjlStringListRelease( &paths, count, free );

    if(i == count)
      path = NULL;

    v = NULL;
    if(path)
    {
      v = oyjlTreeGetValueF( catalog, 0, "%s", path );
      free(path);
    }
    if(v)
      translated = OYJL_GET_STRING(v);
    if(language) free(language);
    if(regex) free(regex);
  }
  return translated ? (char*)translated : (char*)text;
}

oyjl_val oyjl_catalog_ = NULL;
/** @brief   set message translation catalog
 *
 *  @param         catalog             message catalog for oyjlTranslate()
 *                                     - NULL: reset
 *                                     - pointer to NULL: return catalog
 *                                     - oyjl_val catalog: move in as new current
 *  @return                            current catalog
 *
 *  @version Oyjl: 1.0.0
 *  @date    2020/07/29
 *  @since   2020/07/29 (Oyjl: 1.0.0)
 */
oyjl_val       oyjlCatalog           ( oyjl_val          * catalog )
{
  if((!catalog || (catalog && *catalog)) && oyjl_catalog_ && (!catalog || oyjl_catalog_ != *catalog))
  {
    oyjlTreeFree(oyjl_catalog_);
    oyjl_catalog_ = NULL;
  }
  if(catalog && *catalog)
  {
    oyjl_catalog_ = *catalog;
    *catalog = NULL;
  }
  return oyjl_catalog_;
}
/** @} *//* oyjl */

