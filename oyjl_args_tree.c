/*  @file oyjl_args.c
 *
 *  oyjl - UI helpers
 *
 *  @par Copyright:
 *            2018-2019 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl argument handling with libOyjlCore
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *
 * Copyright (c) 2018-2019  Kai-Uwe Behrmann  <ku.b@gmx.de>
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
 *  @date    2018/08/14
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
char * oyjlOptions_ResultsToJson  ( oyjlOptions_s  * opts )
{
  char * args = NULL,
       * rjson = NULL;
  char ** results = opts->private_data;
  oyjl_val root, value;
  int i,n;

  if(!results)
  {
    if(oyjlOptions_Parse( opts ))
      return NULL;

    results = opts->private_data;
    if(!results)
      return NULL;
  }

  args = results[0];
  n = strlen( args );
  root = oyjlTreeNew( "" );
  for(i = 0; i < n; ++i)
  {
    char a[4] = {0,0,0,0};
    a[0] = args[i];
    value = oyjlTreeGetValue( root, OYJL_CREATE_NEW, a );
    oyjlValueSetString( value, results[i+1] );
  }

  i = 0;
  oyjlTreeToJson( root, &i, &rjson );
  oyjlTreeFree( root );

  return rjson;
}

#define CASE_RETURN_ENUM( text_ ) case text_: return #text_;
static const char * oyjlOPTIONTYPE_eToString ( oyjlOPTIONTYPE_e e )
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
static const char * oyjlVARIABLE_eToString ( oyjlVARIABLE_e e )
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

/** @brief    Return a JSON dump
 *  @memberof oyjlUi_s
 *
 *  The returned JSON is a complete dump of all oyjlUi_s data.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/06/16
 *  @since   2019/06/16 (Oyjl: 1.0.0)
 */
char *             oyjlUi_ExportToJson(oyjlUi_s          * ui,
                                       int                 flags OYJL_UNUSED )
{
  char * t = NULL;
  oyjl_val root;
  int i,j,n,ng;

  if(!ui) return t;

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
  n = oyjlUi_CountHeaderSections( ui );
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
  if(ui->opts->user_data)
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "1", OYJL_REG "/ui/options/user_data" );
  int nopts = oyjlOptions_Count( ui->opts );
  for(i = 0; i < nopts; ++i)
  {
    char oo[8];
    oyjlOption_s * o = &ui->opts->array[i];
    sprintf(oo, "%c", o->o);
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, o->type, OYJL_REG "/ui/options/array/[%d]/%s", i, "type" );
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, oo, OYJL_REG "/ui/options/array/[%d]/%s", i, "o" );
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, o->option, OYJL_REG "/ui/options/array/[%d]/%s", i, "option" );
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, o->key, OYJL_REG "/ui/options/array/[%d]/%s", i, "key" );
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, o->name, OYJL_REG "/ui/options/array/[%d]/%s", i, "name" );
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, o->description, OYJL_REG "/ui/options/array/[%d]/%s", i, "description" );
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, o->help, OYJL_REG "/ui/options/array/[%d]/%s", i, "help" );
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, o->value_name, OYJL_REG "/ui/options/array/[%d]/%s", i, "value_name" );
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, oyjlOPTIONTYPE_eToString(o->value_type), OYJL_REG "/ui/options/array/[%d]/%s", i, "value_type" );
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
        oyjlTreeSetStringF( root, OYJL_CREATE_NEW, o->values.getChoices ? "getChoices":NULL, OYJL_REG "/ui/options/array/[%d]/values/%s", i, "getChoices" );
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
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, oyjlVARIABLE_eToString(o->variable_type), OYJL_REG "/ui/options/array/[%d]/%s", i, "variable_type" );
    if(o->variable_type != oyjlNONE)
      oyjlTreeSetStringF( root, OYJL_CREATE_NEW, "dummy", OYJL_REG "/ui/options/array/[%d]/%s", i, "variable_name" );
  }

  ng = oyjlOptions_CountGroups( ui->opts );
  for(i = 0; i < ng; ++i)
  {
    oyjlOptionGroup_s * g = &ui->opts->groups[i];
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, g->type, OYJL_REG "/ui/options/groups/[%d]/%s", i, "type" );
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, g->name, OYJL_REG "/ui/options/groups/[%d]/%s", i, "name" );
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, g->description, OYJL_REG "/ui/options/groups/[%d]/%s", i, "description" );
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, g->help, OYJL_REG "/ui/options/groups/[%d]/%s", i, "help" );
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, g->mandatory, OYJL_REG "/ui/options/groups/[%d]/%s", i, "mandatory" );
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, g->optional, OYJL_REG "/ui/options/groups/[%d]/%s", i, "optional" );
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, g->detail, OYJL_REG "/ui/options/groups/[%d]/%s", i, "detail" );
  }

  i = 0;
  oyjlTreeToJson( root, &i, &t );
  oyjlTreeFree( root );

  return t;
}

