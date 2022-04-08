/** @file oyjl_args.c
 *
 *  oyjl - UI helpers
 *
 *  @par Copyright:
 *            2018-2021 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl argument handling with libOyjlCore
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *
 *  Copyright (c) 2018-2021  Kai-Uwe Behrmann  <ku.b@gmx.de>
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
#include "oyjl_i18n.h"
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
 *  @date    2019/12/01
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
char * oyjlOptions_ResultsToJson  ( oyjlOptions_s  * opts )
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

  i = 0;
  oyjlTreeToJson( root, &i, &rjson );
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

#define OYJL_REG "org/freedesktop/oyjl"
#define OYJL_E(x_) (x_?x_:"")
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
            oyjlStr_AppendN( tmp, o->option, strlen(o->option) );
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
 *  @date    2020/10/12
 *  @since   2019/06/16 (Oyjl: 1.0.0)
 */
char *       oyjlUi_ExportToJson     ( oyjlUi_s          * ui,
                                       int                 flags OYJL_UNUSED )
{
  char * t = NULL;
  int i = 0;
  oyjl_val root = oyjlUi_ExportToJson_( ui, flags );
  oyjlTreeToJson( root, &i, &t );
  oyjlTreeFree( root );

  return t;
}

/** @brief    Return a oyjlUi_s from JSON Import
 *
 *  The returned oyjlUi_s is a reverse of oyjlUi_s().
 *  Static strings inside oyjlUi_s depend on root structure.
 *  The required JSON data need to match the -X export option as
 *  returned by the oyjlUi_ExportToJson() function.
 *
 *  @see oyjlUi_ExportToJson()
 *
 *  @version Oyjl: 1.0.0
 *  @date    2020/10/04
 *  @since   2020/10/04 (Oyjl: 1.0.0)
 */
