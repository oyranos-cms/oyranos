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
    oyjlTreeSetDoubleF( root, OYJL_CREATE_NEW, o->flags, OYJL_REG "/ui/options/array/[%d]/%s", i, "flags" );
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
          oyjl_str tmp = oyjlStrNew(10,0,0);
          if(OYJL_IS_O("@"))
            oyjlStrAppendN( tmp, "base", 4 );
          else if(o->option)
            oyjlStrAppendN( tmp, o->option, strlen(o->option) );
          oyjlStrAdd( tmp, "GetChoices" );
          oyjlStrReplace( tmp, "-", "_", 0, NULL );
          oyjlStrReplace( tmp, "+", "_plus_", 0, NULL );
          txt = oyjlStr(tmp); 
          oyjlTreeSetStringF( root, OYJL_CREATE_NEW, o->values.getChoices ? txt:NULL, OYJL_REG "/ui/options/array/[%d]/values/%s", i, "getChoices" );
          oyjlStrRelease( &tmp );
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
    oyjlTreeSetDoubleF( root, OYJL_CREATE_NEW, g->flags, OYJL_REG "/ui/options/groups/[%d]/%s", i, "flags" );
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
    char ** attr_paths = NULL;
    oyjl_val v;
    n = 0;
    oyjlTreeToPaths( attr, 0, NULL, 0, &attr_paths );
    while(attr_paths && attr_paths[n]) ++n;
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
      char ** attr_paths = NULL;
      oyjl_val attr = oyjlTreeNew( "" );
      n = 0;
      oyjlTreeToPaths( val, 0, NULL, 0, &attr_paths );
      while(attr_paths && attr_paths[n]) ++n;
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
#define OYJL_QUOTE     0x02
#define OYJL_SQUOTE    0x04
#define OYJL_LAST      0x08
static void oyjlStrAddSpaced_( oyjl_str s, const char * text, int flags, int space )
{
  int len = 0, i,n;

  if(text) len = strlen(text);
  if(text && (flags & OYJL_TRANSLATE)) len += 2+1;
  if(text && (flags & OYJL_QUOTE))     len += 1+1;
  if(text && (flags & OYJL_SQUOTE))    len += 1+1;

  if(len)
  {
    if(flags & OYJL_TRANSLATE && text && text[0])
      oyjlStrAdd( s, "_(" );
    if(flags & OYJL_QUOTE)
      oyjlStrAdd( s, "\"" );
    if(flags & OYJL_SQUOTE)
      oyjlStrAdd( s, "'" );
    if(flags & OYJL_QUOTE)
    {
      const char * t = text;
      oyjl_str tmp = oyjlStrNew(10,0,0);
      oyjlStrAppendN( tmp, t, strlen(t) );
      oyjlStrReplace( tmp, "\\", "\\\\", 0, NULL );
      oyjlStrReplace( tmp, "\"", "\\\"", 0, NULL );
      oyjlStrReplace( tmp, "\b", "\\b", 0, NULL );
      oyjlStrReplace( tmp, "\f", "\\f", 0, NULL );
      oyjlStrReplace( tmp, "\n", "\\n", 0, NULL );
      oyjlStrReplace( tmp, "\r", "\\r", 0, NULL );
      oyjlStrReplace( tmp, "\t", "\\t", 0, NULL );
      t = oyjlStr(tmp); 
      oyjlStrAdd( s, "%s", t );
      oyjlStrRelease( &tmp );
    }
    else
      oyjlStrAdd( s, "%s", (text && strlen(text) == 0 && flags & OYJL_SQUOTE) ? "\\000" : text );
    if(flags & OYJL_SQUOTE)
      oyjlStrAdd( s, "'" );
    if(flags & OYJL_QUOTE)
      oyjlStrAdd( s, "\"" );
    if(flags & OYJL_TRANSLATE && text && text[0])
      oyjlStrAdd( s, ")" );
  } else
  {
    len = 4;
      oyjlStrAdd( s, "NULL" );
  }

  if(!(flags & OYJL_LAST))
  {
      ++len;
      oyjlStrAdd( s, "," );
  }

  n = space - len;
  if(n > 0)
    for(i = 0; i < n; ++i)
      oyjlStrAdd( s, " " );
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
  tmp = oyjlStrNew(10,0,0);
  oyjlStrAppendN( tmp, *name, strlen(*name) );
  oyjlStrReplace( tmp, "-", "_", 0, NULL );
  oyjlStrReplace( tmp, "+", "_plus_", 0, NULL );
  oyjlStrReplace( tmp, "=", "_", 0, NULL );
  oyjlStrReplace( tmp, "(", "_", 0, NULL );
  oyjlStrReplace( tmp, "|", "_", 0, NULL );
  oyjlStrReplace( tmp, ".", "_", 0, NULL );
  oyjlStrReplace( tmp, "?", "_", 0, NULL );
  txt = oyjlStr(tmp);
  free(*name); *name = NULL;
  i = 0;
  while(txt[i] && txt[i] == '_') ++i;
  oyjlStringAdd( name, 0,0, "%s", &txt[i] );
  oyjlStrRelease( &tmp );
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
 *  The input is the JSON data from oyjlUi_ExportToJson().
 *
 *  @param[in]     root                the parsed JSON tree to convert
 *  @param[in]     flags               ::OYJL_SOURCE_CODE_C, ::OYJL_SUGGEST_VARIABLE_NAMES,
 *                                     ::OYJL_NO_DEFAULT_OPTIONS ::OYJL_COMPLETION_BASH are supported
 *
 *  @version Oyjl: 1.0.0
 *  @date    2020/10/11
 *  @since   2019/06/24 (Oyjl: 1.0.0)
 */
char *             oyjlUiJsonToCode  ( oyjl_val            root,
                                       int                 flags )
{
  char * c = NULL;
  oyjl_str s = oyjlStrNew( 0, 0,0 );
  if(flags & OYJL_SOURCE_CODE_C)
  {
    oyjl_val val, v;
    char * app_type;
    int i,n, X_found = 0, export_found = 0, h_found = 0, help_found = 0, v_found = 0, v_is_string = 0, verbose_found = 0, version_found = 0, render_found = 0;

    val = oyjlTreeGetValue( root, 0, OYJL_REG "/ui/app_type" ); app_type = OYJL_GET_STRING(val);
    oyjlStrAdd( s, "#include \"oyjl.h\"\n" );
    oyjlStrAdd( s, "#include \"oyjl_version.h\"\n" );
    oyjlStrAdd( s, "extern char **environ;\n" );
    oyjlStrAdd( s, "#ifdef OYJL_HAVE_LOCALE_H\n" );
    oyjlStrAdd( s, "#include <locale.h>\n" );
    oyjlStrAdd( s, "#endif\n" );
    oyjlStrAdd( s, "#ifdef OYJL_USE_GETTEXT\n" );
    oyjlStrAdd( s, "# ifdef OYJL_HAVE_LIBINTL_H\n" );
    oyjlStrAdd( s, "#  include <libintl.h> /* bindtextdomain() */\n" );
    oyjlStrAdd( s, "# endif\n" );
    oyjlStrAdd( s, "# define _(text) dgettext( OYJL_DOMAIN, text )\n" );
    oyjlStrAdd( s, "#else\n" );
    oyjlStrAdd( s, "# define _(text) text\n" );
    oyjlStrAdd( s, "#endif\n" );
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
        oyjlStrAdd( s, "oyjlOptionChoice_s * %s( oyjlOption_s * opt OYJL_UNUSED, int * selected OYJL_UNUSED, oyjlOptions_s * context OYJL_UNUSED )\n{ fprintf(stderr, \"\\t%%s()\\n\", __func__); return NULL; }\n", getChoices );
        if(tmp) free(tmp);
      }
    }
    oyjlStrAdd( s, "\n" );
    oyjlStrAdd( s, "/* This function is called the\n" );
    oyjlStrAdd( s, " * * first time for GUI generation and then\n" );
    oyjlStrAdd( s, " * * for executing the tool.\n" );
    oyjlStrAdd( s, " */\n" );
    oyjlStrAdd( s, "int myMain( int argc, const char ** argv )\n" );
    oyjlStrAdd( s, "{\n" );
    oyjlStrAdd( s, "  int error = 0;\n" );
    if(!(app_type && strcmp(app_type,"tool") == 0))
      oyjlStrAdd( s, "  int state = oyjlUI_STATE_NO_CHECKS;\n" );
    else
      oyjlStrAdd( s, "  int state = 0;\n" );

    n = oyjlValueCount( oyjlTreeGetValue( root, 0, "org/freedesktop/oyjl/ui/options/array" ) );
    for(i = 0; i < n; ++i)
    {
      const char * type = NULL;
      char * t;
      val = oyjlTreeGetValueF( root, 0, "org/freedesktop/oyjl/ui/options/array/[%d]", i );
      t = oyjlUiGetVariableNameC_( val, &type );
      if(type && t)
      {
        oyjlStrAdd( s, "  %s %s = 0;\n", type, t );
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
        oyjlStrAdd( s, "  const char * help = NULL;\n" );
      if(!verbose_found)
        oyjlStrAdd( s, "  int verbose = 0;\n" );
      if(!version_found)
        oyjlStrAdd( s, "  int version = 0;\n" );
      if(!render_found)
        oyjlStrAdd( s, "  const char * render = NULL;\n" );
    }
    if(!export_found)
    oyjlStrAdd( s, "  const char * export_var = 0;\n" );
    oyjlStrAdd( s, "\n" );
    oyjlStrAdd( s, "  /* handle options */\n" );
    oyjlStrAdd( s, "  /* Select a nick from *version*, *manufacturer*, *copyright*, *license*,\n" );
    oyjlStrAdd( s, "   * *url*, *support*, *download*, *sources*, *oyjl_module_author* and\n" );
    oyjlStrAdd( s, "   * *documentation*. Choose what you see fit. Add new ones as needed. */\n" );
    oyjlStrAdd( s, "  oyjlUiHeaderSection_s sections[] = {\n" );
    oyjlStrAdd( s, "    /* type, " );
    oyjlStrAddSpaced_( s, "nick",  0, 17 );
    oyjlStrAddSpaced_( s, "label", 0, 7 );
    oyjlStrAddSpaced_( s, "name",  0, 26 );
    oyjlStrAdd(       s, "description */\n" );
    n = oyjlValueCount( oyjlTreeGetValue( root, 0, "org/freedesktop/oyjl/ui/header/sections" ) );
    for(i = 0; i < n; ++i)
    {
      const char *nick, *label, *name, *desc;
      val = oyjlTreeGetValueF( root, 0, "org/freedesktop/oyjl/ui/header/sections/[%d]", i );
      v = oyjlTreeGetValue( val, 0, "nick" ); nick = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "label" ); label = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "name" ); name = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "description" ); desc = OYJL_GET_STRING(v);
      oyjlStrAdd( s, "    {\"oihs\", ");

      oyjlStrAddSpaced_( s, nick,  OYJL_QUOTE,                17 );
      oyjlStrAddSpaced_( s, label, OYJL_QUOTE|OYJL_TRANSLATE, 7 );
      oyjlStrAddSpaced_( s, name&&name[0]?name:NULL,  OYJL_QUOTE|OYJL_TRANSLATE, 26 );
      oyjlStrAddSpaced_( s, desc,  OYJL_QUOTE|OYJL_TRANSLATE|OYJL_LAST, 4 );
      oyjlStrAdd( s, "},\n");
    }
    oyjlStrAdd( s, "    {\"\",0,0,0,0}};\n" );
    oyjlStrAdd( s, "\n" );
    oyjlStrAdd( s,       "  /* declare the option choices  *   " );
    oyjlStrAddSpaced_( s, "nick",         0, 15 );
    oyjlStrAddSpaced_( s, "name",         0, 20 );
    oyjlStrAddSpaced_( s, "description",  0, 30 );
    oyjlStrAddSpaced_( s, "help",         OYJL_LAST, 4 );
    oyjlStrAdd( s,       " */\n" );
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
          oyjlStrAdd( s, "  oyjlOptionChoice_s %s_choices[] = {", o_fallback );
        for(j = 0; j < count; ++j)
        {
          oyjl_val c = oyjlTreeGetValueF( val, 0, "values/choices/list/[%d]", j );
          v = oyjlTreeGetValue( c, 0, "nick" ); nick = OYJL_GET_STRING(v);
          v = oyjlTreeGetValue( c, 0, "name" ); name = OYJL_GET_STRING(v);
          v = oyjlTreeGetValue( c, 0, "description" ); desc = OYJL_GET_STRING(v);
          v = oyjlTreeGetValue( c, 0, "help" ); help = OYJL_GET_STRING(v);
          if(j)
          oyjlStrAdd( s, "                                    {");
          else
            oyjlStrAdd( s, "{");

          if(option && strstr(option, "man-") != NULL)
            oyjlStrAddSpaced_(s,nick,  OYJL_QUOTE|OYJL_TRANSLATE, 15 );
          else
            oyjlStrAddSpaced_(s,nick,  OYJL_QUOTE,                15 );
          oyjlStrAddSpaced_( s, name,  OYJL_QUOTE|OYJL_TRANSLATE, 20 );
          oyjlStrAddSpaced_( s, desc,  OYJL_QUOTE|OYJL_TRANSLATE, 30 );
          oyjlStrAddSpaced_( s, help,  OYJL_QUOTE|OYJL_TRANSLATE|OYJL_LAST, 4 );
          oyjlStrAdd( s, "},\n");
        }
        if(count)
        {
          oyjlStrAdd( s, "                                    {\"\",\"\",\"\",\"\"}};\n" );
          oyjlStrAdd( s, "\n" );
        }
      }
    }
    oyjlStrAdd( s, "  /* declare options - the core information; use previously declared choices */\n" );
    oyjlStrAdd( s, "  oyjlOption_s oarray[] = {\n" );
    oyjlStrAdd( s, "  /* type,   " );
    oyjlStrAddSpaced_( s, "flags",        0, 28 );
    oyjlStrAddSpaced_( s, "o",            0, 4 );
    oyjlStrAddSpaced_( s, "option",       0, 17 );
    oyjlStrAddSpaced_( s, "key",          0, 10 );
    oyjlStrAddSpaced_( s, "name",         0, 15 );
    oyjlStrAddSpaced_( s, "description",  0, 30 );
    oyjlStrAddSpaced_( s, "help",         0, 6 );
    oyjlStrAddSpaced_( s, "value_name",   0, 20 );
    oyjlStrAdd( s,       "\n        " );
    oyjlStrAddSpaced_( s, "value_type",   0, 25 );
    oyjlStrAddSpaced_( s, "values",       0, 20 );
    oyjlStrAddSpaced_( s, "variable_type",0, 15 );
    oyjlStrAddSpaced_( s, "variable_name",OYJL_LAST, 10 );
    oyjlStrAdd( s,       " */\n" );
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
      oyjlStrAdd( s, "    {\"oiwi\", ");
      oyjlStrAddSpaced_( s, flag_string, 0,                         28 );
      if(*oyjl_debug)
        fprintf( stderr, "o: \"%s\" / %s\n", o, option );
      if(o && o[0])
        oyjlStrAddSpaced_( s, o,         OYJL_QUOTE,                4 );
      else
        oyjlStrAddSpaced_( s, "NULL",    0,                         4 );
      oyjlStrAddSpaced_( s, option,      OYJL_QUOTE,                17 );
      oyjlStrAddSpaced_( s, key,         OYJL_QUOTE,                10 );
      oyjlStrAddSpaced_( s, name,        OYJL_QUOTE|OYJL_TRANSLATE, 15 );
      oyjlStrAddSpaced_( s, desc,        OYJL_QUOTE|OYJL_TRANSLATE, 30 );
      oyjlStrAddSpaced_( s, help,        OYJL_QUOTE|OYJL_TRANSLATE, 6 );
      oyjlStrAddSpaced_( s, value_name,  OYJL_QUOTE|OYJL_TRANSLATE, 20 );
      oyjlStrAdd( s,       "\n        " );
      oyjlStrAddSpaced_( s, value_type && value_type[0] ? value_type : "oyjlOPTIONTYPE_NONE",  0,                         25 );
      if(value_type && strcmp(value_type, "oyjlOPTIONTYPE_CHOICE") == 0)
      {
        int count = oyjlValueCount( oyjlTreeGetValue( val, 0, "values/choices/list" ) );
        if(count)
          oyjlStrAdd( s,   "{.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)%s_choices, sizeof(%s_choices), malloc ), 0}}, ", o,o );
        else
          oyjlStrAddSpaced_(s,"{0}",        0, 20 );
      } else
      if(value_type && strcmp(value_type, "oyjlOPTIONTYPE_FUNCTION") == 0)
      {
        const char * getChoices;
        int j;
        v = oyjlTreeGetValue( val, 0, "values/getChoices" ); getChoices = OYJL_GET_STRING(v);
        if(getChoices)
        oyjlStrAdd( s,     "{.getChoices = %s}, ", getChoices );
        else
        oyjlStrAdd( s,     "{0},                " );
        if(getChoices && strlen(getChoices) < 2)
        {
          int len = strlen(getChoices);
          for(j = 0; j < len - 18; ++j)
            oyjlStrAdd( s, " " );
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
          oyjlStrAdd( s,   "{.dbl = {.d = %g, .start = %g, .end = %g, .tick = %g}}, ", d == -1.0 ? 0 : d, start, end, tick == -1.0 ? 0.0 : tick );
        else
          oyjlStrAdd( s,   "{0}, " );
      } else
      if(!value_type || (value_type && !value_type[0]) || (value_type && strcmp(value_type, "oyjlOPTIONTYPE_NONE") == 0))
      {
        oyjlStrAddSpaced_(s,"{0}",        0, 20 );
      }
      oyjlStrAddSpaced_( s, variable_type,0,                         15 );
      if(variable_type && strcmp(variable_type, "oyjlNONE") == 0)
        oyjlStrAddSpaced_( s, variable_name?variable_name:"{}",OYJL_LAST,                 2 );
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
        oyjlStrAddSpaced_( s, t,OYJL_LAST,                 2 );
        if(t) free( t );
      }
      
      oyjlStrAdd( s, "},\n");
      if(flag_string) free( flag_string );
      if(tmp_name) free( tmp_name );
    }
    if(!(flags & OYJL_NO_DEFAULT_OPTIONS))
    {
      if(!help_found && !oyjlFindOption_( root, 'h' ))
      oyjlStrAdd(s,"    /* default options -h and -v */\n" );
      if(!help_found && !oyjlFindOption_( root, 'h' ))
      oyjlStrAdd(s,"    {\"oiwi\", OYJL_OPTION_FLAG_ACCEPT_NO_ARG, \"h\", \"help\", NULL, NULL, NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s=&help} },\n" );
      if(!verbose_found && !oyjlFindOption_( root, 'v' ))
      oyjlStrAdd(s,"    {\"oiwi\", 0, \"v\", \"verbose\", NULL, _(\"verbose\"), _(\"Verbose\"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&verbose} },\n" );
      if(!render_found && !oyjlFindOption_( root, 'R' ))
      {
      oyjlStrAdd(s,"    /* The --render option can be hidden and used only internally. */\n" );
      oyjlStrAdd(s,"    {\"oiwi\", OYJL_OPTION_FLAG_EDITABLE|OYJL_OPTION_FLAG_MAINTENANCE, \"R\", \"render\",  NULL, _(\"render\"),  _(\"Render\"),  NULL, NULL, oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s=&render} },\n" );
      }
      if(!version_found && !oyjlFindOption_( root, 'V' ))
      oyjlStrAdd(s,"    {\"oiwi\", 0, \"V\", \"version\", NULL, _(\"version\"), _(\"Version\"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&version} },\n" );
      oyjlStrAdd(s,"    /* default option template -X|--export */\n" );
      if(!export_found && !X_found)
      oyjlStrAdd(s,"    {\"oiwi\", 0, \"X\", \"export\", NULL, NULL, NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices = {NULL, 0}}, oyjlSTRING, {.s=&export_var} },\n" );
    }
    oyjlStrAdd( s, "    {\"\",0,0,NULL,NULL,NULL,NULL,NULL, NULL, oyjlOPTIONTYPE_END, {0},oyjlNONE,{0}}\n  };\n\n" );
    oyjlStrAdd( s, "  /* declare option groups, for better syntax checking and UI groups */\n" );
    oyjlStrAdd( s, "  oyjlOptionGroup_s groups[] = {\n" );
    oyjlStrAdd( s, "  /* type,   " );
    oyjlStrAddSpaced_( s, "flags",        0, 7 );
    oyjlStrAddSpaced_( s, "name",         0, 20 );
    oyjlStrAddSpaced_( s, "description",  0, 30 );
    oyjlStrAddSpaced_( s, "help",         0, 20 );
    oyjlStrAddSpaced_( s, "mandatory",    0, 15 );
    oyjlStrAddSpaced_( s, "optional",     0, 15 );
    oyjlStrAddSpaced_( s, "detail",       OYJL_LAST, 4 );
    oyjlStrAdd( s,       " */\n" );
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
      oyjlStrAdd( s, "    {\"oiwg\", ");
      v = oyjlTreeGetValue( val, 0, "flags" ); flg = OYJL_IS_INTEGER(v) ? OYJL_GET_INTEGER(v) : 0;
      if(flg & OYJL_GROUP_FLAG_SUBCOMMAND)
        oyjlStringAdd( &flag_string, 0,0, "%s", "OYJL_GROUP_FLAG_SUBCOMMAND" );
      if(flg & OYJL_GROUP_FLAG_GENERAL_OPTS)
        oyjlStringAdd( &flag_string, 0,0, "%s%s", flag_string?"|":"", "OYJL_GROUP_FLAG_GENERAL_OPTS" );
      if(!flag_string)
        flag_string = oyjlStringCopy("0",0);
      oyjlStrAddSpaced_( s, flag_string, 0,                         7 );
      oyjlStrAddSpaced_( s, name,        OYJL_QUOTE|OYJL_TRANSLATE, 20 );
      oyjlStrAddSpaced_( s, desc,        OYJL_QUOTE|OYJL_TRANSLATE, 30 );
      oyjlStrAddSpaced_( s, help,        OYJL_QUOTE|OYJL_TRANSLATE, 20 );
      oyjlStrAddSpaced_( s, mandatory,   OYJL_QUOTE,                15 );
      oyjlStrAddSpaced_( s, optional,    OYJL_QUOTE,                15 );
      oyjlStrAddSpaced_( s, detail,      OYJL_QUOTE|OYJL_LAST,      4 );
      oyjlStrAdd( s, "},\n");
      if(flag_string) free( flag_string );
    }
    oyjlStrAdd( s, "    {\"\",0,0,0,0,0,0,0}\n" );
    oyjlStrAdd( s, "  };\n\n");
    const char *nick, *name, *description, *logo;
    val = oyjlTreeGetValue( root, 0, OYJL_REG "/ui/nick" ); nick = OYJL_GET_STRING(val);
    val = oyjlTreeGetValue( root, 0, OYJL_REG "/ui/name" ); name = OYJL_GET_STRING(val);
    val = oyjlTreeGetValue( root, 0, OYJL_REG "/ui/description" ); description = OYJL_GET_STRING(val);
    val = oyjlTreeGetValue( root, 0, OYJL_REG "/ui/logo" ); logo = OYJL_GET_STRING(val);
    oyjlStrAdd( s, "  oyjlUi_s * ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */\n" );
    oyjlStrAdd( s, "                                       \"%s\", ", nick );
    if(name)
      oyjlStrAdd( s, "_(\"%s\"), ", name );
    else
      oyjlStrAdd( s, "NULL, " );
    if(description)
      oyjlStrAdd( s, "_(\"%s\"),\n", description );
    else
      oyjlStrAdd( s, "NULL,\n" );
    oyjlStrAdd( s, "#ifdef __ANDROID__\n" );
    oyjlStrAdd( s, "                                       \":/images/logo.svg\", // use qrc\n" );
    oyjlStrAdd( s, "#else\n" );
    if(logo)
      oyjlStrAdd( s, "                                       \"%s\",\n", logo );
    else
      oyjlStrAdd( s, "                                       NULL,\n" );
    oyjlStrAdd( s, "#endif\n" );
    oyjlStrAdd( s, "                                       sections, oarray, groups, &state );\n" );
    if(!(app_type && strcmp(app_type,"tool") == 0))
    oyjlStrAdd( s, "  if(ui) ui->app_type = \"%s\";\n", app_type ? app_type : "lib" );
    if(!help_found && h_found)
      oyjlStrAdd( s, "  help = h ? \"\" : NULL;\n" );
    if(!verbose_found && v_found)
    {
      if(v_is_string)
        oyjlStrAdd( s, "  verbose = v ? 1 : 0;\n" );
      else
        oyjlStrAdd( s, "  verbose = v;\n" );
    }
    oyjlStrAdd( s, "  if( state & oyjlUI_STATE_EXPORT &&\n" );
    oyjlStrAdd( s, "      export_var &&\n" );
    oyjlStrAdd( s, "      strcmp(export_var,\"json+command\") != 0)\n" );
    oyjlStrAdd( s, "    goto clean_main;\n");
    oyjlStrAdd( s, "  if(state & oyjlUI_STATE_HELP)\n" );
    oyjlStrAdd( s, "  {\n" );
    oyjlStrAdd( s, "    fprintf( stderr, \"%%s\\n\\tman %s\\n\\n\", _(\"For more information read the man page:\") );\n", nick, nick );
    oyjlStrAdd( s, "    goto clean_main;\n" );
    oyjlStrAdd( s, "  }\n" );
    oyjlStrAdd( s, "\n" );
    oyjlStrAdd( s, "  if(ui && verbose)\n" );
    oyjlStrAdd( s, "  {\n" );
    oyjlStrAdd( s, "    char * json = oyjlOptions_ResultsToJson( ui->opts );\n" );
    oyjlStrAdd( s, "    if(json)\n" );
    oyjlStrAdd( s, "      fputs( json, stderr );\n" );
    oyjlStrAdd( s, "    fputs( \"\\n\", stderr );\n" );
    oyjlStrAdd( s, "\n" );
    oyjlStrAdd( s, "    char * text = oyjlOptions_ResultsToText( ui->opts );\n" );
    oyjlStrAdd( s, "    if(text)\n" );
    oyjlStrAdd( s, "      fputs( text, stderr );\n" );
    oyjlStrAdd( s, "    fputs( \"\\n\", stderr );\n" );
    oyjlStrAdd( s, "  }\n" );
    oyjlStrAdd( s, "\n" );
    oyjlStrAdd( s, "  if(ui && (export_var && strcmp(export_var,\"json+command\") == 0))\n" );
    oyjlStrAdd( s, "  {\n" );
    oyjlStrAdd( s, "    char * json = oyjlUi_ToJson( ui, 0 ),\n" );
    oyjlStrAdd( s, "         * json_commands = NULL;\n" );
    oyjlStrAdd( s, "    oyjlStringAdd( &json_commands, malloc, free, \"{\\n  \\\"command_set\\\": \\\"%%s\\\",\", argv[0] );\n" );
    oyjlStrAdd( s, "    oyjlStringAdd( &json_commands, malloc, free, \"%%s\", &json[1] ); /* skip opening '{' */\n" );
    oyjlStrAdd( s, "    puts( json_commands );\n" );
    oyjlStrAdd( s, "    goto clean_main;\n" );
    oyjlStrAdd( s, "  }\n" );
    oyjlStrAdd( s, "\n" );
    oyjlStrAdd( s, "  /* Render boilerplate */\n" );
    oyjlStrAdd( s, "  if(ui && render)\n" );
    oyjlStrAdd( s, "  {\n" );
    oyjlStrAdd( s, "#if !defined(NO_OYJL_ARGS_RENDER)\n" );
    oyjlStrAdd( s, "    int debug = verbose;\n" );
    oyjlStrAdd( s, "    oyjlArgsRender( argc, argv, NULL, NULL,NULL, debug, ui, myMain );\n" );
    oyjlStrAdd( s, "#else\n" );
    oyjlStrAdd( s, "    fprintf( stderr, \"No render support compiled in. For a GUI use -X json and load into oyjl-args-render viewer.\" );\n" );
    oyjlStrAdd( s, "#endif\n" );
    oyjlStrAdd( s, "  } else if(ui)\n" );
    oyjlStrAdd( s, "  {\n" );
    oyjlStrAdd( s, "    /* ... working code goes here ... */\n" );
    oyjlStrAdd( s, "  }\n" );
    oyjlStrAdd( s, "  else error = 1;\n" );
    oyjlStrAdd( s, "\n" );
    oyjlStrAdd( s, "  clean_main:\n" );
    oyjlStrAdd( s, "  {\n" );
    oyjlStrAdd( s, "    int i = 0;\n" );
    oyjlStrAdd( s, "    while(oarray[i].type[0])\n" );
    oyjlStrAdd( s, "    {\n" );
    oyjlStrAdd( s, "      if(oarray[i].value_type == oyjlOPTIONTYPE_CHOICE && oarray[i].values.choices.list)\n" );
    oyjlStrAdd( s, "        free(oarray[i].values.choices.list);\n" );
    oyjlStrAdd( s, "      ++i;\n" );
    oyjlStrAdd( s, "    }\n" );
    oyjlStrAdd( s, "  }\n" );
    oyjlStrAdd( s, "  oyjlLibRelease();\n" );
    oyjlStrAdd( s, "\n" );
    oyjlStrAdd( s, "  return error;\n" );
    oyjlStrAdd( s, "}\n" );
    oyjlStrAdd( s, "\n" );
    oyjlStrAdd( s, "extern int * oyjl_debug;\n" );
    oyjlStrAdd( s, "char ** environment = NULL;\n" );
    oyjlStrAdd( s, "int main( int argc_, char**argv_, char ** envv )\n" );
    oyjlStrAdd( s, "{\n" );
    oyjlStrAdd( s, "  int argc = argc_;\n" );
    oyjlStrAdd( s, "  char ** argv = argv_;\n" );
    oyjlStrAdd( s, "\n" );
    oyjlStrAdd( s, "#ifdef __ANDROID__\n" );
    oyjlStrAdd( s, "  setenv(\"COLORTERM\", \"1\", 0); /* show rich text format on non GNU color extension environment */\n" );
    oyjlStrAdd( s, "\n" );
    oyjlStrAdd( s, "  argv = calloc( argc + 2, sizeof(char*) );\n" );
    oyjlStrAdd( s, "  memcpy( argv, argv_, (argc + 2) * sizeof(char*) );\n" );
    oyjlStrAdd( s, "  argv[argc++] = \"--render=gui\"; /* start Renderer (e.g. QML) */\n" );
    oyjlStrAdd( s, "  environment = environ;\n" );
    oyjlStrAdd( s, "#else\n" );
    oyjlStrAdd( s, "  environment = envv;\n" );
    oyjlStrAdd( s, "#endif\n" );
    oyjlStrAdd( s, "\n" );
    oyjlStrAdd( s, "  /* language needs to be initialised before setup of data structures */\n" );
    oyjlStrAdd( s, "  int use_gettext = 0;\n" );
    oyjlStrAdd( s, "#ifdef OYJL_USE_GETTEXT\n" );
    oyjlStrAdd( s, "  use_gettext = 1;\n" );
    oyjlStrAdd( s, "#ifdef OYJL_HAVE_LOCALE_H\n" );
    oyjlStrAdd( s, "  setlocale(LC_ALL,\"\");\n" );
    oyjlStrAdd( s, "#endif\n" );
    oyjlStrAdd( s, "#endif\n" );
    oyjlStrAdd( s, "  oyjlInitLanguageDebug( \"Oyjl\", \"OYJL_DEBUG\", oyjl_debug, use_gettext, \"OYJL_LOCALEDIR\", OYJL_LOCALEDIR, OYJL_DOMAIN, NULL );\n" );
    oyjlStrAdd( s, "\n" );
    oyjlStrAdd( s, "  myMain(argc, (const char **)argv);\n" );
    oyjlStrAdd( s, "\n" );
    oyjlStrAdd( s, "#ifdef __ANDROID__\n" );
    oyjlStrAdd( s, "  free( argv );\n" );
    oyjlStrAdd( s, "#endif\n" );
    oyjlStrAdd( s, "\n" );
    oyjlStrAdd( s, "  return 0;\n" );
    oyjlStrAdd( s, "}\n" );
    oyjlStrAdd( s, "\n" );
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

    oyjlStrAdd( s, "_%s()\n", func );
    oyjlStrAdd( s, "{\n" );
    oyjlStrAdd( s, "    local cur prev words cword\n" );
    oyjlStrAdd( s, "    _init_completion -s || return\n" );
    oyjlStrAdd( s, "\n" );
    oyjlStrAdd( s, "    #set -x -v\n" );
    oyjlStrAdd( s, "\n" );
    oyjlStrAdd( s, "    local SEARCH=${COMP_WORDS[COMP_CWORD]}\n" );
    oyjlStrAdd( s, "    if [[ \"$SEARCH\" == \"=\" ]]; then\n" );
    oyjlStrAdd( s, "      SEARCH=\"\"\n" );
    oyjlStrAdd( s, "    fi\n" );
    oyjlStrAdd( s, "\n" );
    oyjlStrAdd( s, "    : \"autocomplete options with choices for long options \"$prev\"\"\n" );
    oyjlStrAdd( s, "    case \"$prev\" in\n" );
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
          oyjlStrAdd( s, "        --%s) # long option with dynamic args\n", option );
          if(use_getChoicesCompletionBash)
            oyjlStrAdd( s, "            local OYJL_TEXTS=$(%s)\n", getChoices );
          else
          {
            oyjlStrAdd( s, "            local OYJL_TEXTS\n" );
            oyjlStrAdd( s, "            if [[ \"${COMP_WORDS[COMP_CWORD]}\" == \"=\" ]]; then\n" );
            oyjlStrAdd( s, "              OYJL_TEXTS=$(${COMP_LINE}oyjl-list | sed 's/\\[/_/g;s/\\]/_/g')\n" );
            oyjlStrAdd( s, "            else\n" );
            oyjlStrAdd( s, "              OYJL_TEXTS=$(${COMP_LINE} --%s=oyjl-list | sed 's/\\[/_/g;s/\\]/_/g')\n", option );
            oyjlStrAdd( s, "            fi\n" );
          }
          oyjlStrAdd( s, "            local IFS=$'\\n'\n" );
          oyjlStrAdd( s, "            local WORD_LIST=()\n" );
          oyjlStrAdd( s, "            for OYJL_TEXT in $OYJL_TEXTS\n" );
          oyjlStrAdd( s, "              do WORD_LIST=(\"${WORD_LIST[@]}\" \"$OYJL_TEXT\")\n" );
          oyjlStrAdd( s, "            done\n" );
          oyjlStrAdd( s, "            COMPREPLY=($(compgen -W '\"${WORD_LIST[@]}\"' -- \"$cur\"))\n" );
          oyjlStrAdd( s, "            set +x +v\n" );
          oyjlStrAdd( s, "            return\n" );
          oyjlStrAdd( s, "            ;;\n" );
        }
      } else
      if(value_type && strcmp(value_type, "oyjlOPTIONTYPE_CHOICE") == 0)
      {
        const char *nick;
        int count = oyjlValueCount( oyjlTreeGetValue( val, 0, "values/choices/list" ) ), j;
        if(count)
        {
          oyjlStrAdd( s, "        --%s) # long option with static args\n", option );
          oyjlStrAdd( s, "            local IFS=$'\\n'\n" );
          oyjlStrAdd( s, "            local WORD_LIST=(" );
        }
        for(j = 0; j < count; ++j)
        {
          oyjl_val c = oyjlTreeGetValueF( val, 0, "values/choices/list/[%d]", j );
          v = oyjlTreeGetValue( c, 0, "nick" ); nick = OYJL_GET_STRING(v);
          oyjlStrAdd( s, "%s'%s'", j?" ":"", nick );
        }
        if(count)
        {
          oyjlStrAdd( s, ")\n" );
          oyjlStrAdd( s, "            COMPREPLY=($(compgen -W '\"${WORD_LIST[@]}\"' -- \"$cur\"))\n" );
          oyjlStrAdd( s, "            set +x +v\n" );
          oyjlStrAdd( s, "            return\n" );
          oyjlStrAdd( s, "            ;;\n" );
        }
      }
    }
    oyjlStrAdd( s, "    esac\n" );
    oyjlStrAdd( s, "    : \"autocomplete options with choices for single letter options \"$cur\"\"\n" );
    oyjlStrAdd( s, "    case \"$cur\" in\n" );
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
          oyjlStrAdd( s, "        -%s=*) # single letter option with dynamic args\n", o );
          if(use_getChoicesCompletionBash)
            oyjlStrAdd( s, "            local OYJL_TEXTS=$(%s)\n", getChoices );
          else
          {
            oyjlStrAdd( s, "            local OYJL_TEXTS\n" );
            oyjlStrAdd( s, "            if [[ \"${COMP_WORDS[COMP_CWORD]}\" == \"=\" ]]; then\n" );
            oyjlStrAdd( s, "              OYJL_TEXTS=$(${COMP_LINE}oyjl-list | sed 's/\\[/_/g;s/\\]/_/g')\n" );
            oyjlStrAdd( s, "            else\n" );
            oyjlStrAdd( s, "              OYJL_TEXTS=$(${COMP_LINE} -%s=oyjl-list | sed 's/\\[/_/g;s/\\]/_/g')\n", o );
            oyjlStrAdd( s, "            fi\n" );
          }
          oyjlStrAdd( s, "            local IFS=$'\\n'\n" );
          oyjlStrAdd( s, "            local WORD_LIST=()\n" );
          oyjlStrAdd( s, "            for OYJL_TEXT in $OYJL_TEXTS\n" );
          oyjlStrAdd( s, "              do WORD_LIST=(\"${WORD_LIST[@]}\" \"$OYJL_TEXT\")\n" );
          oyjlStrAdd( s, "            done\n" );
          oyjlStrAdd( s, "            COMPREPLY=($(compgen -W '\"${WORD_LIST[@]}\"' -- \"$SEARCH\"))\n" );
          oyjlStrAdd( s, "            set +x +v\n" );
          oyjlStrAdd( s, "            return\n" );
          oyjlStrAdd( s, "            ;;\n" );
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
          oyjlStrAdd( s, "        -%s=*) # single letter option with static args\n", o );
          oyjlStrAdd( s, "            local IFS=$'\\n'\n" );
          oyjlStrAdd( s, "            local WORD_LIST=(" );
        }
        for(j = 0; j < count; ++j)
        {
          oyjl_val c = oyjlTreeGetValueF( val, 0, "values/choices/list/[%d]", j );
          v = oyjlTreeGetValue( c, 0, "nick" ); nick = OYJL_GET_STRING(v);
          oyjlStrAdd( s, "%s'%s'", j?" ":"", nick );
        }
        if(count)
        {
          oyjlStrAdd( s, ")\n" );
          oyjlStrAdd( s, "            COMPREPLY=($(compgen -W '\"${WORD_LIST[@]}\"' -- \"$SEARCH\"))\n" );
          oyjlStrAdd( s, "            set +x +v\n" );
          oyjlStrAdd( s, "            return\n" );
          oyjlStrAdd( s, "            ;;\n" );
        }
      }
    }
    oyjlStrAdd( s, "    esac\n" );
    oyjlStrAdd( s, "\n" );
    oyjlStrAdd( s, "\n" );
    oyjlStrAdd( s, "    : \"autocomplete options \"$cur\"\"\n" );
    oyjlStrAdd( s, "    case \"$cur\" in\n" );
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
          oyjlStrAdd( s, "        " );
        oyjlStrAdd( s, "%s-%s", found?"|":"", o );
        ++found;
      }
      else
      if(value_type && strcmp(value_type, "oyjlOPTIONTYPE_NONE") == 0)
      {
      }
    }
    if(found)
    {
      oyjlStrAdd( s, ")\n" );
      oyjlStrAdd( s, "            : \"finish short options with choices\"\n" );
      oyjlStrAdd( s, "            COMPREPLY=(\"$cur=\\\"\")\n" );
      oyjlStrAdd( s, "            set +x +v\n" );
      oyjlStrAdd( s, "            return\n" );
      oyjlStrAdd( s, "            ;;\n" );
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
          oyjlStrAdd( s, "        " );
        oyjlStrAdd( s, "%s--%s", found?"|":"", option );
        ++found;
      }
      else
      if(value_type && strcmp(value_type, "oyjlOPTIONTYPE_NONE") == 0)
      {
      }
    }
    if(found)
    {
      oyjlStrAdd( s, ")\n" );
      oyjlStrAdd( s, "            : \"finish long options with choices\"\n" );
      oyjlStrAdd( s, "            COMPREPLY=(\"$cur=\\\"\")\n" );
      oyjlStrAdd( s, "            set +x +v\n" );
      oyjlStrAdd( s, "            return\n" );
      oyjlStrAdd( s, "            ;;\n" );
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
          oyjlStrAdd( s, "        " );
        oyjlStrAdd( s, "%s-%s", found?"|":"", o );
        ++found;
      }
    }
    if(found)
    {
      oyjlStrAdd( s, ")\n" );
      oyjlStrAdd( s, "            : \"finish short options without choices\"\n" );
      oyjlStrAdd( s, "            COMPREPLY=(\"$cur \")\n" );
      oyjlStrAdd( s, "            set +x +v\n" );
      oyjlStrAdd( s, "            return\n" );
      oyjlStrAdd( s, "            ;;\n" );
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
          oyjlStrAdd( s, "        " );
        oyjlStrAdd( s, "%s--%s", found?"|":"", option );
        ++found;
      }
    }
    if(found)
    {
      oyjlStrAdd( s, ")\n" );
      oyjlStrAdd( s, "            : \"finish long options without choices\"\n" );
      oyjlStrAdd( s, "            COMPREPLY=(\"$cur \")\n" );
      oyjlStrAdd( s, "            set +x +v\n" );
      oyjlStrAdd( s, "            return\n" );
      oyjlStrAdd( s, "            ;;\n" );
    }
    found = 0;
    oyjlStrAdd( s, "    esac\n" );
    oyjlStrAdd( s, "\n" );
    oyjlStrAdd( s, "\n" );
    oyjlStrAdd( s, "    : \"show help for none '@' UIs\"\n" );
    oyjlStrAdd( s, "    if [[ \"$cur\" == \"\" ]]; then\n" );
    oyjlStrAdd( s, "      if [[ ${COMP_WORDS[1]} == \"\" ]]; then\n" );
    oyjlStrAdd( s, "        $1 help synopsis 1>&2\n" );
    oyjlStrAdd( s, "      else\n" );
    oyjlStrAdd( s, "        $1 help ${COMP_WORDS[1]} 1>&2\n" );
    oyjlStrAdd( s, "      fi\n" );
    oyjlStrAdd( s, "    fi\n" );
    oyjlStrAdd( s, "\n" );
    oyjlStrAdd( s, "\n" );
    oyjlStrAdd( s, "    : \"suggest group options for subcommands\"\n" );
    oyjlStrAdd( s, "    if [[ \"$cur\" == \"\" ]] || [[ \"$cur\" == \"-\" ]] || [[ \"$cur\" == -- ]] || [[ \"$cur\" == -* ]]; then\n" );
    oyjlStrAdd( s, "      case \"${COMP_WORDS[1]}\" in\n" );
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
      oyjlStrAdd( s, "        " );
      found = 0;
      for(j = 0; j < mandatory_n; ++j)
      {
        const char * moption = mandatory_list[j];
        opt = oyjlOptions_GetOptionL( ui->opts, moption, 0 );
        if(opt->o && (strcmp(opt->o,"#") == 0 || strcmp(opt->o,"@") == 0))
          continue;
        oyjlStrAdd( s, "%s%s%s%s%s%s", found?"|":"", opt->o?"-":"", opt->o?opt->o:"", (opt->o && opt->option)?"|":"", (opt->option && !sub)?"--":"", opt->option?opt->option:"" );
        ++found;
      }
      oyjlStrAdd( s, ")\n" );
      oyjlStrAdd( s, "          COMPREPLY=($(compgen -W '" );
      found = 0;
      for(j = 0; j < optional_n; ++j)
      {
        const char * ooption = optional_list[j];
        opt = oyjlOptions_GetOptionL( ui->opts, ooption, 0 );
        oyjlStrAdd( s, "%s%s%s%s%s%s%s%s", found?" ":"", opt->o?"-":"", opt->o?opt->o:"", (opt->o && WANT_ARG(opt))?"=":"", opt->o?" ":"", opt->option?"--":"", opt->option?opt->option:"", (opt->option && WANT_ARG(opt))?"=":"" );
        ++found;
      }
      oyjlStrAdd( s, "' -- \"$cur\"))\n" );
      oyjlStrAdd( s, "            set +x +v\n" );
      oyjlStrAdd( s, "            return\n" );
      oyjlStrAdd( s, "            ;;\n" );
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
      oyjlStrAdd( s, "        " );
      oyjlStrAdd( s, ".*)\n" );
      oyjlStrAdd( s, "          COMPREPLY=($(compgen -W '" );
      found = 0;
      for(j = 0; j < optional_n; ++j)
      {
        const char * ooption = optional_list[j];
        opt = oyjlOptions_GetOptionL( ui->opts, ooption, 0 );
        oyjlStrAdd( s, "%s%s%s%s%s%s%s%s", found?" ":"", opt->o?"-":"", opt->o?opt->o:"", (opt->o && WANT_ARG(opt))?"=":"", opt->o?" ":"", opt->option?"--":"", opt->option?opt->option:"", (opt->option && WANT_ARG(opt))?"=":"" );
        ++found;
      }
      oyjlStrAdd( s, "' -- \"$cur\"))\n" );
      oyjlStrAdd( s, "            set +x +v\n" );
      oyjlStrAdd( s, "            return\n" );
      oyjlStrAdd( s, "            ;;\n" );
      oyjlStringListRelease( &mandatory_list, mandatory_n, free );
      oyjlStringListRelease( &optional_list, optional_n, free );
    }
    oyjlStrAdd( s, "      esac\n" );
    oyjlStrAdd( s, "    fi\n" );
    oyjlStrAdd( s, "\n" );
    oyjlStrAdd( s, "    : \"suggest mandatory options on first args only\"\n" );
    oyjlStrAdd( s, "    if [[ \"${COMP_WORDS[2]}\" == \"\" ]]; then\n" );
    oyjlStrAdd( s, "      local WORD_LIST=()\n" );
    found = 0;
    if(found_at_arg_func)
    {
        oyjlStrAdd( s, "      local OYJL_TEXTS=$(%s)\n", found_at_arg_func );
        oyjlStrAdd( s, "      local IFS=$'\\n'\n" );
        oyjlStrAdd( s, "      for OYJL_TEXT in $OYJL_TEXTS\n" );
        oyjlStrAdd( s, "        do WORD_LIST=(\"${WORD_LIST[@]}\"\n\"$OYJL_TEXT\")\n" );
        oyjlStrAdd( s, "      done\n" );
    }
    if(n)
      oyjlStrAdd( s, "      WORD_LIST=(\"${WORD_LIST[@]}\" " );
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
        oyjlStrAdd( s, "%s%s%s%s%s%s%s%s", found?" ":"", opt->o?"-":"", opt->o?opt->o:"", (opt->o && WANT_ARG(opt))?"=":"", opt->o?" ":"", (opt->option && !sub)?"--":"", opt->option?opt->option:"", (opt->option && WANT_ARG(opt))?"=":"" );
        ++found;
      }
      oyjlStringListRelease( &mandatory_list, mandatory_n, free );
    }
    if(n)
    {
      oyjlStrAdd( s, ")\n" );
    }
