/** @file oyranos_profiles.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2010-2022 (C) Kai-Uwe Behrmann
 *
 *  @brief    ICC profile informations - on the command line
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2010/11/11
 *
 *  The program informs about installed ICC profiles.
 */

#include "oyProfiles_s.h"

#include "oyranos.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_helper_macros.h"
#include "oyranos_internal.h"
#include "oyranos_icc.h"
#include "oyranos_io.h"
#include "oyranos_config.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyranos_version.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// 574 Zeilen

int    installProfile                ( oyProfile_s       * ip,
                                       const char        * path,
                                       int                 is_device_profile,
                                       int                 test,
                                       char              * show_text,
                                       int                 show_gui );
void* oyAllocFunc(size_t size) {return malloc (size);}

const char * jcommands = "{\n\
  \"command_set\": \"oyranos-profiles\",\n\
  \"comment\": \"command_set_delimiter - build key:value; default is '=' key=value\",\n\
  \"comment\": \"command_set_option - use \\\"-s\\\" \\\"key\\\"; skip \\\"--\\\" direct in front of key\"\n\
}";

/* This function is called the
 * * first time for GUI generation and then
 * * for executing the tool.
 */
int myMain( int argc, const char ** argv )
{
  int error = 0;
  int list_profiles = 0, 
      list_profile_full_names = 0, list_profile_internal_names = 0,
      list_paths = 0, user_path = 0, oyranos_path = 0, system_path = 0, machine_path = 0,
      v2 = 0, v4 = 0, no_repair = 0, duplicates = 0;
  int color_space = 0,
      display = 0,
      input = 0,
      output = 0,
      abstract = 0,
      named_color = 0,
      device_link = 0;
  const char * install = 0;
  int is_device_profile = 0;
  int show_gui = 0;
  const char * taxi_id = NULL,
             * path = NULL,
             * meta = NULL;
  int test = 0;

  oyjlOptions_s * opts;
  oyjlOption_s * o;
  oyjlUi_s * ui;
  oyjlUiHeaderSection_s * info;
  const char * export = NULL;
  const char * render = NULL;
  int help = 0;
  int verbose = 0;
  int version = 0;
  int state = 0;

  opts = oyjlOptions_New( argc, (const char**)argv );
  /* nick, name, description, help */
  oyjlOptionChoice_s env_vars[]={{"OY_DEBUG", _("set the Oyranos debug level."), _("Alternatively the -v option can be used."), _("Valid integer range is from 1-20.")},
                                    {"XDG_DATA_HOME XDG_DATA_DIRS", _("route Oyranos to top directories containing resources. The derived paths for ICC profiles have a \"color/icc\" appended. http://www.openicc.org/index.php%3Ftitle=OpenIccDirectoryProposal.html"), "", ""},
                                    {NULL,NULL,NULL,NULL}};
  oyjlOptionChoice_s examples[]={{"",_("List all installed profiles by internal name"), "oyranos-profiles -le", ""},
                                    {"",_("List all installed profiles of the display and output device classes"), "oyranos-profiles -l -od", ""},
                                    {"",_("List all installed profiles in user path"), "oyranos-profiles -lfu", ""},
                                    {"",_("Install a profile for the actual user and show error messages in a GUI"), "oyranos-profiles --install profilename -u --gui", ""},
                                    {"",_("Install a profile for the actual user and show error messages in a GUI"), "oyranos-profiles --install --taxi=taxi_id/0 --gui -d -u", ""},
                                    {"",_("Show file infos"),"SAVEIFS=$IFS ; IFS=$'\\n\\b'; profiles=(`oyranos-profiles -ldf`); IFS=$SAVEIFS; for file in \"${profiles[@]}\"; do ls \"$file\"; done", ""},
                                    {NULL,NULL,NULL,NULL}};
  oyjlOptionChoice_s see_as_well[]={{"oyranos-profile-graph(1) oyranos-config(1) oyranos-policy(1) oyranos(3)", "", "", ""},
                                    {"http://www.oyranos.org","","",""},
                                    {NULL,NULL,NULL,NULL}};
  oyjlOptionChoice_s path_choices[]={{"basICColor","","",""}, {"colord","","",""}, {"edid","","",""}, {"OpenICC","","",""}, {"oyra","","",""}, {"xorg","","",""},
                                    {NULL,NULL,NULL,NULL}};
  oyjlOptionChoice_s effect_meta[]={{"EFFECT_class:sepia","","",""},{"EFFECT_class:bw","","",""},{NULL,NULL,NULL,NULL}};
  /* declare options - the core information; use previously declared choices */
  oyjlOption_s oarray[] = {
  /* type,   flags, o, option, key, name, description, help, value_name, value_type, values, var_type, variable */
    {"oiwi", 0, "2", "icc-version-2", NULL, _("ICC Version 2"), _("Select ICC v2 Profiles"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&v2},NULL},
    {"oiwi", 0, "4", "icc-version-4", NULL, _("ICC Version 4"), _("Select ICC v4 Profiles"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&v4},NULL},
    {"oiwi", 0, "l", "list-profiles", NULL, _("List Profiles"), _("List Profiles"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT,{.i=&list_profiles},NULL},
    {"oiwi", 0, "f", "full-names", NULL, _("Full Names"), _("List profile full names"), _("Show path name and file name."), NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&list_profile_full_names},NULL},
    {"oiwi", 0, "e", "internal-names", NULL, _("Internal Name"), _("List profile internal names"), _("The text string comes from the 'desc' tag."), NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&list_profile_internal_names},NULL},
    {"oiwi", 0, "c", "color-space", NULL, _("Color Space Class"), _("Select Color Space profiles"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&color_space},NULL},
    {"oiwi", 0, "d", "display", NULL, _("Display Class"), _("Select Monitor profiles"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&display},NULL},
    {"oiwi", 0, "i", "input", NULL, _("Input Class"), _("Select Input profiles"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&input},NULL},
    {"oiwi", 0, "o", "output", NULL, _("Output Class"), _("Select Output profiles"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&output},NULL},
    {"oiwi", 0, "a", "abstract", NULL, _("Abstract Class"), _("Select Abstract profiles"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&abstract},NULL},
    {"oiwi", 0, "k", "device-link", NULL, _("(Device) Link Class"), _("Select Device Link profiles"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&device_link},NULL},
    {"oiwi", 0, "n", "named-color", NULL, _("Named Color Class"), _("Select Named Color profiles"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&named_color},NULL},
    {"oiwi", 0, "p", "list-paths", NULL, _("List Paths"), _("List ICC Profile Paths"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&list_paths},NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE, "I", "install", NULL, _("Install"), _("Install Profile"), NULL, _("ICC_PROFILE"), oyjlOPTIONTYPE_CHOICE, {}, oyjlSTRING, {.s=&install},NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE, "t", "taxi", NULL, _("Taxi DB"), _("ICC Taxi Profile DataBase"), NULL, _("TAXI_ID"), oyjlOPTIONTYPE_CHOICE, {}, oyjlSTRING, {.s=&taxi_id},NULL},
    {"oiwi", 0, "g", "gui", NULL, _("GUI"), _("Use Graphical User Interface"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&show_gui},NULL},
    {"oiwi", 0, "u", "user", NULL, _("User"), _("User path"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&user_path},NULL},
    {"oiwi", 0, "y", "oyranos", NULL, _("Oyranos"), _("Oyranos path"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&oyranos_path},NULL},
    {"oiwi", 0, "s", "system", NULL, _("System"), _("System path"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&system_path},NULL},
    {"oiwi", 0, "m", "machine", NULL, _("Machine"), _("Machine path"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&machine_path},NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE, "P", "path", NULL, _("Path filter"), _("Show profiles containing a string as part of their full name"), NULL, _("PATH_SUB_STRING"), oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*)oyjlStringAppendN( NULL, (const char*)path_choices, sizeof(path_choices), 0 )}, oyjlSTRING, {.s=&path},NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE, "T", "meta", NULL, _("Meta"), _("Filter for meta tag key/value pair"), _("Show profiles containing a certain key/value pair of their meta tag. VALUE can contain '*' to allow for substring matching."), _("KEY:VALUE"), oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*)oyjlStringAppendN( NULL, (const char*)effect_meta, sizeof(effect_meta), 0 )}, oyjlSTRING, {.s=&meta},NULL},
    {"oiwi", 0, "r", "no-repair", NULL, _("No repair"), _("No Profile repair of ICC profile ID"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&no_repair},NULL},
    {"oiwi", 0, "D", "duplicates", NULL, _("Duplicates"), _("Show identical multiple installed profiles"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&duplicates},NULL},
    {"oiwi", OYJL_OPTION_FLAG_IMMEDIATE, NULL,"test", NULL, _("Test"), _("No Action"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&test},NULL},

    /* default options -h and -v */
    {"oiwi", OYJL_OPTION_FLAG_ACCEPT_NO_ARG, "h", "help",NULL,NULL,NULL,NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&help},NULL},
    {"oiwi", 0, NULL,"synopsis",NULL, NULL,         NULL,         NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlNONE, {0},NULL},
    {"oiwi", 0, "v", "verbose", NULL, _("verbose"), _("verbose"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&verbose},NULL},
    {"oiwi", 0, "V", "version", NULL, _("version"), _("Version"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&version},NULL},
    /* default option template -X|--export */
    {"oiwi", 0, "X", "export", NULL, NULL, NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices.list = NULL}, oyjlSTRING, {.s=&export},NULL},
    /* The --render option can be hidden and used only internally. */
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE|OYJL_OPTION_FLAG_MAINTENANCE, "R", "render", NULL, NULL,  NULL,  NULL, NULL, oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s=&render},NULL},
    /* blind options, useful only for man page generation */
    {"oiwi", 0, "E", "man-environment_variables", NULL, "", "", NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*)oyjlStringAppendN( NULL, (const char*)env_vars, sizeof(env_vars), 0 )}, oyjlNONE, {.i=NULL},NULL},
    {"oiwi", 0, "A", "man-examples", NULL, "", "", NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*)oyjlStringAppendN( NULL, (const char*)examples, sizeof(examples), 0 )}, oyjlNONE, {.i=NULL},NULL},
    {"oiwi", 0, "S", "man-see_as_well", NULL, "", "", NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*)oyjlStringAppendN( NULL, (const char*)see_as_well, sizeof(see_as_well), 0 )}, oyjlNONE, {.i=NULL},NULL},
    {"",0,0,0,0,0,0,0, NULL, oyjlOPTIONTYPE_END, {},0,{},0}
  };
  opts->array = (oyjlOption_s*)oyjlStringAppendN( NULL, (const char*)oarray, sizeof(oarray), 0 );

  oyjlOptionGroup_s groups[] = {
  /* type,   flags, name,               description,                  help,               mandatory,     optional,      detail */
    {"oiwg", 0,     _("List"),          _("List of available ICC color profiles"), NULL,  "l",           "f,e,a,c,d,k,n,o,i,2,4,P,T,v", "l,f,e,a,c,d,k,n,o,i,2,4,P,T,D", NULL},
    {"oiwg", 0,     _("Paths"),         _("List search paths"),       NULL,               "p",           "u|s|y|m,v",   "p,u,s,y,m",NULL},
    {"oiwg", OYJL_GROUP_FLAG_EXPLICITE,_("Install"),_("Install Profile"), NULL,           "I|t",         "u|s|y|m,g,v", "I,t,u,s,y,m,g,test",NULL},
    {"oiwg", OYJL_GROUP_FLAG_GENERAL_OPTS, _("Misc"), _("General options"), NULL,         "h|X|V",       "",            "h,X,V,R,v",NULL},
    {"",0,0,0,0,0,0,0,0}
  };
  opts->groups = (oyjlOptionGroup_s*)oyjlStringAppendN( NULL, (const char*)groups, sizeof(groups), 0);

  info = oyUiInfo(_("The tool can list installed profiles, search paths and can help install a ICC color profile in a search path."),
                  "2018-10-11T12:00:00", "October 11, 2018");
  ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oyranos-profiles", _("Oyranos Profiles"), _("The Tool gives information around installed ICC color profiles."),
