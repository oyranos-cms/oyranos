/** @file oyjl_args_tree.c
 *
 *  oyjl - UI helpers
 *
 *  @par Copyright:
 *            2018-2023 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl argument handling with libOyjlCore
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *
 *  Copyright (c) 2018-2022  Kai-Uwe Behrmann  <ku.b@gmx.de>
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

#include "oyjl.h"
#include "oyjl_macros.h"
#include "oyjl_i18n_internal.h"
#include "oyjl_tree_internal.h"

#include <stddef.h>
#include <ctype.h> /* toupper() */
#ifdef OYJL_HAVE_LANGINFO_H
#include <langinfo.h> /* nl_langinfo() */
#endif

extern int * oyjl_debug;

oyjlOptionChoice_s * oyjlOption_GetChoices_ (
                                       oyjlOption_s      * o,
                                       int               * selected,
                                       oyjlOptions_s     * opts );

/** @brief    Convert the parsed content to JSON
 *  @memberof oyjlOptions_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2022/05/01
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
char * oyjlOptions_ResultsToJson     ( oyjlOptions_s     * opts,
                                       int                 flags )
{
  char * rjson = NULL;
  oyjlOptsPrivate_s * results = opts->private_data;
  oyjl_val root, value;
  int i;

  if(!results)
  {
    if(oyjlOptions_Parse( opts ))
      return NULL;

    results = opts->private_data;
    if(!results)
      return NULL;
  }

  root = oyjlTreeNew( "" );
  for(i = 0; i < results->count; ++i)
  {
    oyjl_val v = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, "%s", results->options[i] );
    int count = oyjlValueCount( v );
    value = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, "%s/[%d]", results->options[i], count );
    oyjlValueSetString( value, results->values[i] );
  }

  rjson = oyjlTreeToText( root, flags );
  oyjlTreeFree( root );

  return rjson;
}

/** @brief    Add informations
 *  @memberof oyjlOptions_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2020/10/26
 *  @since   2020/10/26 (Oyjl: 1.0.0)
 */
void     oyjlOptions_SetAttributes   ( oyjlOptions_s     * opts,
                                       oyjl_val          * root )
{
  oyjlOptsPrivate_s * results = opts->private_data;

  if(!results)
  {
    if(oyjlOptions_Parse( opts ))
      return;

    results = opts->private_data;
    if(!results)
      return;
  }

  if(results->attr)
    oyjlTreeFree( results->attr );
  results->attr = NULL;
  if(root)
  {
    results->attr = *root;
    *root = NULL;
  }
}

/** \addtogroup oyjl_args
 *  @{ */

/** @brief    Convert separated string to result JSON
 *
 *  @param         opts                separate by colon ':' and omit leading dash '-' or '--'
 *  @result                            result JSON @see oyjlOptions_ResultsToJson()
 *
 *  @version Oyjl: 1.0.0
 *  @date    2023/05/22
 *  @since   2023/05/21 (Oyjl: 1.0.0)
 */
oyjl_val oyjlOptionStringToJson      ( const char        * opts )
{
  oyjl_val root, val;
  int i, n = 0;
  char ** list = oyjlStringSplit( opts, ':', &n, malloc );

  root = oyjlTreeNew( "" );
  for(i = 0; i < n; ++i)
  {
    char * key = oyjlStringCopy( list[i], 0 ), * value;
    if(!key) continue;
    value = strchr( key, '=' );
    if(value && value[0] != '\000')
    {
      value[0] = '\000';
      ++value;
    }
    oyjl_val v = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, "%s", key );
    int count = oyjlValueCount( v );
    val = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, "%s/[%d]", key, count );
    oyjlValueSetString( val, value );
    free(key);
  }

  return root;
}

/** @brief    Change Defaults
 *
 *  @param         root                --export=json output
 *  @param         defaults            parsed oyjlOptionStringToJson() JSON
 *
 *  @see oyjlOptions_ResultsToJson()
 *
 *  @version Oyjl: 1.0.0
 *  @date    2023/05/21
 *  @since   2023/05/21 (Oyjl: 1.0.0)
 */
void             oyjlUiJsonSetDefaults(oyjl_val            root,
                                       oyjl_val            defaults )
{
  oyjl_val module = oyjlTreeGetValue(root, 0, "org/freedesktop/oyjl/modules/[0]"); // use UI JSON
  oyjl_val groups = oyjlTreeGetValue(module, 0, "groups");
  int groups_n = oyjlValueCount(groups), count = 0, i,j,k;
  char ** defaults_list = oyjlTreeToPaths( defaults, OYJL_KEY, NULL, 0, &count );
  if(count)
    oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "found groups: %d defaults: %d", OYJL_DBG_ARGS, groups_n, count );
  for(i = 0; i < groups_n; ++i)
  {
    oyjl_val opts = oyjlTreeGetValueF(groups, 0, "[%d]/options", i), v;
    int opts_n = oyjlValueCount( opts );
    for(j = 0; j < opts_n; ++j)
    {
      oyjl_val o = oyjlTreeGetValueF( opts, 0, "[%d]", j );
      const char * key;
      v = oyjlTreeGetValue( o, 0, "key" );
      key = OYJL_GET_STRING(v);
      for(k = 0; k < count; ++k)
      {
        const char * dpath = defaults_list[k],
                   * dkey = strrchr(dpath,'/') ? strrchr(dpath,'/')+1 : dpath;
        if(strcmp(key, dkey) == 0)
        {
          oyjl_val d = oyjlTreeGetValueF(defaults, 0, "%s", dkey);
          int x = oyjlValueCount( d );
          const char * value;
          if(x == 0) { fprintf( stderr, " noValue:k=%d ", k ); continue; }
          v = oyjlTreeGetValueF( d, 0, "[%d]", x-1 ); /* take the last */
          value = OYJL_GET_STRING(v);
          v = oyjlTreeGetValueF( o, 0, "default" );
          oyjlValueSetString( v, value );
          fprintf( stderr, "\"%s\":default=\"%s\"\n", key, OYJL_E(value,"---") );
        }
      }
    }
  }
}
/** @} *//* oyjl_args */

