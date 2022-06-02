/** @file oyranos-config.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2017-2022 (C) Kai-Uwe Behrmann
 *
 *  @brief    DB manipulation tool
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2017/11/13
 *
 */
/* !cc -Wall -g test_device.c -o test_device2 -L. `oyranos-config --cflags --ldstaticflags` -lm -lltdl */

/* cc -Wall -g test_device.c -o test_device2 -L. `oyranos-config --cflags --ldstaticflags` `pkg-config --cflags --libs libxml-2.0` -lm -I ../../ -I ../../API_generated/ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oyranos.h"
#include "oyranos_db.h"
#include "oyranos_devices.h"
#include "oyranos_helper.h"
#include "oyranos_helper_macros_cli.h"
#include "oyranos_i18n.h"
#include "oyranos_icc.h"
#include "oyranos_module_internal.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"
#include "oyranos_config_internal.h"
#include "oyranos_git_version.h"
#include "oyProfiles_s.h"

#include <locale.h>

void* oyAllocFunc(size_t size) {return malloc (size);}
void  oyDeAllocFunc ( oyPointer ptr) { if(ptr) free (ptr); }
void  getKey                         ( const char        * key,
                                       oySCOPE_e           scope,
                                       int                 verbose,
                                       int                 print );


#ifdef HAVE_DBUS
#include "oyranos_dbus_macros.h"
#include "oyranos_threads.h"
oyDBusFilter_m
oyWatchDBus_m( oyDBusFilter )
oyFinishDBus_m
int oy_dbus_config_changed = 0;
static void oyConfigCallbackDBus     ( double              progress_zero_till_one OY_UNUSED,
                                       char              * status_text,
                                       int                 thread_id_ OY_UNUSED,
                                       int                 job_id OY_UNUSED,
                                       oyStruct_s        * cb_progress_context )
{
  const char * key;
  int verbose = oyOption_GetValueInt( (oyOption_s*)cb_progress_context, 0 );
  if(!status_text) return;

  oyGetPersistentStrings(NULL);

  key = strchr( status_text, '/' );
  if(key)
    ++key;
  else
    return;

  if(strstr(key,OY_STD) == NULL) return;

  fprintf(stdout, "%s ", oyjlPrintTime(OYJL_BRACKETS, oyjlGREEN) );
  getKey( key, oySCOPE_USER_SYS, verbose, 1/*print*/ );

  /* Clear the changed state, before a new check. */
  oy_dbus_config_changed = 1;
}

#endif /* HAVE_DBUS */

static oyjlOptionChoice_s * choicesFromStringList ( char ** list, int n )
{
  oyjlOptionChoice_s * c = NULL;
  int i;

  if(list && n > 0)
  {
    c = calloc(n+1, sizeof(oyjlOptionChoice_s));
    if(c)
    {
      for(i = 0; i < n; ++i)
      {
        c[i].nick = strdup( list[i] );
        c[i].name = strdup("");
        c[i].description = strdup("");
        c[i].help = strdup("");
      }
    }
  }

  return c;
}