oyjlUi_s *     oyjlUi_ImportFromJson ( oyjl_val            root,
                                       int                 flags OYJL_UNUSED )
{
  oyjlUi_s * ui = NULL;
  oyjlAllocHelper_m( ui, oyjlUi_s, 1, malloc, return NULL );

  *(oyjlOBJECT_e*)&ui->type /*"oyui"*/ = oyjlOBJECT_UI;

  {
    oyjl_val val, v;
    char * app_type;
    int i,n;
    oyjlOptsPrivate_s * results;

    val = oyjlTreeGetValue( root, 0, OYJL_REG "/ui/app_type" ); app_type = OYJL_GET_STRING(val);
    ui->app_type = app_type;

    n = oyjlValueCount( oyjlTreeGetValue( root, 0, "org/freedesktop/oyjl/ui/header/sections" ) );
    oyjlAllocHelper_m( ui->sections, oyjlUiHeaderSection_s, n+1, malloc, return NULL );
    for(i = 0; i < n; ++i)
    {
      const char *nick, *label, *name, *desc;
      *(oyjlOBJECT_e*)&ui->sections[i] /*"oihs"*/ = oyjlOBJECT_UI_HEADER_SECTION;
      val = oyjlTreeGetValueF( root, 0, "org/freedesktop/oyjl/ui/header/sections/[%d]", i );
      v = oyjlTreeGetValue( val, 0, "nick" ); nick = OYJL_GET_STRING(v);
      ui->sections[i].nick = nick;
      v = oyjlTreeGetValue( val, 0, "label" ); label = OYJL_GET_STRING(v);
      ui->sections[i].label = label;
      v = oyjlTreeGetValue( val, 0, "name" ); name = OYJL_GET_STRING(v);
      ui->sections[i].name = name;
      v = oyjlTreeGetValue( val, 0, "description" ); desc = OYJL_GET_STRING(v);
      ui->sections[i].description = desc;
    }
    n = oyjlValueCount( oyjlTreeGetValue( root, 0, "org/freedesktop/oyjl/ui/options/array" ) );
    oyjlAllocHelper_m( ui->opts, oyjlOptions_s, 1, malloc, return NULL );
    *(oyjlOBJECT_e*)&ui->opts[0] /*"oiws"*/ = oyjlOBJECT_OPTIONS;
    oyjlAllocHelper_m( ui->opts->private_data, oyjlOptsPrivate_s, 1, malloc, return NULL );
    results = ui->opts->private_data;
    results->memory_allocation = oyjlMEMORY_ALLOCATION_OPTIONS | oyjlMEMORY_ALLOCATION_ARRAY | oyjlMEMORY_ALLOCATION_SECTIONS;
    oyjlAllocHelper_m( ui->opts->array, oyjlOption_s, n+1, malloc, return NULL );
    for(i = 0; i < n; ++i)
    {
      const char *value_type_string, *o, *option;
      const char *key, *name, *desc, *help, *value_name, *variable_type;
      int flg;
      oyjlVARIABLE_e var_type = oyjlNONE;
      oyjlOPTIONTYPE_e value_type = oyjlOPTIONTYPE_START;
      oyjlOption_s * opt = &ui->opts->array[i];
      *(oyjlOBJECT_e*)&opt[0] /*"oiwi"*/ = oyjlOBJECT_OPTION;
      val = oyjlTreeGetValueF( root, 0, "org/freedesktop/oyjl/ui/options/array/[%d]", i );
      v = oyjlTreeGetValue( val, 0, "o" ); o = OYJL_GET_STRING(v);
      opt->o = o;
      v = oyjlTreeGetValue( val, 0, "option" ); option = OYJL_GET_STRING(v);
      opt->option = option;
      v = oyjlTreeGetValue( val, 0, "value_type" ); value_type_string = OYJL_GET_STRING(v);
      if(value_type_string && strcmp(value_type_string, "oyjlOPTIONTYPE_CHOICE") == 0)
        value_type = oyjlOPTIONTYPE_CHOICE;
      else
      if(value_type_string && strcmp(value_type_string, "oyjlOPTIONTYPE_FUNCTION") == 0)
        value_type = oyjlOPTIONTYPE_FUNCTION;
      else
      if(value_type_string && strcmp(value_type_string, "oyjlOPTIONTYPE_DOUBLE") == 0)
        value_type = oyjlOPTIONTYPE_DOUBLE;
      else
      if(value_type_string && strcmp(value_type_string, "oyjlOPTIONTYPE_NONE") == 0)
        value_type = oyjlOPTIONTYPE_NONE;
      opt->value_type = value_type;
      switch(value_type)
      {
        case oyjlOPTIONTYPE_CHOICE:
        {
          char *nick, *name, *desc, *help;
          int count = oyjlValueCount( oyjlTreeGetValue( val, 0, "values/choices/list" ) ), j;
          oyjlAllocHelper_m( opt->values.choices.list, oyjlOptionChoice_s, count+1, malloc, return NULL );
          for(j = 0; j < count; ++j)
          {
            oyjlOptionChoice_s * choice = &opt->values.choices.list[j];
            oyjl_val c = oyjlTreeGetValueF( val, 0, "values/choices/list/[%d]", j );
            v = oyjlTreeGetValue( c, 0, "nick" ); nick = OYJL_GET_STRING(v);
            choice->nick = nick;
            v = oyjlTreeGetValue( c, 0, "name" ); name = OYJL_GET_STRING(v);
            choice->name = name;
            v = oyjlTreeGetValue( c, 0, "description" ); desc = OYJL_GET_STRING(v);
            choice->description = desc;
            v = oyjlTreeGetValue( c, 0, "help" ); help = OYJL_GET_STRING(v);
            choice->help = help;
          }
        }
        break;
        case oyjlOPTIONTYPE_DOUBLE:
        {
          double d, start, tick, end;
          v = oyjlTreeGetValue( val, 0, "values/dbl/d" ); d = OYJL_IS_DOUBLE(v) ? OYJL_GET_DOUBLE(v) : -1.0;
          opt->values.dbl.d = d;
          v = oyjlTreeGetValue( val, 0, "values/dbl/start" ); start = OYJL_IS_DOUBLE(v) ? OYJL_GET_DOUBLE(v) : -1.0;
          opt->values.dbl.start = start;
          v = oyjlTreeGetValue( val, 0, "values/dbl/tick" ); tick = OYJL_IS_DOUBLE(v) ? OYJL_GET_DOUBLE(v) : -1.0;
          opt->values.dbl.tick = tick;
          v = oyjlTreeGetValue( val, 0, "values/dbl/end" ); end = OYJL_IS_DOUBLE(v) ? OYJL_GET_DOUBLE(v) : -1.0;
          opt->values.dbl.end = end;
        }
        break;
        default:
        break;
      }
      v = oyjlTreeGetValue( val, 0, "flags" ); flg = OYJL_IS_INTEGER(v) ? OYJL_GET_INTEGER(v) : 0;
      opt->flags = flg;
      v = oyjlTreeGetValue( val, 0, "name" ); name = OYJL_GET_STRING(v);
      opt->name = name;
      v = oyjlTreeGetValue( val, 0, "key" ); key = OYJL_GET_STRING(v);
      opt->key = key;
      v = oyjlTreeGetValue( val, 0, "description" ); desc = OYJL_GET_STRING(v);
      opt->description = desc;
      v = oyjlTreeGetValue( val, 0, "help" ); help = OYJL_GET_STRING(v);
      opt->help = help;
      v = oyjlTreeGetValue( val, 0, "value_name" ); value_name = OYJL_GET_STRING(v);
      opt->value_name = value_name;
      v = oyjlTreeGetValue( val, 0, "variable_type" ); variable_type = OYJL_GET_STRING(v);
      if(variable_type && strcmp(variable_type, "oyjlSTRING") == 0)
        var_type = oyjlSTRING;
      else
      if(variable_type && strcmp(variable_type, "oyjlDOUBLE") == 0)
        var_type = oyjlDOUBLE;
      else
      if(variable_type && strcmp(variable_type, "oyjlINT") == 0)
        var_type = oyjlINT;
      opt->variable_type = var_type;
    }
    n = oyjlValueCount( oyjlTreeGetValue( root, 0, "org/freedesktop/oyjl/ui/options/groups" ) );
    results->memory_allocation |= oyjlMEMORY_ALLOCATION_GROUPS;
    oyjlAllocHelper_m( ui->opts->groups, oyjlOptionGroup_s, n+1, malloc, return NULL );
    for(i = 0; i < n; ++i)
    {
      const char *name, *desc, *help, *mandatory, *optional, *detail;
      int flg;
      oyjlOptionGroup_s * g = &ui->opts->groups[i];
      *(oyjlOBJECT_e*)g /*"oiwg"*/ = oyjlOBJECT_OPTION_GROUP;
      val = oyjlTreeGetValueF( root, 0, "org/freedesktop/oyjl/ui/options/groups/[%d]", i );
      v = oyjlTreeGetValue( val, 0, "name" ); name = OYJL_GET_STRING(v);
      g->name = name;
      v = oyjlTreeGetValue( val, 0, "description" ); desc = OYJL_GET_STRING(v);
      g->description = desc;
      v = oyjlTreeGetValue( val, 0, "help" ); help = OYJL_GET_STRING(v);
      g->help = help;
      v = oyjlTreeGetValue( val, 0, "mandatory" ); mandatory = OYJL_GET_STRING(v);
      g->mandatory = mandatory;
      v = oyjlTreeGetValue( val, 0, "optional" ); optional = OYJL_GET_STRING(v);
      g->optional = optional;
      v = oyjlTreeGetValue( val, 0, "detail" ); detail = OYJL_GET_STRING(v);
      g->detail = detail;
      v = oyjlTreeGetValue( val, 0, "flags" ); flg = OYJL_IS_INTEGER(v) ? OYJL_GET_INTEGER(v) : 0;
      g->flags = flg;
    }
    const char *nick, *name, *description, *logo;
    val = oyjlTreeGetValue( root, 0, OYJL_REG "/ui/nick" ); nick = OYJL_GET_STRING(val);
    ui->nick = nick;
    val = oyjlTreeGetValue( root, 0, OYJL_REG "/ui/name" ); name = OYJL_GET_STRING(val);
    ui->name = name;
    val = oyjlTreeGetValue( root, 0, OYJL_REG "/ui/description" ); description = OYJL_GET_STRING(val);
    ui->description = description;
    val = oyjlTreeGetValue( root, 0, OYJL_REG "/ui/logo" ); logo = OYJL_GET_STRING(val);
    ui->logo = logo;
    val = oyjlTreeGetValue( root, 0, OYJL_REG "/ui/attr" );
    if(val)
    {
      oyjl_val attr = oyjlTreeNew( "" );
      char ** attr_paths = oyjlTreeToPaths( val, 0, NULL, 0, &n );
      for(i = 0; i < n; ++i)
      {
        const char * path = attr_paths[i];
        oyjl_type type;
        oyjl_val attrv, rootv;
        rootv = oyjlTreeGetValue( val, 0, path );
        type = rootv->type;
        attrv = oyjlTreeGetValue( attr, OYJL_CREATE_NEW, path );
        switch(type)
        {
          case oyjl_t_string: /* 1 - a text in UTF-8 */
              oyjlValueSetString( attrv, rootv->u.string );
              break;
          case oyjl_t_number: /* 2 - floating or integer number */
              if(OYJL_IS_INTEGER(rootv))
                oyjlValueSetInt( attrv, rootv->u.number.i );
              else
                oyjlValueSetDouble( attrv, rootv->u.number.d );
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
      oyjlOptions_SetAttributes( ui->opts, &attr );
    }
  }

  return ui;
}

#define OYJL_TRANSLATE 0x01
#define OYJL_SQUOTE    0x04
#define OYJL_LAST      0x08
static void oyjlStr_AddSpaced_( oyjl_str s, const char * text, int flags, int space )
{
  int len = 0, i,n;

  if(text) len = strlen(text);
  if(text && (flags & OYJL_TRANSLATE)) len += 2+1;
  if(text && (flags & OYJL_QUOTE))     len += 1+1;
  if(text && (flags & OYJL_SQUOTE))    len += 1+1;

  if(len)
  {
    if(flags & OYJL_TRANSLATE && text && text[0])
      oyjlStr_Add( s, "_(" );
    if(flags & OYJL_QUOTE)
      oyjlStr_Add( s, "\"" );
    if(flags & OYJL_SQUOTE)
      oyjlStr_Add( s, "'" );
    if(flags & OYJL_QUOTE)
    {
      const char * t = text;
      oyjl_str tmp = oyjlStr_New(10,0,0);
      oyjlStr_AppendN( tmp, t, strlen(t) );
      oyjlStr_Replace( tmp, "\\", "\\\\", 0, NULL );
      oyjlStr_Replace( tmp, "\"", "\\\"", 0, NULL );
      oyjlStr_Replace( tmp, "\b", "\\b", 0, NULL );
      oyjlStr_Replace( tmp, "\f", "\\f", 0, NULL );
      oyjlStr_Replace( tmp, "\n", "\\n", 0, NULL );
      oyjlStr_Replace( tmp, "\r", "\\r", 0, NULL );
      oyjlStr_Replace( tmp, "\t", "\\t", 0, NULL );
      t = oyjlStr(tmp); 
      oyjlStr_Add( s, "%s", t );
      oyjlStr_Release( &tmp );
    }
    else
      oyjlStr_Add( s, "%s", (text && strlen(text) == 0 && flags & OYJL_SQUOTE) ? "\\000" : text );
    if(flags & OYJL_SQUOTE)
      oyjlStr_Add( s, "'" );
    if(flags & OYJL_QUOTE)
      oyjlStr_Add( s, "\"" );
    if(flags & OYJL_TRANSLATE && text && text[0])
      oyjlStr_Add( s, ")" );
  } else
  {
    len = 4;
      oyjlStr_Add( s, "NULL" );
  }

  if(!(flags & OYJL_LAST))
  {
      ++len;
      oyjlStr_Add( s, "," );
  }

  n = space - len;
  if(n > 0)
    for(i = 0; i < n; ++i)
      oyjlStr_Add( s, " " );
}

/* heuristic for variable name */
static char * oyjlGuessSingleWordFromChar_ ( char o, const char * text )
{ /* guess from name */
  int words_n = 0, i, len, found = 0;
  char * name = NULL;
  char ** words = oyjlStringSplit( text, 0, &words_n, malloc );
  /* search first letter of word for a match */
  for(i = 0; i < words_n; ++i)
  {
    const char * word = words[i];
    /* avoid if or at */
    if(strlen(word) <= 2) word = NULL;
    /* avoid all upper case words, as they might come from value_name, which is mostly a generic name */
    if(word && islower(word[1]) && tolower(word[0]) == o)
    {
      name = oyjlStringCopy(word,0);
      break;
    }
  }
  if(!name)
  /* search inside words for a match */
  for(i = 0; i < words_n; ++i)
  {
    const char * word = words[i];
    if(strlen(word) <= 2) word = NULL;
    if(word)
    {
      int j;
      len = strlen(word);
      for(j = 0; j < len; ++j)
        if(tolower(word[j]) == o)
        {
          name = oyjlStringCopy(word,0);
          found = 1;
          break;
        }
      if(found)
        break;
    }
  }
  /* all to lowercase */
  if(name)
  {
    len = strlen(name);
    for(i= 0; i < len; ++i)
      name[i] = tolower(name[i]);
    if(ispunct(name[len-1]))
      name[len-1] = '\000';
  }
  oyjlStringListRelease( &words, words_n, free );
  return name;
}

void oyjlUiCanonicaliseVariableName_ ( char             ** name )
{
  const char * txt;
  int i;
  oyjl_str tmp;
  if(!name || !*name)
    return;
  tmp = oyjlStr_New(10,0,0);
  oyjlStr_AppendN( tmp, *name, strlen(*name) );
  oyjlStr_Replace( tmp, "-", "_", 0, NULL );
  oyjlStr_Replace( tmp, "+", "_plus_", 0, NULL );
  oyjlStr_Replace( tmp, "=", "_", 0, NULL );
  oyjlStr_Replace( tmp, "(", "_", 0, NULL );
  oyjlStr_Replace( tmp, "|", "_", 0, NULL );
  oyjlStr_Replace( tmp, ".", "_", 0, NULL );
  oyjlStr_Replace( tmp, "?", "_", 0, NULL );
  txt = oyjlStr(tmp);
  free(*name); *name = NULL;
  i = 0;
  while(txt[i] && txt[i] == '_') ++i;
  oyjlStringAdd( name, 0,0, "%s", &txt[i] );
  oyjlStr_Release( &tmp );
}

#define OYJL_CLEAN_STRING(name_) if(name_ && !name_[0]) name_ = NULL;
/* obtain a variable name for C inclusion + the according type. e.g.: int var_name */
static char *  oyjlUiGetVariableName_( oyjl_val            val,
                                       const char       ** type )
{
  oyjl_val v;
  char * t = NULL;
  const char *o, *option, *name, *description, *variable_type, *variable_name;
  v = oyjlTreeGetValue( val, 0, "o" ); o = OYJL_GET_STRING(v); OYJL_CLEAN_STRING(o);
  v = oyjlTreeGetValue( val, 0, "option" ); option = OYJL_GET_STRING(v); OYJL_CLEAN_STRING(option);
  v = oyjlTreeGetValue( val, 0, "name" ); name = OYJL_GET_STRING(v); OYJL_CLEAN_STRING(name);
  v = oyjlTreeGetValue( val, 0, "description" ); description = OYJL_GET_STRING(v); OYJL_CLEAN_STRING(description);
  v = oyjlTreeGetValue( val, 0, "variable_type" ); variable_type = OYJL_GET_STRING(v); OYJL_CLEAN_STRING(variable_type);
  v = oyjlTreeGetValue( val, 0, "variable_name" ); variable_name = OYJL_GET_STRING(v); OYJL_CLEAN_STRING(variable_name);
  if(!(variable_type && strcmp(variable_type, "oyjlNONE") == 0))
  {
    if(type)
    {
      if(variable_type && strcmp(variable_type, "oyjlSTRING") == 0)
        *type = "const char *";
      else
      if(variable_type && strcmp(variable_type, "oyjlDOUBLE") == 0)
        *type = "double";
      else
      if(variable_type && strcmp(variable_type, "oyjlINT") == 0)
        *type = "int";
    }

    if(!type || *type)
    {
      if(variable_name)
        oyjlStringAdd( &t, 0,0, "%s", variable_name );
      else if(option)
      {
        t = oyjlStringCopy(option, 0);
        oyjlUiCanonicaliseVariableName_( &t );
      }
      else if(o)
      {
        t = oyjlGuessSingleWordFromChar_( o[0], name );
        oyjlUiCanonicaliseVariableName_( &t );
        if(!t)
        {
          t = oyjlGuessSingleWordFromChar_( o[0], description );
          oyjlUiCanonicaliseVariableName_( &t );
        }
        if(!t)
        {
          if(o[0] == '@')
            t = oyjlStringCopy( "count", 0 );
          else
            oyjlStringAdd( &t, 0,0, "%c", o[0] );
        }
      }
    }
  }
  return t;
}
static char * oyjlUiGetVariableNameC_( oyjl_val            val,
                                       const char       ** type )
{
  char * t = oyjlUiGetVariableName_( val, type );
  if(!t) return t;
  /* replace some reserved C names */
  if('0' <= t[0] && t[0] <= '9') { char * name = NULL; oyjlStringAdd( &name, 0,0, "var_%s", t ); free(t); t = name; }
  if(strcmp(t,"#") == 0) { free(t); t = oyjlStringCopy("no_arg_var",0); }
  if(strcmp(t,"?") == 0) { free(t); t = oyjlStringCopy("question_var",0); }
  if(strcmp(t,"break") == 0) { free(t); t = oyjlStringCopy("break_var",0); }
  if(strcmp(t,"case") == 0) { free(t); t = oyjlStringCopy("case_var",0); }
  if(strcmp(t,"char") == 0) { free(t); t = oyjlStringCopy("char_var",0); }
  if(strcmp(t,"class") == 0) { free(t); t = oyjlStringCopy("class_var",0); }
  if(strcmp(t,"const") == 0) { free(t); t = oyjlStringCopy("const_var",0); }
  if(strcmp(t,"do") == 0) { free(t); t = oyjlStringCopy("do_var",0); }
  if(strcmp(t,"double") == 0) { free(t); t = oyjlStringCopy("double_var",0); }
  if(strcmp(t,"else") == 0) { free(t); t = oyjlStringCopy("else_var",0); }
  if(strcmp(t,"enum") == 0) { free(t); t = oyjlStringCopy("enum_var",0); }
  if(strcmp(t,"export") == 0) { free(t); t = oyjlStringCopy("export_var",0); } /* C++ */
  if(strcmp(t,"extern") == 0) { free(t); t = oyjlStringCopy("extern_var",0); }
  if(strcmp(t,"float") == 0) { free(t); t = oyjlStringCopy("float_var",0); }
  if(strcmp(t,"for") == 0) { free(t); t = oyjlStringCopy("for_var",0); }
  if(strcmp(t,"goto") == 0) { free(t); t = oyjlStringCopy("goto_var",0); }
  if(strcmp(t,"if") == 0) { free(t); t = oyjlStringCopy("if_var",0); }
  if(strcmp(t,"int") == 0) { free(t); t = oyjlStringCopy("int_var",0); }
  if(strcmp(t,"long") == 0) { free(t); t = oyjlStringCopy("long_var",0); }
  if(strcmp(t,"register") == 0) { free(t); t = oyjlStringCopy("register_var",0); }
  if(strcmp(t,"short") == 0) { free(t); t = oyjlStringCopy("short_var",0); }
  if(strcmp(t,"signed") == 0) { free(t); t = oyjlStringCopy("signed_var",0); }
  if(strcmp(t,"static") == 0) { free(t); t = oyjlStringCopy("static_var",0); }
  if(strcmp(t,"struct") == 0) { free(t); t = oyjlStringCopy("struct_var",0); }
  if(strcmp(t,"switch") == 0) { free(t); t = oyjlStringCopy("switch_var",0); }
  if(strcmp(t,"system") == 0) { free(t); t = oyjlStringCopy("system_var",0); }
  if(strcmp(t,"typedef") == 0) { free(t); t = oyjlStringCopy("typedef_var",0); }
  if(strcmp(t,"unsigned") == 0) { free(t); t = oyjlStringCopy("unsigned_var",0); }
  if(strcmp(t,"void") == 0) { free(t); t = oyjlStringCopy("void_var",0); }
  if(strcmp(t,"while") == 0) { free(t); t = oyjlStringCopy("while_var",0); }
  return t;
}

static oyjl_val oyjlFindOption_( oyjl_val root, char o )
{
  char oo[4] = {o,0,0,0};
  int found = 0, k;
  oyjl_val val = oyjlTreeGetValue( root, 0, OYJL_REG "/ui/options/array" );
  int count = oyjlValueCount( val );
  for(k = 0; k < count; ++k)
  {
    oyjl_val v = oyjlTreeGetValueF( val, 0, "[%d]/o", k );
    const char * text = OYJL_GET_STRING(v);
    if(text && strcmp(text, oo) == 0)
    {
      found = 1;
      val = oyjlTreeGetValueF( val, 0, "[%d]", k );
      break;
    }
  }
  if(!found) val = NULL;
  return val;
}

/** @brief    Return a source code from a parsed source tree
 *  @memberof oyjlUi_s
 *
 *  The input format is the JSON data from oyjlUi_ExportToJson().
 *
 *  @param[in]     root                the parsed JSON tree to convert
 *  @param[in]     flags               support;
 *                                     - ::OYJL_SOURCE_CODE_C
 *                                       - ::OYJL_WITH_OYJL_ARGS_C
 *                                       - ::OYJL_SUGGEST_VARIABLE_NAMES
 *                                       - ::OYJL_NO_DEFAULT_OPTIONS
 *                                     - ::OYJL_COMPLETION_BASH
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/10/31
 *  @since   2019/06/24 (Oyjl: 1.0.0)
 */
char *             oyjlUiJsonToCode  ( oyjl_val            root,
                                       int                 flags )
{
  char * c = NULL;
  oyjl_str s = oyjlStr_New( 0, 0,0 );
  if(flags & OYJL_SOURCE_CODE_C)
  {
    oyjl_val val, v;
    char * app_type;
    int i,n, X_found = 0, export_found = 0, h_found = 0, help_found = 0, v_found = 0, v_is_string = 0, verbose_found = 0, version_found = 0, render_found = 0;

    val = oyjlTreeGetValue( root, 0, OYJL_REG "/ui/app_type" ); app_type = OYJL_GET_STRING(val);
    if(flags & OYJL_WITH_OYJL_ARGS_C)
    {
      oyjlStr_Add( s, "/* Compile:\n" );
      oyjlStr_Add( s, " *   cc -Wall -g my-OyjlArgs-enabled-tool.c -DOYJL_HAVE_LOCALE_H -DOYJL_HAVE_LANGINFO_H -DOYJL_HAVE_LIBINTL_H -DOYJL_LOCALEDIR=\"\\\"/usr/local/share/locale\\\"\"  -I oyjl/\n" );
      oyjlStr_Add( s, " */\n" );
      oyjlStr_Add( s, "#include \"oyjl_args.c\"\n" );
    }
    else
    {
      oyjlStr_Add( s, "#include \"oyjl.h\"\n" );
      oyjlStr_Add( s, "#include \"oyjl_version.h\"\n" );
    }
    oyjlStr_Add( s, "extern char **environ;\n" );
    if(flags & OYJL_WITH_OYJL_ARGS_C)
      oyjlStr_Add( s, "#define NO_OYJL_ARGS_RENDER\n" );
    oyjlStr_Add( s, "#ifdef OYJL_HAVE_LOCALE_H\n" );
    oyjlStr_Add( s, "# include <locale.h>\n" );
    oyjlStr_Add( s, "#endif\n" );
    oyjlStr_Add( s, "#define MY_DOMAIN \"oyjl\"\n" );
    oyjlStr_Add( s, "oyjlTr_s * trc = NULL;\n" );
    oyjlStr_Add( s, "# ifdef _\n" );
    oyjlStr_Add( s, "# undef _\n" );
    oyjlStr_Add( s, "# endif\n" );
    if(flags & OYJL_WITH_OYJL_ARGS_C)
    {
      oyjlStr_Add( s, "#ifdef OYJL_HAVE_LIBINTL_H\n" );
      oyjlStr_Add( s, "# define _(text) dgettext( MY_DOMAIN, text )\n" );
      oyjlStr_Add( s, "#else\n" );
      oyjlStr_Add( s, "# ifdef OYJL_H\n" );
      oyjlStr_Add( s, "#  define _(text) oyjlTranslate( trc, text )\n" );
      oyjlStr_Add( s, "# else\n" );
      oyjlStr_Add( s, "#  define _(text) text\n" );
      oyjlStr_Add( s, "# endif\n" );
      oyjlStr_Add( s, "#endif\n" );
    }
    else
    {
      oyjlStr_Add( s, "# define _(text) oyjlTranslate( trc, text )\n" );
    }

    n = oyjlValueCount( oyjlTreeGetValue( root, 0, "org/freedesktop/oyjl/ui/options/array" ) );
    for(i = 0; i < n; ++i)
    {
      const char *value_type;
      val = oyjlTreeGetValueF( root, 0, "org/freedesktop/oyjl/ui/options/array/[%d]", i );
      v = oyjlTreeGetValue( val, 0, "value_type" ); value_type = OYJL_GET_STRING(v);
      if(value_type && strcmp(value_type, "oyjlOPTIONTYPE_FUNCTION") == 0)
      {
        const char * getChoices;
        char * tmp = NULL;
        v = oyjlTreeGetValue( val, 0, "values/getChoices" ); getChoices = OYJL_GET_STRING(v);
        if(!getChoices)
        {
          char * t = oyjlUiGetVariableNameC_(val, NULL);
          oyjlStringAdd( &tmp, 0,0, "getChoicesForVar_%s", t );
          getChoices = tmp;
          free(t);
        }
        oyjlStr_Add( s, "oyjlOptionChoice_s * %s( oyjlOption_s * opt OYJL_UNUSED, int * selected OYJL_UNUSED, oyjlOptions_s * context OYJL_UNUSED )\n{ fprintf(stderr, \"\\t%%s()\\n\", __func__); return NULL; }\n", getChoices );
        if(tmp) free(tmp);
      }
    }
    oyjlStr_Add( s, "\n" );
    oyjlStr_Add( s, "/* This function is called the\n" );
    oyjlStr_Add( s, " * * first time for GUI generation and then\n" );
    oyjlStr_Add( s, " * * for executing the tool.\n" );
    oyjlStr_Add( s, " */\n" );
    oyjlStr_Add( s, "int myMain( int argc, const char ** argv )\n" );
    oyjlStr_Add( s, "{\n" );
    oyjlStr_Add( s, "  int error = 0;\n" );
    if(!(app_type && strcmp(app_type,"tool") == 0))
      oyjlStr_Add( s, "  int state = oyjlUI_STATE_NO_CHECKS;\n" );
    else
      oyjlStr_Add( s, "  int state = 0;\n" );

    n = oyjlValueCount( oyjlTreeGetValue( root, 0, "org/freedesktop/oyjl/ui/options/array" ) );
    for(i = 0; i < n; ++i)
    {
      const char * type = NULL;
      char * t;
      val = oyjlTreeGetValueF( root, 0, "org/freedesktop/oyjl/ui/options/array/[%d]", i );
      t = oyjlUiGetVariableNameC_( val, &type );
      if(type && t)
      {
        oyjlStr_Add( s, "  %s %s = 0;\n", type, t );
        if(strcmp(t, "X") == 0)
          X_found = 1;
        if(strcmp(t,"export_var") == 0)
          export_found = 1;
        if(strcmp(t,"h") == 0)
          h_found = 1;
        if(strcmp(t,"help") == 0)
          help_found = 1;
        if(strcmp(t,"v") == 0)
        {
          char * variable_type = NULL;
          v_found = 1;
          v = oyjlTreeGetValue( val, 0, "variable_type" );
          if(v) variable_type = OYJL_GET_STRING(v);
          if(variable_type && strcmp(variable_type, "oyjlSTRING") == 0)
            v_is_string = 1;
        }
        if(strcmp(t,"verbose") == 0)
          verbose_found = 1;
        if(strcmp(t,"version") == 0)
          version_found = 1;
        if(strcmp(t,"render") == 0)
          render_found = 1;
      }
      if(t) free(t);
    }
    if(!(flags & OYJL_NO_DEFAULT_OPTIONS))
    {
      if(!help_found)
        oyjlStr_Add( s, "  const char * help = NULL;\n" );
      if(!verbose_found)
        oyjlStr_Add( s, "  int verbose = 0;\n" );
      if(!version_found)
        oyjlStr_Add( s, "  int version = 0;\n" );
      if(!render_found)
        oyjlStr_Add( s, "  const char * render = NULL;\n" );
    }
    if(!export_found)
    oyjlStr_Add( s, "  const char * export_var = 0;\n" );
    oyjlStr_Add( s, "\n" );
    oyjlStr_Add( s, "  /* handle options */\n" );
    oyjlStr_Add( s, "  /* Select a nick from *version*, *manufacturer*, *copyright*, *license*,\n" );
    oyjlStr_Add( s, "   * *url*, *support*, *download*, *sources*, *oyjl_module_author* and\n" );
    oyjlStr_Add( s, "   * *documentation*. Choose what you see fit. Add new ones as needed. */\n" );
    oyjlStr_Add( s, "  oyjlUiHeaderSection_s sections[] = {\n" );
    oyjlStr_Add( s, "    /* type, " );
    oyjlStr_AddSpaced_( s, "nick",  0, 17 );
    oyjlStr_AddSpaced_( s, "label", 0, 7 );
    oyjlStr_AddSpaced_( s, "name",  0, 26 );
    oyjlStr_Add(       s, "description */\n" );
    n = oyjlValueCount( oyjlTreeGetValue( root, 0, "org/freedesktop/oyjl/ui/header/sections" ) );
    for(i = 0; i < n; ++i)
    {
      const char *nick, *label, *name, *desc;
      val = oyjlTreeGetValueF( root, 0, "org/freedesktop/oyjl/ui/header/sections/[%d]", i );
      v = oyjlTreeGetValue( val, 0, "nick" ); nick = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "label" ); label = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "name" ); name = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "description" ); desc = OYJL_GET_STRING(v);
      oyjlStr_Add( s, "    {\"oihs\", ");

      oyjlStr_AddSpaced_( s, nick,  OYJL_QUOTE,                17 );
      oyjlStr_AddSpaced_( s, label, OYJL_QUOTE|OYJL_TRANSLATE, 7 );
      oyjlStr_AddSpaced_( s, name&&name[0]?name:NULL,  OYJL_QUOTE|OYJL_TRANSLATE, 26 );
      oyjlStr_AddSpaced_( s, desc,  OYJL_QUOTE|OYJL_TRANSLATE|OYJL_LAST, 4 );
      oyjlStr_Add( s, "},\n");
    }
    oyjlStr_Add( s, "    {\"\",0,0,0,0}};\n" );
    oyjlStr_Add( s, "\n" );
    oyjlStr_Add( s,       "  /* declare the option choices  *   " );
    oyjlStr_AddSpaced_( s, "nick",         0, 15 );
    oyjlStr_AddSpaced_( s, "name",         0, 20 );
    oyjlStr_AddSpaced_( s, "description",  0, 30 );
    oyjlStr_AddSpaced_( s, "help",         OYJL_LAST, 4 );
    oyjlStr_Add( s,       " */\n" );
    n = oyjlValueCount( oyjlTreeGetValue( root, 0, "org/freedesktop/oyjl/ui/options/array" ) );
    for(i = 0; i < n; ++i)
    {
      const char *value_type, *o, *o_fallback, *option;
      val = oyjlTreeGetValueF( root, 0, "org/freedesktop/oyjl/ui/options/array/[%d]", i );
      v = oyjlTreeGetValue( val, 0, "o" ); o_fallback = o = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "option" ); option = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "value_type" ); value_type = OYJL_GET_STRING(v);
      if(value_type && strcmp(value_type, "oyjlOPTIONTYPE_CHOICE") == 0)
      {
        const char *nick, *name, *desc, *help;
        int count = oyjlValueCount( oyjlTreeGetValue( val, 0, "values/choices/list" ) ), j;
        if(count)
          oyjlStr_Add( s, "  oyjlOptionChoice_s %s_choices[] = {", o_fallback );
        for(j = 0; j < count; ++j)
        {
          oyjl_val c = oyjlTreeGetValueF( val, 0, "values/choices/list/[%d]", j );
          v = oyjlTreeGetValue( c, 0, "nick" ); nick = OYJL_GET_STRING(v);
          v = oyjlTreeGetValue( c, 0, "name" ); name = OYJL_GET_STRING(v);
          v = oyjlTreeGetValue( c, 0, "description" ); desc = OYJL_GET_STRING(v);
          v = oyjlTreeGetValue( c, 0, "help" ); help = OYJL_GET_STRING(v);
          if(j)
          oyjlStr_Add( s, "                                    {");
          else
            oyjlStr_Add( s, "{");

          if(option && strstr(option, "man-") != NULL)
            oyjlStr_AddSpaced_(s,nick,  OYJL_QUOTE|OYJL_TRANSLATE, 15 );
          else
            oyjlStr_AddSpaced_(s,nick,  OYJL_QUOTE,                15 );
          oyjlStr_AddSpaced_( s, name,  OYJL_QUOTE|OYJL_TRANSLATE, 20 );
          oyjlStr_AddSpaced_( s, desc,  OYJL_QUOTE|OYJL_TRANSLATE, 30 );
          oyjlStr_AddSpaced_( s, help,  OYJL_QUOTE|OYJL_TRANSLATE|OYJL_LAST, 4 );
          oyjlStr_Add( s, "},\n");
        }
        if(count)
        {
          oyjlStr_Add( s, "                                    {NULL,NULL,NULL,NULL}};\n" );
          oyjlStr_Add( s, "\n" );
        }
      }
    }
    oyjlStr_Add( s, "  /* declare options - the core information; use previously declared choices */\n" );
    oyjlStr_Add( s, "  oyjlOption_s oarray[] = {\n" );
    oyjlStr_Add( s, "  /* type,   " );
    oyjlStr_AddSpaced_( s, "flags",        0, 28 );
    oyjlStr_AddSpaced_( s, "o",            0, 4 );
    oyjlStr_AddSpaced_( s, "option",       0, 17 );
    oyjlStr_AddSpaced_( s, "key",          0, 10 );
    oyjlStr_AddSpaced_( s, "name",         0, 15 );
    oyjlStr_AddSpaced_( s, "description",  0, 30 );
    oyjlStr_AddSpaced_( s, "help",         0, 6 );
    oyjlStr_AddSpaced_( s, "value_name",   0, 20 );
    oyjlStr_Add( s,       "\n        " );
    oyjlStr_AddSpaced_( s, "value_type",   0, 25 );
    oyjlStr_AddSpaced_( s, "values",       0, 20 );
    oyjlStr_AddSpaced_( s, "variable_type",0, 15 );
    oyjlStr_AddSpaced_( s, "variable_name",OYJL_LAST, 10 );
    oyjlStr_Add( s,       " */\n" );
    for(i = 0; i < n; ++i)
    {
      char * flag_string = NULL;
      int flg, j, len;
      char oo[4] = {0,0,0,0}, *tmp_name = NULL, *tmp_variable_name = NULL;
      const char *o, *option, *option_fallback, *key, *name, *desc, *help, *value_name, *value_type, *variable_type, *variable_name;
      val = oyjlTreeGetValueF( root, 0, "org/freedesktop/oyjl/ui/options/array/[%d]", i );
      v = oyjlTreeGetValue( val, 0, "flags" ); flg = OYJL_IS_INTEGER(v) ? OYJL_GET_INTEGER(v) : 0;
      v = oyjlTreeGetValue( val, 0, "value_type" ); value_type = OYJL_GET_STRING(v);
      if( flg & OYJL_OPTION_FLAG_EDITABLE ||
          ( value_type && strcmp(value_type, "oyjlOPTIONTYPE_CHOICE") == 0 &&
            oyjlValueCount( oyjlTreeGetValue( val, 0, "values/choices/list" ) ) == 0
          )
        )
        oyjlStringAdd( &flag_string, 0,0, "%s", "OYJL_OPTION_FLAG_EDITABLE" );
      if(flg & OYJL_OPTION_FLAG_ACCEPT_NO_ARG)
        oyjlStringAdd( &flag_string, 0,0, "%s%s", flag_string?"|":"", "OYJL_OPTION_FLAG_ACCEPT_NO_ARG" );
      if(flg & OYJL_OPTION_FLAG_NO_DASH)
        oyjlStringAdd( &flag_string, 0,0, "%s%s", flag_string?"|":"", "OYJL_OPTION_FLAG_NO_DASH" );
      if(flg & OYJL_OPTION_FLAG_REPETITION)
        oyjlStringAdd( &flag_string, 0,0, "%s%s", flag_string?"|":"", "OYJL_OPTION_FLAG_REPETITION" );
      if(flg & OYJL_OPTION_FLAG_MAINTENANCE)
        oyjlStringAdd( &flag_string, 0,0, "%s%s", flag_string?"|":"", "OYJL_OPTION_FLAG_MAINTENANCE" );
      if(!flag_string)
        flag_string = oyjlStringCopy("0",0);
      v = oyjlTreeGetValue( val, 0, "option" ); option_fallback = option = OYJL_GET_STRING(v); /* "option" is needed as fallback for "name" */
      v = oyjlTreeGetValue( val, 0, "variable_name" ); variable_name = OYJL_GET_STRING(v); /* "variable_name" is needed as fallback for "option" */
      v = oyjlTreeGetValue( val, 0, "o" ); o = OYJL_GET_STRING(v);
      if(!option_fallback && o && o[0] != '@' && flags & OYJL_SUGGEST_VARIABLE_NAMES)
      {
        const char * ctype = NULL;
        tmp_variable_name = oyjlUiGetVariableNameC_( val, &ctype );
        if(tmp_variable_name)
        {
          len = strlen(tmp_variable_name);
          for(j = 0; j < len; ++j)
          {
            if(tmp_variable_name[j] == '_')
              tmp_variable_name[j] = '-';
          }
          option_fallback = tmp_variable_name;
        }
      }
      v = oyjlTreeGetValue( val, 0, "name" ); name = OYJL_GET_STRING(v);
      /* The UI has to provide something. So guess anyway. */
      if(!name && option_fallback && o && o[0] != '@')
      {
        len = strlen( option_fallback );
        tmp_name = oyjlStringCopy( option_fallback, 0 );
        if(tmp_name)
        {
          len = strlen(tmp_name);
          for(j = 0; j < len; ++j)
          {
            if(tmp_name[j] == '-')
              tmp_name[j] = ' ';
          }
          for(j = 0; j < len; ++j)
          {
            if(j == 0 || tmp_name[j-1] == ' ')
              tmp_name[j] = toupper(tmp_name[j]);
          }
        }
        name = tmp_name;
      }
      /* "o" member initialisation is a essential part to run the C code */
      if(!o || !o[0])
      {
        int len = name?strlen(name):0,
            j;
        if(flags & OYJL_SUGGEST_VARIABLE_NAMES)
        {
          for(j = 0; j < len; ++j)
          {
            oyjl_val nv = oyjlFindOption_( root, name[j] );
            if(!nv)
            {
              oo[0] = name[j];
              o = oo;
              oyjlTreeSetStringF( val, OYJL_CREATE_NEW, oo, "o" );
              break;
            }
          }
        }
        else
        {
          oyjlTreeSetStringF( val, OYJL_CREATE_NEW, "", "o" );
          // get again as the old 'o' is overwritten by oyjlTreeSetStringF()
          v = oyjlTreeGetValue( val, 0, "o" ); o = OYJL_GET_STRING(v);
        }
      }
      v = oyjlTreeGetValue( val, 0, "key" ); key = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "description" ); desc = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "help" ); help = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "value_name" ); value_name = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "value_type" ); value_type = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "variable_type" ); variable_type = OYJL_GET_STRING(v);
      oyjlStr_Add( s, "    {\"oiwi\", ");
      oyjlStr_AddSpaced_( s, flag_string, 0,                         28 );
      if(*oyjl_debug)
        fprintf( stderr, "o: \"%s\" / %s\n", o, option );
      if(o && o[0])
        oyjlStr_AddSpaced_( s, o,         OYJL_QUOTE,                4 );
      else
        oyjlStr_AddSpaced_( s, "NULL",    0,                         4 );
      oyjlStr_AddSpaced_( s, option,      OYJL_QUOTE,                17 );
      oyjlStr_AddSpaced_( s, key,         OYJL_QUOTE,                10 );
      oyjlStr_AddSpaced_( s, name,        OYJL_QUOTE|OYJL_TRANSLATE, 15 );
      oyjlStr_AddSpaced_( s, desc,        OYJL_QUOTE|OYJL_TRANSLATE, 30 );
      oyjlStr_AddSpaced_( s, help,        OYJL_QUOTE|OYJL_TRANSLATE, 6 );
      oyjlStr_AddSpaced_( s, value_name,  OYJL_QUOTE|OYJL_TRANSLATE, 20 );
      oyjlStr_Add( s,       "\n        " );
      oyjlStr_AddSpaced_( s, value_type && value_type[0] ? value_type : "oyjlOPTIONTYPE_NONE",  0,                         25 );
      if(value_type && strcmp(value_type, "oyjlOPTIONTYPE_CHOICE") == 0)
      {
        int count = oyjlValueCount( oyjlTreeGetValue( val, 0, "values/choices/list" ) );
        if(count)
          oyjlStr_Add( s,   "{.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)%s_choices, sizeof(%s_choices), malloc ), 0}}, ", o,o );
        else
          oyjlStr_AddSpaced_(s,"{0}",        0, 20 );
      } else
      if(value_type && strcmp(value_type, "oyjlOPTIONTYPE_FUNCTION") == 0)
      {
        const char * getChoices;
        int j;
        v = oyjlTreeGetValue( val, 0, "values/getChoices" ); getChoices = OYJL_GET_STRING(v);
        if(getChoices)
        oyjlStr_Add( s,     "{.getChoices = %s}, ", getChoices );
        else
        oyjlStr_Add( s,     "{0},                " );
        if(getChoices && strlen(getChoices) < 2)
        {
          int len = strlen(getChoices);
          for(j = 0; j < len - 18; ++j)
            oyjlStr_Add( s, " " );
        }

      } else
      if(value_type && strcmp(value_type, "oyjlOPTIONTYPE_DOUBLE") == 0)
      {
        double d, start, tick, end;
        v = oyjlTreeGetValue( val, 0, "values/dbl/d" ); d = OYJL_IS_DOUBLE(v) ? OYJL_GET_DOUBLE(v) : -1.0;
        v = oyjlTreeGetValue( val, 0, "values/dbl/start" ); start = OYJL_IS_DOUBLE(v) ? OYJL_GET_DOUBLE(v) : -1.0;
        v = oyjlTreeGetValue( val, 0, "values/dbl/tick" ); tick = OYJL_IS_DOUBLE(v) ? OYJL_GET_DOUBLE(v) : -1.0;
        v = oyjlTreeGetValue( val, 0, "values/dbl/end" ); end = OYJL_IS_DOUBLE(v) ? OYJL_GET_DOUBLE(v) : -1.0;
        if(!(start == -1.0 && end == -1.0 && tick == -1.0 && d == 1.0))
          oyjlStr_Add( s,   "{.dbl = {.d = %g, .start = %g, .end = %g, .tick = %g}}, ", d == -1.0 ? 0 : d, start, end, tick == -1.0 ? 0.0 : tick );
        else
          oyjlStr_Add( s,   "{0}, " );
      } else
      if(!value_type || (value_type && !value_type[0]) || (value_type && strcmp(value_type, "oyjlOPTIONTYPE_NONE") == 0))
      {
        oyjlStr_AddSpaced_(s,"{0}",        0, 20 );
      }
      oyjlStr_AddSpaced_( s, variable_type,0,                         15 );
      if(variable_type && strcmp(variable_type, "oyjlNONE") == 0)
        oyjlStr_AddSpaced_( s, variable_name?variable_name:"{}",OYJL_LAST,                 2 );
      else
      {
        char * t = NULL;
        char type = 0;
        if(variable_type && strcmp(variable_type, "oyjlSTRING") == 0)
          type = 's';
        else
        if(variable_type && strcmp(variable_type, "oyjlDOUBLE") == 0)
          type = 'd';
        else
        if(variable_type && strcmp(variable_type, "oyjlINT") == 0)
          type = 'i';

        if(type)
        {
          const char * ctype = NULL;
          char * vname = oyjlUiGetVariableNameC_( val, &ctype );
          if(ctype && vname)
            oyjlStringAdd( &t, 0,0, "{.%c=&%s}", type, vname );
          if(vname) free(vname);
        }
        else
          oyjlStringAdd( &t, 0,0, "{0}" );
        oyjlStr_AddSpaced_( s, t,OYJL_LAST,                 2 );
        if(t) free( t );
      }
      
      oyjlStr_Add( s, "},\n");
      if(flag_string) free( flag_string );
      if(tmp_name) free( tmp_name );
    }
    if(!(flags & OYJL_NO_DEFAULT_OPTIONS))
    {
      if(!help_found && !oyjlFindOption_( root, 'h' ))
      oyjlStr_Add(s,"    /* default options -h and -v */\n" );
      if(!help_found && !oyjlFindOption_( root, 'h' ))
      oyjlStr_Add(s,"    {\"oiwi\", OYJL_OPTION_FLAG_ACCEPT_NO_ARG, \"h\", \"help\", NULL, NULL, NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s=&help} },\n" );
      if(!verbose_found && !oyjlFindOption_( root, 'v' ))
      oyjlStr_Add(s,"    {\"oiwi\", 0, \"v\", \"verbose\", NULL, _(\"verbose\"), _(\"Verbose\"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&verbose} },\n" );
      if(!render_found && !oyjlFindOption_( root, 'R' ))
      {
      oyjlStr_Add(s,"    /* The --render option can be hidden and used only internally. */\n" );
      oyjlStr_Add(s,"    {\"oiwi\", OYJL_OPTION_FLAG_EDITABLE|OYJL_OPTION_FLAG_MAINTENANCE, \"R\", \"render\",  NULL, _(\"render\"),  _(\"Render\"),  NULL, NULL, oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s=&render} },\n" );
      }
      if(!version_found && !oyjlFindOption_( root, 'V' ))
      oyjlStr_Add(s,"    {\"oiwi\", 0, \"V\", \"version\", NULL, _(\"version\"), _(\"Version\"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&version} },\n" );
      oyjlStr_Add(s,"    /* default option template -X|--export */\n" );
      if(!export_found && !X_found)
      oyjlStr_Add(s,"    {\"oiwi\", 0, \"X\", \"export\", NULL, NULL, NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices = {NULL, 0}}, oyjlSTRING, {.s=&export_var} },\n" );
    }
    oyjlStr_Add( s, "    {\"\",0,0,NULL,NULL,NULL,NULL,NULL, NULL, oyjlOPTIONTYPE_END, {0},oyjlNONE,{0}}\n  };\n\n" );
    oyjlStr_Add( s, "  /* declare option groups, for better syntax checking and UI groups */\n" );
    oyjlStr_Add( s, "  oyjlOptionGroup_s groups[] = {\n" );
    oyjlStr_Add( s, "  /* type,   " );
    oyjlStr_AddSpaced_( s, "flags",        0, 7 );
    oyjlStr_AddSpaced_( s, "name",         0, 20 );
    oyjlStr_AddSpaced_( s, "description",  0, 30 );
    oyjlStr_AddSpaced_( s, "help",         0, 20 );
    oyjlStr_AddSpaced_( s, "mandatory",    0, 15 );
    oyjlStr_AddSpaced_( s, "optional",     0, 15 );
    oyjlStr_AddSpaced_( s, "detail",       OYJL_LAST, 4 );
    oyjlStr_Add( s,       " */\n" );
    n = oyjlValueCount( oyjlTreeGetValue( root, 0, "org/freedesktop/oyjl/ui/options/groups" ) );
    for(i = 0; i < n; ++i)
    {
      const char *name, *desc, *help, *mandatory, *optional, *detail;
      char * flag_string = NULL;
      int flg;
      val = oyjlTreeGetValueF( root, 0, "org/freedesktop/oyjl/ui/options/groups/[%d]", i );
      v = oyjlTreeGetValue( val, 0, "name" ); name = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "description" ); desc = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "help" ); help = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "mandatory" ); mandatory = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "optional" ); optional = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "detail" ); detail = OYJL_GET_STRING(v);
      oyjlStr_Add( s, "    {\"oiwg\", ");
      v = oyjlTreeGetValue( val, 0, "flags" ); flg = OYJL_IS_INTEGER(v) ? OYJL_GET_INTEGER(v) : 0;
      if(flg & OYJL_GROUP_FLAG_SUBCOMMAND)
        oyjlStringAdd( &flag_string, 0,0, "%s", "OYJL_GROUP_FLAG_SUBCOMMAND" );
      if(flg & OYJL_GROUP_FLAG_GENERAL_OPTS)
        oyjlStringAdd( &flag_string, 0,0, "%s%s", flag_string?"|":"", "OYJL_GROUP_FLAG_GENERAL_OPTS" );
      if(!flag_string)
        flag_string = oyjlStringCopy("0",0);
      oyjlStr_AddSpaced_( s, flag_string, 0,                         7 );
      oyjlStr_AddSpaced_( s, name,        OYJL_QUOTE|OYJL_TRANSLATE, 20 );
      oyjlStr_AddSpaced_( s, desc,        OYJL_QUOTE|OYJL_TRANSLATE, 30 );
      oyjlStr_AddSpaced_( s, help,        OYJL_QUOTE|OYJL_TRANSLATE, 20 );
      oyjlStr_AddSpaced_( s, mandatory,   OYJL_QUOTE,                15 );
      oyjlStr_AddSpaced_( s, optional,    OYJL_QUOTE,                15 );
      oyjlStr_AddSpaced_( s, detail,      OYJL_QUOTE|OYJL_LAST,      4 );
      oyjlStr_Add( s, "},\n");
      if(flag_string) free( flag_string );
    }
    oyjlStr_Add( s, "    {\"\",0,0,0,0,0,0,0}\n" );
    oyjlStr_Add( s, "  };\n\n");
    const char *nick, *name, *description, *logo;
    val = oyjlTreeGetValue( root, 0, OYJL_REG "/ui/nick" ); nick = OYJL_GET_STRING(val);
    val = oyjlTreeGetValue( root, 0, OYJL_REG "/ui/name" ); name = OYJL_GET_STRING(val);
    val = oyjlTreeGetValue( root, 0, OYJL_REG "/ui/description" ); description = OYJL_GET_STRING(val);
    val = oyjlTreeGetValue( root, 0, OYJL_REG "/ui/logo" ); logo = OYJL_GET_STRING(val);
    oyjlStr_Add( s, "  oyjlUi_s * ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */\n" );
    oyjlStr_Add( s, "                                       \"%s\", ", nick );
    if(name)
      oyjlStr_Add( s, "_(\"%s\"), ", name );
    else
      oyjlStr_Add( s, "NULL, " );
    if(description)
      oyjlStr_Add( s, "_(\"%s\"),\n", description );
    else
      oyjlStr_Add( s, "NULL,\n" );
    oyjlStr_Add( s, "#ifdef __ANDROID__\n" );
    oyjlStr_Add( s, "                                       \":/images/logo.svg\", // use qrc\n" );
    oyjlStr_Add( s, "#else\n" );
    if(logo)
      oyjlStr_Add( s, "                                       \"%s\",\n", logo );
    else
      oyjlStr_Add( s, "                                       NULL,\n" );
    oyjlStr_Add( s, "#endif\n" );
    oyjlStr_Add( s, "                                       sections, oarray, groups, &state );\n" );
    if(!(app_type && strcmp(app_type,"tool") == 0))
    oyjlStr_Add( s, "  if(ui) ui->app_type = \"%s\";\n", app_type ? app_type : "lib" );
    if(!help_found && h_found)
      oyjlStr_Add( s, "  help = h ? \"\" : NULL;\n" );
    if(!verbose_found && v_found)
    {
      if(v_is_string)
        oyjlStr_Add( s, "  verbose = v ? 1 : 0;\n" );
      else
        oyjlStr_Add( s, "  verbose = v;\n" );
    }
    oyjlStr_Add( s, "  if( state & oyjlUI_STATE_EXPORT &&\n" );
    oyjlStr_Add( s, "      export_var &&\n" );
    oyjlStr_Add( s, "      strcmp(export_var,\"json+command\") != 0)\n" );
    oyjlStr_Add( s, "    goto clean_main;\n");
    oyjlStr_Add( s, "  if(state & oyjlUI_STATE_HELP)\n" );
    oyjlStr_Add( s, "  {\n" );
    oyjlStr_Add( s, "    fprintf( stderr, \"%%s\\n\\tman %s\\n\\n\", _(\"For more information read the man page:\") );\n", nick, nick );
    oyjlStr_Add( s, "    goto clean_main;\n" );
    oyjlStr_Add( s, "  }\n" );
    oyjlStr_Add( s, "\n" );
    oyjlStr_Add( s, "  if(ui && verbose)\n" );
    oyjlStr_Add( s, "  {\n" );
    if(flags & OYJL_WITH_OYJL_ARGS_C)
    oyjlStr_Add( s, "#if 0\n" );
    oyjlStr_Add( s, "    char * json = oyjlOptions_ResultsToJson( ui->opts );\n" );
    oyjlStr_Add( s, "    if(json)\n" );
    oyjlStr_Add( s, "      fputs( json, stderr );\n" );
    oyjlStr_Add( s, "    fputs( \"\\n\", stderr );\n" );
    if(flags & OYJL_WITH_OYJL_ARGS_C)
    oyjlStr_Add( s, "#endif\n" );
    oyjlStr_Add( s, "\n" );
    oyjlStr_Add( s, "    char * text = oyjlOptions_ResultsToText( ui->opts );\n" );
    oyjlStr_Add( s, "    if(text)\n" );
    oyjlStr_Add( s, "      fputs( text, stderr );\n" );
    oyjlStr_Add( s, "    fputs( \"\\n\", stderr );\n" );
    oyjlStr_Add( s, "  }\n" );
    oyjlStr_Add( s, "\n" );
    oyjlStr_Add( s, "  if(ui && (export_var && strcmp(export_var,\"json+command\") == 0))\n" );
    oyjlStr_Add( s, "  {\n" );
    if(flags & OYJL_WITH_OYJL_ARGS_C)
    oyjlStr_Add( s, "#if 0\n" );
    oyjlStr_Add( s, "    char * json = oyjlUi_ToJson( ui, 0 ),\n" );
    oyjlStr_Add( s, "         * json_commands = NULL;\n" );
    oyjlStr_Add( s, "    oyjlStringAdd( &json_commands, malloc, free, \"{\\n  \\\"command_set\\\": \\\"%%s\\\",\", argv[0] );\n" );
    oyjlStr_Add( s, "    oyjlStringAdd( &json_commands, malloc, free, \"%%s\", &json[1] ); /* skip opening '{' */\n" );
    oyjlStr_Add( s, "    puts( json_commands );\n" );
    if(flags & OYJL_WITH_OYJL_ARGS_C)
    {
      oyjlStr_Add( s, "#else\n" );
      oyjlStr_Add( s, "    fputs( \"oyjlUi_ToJson not supported.\", stderr );\n" );
      oyjlStr_Add( s, "#endif\n" );
    }
    oyjlStr_Add( s, "    goto clean_main;\n" );
    oyjlStr_Add( s, "  }\n" );
    oyjlStr_Add( s, "\n" );
    oyjlStr_Add( s, "  /* Render boilerplate */\n" );
    oyjlStr_Add( s, "  if(ui && render)\n" );
    oyjlStr_Add( s, "  {\n" );
    oyjlStr_Add( s, "#if !defined(NO_OYJL_ARGS_RENDER)\n" );
    oyjlStr_Add( s, "    int debug = verbose;\n" );
    oyjlStr_Add( s, "    oyjlArgsRender( argc, argv, NULL, NULL,NULL, debug, ui, myMain );\n" );
    oyjlStr_Add( s, "#else\n" );
    oyjlStr_Add( s, "    fprintf( stderr, \"No render support compiled in. For a GUI you might by able to use -X json+command and load into oyjl-args-render viewer.\\n\" );\n" );
    oyjlStr_Add( s, "#endif\n" );
    oyjlStr_Add( s, "  } else if(ui)\n" );
    oyjlStr_Add( s, "  {\n" );
    oyjlStr_Add( s, "    /* ... working code goes here ... */\n" );
    oyjlStr_Add( s, "  }\n" );
    oyjlStr_Add( s, "  else error = 1;\n" );
    oyjlStr_Add( s, "\n" );
    oyjlStr_Add( s, "  clean_main:\n" );
    oyjlStr_Add( s, "  {\n" );
    oyjlStr_Add( s, "    int i = 0;\n" );
    oyjlStr_Add( s, "    while(oarray[i].type[0])\n" );
    oyjlStr_Add( s, "    {\n" );
    oyjlStr_Add( s, "      if(oarray[i].value_type == oyjlOPTIONTYPE_CHOICE && oarray[i].values.choices.list)\n" );
    oyjlStr_Add( s, "        free(oarray[i].values.choices.list);\n" );
    oyjlStr_Add( s, "      ++i;\n" );
    oyjlStr_Add( s, "    }\n" );
    oyjlStr_Add( s, "  }\n" );
    oyjlStr_Add( s, "\n" );
    oyjlStr_Add( s, "  return error;\n" );
    oyjlStr_Add( s, "}\n" );
    oyjlStr_Add( s, "\n" );
    oyjlStr_Add( s, "extern int * oyjl_debug;\n" );
    oyjlStr_Add( s, "char ** environment = NULL;\n" );
    oyjlStr_Add( s, "int main( int argc_, char**argv_, char ** envv )\n" );
    oyjlStr_Add( s, "{\n" );
    oyjlStr_Add( s, "  int argc = argc_;\n" );
    oyjlStr_Add( s, "  char ** argv = argv_;\n" );
    oyjlStr_Add( s, "  oyjlTr_s * trc_ = NULL;\n" );
    oyjlStr_Add( s, "  const char * loc = NULL;\n" );
    oyjlStr_Add( s, "  const char * lang = getenv(\"LANG\");\n" );
    oyjlStr_Add( s, "\n" );
    oyjlStr_Add( s, "#ifdef __ANDROID__\n" );
    oyjlStr_Add( s, "  setenv(\"COLORTERM\", \"1\", 0); /* show rich text format on non GNU color extension environment */\n" );
    oyjlStr_Add( s, "\n" );
    oyjlStr_Add( s, "  argv = calloc( argc + 2, sizeof(char*) );\n" );
    oyjlStr_Add( s, "  memcpy( argv, argv_, (argc + 2) * sizeof(char*) );\n" );
    oyjlStr_Add( s, "  argv[argc++] = \"--render=gui\"; /* start Renderer (e.g. QML) */\n" );
    oyjlStr_Add( s, "  environment = environ;\n" );
    oyjlStr_Add( s, "#else\n" );
    oyjlStr_Add( s, "  environment = envv;\n" );
    oyjlStr_Add( s, "#endif\n" );
    oyjlStr_Add( s, "\n" );
    oyjlStr_Add( s, "  /* language needs to be initialised before setup of data structures */\n" );
    oyjlStr_Add( s, "  int use_gettext = 0;\n" );
    oyjlStr_Add( s, "#ifdef OYJL_HAVE_LIBINTL_H\n" );
    oyjlStr_Add( s, "  use_gettext = 1;\n" );
    oyjlStr_Add( s, "#endif\n" );
    oyjlStr_Add( s, "#ifdef OYJL_HAVE_LOCALE_H\n" );
    oyjlStr_Add( s, "  loc = setlocale(LC_ALL,\"\");\n" );
    oyjlStr_Add( s, "#endif\n" );
    oyjlStr_Add( s, "  if(!loc)\n" );
    oyjlStr_Add( s, "  {\n" );
    oyjlStr_Add( s, "    loc = lang;\n" );
    oyjlStr_Add( s, "    fprintf( stderr, \"%%s\", oyjlTermColor(oyjlRED,\"Usage Error:\") );\n" );
    oyjlStr_Add( s, "    fprintf( stderr, \" Environment variable possibly not correct. Translations might fail - LANG=%%s\\n\", oyjlTermColor(oyjlBOLD,lang) );\n" );
    oyjlStr_Add( s, "  }\n" );
    oyjlStr_Add( s, "  if(lang)\n" );
    oyjlStr_Add( s, "    loc = lang;\n" );
    oyjlStr_Add( s, "  if(loc)\n" );
    oyjlStr_Add( s, "  {\n" );
    oyjlStr_Add( s, "    const char * my_domain = MY_DOMAIN;\n" );
    oyjlStr_Add( s, "    if(my_domain && strcmp(my_domain,\"oyjl\") == 0)\n" );
    oyjlStr_Add( s, "      my_domain = NULL;\n" );
    oyjlStr_Add( s, "    trc = trc_ = oyjlTr_New( loc, my_domain, 0,0,0,0,0 );\n" );
    oyjlStr_Add( s, "  }\n" );
    oyjlStr_Add( s, "  oyjlInitLanguageDebug( \"Oyjl\", \"OYJL_DEBUG\", oyjl_debug, use_gettext, \"OYJL_LOCALEDIR\", OYJL_LOCALEDIR, &trc_, NULL );\n" );
    oyjlStr_Add( s, "  if(MY_DOMAIN && strcmp(MY_DOMAIN,\"oyjl\") == 0)\n" );
    oyjlStr_Add( s, "    trc = oyjlTr_Get( MY_DOMAIN );\n" );
    oyjlStr_Add( s, "\n" );
    oyjlStr_Add( s, "  myMain(argc, (const char **)argv);\n" );
    oyjlStr_Add( s, "\n" );
    oyjlStr_Add( s, "  oyjlTr_Release( &trc_ );\n" );
    oyjlStr_Add( s, "  oyjlLibRelease();\n" );
    oyjlStr_Add( s, "\n" );
    oyjlStr_Add( s, "#ifdef __ANDROID__\n" );
    oyjlStr_Add( s, "  free( argv );\n" );
    oyjlStr_Add( s, "#endif\n" );
    oyjlStr_Add( s, "\n" );
    oyjlStr_Add( s, "  return 0;\n" );
    oyjlStr_Add( s, "}\n" );
    oyjlStr_Add( s, "\n" );
  }
  else if(flags & OYJL_COMPLETION_BASH)
  {
    oyjl_val val, v;
    char * func;
    const char *nick_;
    int i,n;
    oyjlUi_s * ui;
    oyjlOption_s * opt;
    int found_at_arg_mode = 0;
    const char * found_at_arg_func = NULL;

    val = oyjlTreeGetValue( root, 0, OYJL_REG "/ui/nick" ); nick_ = OYJL_GET_STRING(val);
    func = oyjlStringCopy( nick_, 0 );
    oyjlUiCanonicaliseVariableName_( &func );
    if(!func) return c;

    ui = oyjlUi_ImportFromJson( root, 0 );

    oyjlStr_Add( s, "_%s()\n", func );
    oyjlStr_Add( s, "{\n" );
    oyjlStr_Add( s, "    local cur prev words cword\n" );
    oyjlStr_Add( s, "    _init_completion -s || return\n" );
    oyjlStr_Add( s, "\n" );
    oyjlStr_Add( s, "    #set -x -v\n" );
    oyjlStr_Add( s, "\n" );
    oyjlStr_Add( s, "    local SEARCH=${COMP_WORDS[COMP_CWORD]}\n" );
    oyjlStr_Add( s, "    if [[ \"$SEARCH\" == \"=\" ]]; then\n" );
    oyjlStr_Add( s, "      SEARCH=\"\"\n" );
    oyjlStr_Add( s, "    fi\n" );
    oyjlStr_Add( s, "\n" );
    oyjlStr_Add( s, "    : \"autocomplete options with choices for long options \"$prev\"\"\n" );
    oyjlStr_Add( s, "    case \"$prev\" in\n" );
    n = oyjlValueCount( oyjlTreeGetValue( root, 0, "org/freedesktop/oyjl/ui/options/array" ) );
    for(i = 0; i < n; ++i)
    {
      const char *value_type, *option, *o;
      val = oyjlTreeGetValueF( root, 0, "org/freedesktop/oyjl/ui/options/array/[%d]", i );
      v = oyjlTreeGetValue( val, 0, "value_type" ); value_type = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "option" ); option = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "o" ); o = OYJL_GET_STRING(v);
      if(option && strstr(option, "man-") != NULL)
        continue;
      if(o && (strcmp(o,"#") == 0 || strcmp(o,"@") == 0))
        continue;
      if(value_type && strcmp(value_type, "oyjlOPTIONTYPE_FUNCTION") == 0)
      {
        const char * getChoices;
        int use_getChoicesCompletionBash = 0;
        v = oyjlTreeGetValue( val, 0, "values/getChoicesCompletionBash" ); getChoices = OYJL_GET_STRING(v);
        if(getChoices)
        {
          use_getChoicesCompletionBash = 1;
          if(*oyjl_debug)
            oyjlMessage_p( oyjlMSG_INFO, 0, "found getChoicesCompletionBash: `%s` for --%s", getChoices, oyjlTermColor( oyjlITALIC, option ) );
        } else
        {
          v = oyjlTreeGetValue( val, 0, "values/getChoices" ); getChoices = OYJL_GET_STRING(v);
        }
        if(getChoices || !use_getChoicesCompletionBash)
        {
          oyjlStr_Add( s, "        --%s) # long option with dynamic args\n", option );
          if(use_getChoicesCompletionBash)
            oyjlStr_Add( s, "            local OYJL_TEXTS=$(%s)\n", getChoices );
          else
          {
            oyjlStr_Add( s, "            local OYJL_TEXTS\n" );
            oyjlStr_Add( s, "            if [[ \"${COMP_WORDS[COMP_CWORD]}\" == \"=\" ]]; then\n" );
            oyjlStr_Add( s, "              OYJL_TEXTS=$(${COMP_LINE}oyjl-list | sed 's/\\[/_/g;s/\\]/_/g')\n" );
            oyjlStr_Add( s, "            else\n" );
            oyjlStr_Add( s, "              OYJL_TEXTS=$(${COMP_LINE} --%s=oyjl-list | sed 's/\\[/_/g;s/\\]/_/g')\n", option );
            oyjlStr_Add( s, "            fi\n" );
          }
          oyjlStr_Add( s, "            local IFS=$'\\n'\n" );
          oyjlStr_Add( s, "            local WORD_LIST=()\n" );
          oyjlStr_Add( s, "            for OYJL_TEXT in $OYJL_TEXTS\n" );
          oyjlStr_Add( s, "              do WORD_LIST=(\"${WORD_LIST[@]}\" \"$OYJL_TEXT\")\n" );
          oyjlStr_Add( s, "            done\n" );
          oyjlStr_Add( s, "            COMPREPLY=($(compgen -W '\"${WORD_LIST[@]}\"' -- \"$cur\"))\n" );
          oyjlStr_Add( s, "            set +x +v\n" );
          oyjlStr_Add( s, "            return\n" );
          oyjlStr_Add( s, "            ;;\n" );
        }
      } else
      if(value_type && strcmp(value_type, "oyjlOPTIONTYPE_CHOICE") == 0)
      {
        const char *nick;
        int count = oyjlValueCount( oyjlTreeGetValue( val, 0, "values/choices/list" ) ), j;
        if(count)
        {
          oyjlStr_Add( s, "        --%s) # long option with static args\n", option );
          oyjlStr_Add( s, "            local IFS=$'\\n'\n" );
          oyjlStr_Add( s, "            local WORD_LIST=(" );
        }
        for(j = 0; j < count; ++j)
        {
          oyjl_val c = oyjlTreeGetValueF( val, 0, "values/choices/list/[%d]", j );
          v = oyjlTreeGetValue( c, 0, "nick" ); nick = OYJL_GET_STRING(v);
          oyjlStr_Add( s, "%s'%s'", j?" ":"", nick );
        }
        if(count)
        {
          oyjlStr_Add( s, ")\n" );
          oyjlStr_Add( s, "            COMPREPLY=($(compgen -W '\"${WORD_LIST[@]}\"' -- \"$cur\"))\n" );
          oyjlStr_Add( s, "            set +x +v\n" );
          oyjlStr_Add( s, "            return\n" );
          oyjlStr_Add( s, "            ;;\n" );
        }
      }
    }
    oyjlStr_Add( s, "    esac\n" );
    oyjlStr_Add( s, "    : \"autocomplete options with choices for single letter options \"$cur\"\"\n" );
    oyjlStr_Add( s, "    case \"$cur\" in\n" );
    for(i = 0; i < n; ++i)
    {
      const char *value_type, *option, *o;
      val = oyjlTreeGetValueF( root, 0, "org/freedesktop/oyjl/ui/options/array/[%d]", i );
      v = oyjlTreeGetValue( val, 0, "value_type" ); value_type = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "option" ); option = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "o" ); o = OYJL_GET_STRING(v);
      if(option && strstr(option, "man-") != NULL)
        continue;
      if(!o)
        continue;
      if(strcmp(o,"@") == 0)
        found_at_arg_mode = 1;
      if(strcmp(o,"#") == 0)
        continue;
      if(value_type && strcmp(value_type, "oyjlOPTIONTYPE_FUNCTION") == 0)
      {
        const char * getChoices;
        int use_getChoicesCompletionBash = 0;
        v = oyjlTreeGetValue( val, 0, "values/getChoicesCompletionBash" ); getChoices = OYJL_GET_STRING(v);
        if(getChoices)
        {
          use_getChoicesCompletionBash = 1;
          if(*oyjl_debug)
            oyjlMessage_p( oyjlMSG_INFO, 0, "found getChoicesCompletionBash: `%s` for -%s", getChoices, oyjlTermColor( oyjlITALIC, o ) );
        } else
        {
          v = oyjlTreeGetValue( val, 0, "values/getChoices" ); getChoices = OYJL_GET_STRING(v);
        }
        if(!getChoices && use_getChoicesCompletionBash)
          continue;
        if(strcmp(o,"@") == 0)
          found_at_arg_func = getChoices;
        else
        {
          if(*oyjl_debug)
            oyjlMessage_p( oyjlMSG_INFO, 0, "found getChoicesCompletionBash: `%s` for -%s", getChoices, oyjlTermColor( oyjlITALIC, o ) );
          oyjlStr_Add( s, "        -%s=*) # single letter option with dynamic args\n", o );
          if(use_getChoicesCompletionBash)
            oyjlStr_Add( s, "            local OYJL_TEXTS=$(%s)\n", getChoices );
          else
          {
            oyjlStr_Add( s, "            local OYJL_TEXTS\n" );
            oyjlStr_Add( s, "            if [[ \"${COMP_WORDS[COMP_CWORD]}\" == \"=\" ]]; then\n" );
            oyjlStr_Add( s, "              OYJL_TEXTS=$(${COMP_LINE}oyjl-list | sed 's/\\[/_/g;s/\\]/_/g')\n" );
            oyjlStr_Add( s, "            else\n" );
            oyjlStr_Add( s, "              OYJL_TEXTS=$(${COMP_LINE} -%s=oyjl-list | sed 's/\\[/_/g;s/\\]/_/g')\n", o );
            oyjlStr_Add( s, "            fi\n" );
          }
          oyjlStr_Add( s, "            local IFS=$'\\n'\n" );
          oyjlStr_Add( s, "            local WORD_LIST=()\n" );
          oyjlStr_Add( s, "            for OYJL_TEXT in $OYJL_TEXTS\n" );
          oyjlStr_Add( s, "              do WORD_LIST=(\"${WORD_LIST[@]}\" \"$OYJL_TEXT\")\n" );
          oyjlStr_Add( s, "            done\n" );
          oyjlStr_Add( s, "            COMPREPLY=($(compgen -W '\"${WORD_LIST[@]}\"' -- \"$SEARCH\"))\n" );
          oyjlStr_Add( s, "            set +x +v\n" );
          oyjlStr_Add( s, "            return\n" );
          oyjlStr_Add( s, "            ;;\n" );
        }
      }
      if(strcmp(o,"@") == 0)
        continue;
      if(value_type && strcmp(value_type, "oyjlOPTIONTYPE_CHOICE") == 0)
      {
        const char *nick;
        int count = oyjlValueCount( oyjlTreeGetValue( val, 0, "values/choices/list" ) ), j;
        if(count)
        {
          oyjlStr_Add( s, "        -%s=*) # single letter option with static args\n", o );
          oyjlStr_Add( s, "            local IFS=$'\\n'\n" );
          oyjlStr_Add( s, "            local WORD_LIST=(" );
        }
        for(j = 0; j < count; ++j)
        {
          oyjl_val c = oyjlTreeGetValueF( val, 0, "values/choices/list/[%d]", j );
          v = oyjlTreeGetValue( c, 0, "nick" ); nick = OYJL_GET_STRING(v);
          oyjlStr_Add( s, "%s'%s'", j?" ":"", nick );
        }
        if(count)
        {
          oyjlStr_Add( s, ")\n" );
          oyjlStr_Add( s, "            COMPREPLY=($(compgen -W '\"${WORD_LIST[@]}\"' -- \"$SEARCH\"))\n" );
          oyjlStr_Add( s, "            set +x +v\n" );
          oyjlStr_Add( s, "            return\n" );
          oyjlStr_Add( s, "            ;;\n" );
        }
      }
    }
    oyjlStr_Add( s, "    esac\n" );
    oyjlStr_Add( s, "\n" );
    oyjlStr_Add( s, "\n" );
    oyjlStr_Add( s, "    : \"autocomplete options \"$cur\"\"\n" );
    oyjlStr_Add( s, "    case \"$cur\" in\n" );
    int found = 0;
    for(i = 0; i < n; ++i)
    {
      const char *value_type, *option, *o;
      val = oyjlTreeGetValueF( root, 0, "org/freedesktop/oyjl/ui/options/array/[%d]", i );
      v = oyjlTreeGetValue( val, 0, "value_type" ); value_type = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "option" ); option = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "o" ); o = OYJL_GET_STRING(v);
      if(option && strstr(option, "man-") != NULL)
        continue;
      if(!o)
        continue;
      if(strcmp(o,"#") == 0 || strcmp(o,"@") == 0)
        continue;
      if(value_type && (strcmp(value_type, "oyjlOPTIONTYPE_CHOICE") == 0 ||
                        strcmp(value_type, "oyjlOPTIONTYPE_FUNCTION") == 0 ||
                        strcmp(value_type, "oyjlOPTIONTYPE_DOUBLE") == 0)
        )
      {
        if(found == 0)
          oyjlStr_Add( s, "        " );
        oyjlStr_Add( s, "%s-%s", found?"|":"", o );
        ++found;
      }
      else
      if(value_type && strcmp(value_type, "oyjlOPTIONTYPE_NONE") == 0)
      {
      }
    }
    if(found)
    {
      oyjlStr_Add( s, ")\n" );
      oyjlStr_Add( s, "            : \"finish short options with choices\"\n" );
      oyjlStr_Add( s, "            COMPREPLY=(\"$cur=\\\"\")\n" );
      oyjlStr_Add( s, "            set +x +v\n" );
      oyjlStr_Add( s, "            return\n" );
      oyjlStr_Add( s, "            ;;\n" );
    }
    found = 0;
    for(i = 0; i < n; ++i)
    {
      const char *value_type, *option;
      val = oyjlTreeGetValueF( root, 0, "org/freedesktop/oyjl/ui/options/array/[%d]", i );
      v = oyjlTreeGetValue( val, 0, "value_type" ); value_type = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "option" ); option = OYJL_GET_STRING(v);
      if(option && strstr(option, "man-") != NULL)
        continue;
      if(!(option && option[0]))
        continue;
      if(value_type && (strcmp(value_type, "oyjlOPTIONTYPE_CHOICE") == 0 ||
                        strcmp(value_type, "oyjlOPTIONTYPE_FUNCTION") == 0 ||
                        strcmp(value_type, "oyjlOPTIONTYPE_DOUBLE") == 0)
        )
      {
        if(found == 0)
          oyjlStr_Add( s, "        " );
        oyjlStr_Add( s, "%s--%s", found?"|":"", option );
        ++found;
      }
      else
      if(value_type && strcmp(value_type, "oyjlOPTIONTYPE_NONE") == 0)
      {
      }
    }
    if(found)
    {
      oyjlStr_Add( s, ")\n" );
      oyjlStr_Add( s, "            : \"finish long options with choices\"\n" );
      oyjlStr_Add( s, "            COMPREPLY=(\"$cur=\\\"\")\n" );
      oyjlStr_Add( s, "            set +x +v\n" );
      oyjlStr_Add( s, "            return\n" );
      oyjlStr_Add( s, "            ;;\n" );
    }
    found = 0;
    for(i = 0; i < n; ++i)
    {
      const char *value_type, *option, *o;
      val = oyjlTreeGetValueF( root, 0, "org/freedesktop/oyjl/ui/options/array/[%d]", i );
      v = oyjlTreeGetValue( val, 0, "value_type" ); value_type = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "option" ); option = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "o" ); o = OYJL_GET_STRING(v);
      if(option && strstr(option, "man-") != NULL)
        continue;
      if(!o)
        continue;
      if(strcmp(o,"#") == 0 || strcmp(o,"@") == 0)
        continue;
      if(value_type && strcmp(value_type, "oyjlOPTIONTYPE_CHOICE") == 0)
      {
      }
      else
      if(value_type && strcmp(value_type, "oyjlOPTIONTYPE_NONE") == 0)
      {
        if(found == 0)
          oyjlStr_Add( s, "        " );
        oyjlStr_Add( s, "%s-%s", found?"|":"", o );
        ++found;
      }
    }
    if(found)
    {
      oyjlStr_Add( s, ")\n" );
      oyjlStr_Add( s, "            : \"finish short options without choices\"\n" );
      oyjlStr_Add( s, "            COMPREPLY=(\"$cur \")\n" );
      oyjlStr_Add( s, "            set +x +v\n" );
      oyjlStr_Add( s, "            return\n" );
      oyjlStr_Add( s, "            ;;\n" );
    }
    found = 0;
    for(i = 0; i < n; ++i)
    {
      const char *value_type, *option;
      val = oyjlTreeGetValueF( root, 0, "org/freedesktop/oyjl/ui/options/array/[%d]", i );
      v = oyjlTreeGetValue( val, 0, "value_type" ); value_type = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "option" ); option = OYJL_GET_STRING(v);
      if(option && strstr(option, "man-") != NULL)
        continue;
      if(!(option && option[0]))
        continue;
      if(value_type && strcmp(value_type, "oyjlOPTIONTYPE_CHOICE") == 0)
      {
      }
      else
      if(value_type && strcmp(value_type, "oyjlOPTIONTYPE_NONE") == 0)
      {
        if(found == 0)
          oyjlStr_Add( s, "        " );
        oyjlStr_Add( s, "%s--%s", found?"|":"", option );
        ++found;
      }
    }
    if(found)
    {
      oyjlStr_Add( s, ")\n" );
      oyjlStr_Add( s, "            : \"finish long options without choices\"\n" );
      oyjlStr_Add( s, "            COMPREPLY=(\"$cur \")\n" );
      oyjlStr_Add( s, "            set +x +v\n" );
      oyjlStr_Add( s, "            return\n" );
      oyjlStr_Add( s, "            ;;\n" );
    }
    found = 0;
    oyjlStr_Add( s, "    esac\n" );
    oyjlStr_Add( s, "\n" );
    oyjlStr_Add( s, "\n" );
    oyjlStr_Add( s, "    : \"show help for none '@' UIs\"\n" );
    oyjlStr_Add( s, "    if [[ \"$cur\" == \"\" ]]; then\n" );
    oyjlStr_Add( s, "      if [[ ${COMP_WORDS[1]} == \"\" ]]; then\n" );
    oyjlStr_Add( s, "        $1 help synopsis 1>&2\n" );
    oyjlStr_Add( s, "      else\n" );
    oyjlStr_Add( s, "        $1 help ${COMP_WORDS[1]} 1>&2\n" );
    oyjlStr_Add( s, "      fi\n" );
    oyjlStr_Add( s, "    fi\n" );
    oyjlStr_Add( s, "\n" );
    oyjlStr_Add( s, "\n" );
    oyjlStr_Add( s, "    : \"suggest group options for subcommands\"\n" );
    oyjlStr_Add( s, "    if [[ \"$cur\" == \"\" ]] || [[ \"$cur\" == \"-\" ]] || [[ \"$cur\" == -- ]] || [[ \"$cur\" == -* ]]; then\n" );
    oyjlStr_Add( s, "      case \"${COMP_WORDS[1]}\" in\n" );
