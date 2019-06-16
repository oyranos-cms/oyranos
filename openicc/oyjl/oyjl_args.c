/*  @file oyjl_args.c
 *
 *  oyjl - UI helpers
 *
 *  @par Copyright:
 *            2018-2019 (C) Kai-Uwe Behrmann
 *
 *  @brief    Oyjl argument handling
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
#include <langinfo.h>
#endif

extern int * oyjl_debug;

/** \addtogroup oyjl_args Options Handling
 *  @brief   Structured Options and Arguments for many aspects on command line and more
 *
 *  @section args_intro Introduction
 *  Oyjl argument handling uses a compact, table like creation syntax.
 *  oyjlUi_Create() is a high level API for tools to feed only the
 *  declarations and obtain the results in one call. On a lower level parsing
 *  with oyjlOptions_Parse() detects conflicts during programming and
 *  on run time. The arguments can be printed as a typical command line tool
 *  help text with oyjlOptions_PrintHelp(). The Oyjl JSON
 *  output from oyjlUi_ToJson() is useable for automatical generated
 *  (G)UI's and further processing. oyjlUi_ToMan() creates unix manual pages.
 *  oyjlUi_ToMarkdown() lets you produce a man page for your web page.
 *  Generation of other formats is simple.
 *  Translations are supported by oyjl-tanslate tool through gettext.
 *
 *  Command line options support single letter in oyjlOption_s::o and
 *  long options in oyjlOption_s::option without
 *  and with empty space and equal sign style single argument.
 *  @verbatim
    > myProgramm -o --option -w=1 --with-argument 1
    @endverbatim
 *
 *  @section args_tut Tutorial
 *  from @ref test-core.c :
    @dontinclude test-core.c
    @skip handle options
    @until done with options handling
 *
 *  @section args_orga Option Organisation - Groups
 *  Options can be read pure informational or express constraints.
 *  A **action** is a way to place the affected option(s) into the result, where
 *  the **result** can be the return of a textual structure or a executed
 *  command line call.
 *  The oyjlOptionGroup_s declares the layout and a simple syntax.
 *  In case the oyjlOptionGroup_s::mandatory or
 *  oyjlOptionGroup_s::optional members contain options, a action should be
 *  displayed inside the group. In absence of oyjlOptionGroup_s::mandatory
 *  options, the oyjlOptionGroup_s::optional options are handled like a
 *  single group and a widget for starting a action is needed.
 *  The oyjlOptionGroup_s::detail simply
 *  tells about displaying of a option inside a group.
 *  All oyjlOptionGroup_s::mandatory options shall be inside the
 *  oyjlOptionGroup_s::detail array for simplicity of man page generation.
 *
 *  @subsection args_single_call One Action Group
 *  Very simple tools might have few optional arguments. It makes
 *  sense to have no constraints in groups. So there is no mandatory
 *  option at all needed. All options shall be applied at once. Example:
 *  @verbatim
    tool [-a] [-b] [-c] [-v]
    @endverbatim
 @code
    oyjlOptionGroup_s groups[] = {
    // type,   flags, name,     description,help, mandatory,  optional, detail
      // place here the action widget; all optional options [a+b+c+v] are in this action group
      {"oiwg", 0,     "Group1", 0,0,              "",         "abcv",   "ab" },
      // only description; no action
      {"oiwg", 0,     "Group2", 0,0,              "",         "",       "c" },
      // only description; no action
      {"oiwg", 0,     "Common", 0,0,              "",         "",       "v" },
      {0,0,0,0,0,0,0,0}
    } // note: no mandatory option is named
 @endcode
 *  The GUI should show one over all action widget, e.g. press button, in the
 *  group(s) with optional options.
 *  This mode implies the empty option mark '#' needs to be specified, as the
 *  tool needs no mandatory option(s).
 *
 *  @subsection args_group_modes Grouped Modes
 *  More complex tools might have different modes, in order to do one
 *  task. The 'tar' tool is organised to do belonging tasks as a task.
 *  Each group is introduced by a mandatory option and might be followed
 *  by some more optional options. Options can be expluced from groups.
 *  Some more options can by applied to all groups. Example:
 *  @verbatim
    tool -a -b [-c] [-v]
    tool -h [-v]
    @endverbatim
 @code
    oyjlOptionGroup_s groups[] = {
    // type,   flags, name,     description,help, mandatory,  optional, detail
      // a separate action widget is needed, so 'a' and 'b' can be set before action; a+b+c+[v] are in this action group
      {"oiwg", 0,     "Group1", 0,0,              "ab",       "cv",     "abc" },
      // the 'h' option could be handled as the action widget; h+[v] are in this action group
      {"oiwg", 0,     "Group2", 0,0,              "h",        "v",      "h" },
      // only description; no action
      {"oiwg", 0,     "Common", 0,0,              "",         "",       "v" },
      {0,0,0,0,0,0,0,0}
    }
 @endcode
 *  The GUI should show one action widget, e.g. press button, per group.
 *
 *  @subsection args_group_independence Independent Options
 *  Each option is independent. However it is possible to group them
 *  in a pure informational way. E.g.
 *  @verbatim
    tool -a|-b [-v]
    tool -h [-v]
    @endverbatim
 *  The above example knows three basic modes: a, b and h. The -v
 *  option might be applied to all of them. Each of a,b,h is called
 *  independently and that means the GUI reacts instantly on any changed state
 *  in them. A group object might look like:
 @code
    oyjlOptionGroup_s groups[] = {
    // type,   flags, name,     description,help, mandatory,  optional, detail
      // 'a' and 'b' can be action widgets, as they are independent; a+[v] or b+[v] are in this action group
      {"oiwg", 0,     "Group1", 0,0,              "a|b",      "v",      "ab" },
      // 'h' can be action widget; h+[v] are in this action group
      {"oiwg", 0,     "Group2", 0,0,              "h",        "v",      "h" },
      // only description; no action
      {"oiwg", 0,     "Common", 0,0,              "",         "",       "v" },
      {0,0,0,0,0,0,0,0}
    } // note: all manatory options are ored '|' or single in the group
 @endcode
 *  The GUI should show one action widget, e.g. press button, per option.
 *  All options in oyjlOptionGroup_s::mandatory should be or'ed with '|',
 *  otherwise the individial actions make no sense.
 *
 *  @section args_renderer Viewers
 *  The option tables allow for different views. Those can be pure
 *  informational, like man pages created with the *cli -X man* command.
 *  Or a view can be interactive and call the command with the options.
 *
 *  @subsection args_interactive Interactive Viewers
 *  A example of a interactive viewer is the included oyjl-args-qml renderer.
 *  Tools have to be more careful, in case they want to be displayed by
 *  a interactive viewer. They should declare, in which order options
 *  apply and add command line information for the -X json+command option.
 *
 *  A GUI renderer might display the result of a command immediately. The
 *  simplest form is plain text output. The text font should eventually be
 *  a monotyped on, like typical for interactive shells. This builds as well
 *  a command like aesthetic. Some tools return graphics in multiline
 *  ASCII or use color codes.
 *  The tool might output HTML and should be displayed formatted. Some tools
 *  stream their result as image to stdout. A oyjl-args-qml viewer supports
 *  PNG image output.
 *
 *  @{ */

/** @brief    Release dynamic structure
 *  @memberof oyjlOptionChoice_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2018/08/14
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
void oyjlOptionChoice_Release     ( oyjlOptionChoice_s**choices )
{
  int n = 0,i;
  oyjlOptionChoice_s * ca;
  if(!choices || !*choices) return;
  ca = *choices;
  while(ca[n].nick && ca[n].nick[0] != '\000') ++n;
  for(i = 0; i < n; ++i)
  {
    oyjlOptionChoice_s * c = &ca[i];
    if(c->nick) free(c->nick);
    if(c->name) free(c->name);
    if(c->description) free(c->description);
    if(c->help) free(c->help);
  }
  *choices = NULL;
  free(*choices);
}

/* true color codes */
#define OYJL_RED_TC "\033[38;2;240;0;0m"
#define OYJL_GREEN_TC "\033[38;2;0;250;100m"
#define OYJL_BLUE_TC "\033[38;2;0;150;255m"
/* basic color codes */
#define OYJL_BOLD "\033[1m"
#define OYJL_ITALIC "\033[3m"
#define OYJL_UNDERLINE "\033[4m"
#define OYJL_RED_B "\033[0;31m"
#define OYJL_GREEN_B "\033[0;32m"
#define OYJL_BLUE_B "\033[0;34m"
/* switch back */
#define OYJL_CTEND "\033[0m"
typedef enum {
  oyjlRED,
  oyjlGREEN,
  oyjlBLUE,
  oyjlBOLD,
  oyjlITALIC,
  oyjlUNDERLINE
} oyjlCOLORTERM_e;
static const char * oyjlTermColor( oyjlCOLORTERM_e rgb, const char * text) {
  int len = strlen(text);
  static char t[256];
  static int colorterm_init = 0;
  static const char * oyjl_colorterm = NULL;
  static int truecolor = 0,
             color = 0;
  if(!colorterm_init)
  {
    colorterm_init = 1;
    oyjl_colorterm = getenv("COLORTERM");
    color = oyjl_colorterm != NULL ? 1 : 0;
    if(!oyjl_colorterm) oyjl_colorterm = getenv("TERM");
    truecolor = oyjl_colorterm && strcmp(oyjl_colorterm,"truecolor") == 0;
  }
  if(len < 200)
  {
    switch(rgb)
    {
      case oyjlRED: sprintf( t, "%s%s%s", truecolor ? OYJL_RED_TC : color ? OYJL_RED_B : "", text, OYJL_CTEND ); break;
      case oyjlGREEN: sprintf( t, "%s%s%s", truecolor ? OYJL_GREEN_TC : color ? OYJL_GREEN_B : "", text, OYJL_CTEND ); break;
      case oyjlBLUE: sprintf( t, "%s%s%s", truecolor ? OYJL_BLUE_TC : color ? OYJL_BLUE_B : "", text, OYJL_CTEND ); break;
      case oyjlBOLD: sprintf( t, "%s%s%s", truecolor || color ? OYJL_BOLD : "", text, truecolor || color ? OYJL_CTEND : "" ); break;
      case oyjlITALIC: sprintf( t, "%s%s%s", truecolor || color ? OYJL_ITALIC : "", text, truecolor || color ? OYJL_CTEND : "" ); break;
      case oyjlUNDERLINE: sprintf( t, "%s%s%s", truecolor || color ? OYJL_UNDERLINE : "", text, truecolor || color ? OYJL_CTEND : "" ); break;
    }
    return t;
  } else
    return text;
}


