/** @file oyjl-args-share.c
 *
 *  oyjl - UI helpers shared code
 *
 *  @par Copyright:
 *            2018-2024 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl argument handling with libOyjlCore
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *
 *  Copyright (c) 2018-2023  Kai-Uwe Behrmann  <ku.b@gmx.de>
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
#include "oyjl_tree_internal.h"
#include <stddef.h>
#include <ctype.h> /* toupper() */

#define OYJL_REG "org/freedesktop/oyjl"
#define OYJL_IS_NOT_O( x ) (!o->o || strcmp(o->o,x) != 0)
#define OYJL_IS_O( x ) (o->o && strcmp(o->o,x) == 0)

/** \addtogroup oyjl_args
 *  @{ *//* oyjl_args */


/* @return  a options index in the list of a groups mandatory/optional/detail options
 * - >= 0 - if found
 * - -1 - if not found
 */
int oyjlOptionGroupGetOptionIndex_   ( const char        * opt,
                                       const char        * list )
{
  int found = oyjlStringSplitFind(list, "|,", opt, 0, NULL, 0,0);
  return found;
}

int oyjlOptionsResultsWithGroupMatches(oyjl_val            results,
                                       const char        * list,
                                       oyjl_val          * args )
{
  oyjl_val a, r;
  int n = oyjlValueCount( results ), i, index,
      status = 0;
  char * key;
  const char * val;
  for(i = 0; i < n; ++i)
  {
    a = NULL;
    r = oyjlValuePosGet( results, i );
    key = oyjlTreeGetPath( results, r );
    if(!key) continue;
    if(key && args && *args)
      a = oyjlTreeGetValue(*args,0,key);
    if(a)
    {
      free(key);
      continue;
    }
    if(!strrchr(key,'/'))
    index = oyjlOptionGroupGetOptionIndex_(key,list);
    if(index == -1)
    {
      free(key);
      continue;
    }
    if(args)
    {
      val = OYJL_GET_STRING(r);
      if(!(*args)) *args = oyjlTreeNew("");
      oyjlTreeSetStringF( *args, OYJL_CREATE_NEW, val, "%s", key );
    }
    free(key);
  }

  return status;
}


int  oyjlUiJsonGroupIsMan_           ( oyjl_val            g )
{
  oyjl_val first_o = oyjlTreeGetValue(g, 0, "options/[0]/option");
  const char * txt = OYJL_GET_STRING(first_o);
  if(oyjlStringStartsWith( txt, "man-", OYJL_COMPARE_CASE))
    return 1;
  return 0;
}

oyjl_val oyjlUiJsonFindOption_       ( oyjl_val            root,
                                       const char        * option,
                                       int               * gcount )
{
  int i, n;
  oyjl_val v = oyjlTreeGetValue(root, 0, "org/freedesktop/oyjl/modules/[0]"); // use UI JSON
  oyjl_val val = oyjlTreeGetValue(v, 0, "groups");
  n = oyjlValueCount( val );
  for(i = 0; i < n; ++i)
  {
    int j, count, has_options;
    oyjl_val g = oyjlTreeGetValueF(val, 0, "[%d]", i);
    has_options = oyjlTreeGetValue(g, 0, "options") == 0?0:oyjlUiJsonGroupIsMan_(g)?0:1;
    if(has_options)
    {
      if(gcount) ++(*gcount);
      v = oyjlTreeGetValue(g, 0, "options");
      count = oyjlValueCount( v );
      for(j = 0; j < count; ++j)
      {
        oyjl_val opt = oyjlTreeGetValueF(v, 0, "[%d]", j);
        oyjl_val o = oyjlTreeGetValue(opt, 0, "key");
        const char * key = OYJL_GET_STRING(o);
        if(key && strcmp(option, key) == 0)
          return opt;
      }
    }
    v = oyjlTreeGetValue(g, 0, "groups");
    count = oyjlValueCount( v );
    for(j = 0; j < count; ++j)
    {
      g = oyjlTreeGetValueF(v, 0, "[%d]", j);
      if(oyjlTreeGetValue(g, 0, "options"))
      {
        if(gcount) ++(*gcount);
      }
    }
  }

  return 0;
}

char * oyjl_ui_json_option_print_cli_text_ = NULL;
const char * oyjlUiJsonOptionPrintCli_(oyjl_val            opt,
                                       const char        * value,
                                       int                 flags )
{
  oyjl_val o = oyjlTreeGetValue(opt, 0, "option");
  const char * txt = OYJL_GET_STRING(o),
             * key, * type, * no_dash;
  if(oyjl_ui_json_option_print_cli_text_) { free(oyjl_ui_json_option_print_cli_text_); oyjl_ui_json_option_print_cli_text_ = NULL; }
  if(oyjlStringStartsWith( txt, "man-", OYJL_COMPARE_CASE ))
    return oyjl_ui_json_option_print_cli_text_;

  o = oyjlTreeGetValue(opt, 0, "key");
  key = OYJL_GET_STRING(o);
  o = oyjlTreeGetValue(opt, 0, "type");
  type = OYJL_GET_STRING(o);
  o = oyjlTreeGetValue(opt, 0, "no_dash");
  no_dash = OYJL_GET_STRING(o);
  if(key[0] == '@')
    no_dash = "1";

  if(type && strcmp(type,"bool") == 0)
  {
    if(!(value && strcmp(value,"false") == 0))
    {
      oyjlStringAdd( &oyjl_ui_json_option_print_cli_text_, 0,0, "%s%s", no_dash?"":strlen(key) == 1?"-":"--", key /*name*/ );
      if(!(flags&OYJL_QUIET))
        oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "%s(bool):%s", OYJL_DBG_ARGS, key, value );
    }
  }
  else
  {
    if(strcmp(key,"@") == 0)
      oyjlStringAdd( &oyjl_ui_json_option_print_cli_text_, 0,0, "%s", value );
    else if(type && (strcmp(type,"double") == 0 || strcmp(type,"choice") == 0 || strcmp(type,"string") == 0) &&
        value && strcmp(value,"false") == 0)
      return oyjl_ui_json_option_print_cli_text_; 
    else
      oyjlStringAdd( &oyjl_ui_json_option_print_cli_text_, 0,0, "%s%s=%s", no_dash?"":strlen(key) == 1?"-":"--", key, value );
    if(!(flags&OYJL_QUIET))
      oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "%s:%s", OYJL_DBG_ARGS, key, value );
  }

  return oyjl_ui_json_option_print_cli_text_; 
}


#define OYJL_TREE_COPY_STRING( from_, from_name_, to_, to_name_ ) \
  o = oyjlTreeGetValue(from_, 0, from_name_); t = OYJL_GET_STRING(o); if(!t) oyjlTreeClearValue( to_, to_name_); else { o = oyjlTreeGetValue(to_, OYJL_CREATE_NEW, to_name_); oyjlValueSetString(o, t); }
/** @brief    Set interactive option and format cli args (libOyjlExtra)
 *  @memberof oyjlUi_s
 *
 *  The input format is the JSON data from oyjlUi_ToJson().
 *
 *  @param[in]     root                the parsed JSON tree to convert
 *  @param[in,out] results             the changed option and state
 *  @param[in]     key                 the option key to change
 *  @param[in]     value               the option value to change
 *  @param[in]     current_group_id    the group which was modified
 *  @param[in]     flags               nothing
 *  @return                            the list of command line args
 *
 *  @version Oyjl: 1.0.0
 *  @date    2023/12/21
 *  @since   2023/12/21 (Oyjl: 1.0.0)
 */
