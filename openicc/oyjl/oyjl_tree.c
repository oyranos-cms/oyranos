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
#include <stdint.h>  /* portable: uint64_t   MSVC: __int64 */
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
         oyjlStringAdd (&t, 0,0, "true"); break;
    case oyjl_t_false:
         oyjlStringAdd (&t, 0,0, "false"); break;
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

typedef struct oyjlXPath_s
{
  uint32_t v_offset;                   /* start of v from start of oyjlXPath_s */
  char * xpath;                        /* OYJL_KEY escaped string. see oyjlTreeToPaths() */
  /* Only a subset of oyjl_t is allowed.
   * - The u.string member is a null terminated array of UTF-8 char.
   * - The u.number.r member is alway NULL. The plain unparsed string is appended.
   * - oyjl_t_false, oyjl_t_true and oyjl_t_null are compatible */
  oyjl_val v;
} oyjlXPath_s;

/* Serialisation of a oyjl_val tree as linear list.
 *
 * oyjlXPath_s follow the oyjlNodes_s::offsets
 * at the oyjlNodes_s::offsets[count].v_offset position. */
typedef struct oyjlNodes_s
{
  char type [8];                       /* place 'oiJS' here for oyjl static Json */
  uint32_t count;                      /* number of entries */
  int32_t flags;
  /* offset from begin of the oyjlNodes_s::offsets array to a oyjlXPath_s element.
   * elements can be of type oyjl_t_null, oyjl_t_string, oyjl_t_true, oyjl_t_false or oyjl_t_number.
   * Nested elements like oyjl_t_array of oyjl_t_object are not allowed. */
  uint64_t offsets[];                  /* distance from oyjlNodes_s to oyjlXPath_s */
} oyjlNodes_s;

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
             oyjl_val v = root->u.array.values[i];
             oyjl_type type = v->type;
             if(pos >= 0 && pos != i) continue;

             if(base)
               oyjlStringAdd( &xpath, 0,0, "%s%s[%d]",base,base[0]?"/":"",i );

             if( flags & OYJL_PATH ||
                 ( flags & OYJL_KEY &&
                   type != oyjl_t_null && type != oyjl_t_array && type != oyjl_t_object)
               )
             {
               n = 0; while(paths && *paths && (*paths)[n]) ++n;
               if(tn-1 <= level)
                 oyjlStringListAddString( paths, &n, xpath, malloc,free );
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
             {
               char * escape = NULL;
               if(strpbrk(key, "[].^$*+?(){|/"))
                 escape = oyjlJsonEscape(key, OYJL_KEY | OYJL_NO_INDEX | OYJL_REGEXP);
               oyjlStringAdd( &xpath, 0,0, "%s%s%s", base,base[0]?"/":"", escape?escape:key );
               if(escape)
                 free(escape);
             }

             if( (flags & OYJL_PATH && ocount) ||
                 (flags & OYJL_KEY && ocount == 0) )
             {
               n = 0; while(paths && *paths && (*paths)[n]) ++n;
               if(tn-1 <= level)
                 oyjlStringListAddString( paths, &n, xpath, malloc,free );
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
 *  @param[in]     root                node
 *  @param         levels              desired level depth
 *  @param         xpath               extented path expression;
 *                                     It accepts even empty terms.
 *  @param         flags               support filters:
 *                                     - ::OYJL_KEY: only keys
 *                                     - ::OYJL_PATH: only paths
 *                                     - 0 for both, paths and keys
 *                                     - ::OYJL_OBSERVE: verbose info
 *                                     - ::OYJL_NO_ALLOC: alloc only array and not strings for oyjlOBJECT_JSON
 *  @param[out]    count               number of strings in returned list
 *  @return                            the resulting string list
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/10/21
 *  @since   2017/11/10 (Oyranos: 0.9.7)
 */
char **    oyjlTreeToPaths           ( oyjl_val            root,
                                       int                 levels,
                                       const char        * xpath,
                                       int                 flags,
                                       int               * count )
{
  int pos = 0;
  int n = 0;
  char ** terms = oyjlStringSplit(xpath, '/', &n, malloc);
  char ** paths = NULL;

  if(count)
    *count = n;
  if(!root) return paths;

  if((long)root->type == oyjlOBJECT_JSON)
  {
    int i;
    oyjlNodes_s * nodes = (oyjlNodes_s *)root;

    n = nodes->count;
    oyjlAllocHelper_m( paths, char*, n + 1, malloc, return NULL );
    for(i = 0; i < n; ++i)
    {
      oyjlXPath_s * node = (oyjlXPath_s *)&((char*)nodes)[nodes->offsets[i]];
      const char * xpath = (const char*)node + sizeof(uint32_t);
      if(flags & OYJL_OBSERVE)
        oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "xpath[%d]: \"%s\"", OYJL_DBG_ARGS, i, xpath );
      if(flags & OYJL_NO_ALLOC)
        paths[i] = (char*)xpath;
      else
        paths[i] = oyjlStringCopy( xpath, malloc );
    }
    if(count)
      *count = n;
    return paths;
  }

  if(!flags) flags = OYJL_PATH | OYJL_KEY;

  oyjlTreeFind_( root, 0, levels, (const char**) terms, flags, "", &paths );

  while(paths && paths[pos]) ++pos;
  if(count)
    *count = pos;

  oyjlStringListRelease( &terms, n, free );

  return paths;
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

void       oyjlRegExpEscape2_        ( oyjl_str            text );
void       oyjlNoBracketCb_(const char * text, const char * start, const char * end, const char * search, const char ** replace, void * data OYJL_UNUSED);
/** @brief Convert strings to pass through JSON
 *
 *  @param         in                  input string
 *  @param         flags               support filters:
 *                                     - ::OYJL_KEY: handle input as a key string
 *                                     - ::OYJL_NO_INDEX: escape square brackets
 *                                     - ::OYJL_QUOTE: escape quotation marks '"'
 *                                     - ::OYJL_REGEXP: oyjlRegExpEscape()
 *                                     - ::OYJL_REVERSE: undo all escaping for storing JSON keys
 *                                     - 0 apply common basic JSON VALUE escaping
 *  @return                            the resulting string
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/09/11
 *  @since   2021/09/01 (Oyjl: 1.0.0)
 */
char *     oyjlJsonEscape            ( const char        * in,
                                       int                 flags )
{
  char * out = NULL;
  const char * t = in;
  oyjl_str tmp;
  if(!in) return NULL;

  tmp = oyjlStr_New(10,0,0);
  oyjlStr_AppendN( tmp, t, strlen(t) );
  if(flags & OYJL_REVERSE)
  {
    if(flags & OYJL_NO_INDEX || flags & OYJL_KEY)
    {
      oyjlStr_Replace( tmp, "\\\\[", "[", 0, NULL );
      oyjlStr_Replace( tmp, "\\[", "[", 0, NULL );
      oyjlStr_Replace( tmp, "\\]", "]", 0, NULL );
    }
    if(!(flags & OYJL_NO_BACKSLASH))
      oyjlStr_Replace( tmp, "\\\\", "\\", 0, NULL );
    if(flags & OYJL_QUOTE)
      oyjlStr_Replace( tmp, "\\\"", "\"", 0, NULL );
    oyjlStr_Replace( tmp, "\\b", "\b", 0, NULL );
    oyjlStr_Replace( tmp, "\\f", "\f", 0, NULL );
    oyjlStr_Replace( tmp, "\\n", "\n", 0, NULL );
    oyjlStr_Replace( tmp, "\\r", "\r", 0, NULL );
    oyjlStr_Replace( tmp, "\\t", "\t", 0, NULL );
    oyjlStr_Replace( tmp, "%33", "\033", 0, NULL );
    if(flags & OYJL_KEY)
      oyjlStr_Replace( tmp, "%37", "/", NULL,NULL );
    /* undo RegExp */
    oyjlStr_Replace( tmp, "\\.", ".", 0, NULL );
    oyjlStr_Replace( tmp, "\\^", "^", 0, NULL );
    oyjlStr_Replace( tmp, "\\$", "$", 0, NULL );
    oyjlStr_Replace( tmp, "\\*", "*", 0, NULL );
    oyjlStr_Replace( tmp, "\\+", "+", 0, NULL );
    oyjlStr_Replace( tmp, "\\?", "?", 0, NULL );
    oyjlStr_Replace( tmp, "\\(", "(", 0, NULL );
    oyjlStr_Replace( tmp, "\\)", ")", 0, NULL );
    oyjlStr_Replace( tmp, "\\{", "{", 0, NULL );
    oyjlStr_Replace( tmp, "\\|", "|", 0, NULL );
  } else
  {
    if(!(flags & OYJL_NO_BACKSLASH) || !(flags & OYJL_REGEXP))
      oyjlStr_Replace( tmp, "\\", "\\\\", oyjlNoBracketCb_, NULL );
    if(flags & OYJL_REGEXP)
      oyjlRegExpEscape2_( tmp );
    if(flags & OYJL_QUOTE)
      oyjlStr_Replace( tmp, "\"", "\\\"", 0, NULL );
    oyjlStr_Replace( tmp, "\b", "\\b", 0, NULL );
    oyjlStr_Replace( tmp, "\f", "\\f", 0, NULL );
    oyjlStr_Replace( tmp, "\n", "\\n", 0, NULL );
    oyjlStr_Replace( tmp, "\r", "\\r", 0, NULL );
    oyjlStr_Replace( tmp, "\t", "\\t", 0, NULL );
    oyjlStr_Replace( tmp, "\033", "%33", 0, NULL );
    if(flags & OYJL_KEY)
      oyjlStr_Replace( tmp, "/", "%37", NULL,NULL );
    if(flags & OYJL_REGEXP)
      oyjlStr_Replace( tmp, "\\[", "\\\\[", 0, NULL );
    else if(flags & OYJL_NO_INDEX || flags & OYJL_KEY)
    {
      oyjlStr_Replace( tmp, "[", "\\\\[", 0, NULL );
      //oyjlStr_Replace( tmp, "]", "\\\\]", 0, NULL );
    }
  }
  out = oyjlStr_Pull(tmp); 
  oyjlStr_Release( &tmp );
  return out;
}

int  oyjlTreeToJson21_(oyjl_val v, int * level, oyjl_str json)
{
  int error = 0;
  if(v)
  switch(v->type)
  {
    case oyjl_t_null:
         oyjlStr_AppendN (json, "null", 4); break;
         break;
    case oyjl_t_number:
         oyjlStr_AppendN (json, v->u.number.r, strlen(v->u.number.r));
         break;
    case oyjl_t_true:
         oyjlStr_AppendN (json, "true", 4); break;
    case oyjl_t_false:
         oyjlStr_AppendN (json, "false", 5); break;
    case oyjl_t_string:
         {
          char * escaped = oyjlJsonEscape( v->u.string, OYJL_QUOTE | OYJL_NO_BACKSLASH );
          oyjlStr_AppendN( json, "\"", 1 );
          oyjlStr_AppendN( json, escaped, strlen(escaped) );
          oyjlStr_AppendN( json, "\"", 1 );
          free( escaped );
         }
         break;
    case oyjl_t_array:
         {
           int i,
               count = v->u.array.len;

           oyjlStr_AppendN( json, "[", 1 );

           *level += 2;
           for(i = 0; i < count; ++i)
           {
             oyjlTreeToJson21_( v->u.array.values[i], level, json );
             if(count > 1)
             {
               if(i < count - 1)
                 oyjlStr_AppendN( json, ",", 1 );
             }
           }
           *level -= 2;

           oyjlStr_AppendN( json, "]", 1 );
         } break;
    case oyjl_t_object:
         {
           int i,j,
               count = v->u.object.len;

           oyjlStr_AppendN( json, "{", 1 );

           *level += 2;
           for(i = 0; i < count; ++i)
           {
             oyjlStr_AppendN( json, "\n", 1 );
             for(j = 0; j < *level; ++j) oyjlStr_AppendN( json, " ", 1 );
             if(!v->u.object.keys || !v->u.object.keys[i])
             {
               oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "missing key", OYJL_DBG_ARGS );
               return 1;
             }
             oyjlStr_AppendN( json, "\"", 1 );
             {
              char * escaped = oyjlJsonEscape( v->u.object.keys[i], OYJL_QUOTE | OYJL_NO_BACKSLASH );
              oyjlStr_AppendN( json, escaped, strlen(escaped) );
              free( escaped );
             }
             oyjlStr_AppendN( json, "\": ", 3 );
             error = oyjlTreeToJson21_( v->u.object.values[i], level, json );
             if(error) return error;
             if(count > 1)
             {
               if(i < count - 1)
                 oyjlStr_AppendN( json, ",", 1 );
             }
           }
           *level -= 2;

           oyjlStr_AppendN( json, "\n", 1 );
           for(j = 0; j < *level; ++j) oyjlStr_AppendN( json, " ", 1 );
           oyjlStr_AppendN( json, "}", 1 );
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
  oyjl_str string = oyjlStr_New(10, 0,0);
  oyjlTreeToJson21_( v, level, string );
  if(oyjlStr(string))
    *json = oyjlStr_Pull(string);
  else
    *json = NULL;
  oyjlStr_Release( &string );
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
         oyjlStringAdd (json, 0,0, "true"); break;
    case oyjl_t_false:
         oyjlStringAdd (json, 0,0, "false"); break;
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
         oyjlStringAdd (text, 0,0, "true"); break;
    case oyjl_t_false:
         oyjlStringAdd (text, 0,0, "false"); break;
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
         oyjlStr_AppendN (text, v->u.number.r, strlen(v->u.number.r));
         break;
    case oyjl_t_true:
         oyjlStr_AppendN (text, "true", 4); break;
    case oyjl_t_false:
         oyjlStr_AppendN (text, "false", 5); break;
    case oyjl_t_string:
         {
          const char * t = v->u.string;
          oyjlStr_AppendN (text, t, strlen(t));
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
                oyjlStr_AppendN( text, "\n", 1 );
                for(j = 0; j < *level; ++j) oyjlStr_AppendN( text, " ", 1 );
                oyjlStr_Add( text, "<%s>", parent_key );

                oyjlTreeToXml2_( v->u.array.values[i], parent_key, level, text );

                oyjlStr_Add( text, "</%s>", parent_key );
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
              oyjlStr_Clear( text );
              return;
            }
          }

          /* print key name of parent object */
          if(parent_key)
          {
            oyjlStr_AppendN( text, "\n", 1 );
            for(j = 0; j < *level; ++j) oyjlStr_AppendN( text, " ", 1 );
            oyjlStr_Add( text, "<%s", parent_key );

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
                oyjlStr_Add( text, " %s=\"%s\"", &v->u.object.keys[i][1], v->u.object.values[i]->u.string );
              else
                oyjlStr_Add( text, " %s=\"%s\"", &v->u.object.keys[i][1], v->u.object.values[i]->u.number.r );
            }

            /* close key name */
            oyjlStr_AppendN( text, ">", 1 );
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
              oyjlStr_AppendN( text, "\n", 1 );
              for(j = 0; j < *level; ++j) oyjlStr_AppendN( text, " ", 1 );
            }

            if( !is_array &&
                !is_object &&
                !is_inner_string )
              oyjlStr_Add( text, "<%s>", key );

            if( strcmp(key, XML_CDATA) == 0 )
              oyjlStr_AppendN( text, "<![CDATA[", 9 );

            oyjlTreeToXml2_( v->u.object.values[i], key, level, text );

            if( strcmp(key, XML_CDATA) == 0 )
              oyjlStr_AppendN( text, "]]>", 3 );

            if( !is_array &&
                !is_object &&
                !is_inner_string )
            {
              oyjlStr_Add( text, "</%s>", key );
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
              oyjlStr_AppendN( text, "\n", 1 );
              for(j = 0; j < *level; ++j) oyjlStr_AppendN( text, " ", 1 );
            }
            oyjlStr_Add( text, "</%s>", parent_key );
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
          oyjl_str str = oyjlStr_NewFrom(text, 0, 0,0);
          oyjlTreeToXml2_( v->u.object.values[0], v->u.object.keys[0], level, str );
          *text = oyjlStr_Pull( str );
          oyjlStr_Release( &str );
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
  if(tindex != NULL && tindex > term && tindex[-1] == '\\')
  {
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
  char ** xlist;
  char ** plist;

  if(flags & OYJL_PATH_MATCH_LAST_ITEMS)
  {
    const char * pterm = strrchr(path,'/');
    const char * xterm = strrchr(xpath,'/');
    if(pterm) ++pterm; else pterm = path;
    if(xterm) ++xterm; else xterm = xpath;
    if(strcmp(pterm, xterm) == 0)
      match = 1;
    return match;
  }

  xlist = oyjlStringSplit(xpath, '/', &xn, malloc);
  plist = oyjlStringSplit(path, '/', &pn, malloc);

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
        const char * key = parent->u.object.keys[j];
        char * regex = NULL;
        /* search for regex escaped key */
        if(strpbrk(key, "[].^$*+?(){|/"))
          regex = oyjlJsonEscape(key, OYJL_KEY | OYJL_REGEXP);
        if((parent->type == oyjl_t_object && strcmp( regex?regex:key, term ) == 0) ||
            /* a empty term matches to everything */
           term[0] == '\000')
        {
          found = 1;
          level = oyjlValuePosGet( parent, j );
          if(regex) free(regex);
          break;
        }
        if(regex) { free(regex); regex = NULL; }
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
          parent->u.object.keys[parent->u.object.len] = oyjlJsonEscape( term, /*OYJL_KEY |*/ OYJL_REVERSE | OYJL_REGEXP | OYJL_KEY );
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
 *  @param[in]     node                the node to search for inside the v tree
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
  int count, i;
  char ** paths = oyjlTreeToPaths( v, 10000000, NULL, OYJL_KEY, &count );
  oyjl_val test = NULL;

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

/** @brief copy only one level of allocation */
void       oyjlValueCopy             ( oyjl_val            v,
                                       oyjl_val            src )
{
    if( v == NULL || src == NULL )
      return;
    oyjlValueDebug_(src, "", "copy", 0);

    v->type = src->type;
    if( OYJL_IS_STRING(src) )
    {
      if(src->u.string)
        v->u.string = oyjlStringCopy(src->u.string, 0);
    } else
      if( OYJL_IS_NUMBER(v) )
    {
      memcpy( v, src, sizeof( oyjl_val_s ) );
      if(src->u.number.r) v->u.number.r = oyjlStringCopy(src->u.number.r, 0);
    }
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

    if((long)v->type != oyjlOBJECT_JSON)
      oyjlValueClear (v);

    free(v);
}

/** @} *//* oyjl_tree */


/** \addtogroup oyjl
 *  @{ *//* oyjl */

const char *       oyjlTreeGetString_( oyjl_val            v,
                                       int                 flags OYJL_UNUSED,
                                       const char        * path );
const char *       oyjlTreeGetStringF_(oyjl_val            v,
                                       int                 flags,
                                       const char        * format,
                                                           ... );
const char*oyjlTreeGetStringF2_      ( oyjl_val            v,
                                       int                 start,
                                       int                 end,
                                       int                 flags,
                                       const char        * format,
                                                           ... );

char *         oyjlTranslate2_       ( const char        * loc,
                                       oyjl_val            catalog,
                                       int                 start,
                                       int                 end,
                                       const char        * domain OYJL_UNUSED,
                                       const char        * text,
                                       int                 flags )
{
  const char * translated = NULL;
  oyjl_val v;
  char * key = NULL, * tmp = NULL, * loc_ = NULL;

  if(flags & OYJL_GETTEXT)
  {
#ifdef OYJL_USE_GETTEXT
    const char * t = NULL;;
    if(!domain)
    {
      if(flags & OYJL_OBSERVE)
        oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "domain is missed", OYJL_DBG_ARGS );
    } else if(text && text[0])
      t = dgettext( domain, text );
    if(t)
      translated = t;
#endif
    return translated ? (char*)translated : (char*)text;
  }

  if(!loc || !loc[0] || strcmp(loc,"C") == 0 || !catalog || !text || (text && !text[0]))
    return (char*)text;

  key = oyjlJsonEscape( text, OYJL_KEY | OYJL_REGEXP );
  if(flags & OYJL_OBSERVE)
    oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "text: \"%s\" escape: \"%s\" key: \"%s\"", OYJL_DBG_ARGS, text, tmp, key );
  if(tmp) {free(tmp); tmp = NULL;}

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
      if(key) {free(key); key = NULL;}
      return translated ? (char*)translated : (char*)text;
    }

    paths = oyjlTreeToPaths( catalog, 10000000, NULL, OYJL_KEY | OYJL_NO_ALLOC, &count );

    for(i = 0; i < count; ++i)
    {
      path = paths[i];
      if(strstr(path, "org/freedesktop/oyjl/translations/back") != NULL)
        continue;
      v = oyjlTreeGetValueF( catalog, 0, "%s", path );
      current = OYJL_GET_STRING(v);
      if(flags & OYJL_OBSERVE)
        oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "\tcurrent: \"%s\"", OYJL_DBG_ARGS, current );
      if(strcmp(current, text) == 0)
      {
        char * value = oyjlJsonEscape(strrchr(path,'/')+1, OYJL_REVERSE | OYJL_REGEXP);
        oyjlTreeSetStringF( catalog, OYJL_CREATE_NEW, value, "org/freedesktop/oyjl/translations/back/%s", key );
        free(value);
        v = oyjlTreeGetValueF( catalog, OYJL_CREATE_NEW, "org/freedesktop/oyjl/translations/back/%s", key );
        translated = OYJL_GET_STRING(v);
        break;
      }
    }

    if(paths && count)
    {
      if((long)catalog->type == oyjlOBJECT_JSON)
        free(paths);
      else
        oyjlStringListRelease( &paths, count, free );
    }

    if(key) {free(key); key = NULL;}
    return translated ? (char*)translated : (char*)text;
  }

  loc_ = oyjlJsonEscape( loc, OYJL_KEY | OYJL_REGEXP );
  translated = oyjlTreeGetStringF2_(catalog, start, end, flags, "org/freedesktop/oyjl/translations/%s/%s", loc_, key );
  if(end && !(flags & OYJL_NO_OPTIMISE))
    goto oyjlTranslate2_clean;
  if(!translated)
  {
    char * language = oyjlLanguage(loc),
         * country = oyjlCountry(loc);
    if(language && country)
        translated = oyjlTreeGetStringF_(catalog, 0, "org/freedesktop/oyjl/translations/%s_%s/%s", language, country, key );
    if(language) free(language);
    if(country) free(country);
  }
  if(!translated)
  {
    char * language = oyjlLanguage(loc);
    translated = oyjlTreeGetStringF_(catalog, 0, "org/freedesktop/oyjl/translations/%s/%s", language, key );
    if(language) free(language);
  }
  if(!translated && text[0])
  {
    char * language = oyjlLanguage(loc);
    char * path = NULL,
         * plain;
    char ** paths = NULL;
    int count, i;
    char * regex = NULL,
         * escape = oyjlRegExpEscape( text );
    oyjlStringAdd( &regex, 0,0, "org/freedesktop/oyjl/translations/%s.*/%s", language, escape );
    free(escape); escape = NULL;

    paths = oyjlTreeToPaths( catalog, 10000000, NULL, OYJL_KEY | OYJL_NO_ALLOC, &count );

    for(i = 0; i < count; ++i)
    {
      path = paths[i];
      plain = oyjlJsonEscape( path, OYJL_REVERSE | OYJL_REGEXP | OYJL_KEY );
      if(flags & OYJL_OBSERVE)
        oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "\tpath[%d]: \"%s\" regex: \"%s\"", OYJL_DBG_ARGS, i, path, regex );
      if(oyjlRegExpFind(plain, regex))
      {
        char * p = strrchr(plain, '/');
        if(p) ++p;
        if(flags & OYJL_OBSERVE)
          oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "\tp[%d]: p:\"%s\" text: \"%s\"", OYJL_DBG_ARGS, i, p, text );
        if(strcmp(p, text) == 0)
        {
          path = oyjlStringCopy( path, 0 );
          p = strrchr(path, '/');
          if(p) ++p;
          p[0] = '\000';
          oyjlStringAdd( &path, 0,0, "%s", key );
          free(plain);
          break;
        }
      }
      free(plain);
    }

    if(paths && count)
    {
      if((long)catalog->type == oyjlOBJECT_JSON)
        free(paths);
      else
        oyjlStringListRelease( &paths, count, free );
    }

    if(i == count)
      path = NULL;

    v = NULL;
    if(path)
    {
      translated = oyjlTreeGetString_( catalog, 0, path );
      if(flags & OYJL_OBSERVE)
        oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "\tpath:\"%s\" %s", OYJL_DBG_ARGS, path, translated?"found":"not found" );
      free(path);
    }
    if(language) free(language);
    if(regex) free(regex);
  }