/** @brief    Return number of "oiwi" array elements
 *  @memberof oyjlOptions_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2018/08/14
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
int oyjlOptions_Count             ( oyjlOptions_s  * opts )
{
  int n = 0;
  while( *(oyjlOBJECT_e*)&opts->array[n] /*"oiwi"*/ == oyjlOBJECT_OPTION) ++n;
  return n;
}

/** @brief    Return number of "oiwi" groups elements
 *  @memberof oyjlOptions_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2018/08/14
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
int oyjlOptions_CountGroups       ( oyjlOptions_s  * opts )
{
  int n = 0;
  while( *(oyjlOBJECT_e*)&opts->groups[n] /*"oiwg"*/ == oyjlOBJECT_OPTION_GROUP) ++n;
  return n;
}


enum {
  oyjlOPTIONSTYLE_ONELETTER = 0x01,
  oyjlOPTIONSTYLE_STRING = 0x02,
  oyjlOPTIONSTYLE_OPTIONAL_START = 0x04,
  oyjlOPTIONSTYLE_OPTIONAL_END = 0x08,
  oyjlOPTIONSTYLE_OPTIONAL_INSIDE_GROUP = 0x10,
  oyjlOPTIONSTYLE_MAN = 0x20,
  oyjlOPTIONSTYLE_MARKDOWN = 0x40
};
#define oyjlOPTIONSTYLE_OPTIONAL (oyjlOPTIONSTYLE_OPTIONAL_START | oyjlOPTIONSTYLE_OPTIONAL_END)
static
const char * oyjlOption_PrintArg     ( oyjlOption_s      * o,
                                       int                 style )
{
  static char * text = NULL;
  if(text) text[0] = '\000';
  if(!o) return "";
  if(style & oyjlOPTIONSTYLE_OPTIONAL_START)
    oyjlStringAdd( &text, malloc, free, "[" );
  if(style & oyjlOPTIONSTYLE_ONELETTER && o->o != '@')
  {
    if(style & oyjlOPTIONSTYLE_MAN)
      oyjlStringAdd( &text, malloc, free, "\\-%c", o->o );
    else
      oyjlStringAdd( &text, malloc, free, "-%c", o->o );
  }
  if(style & oyjlOPTIONSTYLE_ONELETTER && style & oyjlOPTIONSTYLE_STRING && o->o != '@')
    oyjlStringAdd( &text, malloc, free, "|" );
  if(style & oyjlOPTIONSTYLE_STRING && o->o != '@')
  {
    if(style & oyjlOPTIONSTYLE_MAN)
      oyjlStringAdd( &text, malloc, free, "\\-\\-%s", o->option );
    else
      oyjlStringAdd( &text, malloc, free, "--%s", o->option );
  }
  if(o->value_name)
  {
    if(style & oyjlOPTIONSTYLE_MAN)
      oyjlStringAdd( &text, malloc, free, " \\fI%s\\fR", o->value_name );
    else
    {
      if(style & oyjlOPTIONSTYLE_MARKDOWN)
        oyjlStringAdd( &text, malloc, free, " *%s*", o->value_name );
      else if(style & oyjlOPTIONSTYLE_OPTIONAL_INSIDE_GROUP)
        oyjlStringAdd( &text, malloc, free, "%s", oyjlTermColor(oyjlITALIC,o->value_name) );
      else
        oyjlStringAdd( &text, malloc, free, " %s", oyjlTermColor(oyjlITALIC,o->value_name) );
    }
  }
  if(style & oyjlOPTIONSTYLE_OPTIONAL_END)
    oyjlStringAdd( &text, malloc, free, "]" );
  return text;
}

oyjlOptionChoice_s oyjl_X_choices[] = {{"json", "", "", ""},
                                    {"json+command", "", "", ""},
                                    {"man", "", "", ""},
                                    {"markdown", "", "", ""},
                                    {"","","",""}};
void oyjlOptions_EnrichInbuild( oyjlOption_s * o )
{
  int oc = o->o;

  if(oc == 'X')
  {
    if(o->value_type == oyjlOPTIONTYPE_CHOICE && o->values.choices.list == NULL)
    {
      oyjl_X_choices[0].name = _("Json");
      oyjl_X_choices[0].description = _("GUI");
      oyjl_X_choices[0].help = _("Get a Oyjl Json UI declaration");
      oyjl_X_choices[1].name = _("Json + Command");
      oyjl_X_choices[1].description = _("GUI + Command");
      oyjl_X_choices[1].help = _("Get Oyjl Json UI declaration incuding command");
      oyjl_X_choices[2].name = _("Man");
      oyjl_X_choices[2].description = _("Unix Man page");
      oyjl_X_choices[2].help = _("Get a unix man page");
      oyjl_X_choices[3].name = _("Markdown");
      oyjl_X_choices[3].description = _("Formated text");
      oyjl_X_choices[3].help = _("Get formated text");
      o->values.choices.list = oyjl_X_choices;
      if(o->value_name == NULL)
      {
        o->value_name = "json|json+command|man|markdown";
        if(o->name == NULL)
        {
          o->name = _("export");
          if(o->description == NULL)
          {
            o->description = _("Export formated text");
            if(o->help == NULL)
              o->help = _("Get UI converted into text formats");
          }
        }
      }
    }
  }
}

/** @brief    Obtain the specified option from option char
 *  @memberof oyjlOptions_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2018/08/14
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
oyjlOption_s * oyjlOptions_GetOption (
                                       oyjlOptions_s  * opts,
                                       char                oc )
{
  int i;
  int nopts = oyjlOptions_Count( opts );
  oyjlOption_s * o = NULL;

  for(i = 0; i < nopts; ++i)
  {
    o = &opts->array[i];
    if(o->o && o->o == oc)
    {
      if(oc == 'X' && o->value_type == oyjlOPTIONTYPE_CHOICE && o->values.choices.list == NULL)
        oyjlOptions_EnrichInbuild(o);

      return o;
    }
    else
      o = NULL;
  }
  return o;
}

/** @brief    Obtain the specified option from option string
 *  @memberof oyjlOptions_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2018/08/14
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
oyjlOption_s * oyjlOptions_GetOptionL (
                                       oyjlOptions_s  * opts,
                                       const char        * ostring )
{
  int i;
  int nopts = oyjlOptions_Count( opts );
  oyjlOption_s * o = NULL;
  char * str = oyjlStringCopy(ostring, malloc);
  char * t = strchr(str, '=');

  if(t)
    t[0] = '\000';

  for(i = 0; i < nopts; ++i)
  {
    o = &opts->array[i];
    if(o->option && strcmp(o->option, str) == 0)
      return o;
    else
      o = NULL;
  }
  fprintf( stderr, "Option not found: %s\n", str );

  return o;
}
static
oyjlOPTIONSTATE_e oyjlOptions_Check (
                                       oyjlOptions_s  * opts )
{
  int i,j;
  int nopts = oyjlOptions_Count( opts );
  oyjlOption_s * o = NULL, * b = NULL;

  for(i = 0; i < nopts; ++i)
  {
    o = &opts->array[i];
    for(j = i+1; j < nopts; ++j)
    {
      b = &opts->array[j];
      if(o->o == b->o)
      {
        fprintf( stderr, "%s %s \'%c\'\n", _("Usage Error:"), _("Double occuring option"), b->o );
        return oyjlOPTION_DOUBLE_OCCURENCE;
      }

    }
    /* some error checking */
    if(o->o != '#' && o->value_name && o->value_name[0] && o->value_type == oyjlOPTIONTYPE_NONE)
    {
      fprintf( stderr, "%s %s \'%c\' %s\n", _("Usage Error:"), _("Option not supported"), o->o, _("need a value_type") );
      return oyjlOPTION_NOT_SUPPORTED;
    }
    if( o->o != '#' &&
        o->o != 'X' &&
        o->value_type == oyjlOPTIONTYPE_CHOICE &&
        !((o->flags & OYJL_OPTION_FLAG_EDITABLE) || o->values.choices.list))
    {
      fprintf( stderr, "%s %s \'%c\' %s\n", _("Usage Error:"), _("Option not supported"), o->o, _("needs OYJL_OPTION_FLAG_EDITABLE or choices") );
      return oyjlOPTION_NOT_SUPPORTED;
    }
  }
  return oyjlOPTION_NONE;
}