oyjl_val oyjlUiJsonSetOption         ( oyjl_val            root,
                                       oyjl_val            results,
                                       const char        * key,
                                       const char        * value,
                                       int                 current_group_id,
                                       int                 flags OYJL_UNUSED )
{
    const char * t;
    oyjl_val args_cli = NULL;
    oyjl_val o = oyjlTreeGetValue(results, 0, "v"); t = OYJL_GET_STRING(o);
    int verbose = t && strcmp(t,"true") == 0;
    if(verbose)
      oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "group[%d] %s:%s", OYJL_DBG_ARGS, current_group_id, key, value );
    const char * k = key;
    const char * v = value;
    if(!results)
        results = oyjlTreeNew(k);

    o = oyjlTreeGetValue(results, OYJL_CREATE_NEW, k);
    oyjlValueSetString(o, v);

    const char * t2, * mandatory, * optional;
    {
      int key_group_id = -1;
      int immediate = 0;
      
      oyjl_val o = oyjlTreeGetValueF( root, 0, "org/freedesktop/oyjl/modules/[0]/nick" );
      const char * nick = OYJL_GET_STRING(o);
      o = oyjlUiJsonFindOption_( root, k, &key_group_id );
      o = oyjlTreeGetValueF( o, 0, "immediate" );
      immediate = o?1:0;
      const char * groups_path = "org/freedesktop/oyjl/modules/[0]/groups";
      oyjl_val groups = oyjlTreeGetValueF( root, 0, groups_path ),
               group;
      if(current_group_id != key_group_id)
        oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "current group[%d] and group of key %d do not match", OYJL_DBG_ARGS, current_group_id, key_group_id );
      if(!groups)
        oyjlMessage_p( oyjlMSG_ERROR, 0, OYJL_DBG_FORMAT "%s not found %d %s", OYJL_DBG_ARGS, groups_path, oyjlValueCount(root), oyjlTreeToText(root,OYJL_YAML) );
      group = oyjlTreeGetValueF( groups, 0, "[%d]", current_group_id );
      if(!group)
        oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "group[%d] not found", OYJL_DBG_ARGS, current_group_id );
      o = oyjlTreeGetValue(group, 0, "name"); t = OYJL_GET_STRING(o);
      if(!t)
      {
        o = oyjlTreeGetValue(group, 0, "description"); t = OYJL_GET_STRING(o);
        o = oyjlTreeGetValue(results, 0, "oyjl_group/description"); t2 = OYJL_GET_STRING(o);
      }
      else
      {
        o = oyjlTreeGetValue(results, 0, "oyjl_group/name"); t2 = OYJL_GET_STRING(o);
      }
      if(o && t && t2 && strcmp(t,t2) != 0)
      {
        OYJL_TREE_COPY_STRING(results, "oyjl_group/name", results, "oyjl_group/old/name")
        OYJL_TREE_COPY_STRING(results, "oyjl_group/description", results, "oyjl_group/old/description")
        OYJL_TREE_COPY_STRING(results, "oyjl_group/mandatory", results, "oyjl_group/old/mandatory")
        OYJL_TREE_COPY_STRING(results, "oyjl_group/optional", results, "oyjl_group/old/optional")
      }
      OYJL_TREE_COPY_STRING(group, "name", results, "oyjl_group/name")
      OYJL_TREE_COPY_STRING(group, "description", results, "oyjl_group/description")
      OYJL_TREE_COPY_STRING(group, "mandatory", results, "oyjl_group/mandatory")
      mandatory = t;
      OYJL_TREE_COPY_STRING(group, "optional", results, "oyjl_group/optional")
      optional = t;

      int index = oyjlOptionGroupGetOptionIndex_( k, mandatory );
      const char * gkey = "oyjl_group";
      if(index < 0)
      {
        gkey = "oyjl_group/old";
        o = oyjlTreeGetValue(results, 0, "oyjl_group/old/mandatory");
        mandatory = OYJL_GET_STRING(o);
        index = oyjlOptionGroupGetOptionIndex_( k, mandatory );
        o = oyjlTreeGetValue(results, 0, "oyjl_group/old/optional");
        optional = OYJL_GET_STRING(o);
      }

      if(index < 0 && immediate)
      {
        gkey = "oyjl_group";
        o = oyjlTreeGetValue(results, 0, "oyjl_group/optional");
        optional = OYJL_GET_STRING(o);
        index = oyjlOptionGroupGetOptionIndex_( k, optional );
        if(index < 0)
        {
          gkey = "oyjl_group/old";
          o = oyjlTreeGetValue(results, 0, "oyjl_group/old/optional");
          optional = OYJL_GET_STRING(o);
          o = oyjlTreeGetValue(results, 0, "oyjl_group/old/mandatory");
          mandatory = OYJL_GET_STRING(o);
          index = oyjlOptionGroupGetOptionIndex_( k, optional );
        } else
        {
          o = oyjlTreeGetValue(results, 0, "oyjl_group/mandatory");
          mandatory = OYJL_GET_STRING(o);
        }
      }

      char * msg = NULL;
      if(index >= 0)
      {
        o = oyjlTreeGetValueF(results, 0, "%s/name", gkey); t = OYJL_GET_STRING(o);
        if(!t) { o = oyjlTreeGetValueF(results, 0, "%s/description", gkey); t = OYJL_GET_STRING(o); if(!t) fprintf(stderr, "no name/description found for group in results[%s/...]\n", gkey);}
        oyjl_val args = NULL;
        oyjlOptionsResultsWithGroupMatches(results, mandatory, &args );
        oyjlOptionsResultsWithGroupMatches(results, optional, &args );
        char * match = oyjlTreeToText( args, OYJL_JSON );
        if(verbose)
          oyjlStringAdd( &msg, 0,0, " match: %s\n key: %s belongs to group: %s (%s[%d])", match?match:"", oyjlTermColor(oyjlBOLD,k), t, mandatory, index );
        if(match) { free(match); match = NULL; }
        args_cli = oyjlTreeNew("");
        int n = oyjlValueCount(args), i, pos;
        for(pos = i = 0; i < n; ++i)
        {
          o = oyjlValuePosGet( args, i );
          char * okey = oyjlTreeGetPath( args, o );
          const char * ovalue = OYJL_GET_STRING(o);
          if(!okey) continue;
          o = oyjlUiJsonFindOption_( root, okey, NULL );
          const char * arg_cli = oyjlUiJsonOptionPrintCli_( o, ovalue, verbose?0:OYJL_QUIET );
          if(arg_cli)
            oyjlTreeSetStringF( args_cli, OYJL_CREATE_NEW, arg_cli, "[%d]", pos++ );
          else
            oyjlTreeClearValue( results, okey );
        }
        if(verbose)
          oyjlStringAdd( &msg, 0,0, " cli: %s %s", nick, oyjlTreeToText( args_cli, OYJL_JSON ) );
      }

      char * json = oyjlTreeToText(results, OYJL_JSON);
      if(verbose)
        oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "results: %s \"%s\": %s %s", OYJL_DBG_ARGS, json, k, v, msg );
      if(json) free(json);
      if(msg) free(msg);
    }

    return args_cli;
}