#if 0
static oyjlOptionChoice_s * choicesFromCommand ( const char * command )
{
  oyjlOptionChoice_s * c = NULL;

  int size = 0, i,n = 0;
  char * result = oyjlReadCommandF( &size, "r", malloc, command );
  char ** list = oyjlStringSplit( result, '\n', &n, 0 );

  if(list)
  {
    c = choicesFromStringList( list, n );
    oyjlStringListRelease( &list, n, free );
  }

  return c;
}
static oyjlOptionChoice_s * listKeys ( oyjlOption_s * o OYJL_UNUSED, int * y OYJL_UNUSED, oyjlOptions_s * opts OYJL_UNUSED )
{
  return choicesFromCommand("oyranos-config -l");
}
#else
static char ** getDBPaths( oySCOPE_e scope, int * n )
{
  char * p = oyGetInstallPath( oyPATH_POLICY, scope, oyAllocateFunc_ );
  char ** paths = NULL;

    if(p)
    {
      size_t size = 0;
      int count = 0;
      char * db;
      STRING_ADD( p, "/openicc.json" );
      db = oyReadFileToMem_( p, &size, oyAllocateFunc_ );
      if(db)
      {
        oyjl_val root = oyJsonParse( db, NULL );

        paths = oyjlTreeToPaths( root, 1000000, NULL, OYJL_KEY, &count );
        *n = count;

        oyFree_m_(db);
      }
      oyFree_m_(p);
    }

  return paths;
}
static oyjlOptionChoice_s * listKeys ( oyjlOption_s * o OYJL_UNUSED, int * y OYJL_UNUSED, oyjlOptions_s * opts OYJL_UNUSED )
{
  int count = 0;
  char ** paths = getDBPaths( oySCOPE_USER, &count );
  oyjlOptionChoice_s * c = NULL;

  c = choicesFromStringList( paths, count );
  oyjlStringListRelease( &paths, count, free );

  return c;
}
char ** getDBVals( oySCOPE_e scope, int *n )
{
  int count = 0, i;
  char ** paths = getDBPaths( scope, &count );

  for(i = 0; i < count; ++i)
  {
    char * key = paths[i];
    char * v = oyGetPersistentString( key, 0, scope, malloc );
    oyjlStringAdd( &key, 0,0, ":%s", v );
    paths[i] = key;
  }
  *n = count;

  return paths;
}
static oyjlOptionChoice_s * listVals ( oyjlOption_s * o OYJL_UNUSED, int * y OYJL_UNUSED, oyjlOptions_s * opts OYJL_UNUSED )
{
  int count = 0;
  char ** paths = getDBVals( oySCOPE_USER, &count );
  oyjlOptionChoice_s * c = choicesFromStringList( paths, count );
  oyjlStringListRelease( &paths, count, free );

  return c;
}
#endif

/* This function is called the
 * * first time for GUI generation and then
 * * for executing the tool.
 */