/** @brief    Parse the options into a private data structure
 *  @memberof oyjlOptions_s
 *
 *  The returned status can be used to detect usage errors and hint them on
 *  the command line.
 *  In the usual case where the variable fields are set, the results
 *  will be set too.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2018/08/19
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
oyjlOPTIONSTATE_e oyjlOptions_Parse (
                                       oyjlOptions_s  * opts )
{
  oyjlOPTIONSTATE_e state = oyjlOPTION_NONE;
  oyjlOption_s * o;
  char ** result;

  /* parse the command line arguments */
  if(opts && !opts->private_data)
  {
    int i, pos = 0;
    result = (char**) calloc( 2, sizeof(char*) );
    result[0] = (char*) calloc( 65536, sizeof(char) );
    if((state = oyjlOptions_Check(opts)) != oyjlOPTION_NONE)
      goto clean_parse;
    for(i = 1; i < opts->argc; ++i)
    {
      const char * str = opts->argv[i];
      int l = strlen(str);
      char arg = ' ';
      const char * long_arg = NULL;
      const char * value = NULL;

      if(strstr(str,"-qmljsdebugger") != NULL) /* detect a QML option */
        continue;

      /* parse -a | -a value | -a=value | -ba | -ba value | -ba=value */
           if(l > 1 && str[0] == '-' && str[1] != '-')
      {
        int require_value, j;
        for(j = 1; j < l; ++j)
        {
          arg = str[j];
          o = oyjlOptions_GetOption( opts, arg );
          if(!o)
          {
            fprintf( stderr, "%s %s \'%c\'\n", _("Usage Error:"), _("Option not supported"), arg );
            state = oyjlOPTION_NOT_SUPPORTED;
            break;
          }
          require_value = o->value_type != oyjlOPTIONTYPE_NONE;
          if( require_value )
          {
            value = NULL;
            if( j == l-1 && opts->argc > i+1 && (opts->argv[i+1][0] != '-' || strlen(opts->argv[i+1]) <= 1) )
            {
              value = opts->argv[i+1];
              ++i;
            }
            else if( str[j+1] == '=' )
            {
              ++j;
              value = &str[j+1];
              j = l;
            }
            else
            {
              fprintf( stderr, "%s %s \'%c\'\n", _("Usage Error:"), _("Option needs a argument"), arg );
              state = oyjlOPTION_MISSING_VALUE;
            }
            if(value)
            {
              int llen = 0;
              while(result[llen]) ++llen;
              result[0][pos] = arg;
              ++pos;
              oyjlStringListAddStaticString( &result, &llen, value, malloc, free );
            }
          }
          else if(!require_value && !(j < l-1 && str[j+1] == '='))
          {
            int llen = 0;
            while(result[llen]) ++llen;
            result[0][pos] = arg;
            ++pos;
            oyjlStringListAddStaticString( &result, &llen, "1", malloc, free );
          }
          else
          {
            int i;
            for(i = 0; i < opts->argc; ++i)
              fprintf( stderr, "\'%s\' ", opts->argv[i]);
            fprintf( stderr, "\n");
            fprintf( stderr, "%s %s \'%c\'\n", _("Usage Error:"), _("Option has a unexpected argument"), arg );
            state = oyjlOPTION_UNEXPECTED_VALUE;
            j = l;
          }
        }
      }
      /* parse --arg | --arg value | --arg=value */
      else if(l > 2 && str[0] == '-' && str[1] == '-')
      {
        int require_value;
        long_arg = &str[2];
        o = oyjlOptions_GetOptionL( opts, long_arg );
        if(!o)
        {
          fprintf( stderr, "%s %s \'%s\'\n", _("Usage Error:"), _("Option not supported"), long_arg );
          state = oyjlOPTION_NOT_SUPPORTED;
          goto clean_parse;
        }
        require_value = o->value_type != oyjlOPTIONTYPE_NONE;
        if( require_value )
        {
          value = NULL;

          if( strchr(str, '=') != NULL )
            value = strchr(str, '=') + 1;
          else if( opts->argc > i+1 && opts->argv[i+1][0] != '-' )
          {
            value = opts->argv[i+1];
            ++i;
          }
          else
          {
            fprintf( stderr, "%s %s \'%s\'\n", _("Usage Error:"), _("Option needs a argument"), long_arg );
            state = oyjlOPTION_MISSING_VALUE;
          }

          if(value)
          {
            int llen = 0;
            while(result[llen]) ++llen;
            result[0][pos] = o->o;
            ++pos;
            oyjlStringListAddStaticString( &result, &llen, value, malloc, free );
          }
        } else
        {
          if(!( strchr(str, '=') != NULL || (opts->argc > i+1 && opts->argv[i+1][0] != '-') ))
          {
            int llen = 0;
            while(result[llen]) ++llen;
            result[0][pos] = o->o;
            ++pos;
            oyjlStringListAddStaticString( &result, &llen, "1", malloc, free );
          } else
          {
            fprintf( stderr, "%s %s \'%s\'\n", _("Usage Error:"), _("Option has a unexpected argument"), opts->argv[i+1] );
            state = oyjlOPTION_UNEXPECTED_VALUE;
          }
        }
      }
      /* parse anonymous value, if requested */
      else
      {
        result[0][pos] = '@';
        o = oyjlOptions_GetOption( opts, '@' );
        if(o)
          value = str;
        if(value)
        {
          int llen = 0;
          while(result[llen]) ++llen;
          oyjlStringListAddStaticString( &result, &llen, value, malloc, free );
        }
        ++pos;
      }
    }
    opts->private_data = result;

    pos = 0;
    while(result[0][pos])
    {
      oyjlOption_s * o = oyjlOptions_GetOption( opts, result[0][pos] );
      if(o)
      switch(o->variable_type)
      {
        case oyjlNONE:   break;
        case oyjlSTRING: oyjlOptions_GetResult( opts, o->o, o->variable.s, 0, 0 ); break;
        case oyjlDOUBLE: oyjlOptions_GetResult( opts, o->o, 0, o->variable.d, 0 ); break;
        case oyjlINT:    oyjlOptions_GetResult( opts, o->o, 0, 0, o->variable.i ); break;
      }
      ++pos;
    }

    /** Put the count of found anonymous arguments into '@' options variable.i of variable_type oyjlINT. */
    o = oyjlOptions_GetOption( opts, '@' );
    if(o && o->variable_type == oyjlINT && o->variable.i)
    {
      int count = 0;
      /* detect all '@' anonymous arguments */
      char ** results = oyjlOptions_ResultsToList( opts, '@', &count );
      *o->variable.i = count;
      oyjlStringListRelease( &results, count, free );
    }
  }

  return state;

clean_parse:
  free(result[0]);
  free(result);
  return state;
}

/** @brief    Obtain the parsed result
 *  @memberof oyjlOptions_s
 *
 *  This function is only useful, if the results shall be obtained
 *  independently from oyjlOptions_Parse().
 *
 *  If the option was not specified the state oyjlOPTION_NONE will be
 *  returned and otherwise oyjlOPTION_USER_CHANGED. With result_int and
 *  a option type of oyjlOPTIONTYPE_NONE, the number of occurences is
 *  obtained, e.g. -vvv will give result_int = 3. A option type
 *  oyjlOPTIONTYPE_DOUBLE can ask for the floating point result with a
 *  result_dbl argument.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2018/08/14
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
oyjlOPTIONSTATE_e oyjlOptions_GetResult (
                                       oyjlOptions_s     * opts,
                                       char                oc,
                                       const char       ** result_string,
                                       double            * result_dbl,
                                       int               * result_int )
{
  oyjlOPTIONSTATE_e state = oyjlOPTION_NONE;
  ptrdiff_t pos = -1;
  const char * t;
  const char ** results;
  const char * list;

  /* parse the command line arguments */
  if(!opts->private_data)
    state = oyjlOptions_Parse( opts );
  if(state != oyjlOPTION_NONE)
    return state;

  results = opts->private_data;
  list = results[0];
  t = strrchr( list, oc );
  if(t)
  {
    pos = t - list;
    state = oyjlOPTION_USER_CHANGED;
  }
  else if(oc == ' ' && strlen(list))
  {
    if(result_int)
      *result_int = strlen(list);
    return oyjlOPTION_USER_CHANGED;
  }
  else
    return oyjlOPTION_NONE;

  t = results[1 + pos];

  if(result_string)
    *result_string = t;

  if(result_dbl)
  {
    oyjlOption_s * o = oyjlOptions_GetOption( opts, oc );
    oyjlStringToDouble( t, result_dbl );
    if( o->value_type == oyjlOPTIONTYPE_DOUBLE &&
        ( o->values.dbl.start > *result_dbl ||
          o->values.dbl.end < *result_dbl) )
    {
      fprintf( stderr, "%s %s \'%c\' %s %g->%g !: %g\n", _("Usage Error:"), _("Option has a different value range"), oc, o->name, o->values.dbl.start, o->values.dbl.end, *result_dbl  );
    }
  }
  if(result_int)
  {
    int l = strlen( list ), i,n = 0;
    oyjlOption_s * o = oyjlOptions_GetOption( opts, oc );
    if(o->value_type == oyjlOPTIONTYPE_NONE)
    {
      for(i = 0; i < l; ++i)
        if(list[i] == oc)
          ++n;
      *result_int = n;
    } else
    {
      long lo = 0;
      if(oyjlStringToLong( t, &lo ) == 0)
        *result_int = lo;
    }
  }

  return state;
}

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

/** @brief    Convert the parsed content to a text list
 *  @memberof oyjlOptions_s
 *
 *  @param[in]     opts                the argument object
 *  @param[in]     oc                  a filter; use '\000' to get all results;
 *                                     e.g. use '@' for all anonymous results
 *  @param[out]    count               the number of matched results
 *  @return                            a possibly filterd string list of results;
 *                                     Without a filter it contains the argument
 *                                     Id followed by double point and the
 *                                     result starting at index 3.
 *                                     With filter it contains only results if
 *                                     apply and without Id.
 *                                     The memory is owned by caller.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/03/25
 *  @since   2019/03/25 (Oyjl: 1.0.0)
 */