#ifdef __ANDROID__
                                       ":/images/logo.svg", // use qrc
#else
                                       "oyranos_logo",
#endif
                                       info, opts->array, opts->groups, &state );
  if( state & oyjlUI_STATE_EXPORT &&
      export &&
      strcmp(export,"json+command") != 0)
    goto clean_main;
  if(state & oyjlUI_STATE_HELP)
  {
    fprintf( stderr, "%s\n\tman oyranos-profiles\n\n", _("For more information read the man page:"));
    goto clean_main;
  }

  {
    int n = 0,i;
    char ** path_names =  oyProfilePathsGet_( &n, oyAllocateFunc_ );
    char * text = NULL;
    o = oyjlOptions_GetOptionL( opts, "list-paths", 0 );
    for(i = 0; i < n; ++i)
      oyjlStringAdd( &text, malloc, free, "%s%s", i?"\n":"",path_names[i] );
    o->help = text;
  }
  o = oyjlOptions_GetOptionL( opts, "user", 0 );
  o->help = oyGetInstallPath( oyPATH_ICC, oySCOPE_USER, malloc );
  o = oyjlOptions_GetOptionL( opts, "system", 0 );
  o->help = oyGetInstallPath( oyPATH_ICC, oySCOPE_SYSTEM, malloc );
  o = oyjlOptions_GetOptionL( opts, "oyranos", 0 );
  o->help = oyGetInstallPath( oyPATH_ICC, oySCOPE_OYRANOS, malloc );
  o = oyjlOptions_GetOptionL( opts, "machine", 0 );
  o->help = oyGetInstallPath( oyPATH_ICC, oySCOPE_MACHINE, malloc );

  if(verbose > 1)
    oy_debug += verbose -1;

  if(verbose)
  {
    char * json = oyjlOptions_ResultsToJson( opts, 0 );
    if(json)
      fputs( json, stderr );
    fputs( "\n", stderr );

    char * text = oyjlOptions_ResultsToText( opts );
    if(text)
      fputs( text, stderr );
    fputs( "\n", stderr );
  }

  if((export && strcmp(export,"json+command") == 0))
  {
    char * json = oyjlUi_ToJson( ui, 0 ),
         * json_commands = strdup(jcommands);
    json_commands[strlen(json_commands)-2] = ',';
    json_commands[strlen(json_commands)-1] = '\000';
    oyjlStringAdd( &json_commands, malloc, free, "%s", &json[1] );
    puts( json_commands );
    return 0;
  }


  if(display)
    is_device_profile = 1;
  if(verbose > 1)
    oy_debug += verbose -1;

  if(oy_debug)
    fprintf( stderr, "  Oyranos v%s\n",
                  oyNoEmptyName_m_(oyVersionString(1)));
  /* check the default paths */
  /*oyPathAdd( OY_PROFILE_PATH_USER_DEFAULT );*/