int myMain( int argc , const char** argv )
{
  char * v;
  int count = 0;
  oySCOPE_e scope = oySCOPE_USER_SYS;

  /* the functional switches */
  int error = 0;
  int state = 0;
  int no_arg_var = 0;
  int path = 0;
  int list = 0;
  const char * get = 0;
  const char * set = 0;
  int dump_db = 0;
  int daemon = 0;
  int system_wide = 0;
  int syscolordir = 0;
  int usercolordir = 0;
  int iccdirname = 0;
  int settingsdirname = 0;
  int cmmdir = 0;
  int metadir = 0;
  int Version = 0;
  int api_version = 0;
  int num_version = 0;
  int git_version = 0;
  int cflags = 0;
  int ldflags = 0;
  int ldstaticflags = 0;
  int sourcedir = 0;
  int builddir = 0;
  const char * export = 0;
  const char * render = 0;
  const char * help = 0;
  int verbose = 0;
  int version = 0;


  /* nick, name, description, help */
  /* declare the option choices  *   nick,          name,               description,                  help */
  oyjlOptionChoice_s d_choices[] = {{"0",           _("Deactivate"),    _("Deactivate"),              ""},
                                    {"1",           _("Activate"),      _("Activate"),                ""},
                                    {NULL,NULL,NULL,NULL}};

  oyjlOptionChoice_s E_choices[] = {{"OY_DEBUG",    _("set the Oyranos debug level."),_("Alternatively the -v option can be used."),_("Valid integer range is from 1-20.")},
                                    {"OY_MODULE_PATH",_("route Oyranos to additional directories containing modules."),"",                        ""},
                                    {NULL,NULL,NULL,NULL}};

  oyjlOptionChoice_s A_choices[] = {{"",_("Show a settings value"),_("oyranos-config -g org/freedesktop/openicc/behaviour/effect_switch"),""},
                                    {"",_("Change a setting"),_("oyranos-config -s org/freedesktop/openicc/behaviour/effect_switch:1"),""},
                                    {"",_("Show all settings with values"),_("oyranos-config -l -v"),                        ""},
                                    {"",_("Watch events"),_("oyranos-config -d 1 -v > log-file.txt"),""},
                                    {"",_("Compile a simple programm"),_("cc `oyranos-config --cflags` myFile.c `oyranos-config --ldflags` -o myProg"),""},
                                    {"",_("Show system wide visible profiles from the Oyranos installation path"),_("ls `oyranos-config --syscolordir --iccdirname`"),""},
                                    {NULL,NULL,NULL,NULL}};

  oyjlOptionChoice_s S_choices[] = {{"oyranos-policy(1) oyranos-config-synnefo(1) oyranos(3)","",              "",                        ""},
                                    {"http://www.oyranos.org","",              "",                        ""},
                                    {NULL,NULL,NULL,NULL}};

  /* declare options - the core information; use previously declared choices */
  oyjlOption_s oarray[] = {
  /* type,   flags,                      o,  option,          key,      name,          description,                  help, value_name,         
        value_type,              values,             variable_type, variable_name */
    {"oiwi", 0,                          "#",NULL,            NULL,     _("Name"),     _("Project Name"),            NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&no_arg_var}},
    {"oiwi", 0,                          "p","path",          NULL,     _("Path"),     _("Show DB File"),            NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&path}},
    {"oiwi", 0,                          "l","list",          NULL,     _("Paths"),    _("List existing paths inside DB"),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&list}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "g","get",           NULL,     _("Get"),      _("Get a Value"),             NULL, _("XPATH"),         
        oyjlOPTIONTYPE_FUNCTION, {.getChoices=listKeys}, oyjlSTRING,{.s=&get}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "s","set",           NULL,     _("Set"),      _("Set a Value"),             NULL, _("XPATH:VALUE"),   
        oyjlOPTIONTYPE_FUNCTION, {.getChoices=listVals}, oyjlSTRING,{.s=&set}},
    {"oiwi", 0,                          NULL,"dump-db",      NULL,     _("Dump DB"),  _("Dump OpenICC DB"),         NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&dump_db}},
    {"oiwi", 0,                          "d","daemon",        NULL,     _("Daemon"),   _("Watch DB changes"),        NULL, _("0|1"),           
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)d_choices, sizeof(d_choices), malloc ), 0}}, oyjlINT,       {.i=&daemon}},
    {"oiwi", 0,                          "z","system-wide",   NULL,     _("System Wide"),_("System wide DB setting"),  NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&system_wide}},
    {"oiwi", 0,                          NULL,"syscolordir",   NULL,    "syscolordir", _("Path to system main color directory"),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&syscolordir}},
    {"oiwi", 0,                          NULL,"usercolordir",  NULL,    "usercolordir",_("Path to users main color directory"),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&usercolordir}},
    {"oiwi", 0,                          NULL,"iccdirname",    NULL,    "iccdirname",  _("ICC profile directory name"),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&iccdirname}},
    {"oiwi", 0,                          NULL,"settingsdirname",NULL,   "settingsdirname",_("Oyranos settings directory name"),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&settingsdirname}},
    {"oiwi", 0,                          NULL,"cmmdir",        NULL,    "cmmdir",      _("Oyranos CMM directory name"),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&cmmdir}},
    {"oiwi", 0,                          NULL,"metadir",       NULL,    "metadir",     _("Oyranos meta module directory name"),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&metadir}},
    {"oiwi", 0,                          NULL,"Version",       NULL,     _("Version"), _("Show official version"),   _("API|ABI-Feature-Patch|BugFix Release"), NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&Version}},
    {"oiwi", 0,                          NULL,"api-version",   NULL,    "api-version", _("Show version of API"),     NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&api_version}},
    {"oiwi", 0,                          NULL,"num-version",   NULL,    "num-version", _("Show version as a simple number"),_("10000*API+100*Feature+Patch"), NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&num_version}},
    {"oiwi", 0,                          NULL,"git-version",   NULL,    "git-version", _("Show version as in git"),  _("lastReleaseVersion-gitCommitNumber-gitCommitSHA1ID-Year-month-day"), NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&git_version}},
    {"oiwi", 0,                          NULL,"cflags",        NULL,    "cflags",      _("compiler flags"),          NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&cflags}},
    {"oiwi", 0,                          NULL,"ldflags",       NULL,    "ldflags",     _("dynamic link flags"),      NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&ldflags}},
    {"oiwi", 0,                          NULL,"ldstaticflags", NULL,    "ldstaticflags",_("static linking flags"),    NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&ldstaticflags}},
    {"oiwi", 0,                          NULL,"sourcedir",     NULL,    "sourcedir",   _("Oyranos local source directory name"),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&sourcedir}},
    {"oiwi", 0,                          NULL,"builddir",      NULL,    "builddir",    _("Oyranos local build directory name"),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&builddir}},
    {"oiwi", 0,                          "E","man-environment_variables",NULL,     "",         "",                        NULL, NULL,               
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)E_choices, sizeof(E_choices), malloc ), 0}}, oyjlNONE,      {}},
    {"oiwi", 0,                          "A","man-examples",  NULL,     "",         "",                        NULL, NULL,               
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)A_choices, sizeof(A_choices), malloc ), 0}}, oyjlNONE,      {}},
    {"oiwi", 0,                          "S","man-see_as_well",NULL,     "",         "",                        NULL, NULL,               
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)S_choices, sizeof(S_choices), malloc ), 0}}, oyjlNONE,      {}},
    /* default option template -X|--export */
    {"oiwi", 0, "X", "export", NULL, NULL, NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices.list = NULL}, oyjlSTRING, {.s=&export} },
    /* The --render option can be hidden and used only internally. */
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE, "R", "render", NULL, NULL,  NULL,  NULL, NULL, oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s=&render} },
    {"oiwi", OYJL_OPTION_FLAG_ACCEPT_NO_ARG, "h", "help", NULL, NULL, NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s=&help} },
    {"oiwi", 0, NULL,"synopsis",NULL, NULL,         NULL,         NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlNONE, {0} },
    {"oiwi", 0, "v", "verbose", NULL, _("verbose"), _("verbose"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&verbose} },
    {"oiwi", 0, "V", "version", NULL, _("version"), _("Version"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&version} },
    {"",0,0,NULL,NULL,NULL,NULL,NULL, NULL, oyjlOPTIONTYPE_END, {0},0,{0}}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups[] = {
  /* type,   flags, name,               description,                  help,               mandatory,     optional,      detail */
    {"oiwg", 0,     _("Settings"),      _("Persistent Settings"),     _("Handle OpenICC DB configuration on low level."),"g|s|l|dump-db|p","v,z","g,s,l,dump-db,p"},
#ifdef HAVE_DBUS
    {"oiwg", 0,     _("Watch"),         _("Observe config changes"),  _("Will only work on command line."),"d",           "v",           "d" },
#endif
    {"oiwg", 0,     _("Install Paths"), _("Show Install Paths"),      _("Show statically configured and compiled in paths of Oyranos CMS."),               "syscolordir|usercolordir|iccdirname|settingsdirname|cmmdir|metadir","v,z",         "syscolordir|usercolordir|iccdirname|settingsdirname|cmmdir|metadir"},
    {"oiwg", 0,     _("Version"),       _("Show Version"),            _("Release Version follow of a Major(API|ABI)-Minor(Feature)-Micro(Patch|Bug Fix) scheme. For orientation in git the last release, commit number, SHA1 ID and Year-month-day parts are available."), "Version|api-version|num-version|git-version","v",           "Version|api-version|num-version|git-version"},
    {"oiwg", 0,     _("Options"),       _("Miscellaneous options"),   _("These strings can be used to compile programs."),"#|cflags|ldflags|ldstaticflags|sourcedir|builddir","v",           "#|cflags|ldflags|ldstaticflags|sourcedir|builddir"},
    {"oiwg", OYJL_GROUP_FLAG_GENERAL_OPTS, _("Misc"), _("General options"), NULL,         "X|h|V|R",     "v",           "h,X,R,V,z,v"},
    {"",0,0,0,0,0,0,0}
  };

  oyjlUiHeaderSection_s * sections = oyUiInfo(_("The tool can read and set OpenICC DB options, and display paths and static information."),
                  "2020-09-23T12:00:00", "September 23, 2020");
  oyjlUi_s * ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oyranos-config", _("Config"), _("Oyranos Config tool"),