char **  oyjlOptions_ResultsToList   ( oyjlOptions_s     * opts,
                                       char                oc,
                                       int               * count )
{
  char * args = NULL,
       * text = NULL,
       ** list = NULL;
  char ** results = NULL;
  int i,n,llen = 0;

  if(!opts)
    return results;
  results = opts->private_data;
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
  for(i = 0; i < n; ++i)
  {
    char a[4] = {args[i],0,0,0};
    char * value = results[i+1];
    if(oc == '\000')
      oyjlStringAdd( &text, malloc, free, "%s:%s", a, value );
    else if(oc == a[0])
      oyjlStringAdd( &text, malloc, free, "%s", value );
    if(text)
    {
      oyjlStringListAddStaticString( &list, &llen, text, malloc, free );
      free(text); text = NULL;
    }
  }
  if(count)
    *count = llen;

  return list;
}

/** @brief    Convert the parsed content to simple text
 *  @memberof oyjlOptions_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2018/08/14
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
char * oyjlOptions_ResultsToText  ( oyjlOptions_s  * opts )
{
  char * args = NULL,
       * text = NULL;
  char ** results = opts->private_data;
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
  for(i = 0; i < n; ++i)
  {
    char a[4] = {args[i],0,0,0};
    char * value = results[i+1];
    oyjlStringAdd( &text, malloc, free, "%s:%s\n", a, value );
  }

  return text;
}

/** @internal
 *  @brief    Print synopsis of a option group to stderr
 *  @memberof oyjlOptions_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2018/08/14
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
static const char * oyjlOptions_PrintHelpSynopsis (
                                       oyjlOptions_s  *    opts,
                                       oyjlOptionGroup_s * g,
                                       int                 style )
{
  int i;
  int m = g->mandatory ? strlen(g->mandatory) : 0;
  int on = g->optional ? strlen(g->optional) : 0;
  static char * text = NULL;
  int opt_group = 0;
  int gstyle = style;
  const char * prog = opts->argv[0];
  if(prog && strchr(prog,'/'))
    prog = strrchr(prog,'/') + 1;

  if(text) text[0] = 0;

  if( m || on )
  {
    if(style & oyjlOPTIONSTYLE_MAN)
      oyjlStringAdd( &text, malloc, free, "\\fB%s\\fR", prog );
    else if(style & oyjlOPTIONSTYLE_MARKDOWN)
      oyjlStringAdd( &text, malloc, free, "**%s**", prog );
    else
      oyjlStringAdd( &text, malloc, free, "%s", oyjlTermColor(oyjlBOLD,prog) );
  }
  else
    return text;

  for(i = 0; i < m; ++i)
  {
    char oc = g->mandatory[i];
    oyjlOption_s * o = oyjlOptions_GetOption( opts, oc );
    if(oc == '|')
      oyjlStringAdd( &text, malloc, free, "|" );
    else if(!o)
    {
      fprintf(stderr, "\n%s: option not declared: %c\n", g->name, oc);
      exit(1);
    }
    if(oc != '@' && oc != '#')
      oyjlStringAdd( &text, malloc, free, " %s", oyjlOption_PrintArg(o, style) );
  }
  for(i = 0; i < on; ++i)
  {
    char oc = g->optional[i];
    oyjlOption_s * o = oyjlOptions_GetOption( opts, oc );
    gstyle = style | oyjlOPTIONSTYLE_OPTIONAL;
    if(i < on - 1 && g->optional[i+1] == '|')
    {
      if(opt_group == 0)
        gstyle = style | oyjlOPTIONSTYLE_OPTIONAL_START | oyjlOPTIONSTYLE_OPTIONAL_INSIDE_GROUP;
      else
        gstyle = style | oyjlOPTIONSTYLE_OPTIONAL_INSIDE_GROUP;
      opt_group = 1;
    }
    else if(oc == '|')
    {
      oyjlStringAdd( &text, malloc, free, "|" );
      continue;
    }
    else if(opt_group)
    {
      gstyle = style | oyjlOPTIONSTYLE_OPTIONAL_END;
      opt_group = 0;
    }
    else if(!o)
    {
      fprintf(stderr, "\n%s: option not declared: %c\n", g->name, oc);
      exit(1);
    }

    oyjlStringAdd( &text, malloc, free, "%s%s", gstyle & oyjlOPTIONSTYLE_OPTIONAL_START ? " ":"", oyjlOption_PrintArg(o, gstyle) );
  }
  for(i = 0; i < m; ++i)
  {
    char oc = g->mandatory[i];
    oyjlOption_s * o = oyjlOptions_GetOption( opts, oc );
    if(oc != '|' && !o)
    {
      fprintf(stderr, "\n%s: option not declared: %c\n", g->name, oc);
      exit(1);
    }
    if(oc == '@')
      oyjlStringAdd( &text, malloc, free, " %s", o->value_name?o->value_name:"..." );
  }
  return text;
}

static oyjlOptionChoice_s ** oyjl_get_choices_list_ = NULL;
static int oyjl_get_choices_list_selected_[256];
static
oyjlOptionChoice_s * oyjlOption_GetChoices_ (
                                       oyjlOption_s      * o,
                                       int               * selected,
                                       oyjlOptions_s     * opts )
{
  if(!o) return NULL;

  if(!oyjl_get_choices_list_)
  {
    int i;
    for(i = 0; i < 256; ++i) oyjl_get_choices_list_selected_[i] = -1;
    oyjl_get_choices_list_ = calloc( sizeof(oyjlOptionChoice_s*), 256 ); /* number of possible chars */
  }

  if( !oyjl_get_choices_list_[(int)o->o] ||
      (selected && oyjl_get_choices_list_selected_[(int)o->o] == -1) )
    oyjl_get_choices_list_[(int)o->o] = o->values.getChoices(o, selected ? &oyjl_get_choices_list_selected_[(int)o->o] : selected, opts );

  if(selected)
    *selected = oyjl_get_choices_list_selected_[(int)o->o];
  return oyjl_get_choices_list_[(int)o->o];
}


#include <stdarg.h> /* va_list */
/** @brief    Print help text to stderr
 *  @memberof oyjlOptions_s
 *
 *  @param   opts                      options to print
 *  @param   ui                        more info for e.g. from the documentation section for the description block; optional
 *  @param   verbose                   gives debug output
 *  @param   motto_format              prints a customised intoduction line
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/05/06
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
void  oyjlOptions_PrintHelp          ( oyjlOptions_s     * opts,
                                       oyjlUi_s          * ui,
                                       int                 verbose,
                                       const char        * motto_format,
                                                           ... )
{
  int i,ng;
  va_list list;
  int indent = 2;
  oyjlUiHeaderSection_s * section = NULL;
  fprintf( stdout, "\n");
  if(verbose)
  {
    for(i = 0; i < opts->argc; ++i)
      fprintf( stdout, "\'%s\' ", oyjlTermColor( oyjlITALIC, opts->argv[i] ));
    fprintf( stdout, "\n");
  }

  if(!motto_format)
  {
    oyjlUiHeaderSection_s * version = oyjlUi_GetHeaderSection( ui,
                                                               "version" );
    fprintf( stdout, "%s v%s - %s", oyjlTermColor( oyjlBOLD, opts->argv[0] ),
                              version && version->name ? version->name : "",
                              ui->description ? ui->description : "" );
  }
  else
  {
    va_start( list, motto_format );
    vfprintf( stdout, motto_format, list );
    va_end  ( list );
  }
  fprintf( stdout, "\n");

  ng = oyjlOptions_CountGroups(opts);
  if(!ng) return;

  if( ui && (section = oyjlUi_GetHeaderSection(ui, "documentation")) != NULL &&
      section->description )
    fprintf( stdout, "\n%s:\n  %s\n", oyjlTermColor(oyjlBOLD,_("Description")), section->description );

  fprintf( stdout, "\n%s:\n", oyjlTermColor(oyjlBOLD,_("Synopsis")) );
  for(i = 0; i < ng; ++i)
  {
    oyjlOptionGroup_s * g = &opts->groups[i];
    fprintf( stdout, "  %s\n", oyjlOptions_PrintHelpSynopsis( opts, g, oyjlOPTIONSTYLE_ONELETTER ) );
  }

  fprintf( stdout, "\n%s:\n", oyjlTermColor(oyjlBOLD,_("Usage"))  );
  for(i = 0; i < ng; ++i)
  {
    oyjlOptionGroup_s * g = &opts->groups[i];
    int d = g->detail ? strlen(g->detail) : 0,
        j,k;
    fprintf( stdout, "  %s\n", oyjlTermColor(oyjlUNDERLINE,g->description) );
    if(g->mandatory && g->mandatory[0])
    {
      fprintf( stdout, "\t%s\n", oyjlOptions_PrintHelpSynopsis( opts, g, oyjlOPTIONSTYLE_ONELETTER ) );
    }
    for(j = 0; j < d; ++j)
    {
      char oc = g->detail[j];
      oyjlOption_s * o = oyjlOptions_GetOption( opts, oc );
      if(!o)
      {
        fprintf(stdout, "\n%s: option not declared: %c\n", g->name, oc);
        exit(1);
      }
      for(k = 0; k < indent; ++k) fprintf( stdout, " " );
      switch(o->value_type)
      {
        case oyjlOPTIONTYPE_CHOICE:
          {
            int n = 0,l;
            if(o->value_name)
            {
              fprintf( stdout, "\t" );
              fprintf( stdout, "%s", oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING) );
              fprintf( stdout, "\t%s%s%s\n", o->description ? o->description:"", o->help?": ":"", o->help?o->help :"" );
            }
            if(o->flags & OYJL_OPTION_FLAG_EDITABLE)
              break;
            while(o->values.choices.list[n].nick && o->values.choices.list[n].nick[0] != '\000')
              ++n;
            for(l = 0; l < n; ++l)
              fprintf( stdout, "\t\t-%c %s\t\t# %s%s%s\n",
                  o->o,
                  o->values.choices.list[l].nick,
                  o->values.choices.list[l].name && o->values.choices.list[l].nick[0] ? o->values.choices.list[l].name : o->values.choices.list[l].description,
                  o->values.choices.list[l].help&&o->values.choices.list[l].help[0]?" - ":"",
                  o->values.choices.list[l].help?o->values.choices.list[l].help:"" );
          }
          break;
        case oyjlOPTIONTYPE_FUNCTION:
          {
            int n = 0,l;
            oyjlOptionChoice_s * list;
            if(o->value_name)
            {
              fprintf( stdout, "\t" );
              fprintf( stdout, "%s", oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING) );
              fprintf( stdout, "\t%s%s%s\n", o->description ? o->description:"", o->help?": ":"", o->help?o->help :"" );
            }
            if(o->flags & OYJL_OPTION_FLAG_EDITABLE)
              break;
            list = oyjlOption_GetChoices_(o, NULL, opts );
            if(list)
              while(list[n].nick && list[n].nick[0] != '\000')
                ++n;
            for(l = 0; l < n; ++l)
              fprintf( stdout, "\t  -%c %s\t\t# %s\n", o->o, list[l].nick, list[l].name && list[l].nick[0] ? list[l].name : list[l].description );
            /* not possible, as the result of oyjlOption_GetChoices_() is cached - oyjlOptionChoice_Release( &list ); */
          }
          break;
        case oyjlOPTIONTYPE_DOUBLE:
          fprintf( stdout, "\t" );
          fprintf( stdout, "%s", oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING) );
          fprintf( stdout, "\t%s%s%s (%s%s%g [≥%g ≤%g])\n", o->description ? o->description:"", o->help?": ":"", o->help?o->help :"", o->value_name?o->value_name:"", o->value_name?":":"", o->values.dbl.d, o->values.dbl.start, o->values.dbl.end );
          break;
        case oyjlOPTIONTYPE_NONE:
          fprintf( stdout, "\t" );
          fprintf( stdout, "%s", oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING) );
          fprintf( stdout, "\t%s%s%s\n", o->description ? o->description:"", o->help?": ":"", o->help?o->help :"" );
        break;
        case oyjlOPTIONTYPE_START: break;
        case oyjlOPTIONTYPE_END: break;
      }
    }
    if(d) fprintf( stdout, "\n" );
  }
  fprintf( stdout, "\n" );
}