oyjlTranslate2_clean:
  if(key) {free(key); key = NULL;}
  if(loc_) {free(loc_); loc_ = NULL;}

  return translated ? (char*)translated : (char*)text;
}

struct oyjl_tr_example_s {
  int start;                           /**< @brief loc start in catalog paths */
  int end;                             /**< @brief loc end in catalog paths */
};

/** @brief   translate string
 *
 *  @param         context             translation variables; optional,
 *                                     will try gettext without
 *  @param         text                the to be translated text; optional, will return without
 *  @return                            translated item; must not be freed
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/10/24
 *  @since   2020/07/27 (Oyjl: 1.0.0)
 */
char *         oyjlTranslate         ( oyjlTr_s          * context,
                                       const char        * text )
{
  char * translated = NULL;
  const char * lang = "C";

  if(context && text)
  {
    oyjl_val catalog = oyjlTr_GetCatalog( context );
    struct oyjl_tr_example_s * bounds = oyjlTr_GetUserData( context );
    const char * domain = oyjlTr_GetDomain( context );
    int start, end,
        flags = oyjlTr_GetFlags( context );
    if(bounds)
    {
      start = bounds->start;
      end = bounds->end;
    } else
    {
      start = oyjlTr_GetStart_( context );
      end = oyjlTr_GetEnd_( context );
    }
    if(!catalog)
      flags |= OYJL_GETTEXT;

    lang = oyjlTr_GetLang( context );
    if(start >= 0 && end >= 0)
      translated = oyjlTranslate2_( lang, catalog, start, end, domain, text, flags );
  }

  return translated ? translated : (char*)text;
}

