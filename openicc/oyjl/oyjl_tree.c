/*  @file oyjl_tree.c
 *
 *  oyjl - Yajl tree extension
 *
 *  @par Copyright:
 *            2016-2019 (C) Kai-Uwe Behrmann
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
#include "oyjl_version.h"
#include "oyjl_tree_internal.h"
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

/** \addtogroup oyjl_tree
 *  @{ *//* oyjl_tree */
static oyjl_val oyjlValueAlloc (oyjl_type type)
{
    oyjl_val v;

    v = malloc (sizeof (*v));
    if (v == NULL) return (NULL);
    memset (v, 0, sizeof (*v));
    v->type = type;

    return (v);
}

static void oyjlObjectFree (oyjl_val v)
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

static void oyjlArrayFree (oyjl_val v)
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

int        oyjlPathTermGetIndex      ( const char        * term,
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
         oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT_ "unknown type: %d", OYJL_DBG_ARGS_, v->type );
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
    oyjlPathTermGetIndex( term, &pos );

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
         oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT_ "unknown type: %d", OYJL_DBG_ARGS_, root->type );
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

static void oyjlJsonIndent ( char ** json, const char * before, int level, const char * after )
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

/** @brief convert a C tree into a JSON string
 *
 *  @see oyjlTreeParse()
 */
void oyjlTreeToJson (oyjl_val v, int * level, char ** json)
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
          char * tmp = NULL;
          if(t && strstr(t, "\\")) t = tmp = oyjlStringReplace( t, "\\", "\\\\", 0, 0);
          if(t && strstr(t, "\"")) t = tmp = oyjlStringReplace( t, "\"", "\\\"", 0, 0);
          if(t && strstr(t, "\b")) t = tmp = oyjlStringReplace( t, "\b", "\\b", 0, 0);
          if(t && strstr(t, "\f")) t = tmp = oyjlStringReplace( t, "\f", "\\f", 0, 0);
          if(t && strstr(t, "\n")) t = tmp = oyjlStringReplace( t, "\n", "\\n", 0, 0);
          if(t && strstr(t, "\r")) t = tmp = oyjlStringReplace( t, "\r", "\\r", 0, 0);
          if(t && strstr(t, "\t")) t = tmp = oyjlStringReplace( t, "\t", "\\t", 0, 0);
          oyjlStringAdd (json, 0,0, "\"%s\"", t);
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
             oyjlTreeToJson( v->u.array.values[i], level, json );
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
             oyjlJsonIndent( json, "\n", *level, NULL );
             if(!v->u.object.keys || !v->u.object.keys[i])
             {
               oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT_ "missing key", OYJL_DBG_ARGS_ );
               if(json && *json)
               {
                 free(*json);
                 *json = NULL;
               }
               return;
             }
             oyjlStringAdd( json, 0,0, "\"%s\": ", v->u.object.keys[i] );
             oyjlTreeToJson( v->u.object.values[i], level, json );
             if(count > 1)
             {
               if(i < count - 1)
                 oyjlStringAdd( json, 0,0, "," );
             }
           }
           *level -= 2;

           oyjlJsonIndent( json, "\n", *level, "}" );
         }
         break;
    default:
         oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT_ "unknown type: %d", OYJL_DBG_ARGS_, v->type );
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
          char * tmp = NULL;
          if(t && strstr(t, "\""))
            t = tmp = oyjlStringReplace( t, "\"", "\\\"", 0, 0);
          if(t && strstr(t, ": "))
            t = tmp = oyjlStringReplace( t, ": ", ":\\ ", 0, 0);
          oyjlStringAdd (text, 0,0, YAML_INDENT "%s", t);
          if(tmp) free(tmp);
         }
         break;
    case oyjl_t_array:
         {
           int i,
               count = v->u.array.len;

           for(i = 0; i < count; ++i)
           {
             oyjlJsonIndent( text, "\n", *level, "-" );
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
             oyjlJsonIndent( text, "\n", *level, NULL );
             if(!v->u.object.keys || !v->u.object.keys[i])
             {
               oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT_ "missing key", OYJL_DBG_ARGS_ );
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
         oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT_ "unknown type: %d", OYJL_DBG_ARGS_, v->type );
         break;
  }
  return;