/** @brief    Release "oiui" completely
 *  @memberof oyjlUi_s
 *
 *  Release oyjlUi_s::opts, oyjlUi_s::private_data, oyjlUi_s and
 *  tree objects.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2020/11/06
 *  @since   2020/11/06 (Oyjl: 1.0.0)
 */

void           oyjlUi_Release     ( oyjlUi_s      ** ui )
{
  if(!ui || !*ui) return;
  if( *(oyjlOBJECT_e*)*ui != oyjlOBJECT_UI)
  {
    char * a = (char*)*ui;
    char type[5] = {a[0],a[1],a[2],a[3],0};
    fprintf(stderr, "Unexpected object: \"%s\"(expected: \"oyjlUi_s\")\n", type );
    return;
  }
  oyjlOptions_SetAttributes( (*ui)->opts, NULL );
  oyjlUi_ReleaseArgs( ui );
}

#define CASE_RETURN_ENUM( text_ ) case text_: return #text_;
static const char * oyjlOPTIONTYPE_eToString_( oyjlOPTIONTYPE_e e )
{
  switch( e )
  {
    CASE_RETURN_ENUM(oyjlOPTIONTYPE_START)
    CASE_RETURN_ENUM(oyjlOPTIONTYPE_CHOICE)
    CASE_RETURN_ENUM(oyjlOPTIONTYPE_FUNCTION)
    CASE_RETURN_ENUM(oyjlOPTIONTYPE_DOUBLE)
    CASE_RETURN_ENUM(oyjlOPTIONTYPE_NONE)
    CASE_RETURN_ENUM(oyjlOPTIONTYPE_END)
  }
  return NULL;
}
static const char * oyjlVARIABLE_eToString_( oyjlVARIABLE_e e )
{
  switch( e )
  {
    CASE_RETURN_ENUM(oyjlNONE)
    CASE_RETURN_ENUM(oyjlSTRING)
    CASE_RETURN_ENUM(oyjlDOUBLE)
    CASE_RETURN_ENUM(oyjlINT)
  }
  return NULL;
}

#define OYJL_PARENT_OIWI 0x01
#define OYJL_PARENT_OIWG 0x02
void oyjlUi_ExportToJson_SetProperties_(oyjl_val           parent,
                                       const char        * properties,
                                       int                 flags )
{
  if(properties && properties[0])
  {
    int n = 0, plen = 0;
    char ** list = oyjlStringSplit( properties, '\n', &n, malloc );
    const char * parent_id = NULL;
    if(flags & OYJL_PARENT_OIWI) parent_id = "oiwi/";
    if(flags & OYJL_PARENT_OIWG) parent_id = "oiwg/";
    if(parent) plen = strlen(parent_id) ;
    if(!list)
      oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "found issue with properties: %s", OYJL_DBG_ARGS,
                     properties );
    else
    {
      char * key, * value, * t;
      int j;
      for(j = 0; j < n; ++j)
      {
        key = list[j];
        t = strchr(key, '=');
        if(!t)
          oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "found issue with properties: %s", OYJL_DBG_ARGS,
                         key );
        else
        {
          value = strchr(key, '=') + 1;
          t[0] = '\000';
          oyjlTreeSetStringF( parent, OYJL_CREATE_NEW, value, "properties/%s", key );
          if(oyjlStringStartsWith(key, parent_id))
            oyjlTreeSetStringF( parent, OYJL_CREATE_NEW, value, key+plen );
        }
      }
      oyjlStringListRelease( &list, n, free );
    }
  }
}

#define OYJL_REG "org/freedesktop/oyjl"
#define OYJL_IS_NOT_O( x ) (!o->o || strcmp(o->o,x) != 0)
#define OYJL_IS_O( x ) (o->o && strcmp(o->o,x) == 0)

/** @brief    Return a JSON dump
 *  @memberof oyjlUi_s
 *  @internal
 *
 *  The returned JSON is a complete dump of all oyjlUi_s data.
 *
 *  @see oyjlUi_ExportToJson()
 *
 *  @version Oyjl: 1.0.0
 *  @date    2020/10/12
 *  @since   2019/06/16 (Oyjl: 1.0.0)
 */