int  oyjlXPathGetSize_               ( oyjl_val            v,
                                       const char        * xpath,
                                       uint32_t          * v_offset )
{
  int size = sizeof(uint32_t),
      /* allow for ending '\000' byte */
      v_offset_ = size + strlen( xpath ) + 1;
  oyjl_type type = v->type;

  size = v_offset_;
  if(v_offset)
    *v_offset = v_offset_;

  switch(type)
  {
    case oyjl_t_null:
    case oyjl_t_true:
    case oyjl_t_false:
      size += sizeof(oyjl_type);
      break;
    case oyjl_t_number:
      size += sizeof(oyjl_val_s) + strlen(v->u.number.r) + 1;
      break;
    case oyjl_t_string:
      size += sizeof(oyjl_type) + strlen(v->u.string) + 1;
      break;
    default: break;
  }

  return size;
}

/** @brief   write tree to data block
 *
 *  @param         v                   tree to serialise
 *  @param[in]     flags               supported:
 *                                     - OYJL_OBSERVE : to print verbose info message
 *  @param[out]    size                the size of the returned data block
 *  @return                            serialised tree
 *
 *  @see oyjlTreeDeSerialise()
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/11/01
 *  @since   2021/09/16 (Oyjl: 1.0.0)
 */
oyjl_val   oyjlTreeSerialise         ( oyjl_val            v,
                                       int                 flags,
                                       int               * size )
{
  oyjlNodes_s * nodes = NULL;
  if(v && (long)v->type == oyjlOBJECT_JSON)
  {
    char * t = oyjlBT(0);
    oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "%spassed a already serialised oiJS in", OYJL_DBG_ARGS, t );
    free(t);
  }
  else
  if(v)
  {
    int count = 0, i,
        max_u_size = sizeof(oyjl_val),
        size_ = 0;
    char ** paths = oyjlTreeToPaths( v, 1000000, NULL, OYJL_KEY, &count );

    if(flags & OYJL_OBSERVE)
      oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "oyjlNodes_s: %d oyjlXPath_s: %d count: %d", OYJL_DBG_ARGS, sizeof(oyjlNodes_s), sizeof(oyjlXPath_s), count );

    size_ += sizeof(oyjlNodes_s) + sizeof(uint64_t) * count;
    for(i = 0; i < count; ++i)
    {
      const char * xpath = paths[i];
      oyjl_val val = oyjlTreeGetValue( v, 0, xpath );
      uint32_t v_offset = 0;
      int size__ = size_;
      size_ += oyjlXPathGetSize_( val, xpath, &v_offset );
      if(flags & OYJL_OBSERVE)
        oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "xpath[%d]:\"%s\" %s v_offset: %d offset2: %d", OYJL_DBG_ARGS, i, xpath, val?"found":"not found", v_offset, size__ );
    }

    if(size)
      *size = size_;

    nodes = (oyjlNodes_s*)calloc(size_, sizeof(char));
    if(!nodes) return NULL;
    memcpy( nodes, "oiJS", 4 );

    size_ = sizeof(oyjlNodes_s) + sizeof(uint64_t) * count;
    nodes->count = count;
    for(i = 0; i < count; ++i)
    {
      const char * xpath = paths[i];
      oyjl_val val = oyjlTreeGetValue( v, 0, xpath );
      oyjl_type type = val->type;
      int len = strlen(xpath);
      uint32_t v_offset = 0;
      nodes->offsets[i] = size_;
      size_ += oyjlXPathGetSize_( val, xpath, &v_offset );
      oyjlXPath_s * node = (oyjlXPath_s *)&((char*)nodes)[nodes->offsets[i]];
      node->v_offset = v_offset;
      oyjl_val node_v = (oyjl_val)((char*)node + v_offset);
      strcpy( (char*)node + sizeof(uint32_t), xpath );
      if(flags & OYJL_OBSERVE)
        oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "\txpath:\"%s\" len:%d type:%d offsets:%d %d %d v_offset:%d", OYJL_DBG_ARGS, xpath, len, type, size_, nodes->offsets[i], (char*)node - (char*)nodes, v_offset );
      node_v->type = type;
      switch(type)
      {
        case oyjl_t_null:
        case oyjl_t_true:
        case oyjl_t_false:
          break;
        case oyjl_t_number:
          {
            int i = val->u.number.i;
            double d = val->u.number.d;
            memcpy( node_v, val, max_u_size );
            strcpy( (char*)node_v + sizeof(oyjl_val_s), val->u.number.r );
            if(flags & OYJL_OBSERVE)
              oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "\t\tnumber.i:%d number.d:%f number.r:%s", OYJL_DBG_ARGS, i, d, (char*)node_v + sizeof(oyjl_val_s) );
            break;
          }
        case oyjl_t_string:
          {
            const char * t = val->u.string;
            memcpy( node_v, val, sizeof(oyjl_type) );
            strcpy( (char*)node + v_offset + sizeof(oyjl_type), t );
            if(flags & OYJL_OBSERVE)
              oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "\t\toffset %d %s", OYJL_DBG_ARGS, max_u_size + len + 1 + v_offset, t );
          }
          break;
        default: break;
      }
    }

    oyjlStringListRelease( &paths, count, free );
  }

  return (oyjl_val)nodes;
}