#undef YAML_INDENT
}

void oyjlTreeToXml2 (oyjl_val v, const char * parent_key, int * level, char ** text)
{
  if(!v) return;

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
          char * tmp = NULL;
          if(t && strstr(t, "\\")) t = tmp = oyjlStringReplace( t, "\\", "\\\\", 0, 0);
          if(t && strstr(t, "\"")) t = tmp = oyjlStringReplace( t, "\"", "\\\"", 0, 0);
          if(t && strstr(t, "\b")) t = tmp = oyjlStringReplace( t, "\b", "\\b", 0, 0);
          if(t && strstr(t, "\f")) t = tmp = oyjlStringReplace( t, "\f", "\\f", 0, 0);
          if(t && strstr(t, "\n")) t = tmp = oyjlStringReplace( t, "\n", "\\n", 0, 0);
          if(t && strstr(t, "\r")) t = tmp = oyjlStringReplace( t, "\r", "\\r", 0, 0);
          if(t && strstr(t, "\t")) t = tmp = oyjlStringReplace( t, "\t", "\\t", 0, 0);
          oyjlStringAdd (text, 0,0, "%s", t);
          if(tmp) free(tmp);
         }
         break;
    case oyjl_t_array:
         {
          int i,
              count = v->u.array.len;

          for(i = 0; i < count; ++i)
          {
            if( v->u.array.values[i] && v->u.array.values[i]->type == oyjl_t_object )
              oyjlTreeToXml2( v->u.array.values[i], parent_key, level, text );
            else
            {
              /* print only values and the closing */
              for(i = 0; i < count; ++i)
              {
                oyjlJsonIndent( text, "\n", *level, NULL );
                oyjlStringAdd( text, 0,0, "<%s>", parent_key );

                oyjlTreeToXml2( v->u.array.values[i], parent_key, level, text );

                oyjlStringAdd( text, 0,0, "</%s>", parent_key );
              }
            }
          }
         }
         break;
    case oyjl_t_object:
         {
          int i,
              count = v->u.object.len;
          int is_inner_string;
#define XML_TEXT "@text"
          int is_attribute;
#define XML_ATTR '@'
          int is_array, is_object, last_is_content;
          const char * key = NULL;

          /* check */
          for(i = 0; i < count; ++i)
          {
            if(!v->u.object.keys || !v->u.object.keys[i])
            {
              oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT_ "missing key", OYJL_DBG_ARGS_ );
              if(text && *text)
              {
                free(*text);
                *text = NULL;
              }
              return;
            }
          }

          /* print key name of parent object */
          if(parent_key)
          {
            oyjlJsonIndent( text, "\n", *level, NULL );
            oyjlStringAdd( text, 0,0, "<%s", parent_key );

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
                  strcmp(v->u.object.keys[i], XML_TEXT) == 0 )
                is_inner_string = 1;
              else
              if( v->u.object.values[i]->type == oyjl_t_string &&
                  v->u.object.values[i]->u.string &&
                  v->u.object.keys[i][0] == XML_ATTR )
                is_attribute = 1;

              if( !is_attribute )
                continue;

              oyjlStringAdd( text, 0,0, " %s=\"%s\"", &v->u.object.keys[i][1], v->u.object.values[i]->u.string );
            }

            /* close key name */
            oyjlStringAdd( text, 0,0, ">" );
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
                strcmp(key, XML_TEXT) == 0 )
              is_inner_string = 1;
            else
            if( v->u.object.values[i]->type == oyjl_t_string &&
                v->u.object.values[i]->u.string &&
                key[0] == XML_ATTR )
              is_attribute = 1;

            if( is_attribute ) continue;

            if( !is_array &&
                !is_object &&
                !is_inner_string )
              oyjlJsonIndent( text, "\n", *level, NULL );

            if( !is_array &&
                !is_object &&
                !is_inner_string )
              oyjlStringAdd( text, 0,0, "<%s>", key );

            oyjlTreeToXml2( v->u.object.values[i], key, level, text );

            if( !is_array &&
                !is_object &&
                !is_inner_string )
            {
              oyjlStringAdd( text, 0,0, "</%s>", key );
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
              oyjlJsonIndent( text, "\n", *level, NULL );
            oyjlStringAdd( text, 0,0, "</%s>", parent_key );
          }
         }
         break;
    default:
          oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT_ "unknown type: %d", OYJL_DBG_ARGS_, v->type );
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
 *  @date    2019/01/01
 *  @since   2019/01/01 (Oyranos: 0.9.7)
 */