/** @brief    Allocate a new options structure
 *  @memberof oyjlOptions_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/03/24
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
oyjlOptions_s * oyjlOptions_New      ( int                 argc,
                                       const char       ** argv )
{
  oyjlOptions_s * opts = calloc( sizeof(oyjlOptions_s), 1 );
  memcpy( opts->type, "oiws", 4 );

  opts->argc = argc;
  opts->argv = argv;
  return opts;
}

/** @brief    Allocate a new ui structure
 *  @memberof oyjlUi_s
 *
 *  The oyjlUi_s contains already options in the opts member.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/03/24
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
oyjlUi_s* oyjlUi_New                 ( int                 argc,
                                       const char       ** argv )
{
  oyjlUi_s * ui = calloc( sizeof(oyjlUi_s), 1 );
  memcpy( ui->type, "oiui", 4 );
  ui->opts = oyjlOptions_New( argc, argv );
  return ui;
}

oyjlOPTIONSTATE_e  oyjlUi_Check      ( oyjlUi_s          * ui,
                                       int                 flags OYJL_UNUSED )
{
  oyjlOPTIONSTATE_e status = oyjlOPTION_NONE;
  int i,ng;
  oyjlOptions_s * opts;
 
  if(!ui) return status;
  opts = ui->opts;

  ng = oyjlOptions_CountGroups(opts);
  if(!ng)
  {
    fprintf(stderr, "no ui::opts::groups\n");
    status = oyjlOPTION_MISSING_VALUE;
  }

  if(!ui->nick || !ui->nick[0])
  {
    fprintf(stderr, "no ui::nick\n");
    status = oyjlOPTION_MISSING_VALUE;
  }

  if(!ui->name || !ui->name[0])
  {
    fprintf(stderr, "no ui::name\n");
    status = oyjlOPTION_MISSING_VALUE;
  }

  for(i = 0; i < ng; ++i)
  {
    oyjlOptionGroup_s * g = &opts->groups[i];
    int d = g->detail ? strlen(g->detail) : 0,
        j;
    if(g->mandatory && g->mandatory[0])
    {
      int n = strlen(g->mandatory);
      for( j = 0; j  < n; ++j )
      {
        char o = g->mandatory[j];
        if( !g->detail || (!strchr(g->detail, o) &&
            o != '|' &&
            o != '#'))
        {
          fprintf(stderr, "\'%c\' not found in group->details\n", o);
          status = oyjlOPTION_MISSING_VALUE;
        }
      }
    }
    for(j = 0; j < d; ++j)
    {
      char oc = g->detail[j];
      oyjlOption_s * o = oyjlOptions_GetOption( opts, oc );
      if(!o)
      {
        fprintf(stderr, "\n%s: option not declared: %c\n", g->name, oc);
        exit(1);
      }
      switch(o->value_type)
      {
        case oyjlOPTIONTYPE_CHOICE:
          {
            int n = 0;
            while(o->values.choices.list && o->values.choices.list[n].nick && o->values.choices.list[n].nick[0] != '\000')
              ++n;
            if( !n && !(o->flags & OYJL_OPTION_FLAG_EDITABLE) &&
                o->o != 'X')
            {
              fprintf( stderr, "%s %s \'%c\' %s\n", _("Usage Error:"), _("Option not supported"), o->o, _("needs OYJL_OPTION_FLAG_EDITABLE or choices") );
              status = oyjlOPTION_NOT_SUPPORTED;
            }
          }
          break;
        case oyjlOPTIONTYPE_FUNCTION:
          break;
        case oyjlOPTIONTYPE_DOUBLE:
          break;
        case oyjlOPTIONTYPE_NONE:
        break;
        case oyjlOPTIONTYPE_START: break;
        case oyjlOPTIONTYPE_END: break;
      }
    }
  }

  return status;
}

/** @brief    Create a new UI structure
 *  @memberof oyjlUi_s
 *
 *  This is a high level convinience function.
 *  The returned oyjlUi_s is a comlete description of the UI and can be
 *  used instantly. The options are parsed, errors are printed, help text
 *  is printed for the boolean -h/--help option. Boolean -v/--verbose
 *  is handled too. The results are set to the declared variables. 
 *  The app_type defaults to "tool", but it can be replaced if needed.
 *
 *  @code
  oyjlUi_s * ui = oyjlUi_Create( argc, argv,
                                       "myCl",
                                       _("My Command"),
                                       _("My Command line tool from Me"),
                                       "my_logo",
                                       info, options, groups, NULL )
    @endcode
 *
 *  @param[in]     argc                number of command line arguments
 *  @param[in]     argv                command line args from C/C++ main()
 *  @param[in]     nick                four byte string; e.g. "myCl"
 *  @param[in]     name                short name of the tool; i18n;
 *                 e.g. _("My Command")
 *  @param[in]     description         compact sentence starting with full name; i18n;
 *                 e.g. _("My Command line tool from Me")
 *  @param[in]     logo                icon name; This variable must contain
 *                 the file name only, without ending. The icon needs
 *                 to be installed in typical icon search path and will be
 *                 detected there. e.g. "my_logo" points to "my_logo.{png|svg}"
 *  @param[in]     info                general information for rich UI's and
 *                                     for help text
 *  @param[in,out] options             the main option declaration, with
 *                 syntax declaration and variable passing for setting results
 *  @param[in]     groups              the option grouping declares
 *                 dependencies of options and provides a UI layout
 *  @param[out]    state               inform about processing
 *                                     - ::oyjlUI_STATE_HELP : help was detected, printed and oyjlUi_s was released
 *                                     - ::oyjlUI_STATE_EXPORT : export of json, man or markdown was detected, printed and oyjlUi_s was released
 *                                     - ::oyjlUI_STATE_VERBOSE : verbose was detected
 *                                     - ::oyjlUI_STATE_OPTION+ : error occured in option parser, message printed, ::oyjlOPTIONSTATE_e is placed in >> ::oyjlUI_STATE_OPTION and oyjlUi_s was released
 *  @return                            UI object for later use
 *
 *  @version Oyjl: 1.0.0
 *  @date    2019/03/24
 *  @since   2018/08/20 (OpenICC: 0.1.1)
 */