const char * oyjlXPath_Print_        ( oyjlXPath_s       * node )
{
  const char * text = NULL;
  uint32_t v_offset = node->v_offset;
  int max_u_size = sizeof(oyjl_val_s);
  oyjl_val val = (oyjl_val)((char*)node + v_offset);
  oyjl_type type = val->type;
  switch(type)
  {
    case oyjl_t_null:
      text = "null";
      break;
    case oyjl_t_true:
      text = "true";
      break;
    case oyjl_t_false:
      text = "false";
      break;
    case oyjl_t_number:
      text = (const char*)val + max_u_size;
      break;
    case oyjl_t_string:
      text = (const char*)node + v_offset + sizeof(oyjl_type);
      break;
    default:
      text = ":---";
      break;
  }

  return text;
}

/** @brief   print
 *
 *  @param         v                   block to decode
 *  @param[in]     flags               supported:
 *                                     - OYJL_OBSERVE : to print verbose info message
 *  @return                            text
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/09/17
 *  @since   2021/09/17 (Oyjl: 1.0.0)
 */
char *     oyjlTreeSerialisedPrint_  ( oyjl_val            v,
                                       int                 flags OYJL_UNUSED )
{
  char * text = NULL;
  if(v && (long)v->type == oyjlOBJECT_JSON)
  {
    int count = 0, i;
    oyjlNodes_s * nodes = (oyjlNodes_s *)v;

    count = nodes->count;
    oyjlStringAdd( &text, 0,0, "oiJS:\ncount: %d\n", count );
    for(i = 0; i < count; ++i)
    {
      oyjlXPath_s * node = (oyjlXPath_s *)&((char*)nodes)[nodes->offsets[i]];
      const char * xpath = ((const char*)node) + sizeof(uint32_t);
      const char * value = oyjlXPath_Print_( node );
      oyjlStringAdd( &text, 0,0, "%s:%s\n", xpath, value );
    }
  }

  return text;
}