#ifdef __ANDROID__
                                       ":/images/logo.svg", // use qrc
#else
                                       "oyranos_logo",
#endif
                                       sections, oarray, groups, &state );
  if( state & oyjlUI_STATE_EXPORT &&
      export &&
      strcmp(export,"json+command") != 0)
    goto clean_main;
  if(state & oyjlUI_STATE_HELP)
  {
    fprintf( stderr, "%s\n\tman oyranos-config\n\n", _("For more information read the man page:") );
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

  const char * jcommands = "{\n\
  \"command_set\": \"oyranos-config\",\n\
  \"comment\": \"command_set_delimiter - build key:value; default is '=' key=value\",\n\
  \"comment\": \"command_set_option - use \\\"-s\\\" \\\"key\\\"; skip \\\"--\\\" direct in front of key\",\n\
  \"command_get\": \"oyranos-config\",\n\
  \"command_get_args\": [\"-X\",\"json+command\"]\n\
}";
  if(ui && (export && strcmp(export,"json+command") == 0))
  {
    char * json = oyjlUi_ToJson( ui, 0 ),
         * json_commands = strdup( jcommands );
    json_commands[strlen(json_commands)-2] = ',';
    json_commands[strlen(json_commands)-1] = '\000';
    oyjlStringAdd( &json_commands, malloc, free, "%s", &json[1] ); /* skip opening '{' */
    puts( json_commands );
    free( json_commands );
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


  if(oy_debug)
    fprintf(stderr, " %.06g %s\n", DBG_UHR_, oyPrintTime() );

  if(system_wide)
    scope = oySCOPE_SYSTEM;

  if(oy_debug)
    fprintf( stderr, "  Oyranos v%s\n",
                  oyNoEmptyName_m_(oyVersionString(1)));



  if(verbose > 1)
    oy_debug += verbose-1;

  if(getenv(OY_DEBUG))
  {
    int value = atoi(getenv(OY_DEBUG));
    if(value > 0)
      oy_debug += value;
  }

  if(no_arg_var)
  {
    fprintf( stdout, "oyranos" );
    goto clean_main;
  }

  if(oy_debug)
    fprintf( stderr, "  Oyranos v%s\n",
                  oyNoEmptyName_m_(oyVersionString(1)));


  int dir = 0;
  if(syscolordir)
  { printf( OY_SYSCOLORDIR ); dir = 1; }
  else if(usercolordir)
  { printf( OY_USERCOLORDIR ); dir = 1; }
  if( dir &&
      (iccdirname || settingsdirname || cmmdir || metadir) )
    printf("/");
  if(iccdirname)
  { puts( OY_ICCDIRNAME ); dir = 1; }
  else if(settingsdirname)
  { puts( OY_SETTINGSDIRNAME ); dir = 1; }
  else if(cmmdir)
  { puts( OY_CMMDIR ); dir = 1; }
  else if(metadir)
  { puts( OY_LIBDIR OY_SLASH OY_METASUBPATH ); dir = 1; }
  if(dir)
  {
    puts("\n");
    return 0;
  }

  if(Version)
  { fprintf( stdout, "%s\n", oyVersionString(1) ); return 0; }
  else if(api_version)
  { fprintf( stdout, "%d\n", OYRANOS_VERSION_A ); return 0; }
  else if(num_version)
  { fprintf( stdout, "%d\n", OYRANOS_VERSION ); return 0; }
  else if(git_version)
  { fprintf( stdout, "%s\n", OY_GIT_VERSION ); return 0; }

  if(cflags)
  { system( "pkg-config --cflags oyranos" ); }
  if(ldflags)
  { system( "pkg-config --libs oyranos" ); }
  if(ldstaticflags)
  { puts( "-L" OY_LIBDIR " -loyranos-static" ); }
  if(sourcedir)
  { puts( OY_SOURCEDIR "\n" ); return 0; }
  if(builddir)
  { puts( OY_BUILDDIR "\n" ); return 0; }

  if(path)
  {
    if(scope == oySCOPE_USER_SYS) scope = oySCOPE_USER;
    v = oyGetInstallPath( oyPATH_POLICY, scope, oyAllocateFunc_ );
    if(v)
      STRING_ADD( v, "/openicc.json" );
    fprintf( stdout, "%s\n", oyNoEmptyString_m_(v) );
    if(v) oyFree_m_(v);
  }

  if(dump_db)
  {
    if(scope == oySCOPE_USER_SYS) scope = oySCOPE_USER;
    v = oyGetInstallPath( oyPATH_POLICY, scope, oyAllocateFunc_ );
    if(v)
    {
      size_t size = 0;
      int count = 0;
      char * db;
      oyjl_val root;

      STRING_ADD( v, "/openicc.json" );
      db = oyReadFileToMem_( v, &size, oyAllocateFunc_ );
      root = oyJsonParse( db, NULL );
      if(root)
      {
        char * json = NULL;
        int level = 0;
        oyjlTreeToJson( root, &level, &json );
        if(json)
          puts(json);
        oyFree_m_(json);
        oyjlTreeFree( root ); root = NULL;
      }
      else
      if(db)
      {
        puts(db);
        oyFree_m_(db);
      }
    }
    if(v) oyFree_m_(v);
  }

  if(list)
  {
    int count = 0, i;
    char ** paths;

    if(scope == oySCOPE_USER_SYS) scope = oySCOPE_USER;
    if(verbose)
      paths = getDBVals( scope, &count );
    else
      paths = getDBPaths( scope, &count );

    for(i = 0; i < count; ++i)
      fprintf(stdout,"%s\n", paths[i]);

    oyjlStringListRelease( &paths, count, free );
  }

  if(get)
    getKey( get, scope, verbose, 1 );

  if(set)
  {
    char ** list = oyStringSplit( set, ':', &count, oyAllocateFunc_ );

    if(count == 2)
    {
      const char * key_name = list[0];
      const char * value = list[1],
                 * comment = NULL;
      if(scope == oySCOPE_USER_SYS) scope = oySCOPE_USER;
      if(verbose) getKey( key_name, scope, verbose, 0 );
      error = oySetPersistentString( key_name, scope, value, comment );
      if(verbose)
        fprintf( stderr, "%s->%s\n", oyNoEmptyString_m_(list[0]), oyNoEmptyString_m_(value) );
      if(verbose) getKey( key_name, scope, verbose, 0 );

      oyStringListRelease( &list, count, oyDeAllocateFunc_ );

      {
        /* ping X11 observers about option change
         * ... by setting a known property again to its old value
         */
        oyOptions_s * opts = oyOptions_New(NULL), * results = 0;
        error = oyOptions_Handle( "//" OY_TYPE_STD "/send_native_update_event",
                      opts,"send_native_update_event",
                      &results );
        oyOptions_Release( &opts );
      }

    if(error)
      printf("send_native_update_event failed\n");
    }
    else if(count == 1)
    {
      const char * key_name = list[0];
      const char * value = NULL,
                 * comment = NULL;
      if(scope == oySCOPE_USER_SYS) scope = oySCOPE_USER;
      if(verbose) getKey( key_name, scope, verbose, 0 );
      error = oySetPersistentString( key_name, scope, value, comment );
      if(verbose)
        fprintf( stderr, "erase %s\n", oyNoEmptyString_m_(key_name) );
      if(verbose) getKey( key_name, scope, verbose, 0 );

      oyStringListRelease( &list, count, oyDeAllocateFunc_ );

      {
        /* ping X11 observers about option change
         * ... by setting a known property again to its old value
         */
        oyOptions_s * opts = oyOptions_New(NULL), * results = 0;
        error = oyOptions_Handle( "//" OY_TYPE_STD "/send_native_update_event",
                      opts,"send_native_update_event",
                      &results );
        oyOptions_Release( &opts );
      }

      if(error)
        printf("send_native_update_event failed\n");
    }
    else
    {
      oyjlOptions_PrintHelp( ui->opts, ui, verbose, NULL );
      goto clean_main;
    }
  }

#ifdef HAVE_DBUS
  if(daemon == 1)
  {
    int id;
    double hour_old = 0.0;
    oyOption_s * cb_option = oyOption_FromRegistration(OY_STD "/verbose",0);;
    oyOption_SetFromInt( cb_option, verbose+1, 0, 0 );

    oyStartDBusObserver( oyWatchDBus, oyFinishDBus, oyConfigCallbackDBus, OY_STD, cb_option )

    if(id && verbose)
      fprintf(stderr, "oyStartDBusObserver ID: %d\n", id);

    while(1)
    {
      double hour = oySeconds( ) / 3600.;
      double repeat_check = 1.0/60.0; /* every minute */

      oyLoopDBusObserver( hour, repeat_check, oy_dbus_config_changed, 0 )

      /* delay next polling */
      oySleep( 0.25 );
    }
  }
#endif /* HAVE_DBUS */

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

int main( int argc_, char ** argv_)
{
  int argc = argc_;
  char ** argv = argv_;

#ifdef __ANDROID__
  argv = calloc( argc + 2, sizeof(char*) );
  memcpy( argv, argv_, (argc + 2) * sizeof(char*) );
  argv[argc++] = "--render=gui"; /* start QML */
#endif

  /* language needs to be initialised before setup of data structures */
#ifdef OYJL_USE_GETTEXT
#ifdef OYJL_HAVE_LOCALE_H
  setlocale(LC_ALL,"");
#endif
#endif

  oyExportStart_(EXPORT_CHECK_NO);

  myMain(argc, (const char **)argv);

#ifdef __ANDROID__
  free( argv );
#endif
  oyFinish_( FINISH_IGNORE_I18N | FINISH_IGNORE_CACHES );

  return 0;
}

void  getKey                         ( const char        * key,
                                       oySCOPE_e           scope,
                                       int                 verbose,
                                       int                 print )
{
  if(key)
  {
    char * v = oyGetPersistentString( key, 0, scope, oyAllocateFunc_ );
    int hour, minute, second, gmt_diff_second;

    oyGetCurrentGMTHour( &gmt_diff_second );
    oySplitHour( oyGetCurrentLocalHour( oyGetCurrentGMTHour(0), gmt_diff_second ), &hour, &minute, &second );
    if(verbose > 1) fprintf( print == 1 ? stdout : stderr, "%02d:%02d:%02d ", hour, minute, second );
    if(verbose)     fprintf( print == 1 ? stdout : stderr, "%s:", key );
    fprintf( print ? stdout : stderr, "%s\n", oyNoEmptyString_m_(v) );
    if(v) oyFree_m_(v);
  }
}