oyjlUi_s *  oyjlUi_Create            ( int                 argc,
                                       const char       ** argv,
                                       const char        * nick,
                                       const char        * name,
                                       const char        * description,
                                       const char        * logo,
                                       oyjlUiHeaderSection_s * info,
                                       oyjlOption_s   * options,
                                       oyjlOptionGroup_s * groups,
                                       int               * status )
{
  int help = 0, verbose = 0;
  const char * export = NULL;
  oyjlOption_s * h, * v, * X;
  oyjlOPTIONSTATE_e opt_state = oyjlOPTION_NONE;

  int use_gettext = 0;
#ifdef OYJL_USE_GETTEXT
  use_gettext = 1;
#endif
  oyjlInitLanguageDebug( "Oyjl", "OYJL_DEBUG", oyjl_debug, use_gettext, "OYJL_LOCALEDIR", OYJL_LOCALEDIR, OYJL_DOMAIN, NULL );

  /* allocate options structure */
  oyjlUi_s * ui = oyjlUi_New( argc, argv ); /* argc+argv are required for parsing the command line options */
  /* tell about the tool */
  ui->app_type = "tool";
  ui->nick = nick;
  ui->name = name;
  ui->description = description;
  ui->logo = logo;

  /* Select from *version*, *manufacturer*, *copyright*, *license*, *url*,
   * *support*, *download*, *sources*, *oyjl_module_author* and
   * *documentation* what you see fit. Add new ones as needed. */
  ui->sections = info;
  ui->opts->array = options;
  ui->opts->groups = groups;

  /* get results and check syntax ... */
  opt_state = oyjlOptions_Parse( ui->opts );
  if(opt_state == oyjlOPTION_NONE)
    opt_state = oyjlUi_Check(ui, 0);
  /* ... and report detected errors */
  if(opt_state != oyjlOPTION_NONE)
  {
    fputs( _("... try with --help|-h option for usage text. give up"), stderr );
    fputs( "\n", stderr );
    oyjlUi_Release( &ui);
    if(status)
      *status = opt_state << oyjlUI_STATE_OPTION;
    return NULL;
  }

  X = oyjlOptions_GetOption( ui->opts, 'X' );
  if(X && X->variable_type == oyjlSTRING && X->variable.s)
    export = *X->variable.s;
  h = oyjlOptions_GetOption( ui->opts, 'h' );
  if(h && h->variable_type == oyjlINT && h->variable.i)
    help = *h->variable.i;
  v = oyjlOptions_GetOption( ui->opts, 'v' );
  if(v && v->variable_type == oyjlINT && v->variable.i)
  {
    verbose = *v->variable.i;
    if(status && verbose)
      *status |= oyjlUI_STATE_VERBOSE;
  }
  if(help)
  {
    oyjlUiHeaderSection_s * version = oyjlUi_GetHeaderSection( ui,
                                                               "version" );
    const char * prog = argv[0];
    if(!verbose && prog && strchr(prog,'/'))
      prog = strrchr(prog,'/') + 1;
    oyjlOptions_PrintHelp( ui->opts, ui, verbose, "%s v%s - %s", prog,
                              version && version->name ? version->name : "",
                              ui->description ? ui->description : "" );
    oyjlUi_Release( &ui);
    if(status)
      *status |= oyjlUI_STATE_HELP;
    return NULL;
  }
  if(export)
  {
    if(status)
      *status |= oyjlUI_STATE_EXPORT;
    if(strcmp(export, "json") == 0)
    {
      puts( oyjlUi_ToJson( ui, 0 ) );
      oyjlUi_Release( &ui);
      return NULL;
    }
    if(strcmp(export, "man") == 0)
    {
      puts( oyjlUi_ToMan( ui, 0 ) );
      oyjlUi_Release( &ui);
      return NULL;
    }
    if(strcmp(export, "markdown") == 0)
    {
      puts( oyjlUi_ToMarkdown( ui, 0 ) );
      oyjlUi_Release( &ui);
      return NULL;
    }
  }
  /* done with options handling */

  return ui;
}

/** @brief    Release "oiui"
 *  @memberof oyjlUi_s
 *
 *  Release oyjlUi_s::opts, oyjlUi_s::private_data and oyjlUi_s.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2018/08/14
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
void           oyjlUi_Release     ( oyjlUi_s      ** ui )
{
  char ** list;
  int pos = 0;
  if(!ui || !*ui) return;
  if( *(oyjlOBJECT_e*)*ui != oyjlOBJECT_UI)
  {
    char * a = (char*)*ui;
    char type[5] = {a[0],a[1],a[2],a[3],0};
    fprintf(stderr, "Unexpected object: \"%s\"(expected: \"oyjlUi_s\")\n", type );
    return;
  }
  list = (*ui)->opts->private_data;
  while( list && list[pos] )
    free(list[pos++]);
  if((*ui)->opts->private_data) free((*ui)->opts->private_data);
  if((*ui)->opts) free((*ui)->opts);
  free((*ui));
  *ui = NULL;
}

/** @brief    Return the number of sections of type "oihs"
 *  @memberof oyjlUi_s
 *
 *  The oyjlUi_s contains already options in the opts member.
 *
 *  @version Oyjl: 1.0.0
 *  @date    2018/08/14
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
int     oyjlUi_CountHeaderSections( oyjlUi_s       * ui )
{
  int n = 0;
  while( *(oyjlOBJECT_e*)&ui->sections[n] /*"oihs"*/ == oyjlOBJECT_UI_HEADER_SECTION) ++n;
  return n;
}

/** @brief    Return the section which was specified by its nick name
 *  @memberof oyjlUi_s
 *
 *  @version Oyjl: 1.0.0
 *  @date    2018/08/14
 *  @since   2018/08/14 (OpenICC: 0.1.1)
 */
oyjlUiHeaderSection_s * oyjlUi_GetHeaderSection (
                                       oyjlUi_s          * ui,
                                       const char        * nick )
{
  oyjlUiHeaderSection_s * section = NULL;
  int i, count = oyjlUi_CountHeaderSections(ui);
  for(i = 0; i < count; ++i)
    if( strcmp(ui->sections[i].nick, nick) == 0 )
      section = &ui->sections[i];
  return section;
}

#define OYJL_REG "org/freedesktop/oyjl"

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

char *       oyjlStringToUpper       ( const char        * t )
{
  char * text = oyjlStringCopy(t, malloc);
  int slen = strlen(t), i;
  for(i = 0; i < slen; ++i)
    text[i] = toupper(t[i]);
  return text;
}

static
char *       oyjlExtraManSection  ( oyjlOptions_s  * opts,
                                       const char        * opt_name,
                                       int                 flags )
{
  oyjlOption_s * o = oyjlOptions_GetOptionL( opts, opt_name );
  char * text = NULL;
  if(o)
  {
    int n = 0,l;
    if(o->value_type == oyjlOPTIONTYPE_CHOICE)
    {
      oyjlOptionChoice_s * list = o->values.choices.list;
      while(o->values.choices.list[n].nick && o->values.choices.list[n].nick[0] != '\000') ++n;
      if(n)
      {
        char * up = oyjlStringToUpper( &opt_name[4] );
        oyjlStringReplace( &up, "_", " ", malloc, free );
        const char * section = up;
        if(strcmp(section,"EXAMPLES") == 0)
          section = _("EXAMPLES");
        else if(strcmp(section,"EXIT-STATE") == 0)
          section = _("EXIT-STATE");
        else if(strcmp(section,"ENVIRONMENT VARIABLES") == 0)
          section = _("ENVIRONMENT VARIABLES");
        else if(strcmp(section,"HISTORY") == 0)
          section = _("HISTORY");
        else if(strcmp(section,"FILES") == 0)
          section = _("FILES");
        else if(strcmp(section,"SEE AS WELL") == 0)
          section = _("SEE AS WELL");
        if(flags & oyjlOPTIONSTYLE_MARKDOWN)
          oyjlStringAdd( &text, malloc, free, "## %s\n", _(section) );
        else
          oyjlStringAdd( &text, malloc, free, ".SH %s\n", _(section) );
        for(l = 0; l < n; ++l)
          if(flags & oyjlOPTIONSTYLE_MARKDOWN)
            oyjlStringAdd( &text, malloc, free, "### %s\n%s %s %s\n", list[l].nick, list[l].name, list[l].description, list[l].help );
          else
            oyjlStringAdd( &text, malloc, free, ".TP\n%s\n.br\n%s %s %s\n", list[l].nick, list[l].name, list[l].description, list[l].help );
        free(up);
      }
    }
  }
  return text;
}

static
char *       oyjlExtraManSections ( oyjlOptions_s  * opts, int flags )
{
  char * text = NULL;
  int nopts = oyjlOptions_Count( opts );
  int l;
  for(l = 0; l < nopts; ++l)
  {
    oyjlOption_s * o = &opts->array[l];
    const char * option = o->option;
    int olen = option ? strlen(option) : 0;
    if(olen > 7 && option[0] == 'm' && option[1] == 'a' && option[2] == 'n' && option[3] == '-')
    {
      char * tmp = oyjlExtraManSection(opts, option, flags);
      if(tmp)
      {
        oyjlStringAdd( &text, malloc, free, tmp );
        free(tmp);
      }
    }
  }
  return text;
}

/** @brief    Return a man page from options
 *  @memberof oyjlUi_s
 *
 *  Some man pages might contain some additional sections.
 *  They are supported as options. To generate a custom man page section,
 *  add a blind option to your options list and set the oyjlOption_s::o
 *  char to something non interupting like, dot '.' or similar.
 *  The oyjlOption_s::option string
 *  contains "man-section_head", with "section-head" being adapted to your
 *  needs. The "man-" part will be cut off and 
 *  "section_head" will become uppercase and underline '_' become empty
 *  space: "SECTION HEAD".
 *  Use oyjlOption_s::value_type=oyjlOPTIONTYPE_CHOICE
 *  and place your string list into oyjlOptionChoice_s::nick by filling all
 *  members. Translated section heads are "EXAMPLES, "SEE AS WELL", "HISTORY",
 *  "ENVIRONMENT VARIABLES", "EXIT-STATE" and "FILES".
 *
 *  @version Oyjl: 1.0.0
 *  @date    2018/10/15
 *  @since   2018/10/10 (OpenICC: 0.1.1)
 */