/** @brief   create tree from serialised data block
 *
 *  @param         v                   serialised tree
 *  @param[in]     flags               unused:
 *  @param[in]     size                the size of the data block v - not used
 *  @return                            serialised tree
 *
 *  @see oyjlTreeSerialise()
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/11/01
 *  @since   2021/11/01 (Oyjl: 1.0.0)
 */
oyjl_val   oyjlTreeDeSerialise       ( oyjl_val            v,
                                       int                 flags OYJL_UNUSED,
                                       int                 size OYJL_UNUSED )
{
  oyjl_val root = NULL;

  if(v && (long)v->type != oyjlOBJECT_JSON)
  {
    char * t = oyjlBT(0);
    oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "%spassed a non serialised oiJS in", OYJL_DBG_ARGS, t );
    free(t);
  }
  else
  if(v && (long)v->type == oyjlOBJECT_JSON)
  {
    int count = 0, i;
    oyjlNodes_s * nodes = (oyjlNodes_s *)v;

    count = nodes->count;
    if(count)
      root = oyjlTreeNew("");
    for(i = 0; i < count; ++i)
    {
      oyjlXPath_s * node = (oyjlXPath_s *)&((char*)nodes)[nodes->offsets[i]];
      const char * xpath = ((const char*)node) + sizeof(uint32_t);
      const char * text = NULL;
      uint32_t v_offset = node->v_offset;
      int max_u_size = sizeof(oyjl_val_s);
      oyjl_val val = (oyjl_val)((char*)node + v_offset);
      oyjl_type type = val->type;
      v = oyjlTreeGetValue( root, OYJL_CREATE_NEW, xpath );
      v->type = type;
      switch(type)
      {
      case oyjl_t_null:
      case oyjl_t_true:
      case oyjl_t_false:
        break;
      case oyjl_t_number:
        text = (const char*)val + max_u_size;
        memcpy( v, val, max_u_size );
        v->u.number.r = oyjlStringCopy( text, 0 );
        break;
      case oyjl_t_string:
        text = (const char*)node + v_offset + sizeof(oyjl_type);
        oyjlValueSetString( v, text );
        break;
      default:
        break;
      }
    }
  }

  return root;
}

const char * oyjlTreeGetString2_     ( oyjl_val            v,
                                       int                 flags,
                                       const char        * path,
                                       int                 start,
                                       int                 end )
{
  const char * text = NULL;
  if(v && (long)v->type == oyjlOBJECT_JSON)
  {
    int count = 0, i;
    oyjlNodes_s * nodes = (oyjlNodes_s *)v;

    count = nodes->count;
    if(start < end && end <= count)
      count = end;
    if(end && !(flags & OYJL_NO_OPTIMISE))
    {
      oyjlXPath_s * node = NULL;
      const char * xpath = NULL;
      int low = start, mid, last_mid = -1,
          high = end, compare, found = 0;
      while( low < high )
      {
        mid = start + (high-start + low-start) / 2;
        if(last_mid == mid)
          break;
        last_mid = mid;
        node = (oyjlXPath_s *)&((char*)nodes)[nodes->offsets[mid]];
        xpath = ((const char*)node) + sizeof(uint32_t);
        compare = strcmp( path, xpath );
             if( compare > 0 )
          low = mid;
        else if( compare < 0 )
          high = mid;
        else
        {
          found = 1;
          break;
        }
        if(flags & OYJL_OBSERVE)
          fprintf( stderr, "xpath[%d %d %d] = %s\n", low, mid, high, oyjlTermColor(compare>0?oyjlGREEN:oyjlBLUE, xpath) );
      }
      if(found)
        text = oyjlXPath_Print_( node );
      if(flags & OYJL_OBSERVE)
        fprintf( stderr, "     [%d] = %s: \"%s\"\n", mid, xpath, text&&found?text:oyjlTermColor(oyjlRED,"------------") );
    }
    else
    {
      for(i = start; i < count; ++i)
      {
        oyjlXPath_s * node = (oyjlXPath_s *)&((char*)nodes)[nodes->offsets[i]];
        const char * xpath = ((const char*)node) + sizeof(uint32_t);
        if(strcmp(path, xpath) != 0)
          continue;

        text = oyjlXPath_Print_( node );
        break;
      }
    }
  } else
  {
    oyjl_val val = oyjlTreeGetValue( v, 0, path );
    if(val)
      text = OYJL_GET_STRING(val);
  }

  return text;
}

const char * oyjlTreeGetString_      ( oyjl_val            v,
                                       int                 flags,
                                       const char        * path )
{
  return oyjlTreeGetString2_( v, flags, path, 0,0 );
}

const char*oyjlTreeGetStringF2_      ( oyjl_val            v,
                                       int                 start,
                                       int                 end,
                                       int                 flags,
                                       const char        * format,
                                                           ... )
{
  const char * value = NULL;
  char * text = NULL;

  OYJL_CREATE_VA_STRING(format, text, malloc, return value)

  value = oyjlTreeGetString2_( v, flags, text, start, end );

  if(text) free(text);

  return value;
}