#define OYJL_TRANSLATE 0x01
#define OYJL_SQUOTE    0x04
#define OYJL_LAST      0x08
static void oyjlStr_AddSpaced_( oyjl_str s, const char * text, int flags, int space )
{
  int len = 0, i,n;
  static int difference = 0;

  if(text) len = strlen(text);
  if(text && (flags & OYJL_TRANSLATE)) len += 2+1;
  if(text && (flags & OYJL_QUOTE))     len += 1+1;
  if(text && (flags & OYJL_SQUOTE))    len += 1+1;

  if(len)
  {
    if(flags & OYJL_TRANSLATE && text && text[0])
      oyjlStr_Push( s, "_(" );
    if(flags & OYJL_QUOTE)
      oyjlStr_Push( s, "\"" );
    if(flags & OYJL_SQUOTE)
      oyjlStr_Push( s, "'" );
    if(flags & OYJL_QUOTE)
    {
      const char * t = text;
      oyjl_str tmp = oyjlStr_New(10,0,0);
      oyjlStr_Push( tmp, t );
      oyjlStr_Replace( tmp, "\\", "\\\\", 0, NULL );
      oyjlStr_Replace( tmp, "\"", "\\\"", 0, NULL );
      oyjlStr_Replace( tmp, "\b", "\\b", 0, NULL );
      oyjlStr_Replace( tmp, "\f", "\\f", 0, NULL );
      oyjlStr_Replace( tmp, "\n", "\\n", 0, NULL );
      oyjlStr_Replace( tmp, "\r", "\\r", 0, NULL );
      oyjlStr_Replace( tmp, "\t", "\\t", 0, NULL );
      t = oyjlStr(tmp); 
      oyjlStr_Push( s, t );
      oyjlStr_Release( &tmp );
    }
    else
      oyjlStr_Push( s, (text && strlen(text) == 0 && flags & OYJL_SQUOTE) ? "\\000" : text );
    if(flags & OYJL_SQUOTE)
      oyjlStr_Push( s, "'" );
    if(flags & OYJL_QUOTE)
      oyjlStr_Push( s, "\"" );
    if(flags & OYJL_TRANSLATE && text && text[0])
      oyjlStr_Push( s, ")" );
  } else
  {
    len = 4;
      oyjlStr_Push( s, "NULL" );
  }

  if(!(flags & OYJL_LAST))
  {
      ++len;
      oyjlStr_Push( s, "," );
  }

  n = space - len;
  if((n + difference) > 0)
  {
    for(i = 0; i < n + difference; ++i)
      oyjlStr_Push( s, " " );
    difference = 0;
  }
  else
    difference += n;

  if(flags & OYJL_LAST)
    difference = 0;
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
  oyjlStr_Push( tmp, *name );
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
static char oyjlUiGetVariableNameSuffixFromWord( const char * o, const char * option )
{
  char c;
  if(!o && !option)
    oyjlMessage_p( oyjlMSG_PROGRAM_ERROR, 0, "arg missed" );
  c = o?o[0]:0;
  if(!c)
    c = option?option[0]:0;
  if(!c)
    c = ' ';
  return c;
}

static oyjl_val oyjlUiExportFindOption_( oyjl_val root, char o )
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

/** @brief    Return a source code from a parsed source tree (libOyjlExtra)
 *  @memberof oyjlUi_s
 *
 *  The input format is the JSON data from oyjlUi_ExportToJson().
 *
 *  @param[in]     root                the parsed JSON tree to convert
 *  @param[in]     flags               support;
 *                                     - ::OYJL_SOURCE_CODE_C
 *                                       - ::OYJL_WITH_OYJL_ARGS_C
 *                                       - ::OYJL_WITH_OYJL_ARGS_BASE_API
 *                                       - ::OYJL_SUGGEST_VARIABLE_NAMES
 *                                       - ::OYJL_NO_DEFAULT_OPTIONS
 *                                     - ::OYJL_COMPLETION_BASH
 *
 *  @version Oyjl: 1.0.0
 *  @date    2023/03/10
 *  @since   2019/06/24 (Oyjl: 1.0.0)
 */
char *             oyjlUiExportToCode( oyjl_val            root,
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
      oyjlStr_Push( s, "/* Compile:\n" );
      oyjlStr_Push( s, " *   cc -Wall -g my-OyjlArgs-enabled-tool.c -DOYJL_HAVE_LOCALE_H -DOYJL_HAVE_LANGINFO_H -DOYJL_HAVE_LIBINTL_H -DOYJL_LOCALEDIR=\"\\\"/usr/local/share/locale\\\"\"  -I oyjl/\n" );
      oyjlStr_Push( s, " */\n" );
      if(flags & OYJL_WITH_OYJL_ARGS_BASE_API)
        oyjlStr_Push( s, "#define OYJL_ARGS_BASE\n" );
      oyjlStr_Push( s, "#include \"oyjl_args.c\"\n" );
    }
    else
    {
      oyjlStr_Push( s, "#include \"oyjl.h\"\n" );
      oyjlStr_Push( s, "#include \"oyjl_version.h\"\n" );
    }
    oyjlStr_Push( s, "extern char **environ;\n" );
    //if(flags & OYJL_WITH_OYJL_ARGS_C)
      //oyjlStr_Push( s, "#define NO_OYJL_ARGS_RENDER\n" );
    if(!(flags & OYJL_WITH_OYJL_ARGS_BASE_API))
    {
      oyjlStr_Push( s, "#ifdef OYJL_HAVE_LOCALE_H\n" );
      oyjlStr_Push( s, "# include <locale.h>\n" );
      oyjlStr_Push( s, "#endif\n" );
      oyjlStr_Push( s, "#define MY_DOMAIN \"oyjl\"\n" );
      oyjlStr_Push( s, "oyjlTranslation_s * trc = NULL;\n" );
      oyjlStr_Push( s, "# ifdef _\n" );
      oyjlStr_Push( s, "# undef _\n" );
      oyjlStr_Push( s, "# endif\n" );
    }
    if(flags & OYJL_WITH_OYJL_ARGS_C)
    {
      if(!(flags & OYJL_WITH_OYJL_ARGS_BASE_API))
      {
        oyjlStr_Push( s, "#if defined (OYJL_HAVE_LIBINTL_H)\n" );
        oyjlStr_Push( s, "# define _(text) dgettext( MY_DOMAIN, text )\n" );
        oyjlStr_Push( s, "#else\n" );
        oyjlStr_Push( s, "# ifdef OYJL_H\n" );
        oyjlStr_Push( s, "#  define _(text) oyjlTranslate( trc, text )\n" );
        oyjlStr_Push( s, "# else\n" );
        oyjlStr_Push( s, "#  define _(text) text\n" );
        oyjlStr_Push( s, "# endif\n" );
        oyjlStr_Push( s, "#endif\n" );
      }
    }
    else
    {
      oyjlStr_Push( s, "# define _(text) oyjlTranslate( trc, text )\n" );
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
        /* the none submitted funtion might be intentional, so skip */
        if(0 && !getChoices)
        {
          /* activate when the function prototype is really figured out */
          char * t = oyjlUiGetVariableNameC_(val, NULL);
          oyjlStringAdd( &tmp, 0,0, "getChoicesForVar_%s", t );
          getChoices = tmp;
          free(t);
        }
        if(getChoices)
        oyjlStr_Add( s, "oyjlOptionChoice_s * %s( oyjlOption_s * opt OYJL_UNUSED, int * selected OYJL_UNUSED, oyjlOptions_s * context OYJL_UNUSED )\n{ fprintf(stderr, \"\\t%%s()\\n\", __func__); return NULL; }\n", getChoices );
        if(tmp) free(tmp);
      }
    }
    oyjlStr_Push( s, "\n" );
    oyjlStr_Push( s, "/* This function is called the\n" );
    oyjlStr_Push( s, " * * first time for GUI generation and then\n" );
    oyjlStr_Push( s, " * * for executing the tool.\n" );
    oyjlStr_Push( s, " */\n" );
    oyjlStr_Push( s, "int myMain( int argc, const char ** argv )\n" );
    oyjlStr_Push( s, "{\n" );
    oyjlStr_Push( s, "  int error = 0;\n" );
    if(!(app_type && strcmp(app_type,"tool") == 0))
      oyjlStr_Push( s, "  int state = oyjlUI_STATE_NO_CHECKS;\n" );
    else
      oyjlStr_Push( s, "  int state = 0;\n" );

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
        oyjlStr_Push( s, "  const char * help = NULL;\n" );
      if(!verbose_found)
        oyjlStr_Push( s, "  int verbose = 0;\n" );
      if(!version_found)
        oyjlStr_Push( s, "  int version = 0;\n" );
      if(!render_found)
        oyjlStr_Push( s, "  const char * render = NULL;\n" );
    }
    if(!export_found)
    oyjlStr_Push( s, "  const char * export_var = 0;\n" );
    oyjlStr_Push( s, "\n" );
    oyjlStr_Push( s, "  /* handle options */\n" );
    oyjlStr_Push( s, "  /* Select a nick from *version*, *manufacturer*, *copyright*, *license*,\n" );
    oyjlStr_Push( s, "   * *url*, *support*, *download*, *sources*, *oyjl_module_author* and\n" );
    oyjlStr_Push( s, "   * *documentation*. Choose what you see fit. Add new ones as needed. */\n" );
    oyjlStr_Push( s, "  oyjlUiHeaderSection_s sections[] = {\n" );
    oyjlStr_Push( s, "    /* type, " );
    oyjlStr_AddSpaced_( s, "nick",        0, 17 );
    oyjlStr_AddSpaced_( s, "label",       0, 7 );
    oyjlStr_AddSpaced_( s, "name",        0, 26 );
    oyjlStr_AddSpaced_( s, "description", OYJL_LAST, 4 );
    oyjlStr_Push(       s, " */\n" );
    n = oyjlValueCount( oyjlTreeGetValue( root, 0, "org/freedesktop/oyjl/ui/header/sections" ) );
    for(i = 0; i < n; ++i)
    {
      const char *nick, *label, *name, *desc;
      val = oyjlTreeGetValueF( root, 0, "org/freedesktop/oyjl/ui/header/sections/[%d]", i );
      v = oyjlTreeGetValue( val, 0, "nick" ); nick = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "label" ); label = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "name" ); name = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "description" ); desc = OYJL_GET_STRING(v);
      oyjlStr_Push( s, "    {\"oihs\", ");

      oyjlStr_AddSpaced_( s, nick,  OYJL_QUOTE,                17 );
      oyjlStr_AddSpaced_( s, label, OYJL_QUOTE|OYJL_TRANSLATE, 7 );
      oyjlStr_AddSpaced_( s, name&&name[0]?name:NULL,  OYJL_QUOTE|OYJL_TRANSLATE, 26 );
      oyjlStr_AddSpaced_( s, desc,  OYJL_QUOTE|OYJL_TRANSLATE|OYJL_LAST, 4 );
      oyjlStr_Push( s, "},\n");
    }
    oyjlStr_Push( s, "    {\"\",0,0,0,0}};\n" );
    oyjlStr_Push( s, "\n" );
    oyjlStr_Push( s,       "  /* declare the option choices  *   " );
    oyjlStr_AddSpaced_( s, "nick",         0, 15 );
    oyjlStr_AddSpaced_( s, "name",         0, 20 );
    oyjlStr_AddSpaced_( s, "description",  0, 30 );
    oyjlStr_AddSpaced_( s, "help",         OYJL_LAST, 4 );
    oyjlStr_Push( s,       " */\n" );
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
        {
          char c = oyjlUiGetVariableNameSuffixFromWord(o_fallback,option);
          oyjlStr_Add( s, "  oyjlOptionChoice_s %c_choices[] = {", c );
        }
        for(j = 0; j < count; ++j)
        {
          oyjl_val c = oyjlTreeGetValueF( val, 0, "values/choices/list/[%d]", j );
          v = oyjlTreeGetValue( c, 0, "nick" ); nick = OYJL_GET_STRING(v);
          v = oyjlTreeGetValue( c, 0, "name" ); name = OYJL_GET_STRING(v);
          v = oyjlTreeGetValue( c, 0, "description" ); desc = OYJL_GET_STRING(v);
          v = oyjlTreeGetValue( c, 0, "help" ); help = OYJL_GET_STRING(v);
          if(j)
          oyjlStr_Push( s, "                                    {");
          else
            oyjlStr_Push( s, "{");

          if(option && strstr(option, "man-") != NULL)
            oyjlStr_AddSpaced_(s,nick,  OYJL_QUOTE|OYJL_TRANSLATE, 15 );
          else
            oyjlStr_AddSpaced_(s,nick,  OYJL_QUOTE,                15 );
          oyjlStr_AddSpaced_( s, name,  OYJL_QUOTE|OYJL_TRANSLATE, 20 );
          oyjlStr_AddSpaced_( s, desc,  OYJL_QUOTE|OYJL_TRANSLATE, 30 );
          oyjlStr_AddSpaced_( s, help,  OYJL_QUOTE|OYJL_TRANSLATE|OYJL_LAST, 4 );
          oyjlStr_Push( s, "},\n");
        }
        if(count)
        {
          oyjlStr_Push( s, "                                    {NULL,NULL,NULL,NULL}};\n" );
          oyjlStr_Push( s, "\n" );
        }
      }
    }
    oyjlStr_Push( s, "  /* declare options - the core information; use previously declared choices */\n" );
    oyjlStr_Push( s, "  oyjlOption_s oarray[] = {\n" );
    oyjlStr_Push( s, "  /* type,   " );
    oyjlStr_AddSpaced_( s, "flags",        0, 28 );
    oyjlStr_AddSpaced_( s, "o",            0, 4 );
    oyjlStr_AddSpaced_( s, "option",       0, 17 );
    oyjlStr_AddSpaced_( s, "key",          0, 10 );
    oyjlStr_AddSpaced_( s, "name",         0, 15 );
    oyjlStr_AddSpaced_( s, "description",  0, 30 );
    oyjlStr_AddSpaced_( s, "help",         0, 6 );
    oyjlStr_AddSpaced_( s, "value_name",   0, 20 );
    oyjlStr_Push( s,       "\n        " );
    oyjlStr_AddSpaced_( s, "value_type",   0, 25 );
    oyjlStr_AddSpaced_( s, "values",       0, 20 );
    oyjlStr_AddSpaced_( s, "variable_type",0, 15 );
    oyjlStr_AddSpaced_( s, "variable_name",0, 15 );
    oyjlStr_AddSpaced_( s, "properties", OYJL_LAST, 4 );
    oyjlStr_Push( s,       " */\n" );
    for(i = 0; i < n; ++i)
    {
      char * flag_string = NULL;
      int flg, j, len;
      char oo[4] = {0,0,0,0}, *tmp_name = NULL, *tmp_variable_name = NULL, * properties = NULL, *t = NULL;
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
            oyjl_val nv = oyjlUiExportFindOption_( root, name[j] );
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
      v = oyjlTreeGetValue( val, 0, "properties" );
      if(v)
      {
        int count = oyjlValueCount( v ), j;
        char ** keys = oyjlTreeToPaths( v, OYJL_KEY, NULL, 0, &count );
        if(*oyjl_debug)
          oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "%s found %d properties", OYJL_DBG_ARGS, o?o:key, count );
        for(j = 0; j < count; ++j)
        {
          char * key = keys[j];
          oyjl_val val = oyjlTreeGetValue( v, 0, key );
          const char * value = OYJL_GET_STRING( val );
          if(value)
            oyjlStringAdd( &properties, 0,0, "%s%s=%s", properties?"\n":"", key, value ); 
        }
      }
      oyjlStr_Push( s, "    {\"oiwi\", ");
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
      oyjlStr_AddSpaced_( s, value_name,  OYJL_QUOTE|OYJL_TRANSLATE|OYJL_LAST, 2 );
      oyjlStr_Push( s,       ",\n        " );
      oyjlStr_AddSpaced_( s, value_type && value_type[0] ? value_type : "oyjlOPTIONTYPE_NONE",  0,                         25 );
      if(value_type && strcmp(value_type, "oyjlOPTIONTYPE_CHOICE") == 0)
      {
        int count = oyjlValueCount( oyjlTreeGetValue( val, 0, "values/choices/list" ) );
        if(count)
        {
          char c = oyjlUiGetVariableNameSuffixFromWord(o,option);
          oyjlStringAdd( &t, 0,0, "{.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)%c_choices, sizeof(%c_choices), malloc ), 0}}", c,c );
          oyjlStr_AddSpaced_(s,t,            0, 20 );
          if(t) { free( t ); t = NULL; }
        }
        else
          oyjlStr_AddSpaced_(s,"{0}",        0, 20 );
      } else
      if(value_type && strcmp(value_type, "oyjlOPTIONTYPE_FUNCTION") == 0)
      {
        const char * getChoices;
        int j;
        v = oyjlTreeGetValue( val, 0, "values/getChoices" ); getChoices = OYJL_GET_STRING(v);
        if(getChoices)
        {
          oyjlStringAdd( &t, 0,0, "{.getChoices = %s}", getChoices );
          oyjlStr_AddSpaced_(s,t,            0, 20 );
          if(t) { free( t ); t = NULL; }
        }
        else
          oyjlStr_AddSpaced_(s,"{0}",        0, 20 );
        if(getChoices && strlen(getChoices) < 2)
        {
          int len = strlen(getChoices);
          for(j = 0; j < len - 18; ++j)
            oyjlStr_Push( s, " " );
        }

      } else
      if(value_type && strcmp(value_type, "oyjlOPTIONTYPE_DOUBLE") == 0)
      {
        double d, start, tick, end;
        v = oyjlTreeGetValue( val, 0, "values/dbl/d" ); d = OYJL_IS_DOUBLE(v) ? OYJL_GET_DOUBLE(v) : -1.0;
        v = oyjlTreeGetValue( val, 0, "values/dbl/start" ); start = OYJL_IS_DOUBLE(v) ? OYJL_GET_DOUBLE(v) : -1.0;
        v = oyjlTreeGetValue( val, 0, "values/dbl/tick" ); tick = OYJL_IS_DOUBLE(v) ? OYJL_GET_DOUBLE(v) : -1.0;
        v = oyjlTreeGetValue( val, 0, "values/dbl/end" ); end = OYJL_IS_DOUBLE(v) ? OYJL_GET_DOUBLE(v) : -1.0;

        oyjlStringAdd( &t, 0,0, "{.dbl = {.d = %g, .start = %g, .end = %g, .tick = %g}}", d == -1.0 ? 0 : d, start, end, tick == -1.0 ? 0.0 : tick );
        if(!(start == -1.0 && end == -1.0 && tick == -1.0 && d == 1.0))
          oyjlStr_AddSpaced_( s, t,        0, 4 );
        else
          oyjlStr_AddSpaced_( s, "{0}",    0, 4 );
        if(t) { free( t ); t = NULL; }
      } else
      if(!value_type || (value_type && !value_type[0]) || (value_type && strcmp(value_type, "oyjlOPTIONTYPE_NONE") == 0))
      {
        oyjlStr_AddSpaced_(s,"{0}",        0, 20 );
      }
      oyjlStr_AddSpaced_( s, variable_type,0,                         15 );
      if(variable_type && strcmp(variable_type, "oyjlNONE") == 0)
        oyjlStr_AddSpaced_( s, variable_name?variable_name:"{0}", 0,                 15 );
      else
      {
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
        oyjlStr_AddSpaced_( s, t, 0,                 15 );
        if(t) { free( t ); t = NULL; }
      }
      if(properties && properties[0])
        oyjlStr_AddSpaced_( s, properties,   OYJL_QUOTE|OYJL_LAST, 4 );
      else
        oyjlStr_AddSpaced_( s, "NULL", OYJL_LAST,    4);
      oyjlStr_Add( s, "},\n" );
      if(flag_string) free( flag_string );
      if(properties) free( properties );
      if(tmp_name) free( tmp_name );
    }
    if(!(flags & OYJL_NO_DEFAULT_OPTIONS))
    {
      if(!help_found && !oyjlUiExportFindOption_( root, 'h' ))
      oyjlStr_Push(s,"    /* default options -h and -v */\n" );
      if(!help_found && !oyjlUiExportFindOption_( root, 'h' ))
      oyjlStr_Push(s,"    {\"oiwi\", OYJL_OPTION_FLAG_ACCEPT_NO_ARG, \"h\", \"help\", NULL, NULL, NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s=&help}, NULL },\n" );
      if(!verbose_found && !oyjlUiExportFindOption_( root, 'v' ))
      oyjlStr_Push(s,"    {\"oiwi\", 0, \"v\", \"verbose\", NULL, _(\"verbose\"), _(\"Verbose\"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&verbose}, NULL },\n" );
      if(!render_found && !oyjlUiExportFindOption_( root, 'R' ))
      {
      oyjlStr_Push(s,"    /* The --render option can be hidden and used only internally. */\n" );
      oyjlStr_Push(s,"    {\"oiwi\", OYJL_OPTION_FLAG_EDITABLE|OYJL_OPTION_FLAG_MAINTENANCE, \"R\", \"render\",  NULL, _(\"render\"),  _(\"Render\"),  NULL, NULL, oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s=&render}, NULL },\n" );
      }
      if(!version_found && !oyjlUiExportFindOption_( root, 'V' ))
      oyjlStr_Push(s,"    {\"oiwi\", 0, \"V\", \"version\", NULL, _(\"version\"), _(\"Version\"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&version}, NULL },\n" );
      oyjlStr_Push(s,"    /* default option template -X|--export */\n" );
      if(!export_found && !X_found)
      oyjlStr_Push(s,"    {\"oiwi\", 0, \"X\", \"export\", NULL, NULL, NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices = {NULL, 0}}, oyjlSTRING, {.s=&export_var}, NULL },\n" );
    }
    oyjlStr_Push( s, "    {\"\",0,0,NULL,NULL,NULL,NULL,NULL, NULL, oyjlOPTIONTYPE_END, {0},oyjlNONE,{0},0}\n  };\n\n" );
    oyjlStr_Push( s, "  /* declare option groups, for better syntax checking and UI groups */\n" );
    oyjlStr_Push( s, "  oyjlOptionGroup_s groups[] = {\n" );
    oyjlStr_Push( s, "  /* type,   " );
    oyjlStr_AddSpaced_( s, "flags",        0, 7 );
    oyjlStr_AddSpaced_( s, "name",         0, 20 );
    oyjlStr_AddSpaced_( s, "description",  0, 30 );
    oyjlStr_AddSpaced_( s, "help",         0, 20 );
    oyjlStr_AddSpaced_( s, "mandatory",    0, 15 );
    oyjlStr_AddSpaced_( s, "optional",     0, 15 );
    oyjlStr_AddSpaced_( s, "detail",       0, 15 );
    oyjlStr_AddSpaced_( s, "properties",   OYJL_LAST, 4 );
    oyjlStr_Push( s,       " */\n" );
    n = oyjlValueCount( oyjlTreeGetValue( root, 0, "org/freedesktop/oyjl/ui/options/groups" ) );
    for(i = 0; i < n; ++i)
    {
      const char *name, *desc, *help, *mandatory, *optional, *detail;
      char * flag_string = NULL, * properties = NULL;
      int flg;
      val = oyjlTreeGetValueF( root, 0, "org/freedesktop/oyjl/ui/options/groups/[%d]", i );
      v = oyjlTreeGetValue( val, 0, "name" ); name = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "description" ); desc = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "help" ); help = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "mandatory" ); mandatory = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "optional" ); optional = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "detail" ); detail = OYJL_GET_STRING(v);
      v = oyjlTreeGetValue( val, 0, "properties" );
      if(v)
      {
        int count = oyjlValueCount( v ), j;
        char ** keys = oyjlTreeToPaths( v, OYJL_KEY, NULL, 0, &count );
        if(*oyjl_debug)
          oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "%i %s found properties: %d", OYJL_DBG_ARGS, i, name, count );
        for(j = 0; j < count; ++j)
        {
          char * key = keys[j];
          oyjl_val val = oyjlTreeGetValue( v, 0, key );
          const char * value = OYJL_GET_STRING( val );
          if(value)
            oyjlStringAdd( &properties, 0,0, "%s%s=%s", properties?"\n":"", key, value ); 
        }
      }
      oyjlStr_Push( s, "    {\"oiwg\", ");
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
      oyjlStr_AddSpaced_( s, detail,      OYJL_QUOTE,                15 );
      oyjlStr_AddSpaced_( s, properties,       OYJL_QUOTE|OYJL_LAST,      4 );
      oyjlStr_Push( s, "},\n");
      if(flag_string) free( flag_string );
      if(properties) free( properties );
    }
    oyjlStr_Push( s, "    {\"\",0,0,0,0,0,0,0,0}\n" );
    oyjlStr_Push( s, "  };\n\n");
    const char *nick, *name, *description, *logo;
    val = oyjlTreeGetValue( root, 0, OYJL_REG "/ui/nick" ); nick = OYJL_GET_STRING(val);
    val = oyjlTreeGetValue( root, 0, OYJL_REG "/ui/name" ); name = OYJL_GET_STRING(val);
    val = oyjlTreeGetValue( root, 0, OYJL_REG "/ui/description" ); description = OYJL_GET_STRING(val);
    val = oyjlTreeGetValue( root, 0, OYJL_REG "/ui/logo" ); logo = OYJL_GET_STRING(val);
    oyjlStr_Push( s, "  oyjlUi_s * ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */\n" );
    oyjlStr_Add( s, "                                       \"%s\", ", nick );
    if(name)
      oyjlStr_Add( s, "_(\"%s\"), ", name );
    else
      oyjlStr_Push( s, "NULL, " );
    if(description)
      oyjlStr_Add( s, "_(\"%s\"),\n", description );
    else
      oyjlStr_Push( s, "NULL,\n" );
    oyjlStr_Push( s, "#ifdef __ANDROID__\n" );
    oyjlStr_Push( s, "                                       \":/images/logo.svg\", // use qrc\n" );
    oyjlStr_Push( s, "#else\n" );
    if(logo)
      oyjlStr_Add( s, "                                       \"%s\",\n", logo );
    else
      oyjlStr_Push( s, "                                       NULL,\n" );
    oyjlStr_Push( s, "#endif\n" );
    oyjlStr_Push( s, "                                       sections, oarray, groups, &state );\n" );
    if(!(app_type && strcmp(app_type,"tool") == 0))
    oyjlStr_Add( s, "  if(ui) ui->app_type = \"%s\";\n", app_type ? app_type : "lib" );
    if(!help_found && h_found)
      oyjlStr_Push( s, "  help = h ? \"\" : NULL;\n" );
    if(!verbose_found && v_found)
    {
      if(v_is_string)
        oyjlStr_Push( s, "  verbose = v ? 1 : 0;\n" );
      else
        oyjlStr_Push( s, "  verbose = v;\n" );
    }
    oyjlStr_Push( s, "  if( state & oyjlUI_STATE_EXPORT && !ui )\n" );
    oyjlStr_Push( s, "    goto clean_main;\n");
    oyjlStr_Push( s, "  if(state & oyjlUI_STATE_HELP)\n" );
    oyjlStr_Push( s, "  {\n" );
    oyjlStr_Add( s, "    fprintf( stderr, \"%%s\\n\\tman %s\\n\\n\", _(\"For more information read the man page:\") );\n", nick );
    oyjlStr_Push( s, "    goto clean_main;\n" );
    oyjlStr_Push( s, "  }\n" );
    oyjlStr_Push( s, "\n" );
    oyjlStr_Push( s, "  if(ui && verbose)\n" );
    oyjlStr_Push( s, "  {\n" );
    if(!(flags & OYJL_WITH_OYJL_ARGS_C))
    {
      oyjlStr_Push( s, "    char * json = oyjlOptions_ResultsToJson( ui->opts, OYJL_JSON );\n" );
      oyjlStr_Push( s, "    if(json)\n" );
      oyjlStr_Push( s, "      fputs( json, stderr );\n" );
      oyjlStr_Push( s, "    fputs( \"\\n\", stderr );\n" );
      oyjlStr_Push( s, "\n" );
    }
    oyjlStr_Push( s, "    int count = 0, i;\n" );
    oyjlStr_Push( s, "    char ** results = oyjlOptions_ResultsToList( ui->opts, NULL, &count );\n" );
    oyjlStr_Push( s, "    for(i = 0; i < count; ++i) fprintf( stderr, \"\%s\\n\", results[i] );\n" );
    oyjlStr_Push( s, "    oyjlStringListRelease( &results, count, free );\n" );
    oyjlStr_Push( s, "    fputs( \"\\n\", stderr );\n" );
    oyjlStr_Push( s, "  }\n" );
    oyjlStr_Push( s, "\n" );
    oyjlStr_Push( s, "  if(ui && (export_var && strcmp(export_var,\"json+command\") == 0))\n" );
    oyjlStr_Push( s, "  {\n" );
    if(flags & OYJL_WITH_OYJL_ARGS_C)
      oyjlStr_Push( s, "    char * json = oyjlUi_ToTextArgsBase( ui, oyjlARGS_EXPORT_JSON, 0 ),\n" );
    else
      oyjlStr_Push( s, "    char * json = oyjlUi_ToText( ui, oyjlARGS_EXPORT_JSON, 0 ),\n" );
    oyjlStr_Push( s, "         * json_commands = NULL;\n" );
    if(flags & OYJL_WITH_OYJL_ARGS_BASE_API)
    {
      oyjlStr_Push( s, "    oyjlStringPush( &json_commands, \"{\\n  \\\"command_set\\\": \\\"\", malloc, free );\n" );
      oyjlStr_Push( s, "    oyjlStringPush( &json_commands, argv[0], malloc, free );\n" );
      oyjlStr_Push( s, "    oyjlStringPush( &json_commands, \"\\\",\", malloc, free );\n" );
      oyjlStr_Push( s, "    oyjlStringPush( &json_commands, &json[1], malloc, free ); /* skip opening '{' */\n" );
    } else
    {
      oyjlStr_Push( s, "    oyjlStringAdd( &json_commands, malloc, free, \"{\\n  \\\"command_set\\\": \\\"%s\\\",\", argv[0] );\n" );
      oyjlStr_Push( s, "    oyjlStringAdd( &json_commands, malloc, free, \"%s\", &json[1] ); /* skip opening '{' */\n" );
    }
    oyjlStr_Push( s, "    puts( json_commands );\n" );
    oyjlStr_Push( s, "    goto clean_main;\n" );
    oyjlStr_Push( s, "  }\n" );
    oyjlStr_Push( s, "\n" );
    oyjlStr_Push( s, "  /* Render boilerplate */\n" );
    oyjlStr_Push( s, "  if(ui && render)\n" );
    oyjlStr_Push( s, "  {\n" );
    oyjlStr_Push( s, "#if !defined(NO_OYJL_ARGS_RENDER)\n" );
    oyjlStr_Push( s, "    int debug = verbose;\n" );
    if(!(flags & OYJL_WITH_OYJL_ARGS_BASE_API))
    oyjlStr_Push( s, "    oyjlTermColorInit( OYJL_RESET_COLORTERM | OYJL_FORCE_COLORTERM ); /* show rich text format on non GNU color extension environment */\n" );
    oyjlStr_Push( s, "    oyjlArgsRender( argc, argv, NULL, NULL,NULL, debug, ui, myMain );\n" );
    oyjlStr_Push( s, "#else\n" );
    oyjlStr_Push( s, "    fprintf( stderr, \"No render support compiled in. For a GUI you might by able to use -X json+command and load into oyjl-args-render viewer.\\n\" );\n" );
    oyjlStr_Push( s, "#endif\n" );
    oyjlStr_Push( s, "  } else if(ui)\n" );
    oyjlStr_Push( s, "  {\n" );
    oyjlStr_Push( s, "    /* ... working code goes here ... */\n" );
    oyjlStr_Push( s, "  }\n" );
    oyjlStr_Push( s, "  else error = 1;\n" );
    oyjlStr_Push( s, "\n" );
    oyjlStr_Push( s, "  clean_main:\n" );
    oyjlStr_Push( s, "  {\n" );
    oyjlStr_Push( s, "    int i = 0;\n" );
    oyjlStr_Push( s, "    while(oarray[i].type[0])\n" );
    oyjlStr_Push( s, "    {\n" );
    oyjlStr_Push( s, "      if(oarray[i].value_type == oyjlOPTIONTYPE_CHOICE && oarray[i].values.choices.list)\n" );
    oyjlStr_Push( s, "        free(oarray[i].values.choices.list);\n" );
    oyjlStr_Push( s, "      ++i;\n" );
    oyjlStr_Push( s, "    }\n" );
    oyjlStr_Push( s, "  }\n" );
    if(flags & OYJL_WITH_OYJL_ARGS_C)
      oyjlStr_Push(s,"  oyjlUi_ReleaseArgs( &ui );\n" );
    else
      oyjlStr_Push(s,"  oyjlUi_Release( &ui );\n" );
    oyjlStr_Push( s, "\n" );
    oyjlStr_Push( s, "  return error;\n" );
    oyjlStr_Push( s, "}\n" );
    oyjlStr_Push( s, "\n" );
    oyjlStr_Push( s, "extern int * oyjl_debug;\n" );
    oyjlStr_Push( s, "char ** environment = NULL;\n" );
    oyjlStr_Push( s, "int main( int argc_, char**argv_, char ** envv )\n" );
    oyjlStr_Push( s, "{\n" );
    oyjlStr_Push( s, "  int argc = argc_;\n" );
    oyjlStr_Push( s, "  char ** argv = argv_;\n" );
    if(!(flags & OYJL_WITH_OYJL_ARGS_BASE_API))
    {
      oyjlStr_Push( s, "  oyjlTranslation_s * trc_ = NULL;\n" );
      oyjlStr_Push( s, "  const char * loc = NULL;\n" );
      oyjlStr_Push( s, "  const char * lang;\n" );
    }
    oyjlStr_Push( s, "\n" );
    oyjlStr_Push( s, "#ifdef __ANDROID__\n" );
    oyjlStr_Push( s, "  argv = calloc( argc + 2, sizeof(char*) );\n" );
    oyjlStr_Push( s, "  memcpy( argv, argv_, (argc + 2) * sizeof(char*) );\n" );
    oyjlStr_Push( s, "  argv[argc++] = \"--render=gui\"; /* start Renderer (e.g. QML) */\n" );
    oyjlStr_Push( s, "  environment = environ;\n" );
    oyjlStr_Push( s, "#else\n" );
    oyjlStr_Push( s, "  environment = envv;\n" );
    oyjlStr_Push( s, "#endif\n" );
    oyjlStr_Push( s, "\n" );
    if(!(flags & OYJL_WITH_OYJL_ARGS_BASE_API))
    {
      oyjlStr_Push( s, "  /* language needs to be initialised before setup of data structures */\n" );
      oyjlStr_Push( s, "  int use_gettext = 0;\n" );
      oyjlStr_Push( s, "#ifdef OYJL_HAVE_LIBINTL_H\n" );
      oyjlStr_Push( s, "  use_gettext = 1;\n" );
      oyjlStr_Push( s, "#endif\n" );
      oyjlStr_Push( s, "#ifdef OYJL_HAVE_LOCALE_H\n" );
      oyjlStr_Push( s, "  loc = oyjlSetLocale(LC_ALL,\"\");\n" );
      oyjlStr_Push( s, "#endif\n" );
      oyjlStr_Push( s, "  lang = getenv(\"LANG\");\n" );
      oyjlStr_Push( s, "  if(!loc)\n" );
      oyjlStr_Push( s, "  {\n" );
      oyjlStr_Push( s, "    loc = lang;\n" );
      oyjlStr_Push( s, "    fprintf( stderr, \"\%s\", oyjlTermColor(oyjlRED,\"Usage Error:\") );\n" );
      oyjlStr_Add( s, "    fprintf( stderr, \" Environment variable possibly not correct. Translations might fail - LANG=%%s\\n\", oyjlTermColor(oyjlBOLD,lang) );\n" );
      oyjlStr_Push( s, "  }\n" );
      oyjlStr_Push( s, "  if(lang)\n" );
      oyjlStr_Push( s, "    loc = lang;\n" );
      oyjlStr_Push( s, "  if(loc)\n" );
      oyjlStr_Push( s, "  {\n" );
      oyjlStr_Push( s, "    const char * my_domain = MY_DOMAIN;\n" );
      oyjlStr_Push( s, "    if(my_domain && strcmp(my_domain,\"oyjl\") == 0)\n" );
      oyjlStr_Push( s, "      my_domain = NULL;\n" );
      oyjlStr_Push( s, "    trc = trc_ = oyjlTranslation_New( loc, my_domain, 0,0,0,0,0 );\n" );
      oyjlStr_Push( s, "  }\n" );
      oyjlStr_Push( s, "  oyjlInitLanguageDebug( \"Oyjl\", \"OYJL_DEBUG\", oyjl_debug, use_gettext, \"OYJL_LOCALEDIR\", OYJL_LOCALEDIR, &trc_, NULL );\n" );
      oyjlStr_Push( s, "  if(MY_DOMAIN && strcmp(MY_DOMAIN,\"oyjl\") == 0)\n" );
      oyjlStr_Push( s, "    trc = oyjlTranslation_Get( MY_DOMAIN );\n" );
    }
    oyjlStr_Push( s, "\n" );
    oyjlStr_Push( s, "  myMain(argc, (const char **)argv);\n" );
    oyjlStr_Push( s, "\n" );
    if(!(flags & OYJL_WITH_OYJL_ARGS_BASE_API))
    {
      oyjlStr_Push( s, "  oyjlTranslation_Release( &trc_ );\n" );
      oyjlStr_Push( s, "  oyjlLibRelease();\n" );
    }
    oyjlStr_Push( s, "\n" );
    oyjlStr_Push( s, "#ifdef __ANDROID__\n" );
    oyjlStr_Push( s, "  free( argv );\n" );
    oyjlStr_Push( s, "#endif\n" );
    oyjlStr_Push( s, "\n" );
    oyjlStr_Push( s, "  return 0;\n" );
    oyjlStr_Push( s, "}\n" );
    oyjlStr_Push( s, "\n" );
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
    oyjlStr_Push( s, "{\n" );
    oyjlStr_Push( s, "    local cur prev words cword\n" );
    oyjlStr_Push( s, "    _init_completion -s || return\n" );
    oyjlStr_Push( s, "\n" );
    oyjlStr_Push( s, "    #set -x -v\n" );
    oyjlStr_Push( s, "\n" );
    oyjlStr_Push( s, "    local SEARCH=${COMP_WORDS[COMP_CWORD]}\n" );
    oyjlStr_Push( s, "    if [[ \"$SEARCH\" == \"=\" ]]; then\n" );
    oyjlStr_Push( s, "      SEARCH=\"\"\n" );
    oyjlStr_Push( s, "    fi\n" );
    oyjlStr_Push( s, "\n" );
    oyjlStr_Push( s, "    : \"autocomplete options with choices for long options \"$prev\"\"\n" );
    oyjlStr_Push( s, "    case \"$prev\" in\n" );
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
            oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "found getChoicesCompletionBash: `%s` for --%s", OYJL_DBG_ARGS, getChoices, oyjlTermColor( oyjlITALIC, option ) );
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
            oyjlStr_Push( s, "            local OYJL_TEXTS\n" );
            oyjlStr_Push( s, "            if [[ \"${COMP_WORDS[COMP_CWORD]}\" == \"=\" ]]; then\n" );
            oyjlStr_Push( s, "              OYJL_TEXTS=$(${COMP_LINE}oyjl-list | sed 's/\\[/_/g;s/\\]/_/g')\n" );
            oyjlStr_Push( s, "            else\n" );
            oyjlStr_Add( s, "              OYJL_TEXTS=$(${COMP_LINE} --%s=oyjl-list | sed 's/\\[/_/g;s/\\]/_/g')\n", option );
            oyjlStr_Push( s, "            fi\n" );
          }
          oyjlStr_Push( s, "            local IFS=$'\\n'\n" );
          oyjlStr_Push( s, "            local WORD_LIST=()\n" );
          oyjlStr_Push( s, "            for OYJL_TEXT in $OYJL_TEXTS\n" );
          oyjlStr_Push( s, "              do WORD_LIST=(\"${WORD_LIST[@]}\" \"$OYJL_TEXT\")\n" );
          oyjlStr_Push( s, "            done\n" );
          oyjlStr_Push( s, "            COMPREPLY=($(compgen -W '\"${WORD_LIST[@]}\"' -- \"$cur\"))\n" );
          oyjlStr_Push( s, "            set +x +v\n" );
          oyjlStr_Push( s, "            return\n" );
          oyjlStr_Push( s, "            ;;\n" );
        }
      } else
      if(value_type && strcmp(value_type, "oyjlOPTIONTYPE_CHOICE") == 0)
      {
        const char *nick;
        int count = oyjlValueCount( oyjlTreeGetValue( val, 0, "values/choices/list" ) ), j;
        if(count)
        {
          oyjlStr_Add( s, "        --%s) # long option with static args\n", option );
          oyjlStr_Push( s, "            local IFS=$'\\n'\n" );
          oyjlStr_Push( s, "            local WORD_LIST=(" );
        }
        for(j = 0; j < count; ++j)
        {
          oyjl_val c = oyjlTreeGetValueF( val, 0, "values/choices/list/[%d]", j );
          v = oyjlTreeGetValue( c, 0, "nick" ); nick = OYJL_GET_STRING(v);
          oyjlStr_Add( s, "%s'%s'", j?" ":"", nick );
        }
        if(count)
        {
          oyjlStr_Push( s, ")\n" );
          oyjlStr_Push( s, "            COMPREPLY=($(compgen -W '\"${WORD_LIST[@]}\"' -- \"$cur\"))\n" );
          oyjlStr_Push( s, "            set +x +v\n" );
          oyjlStr_Push( s, "            return\n" );
          oyjlStr_Push( s, "            ;;\n" );
        }
      }
    }
    oyjlStr_Push( s, "    esac\n" );
    oyjlStr_Push( s, "    : \"autocomplete options with choices for single letter options \"$cur\"\"\n" );
    oyjlStr_Push( s, "    case \"$cur\" in\n" );
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
        {
          if(use_getChoicesCompletionBash)
            found_at_arg_func = getChoices;
          else
            found_at_arg_func = "${COMP_LINE} oyjl-list | sed 's/\\[/_/g;s/\\]/_/g'";
        }
        else
        {
          if(*oyjl_debug)
            oyjlMessage_p( oyjlMSG_INFO, 0, "found getChoicesCompletionBash: `%s` for -%s", getChoices, oyjlTermColor( oyjlITALIC, o ) );
          oyjlStr_Add( s, "        -%s=*) # single letter option with dynamic args\n", o );
          if(use_getChoicesCompletionBash)
            oyjlStr_Add( s, "            local OYJL_TEXTS=$(%s)\n", getChoices );
          else
          {
            oyjlStr_Push( s, "            local OYJL_TEXTS\n" );
            oyjlStr_Push( s, "            if [[ \"${COMP_WORDS[COMP_CWORD]}\" == \"=\" ]]; then\n" );
            oyjlStr_Push( s, "              OYJL_TEXTS=$(${COMP_LINE}oyjl-list | sed 's/\\[/_/g;s/\\]/_/g')\n" );
            oyjlStr_Push( s, "            else\n" );
            oyjlStr_Add( s, "              OYJL_TEXTS=$(${COMP_LINE} -%s=oyjl-list | sed 's/\\[/_/g;s/\\]/_/g')\n", o );
            oyjlStr_Push( s, "            fi\n" );
          }
          oyjlStr_Push( s, "            local IFS=$'\\n'\n" );
          oyjlStr_Push( s, "            local WORD_LIST=()\n" );
          oyjlStr_Push( s, "            for OYJL_TEXT in $OYJL_TEXTS\n" );
          oyjlStr_Push( s, "              do WORD_LIST=(\"${WORD_LIST[@]}\" \"$OYJL_TEXT\")\n" );
          oyjlStr_Push( s, "            done\n" );
          oyjlStr_Push( s, "            COMPREPLY=($(compgen -W '\"${WORD_LIST[@]}\"' -- \"$SEARCH\"))\n" );
          oyjlStr_Push( s, "            set +x +v\n" );
          oyjlStr_Push( s, "            return\n" );
          oyjlStr_Push( s, "            ;;\n" );
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
          oyjlStr_Push( s, "            local IFS=$'\\n'\n" );
          oyjlStr_Push( s, "            local WORD_LIST=(" );
        }
        for(j = 0; j < count; ++j)
        {
          oyjl_val c = oyjlTreeGetValueF( val, 0, "values/choices/list/[%d]", j );
          v = oyjlTreeGetValue( c, 0, "nick" ); nick = OYJL_GET_STRING(v);
          oyjlStr_Add( s, "%s'%s'", j?" ":"", nick );
        }
        if(count)
        {
          oyjlStr_Push( s, ")\n" );
          oyjlStr_Push( s, "            COMPREPLY=($(compgen -W '\"${WORD_LIST[@]}\"' -- \"$SEARCH\"))\n" );
          oyjlStr_Push( s, "            set +x +v\n" );
          oyjlStr_Push( s, "            return\n" );
          oyjlStr_Push( s, "            ;;\n" );
        }
      }
    }
    oyjlStr_Push( s, "    esac\n" );
    oyjlStr_Push( s, "\n" );
    oyjlStr_Push( s, "\n" );
    oyjlStr_Push( s, "    : \"autocomplete options \"$cur\"\"\n" );
    oyjlStr_Push( s, "    case \"$cur\" in\n" );
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
          oyjlStr_Push( s, "        " );
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
      oyjlStr_Push( s, ")\n" );
      oyjlStr_Push( s, "            : \"finish short options with choices\"\n" );
      oyjlStr_Push( s, "            COMPREPLY=(\"$cur=\\\"\")\n" );
      oyjlStr_Push( s, "            set +x +v\n" );
      oyjlStr_Push( s, "            return\n" );
      oyjlStr_Push( s, "            ;;\n" );
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
          oyjlStr_Push( s, "        " );
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
      oyjlStr_Push( s, ")\n" );
      oyjlStr_Push( s, "            : \"finish long options with choices\"\n" );
      oyjlStr_Push( s, "            COMPREPLY=(\"$cur=\\\"\")\n" );
      oyjlStr_Push( s, "            set +x +v\n" );
      oyjlStr_Push( s, "            return\n" );
      oyjlStr_Push( s, "            ;;\n" );
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
          oyjlStr_Push( s, "        " );
        oyjlStr_Add( s, "%s-%s", found?"|":"", o );
        ++found;
      }
    }
    if(found)
    {
      oyjlStr_Push( s, ")\n" );
      oyjlStr_Push( s, "            : \"finish short options without choices\"\n" );
      oyjlStr_Push( s, "            COMPREPLY=(\"$cur \")\n" );
      oyjlStr_Push( s, "            set +x +v\n" );
      oyjlStr_Push( s, "            return\n" );
      oyjlStr_Push( s, "            ;;\n" );
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
          oyjlStr_Push( s, "        " );
        oyjlStr_Add( s, "%s--%s", found?"|":"", option );
        ++found;
      }
    }
    if(found)
    {
      oyjlStr_Push( s, ")\n" );
      oyjlStr_Push( s, "            : \"finish long options without choices\"\n" );
      oyjlStr_Push( s, "            COMPREPLY=(\"$cur \")\n" );
      oyjlStr_Push( s, "            set +x +v\n" );
      oyjlStr_Push( s, "            return\n" );
      oyjlStr_Push( s, "            ;;\n" );
    }
    found = 0;
    oyjlStr_Push( s, "    esac\n" );
    oyjlStr_Push( s, "\n" );
    oyjlStr_Push( s, "\n" );
    oyjlStr_Push( s, "    : \"show help for none '@' UIs\"\n" );
    oyjlStr_Push( s, "    if [[ \"$cur\" == \"\" ]]; then\n" );
    oyjlStr_Push( s, "      if [[ ${COMP_WORDS[1]} == \"\" ]]; then\n" );
    oyjlStr_Push( s, "        $1 help synopsis 1>&2\n" );
    oyjlStr_Push( s, "      else\n" );
    oyjlStr_Push( s, "        $1 help ${COMP_WORDS[1]} 1>&2\n" );
    oyjlStr_Push( s, "      fi\n" );
    oyjlStr_Push( s, "    fi\n" );
    oyjlStr_Push( s, "\n" );
    oyjlStr_Push( s, "\n" );
    oyjlStr_Push( s, "    : \"suggest group options for subcommands\"\n" );
    oyjlStr_Push( s, "    if [[ \"$cur\" == \"\" ]] || [[ \"$cur\" == \"-\" ]] || [[ \"$cur\" == -- ]] || [[ \"$cur\" == -* ]]; then\n" );
    oyjlStr_Push( s, "      case \"${COMP_WORDS[1]}\" in\n" );
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
      mandatory_list = oyjlStringSplit2( mandatory, "|,", 0, &mandatory_n, NULL, malloc );
      optional_list = oyjlStringSplit2( optional, "|,", 0, &optional_n, NULL, malloc );
      oyjlStr_Push( s, "        " );
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
        oyjlStr_Push( s, ")\n" );
        oyjlStr_Push( s, "          COMPREPLY=($(compgen -W '" );
        found = 0;
        for(j = 0; j < optional_n; ++j)
        {
          const char * ooption = optional_list[j];
          opt = oyjlOptions_GetOptionL( ui->opts, ooption, 0 );
          oyjlStr_Add( s, "%s%s%s%s%s%s%s%s", found?" ":"", opt->o?"-":"", opt->o?opt->o:"", (opt->o && WANT_ARG(opt))?"=":"", opt->o?" ":"", opt->option?"--":"", opt->option?opt->option:"", (opt->option && WANT_ARG(opt))?"=":"" );
          ++found;
        }
        oyjlStr_Push( s, "' -- \"$cur\"))\n" );
        oyjlStr_Push( s, "            set +x +v\n" );
        oyjlStr_Push( s, "            return\n" );
        oyjlStr_Push( s, "            ;;\n" );
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
      mandatory_list = oyjlStringSplit2( mandatory, "|,", 0, &mandatory_n, NULL, malloc );
      optional_list = oyjlStringSplit2( optional, "|,", 0, &optional_n, NULL, malloc );
      oyjlStr_Push( s, "        " );
      oyjlStr_Push( s, ".*)\n" );
      oyjlStr_Push( s, "          COMPREPLY=($(compgen -W '" );
      found = 0;
      for(j = 0; j < optional_n; ++j)
      {
        const char * ooption = optional_list[j];
        opt = oyjlOptions_GetOptionL( ui->opts, ooption, 0 );
        oyjlStr_Add( s, "%s%s%s%s%s%s%s%s", found?" ":"", opt->o?"-":"", opt->o?opt->o:"", (opt->o && WANT_ARG(opt))?"=":"", opt->o?" ":"", opt->option?"--":"", opt->option?opt->option:"", (opt->option && WANT_ARG(opt))?"=":"" );
        ++found;
      }
      oyjlStr_Push( s, "' -- \"$cur\"))\n" );
      oyjlStr_Push( s, "            set +x +v\n" );
      oyjlStr_Push( s, "            return\n" );
      oyjlStr_Push( s, "            ;;\n" );
      oyjlStringListRelease( &mandatory_list, mandatory_n, free );
      oyjlStringListRelease( &optional_list, optional_n, free );
    }
    oyjlStr_Push( s, "      esac\n" );
    oyjlStr_Push( s, "    fi\n" );
    oyjlStr_Push( s, "\n" );
    oyjlStr_Push( s, "    : \"suggest mandatory options on first args only\"\n" );
    oyjlStr_Push( s, "    if [[ \"${COMP_WORDS[2]}\" == \"\" ]]; then\n" );
    oyjlStr_Push( s, "      local WORD_LIST=()\n" );
    found = 0;
    if(found_at_arg_func)
    {
        oyjlStr_Add( s, "      local OYJL_TEXTS=$(%s)\n", found_at_arg_func );
        oyjlStr_Push( s, "      local IFS=$'\\n'\n" );
        oyjlStr_Push( s, "      for OYJL_TEXT in $OYJL_TEXTS\n" );
        oyjlStr_Push( s, "        do WORD_LIST=(\"${WORD_LIST[@]}\"\n\"$OYJL_TEXT\")\n" );
        oyjlStr_Push( s, "      done\n" );
    }
    if(n)
      oyjlStr_Push( s, "      WORD_LIST=(\"${WORD_LIST[@]}\" " );
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
      mandatory_list = oyjlStringSplit2( mandatory, "|,", 0, &mandatory_n, NULL, malloc );
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
      oyjlStr_Push( s, ")\n" );
    }
#undef WANT_ARG
    oyjlStr_Push( s, "      COMPREPLY=($(compgen -W '\"${WORD_LIST[@]}\"' -- \"$cur\"))\n" );
    oyjlStr_Push( s, "      set +x +v\n" );
    oyjlStr_Push( s, "      return\n" );
    oyjlStr_Push( s, "    fi\n" );
    oyjlStr_Push( s, "\n" );
    oyjlStr_Push( s, "    set +x +v\n" );
    oyjlStr_Push( s, "} &&\n" );
    oyjlStr_Add( s, "complete -o nosort -F _%s -o nospace %s\n", func, nick_ );
    oyjlStr_Push( s, "\n" );
    free(func);
    oyjlUi_Release( &ui );
  }
  else
    fputs( "can only generate C code, or bash completion", stderr );

  c = oyjlStr_Pull( s );
  oyjlStr_Release( &s );
  return c;
}

/** @} *//* oyjl_args */