oyjl_val       oyjlUi_ExportToJson_  ( oyjlUi_s          * ui,
                                       int                 flags OYJL_UNUSED )
{
  oyjl_val root = NULL, attr = NULL;
  int i,j,n,ng;
  oyjlOptsPrivate_s * results = NULL;

  if(!ui) return root;
  if(!ui->opts)
  {
    oyjlMessage_p( oyjlMSG_INSUFFICIENT_DATA, 0, "ui->opts are missed");
    return root;
  }

  results = ui->opts->private_data;
  if(results && results->attr)
    attr = results->attr;

  root = oyjlTreeNew( "" );
  oyjlTreeSetStringF(   root, OYJL_CREATE_NEW, "1", OYJL_REG "/ui/oyjl_args_api_version" );
  oyjlTreeSetStringF(   root, OYJL_CREATE_NEW, "This is a dump of the oyjlUi_s data structure", OYJL_REG "/ui/comment" );
  oyjlTreeSetStringF(   root, OYJL_CREATE_NEW, ui->type, OYJL_REG "/ui/type" );
  if(ui->app_type && ui->app_type[0])
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, ui->app_type, OYJL_REG "/ui/app_type" );
  oyjlTreeSetStringF(   root, OYJL_CREATE_NEW, ui->nick, OYJL_REG "/ui/nick" );
  oyjlTreeSetStringF(   root, OYJL_CREATE_NEW, ui->name, OYJL_REG "/ui/name" );
  oyjlTreeSetStringF(   root, OYJL_CREATE_NEW, ui->description, OYJL_REG "/ui/description" );
  if(ui->logo)
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, ui->logo, OYJL_REG "/ui/logo" );

  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "oyjlHeaderSection_s", OYJL_REG "/ui/header/%s", "comment" );
  n = oyjlUiHeaderSection_Count( ui->sections );
  for(i = 0; i < n; ++i)
  {
    oyjlUiHeaderSection_s * s = &ui->sections[i];
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, s->type, OYJL_REG "/ui/header/sections/[%d]/%s", i, "type" );
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, s->nick, OYJL_REG "/ui/header/sections/[%d]/%s", i, "nick" );
    if(s->label)
      oyjlTreeSetStringF(root,OYJL_CREATE_NEW, s->label, OYJL_REG "/ui/header/sections/[%d]/%s", i, "label" );
    if(s->name)
      oyjlTreeSetStringF(root,OYJL_CREATE_NEW, s->name, OYJL_REG "/ui/header/sections/[%d]/%s", i, "name" );
    if(s->description)
      oyjlTreeSetStringF(root,OYJL_CREATE_NEW, s->description, OYJL_REG "/ui/header/sections/[%d]/%s", i, "description" );
  }

  oyjlTreeSetStringF( root, OYJL_CREATE_NEW,   ui->opts->type, OYJL_REG "/ui/options/type" );
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW,   "oyjlOptions_s", OYJL_REG "/ui/options/comment" );
  if(ui->opts && ui->opts->user_data)
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "1", OYJL_REG "/ui/options/user_data" );
  int nopts = oyjlOptions_Count( ui->opts );
  for(i = 0; i < nopts; ++i)
  {
    oyjlOption_s * o = &ui->opts->array[i];
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, o->type, OYJL_REG "/ui/options/array/[%d]/%s", i, "type" );
    oyjlTreeSetIntF( root, OYJL_CREATE_NEW, o->flags, OYJL_REG "/ui/options/array/[%d]/%s", i, "flags" );
    if(o->o)
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, o->o, OYJL_REG "/ui/options/array/[%d]/%s", i, "o" );
    if(o->option)
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, o->option, OYJL_REG "/ui/options/array/[%d]/%s", i, "option" );
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, o->key, OYJL_REG "/ui/options/array/[%d]/%s", i, "key" );
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, o->name, OYJL_REG "/ui/options/array/[%d]/%s", i, "name" );
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, o->description, OYJL_REG "/ui/options/array/[%d]/%s", i, "description" );
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, o->help, OYJL_REG "/ui/options/array/[%d]/%s", i, "help" );
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, o->value_name, OYJL_REG "/ui/options/array/[%d]/%s", i, "value_name" );
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, oyjlOPTIONTYPE_eToString_(o->value_type), OYJL_REG "/ui/options/array/[%d]/%s", i, "value_type" );
    switch(o->value_type)
    {
      case oyjlOPTIONTYPE_CHOICE:
        oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "oyjlOptionChoice_s * list", OYJL_REG "/ui/options/array/[%d]/values/choices/%s", i, "comment" );
        n = oyjlOptionChoice_Count( o->values.choices.list );
        if(n)
          for(j = 0; j < n; ++j)
          {
            oyjlOptionChoice_s * c = &o->values.choices.list[j];
            oyjlTreeSetStringF( root, OYJL_CREATE_NEW, c->nick, OYJL_REG "/ui/options/array/[%d]/values/choices/list/[%d]/%s", i, j, "nick" );
            oyjlTreeSetStringF( root, OYJL_CREATE_NEW, c->name, OYJL_REG "/ui/options/array/[%d]/values/choices/list/[%d]/%s", i, j, "name" );
            oyjlTreeSetStringF( root, OYJL_CREATE_NEW, c->description, OYJL_REG "/ui/options/array/[%d]/values/choices/list/[%d]/%s", i, j, "description" );
            oyjlTreeSetStringF( root, OYJL_CREATE_NEW, c->help, OYJL_REG "/ui/options/array/[%d]/values/choices/list/[%d]/%s", i, j, "help" );
          }
        else
          oyjlTreeSetStringF( root, OYJL_CREATE_NEW, NULL, OYJL_REG "/ui/options/array/[%d]/values/choices/list", i );
        oyjlTreeSetDoubleF( root, OYJL_CREATE_NEW, o->values.choices.selected, OYJL_REG "/ui/options/array/[%d]/values/choices/%s", i, "selected" );
        break;
      case oyjlOPTIONTYPE_FUNCTION:
        {
          const char * txt;
          oyjl_str tmp = oyjlStr_New(10,0,0);
          if(OYJL_IS_O("@"))
            oyjlStr_AppendN( tmp, "base", 4 );
          else if(o->option)
            oyjlStr_Push( tmp, o->option );
          oyjlStr_Add( tmp, "GetChoices" );
          oyjlStr_Replace( tmp, "-", "_", 0, NULL );
          oyjlStr_Replace( tmp, "+", "_plus_", 0, NULL );
          txt = oyjlStr(tmp); 
          oyjlTreeSetStringF( root, OYJL_CREATE_NEW, o->values.getChoices ? txt:NULL, OYJL_REG "/ui/options/array/[%d]/values/%s", i, "getChoices" );
          oyjlStr_Release( &tmp );
        }
        break;
      case oyjlOPTIONTYPE_DOUBLE:
        oyjlTreeSetDoubleF( root, OYJL_CREATE_NEW, o->values.dbl.d, OYJL_REG "/ui/options/array/[%d]/values/dbl/%s", i, "d" );
        oyjlTreeSetDoubleF( root, OYJL_CREATE_NEW, o->values.dbl.start, OYJL_REG "/ui/options/array/[%d]/values/dbl/%s", i, "start" );
        oyjlTreeSetDoubleF( root, OYJL_CREATE_NEW, o->values.dbl.end, OYJL_REG "/ui/options/array/[%d]/values/dbl/%s", i, "end" );
        oyjlTreeSetDoubleF( root, OYJL_CREATE_NEW, o->values.dbl.tick, OYJL_REG "/ui/options/array/[%d]/values/dbl/%s", i, "tick" );
        break;
      case oyjlOPTIONTYPE_NONE:
        oyjlTreeSetStringF( root, OYJL_CREATE_NEW, NULL, OYJL_REG "/ui/options/array/[%d]/%s", i, "values" );
        break;
      case oyjlOPTIONTYPE_START:
      case oyjlOPTIONTYPE_END:
        break;
    }
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, oyjlVARIABLE_eToString_(o->variable_type), OYJL_REG "/ui/options/array/[%d]/%s", i, "variable_type" );
    if(o->variable_type != oyjlNONE)
      oyjlTreeSetStringF( root, OYJL_CREATE_NEW, NULL, OYJL_REG "/ui/options/array/[%d]/%s", i, "variable_name" );
    oyjlUi_ExportToJson_SetProperties_( oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/ui/options/array/[%d]", i ), o->properties, OYJL_PARENT_OIWI );
  }

  ng = oyjlOptions_CountGroups( ui->opts );
  for(i = 0; i < ng; ++i)
  {
    oyjlOptionGroup_s * g = &ui->opts->groups[i];
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, g->type, OYJL_REG "/ui/options/groups/[%d]/%s", i, "type" );
    oyjlTreeSetIntF( root, OYJL_CREATE_NEW, g->flags, OYJL_REG "/ui/options/groups/[%d]/%s", i, "flags" );
    if(g->name)
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, g->name, OYJL_REG "/ui/options/groups/[%d]/%s", i, "name" );
    if(g->description)
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, g->description, OYJL_REG "/ui/options/groups/[%d]/%s", i, "description" );
    if(g->help)
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, g->help, OYJL_REG "/ui/options/groups/[%d]/%s", i, "help" );
    if(g->mandatory)
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, g->mandatory, OYJL_REG "/ui/options/groups/[%d]/%s", i, "mandatory" );
    if(g->optional)
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, g->optional, OYJL_REG "/ui/options/groups/[%d]/%s", i, "optional" );
    if(g->detail)
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, g->detail, OYJL_REG "/ui/options/groups/[%d]/%s", i, "detail" );
    if(g->properties)
      oyjlUi_ExportToJson_SetProperties_( oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/ui/options/groups/[%d]", i ), g->properties, OYJL_PARENT_OIWG );
  }

  /** Merge in the JSON strings and numbers from oyjlOptions_SetAttributes(). */
  if(attr)
  {
    oyjl_val v;
    char ** attr_paths = oyjlTreeToPaths( attr, 0, NULL, 0, &n );
    for(i = 0; i < n; ++i)
    {
      oyjl_type type;
      oyjl_val rootv;
      oyjl_val attrv;
      const char * path = attr_paths[i];
      attrv = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/ui/attr/%s", path ); /* store for easy deserialisation */
      v = oyjlTreeGetValue( attr, 0, path );
      if(!v) continue;
      type = v->type;
      rootv = oyjlTreeGetValue( root, OYJL_CREATE_NEW, path );
      switch(type)
      {
        case oyjl_t_string: /* 1 - a text in UTF-8 */
              if(path[strlen(path)-2] == '/' && path[strlen(path)-1] == 'o')
              {
                const char * attr_string = OYJL_GET_STRING(v), * root_string = OYJL_GET_STRING(rootv);
                if(attr_string && root_string && strcmp( attr_string, root_string ) != 0)
                  oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "%s changing ID: %s -> %s",
                                 OYJL_DBG_ARGS, path, root_string, attr_string );
              }
              oyjlValueSetString( rootv, v->u.string );
              oyjlValueSetString( attrv, v->u.string );
              break;
        case oyjl_t_number: /* 2 - floating or integer number */
              oyjlValueSetDouble( rootv, v->u.number.d );
              oyjlValueSetDouble( attrv, v->u.number.d );
              break;
        case oyjl_t_true:   /* 5 - boolean true or 1 */
        case oyjl_t_false:  /* 6 - boolean false or 0 */
        case oyjl_t_any:    /* 8 - not valid */
              oyjlMessage_p( oyjlMSG_INSUFFICIENT_DATA, 0, "%s->type is set to unsupported: %d", path, type );
              break;
        case oyjl_t_object: /* 3 - a JSON object */
        case oyjl_t_array:  /* 4 - a JSON array */
        case oyjl_t_null:   /* 7 - empty value */
              break;
      }
    }
    oyjlStringListRelease( &attr_paths, n, free );
  }

  return root;
}

