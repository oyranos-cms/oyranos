/** @file oyranos_policy.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2006-2022 (C) Kai-Uwe Behrmann
 *
 *  @brief    policy loader - for usage during installation and on commandline
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2006/09/14
 *
 *  The program takes a policy XML file as argument and sets the behaviour 
 *  accordingly.
 */


#include "oyranos.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_helper_macros.h"
#include "oyranos_internal.h"
#include "oyranos_config.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyranos_version.h"

#include "oyjl.h"
#include "oyjl_version.h"
extern char **environ;
#ifdef OYJL_HAVE_LOCALE_H
#include <locale.h>
#endif
#ifdef OYJL_USE_GETTEXT
# ifdef OYJL_HAVE_LIBINTL_H
#  include <libintl.h> /* bindtextdomain() */
# endif
#endif

/* This function is called the
 * * first time for GUI generation and then
 * * for executing the tool.
 */
int myMain( int argc, const char ** argv )
{
  int error = 0;
  int state = 0;
  int current_policy = 0;
  int file_name = 0;
  int internal_name = 0;
  int v = 0;
  int list_policies = 0;
  int dump_policy = 0;
  const char * import_policy = 0;
  const char * save_policy = 0;
  int list_paths = 0;
  int system_wide = 0;
  int docbook = 0;
  const char * doc_title = NULL,
             * doc_version = NULL;
  int long_help = 0;
  oySCOPE_e scope = oySCOPE_USER;
  size_t size = 0;
  char * xml = NULL;
  char * import_policy_fn = NULL;

  const char * help = NULL;
  int verbose = 0;
  int version = 0;
  const char * render = NULL;
  const char * export = 0;

  /* handle options */

  /* declare the option choices  *   nick,          name,               description,                  help */
  oyjlOptionChoice_s E_choices[] = {{"OY_DEBUG", _("set the Oyranos debug level."),NULL,                         NULL},
                                    {"XDG_CONFIG_HOME",_("route Oyranos to top directories containing resources. The derived paths for policies have a \"color/settings\" appended."),_("http://www.oyranos.com/wiki/index.php?title=OpenIccDirectoryProposal"),NULL},
                                    {NULL,NULL,NULL,NULL}};

  oyjlOptionChoice_s A_choices[] = {{"",_("Dump out the actual settings"),_("oyranos-policy -d"),                         NULL},
                                    {"",_("Set new policy"),_("oyranos-policy -i policy_filename"),                         NULL},
                                    {"",_("List available policies"),_("oyranos-policy -l"),                         NULL},
                                    {"",_("Currently active policy including its file name"),_("oyranos-policy -cfe"),                         NULL},
                                    {"",_("Save and Install to a new policy"),_("oyranos-policy -s policy_name"),                         NULL},
                                    {"",_("Print a help text"),_("oyranos-policy -h"),                         NULL},
                                    {NULL,NULL,NULL,NULL}};

  oyjlOptionChoice_s S_choices[] = {{"oyranos-config(3) oyranos-monitor(1) oyranos(3)",NULL,               NULL,                         NULL},
                                    {"http://www.oyranos.org",NULL,               NULL,                         NULL},
                                    {NULL,NULL,NULL,NULL}};

  /* declare options - the core information; use previously declared choices */
  oyjlOption_s oarray[] = {
  /* type,   flags,                      o,  option,          key,      name,          description,                  help, value_name,         
        value_type,              values,             variable_type, variable_name */
    {"oiwi", 0,                          "c","current-policy",NULL,     _("Current Policy"),  _("Show current active policy name"),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&current_policy}},
    {"oiwi", 0,                          "f","file-name",     NULL,     _("File Name"),_("Show full filename including path."),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&file_name}},
    {"oiwi", 0,                          "e","internal-name", NULL,     _("Internal Name"),_("Show display name."),      NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&internal_name}},
    {"oiwi", 0,                          "l","list-policies", NULL,     _("List Policies"),     _("List available policies"), NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&list_policies}},
    {"oiwi", 0,                          "d","dump",          NULL,     _("Dump"),     _("Dump out all settings in Oyranos' own xml-ish text format"),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&dump_policy}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "i","import-policy", NULL,     _("Import Policy"),_("Must be in Oyranos' own xml-ish text format"),NULL, _("FILENAME"),      
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&import_policy}},
    {"oiwi", 0,                          "p","list-paths",    NULL,     _("List Paths"),_("List search paths"),       NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&list_paths}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "s","save-policy",   NULL,     _("Save"),     _("Save and Install to a new policy"),NULL, _("FILENAME"),      
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&save_policy}},
    {"oiwi", 0,                          NULL,"system-wide",  NULL,     _("System Wide"),_("Do system wide, might need admin or root privileges"),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&system_wide}},
    {"oiwi", 0,                          NULL,"long-help",    NULL,     _("Long Help"),_("Generate Oyranos Documentation"), _("HTML Format"), NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&long_help}},
    {"oiwi", 0,                          NULL,"docbook",      NULL,     _("Docbook"),  _("Generate Oyranos Documentation"), _("Docbook Format"), NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&docbook}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  NULL,"doc-title",    NULL,     _("Doc Title"),NULL,                         NULL, _("TEXT"),      
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&doc_title}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  NULL,"doc-version",  NULL,     _("Doc Version"),NULL,                       NULL, _("TEXT"),      
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&doc_version}},
    {"oiwi", 0,                          "E","man-environment",NULL,    "Man Environment",NULL,                         NULL, NULL,               
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)E_choices, sizeof(E_choices), malloc ), 0}}, oyjlNONE,      {}},
    {"oiwi", 0,                          "A","man-examples",  NULL,     "Man Examples",NULL,                         NULL, NULL,               
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)A_choices, sizeof(A_choices), malloc ), 0}}, oyjlNONE,      {}},
    {"oiwi", 0,                          "S","man-see_also",  NULL,     "Man See_also",NULL,                         NULL, NULL,               
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)S_choices, sizeof(S_choices), malloc ), 0}}, oyjlNONE,      {}},
    /* The --render option can be hidden and used only internally. */
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE|OYJL_OPTION_FLAG_MAINTENANCE, "R", "render", NULL, NULL,  NULL,  NULL, NULL, oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s=&render} },
    {"oiwi", OYJL_OPTION_FLAG_ACCEPT_NO_ARG, "h", "help", NULL, NULL, NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s=&help} },
    {"oiwi", 0, NULL,"synopsis",NULL, NULL,         NULL,         NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlNONE, {0} },
    {"oiwi", 0, "v", "verbose", NULL, _("verbose"), _("verbose"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&verbose} },
    {"oiwi", 0, "V", "version", NULL, _("version"), _("Version"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&version} },
    /* default option template -X|--export */
    {"oiwi", 0, "X", "export", NULL, NULL, NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices = {NULL, 0}}, oyjlSTRING, {.s=&export} },
    {"",0,0,NULL,NULL,NULL,NULL,NULL, NULL, oyjlOPTIONTYPE_END, {0},0,{0}}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups[] = {
  /* type,   flags, name,               description,                  help,               mandatory,     optional,      detail */
    {"oiwg", 0,     NULL,               _("Current policy"), NULL,               "c",           "f,e,v",       "c" },
    {"oiwg", 0,     NULL,               _("List available policies"), NULL,               "l",           "f,e,v",       "l,f,e"},
    {"oiwg", 0,     NULL,               _("Dump out the actual settings"),NULL,           "d",           "v",           "d" },
    {"oiwg", 0,     NULL,               _("Select active policy"),    NULL,               "i",           "v",           "i" },
    {"oiwg", 0,     NULL,               _("List search paths"),       NULL,               "p",           "v",           "p" },
    {"oiwg", 0,     NULL,               _("Save to a new policy"),    NULL,               "s",           "system-wide,v","s,system-wide"},
    {"oiwg", 0,     NULL,               _("Documentation"),           NULL,               "long-help|docbook","doc-title,doc-version","long-help,docbook,doc-title,doc-version"},
    {"oiwg", OYJL_GROUP_FLAG_GENERAL_OPTS, _("Misc"), _("General options"), NULL,         "X|h|V|R",     "v",           "h,X,R,V,v"},
    {"",0,0,0,0,0,0,0}
  };

  oyjlUiHeaderSection_s * sections = oyUiInfo(_("The tool dumps out / read a configuration of the Oyranos color management system (CMS)."),
                  "2015-02-13T12:00:00", "February 13, 2015");
  oyjlUi_s * ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oyranos-policy", _("Oyranos CMS policy tool"), _("The Tool gives information of Oyranos CMS policies."),