char *       oyjlUi_ToMan            ( oyjlUi_s          * ui,
                                       int                 flags OYJL_UNUSED )
{
  char * text = NULL, * tmp;
  const char * date = NULL,
             * desc = NULL,
             * mnft = NULL, * mnft_url = NULL,
             * copy = NULL, * lice = NULL,
             * bugs = NULL, * bugs_url = NULL,
             * vers = NULL;
  int i,n,ng;
  oyjlOptions_s * opts;
 
  if(!ui) return text;
  opts = ui->opts;

  n = oyjlUi_CountHeaderSections( ui );
  for(i = 0; i < n; ++i)
  {
    oyjlUiHeaderSection_s * s = &ui->sections[i];
    if(strcmp(s->nick, "manufacturer") == 0) { mnft = s->name; mnft_url = s->description; }
    else if(strcmp(s->nick, "copyright") == 0) copy = s->name;
    else if(strcmp(s->nick, "license") == 0) lice = s->name;
    else if(strcmp(s->nick, "url") == 0) continue;
    else if(strcmp(s->nick, "support") == 0) { bugs = s->name; bugs_url = s->description; }
    else if(strcmp(s->nick, "download") == 0) continue;
    else if(strcmp(s->nick, "sources") == 0) continue;
    else if(strcmp(s->nick, "development") == 0) continue;
    else if(strcmp(s->nick, "oyjl_module_author") == 0) continue;
    else if(strcmp(s->nick, "documentation") == 0) desc = s->description ? s->description : s->name;
    else if(strcmp(s->nick, "version") == 0) vers = s->name;
    else if(strcmp(s->nick, "date") == 0) date = s->description ? s->description : s->name;
  }

  ng = oyjlOptions_CountGroups(opts);
  if(!ng) return NULL;

  if(ui->app_type && ui->app_type[0])
  {
    int tool = strcmp( ui->app_type, "tool" ) == 0;
    oyjlStringAdd( &text, malloc, free, ".TH \"%s\" %d \"%s\" \"%s\"\n", ui->nick,
                   tool?1:7, date?date:"", tool?"User Commands":"Misc" );
  }

  oyjlStringAdd( &text, malloc, free, ".SH NAME\n%s %s%s \\- %s\n", ui->nick, vers?"v":"", vers?vers:"", ui->name );

  oyjlStringAdd( &text, malloc, free, ".SH %s\n", _("SYNOPSIS") );
  for(i = 0; i < ng; ++i)
  {
    oyjlOptionGroup_s * g = &opts->groups[i];
    const char * syn = oyjlOptions_PrintHelpSynopsis( opts, g,
                         oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MAN );
    if(syn[0])
      oyjlStringAdd( &text, malloc, free, "%s\n%s", syn, (i < (ng-1)) ? ".br\n" : "" );
  }

  if(desc)
    oyjlStringAdd( &text, malloc, free, ".SH %s\n%s\n", _("DESCRIPTION"), desc );

  oyjlStringAdd( &text, malloc, free, ".SH %s\n", _("OPTIONS") );
  for(i = 0; i < ng; ++i)
  {
    oyjlOptionGroup_s * g = &opts->groups[i];
    int d = g->detail ? strlen(g->detail) : 0,
        j;
    oyjlStringAdd( &text, malloc, free, ".TP\n%s\n", g->description  );
    if(g->mandatory && g->mandatory[0])
    {
      oyjlStringAdd( &text, malloc, free, "%s\n", oyjlOptions_PrintHelpSynopsis( opts, g, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MAN ) );
    }
    oyjlStringAdd( &text, malloc, free, ".br\n"  );
    if(g->help)
    {
      oyjlStringAdd( &text, malloc, free, "%s\n.br\n.sp\n.br\n", g->help );
    }
    for(j = 0; j < d; ++j)
    {
      char oc = g->detail[j];
      oyjlOption_s * o = oyjlOptions_GetOption( opts, oc );
      if(!o)
      {
        fprintf(stderr, "\n%s: option not declared: %c\n", g->name, oc);
        exit(1);
      }
      switch(o->value_type)
      {
        case oyjlOPTIONTYPE_CHOICE:
          {
            int n = 0,l;
            oyjlStringAdd( &text, malloc, free, "%s", oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING | oyjlOPTIONSTYLE_MAN) );
            oyjlStringAdd( &text, malloc, free, "\t%s%s%s\n.br\n", o->description ? o->description:"", o->help?": ":"", o->help?o->help :"" );
            if(o->flags & OYJL_OPTION_FLAG_EDITABLE)
              break;
            while(o->values.choices.list[n].nick && o->values.choices.list[n].nick[0] != '\000')
              ++n;
            for(l = 0; l < n; ++l)
              oyjlStringAdd( &text, malloc, free, "\t\\-%c %s\t\t# %s %s %s\n.br\n",
                  o->o,
                  o->values.choices.list[l].nick,
                  o->values.choices.list[l].name && o->values.choices.list[l].nick[0] ? o->values.choices.list[l].name : o->values.choices.list[l].description,
                  o->values.choices.list[l].help&&o->values.choices.list[l].help[0]?" - ":"",
                  o->values.choices.list[l].help?o->values.choices.list[l].help:"" );
          }
          break;
        case oyjlOPTIONTYPE_FUNCTION:
          {
            int n = 0,l;
            oyjlOptionChoice_s * list;
            oyjlStringAdd( &text, malloc, free, "%s", oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING | oyjlOPTIONSTYLE_MAN) );
            oyjlStringAdd( &text, malloc, free, "\t%s%s%s\n.br\n", o->description ? o->description:"", o->help?": ":"", o->help?o->help :"" );
            if(o->flags & OYJL_OPTION_FLAG_EDITABLE)
              break;
            list = oyjlOption_GetChoices_(o, NULL, opts );
            if(list)
              while(list[n].nick && list[n].nick[0] != '\000')
                ++n;
            for(l = 0; l < n; ++l)
              oyjlStringAdd( &text, malloc, free, "\t\\-%c %s\t\t# %s\n.br\n", o->o, list[l].nick, list[l].name && list[l].nick[0] ? list[l].name : list[l].description );
            /* not possible, as the result of oyjlOption_GetChoices_() is cached - oyjlOptionChoice_Release( &list ); */
          }
          break;
        case oyjlOPTIONTYPE_DOUBLE:
          oyjlStringAdd( &text, malloc, free, "%s", oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING | oyjlOPTIONSTYLE_MAN) );
          oyjlStringAdd( &text, malloc, free, "\t%s%s%s (%s%s%g [≥%g ≤%g])\n.br\n", o->description ? o->description:"", o->help?": ":"", o->help?o->help :"", o->value_name?o->value_name:"", o->value_name?":":"", o->values.dbl.d, o->values.dbl.start, o->values.dbl.end );
          break;
        case oyjlOPTIONTYPE_NONE:
          oyjlStringAdd( &text, malloc, free, "%s", oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING | oyjlOPTIONSTYLE_MAN) );
          oyjlStringAdd( &text, malloc, free, "\t%s%s%s\n.br\n", o->description ? o->description:"", o->help?": ":"", o->help?o->help :"" );
        break;
        case oyjlOPTIONTYPE_START: break;
        case oyjlOPTIONTYPE_END: break;
      }
    }
  }

  tmp = oyjlExtraManSections( opts, oyjlOPTIONSTYLE_MAN );
  if(tmp)
  {
    oyjlStringAdd( &text, malloc, free, "%s", tmp );
    free(tmp);
  }

  if(mnft)
    oyjlStringAdd( &text, malloc, free, ".SH %s\n%s %s\n", _("AUTHOR"), mnft, mnft_url?mnft_url:"" );

  if(lice || copy)
  {
    oyjlStringAdd( &text, malloc, free, ".SH %s\n%s\n", _("COPYRIGHT"), copy?copy:"" );
    if(lice)
      oyjlStringAdd( &text, malloc, free, ".br\n%s: %s\n", _("License"), lice?lice:"" );
  }

  if(bugs)
    oyjlStringAdd( &text, malloc, free, ".SH %s\n%s %s\n", _("BUGS"), bugs, bugs_url?bugs_url:"" );

  return text;
}

/** @brief    Return markdown formated text from options
 *  @memberof oyjlUi_s
 *
 *  @see oyjlUi_ToMan()
 *
 *  @version Oyjl: 1.0.0
 *  @date    2018/11/07
 *  @since   2018/11/07 (OpenICC: 0.1.1)
 */