char *             oyjlUiJsonToCode  ( char              * json,
                                       int                 flags )
{
  char * c = NULL;
  oyjl_val root, key;
  if(flags & OYJL_SOURCE_CODE_C)
  {

  }
  else
    fputs( "can only generate C code. Try OYJL_SOURCE_CODE_C flag", stderr );
  return c;
}

/** @brief    Return a JSON representation from options
 *  @memberof oyjlUi_s
 *
 *  The JSON data shall be useable with oyjl-args-qml options renderer.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/05/30
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
char *       oyjlUi_ToJson           ( oyjlUi_s          * ui,
                                       int                 flags OYJL_UNUSED )
{
  char * t = NULL, num[64];
  oyjl_val root, key;
  int i,j,n,ng;

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
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, ui->nick, OYJL_REG "/modules/[0]/nick" );
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, ui->name, OYJL_REG "/modules/[0]/name" );
  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, ui->description, OYJL_REG "/modules/[0]/description" );
  if(ui->logo)
    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, ui->logo, OYJL_REG "/modules/[0]/logo" );

  n = oyjlUi_CountHeaderSections( ui );
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
      else oyjlValueSetString( key, _(s->nick) );
    }
    key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/information/[%d]/%s", i, "name" );
    oyjlValueSetString( key, s->name );
    if(s->description)
    {
      key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/information/[%d]/%s", i, "description" );
      oyjlValueSetString( key, s->description );
    }
  }

  ng = oyjlOptions_CountGroups( ui->opts );
  for(i = 0; i < ng; ++i)
  {
    oyjlOptionGroup_s * g = &ui->opts->groups[i];
    oyjlOptions_s * opts = ui->opts;

    if(!(g->detail && g->detail[0]))
      continue;

    key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/%s", i, "name" );
    oyjlValueSetString( key, g->name );
    key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/%s", i, "description" );
    oyjlValueSetString( key, g->description );
    if(g->help)
    {
      key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/%s", i, "help" );
      oyjlValueSetString( key, g->help );
        fprintf(stderr, "found help: %s\n", g->help);
    }

    key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/%s", i, "mandatory" );
    oyjlValueSetString( key, g->mandatory );
    key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/%s", i, "optional" );
    oyjlValueSetString( key, g->optional );
    {
      char ** results = oyjlOptions_ResultsToList( opts, 0, &n );
      char * changed = NULL;
      for(j = 0; j < n; ++j)
        if(results[j][0] != 'X')
          oyjlStringAdd( &changed, 0,0, "%c", results[j][0] );
      if(changed)
      {
        key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/%s", i, "changed" );
        oyjlValueSetString( key, changed );
      }
      if(changed) free(changed);
      oyjlStringListRelease( &results, n, free );
    }
    int d = g->detail ? strlen(g->detail) : 0;
    for(j = 0; j < d; ++j)
    {
      char oc = g->detail[j];
      oyjlOption_s * o = oyjlOptions_GetOption( opts, oc );
      key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "key" );
      if(!o->key)
        sprintf(num, "%c", o->o);
      oyjlValueSetString( key, o->key?o->key:num );
      key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "name" );
      oyjlValueSetString( key, o->name );
      key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "description" );
      oyjlValueSetString( key, o->description );
      if(o->help)
      {
        key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "help" );
        oyjlValueSetString( key, o->help );
      }

      switch(o->value_type)
      {
        case oyjlOPTIONTYPE_CHOICE:
          {
            int l = 0, pos = 0;
            if(o->flags & OYJL_OPTION_FLAG_EDITABLE)
            {
              int count = 0;
              char ** results = oyjlOptions_ResultsToList( opts, o->o, &count );
              for( l = 0; l < count; ++l )
              {
                key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "nick" );
                oyjlValueSetString( key, results[l] );
                key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "name" );
                oyjlValueSetString( key, results[l] );
              }
              oyjlStringListRelease( &results, count, free );
              pos = l;
            }

            if(o->values.choices.list)
            {
              int n = 0;
              key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "default" );
              sprintf( num, "%d", o->values.choices.selected );
              oyjlValueSetString( key, num );
              while(o->values.choices.list[n].nick && o->values.choices.list[n].nick[0] != '\000')
                ++n;
              for(l = pos; l < n+pos; ++l)
              {
                key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "nick" );
                oyjlValueSetString( key, o->values.choices.list[l-pos].nick );
                key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "name" );
                oyjlValueSetString( key, o->values.choices.list[l-pos].name );
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
          }
          break;
        case oyjlOPTIONTYPE_FUNCTION:
          {
            int n = 0,l, selected, pos = 0;
            oyjlOptionChoice_s * list = oyjlOption_GetChoices_(o, &selected, opts );
            if(list)
              while(list[n].nick && list[n].nick[0] != '\000')
                ++n;

            if(o->flags & OYJL_OPTION_FLAG_EDITABLE)
            {
              int count = 0;
              char ** results = oyjlOptions_ResultsToList( opts, o->o, &count );
              for( l = 0; l < count; ++l )
              {
                key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "nick" );
                oyjlValueSetString( key, results[l] );
                key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "name" );
                oyjlValueSetString( key, results[l] );
              }
              oyjlStringListRelease( &results, count, free );
              pos = l;
            }

            if(0 <= selected && selected < n && strlen(list[selected].nick))
            {
              key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "default" );
              oyjlValueSetString( key, list[selected].nick );
            }
            for(l = pos; l < n+pos; ++l)
            {
              key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "nick" );
              oyjlValueSetString( key, list[l-pos].nick );
              key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/choices/[%d]/%s", i,j,l, "name" );
              oyjlValueSetString( key, list[l-pos].name );
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
          key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "default" );
          {
              int count = 0;
              char ** results = oyjlOptions_ResultsToList( opts, o->o, &count );
              if( count )
                sprintf( num, "%s", results[0] );
              else
                sprintf( num, "%g", o->values.dbl.d );
              oyjlValueSetString( key, num );
              oyjlStringListRelease( &results, count, free );
          }
          key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "start" );
          sprintf( num, "%g", o->values.dbl.start ); oyjlValueSetString( key, num );
          key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "end" );
          sprintf( num, "%g", o->values.dbl.end ); oyjlValueSetString( key, num );
          key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "tick" );
          sprintf( num, "%g", o->values.dbl.tick ); oyjlValueSetString( key, num );
          key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "type" );
          oyjlValueSetString( key, "double" );
          break;
        case oyjlOPTIONTYPE_NONE:
          key = oyjlTreeGetValueF( root, OYJL_CREATE_NEW, OYJL_REG "/modules/[0]/groups/[%d]/options/[%d]/%s", i,j, "default" );
          oyjlValueSetString( key, "0" );
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
  }
  i = 0;
  oyjlTreeToJson( root, &i, &t );
  oyjlTreeFree( root );

  return t;
}