#ifdef __ANDROID__
                                       ":/images/logo.svg", // use qrc
#else
                                       "oyranos_logo",
#endif
                                       sections, oarray, groups, &state );
  verbose = v;
  if( state & oyjlUI_STATE_EXPORT &&
      export &&
      strcmp(export,"json+command") != 0)
    goto clean_main;
  if(state & oyjlUI_STATE_HELP)
  {
    fprintf( stderr, "%s\n\tman oyranos-policy\n\n", _("For more information read the man page:") );
    goto clean_main;
  }

  if(ui && verbose)
  {
    char * json = oyjlOptions_ResultsToJson( ui->opts, 0 );
    if(json)
      fputs( json, stderr );
    fputs( "\n", stderr );

    char * text = oyjlOptions_ResultsToText( ui->opts );
    if(text)
      fputs( text, stderr );
    fputs( "\n", stderr );
  }

  if(ui && (export && strcmp(export,"json+command") == 0))
  {
    char * json = oyjlUi_ToJson( ui, 0 ),
         * json_commands = NULL;
    oyjlStringAdd( &json_commands, malloc, free, "{\n  \"command_set\": \"%s\"", argv[0] );
    oyjlStringAdd( &json_commands, malloc, free, "%s", &json[1] ); /* skip opening '{' */
    puts( json_commands );
    goto clean_main;
  }

  /* Render boilerplate */
  if(ui && render)
  {
#if !defined(NO_OYJL_ARGS_RENDER)
    int debug = verbose;
    oyjlTermColorInit( OYJL_RESET_COLORTERM | OYJL_FORCE_COLORTERM );
    oyjlArgsRender( argc, argv, NULL, NULL,NULL, debug, ui, myMain );
#else
    fprintf( stderr, "No render support compiled in. For a GUI use -X json and load into oyjl-args-render viewer." );
#endif
  } else if(ui)
  {
    /* ... working code goes here ... */

    if(system_wide)
      scope = oySCOPE_SYSTEM;
  /* check the default paths */
  /*oyPathAdd( OY_PROFILE_PATH_USER_DEFAULT );*/


  /* load the policy file into memory */
  import_policy_fn = oyMakeFullFileDirName_(import_policy);
  if(oyIsFile_(import_policy_fn))
  {
    xml = oyReadFileToMem_( oyMakeFullFileDirName_(import_policy), &size,
                            oyAllocateFunc_ );
    oyDeAllocateFunc_( import_policy_fn );
  }
  /* parse and set policy */
  if(xml)
  {
    oyReadXMLPolicy( oyGROUP_ALL, xml );
    oyDeAllocateFunc_( xml );
  }
  else if ( import_policy )
  {
    error = oyPolicySet( import_policy, 0 );
    if(error)
      fprintf( stderr, "%s:%d could not read file: %s\n",__FILE__,__LINE__, import_policy);
    return 1;
  }

  if(save_policy)
  {
    error = oyPolicySaveActual( oyGROUP_ALL, scope, save_policy );
    if(!error)
      fprintf( stdout, "%s \"%s\"\n",
               _("installed new policy"), save_policy);
    else
      fprintf( stdout, "\"%s\" %s %d\n", save_policy,
               _("installation of new policy file failed with error:"), error);

  } else
  if(current_policy || list_policies || list_paths)
  {
    const char ** names = NULL;
    int count = 0, i, current = -1;
    oyOptionChoicesGet( oyWIDGET_POLICY, &count, &names, &current );

    if(list_policies)
      for(i = 0; i < count; ++i)
      {
        if(file_name)
        {
          char * full_name = NULL;
          error = oyPolicyFileNameGet_( names[i],
                                            &full_name,
                                            oyAllocateFunc_ );
          if(error)
            fprintf(stderr, "%s error: %d\n", names[i], error);
          if(internal_name)
            fprintf(stdout, "%s (%s)\n", names[i], full_name);
          else
            fprintf(stdout, "%s\n", full_name);
          oyFree_m_( full_name );
        } else
          fprintf(stdout, "%s\n", names[i]);
      }

    if(current_policy)
    {
      fprintf( stderr, "%s\n", _("Currently active policy:"));
      if(current >= 0 && file_name)
      {
        char * full_name = NULL;
        error = oyPolicyFileNameGet_( names[current], &full_name,
                                          oyAllocateFunc_ );
        if(internal_name)
          fprintf(stdout, "%s (%s)\n", names[current], full_name);
        else
          fprintf(stdout, "%s\n", full_name);
        oyFree_m_( full_name );
      } else
        fprintf( stdout, "%s\n", current>=0?names[current]:"---");
    }

    if(list_paths)
    {
      char ** path_names = oyDataPathsGet_( &count, "color/settings",
                                              oyALL, oySCOPE_USER_SYS,
                                              oyAllocateFunc_ );
      fprintf(stdout, "%s:\n", _("Policy search paths"));
      for(i = 0; i < count; ++i)
        fprintf(stdout, "%s\n", path_names[i]);

      oyStringListRelease_(&path_names, count, oyDeAllocateFunc_);
    }

  } else
  if(dump_policy)
  {
    size = 0;
    xml = oyPolicyToXML( oyGROUP_ALL, 1, oyAllocateFunc_ );
    DBG_PROG2_S("%s:%d new policy:\n\n",__FILE__,__LINE__);
    fprintf(stdout, "%s\n", xml);

    if(xml) oyDeAllocateFunc_( xml );

  } else
  if(long_help)
  {
    const char * opts[] = {"add_html_header","1",
                           "add_oyranos_title","1",
                           "add_oyranos_copyright","1",
                           NULL,NULL,
                           NULL,NULL,
                           NULL};
    int pos = 6;
    if(doc_title)
    {
      opts[pos++] = "title";
      opts[pos++] = doc_title;
    }
    if(doc_version)
    {
      opts[pos++] = "version";
      opts[pos++] = doc_version;
    }
    size = 0;
    xml = oyDescriptionToHTML( oyGROUP_ALL, opts, oyAllocateFunc_ );
    fprintf(stdout, "%s\n", xml);

    if(xml) oyDeAllocateFunc_( xml );
  } else
  if(docbook)
  {
    const char * opts[] = {"add_html_header","1",
                           "add_oyranos_title","1",
                           "add_oyranos_copyright","1",
                           "format","2",
                           NULL,NULL,
                           NULL,NULL,
                           NULL};
    int pos = 8;
    if(doc_title)
    {
      opts[pos++] = "title";
      opts[pos++] = doc_title;
    }
    if(doc_version)
    {
      opts[pos++] = "version";
      opts[pos++] = doc_version;
    }
    size = 0;
    xml = oyDescriptionToHTML( oyGROUP_ALL, opts, oyAllocateFunc_ );
    fprintf(stdout, "%s\n", xml);

    if(xml) oyDeAllocateFunc_( xml );
  }

  }
  else error = 1;

  clean_main:
  free(sections);
  {
    int i = 0;
    while(oarray[i].type[0])
    {
      if(oarray[i].value_type == oyjlOPTIONTYPE_CHOICE && oarray[i].values.choices.list)
        free(oarray[i].values.choices.list);
      ++i;
    }
  }
  oyjlUi_Release( &ui );

  return error;
}

extern int * oyjl_debug;
char ** environment = NULL;
int main( int argc_, char**argv_, char ** envv )
{
  int argc = argc_;
  char ** argv = argv_;

#ifdef __ANDROID__
  argv = calloc( argc + 2, sizeof(char*) );
  memcpy( argv, argv_, (argc + 2) * sizeof(char*) );
  argv[argc++] = "--render=gui"; /* start Renderer (e.g. QML) */
  environment = environ;
#else
  environment = envv;
#endif

  /* language needs to be initialised before setup of data structures */
#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif
  oyExportStart_(EXPORT_CHECK_NO);

  myMain(argc, (const char **)argv);

#ifdef __ANDROID__
  free( argv );
#endif
  oyFinish_( FINISH_IGNORE_I18N | FINISH_IGNORE_CACHES );

  return 0;
}