/** @brief    Return a JSON dump
 *  @memberof oyjlUi_s
 *
 *  The returned JSON is a complete dump of all oyjlUi_s data.
 *
 *  @see oyjlUiJsonToCode()
 *
 *  @version Oyjl: 1.0.0
 *  @date    2022/05/01
 *  @since   2019/06/16 (Oyjl: 1.0.0)
 */
char *       oyjlUi_ExportToJson     ( oyjlUi_s          * ui,
                                       int                 flags )
{
  oyjl_val root = oyjlUi_ExportToJson_( ui, flags );
  char * t = oyjlTreeToText( root, flags );
  oyjlTreeFree( root );

  return t;
}

oyjl_val     oyjlUi_ToJson_          ( oyjlUi_s          * ui )
{
  char * t = NULL, num[64];
  oyjl_val root = NULL, key;
  int i,j,k,n,ng,nopts;
  oyjlOptions_s * opts;

  if(!ui) return root;

  root = oyjlTreeNew( "" );
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "1", OYJL_REG "/modules/[0]/oyjl_module_api_version" );
  if(ui->app_type && ui->app_type[0])
  {
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, ui->app_type, OYJL_REG "/modules/[0]/type" );
    if(strcmp( ui->app_type, "tool" ) == 0)
      oyjlTreeSetStringF( root, OYJL_CREATE_NEW, _("Tool"), OYJL_REG "/modules/[0]/label" );
    else if(strcmp( ui->app_type, "module" ) == 0)
      oyjlTreeSetStringF( root, OYJL_CREATE_NEW, _("Module"), OYJL_REG "/modules/[0]/label" );
  }
  if(ui->nick)
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, ui->nick, OYJL_REG "/modules/[0]/nick" );
  if(ui->name)
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, ui->name, OYJL_REG "/modules/[0]/name" );
  if(ui->description)
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, ui->description, OYJL_REG "/modules/[0]/description" );
  if(ui->logo)
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, ui->logo, OYJL_REG "/modules/[0]/logo" );

  n = oyjlUiHeaderSection_Count( ui->sections );
  for(i = 0; i < n; ++i)
  {
    oyjlUiHeaderSection_s * s = &ui->sections[i];
    key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/information/[%d]/%s", i, "type" );
    oyjlValueSetString( key, s->nick );
    key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/information/[%d]/%s", i, "label" );
    if(s->label)
      oyjlValueSetString( key, s->label );
    else
    {
      if(strcmp(s->nick, "manufacturer") == 0) oyjlValueSetString( key, _("Manufacturer") );
      else if(strcmp(s->nick, "copyright") == 0) oyjlValueSetString( key, _("Copyright") );
      else if(strcmp(s->nick, "license") == 0) oyjlValueSetString( key, _("License") );
      else if(strcmp(s->nick, "url") == 0) oyjlValueSetString( key, _("Url") );
      else if(strcmp(s->nick, "support") == 0) oyjlValueSetString( key, _("Support") );
      else if(strcmp(s->nick, "download") == 0) oyjlValueSetString( key, _("Download") );
      else if(strcmp(s->nick, "sources") == 0) oyjlValueSetString( key, _("Sources") );
      else if(strcmp(s->nick, "development") == 0) oyjlValueSetString( key, _("Development") );
      else if(strcmp(s->nick, "oyjl_module_author") == 0) oyjlValueSetString( key, _("Oyjl Module Author") );
      else if(strcmp(s->nick, "documentation") == 0) oyjlValueSetString( key, _("Documentation") );
      else if(strcmp(s->nick, "date") == 0) oyjlValueSetString( key, _("Date") );
      else if(strcmp(s->nick, "version") == 0) oyjlValueSetString( key, _("Version") );
      else if(strcmp(s->nick, "permissions") == 0) oyjlValueSetString( key, _("Permissions") );
      else oyjlValueSetString( key, _(s->nick) );
    }
    if(s->name)
    {
      key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/information/[%d]/%s", i, "name" );
      oyjlValueSetString( key, s->name );
    }
    if(s->description)
    {
      key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/information/[%d]/%s", i, "description" );
      oyjlValueSetString( key, s->description );
    }
  }

  i = 0;
  key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/sections/[%d]/%s", 0, "comment" ); \
  oyjlValueSetString( key, "The \"name\" strings are here to initialy feed oyjl-translate tool for generating message catalogs." );