const char*oyjlTreeGetStringF_       ( oyjl_val            v,
                                       int                 flags,
                                       const char        * format,
                                                           ... )
{
  const char * value = NULL;
  char * text = NULL;

  OYJL_CREATE_VA_STRING(format, text, malloc, return value)

  value = oyjlTreeGetString_( v, flags, text );

  if(text) free(text);

  return value;
}


char **  oyjlCatalogGetLangs_        ( char             ** paths,
                                       int                 count,
                                       int               * langs_n,
                                       int              ** lang_positions_start )
{
  char ** locs = NULL;
  int * locs_start = NULL;

  if(paths && count)
  {
    int i,j;
    int locs_n = 0;
    for(i = 0; i < count; ++i)
    {
      const char * path = paths[i];
      int len = strlen(path);
      const char * base = "org/freedesktop/oyjl/translations/";
      int base_len = strlen(base);
      char * loc, * t;
      int found = 0;

      if(!strstr(path, base))
        continue;
      loc = len > base_len + 5 ? oyjlStringCopy(&path[base_len], 0):NULL,
      t = loc ? strrchr(loc, '/') : NULL;
      if(t) t[0] = '\000';
      t = oyjlJsonEscape( loc, OYJL_REVERSE | OYJL_REGEXP | OYJL_KEY );
      free(loc);
      loc = t;
      for(j = 0; j < locs_n; ++j)
      {
        char * l = locs[j];
        if(!loc || strcmp(l, loc) == 0)
        {
          found = 1;
          break;
        }
      }
      if(found == 0)
      {
        if(!locs_start)
          oyjlAllocHelper_m( locs_start, int, 1024, malloc, return NULL );
        locs_start[locs_n] = i;
        oyjlStringListAddString( &locs, &locs_n, loc, malloc,free );
      }
      if(loc) free(loc);
    }

    if(langs_n)
      *langs_n = locs_n;
    if(lang_positions_start)
      *lang_positions_start = locs_start;
  }

  return locs;
}

/* Select a perfect fitting language for optimised access in oyjlTranslate(). */
char *             oyjlLangForCatalog_(const char        * loc,
                                       oyjl_val            catalog,
                                       int               * cstart,
                                       int               * cend,
                                       int                 flags )
{
  char * new_loc = NULL;
  int is_static = catalog && (long)catalog->type == oyjlOBJECT_JSON;

  if(!loc || flags & OYJL_OBSERVE)
  {
    const char * lang = getenv("LANG");
    if(!(!loc && lang))
    {
      char * t;
      int old_dbg = *oyjl_debug;
      *oyjl_debug = 0;
      t = oyjlBT(0);
      *oyjl_debug = old_dbg;
      fprintf( stderr, "%s", t );
      free(t);
    }
    if(loc)
      fprintf( stderr, OYJL_DBG_FORMAT "loc: %s catalog: %s contains langs:", OYJL_DBG_ARGS, loc, catalog?is_static?"oiJS":"tree":"none" );
    else
    {
      if(lang)
      {
        fprintf( stderr, "%s", oyjlTermColor(oyjlRED,"Usage Error:") );
        fprintf( stderr, OYJL_DBG_FORMAT " Environment variable possibly not correct. Translations might fail - LANG=%s\n", OYJL_DBG_ARGS, lang );
      }
      else
        fprintf( stderr, "loc argument missed\n" );
    }
  }

  if(!loc || strcmp(loc,"C") == 0 || strcmp(loc,"back") == 0)
    return new_loc;

  if(catalog)
  {
    int count, i,j;
    char ** paths = oyjlTreeToPaths( catalog, 10000000, NULL, OYJL_KEY | OYJL_NO_ALLOC, &count );

    if(paths && count)
    {
      int opt_start = 0,
          opt_end = count;
      int lang_matches = 0,
          language_matches = 0,
          lang_pos = -1;
      int langs_n = 0;
      int * lang_positions_start = NULL;
      char ** langs = NULL;
      char * language = oyjlLanguage( loc );

      langs = oyjlCatalogGetLangs_( paths, count,
                                    &langs_n, &lang_positions_start );

      for(j = 0; j < langs_n; ++j)
      {
        char * l = langs[j];
        if(strstr(l, loc) || strstr(loc, l))
        {
          ++lang_matches;
          if(lang_matches == 1)
            lang_pos = j;
        }
        if(strstr(l, language))
          ++language_matches;
        if(flags & OYJL_OBSERVE)
          fprintf( stderr, " %s", oyjlTermColor(oyjlITALIC, l) );
      }
      if(lang_matches == 1 && (!loc || strcmp(loc, langs[lang_pos]) != 0))
      {
        char * l = langs[lang_pos];
        new_loc = oyjlStringCopy( l, malloc );
        if(flags & OYJL_OBSERVE)
          fprintf( stderr, " -> %s", oyjlTermColor(oyjlBOLD, l) );
      }

      if(cstart && cend && lang_matches == 1 && lang_matches == language_matches)
      {
        for(i = 0; i < langs_n; ++i)
        {
          const char * lang = langs[i];
          if(strcmp(new_loc?new_loc:loc, lang) == 0)
          {
            opt_start = lang_positions_start[i];
            opt_end = i>=langs_n-1?count:lang_positions_start[i+1];
            break;
          }
        }
        *cstart = opt_start;
        *cend = opt_end;
        if(flags & OYJL_OBSERVE || *oyjl_debug)
        {
          fprintf( stderr, "\n" );
          oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "optimise catalog[%s] access %d -> %d\n", OYJL_DBG_ARGS, new_loc?new_loc:loc, opt_start, opt_end );
        }
      }
      else if(cstart && cend && lang_matches == 0)
      {
        if(*oyjl_debug)
        {
          if(flags & OYJL_OBSERVE)
            fprintf( stderr, "\n" );
          fprintf(stderr,OYJL_DBG_FORMAT "", OYJL_DBG_ARGS);
          fprintf(stderr,"loc: %s not supported by catalog\n", oyjlTermColor(oyjlBOLD,loc));
        }
        *cstart = -1;
        *cend = -1;
      }

      if(langs_n)
      {
        oyjlStringListRelease( &langs, langs_n, free );
        free(lang_positions_start);
        lang_positions_start = NULL;
      }

      if(is_static)
        free(paths);
      else
        oyjlStringListRelease( &paths, count, free );
      if(language)
        free(language);
    }
  }
  if(flags & OYJL_OBSERVE)
    fprintf( stderr, "\n" );

  return new_loc;
}

/** @brief   translate JSON
 *
 *  @see oyjlUi_Translate() oyjlTr_New()
 *
 *  @param[in,out] root               the tree to translate strings inside
 *  @param         context             translation variables; optional,
 *                                     will try gettext without
 *  @param[in]     key_list           comma separate list of keys to translate; optional, without the function will return
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/10/24
 *  @since   2021/07/09 (Oyjl: 1.0.0)
 */