#define WANT_ARG(x) (x->value_type == oyjlOPTIONTYPE_CHOICE || x->value_type == oyjlOPTIONTYPE_FUNCTION || x->value_type == oyjlOPTIONTYPE_DOUBLE)
    n = oyjlValueCount( oyjlTreeGetValue( root, 0, "org/freedesktop/oyjl/ui/options/groups" ) );
    for(i = 0; i < n; ++i)
    {
      const char *mandatory, *optional;
      int mandatory_n = 0, optional_n = 0, j, flg, sub = 0;
      char **mandatory_list, **optional_list;
      val = oyjlTreeGetValueF( root, 0, "org/freedesktop/oyjl/ui/options/groups/[%d]", i );
      v = oyjlTreeGetValue( val, 0, "mandatory" ); mandatory = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "optional" ); optional = OYJL_GET_STRING(v);
      if(!(mandatory && mandatory[0]) || !(optional && optional[0]))
        continue;
      if(strcmp(mandatory,"@") == 0 || strcmp(mandatory,"#") == 0)
        continue;
      v = oyjlTreeGetValue( val, 0, "flags" ); flg = OYJL_IS_INTEGER(v) ? OYJL_GET_INTEGER(v) : 0;
      if(flg & OYJL_GROUP_FLAG_SUBCOMMAND)
        sub = 1;
      mandatory_list = oyjlStringSplit2( mandatory, "|,", &mandatory_n, NULL, malloc );
      optional_list = oyjlStringSplit2( optional, "|,", &optional_n, NULL, malloc );
      oyjlStr_Add( s, "        " );
      found = 0;
      for(j = 0; j < mandatory_n; ++j)
      {
        const char * moption = mandatory_list[j];
        opt = oyjlOptions_GetOptionL( ui->opts, moption, 0 );
        if(opt->o && (strcmp(opt->o,"#") == 0 || strcmp(opt->o,"@") == 0))
          continue;
        oyjlStr_Add( s, "%s%s%s%s%s%s", found?"|":"", opt->o?"-":"", opt->o?opt->o:"", (opt->o && opt->option)?"|":"", (opt->option && !sub)?"--":"", opt->option?opt->option:"" );
        ++found;
      }
      if(found)
      {
        oyjlStr_Add( s, ")\n" );
        oyjlStr_Add( s, "          COMPREPLY=($(compgen -W '" );
        found = 0;
        for(j = 0; j < optional_n; ++j)
        {
          const char * ooption = optional_list[j];
          opt = oyjlOptions_GetOptionL( ui->opts, ooption, 0 );
          oyjlStr_Add( s, "%s%s%s%s%s%s%s%s", found?" ":"", opt->o?"-":"", opt->o?opt->o:"", (opt->o && WANT_ARG(opt))?"=":"", opt->o?" ":"", opt->option?"--":"", opt->option?opt->option:"", (opt->option && WANT_ARG(opt))?"=":"" );
          ++found;
        }
        oyjlStr_Add( s, "' -- \"$cur\"))\n" );
        oyjlStr_Add( s, "            set +x +v\n" );
        oyjlStr_Add( s, "            return\n" );
        oyjlStr_Add( s, "            ;;\n" );
      }
      oyjlStringListRelease( &mandatory_list, mandatory_n, free );
      oyjlStringListRelease( &optional_list, optional_n, free );
    }
    if(found_at_arg_mode) /* put '@)' at the end of cases */
    for(i = 0; i < n; ++i)
    {
      const char *mandatory, *optional;
      int mandatory_n = 0, optional_n = 0, j;
      char **mandatory_list, **optional_list;
      val = oyjlTreeGetValueF( root, 0, "org/freedesktop/oyjl/ui/options/groups/[%d]", i );
      v = oyjlTreeGetValue( val, 0, "mandatory" ); mandatory = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "optional" ); optional = OYJL_GET_STRING(v);
      if(!(mandatory && mandatory[0]) || !(optional && optional[0]))
        continue;
      if(strcmp(mandatory,"@") != 0)
        continue;
      mandatory_list = oyjlStringSplit2( mandatory, "|,", &mandatory_n, NULL, malloc );
      optional_list = oyjlStringSplit2( optional, "|,", &optional_n, NULL, malloc );
      oyjlStr_Add( s, "        " );
      oyjlStr_Add( s, ".*)\n" );
      oyjlStr_Add( s, "          COMPREPLY=($(compgen -W '" );
      found = 0;
      for(j = 0; j < optional_n; ++j)
      {
        const char * ooption = optional_list[j];
        opt = oyjlOptions_GetOptionL( ui->opts, ooption, 0 );
        oyjlStr_Add( s, "%s%s%s%s%s%s%s%s", found?" ":"", opt->o?"-":"", opt->o?opt->o:"", (opt->o && WANT_ARG(opt))?"=":"", opt->o?" ":"", opt->option?"--":"", opt->option?opt->option:"", (opt->option && WANT_ARG(opt))?"=":"" );
        ++found;
      }
      oyjlStr_Add( s, "' -- \"$cur\"))\n" );
      oyjlStr_Add( s, "            set +x +v\n" );
      oyjlStr_Add( s, "            return\n" );
      oyjlStr_Add( s, "            ;;\n" );
      oyjlStringListRelease( &mandatory_list, mandatory_n, free );
      oyjlStringListRelease( &optional_list, optional_n, free );
    }
    oyjlStr_Add( s, "      esac\n" );
    oyjlStr_Add( s, "    fi\n" );
    oyjlStr_Add( s, "\n" );
    oyjlStr_Add( s, "    : \"suggest mandatory options on first args only\"\n" );
    oyjlStr_Add( s, "    if [[ \"${COMP_WORDS[2]}\" == \"\" ]]; then\n" );
    oyjlStr_Add( s, "      local WORD_LIST=()\n" );
    found = 0;
    if(found_at_arg_func)
    {
        oyjlStr_Add( s, "      local OYJL_TEXTS=$(%s)\n", found_at_arg_func );
        oyjlStr_Add( s, "      local IFS=$'\\n'\n" );
        oyjlStr_Add( s, "      for OYJL_TEXT in $OYJL_TEXTS\n" );
        oyjlStr_Add( s, "        do WORD_LIST=(\"${WORD_LIST[@]}\"\n\"$OYJL_TEXT\")\n" );
        oyjlStr_Add( s, "      done\n" );
    }
    if(n)
      oyjlStr_Add( s, "      WORD_LIST=(\"${WORD_LIST[@]}\" " );
    for(i = 0; i < n; ++i)
    {
      const char *mandatory;
      int mandatory_n = 0, j, flg, sub = 0;
      char **mandatory_list;
      val = oyjlTreeGetValueF( root, 0, "org/freedesktop/oyjl/ui/options/groups/[%d]", i );
      v = oyjlTreeGetValue( val, 0, "mandatory" ); mandatory = OYJL_GET_STRING(v);
      if(!(mandatory && mandatory[0]))
        continue;
      v = oyjlTreeGetValue( val, 0, "flags" ); flg = OYJL_IS_INTEGER(v) ? OYJL_GET_INTEGER(v) : 0;
      if(flg & OYJL_GROUP_FLAG_SUBCOMMAND)
        sub = 1;
      mandatory_list = oyjlStringSplit2( mandatory, "|,", &mandatory_n, NULL, malloc );
      for(j = 0; j < mandatory_n; ++j)
      {
        const char * moption = mandatory_list[j];
        opt = oyjlOptions_GetOptionL( ui->opts, moption, 0 );
        if(opt->o && (strcmp(opt->o,"#") == 0 || strcmp(opt->o,"@") == 0))
          continue;
        oyjlStr_Add( s, "%s%s%s%s%s%s%s%s", found?" ":"", opt->o?"-":"", opt->o?opt->o:"", (opt->o && WANT_ARG(opt))?"=":"", opt->o?" ":"", (opt->option && !sub)?"--":"", opt->option?opt->option:"", (opt->option && WANT_ARG(opt))?"=":"" );
        ++found;
      }
      oyjlStringListRelease( &mandatory_list, mandatory_n, free );
    }
    if(n)
    {
      oyjlStr_Add( s, ")\n" );
    }