#define ADD_SECTIONS( name_ ) \
  key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/sections/[%d]/%s", i++, "name" ); \
  oyjlValueSetString( key, name_ );
  ADD_SECTIONS( _("Name") )
  ADD_SECTIONS( _("NAME") )
  ADD_SECTIONS( _("Synopsis") )
  ADD_SECTIONS( _("SYNOPSIS") )
  ADD_SECTIONS( _("Description") )
  ADD_SECTIONS( _("DESCRIPTION") )
  ADD_SECTIONS( _("Usage") )
  ADD_SECTIONS( _("OPTIONS") )
  ADD_SECTIONS( _("AUTHOR") )
  ADD_SECTIONS( _("COPYRIGHT") )
  ADD_SECTIONS( _("License") )
  ADD_SECTIONS( _("BUGS") )
  ADD_SECTIONS( _("EXAMPLES") )
  ADD_SECTIONS( _("EXIT-STATE") )
  ADD_SECTIONS( _("ENVIRONMENT VARIABLES") )
  ADD_SECTIONS( _("HISTORY") )
  ADD_SECTIONS( _("FILES") )
  ADD_SECTIONS( _("SEE AS WELL") )
  ADD_SECTIONS( _("SEE ALSO") )

  opts = ui->opts;
  ng = oyjlOptions_CountGroups( opts );
  for(i = 0; i < ng; ++i)
  {
    oyjlOptionGroup_s * g = &opts->groups[i];
    int sub_command;
    oyjlOption_s * o;
    char * synopsis;

    if(!(g->detail && g->detail[0]))
      continue;

    synopsis = oyjlOptions_PrintHelpSynopsis_( opts, g, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MARKDOWN );

    sub_command = (g->flags & OYJL_GROUP_FLAG_SUBCOMMAND) ? OYJL_GROUP_FLAG_SUBCOMMAND : 0;
    if(g->name)
    {
      key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/%s", i, "name" );
      oyjlValueSetString( key, g->name );
    }
    if(g->description)
    {
      key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/%s", i, "description" );
      oyjlValueSetString( key, g->description );
    }
    if(g->help)
    {
      key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/%s", i, "help" );
      oyjlValueSetString( key, g->help );
      if(*oyjl_debug)
        fprintf(stderr, "found help: %s\n", g->help);
    }
    if(synopsis)
    {
      oyjlTreeSetStringF( root, OYJL_CREATE_NEW, synopsis, OYJL_REG "/modules/[0]/groups/[%d]/%s", i, "synopsis" );
      free(synopsis);
      synopsis = NULL;
    }
    if(sub_command)
    {
      key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/%s", i, "sub_command" );
      oyjlValueSetString( key, "1" );
    }
    if(g->flags & OYJL_GROUP_FLAG_EXPLICITE)
    {
      key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/%s", i, "explicite" );
      oyjlValueSetString( key, "1" );
    }
    if(sub_command || g->mandatory)
      key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/%s", i, "mandatory" );
    if(sub_command)
    {
      /* assume the first option is the sub command name */
      o = oyjlOptions_GetOptionL( opts, g->mandatory, 0 );
      oyjlValueSetString( key, o->option );
    }
    else if(g->mandatory)
      oyjlValueSetString( key, g->mandatory );
    if(g->optional)
    {
      key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/%s", i, "optional" );
      oyjlValueSetString( key, g->optional );
    }

    int arn = 0;
    char ** aresults = oyjlOptions_ResultsToList( opts, 0, &arn ); /* all results */
    {
      char * changed = NULL;
      int pos = 0;
      for(j = 0; j < arn; ++j)
      {
        const char * opt = aresults[j];
        int found = oyjlOptions_GroupHasOptionL_( opts, i, opt );
        if( !(strlen(opt) >= 3 && memcmp(opt, "-X=", 3) == 0) &&
            found )
        {
          oyjlStringAdd( &changed, 0,0, "%s%s", pos?",":"", opt );
          ++pos;
        }
      }
      if(changed)
      {
        key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/%s", i, "changed" );
        oyjlValueSetString( key, changed );
      }
      if(changed) free(changed);
    }

    int d = 0;
    char ** d_list = oyjlStringSplit2( g->detail, "|,", 0, &d, NULL, malloc );
    for(j = 0; j < d; ++j)
    {
      char * option = d_list[j];
      oyjlOption_s * o = oyjlOptions_GetOptionL( opts, option, 0 );
      int mandatory_index;
      if(!o) continue;
      mandatory_index = oyjlOptionMandatoryIndex_( o, g );
      num[0] = '\000';
      if(!o->key)
        sprintf(num, "%s", OYJL_E(option,""));
      if(sub_command && mandatory_index == 0)
        sprintf(num, "%s", OYJL_E(o->option,""));
      if(*oyjl_debug)
        fprintf( stderr, "%d %d %s\n", sub_command, mandatory_index, num );
      if(num[0] || o->key)
      {
        key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "key" );
        oyjlValueSetString( key, num[0] ? num : o->key );
      }
      if(o->name)
      {
        key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "name" );
        oyjlValueSetString( key, o->name );
      }
      if(o->description)
      {
        key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "description" );
        oyjlValueSetString( key, o->description );
      }
      if(o->help)
      {
        key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "help" );
        oyjlValueSetString( key, o->help );
      }
      if(o->value_name)
      {
        key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "value_name" );
        oyjlValueSetString( key, o->value_name );
      }
      if(o->flags & OYJL_OPTION_FLAG_IMMEDIATE)
      {
        key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "immediate" );
        oyjlValueSetString( key, "1" );
      }
      if(o->flags & OYJL_OPTION_FLAG_REPETITION)
      {
        key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "repetition" );
        oyjlValueSetString( key, "1" );
      }
      if(o->flags & OYJL_OPTION_FLAG_NO_DASH)
      {
        key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "no_dash" );
        oyjlValueSetString( key, "1" );
      }

      switch(o->value_type)
      {
        case oyjlOPTIONTYPE_CHOICE:
          {
            int l = 0, pos = 0;
            int count = 0;
            char ** results = oyjlOptions_ResultsToList( opts, o->o?o->o:o->option, &count );
            if(o->flags & OYJL_OPTION_FLAG_EDITABLE)
            {
              for( l = 0; l < count; ++l )
              {
                if(!results[l])
                  continue;
                key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "nick" );
                oyjlValueSetString( key, results[l] );
                key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "name" );
                oyjlValueSetString( key, results[l] );
              }
              pos = l;
            }
            char * changed = NULL;
            for(l = 0; l < count; ++l)
            {
              const char * opt = results[l];
              if(strchr(opt, '='))
                opt = strchr(opt, '=') + 1;
              oyjlStringAdd( &changed, 0,0, "%s%s", l?",":"", opt );
            }
            if(changed)
            {
              key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "changed" );
              oyjlValueSetString( key, changed );
            }
            oyjlStringListRelease( &results, count, free );

            if(o->values.choices.list)
            {
              n = 0;
              key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "default" );
              sprintf( num, "%d", o->values.choices.selected );
              if(changed)
                oyjlValueSetString( key, changed );
              else
                oyjlValueSetString( key, num );
              while(o->values.choices.list[n].nick && o->values.choices.list[n].nick[0] != '\000')
                ++n;
              for(l = pos; l < n+pos; ++l)
              {
                if(o->values.choices.list[l-pos].nick)
                {
                  key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "nick" );
                  oyjlValueSetString( key, o->values.choices.list[l-pos].nick );
                }
                if(o->values.choices.list[l-pos].name)
                {
                  key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "name" );
                  oyjlValueSetString( key, o->values.choices.list[l-pos].name );
                }
                if(o->values.choices.list[l-pos].description && o->values.choices.list[l-pos].description[0])
                {
                  key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "description" );
                  oyjlValueSetString( key, o->values.choices.list[l-pos].description );
                }
                if(o->values.choices.list[l-pos].help && o->values.choices.list[l-pos].help[0])
                {
                  key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "help" );
                  oyjlValueSetString( key, o->values.choices.list[l-pos].help );
                }
              }
            } else
              if(!(o->flags & OYJL_OPTION_FLAG_EDITABLE))
              {
                oyjlTreeFree( root ); root = NULL;
                if(t) { free(t); t = NULL; }
                oyjlStringAdd( &t, malloc, free, "Option '%s' has no choices but is not editable (flag&OYJL_OPTION_FLAG_EDITABLE)", o->o?o->o:o->option );
                return root;
              }
            key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "type" );
            if(o->flags & OYJL_OPTION_FLAG_EDITABLE)
              oyjlValueSetString( key, "string" );
            else
              oyjlValueSetString( key, "choice" );
            if(changed) free(changed);
          }
          break;
        case oyjlOPTIONTYPE_FUNCTION:
          {
            int l, selected, pos = 0;
            oyjlOptionChoice_s * list = oyjlOption_GetChoices_(o, &selected, opts );
            n = 0;
            if(list)
              while(list[n].nick && list[n].nick[0] != '\000') ++n;

            int count = 0;
            char ** results = oyjlOptions_ResultsToList( opts, o->o?o->o:o->option, &count );
            if(o->flags & OYJL_OPTION_FLAG_EDITABLE)
            {
              for( l = 0; l < count; ++l )
              {
                key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "nick" );
                oyjlValueSetString( key, results[l] );
                key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "name" );
                oyjlValueSetString( key, results[l] );
              }
              pos = l;
            }
            char * changed = NULL;
            for(l = 0; l < count; ++l)
            {
              const char * opt = results[l];
              if(strchr(opt, '='))
                opt = strchr(opt, '=') + 1;
              oyjlStringAdd( &changed, 0,0, "%s%s", l?",":"", opt );
            }
            if(changed)
            {
              key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "changed" );
              oyjlValueSetString( key, changed );
            }
            oyjlStringListRelease( &results, count, free );

            if((0 <= selected && selected < n && strlen(list[selected].nick)) || changed)
            {
              key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "default" );
              if(changed)
                oyjlValueSetString( key, changed );
              else
                oyjlValueSetString( key, list[selected].nick );
            }
            if(changed) free(changed);
            for(l = pos; l < n+pos; ++l)
            {
              const char * value;
              if(list[l-pos].nick)
              {
                value = list[l-pos].nick;
                key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "nick" );
                oyjlValueSetString( key, value );
              }
              if(list[l-pos].name)
              {
                value = list[l-pos].name;
                key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "name" );
                oyjlValueSetString( key, value );
              }
            }
            /* not possible, as the result of oyjlOption_GetChoices_() is cached - oyjlOptionChoice_Release( &list ); */
          }
          key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "type" );
          if(o->flags & OYJL_OPTION_FLAG_EDITABLE)
            oyjlValueSetString( key, "string" );
          else
            oyjlValueSetString( key, "choice" );
          break;
        case oyjlOPTIONTYPE_DOUBLE:
          {
              int count = 0;
              char ** results = oyjlOptions_ResultsToList( opts, o->o?o->o:o->option, &count );
              if( count )
              {
                sprintf( num, "%s", results[0] );
                key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "changed" );
                oyjlValueSetString( key, num );
              } else
                sprintf( num, "%g", o->values.dbl.d );
              key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "default" );
              oyjlValueSetString( key, num );
              oyjlStringListRelease( &results, count, free );
          }
          oyjlTreeSetDoubleF( root, OYJL_CREATE_NEW, o->values.dbl.start, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "start" );
          oyjlTreeSetDoubleF( root, OYJL_CREATE_NEW, o->values.dbl.end, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "end" );
          oyjlTreeSetDoubleF( root, OYJL_CREATE_NEW, o->values.dbl.tick, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "tick" );
          key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "type" );
          oyjlValueSetString( key, "double" );
          break;
        case oyjlOPTIONTYPE_NONE:
          {
            int count = 0;
            char ** results = oyjlOptions_ResultsToList( opts, o->o?o->o:o->option, &count );
            if( count )
            {
              sprintf( num, "%s", results[0] );
              key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "changed" );
              oyjlValueSetString( key, num );
            } else
              sprintf( num, "0" );
            key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "default" );
            oyjlValueSetString( key, num );
            oyjlStringListRelease( &results, count, free );
          }
          key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "type" );
          oyjlValueSetString( key, "bool" );
          {
            int l; char t[12];
            for(l = 0; l < 2; ++l)
            {
              sprintf(t, "%d", l);
              key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "nick" );
              oyjlValueSetString( key, t );
              key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "name" );
              oyjlValueSetString( key, l?_("Yes"):_("No") );
            }
          }
          break;
        case oyjlOPTIONTYPE_START: break;
        case oyjlOPTIONTYPE_END: break;
      }

      if(o->properties && o->properties[0])
      {
        n = 0;
        char ** list = oyjlStringSplit( o->properties, '\n', &n, malloc );
        if(!list)
          oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "found issue with properties: %s", OYJL_DBG_ARGS,
                         o->properties );
        else
        {
          char * key, * value, * t;
          int k;
          for(k = 0; k < n; ++k)
          {
            key = list[k];
            t = strchr(key, '=');
            if(!t)
              oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "found issue with properties: %s", OYJL_DBG_ARGS,
                             key );
            else
            {
              value = strchr(key, '=') + 1;
              t[0] = '\000';
              oyjlTreeSetStringF( root, OYJL_CREATE_NEW, value, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/properties/%s", i,j, key );
            }
          }
          oyjlStringListRelease( &list, n, free );
        }
      }
    }
    oyjlStringListRelease( &aresults, arn, free );
    oyjlStringListRelease( &d_list, d, free );

    if(g->properties && g->properties[0])
    {
      n = 0;
      char ** list = oyjlStringSplit( g->properties, '\n', &n, malloc );
      if(!list)
        oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "found issue with properties: %s", OYJL_DBG_ARGS,
                       g->properties );
      else
      {
        char * key, * value, * t;
        int k;
        for(k = 0; k < n; ++k)
        {
          key = list[k];
          t = strchr(key, '=');
          if(!t)
            oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "found issue with properties: %s", OYJL_DBG_ARGS,
                           key );
          else
          {
            value = strchr(key, '=') + 1;
            t[0] = '\000';
            oyjlTreeSetStringF( root, OYJL_CREATE_NEW, value, OYJL_REG "/modules/[0]/groups/[%d]/properties/%s", i, key );
          }
        }
        oyjlStringListRelease( &list, n, free );
      }
    }
  }

  j = 0;
  nopts = oyjlOptions_Count( opts );
  for(k = 0; k < nopts; ++k)
  {
    oyjlOption_s * o = &opts->array[k];
    if(o->option && strstr(o->option,"man-"))
    {
      key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "key" );
      oyjlValueSetString( key, o->o );
      key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "option" );
      oyjlValueSetString( key, o->option );
      if(o->name && o->name[0])
      {
        key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "name" );
        oyjlValueSetString( key, o->name );
      }

      if(o->value_type == oyjlOPTIONTYPE_CHOICE)
      {
            int l = 0, pos = 0;
            if(o->values.choices.list)
            {
              n = 0;
              while(o->values.choices.list[n].nick && o->values.choices.list[n].nick[0] != '\000')
                ++n;
              for(l = pos; l < n+pos; ++l)
              {
                if(o->values.choices.list[l-pos].nick)
                {
                  key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "label" );
                  oyjlValueSetString( key, o->values.choices.list[l-pos].nick );
                }
                if(o->values.choices.list[l-pos].name)
                {
                  key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "name" );
                  oyjlValueSetString( key, o->values.choices.list[l-pos].name );
                }
                if(o->values.choices.list[l-pos].description && o->values.choices.list[l-pos].description[0])
                {
                  key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "description" );
                  oyjlValueSetString( key, o->values.choices.list[l-pos].description );
                }
                if(o->values.choices.list[l-pos].help && o->values.choices.list[l-pos].help[0])
                {
                  key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "help" );
                  oyjlValueSetString( key, o->values.choices.list[l-pos].help );
                }
              }
            }
      }
      ++j;
    }
  }

  return root;
}