void               oyjlTranslateJson ( oyjl_val            root,
                                       oyjlTr_s          * context,
                                       const char        * key_list )
{
  if(root)
  {
    int count = 0, i;
    char ** paths = oyjlTreeToPaths( root, 1000000, NULL,  OYJL_KEY, &count );
    oyjlTranslate_f translator = NULL;

    if(!context)
      oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "no context arg\n", OYJL_DBG_ARGS );

    translator = oyjlTr_GetTranslator( context );
    if(!translator)
      translator = oyjlTranslate;

    if(count)
    {
      int n = 0;
      char ** list = oyjlStringSplit( key_list, ',', &n, malloc );

      for(i = 0; i < count; ++i)
      {
        const char * path = paths[i];
        int j;
        const char * p = strrchr(path, '/');
        if(p) ++p;
        if(p && strstr(key_list, p))
        for(j = 0; j < n; ++j)
        {
          const char * key = list[j];
          if(oyjlPathMatch(path, key, OYJL_PATH_MATCH_LAST_ITEMS ))
          {
            const char * t = NULL;
            oyjl_val v = oyjlTreeGetValue( root, 0, path );
            if(v)
              t = OYJL_GET_STRING(v);
            if(t)
            {
              int error = 0;
              const char * i18n = translator( context, t );
              if(i18n && t && strcmp(i18n,t) != 0)
                error = oyjlValueSetString(v,i18n);
              if(error)
                oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "text: %s", OYJL_DBG_ARGS, t );
              break;
            }
          }
        }
      }
      oyjlStringListRelease( &list, n, free );
    }
    oyjlStringListRelease( &paths, count, free );
  }
}

/**
 *  Translation context to hold information for oyjlTranslate()
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/10/24
 *  @since   2021/10/24 (Oyjl: 1.0.0)
 */
struct oyjlTr_s
{
  char type [8];                       /**< @brief must be 'oitr' */
  const char * loc;                    /**< @brief original provided locale */
  char * lang;                         /**< @brief optimised loc for translator */
  const char * domain;                 /**< @brief identiefier for catalog */
  oyjl_val catalog;                    /**< @brief the translation tables */
  int start;                           /**< @brief lang start in catalog paths */
  int end;                             /**< @brief lang end in catalog paths */
  oyjlTranslate_f translator;          /**< @brief the function */
  void * user_data;                    /**< @brief additional data for translator */
  void (*deAlloc)(void*);              /**< @brief custom deallocator; optional */
  int flags;                           /**< @brief flags for translator; optional */
};

/** @brief create i18n context 
 *
 *  The passed in catalog shall contain its translations in the
 *  "org/freedesktop/oyjl/translations/loc" path.
 *
 *  @param         loc                 locale name as from setlocale(0,""), the special locale "back" will inverse the translation; optional
 *  @param         translator          the function; optional
 *  @param         catalog             the parsed catalog as tree; optional
 *  @param         user_data           optional
 *  @param         deAlloc             free user_data on oyjlTr_Release(); optional
 *  @param[in]     flags               supported:
 *                                     - ::OYJL_OBSERVE: to print verbose info
 *                                       message
 *                                     - ::OYJL_GETTEXT: force gettext API
 *                                     - ::OYJL_NO_OPTIMISE
 *  @return                            context
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/10/26
 *  @since   2021/10/24 (Oyjl: 1.0.0)
 */
oyjlTr_s *     oyjlTr_New            ( const char        * loc,
                                       const char        * domain,
                                       oyjl_val          * catalog,
                                       oyjlTranslate_f     translator,
                                       void              * user_data,
                                       void             (* deAlloc)(void*),
                                       int                 flags )
{
  oyjlTr_s * context = NULL;
  char * lang = NULL;

  oyjlAllocHelper_m( context, struct oyjlTr_s, 1, malloc, return NULL );
  memcpy( context->type, "oitr", 4 );
  context->loc = loc;
  context->domain = domain;
  if(*oyjl_debug > 1)
    fprintf(stderr, OYJL_DBG_FORMAT "loc: %s domain: %s\n", OYJL_DBG_ARGS, loc, domain );
  if(catalog)
  {
    context->catalog = *catalog;
    *catalog = NULL;
  }
  context->translator = translator;
  context->user_data = user_data;
  context->deAlloc = deAlloc;
  context->flags = flags;
  lang  = oyjlLangForCatalog_( loc, context->catalog,
                                    &context->start, &context->end,
                                    context->flags );
  context->lang = lang;

  return context;
}

int oyjlTr_Check_                    ( oyjlTr_s          * context )
{
  int success = 1;
  if( context && *(oyjlOBJECT_e*)context != oyjlOBJECT_TR)
  {
    char * a = (char*)context;
    char type[5] = {a[0],a[1],a[2],a[3],0};
    char * t = oyjlBT(0);
    fprintf(stderr, "%sUnexpected object: \"%s\"(expected: \"oyjlTr_s\")\n", t, type );
    free(t);
    success = 0;
    return success;
  }
  return success;
}
/** @brief get function
 *
 *  Fall back to oyjlTranslate().
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/10/24
 *  @since   2021/10/24 (Oyjl: 1.0.0)
 */
oyjlTranslate_f oyjlTr_GetTranslator ( oyjlTr_s          * context )
{
  return context && oyjlTr_Check_(context)?context->translator:oyjlTranslate;
}

/** @brief get catalog lang
 *
 *  Fall back to loc from oyjlTr_New() otherwise NULL.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/10/24
 *  @since   2021/10/24 (Oyjl: 1.0.0)
 */
const char * oyjlTr_GetLang          ( oyjlTr_s          * context )
{
  return context && oyjlTr_Check_(context) && context->lang ? context->lang : context?context->loc:NULL;
}

/** @brief get domain
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/10/26
 *  @since   2021/10/26 (Oyjl: 1.0.0)
 */
const char *   oyjlTr_GetDomain      ( oyjlTr_s          * context )
{
  return context && oyjlTr_Check_(context) ? context->domain : NULL;
}

/** @brief get catalog
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/10/24
 *  @since   2021/10/24 (Oyjl: 1.0.0)
 */
oyjl_val     oyjlTr_GetCatalog       ( oyjlTr_s          * context )
{
  return context && oyjlTr_Check_(context) ? context->catalog : NULL;
}

/** @brief get UserData
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/10/24
 *  @since   2021/10/24 (Oyjl: 1.0.0)
 */
void *       oyjlTr_GetUserData      ( oyjlTr_s          * context )
{
  return context && oyjlTr_Check_(context) ? context->user_data : NULL;
}

/** @brief get flags
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/10/24
 *  @since   2021/10/24 (Oyjl: 1.0.0)
 */
int          oyjlTr_GetFlags         ( oyjlTr_s          * context )
{
  return context && oyjlTr_Check_(context) ? context->flags : 0;
}

/** @brief get start
 *  @internal
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/10/24
 *  @since   2021/10/24 (Oyjl: 1.0.0)
 */
int          oyjlTr_GetStart_        ( oyjlTr_s          * context )
{
  return context && oyjlTr_Check_(context) ? context->start : 0;
}

/** @brief get end
 *  @internal
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/10/24
 *  @since   2021/10/24 (Oyjl: 1.0.0)
 */
int          oyjlTr_GetEnd_          ( oyjlTr_s          * context )
{
  return context && oyjlTr_Check_(context) ? context->end : 0;
}

/** @brief   change flags
*
*  @param          context            the translation context
*  @param          flags              translation flags
*
*  @version Oyjl: 1.0.0
*  @date    2021/10/24
*  @since   2021/10/24 (Oyjl: 1.0.0)
*/
void           oyjlTr_SetFlags       ( oyjlTr_s          * context,
                                       int                 flags )
{
  if(context && oyjlTr_Check_(context))
    context->flags = flags;
}
/** @brief   change language
*
*  @param          context            the translation context
*  @param          loc                locale name as from setlocale("")
*                                     - "something": set lang to "something"
*
*  @version Oyjl: 1.0.0
*  @date    2021/10/24
*  @since   2021/10/24 (Oyjl: 1.0.0)
*/
void           oyjlTr_SetLocale      ( oyjlTr_s          * context,
                                       const char        * loc )
{
  if(context && oyjlTr_Check_(context) && loc && loc[0])
  {
    context->loc = loc;
    context->start = 0;
    context->end = 0;
    if(context->lang)
      free(context->lang);
    context->lang  = oyjlLangForCatalog_( loc, context->catalog,
                                          &context->start, &context->end,
                                          context->flags );
  }
}