#define flags (v2?OY_ICC_VERSION_2:0 | v4?OY_ICC_VERSION_4:0 | no_repair?OY_NO_REPAIR:0 | duplicates?OY_ALLOW_DUPLICATES:0)

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
  } else if(ui &&
    (list_profiles || list_paths || install || taxi_id))
  {
    oyProfile_s * p = 0;
    oyProfiles_s * ps = 0;
    char ** names = NULL;
    uint32_t count = 0, i;
    int accept;
    const char * t = 0;

    if(user_path || oyranos_path || system_path || machine_path)
    {
        if(user_path)
          path = oyGetInstallPath( oyPATH_ICC, oySCOPE_USER, oyAllocateFunc_ );
        if(system_path)
          path = oyGetInstallPath( oyPATH_ICC, oySCOPE_SYSTEM, oyAllocateFunc_ );
        if(oyranos_path)
          path = oyGetInstallPath( oyPATH_ICC, oySCOPE_OYRANOS, oyAllocateFunc_ );
        if(machine_path)
          path = oyGetInstallPath( oyPATH_ICC, oySCOPE_MACHINE, oyAllocateFunc_ );

      if(oy_debug)
        fprintf( stderr, "%s: %s\n", _("Search path"), path );
    }
    if(list_profiles)
    {
      fprintf(stderr, "%s:\n", _("ICC profiles"));

      if(!(list_profile_full_names || list_profile_internal_names ||
           color_space || input || display || output || abstract ||
           named_color || device_link || v2 ||
           v4 || meta != NULL))
      {
      names = oyProfileListGet_ ( NULL, flags, &count );
      for(i = 0; i < count; ++i)
        {
          const char * sfn = names[i];
          if(path && strstr(sfn, path) == NULL)
            continue;
          if(strrchr(sfn, OY_SLASH_C))
            sfn = strrchr(sfn, OY_SLASH_C) + 1;
          fprintf(stdout, "%s\n", sfn);
        }
      } else
      {
        oyProfiles_s * patterns = NULL;
        if(meta)
        {
          oyProfile_s * pattern;
          char * t = NULL,
               * tmp = oyjlStringCopy( meta, 0 );
          oyjlStringReplace( &tmp, ":", ";", 0,0 );
          oyStringAddPrintf( &t, oyAllocateFunc_,oyDeAllocateFunc_, "meta:%s", tmp );
          free(tmp);
          pattern = oyProfile_FromFile( t, OY_NO_LOAD, NULL );
          if(!pattern)
          {
            fprintf(stderr, "%s %s\n", _("wrong argument to option:"), meta);
            FILE * f = NULL;
            char * t = oyjlOptions_PrintHelp( ui->opts, ui, verbose, &f, NULL );
            fputs( t, f ); free(t);
            if(!getenv("OYJL_NO_EXIT")) exit(1);
          }
          patterns = oyProfiles_New(0);
          oyProfiles_MoveIn( patterns, &pattern, -1 );
        }

        ps = oyProfiles_Create( patterns, flags, 0 );
        oyProfiles_Release( &patterns );
        count = oyProfiles_Count(ps);
        for(i = 0; i < count; ++i)
        {
          icSignature sig_class = 0;

          accept = 1;
          p = 0;
          t = 0;

          if(list_profile_full_names || list_profile_internal_names ||
             v2 || v4 || meta)
          {
            p = oyProfiles_Get( ps, i );
          }

          if( color_space || input || display || output || abstract ||
              named_color || device_link)
          {
            accept = 0;
            if(!p)
              p = oyProfiles_Get( ps, i );
            sig_class = oyProfile_GetSignature( p, oySIGNATURE_CLASS );
          }

          if(!accept)
          {
            if(color_space && sig_class == icSigColorSpaceClass)
              accept = 1;
            else if(input && sig_class == icSigInputClass)
              accept = 1;
            else if(display && sig_class == icSigDisplayClass)
              accept = 1;
            else if(output && sig_class == icSigOutputClass)
              accept = 1;
            else if(abstract && sig_class == icSigAbstractClass)
              accept = 1;
            else if(device_link && sig_class == icSigLinkClass)
              accept = 1;
            else if(named_color && sig_class == icSigNamedColorClass)
              accept = 1;
          }

          if( accept && (v2 || v4) )
          {
            icSignature vs = oyValueUInt32( oyProfile_GetSignature(p,oySIGNATURE_VERSION) );      
            char * v = (char*)&vs;
            if(!((v2 && (int)v[0] == 2) ||
                 (v4 && (int)v[0] == 4)))
              accept = 0;
          }

          if(path)
          {
            const char * sfn = oyProfile_GetFileName(p, -1);
            if(strstr(sfn, path) == NULL)
            accept = 0;
          }

          if(!list_profile_full_names && !list_profile_internal_names &&
             accept)
          {
            const char * sfn = oyProfile_GetFileName(p, -1);
            if(strrchr(sfn, OY_SLASH_C))
              sfn = strrchr(sfn, OY_SLASH_C) + 1;
            fprintf(stdout, "%s", sfn);
          }

          if(list_profile_internal_names && accept)
          {
            t = oyProfile_GetText(p, oyNAME_DESCRIPTION);
            if(t)
              fprintf(stdout, "%s", t);
          }

          if(list_profile_full_names && accept)
          {
            if(list_profile_internal_names)
              fprintf(stdout, " (");
            t = oyProfile_GetFileName(p, -1);
            if(t)
              fprintf(stdout, "%s", t);
            if(list_profile_internal_names)
              fprintf(stdout, ")");
          }

          if(accept)
            fprintf(stdout, "\n");

          oyProfile_Release( &p );
        }
        oyProfiles_Release( &ps );
      }
    }

    if(list_paths)
    {
      int n = 0;
      char ** path_names =  oyProfilePathsGet_( &n, oyAllocateFunc_ );
      fprintf(stderr, "%s:\n", _("ICC profile search paths"));
      if(path)
      {
        char * pn = oyResolveDirFileName_(path);
        if(pn)
        {
          fprintf(stdout, "%s\n", pn );
          free(pn);
        }
      } else
        for(i = 0; (int)i < n; ++i)
          fprintf(stdout, "%s\n", path_names[i]);

      oyStringListRelease_(&path_names, n, oyDeAllocateFunc_);
    }

    if(taxi_id)
    {
      oyProfile_s * ip;
      oyOptions_s * options = NULL;
      char * show_text = 0;
      const char * file_name = NULL;

      error = oyOptions_SetFromString( &options,
                                 "//" OY_TYPE_STD "/argv/TAXI_id",
                                 taxi_id,
                                 OY_CREATE_NEW );

      ip = oyProfile_FromTaxiDB( options, NULL );
      if(!ip)
      {
        STRING_ADD( show_text, _("Could not open: ") );
        STRING_ADD( show_text, file_name );
      } else
      {
        is_device_profile = 1;
        installProfile( ip, path, is_device_profile, test, show_text, show_gui);

        oyProfile_Release( &ip );
      }
    }

    if(install)
    {
      const char * file_name = install;
      oyProfile_s * ip = 0;
      char * show_text = 0;

      if(strcmp(install, "-") != 0)
      {
        if(file_name && strlen(file_name) > 10 && memcmp(file_name,"http://", 7) == 0)
        {
          size_t size = 0;
          char * mem = NULL;

          mem = oyReadUrlToMemf_( &size, "r", oyAllocateFunc_,
                                  file_name );

          if(mem && size)
          {
            ip = oyProfile_FromMem( size, mem, OY_NO_CACHE_WRITE, NULL );
            oyFree_m_( mem ); size = 0;
          }

        } else
          ip = oyProfile_FromFile( file_name, OY_NO_CACHE_WRITE | flags, 0 );
      }

      if(!ip)
      {
        STRING_ADD( show_text, _("Could not open or invalid data: ") );
        STRING_ADD( show_text, file_name );
      }

      installProfile( ip, path, is_device_profile, test, show_text, show_gui);

      oyProfile_Release( &ip );
    }

    if(names)
      oyStringListRelease_(&names, count, oyDeAllocateFunc_);
  } else
    if(ui)
    {
    FILE * f = NULL;
    char * t = oyjlOptions_PrintHelp( ui->opts, ui, verbose, &f, NULL );
    fputs( t, f ); free(t);
    }