#undef WANT_ARG
    oyjlStrAdd( s, "      COMPREPLY=($(compgen -W '\"${WORD_LIST[@]}\"' -- \"$cur\"))\n" );
    oyjlStrAdd( s, "      set +x +v\n" );
    oyjlStrAdd( s, "      return\n" );
    oyjlStrAdd( s, "    fi\n" );
    oyjlStrAdd( s, "\n" );
    oyjlStrAdd( s, "    set +x +v\n" );
    oyjlStrAdd( s, "} &&\n" );
    oyjlStrAdd( s, "complete -o nosort -F _%s -o nospace %s\n", func, nick_ );
    oyjlStrAdd( s, "\n" );
    free(func);
    oyjlUi_Release( &ui );
  }
  else
    fputs( "can only generate C code, or bash completion", stderr );

  c = oyjlStrPull( s );
  oyjlStrRelease( &s );
  return c;
}

/** @brief    Return a JSON representation from options
 *  @memberof oyjlUi_s
 *
 *  The JSON data shall be useable with oyjl-args-qml options renderer.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2020/05/29
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

    if(!(g->detail && g->detail[0]))
      continue;

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
              while(list[n].nick && list[n].nick[0] != '\000')
                ++n;

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
              if(list[l-pos].nick)
              {
                key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "nick" );
                oyjlValueSetString( key, list[l-pos].nick );
              }
              if(list[l-pos].name)
              {
                key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "name" );
                oyjlValueSetString( key, list[l-pos].name );
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
 *  the previous language.
 *
 *  @param[in,out] ui                 the structure to translate strings inside
 *  @param[in]     loc                the desired language, the locale "back" will inverse the translation; optional, without the function will return
 *  @param[int]    catalog            the message catalog; optional, without the function will return
 *  @param[int]    translator         the translation function; optional, defaut is oyjlTranslate()
 *
 *  @version Oyjl: 1.0.0
 *  @date    2020/08/18
 *  @since   2020/07/30 (Oyjl: 1.0.0)
 */