void               oyjlTreeToXml     ( oyjl_val            v,
                                       int               * level,
                                       char             ** text)
{
  oyjlStringAdd( text, 0,0, "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" );
  if(v)
  switch(v->type)
  {
    case oyjl_t_null:
    case oyjl_t_number:
    case oyjl_t_true:
    case oyjl_t_false:
    case oyjl_t_string:
    case oyjl_t_array:
          oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT_ "Need one key as root element! This object has no key. type: %d", OYJL_DBG_ARGS_, v->type );
         break;
    case oyjl_t_object:
         {
          int count = v->u.object.len;

          if(count != 1)
          {
            oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT_ "Need one single object as root element! count: %d", OYJL_DBG_ARGS_, count );
            break;
          }
          if(!v->u.object.keys || !v->u.object.keys[0])
          {
            oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT_ "missing key", OYJL_DBG_ARGS_ );
            if(text && *text)
            {
              free(*text);
              *text = NULL;
            }
            return;
          }
          oyjlTreeToXml2( v->u.object.values[0], v->u.object.keys[0], level, text );
         }
         break;
    default:
          oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT_ "unknown type: %d", OYJL_DBG_ARGS_, v->type );
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
int        oyjlPathTermGetIndex      ( const char        * term,
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

    oyjlPathTermGetIndex( xterm, &xindex );
    oyjlPathTermGetIndex( pterm, &pindex );

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
  oyjl_val level = 0, parent = v, root = NULL;
  int n = 0, i, found = 0;
  char ** list = oyjlStringSplit(xpath, '/', &n, malloc);

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

    oyjlPathTermGetIndex( term, &pos );

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
        level = oyjlValueAlloc( oyjl_t_null );

        if(parent)
        {
          if(parent->type != oyjl_t_array)
          {
            oyjlValueClear( parent );
            parent->type = oyjl_t_array;
            oyjlAllocHelper_m_( parent->u.array.values, oyjl_val, 2, malloc, oyjlTreeFree( level ); goto clean );
            parent->u.array.len = 0;
          } else
          {
            oyjl_val *tmp;

            tmp = realloc(parent->u.array.values,
                    sizeof(*(parent->u.array.values)) * (parent->u.array.len + 1));
            if (tmp == NULL)
            {
              oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT_ "could not allocate memory", OYJL_DBG_ARGS_ );
              oyjlTreeFree( level );
              goto  clean;
            }
            parent->u.array.values = tmp;
          }
          parent->u.array.values[parent->u.array.len] = level;
          parent->u.array.len++;
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
        level = oyjlValueAlloc( oyjl_t_null );

        if(parent)
        {
          if(parent->type != oyjl_t_object)
          {
            oyjlValueClear( parent );
            parent->type = oyjl_t_object;
            oyjlAllocHelper_m_( parent->u.object.values, oyjl_val, 2, malloc, oyjlTreeFree( level ); goto clean );
            oyjlAllocHelper_m_( parent->u.object.keys, char*, 2, malloc, oyjlTreeFree( level ); goto clean );
            parent->u.object.len = 0;
          } else
          {
            oyjl_val *tmp;
            char ** keys;

            tmp = realloc(parent->u.object.values,
                    sizeof(*(parent->u.object.values)) * (parent->u.object.len + 1));
            if (tmp == NULL)
            {
              oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT_ "could not allocate memory", OYJL_DBG_ARGS_ );
              oyjlTreeFree( level );
              goto clean;
            }
            parent->u.object.values = tmp;

            keys = realloc(parent->u.object.keys,
                    sizeof(*(parent->u.object.keys)) * (parent->u.object.len + 1));
            if (keys == NULL)
            {
              oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT_ "could not allocate memory", OYJL_DBG_ARGS_ );
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
    return root;
  if(found && parent)
    return parent;
  else
  {
    if(root)
      oyjlTreeFree(root);
    else if(!v && parent)
      oyjlTreeFree(parent);
    return NULL;
  }
}
/** @brief create a node by a path expression
 *
 *  A NULL argument allocates just a node of type oyjl_t_null.
 *
 *  @see oyjlTreeGetValuef() */
oyjl_val   oyjlTreeNew               ( const char        * path )
{
  if(path && path[0])
    return oyjlTreeGetValue_( NULL, OYJL_CREATE_NEW, path );
  else
    return oyjlValueAlloc( oyjl_t_null );
}

/** @brief obtain a node by a path expression
 *
 *  @see oyjlTreeGetValuef() */
oyjl_val   oyjlTreeGetValue          ( oyjl_val            v,
                                       int                 flags,
                                       const char        * xpath )
{
  if(!v || !xpath)
    return NULL;
  else
    return oyjlTreeGetValue_(v,flags,xpath);
}


/** Function oyjlTreeGetValuef
 *  @brief   get a child node by a path expression
 *
 *  Creating a new node inside a existing tree needs just a root node - v.
 *  The flags should contain ::OYJL_CREATE_NEW.
 *  @code
    oyjl_val new_node = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "my/new/node" );
    @endcode
 *
 *  Example: "foo/[]/bar" will append a node to the foo array and create
 *  the bar node, which is empty.
 *  @code
    oyjl_val new_node = oyjlTreeGetValuef( root, OYJL_CREATE_NEW, "foo/[]/bar" );
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
oyjl_val   oyjlTreeGetValuef         ( oyjl_val            v,
                                       int                 flags,
                                       const char        * format,
                                                           ... )
{
  oyjl_val value = 0;

  char * text = 0;
  va_list list;
  int len;
  size_t sz = strlen(format) * 2;

  text = malloc( sz );
  if(!text)
  {
    oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT_ "could not allocate memory", OYJL_DBG_ARGS_ );
    return 0;
  }

  text[0] = 0;

  va_start( list, format);
  len = vsnprintf( text, sz, format, list );
  va_end  ( list );

  if ((size_t)len >= sz)
  {
    text = realloc( text, (len+1)*sizeof(char) );
    if(!text) return NULL;
    va_start( list, format);
    len = vsnprintf( text, len+1, format, list );
    va_end  ( list );
  }

  value = oyjlTreeGetValue( v, flags, text );

  if(text) free(text);

  return value;
}

/** @brief set the node value to a string */
int        oyjlValueSetString        ( oyjl_val            v,
                                       const char        * string )
{
  int error = -1;
  if(v)
  {
    oyjlValueClear( v );
    v->type = oyjl_t_string;
    v->u.string = NULL;
    error = oyjlStringAdd( &v->u.string, 0,0, "%s", string );
  }
  return error;
}

/** @brief release all childs recursively */
void oyjlValueClear          (oyjl_val v)
{
    if (v == NULL) return;

    if (OYJL_IS_STRING(v)) {
        if(v->u.string) free(v->u.string);
        v->u.string = NULL;
    } else if (OYJL_IS_NUMBER(v)) {
        if(v->u.number.r) free(v->u.number.r);
        v->u.number.r = NULL;
    } else if (OYJL_GET_OBJECT(v))
        oyjlObjectFree(v);
    else if (OYJL_GET_ARRAY(v))
        oyjlArrayFree(v);

    v->type = oyjl_t_null;
}

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