char *       oyjlUi_ToMarkdown       ( oyjlUi_s          * ui,
                                       int                 flags OYJL_UNUSED )
{
  char * text = NULL, * tmp, * doxy_link = NULL;
  const char * date = NULL,
             * desc = NULL,
             * mnft = NULL, * mnft_url = NULL,
             * copy = NULL, * lice = NULL,
             * bugs = NULL, * bugs_url = NULL,
             * vers = NULL,
             * country = NULL;
  int i,n,ng;
  oyjlOptions_s * opts;

  if( !ui ) return text;

  opts = ui->opts;
  n = oyjlUi_CountHeaderSections( ui );
  for(i = 0; i < n; ++i)
  {
    oyjlUiHeaderSection_s * s = &ui->sections[i];
    if(strcmp(s->nick, "manufacturer") == 0) { mnft = s->name; mnft_url = s->description; }
    else if(strcmp(s->nick, "copyright") == 0) copy = s->name;
    else if(strcmp(s->nick, "license") == 0) lice = s->name;
    else if(strcmp(s->nick, "url") == 0) continue;
    else if(strcmp(s->nick, "support") == 0) { bugs = s->name; bugs_url = s->description; }
    else if(strcmp(s->nick, "download") == 0) continue;
    else if(strcmp(s->nick, "sources") == 0) continue;
    else if(strcmp(s->nick, "development") == 0) continue;
    else if(strcmp(s->nick, "oyjl_module_author") == 0) continue;
    else if(strcmp(s->nick, "documentation") == 0) desc = s->description ? s->description : s->name;
    else if(strcmp(s->nick, "version") == 0) vers = s->name;
    else if(strcmp(s->nick, "date") == 0) date = s->description ? s->description : s->name;
  }

  ng = oyjlOptions_CountGroups(opts);
  if(!ng) return NULL;

#ifdef OYJL_HAVE_LANGINFO_H
  country = nl_langinfo( _NL_ADDRESS_LANG_AB );
#endif

  oyjlStringAdd( &doxy_link, malloc, free, "{#%s%s}", ui->nick, country?country:"" );
  oyjlStringReplace( &doxy_link, "-", "", malloc, free );

  oyjlStringAdd( &text, malloc, free, "# %s %s%s %s\n", ui->nick, vers?"v":"", vers?vers:"", doxy_link );

  if(ui->app_type && ui->app_type[0])
  {
    int tool = strcmp( ui->app_type, "tool" ) == 0;
    oyjlStringAdd( &text, malloc, free, "*\"%s\"* *%d* *\"%s\"* \"%s\"\n", ui->nick,
                   tool?1:7, date?date:"", tool?"User Commands":"Misc" );
  }

  oyjlStringAdd( &text, malloc, free, "## NAME\n%s %s%s - %s\n", ui->nick, vers?"v":"", vers?vers:"", ui->name );

  oyjlStringAdd( &text, malloc, free, "## %s\n", _("SYNOPSIS") );
  for(i = 0; i < ng; ++i)
  {
    oyjlOptionGroup_s * g = &opts->groups[i];
    const char * syn = oyjlOptions_PrintHelpSynopsis( opts, g,
                         oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MARKDOWN );
    if(syn[0])
      oyjlStringAdd( &text, malloc, free, "%s\n%s", syn, (i < (ng-1)) ? "<br />\n" : "" );
  }

  if(desc)
    oyjlStringAdd( &text, malloc, free, "## %s\n%s\n", _("DESCRIPTION"), desc );

  oyjlStringAdd( &text, malloc, free, "## %s\n", _("OPTIONS") );
  for(i = 0; i < ng; ++i)
  {
    oyjlOptionGroup_s * g = &opts->groups[i];
    int d = g->detail ? strlen(g->detail) : 0,
        j;
    oyjlStringAdd( &text, malloc, free, "### %s\n", g->description  );
    if(g->mandatory && g->mandatory[0])
    {
      oyjlStringAdd( &text, malloc, free, "%s\n", oyjlOptions_PrintHelpSynopsis( opts, g, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_MARKDOWN ) );
    }
    oyjlStringAdd( &text, malloc, free, "\n"  );
    if(g->help)
    {
      oyjlStringAdd( &text, malloc, free, "%s\n\n", g->help );
    }
    for(j = 0; j < d; ++j)
    {
      char oc = g->detail[j];
      oyjlOption_s * o = oyjlOptions_GetOption( opts, oc );
      if(!o)
      {
        fprintf(stderr, "\n%s: option not declared: %c\n", g->name, oc);
        exit(1);
      }
      switch(o->value_type)
      {
        case oyjlOPTIONTYPE_CHOICE:
          {
            int n = 0,l;
            oyjlStringAdd( &text, malloc, free, "* %s", oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING | oyjlOPTIONSTYLE_MARKDOWN) );
            oyjlStringAdd( &text, malloc, free, "\t%s%s%s\n", o->description ? o->description:"", o->help?": ":"", o->help?o->help :"" );
            if(o->flags & OYJL_OPTION_FLAG_EDITABLE)
              break;
            while(o->values.choices.list[n].nick && o->values.choices.list[n].nick[0] != '\000')
              ++n;
            for(l = 0; l < n; ++l)
              oyjlStringAdd( &text, malloc, free, "   * -%c %s\t\t# %s\n", o->o, o->values.choices.list[l].nick, o->values.choices.list[l].name && o->values.choices.list[l].nick[0] ? o->values.choices.list[l].name : o->values.choices.list[l].description );
          }
          break;
        case oyjlOPTIONTYPE_FUNCTION:
          {
            int n = 0,l;
            oyjlOptionChoice_s * list;
            oyjlStringAdd( &text, malloc, free, "* %s", oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING | oyjlOPTIONSTYLE_MARKDOWN) );
            oyjlStringAdd( &text, malloc, free, "\t%s%s%s\n", o->description ? o->description:"", o->help?": ":"", o->help?o->help :"" );
            if(o->flags & OYJL_OPTION_FLAG_EDITABLE)
              break;
            list = oyjlOption_GetChoices_(o, NULL, opts );
            if(list)
              while(list[n].nick && list[n].nick[0] != '\000')
                ++n;
            for(l = 0; l < n; ++l)
              oyjlStringAdd( &text, malloc, free, "   * -%c %s\t\t# %s\n", o->o, list[l].nick, list[l].name && list[l].nick[0] ? list[l].name : list[l].description );
            /* not possible, as the result of oyjlOption_GetChoices_() is cached - oyjlOptionChoice_Release( &list ); */
          }
          break;
        case oyjlOPTIONTYPE_DOUBLE:
          oyjlStringAdd( &text, malloc, free, "* %s", oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING | oyjlOPTIONSTYLE_MARKDOWN) );
          oyjlStringAdd( &text, malloc, free, "\t%s%s%s (%s%s%g [≥%g ≤%g])\n", o->description ? o->description:"", o->help?": ":"", o->help?o->help :"", o->value_name?o->value_name:"", o->value_name?":":"", o->values.dbl.d, o->values.dbl.start, o->values.dbl.end );
          break;
        case oyjlOPTIONTYPE_NONE:
          oyjlStringAdd( &text, malloc, free, "* %s", oyjlOption_PrintArg(o, oyjlOPTIONSTYLE_ONELETTER | oyjlOPTIONSTYLE_STRING | oyjlOPTIONSTYLE_MARKDOWN) );
          oyjlStringAdd( &text, malloc, free, "\t%s%s%s\n", o->description ? o->description:"", o->help?": ":"", o->help?o->help :"" );
        break;
        case oyjlOPTIONTYPE_START: break;
        case oyjlOPTIONTYPE_END: break;
      }
    }
    oyjlStringAdd( &text, malloc, free, "\n"  );
  }

  tmp = oyjlExtraManSections( opts, oyjlOPTIONSTYLE_MARKDOWN );
  if(tmp)
  {
    oyjlStringAdd( &text, malloc, free, "%s", tmp );
    free(tmp);
  }

  if(mnft)
    oyjlStringAdd( &text, malloc, free, "## %s\n%s %s\n", _("AUTHOR"), mnft, mnft_url?mnft_url:"" );

  if(lice || copy)
  {
    oyjlStringAdd( &text, malloc, free, "## %s\n*%s*\n", _("COPYRIGHT"), copy?copy:"" );
    if(lice)
      oyjlStringAdd( &text, malloc, free, "\n\n### %s\n%s\n", _("License"), lice?lice:"" );
  }

  if(bugs && bugs_url)
    oyjlStringAdd( &text, malloc, free, "## %s\n[%s](%s)\n", _("BUGS"), bugs, bugs_url?bugs_url:"" );
  else if(bugs)
    oyjlStringAdd( &text, malloc, free, "## %s\n[%s](%s)\n", _("BUGS"), bugs, bugs );

  oyjlStringReplace( &text, "`", "\\`", malloc, free );
  oyjlStringReplace( &text, "-", "\\-", malloc, free );
  oyjlStringReplace( &text, "_", "\\_", malloc, free );

  return text;
}
// TODO: make the qml renderer aware of mandatory options as part of sending a call to the tool; add action button to all manatory options except bool options; render mandatory switch as a button
// TODO: the renderer keeps as simple as possible like the command line
// TODO: MAN page synopsis logic ...
// TODO: man page generator: /usr/share/man/man1/ftp.1.gz + http://man7.org/linux/man-pages/man7/groff_mdoc.7.html
// TODO: synopsis syntax ideas: https://unix.stackexchange.com/questions/17833/understand-synopsis-in-manpage


/** 
 *  @} *//* oyjl_args
 */

/* private stuff */

oyjlUiHeaderSection_s * oyjlUiInfo   ( const char          * documentation )
{
  oyjlUiHeaderSection_s s[] = {
    /* type,  nick,      label,name,                 description */
    { "oihs", "version", NULL, OYJL_VERSION_NAME, NULL },
    { "oihs", "manufacturer", NULL, "Kai-Uwe Behrmann", "http://www.oyranos.org" },
    { "oihs", "copyright", NULL, "Copyright 2018-2019 Kai-Uwe Behrmann", NULL },
    { "oihs", "license", NULL, "newBSD", "http://www.oyranos.org" },
    { "oihs", "url", NULL, "http://www.oyranos.org", NULL },
    { "oihs", "support", NULL, "https://www.github.com/oyranos-cms/oyranos/issues", NULL },
    { "oihs", "download", NULL, "https://github.com/oyranos-cms/oyranos/releases", NULL },
    { "oihs", "sources", NULL, "https://github.com/oyranos-cms/oyranos", NULL },
    { "oihs", "development", NULL, "https://github.com/oyranos-cms/oyranos", NULL },
    { "oihs", "oyjl_module_author", NULL, "Kai-Uwe Behrmann", "http://www.behrmann.name" },
    { "oihs", "documentation", NULL, "http://www.oyranos.org", documentation },
    { "oihs", "date", NULL, "1970-01-01T12:00:00", "" },
    { "", NULL, NULL, NULL, NULL }
  };
  return (oyjlUiHeaderSection_s*) oyjlStringAppendN( NULL, (const char*)s, sizeof(s), malloc );
}