/** @brief get flags
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/10/24
 *  @since   2021/10/24 (Oyjl: 1.0.0)
 */
void           oyjlTr_Release        ( oyjlTr_s         ** context_ )
{
  oyjlTr_s * context;
  if(!(context_ && *context_ && oyjlTr_Check_(*context_)))
    return;

  context = *context_;
  context->loc = NULL;
  if(context->catalog)
    oyjlTreeFree( context->catalog );
  context->catalog = NULL;
  context->translator = NULL;
  if(context->deAlloc)
    context->deAlloc( context->user_data );
  context->user_data = NULL;
  context->deAlloc = NULL;
  context->flags = 0;
  context->start = 0;
  context->end = 0;
  if(context->lang)
    free(context->lang);
  context->lang = NULL;
  free(context);
  context = NULL;

  *context_ = context;
}



oyjlTr_s ** oyjl_tr_context_ = NULL;
/** @brief   change language
 *
 *  Call after oyjlTr().
 *
 *  @param         loc                 locale name as from setlocale("")
 *                                     - NULL: reset
 *                                     - "": get existing without change
 *                                     - "something": set lang to "something"
 *  @return                            current locale
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/10/24
 *  @since   2020/07/28 (Oyjl: 1.0.0)
 */
const char *   oyjlLang              ( const char        * loc )
{
  const char * lang = NULL;

  if(oyjl_tr_context_)
  {
    int i = 0;
    while(oyjl_tr_context_[i])
    {
      oyjlTr_s * context = oyjl_tr_context_[i];
      const char * domain = oyjlTr_GetDomain(context);
      if(*oyjl_debug > 1)
      {
        char * t = oyjlBT(0);
        oyjlMessage_p( oyjlMSG_INFO, 0, "%s", t );
        free(t);
        oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "loc: %s context[%d]->loc: %s lang: %s domain: %s", OYJL_DBG_ARGS, loc, i, context->loc, lang, domain );
      }
      oyjlTr_SetLocale( context, loc );
      lang = context->lang?context->lang:context->loc;
      if(*oyjl_debug > 1)
        oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "loc: %s context[%d]->loc: %s lang: %s", OYJL_DBG_ARGS, loc, i, context->loc, lang );
      ++i;
    }
  }

  return lang;
}

/** @brief   get message translation context
 *
 *  @param         domain              select domain of library or application
 *  @return                            context for domain
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/10/26
 *  @since   2021/10/26 (Oyjl: 1.0.0)
 */
oyjlTr_s *     oyjlTr_Get            ( const char        * domain )
{
  oyjlTr_s * context = NULL;

  if(oyjl_tr_context_ && domain)
  {
    int i = 0;
    while(oyjl_tr_context_[i])
    {
      context = oyjl_tr_context_[i];
      if(context->domain && domain && strcmp(context->domain, domain) == 0)
        break;
      else
        context = NULL;
      ++i;
    }
  }

  return context;
}

int         oyjl_tr_context_reserve_ = 0;
/** @brief   set message translation context
 *
 *  @param         context             message context for oyjlTranslate()
 *                                     - oyjlTr_s context: move in as new current
 *  @return                            state
 *                                     - -1: no domain
 *                                     - 0: nothing found for erase
 *                                     - 1: old context found
 *                                     - 2: old context erased
 *                                     - 3: context replaced
 *                                     - 5: context kept
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/11/01
 *  @since   2021/10/26 (Oyjl: 1.0.0)
 */
int            oyjlTr_Set            ( oyjlTr_s         ** context )
{
  int i = 0, pos = -1;
  oyjlTr_s * oyjl_tr_context = NULL;
  int state = -1;
  const char * domain;

  if(context && *context && !oyjlTr_Check_(*context))
    return -2;

  domain = oyjlTr_GetDomain( *context );

  if(!domain)
  {
    oyjlMessage_p( oyjlMSG_INSUFFICIENT_DATA, 0, OYJL_DBG_FORMAT "domain arg missed", OYJL_DBG_ARGS );
    return state;
  }
  state = 0;

  while(oyjl_tr_context_ && oyjl_tr_context_[i])
  {
    oyjlTr_s * context = oyjl_tr_context_[i];
    if(context->domain && domain && strcmp(context->domain, domain) == 0)
    {
      pos = i;
      break;
    }
    ++i;
  }
  if(pos >= 0)
  {
    oyjl_tr_context = oyjl_tr_context_[pos];
    state |= 1;
    if(*oyjl_debug)
    {
      int erase = context && oyjl_tr_context && oyjl_tr_context != *context,
          keep = context && oyjl_tr_context == *context,
          replace = !erase && context && *context;
      char * t = oyjlBT(0);
      oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "%s[%d] domain: \"%s\" show", OYJL_DBG_ARGS, t, pos, domain, erase?"erase":keep?"keep":replace?"replace":"show on return" );
      free(t);
    }
  }

  if(context && oyjl_tr_context == *context)
    return 1|4;

  if(context && oyjl_tr_context && oyjl_tr_context != *context)
  {
    oyjlTr_Release(&oyjl_tr_context_[pos]);
    state |= 2;
  }

  if(context && *context)
  {
    if(!oyjl_tr_context_)
    {
      oyjlAllocHelper_m( oyjl_tr_context_, oyjlTr_s*, 10, malloc, return -2 );
      oyjl_tr_context_reserve_ = 10;
    }
    if(i == oyjl_tr_context_reserve_)
    {
      oyjl_tr_context_ = realloc( oyjl_tr_context_, sizeof(oyjlTr_s*) * oyjl_tr_context_reserve_ * 2 );
      if(!oyjl_tr_context_)
      {
        oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "domain: \"%s\" alloc failed: %d", OYJL_DBG_ARGS, domain, i );
        return -2;
      } else
        oyjl_tr_context_reserve_ *= 2;
    }
    oyjl_tr_context_[i] = *context;
    *context = NULL;
  }

  return state;
}

/** @brief   unregister message translation context
 *
 *  @param         domain              select domain of library or application
 *  @return                            state
 *                                     - -1: no domain
 *                                     - 0: nothing found for erase
 *                                     - 1: old context found
 *                                     - 2: old context erased
 *                                     - 3: context replaced
 *                                     - 5: context kept
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/11/01
 *  @since   2021/10/26 (Oyjl: 1.0.0)
 */
int            oyjlTr_Unset          ( const char        * domain )
{
  int i = 0, pos = -1;
  oyjlTr_s * oyjl_tr_context = NULL;
  int state = -1;

  if(!domain)
  {
    oyjlMessage_p( oyjlMSG_INSUFFICIENT_DATA, 0, OYJL_DBG_FORMAT "domain arg missed", OYJL_DBG_ARGS );
    return state;
  }
  state = 0;

  while(oyjl_tr_context_ && oyjl_tr_context_[i])
  {
    oyjlTr_s * context = oyjl_tr_context_[i];
    if(context->domain && domain && strcmp(context->domain, domain) == 0)
    {
      pos = i;
      break;
    }
    ++i;
  }
  if(pos >= 0)
  {
    oyjl_tr_context = oyjl_tr_context_[pos];
    state |= 1;
    if(*oyjl_debug)
    {
      char * t = oyjlBT(0);
      oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "%s[%d] domain: \"%s\" %s", OYJL_DBG_ARGS, t, pos, domain, "erase" );
      free(t);
    }
  }

  if(oyjl_tr_context && pos >= 0 && oyjl_tr_context != oyjl_tr_context_[pos])
  {
    oyjlTr_Release(&oyjl_tr_context_[pos]);
    state |= 2;
  }

  return state;
}

/** @} *//* oyjl */