#undef WANT_ARG
    oyjlStr_Add( s, "      COMPREPLY=($(compgen -W '\"${WORD_LIST[@]}\"' -- \"$cur\"))\n" );
    oyjlStr_Add( s, "      set +x +v\n" );
    oyjlStr_Add( s, "      return\n" );
    oyjlStr_Add( s, "    fi\n" );
    oyjlStr_Add( s, "\n" );
    oyjlStr_Add( s, "    set +x +v\n" );
    oyjlStr_Add( s, "} &&\n" );
    oyjlStr_Add( s, "complete -o nosort -F _%s -o nospace %s\n", func, nick_ );
    oyjlStr_Add( s, "\n" );
    free(func);
    oyjlUi_Release( &ui );
  }
  else
    fputs( "can only generate C code, or bash completion", stderr );

  c = oyjlStr_Pull( s );
  oyjlStr_Release( &s );
  return c;
}

/** @brief    Return a JSON representation from options
 *  @memberof oyjlUi_s
 *
 *  The JSON data shall be useable with oyjl-args-qml options renderer and
 *  the -R option.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2021/11/14
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
char *       oyjlUi_ToJson           ( oyjlUi_s          * ui,
                                       int                 flags OYJL_UNUSED )
{
  char * t = NULL, num[64];
  oyjl_val root, key;
  int i,j,k,n,ng,nopts;
  oyjlOptions_s * opts;

  if(!ui) return t;

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
    char ** d_list = oyjlStringSplit2( g->detail, "|,", &d, NULL, malloc );
    for(j = 0; j < d; ++j)
    {
      char * option = d_list[j];
      oyjlOption_s * o = oyjlOptions_GetOptionL( opts, option, 0 );
      int mandatory_index;
      if(!o) continue;
      mandatory_index = oyjlOptionMandatoryIndex_( o, g );
      num[0] = '\000';
      if(!o->key)
        sprintf(num, "%s", OYJL_E(option));
      if(sub_command && mandatory_index == 0)
        sprintf(num, "%s", OYJL_E(o->option));
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
                oyjlTreeFree( root );
                if(t) { free(t); t = NULL; }
                oyjlStringAdd( &t, malloc, free, "Option '%c' has no choices but is not editable (flag&OYJL_OPTION_FLAG_EDITABLE)", o->o );
                return t;
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
    }
    oyjlStringListRelease( &aresults, arn, free );
    oyjlStringListRelease( &d_list, d, free );
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

  i = 0;
  oyjlTreeToJson( root, &i, &t );
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
 *  @param[int]    context            the translation context; optional, without will use gettext
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
    { "oihs", "version", NULL, OYJL_VERSION_NAME, NULL },
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