/** @brief    Return a JSON representation from options
 *  @memberof oyjlUi_s
 *
 *  The JSON data shall be useable with oyjl-args-qml options renderer and
 *  the -R option.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2023/05/22
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
char *       oyjlUi_ToJson           ( oyjlUi_s          * ui,
                                       int                 flags )
{
  oyjl_val root = oyjlUi_ToJson_( ui );
  char * t = oyjlTreeToText( root, flags );
  oyjlTreeFree( root );

  return t;
}

/** @brief    Translate a Ui at any time
 *  @memberof oyjlUi_s
 *
 *  Typically translation occur at initialisation time.
 *  However, it might be desireable to update the Ui due
 *  to later language change. This function helps to replace
 *  the previous language with loc set to "back" in the context.
 *
 *  @see oyjlTr_New()
 *
 *  @param[in,out] ui                 the structure to translate strings inside
 *  @param[in]     context            the translation context; optional, without will use gettext
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/10/24
 *  @since   2020/07/30 (Oyjl: 1.0.0)
 */
void               oyjlUi_Translate  ( oyjlUi_s          * ui,
                                       oyjlTr_s          * context )
{
  int i,j,n,ng;
  oyjlTranslate_f translator = NULL;

  translator = oyjlTr_GetTranslator( context );

  if(!ui) return;

  if(!translator)
    translator = oyjlTranslate;

#define tr( text ) text = translator(context, text)
  tr(ui->name);
  tr(ui->description);
  n = oyjlUiHeaderSection_Count( ui->sections );
  for(i = 0; i < n; ++i)
  {
    oyjlUiHeaderSection_s * s = &ui->sections[i];
    if(s->label)
      tr(ui->sections[i].label);
    if(s->name)
      tr(ui->sections[i].name);
    if(s->description)
      tr(ui->sections[i].description);
  }

  int nopts = oyjlOptions_Count( ui->opts );
  for(i = 0; i < nopts; ++i)
  {
    oyjlOption_s * o = &ui->opts->array[i];
    tr(ui->opts->array[i].name);
    tr(ui->opts->array[i].description);
    tr(ui->opts->array[i].help);
    tr(ui->opts->array[i].value_name);
    switch(o->value_type)
    {
      case oyjlOPTIONTYPE_CHOICE:
        n = oyjlOptionChoice_Count( o->values.choices.list );
        if(n)
          for(j = 0; j < n; ++j)
          {
            tr(ui->opts->array[i].values.choices.list[j].name);
            tr(ui->opts->array[i].values.choices.list[j].description);
            tr(ui->opts->array[i].values.choices.list[j].help);
          }
        break;
      case oyjlOPTIONTYPE_FUNCTION:
      case oyjlOPTIONTYPE_DOUBLE:
      case oyjlOPTIONTYPE_NONE:
      case oyjlOPTIONTYPE_START:
      case oyjlOPTIONTYPE_END:
        break;
    }
  }

  ng = oyjlOptions_CountGroups( ui->opts );
  for(i = 0; i < ng; ++i)
  {
    tr(ui->opts->groups[i].name);
    tr(ui->opts->groups[i].description);
    tr(ui->opts->groups[i].help);
  }
#undef tr
}