#undef flags

  clean_main:
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

char ** environment = NULL;
int main( int argc_, char**argv_, char ** envv )
{
  int argc = argc_;
  char ** argv = argv_;

#ifdef __ANDROID__
  argv = calloc( argc + 2, sizeof(char*) );
  memcpy( argv, argv_, (argc + 2) * sizeof(char*) );
  argv[argc++] = "--render=gui"; /* start QML */
  environment = environ;
#else
  environment = envv;
#endif

  /* language needs to be initialised before setup of data structures */
#ifdef USE_GETTEXT
#ifdef HAVE_LOCALE_H
  setlocale(LC_ALL,"");
#endif
#endif

  oyExportStart_(EXPORT_CHECK_NO);

  myMain(argc, (const char **)argv);

#ifdef __ANDROID__
  free( argv );
#endif

  oyjlLibRelease();

  oyFinish_( FINISH_IGNORE_I18N | FINISH_IGNORE_CACHES );

  return 0;
}

int    installProfile                ( oyProfile_s       * ip,
                                       const char        * path,
                                       int                 is_device_profile,
                                       int                 test,
                                       char              * show_text,
                                       int                 show_gui )
{
  int error = 0,
      severity = 0;
      {
        const char * in = oyProfile_GetText( ip, oyNAME_DESCRIPTION );
        oyOptions_s * opts = 0;

        oyOptions_SetFromString( &opts, "////path", path, OY_CREATE_NEW );
        if(is_device_profile)
          oyOptions_SetFromString( &opts, "////device", "1", OY_CREATE_NEW );
        if(test)
          oyOptions_SetFromString( &opts, "////test", "1", OY_CREATE_NEW );
        error = oyProfile_Install( ip, oySCOPE_USER, opts );

        if(error == oyERROR_DATA_AMBIGUITY)
        {
          if(!show_text)
          {
            STRING_ADD( show_text, _("Profile already installed") );
            STRING_ADD( show_text, ":" );
          }
          STRING_ADD( show_text, " \'" );
          if(in)
            STRING_ADD( show_text, in );
          STRING_ADD( show_text, "\'" );
          severity = oyMSG_ERROR;
        } else if(error == oyERROR_DATA_WRITE)
        {
          if(!show_text)
          {
            STRING_ADD( show_text, _("Path can not be written") );
            STRING_ADD( show_text, ":" );
          }
          STRING_ADD( show_text, " \'" );
          if(path)
            STRING_ADD( show_text, path );
          STRING_ADD( show_text, "\'" );
          severity = oyMSG_ERROR;
        } else if(error == oyCORRUPTED)
        {
          if(!show_text)
          {
            STRING_ADD( show_text, _("Profile not useable") );
            STRING_ADD( show_text, ":" );
          }
          STRING_ADD( show_text, " \'" );
          if(in)
            STRING_ADD( show_text, in );
          STRING_ADD( show_text, "\'" );
          severity = oyMSG_ERROR;
        } else if(error > 0 && !show_text)
          oyStringAddPrintf_( &show_text, oyAllocateFunc_, oyDeAllocateFunc_,
                              "%s - %d",_("Internal Error"), error );

        oyShowMessage( severity, show_text, show_gui ); 
      }

  return error;
}