void               oyjlUi_Translate  ( oyjlUi_s          * ui,
                                       const char        * loc,
                                       oyjl_val            catalog,
                                       oyjlTranslate_f     translator )
{
  int i,j,n,ng;

  if(!ui || !loc || !catalog) return;

  if(!translator)
    translator = oyjlTranslate;

#define tr( text ) text = translator(loc, catalog, text)
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
    { "oihs", "copyright", NULL, "Copyright © 2017-2021 Kai-Uwe Behrmann", NULL },
    { "oihs", "license", NULL, "newBSD", "http://www.oyranos.org" },
    { "oihs", "url", NULL, "http://www.oyranos.org", NULL },
    { "oihs", "support", NULL, "https://www.github.com/oyranos-cms/oyranos/issues", NULL },
    { "oihs", "download", NULL, "https://github.com/oyranos-cms/oyranos/releases", NULL },
    { "oihs", "sources", NULL, "https://github.com/oyranos-cms/oyranos", NULL },
    { "oihs", "development", NULL, "https://github.com/oyranos-cms/oyranos", NULL },
    { "oihs", "oyjl_module_author", NULL, "Kai-Uwe Behrmann", "http://www.behrmann.name" },
    { "oihs", "documentation", NULL, "http://www.oyranos.org", documentation },
    { "oihs", "date", NULL, date_name, date_description },
    { "", NULL, NULL, NULL, NULL }
  };
  return (oyjlUiHeaderSection_s*) oyjlStringAppendN( NULL, (const char*)s, sizeof(s), malloc );
}