/* private stuff */

oyjlUiHeaderSection_s * oyjlUiInfo_  ( const char          * documentation,
                                       const char          * date_name,
                                       const char          * date_description )
{
  oyjlUiHeaderSection_s s[] = {
    /* type,  nick,      label,name,                 description */
    { "oihs", "version", NULL, OYJL_VERSION_NAME, oyjlVersionName(1) },
    { "oihs", "manufacturer", NULL, "Kai-Uwe Behrmann", "http://www.oyranos.org" },
    { "oihs", "copyright", NULL, "Copyright © 2017-2022 Kai-Uwe Behrmann", NULL },
    { "oihs", "license", NULL, "newBSD", "http://www.oyranos.org" },
    { "oihs", "url", NULL, "http://www.oyranos.org", NULL },
    { "oihs", "support", NULL, "https://www.gitlab.com/oyranos/oyranos/issues", NULL },
    { "oihs", "download", NULL, "https://gitlab.com/oyranos/oyranos/-/releases", NULL },
    { "oihs", "sources", NULL, "https://gitlab.com/oyranos/oyranos", NULL },
    { "oihs", "development", NULL, "https://gitlab.com/oyranos/oyranos", NULL },
    { "oihs", "oyjl_module_author", NULL, "Kai-Uwe Behrmann", "http://www.behrmann.name" },
    { "oihs", "documentation", NULL, "http://www.oyranos.org", documentation },
    { "oihs", "date", NULL, date_name, date_description },
    { "", NULL, NULL, NULL, NULL }
  };
  return (oyjlUiHeaderSection_s*) oyjlStringAppendN( NULL, (const char*)s, sizeof(s), malloc );
}

