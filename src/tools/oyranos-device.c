/** @file oyranos_device.cpp
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2012-2021 (C) Kai-Uwe Behrmann
 *
 *  @brief    device manipulation tool
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2012/12/07
 *
 */
/* !cc -Wall -g test_device.c -o test_device2 -L. `oyranos-config --cflags --ldstaticflags` -lm -lltdl */

/* cc -Wall -g test_device.c -o test_device2 -L. `oyranos-config --cflags --ldstaticflags` `pkg-config --cflags --libs libxml-2.0` -lm -I ../../ -I ../../API_generated/ */

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
#include "oyProfiles_s.h"

#include "oyjl.h"
#include "oyjl_macros.h"
#include "oyjl_version.h"
extern char **environ;

void* oyAllocFunc(size_t size) {return malloc (size);}
void  oyDeAllocFunc ( oyPointer ptr) { if(ptr) free (ptr); }

oyjlOptionChoice_s * getDevicePosChoices        ( oyjlOption_s      * o OYJL_UNUSED,
                                                  int               * selected,
                                                  oyjlOptions_s     * opts )
{
  oyjlOptionChoice_s * c = NULL;
  oyConfigs_s * devices = 0;
  oyOptions_s * options = 0;
  int i,
      flags = 0,
      choices = 0;
  OYJL_GET_RESULT_INT( opts, "r", 0, skip_x_color_region_target );
  OYJL_GET_RESULT_INT( opts, "2", 0, icc_version_2 );
  OYJL_GET_RESULT_INT( opts, "4", 0, icc_version_4 );
  OYJL_GET_RESULT_STRING( opts, "c", NULL, device_class );
  if(!device_class) return c;

  if(!skip_x_color_region_target)
    oyOptions_SetFromString( &options,
              "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target", "yes",
                                     OY_CREATE_NEW );
  oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/command",
                                     "list", OY_CREATE_NEW );
  if(icc_version_2) flags |= OY_ICC_VERSION_2;
  if(icc_version_4) flags |= OY_ICC_VERSION_4;
  oyOptions_SetFromInt( &options,
                                    "//" OY_TYPE_STD "/icc_profile_flags",
                                    flags, 0, OY_CREATE_NEW );
  oyDevicesGet( OY_TYPE_STD, device_class, options, &devices );
  choices = oyConfigs_Count( devices );
  c = calloc(choices+1, sizeof(oyjlOptionChoice_s));

  if(c)
  {
    for(i = 0; i < choices; ++i)
    {
      char * v = malloc(12);

      sprintf(v, "%d", i);
      c[i].nick = v;
      c[i].name = strdup(v);
      c[i].description = strdup("");
      c[i].help = strdup("");
    }
  }
  if(selected)
    *selected = -1;

  oyConfigs_Release( &devices );
  oyOptions_Release( &options );

  return c;
}

icSignature  getProfileClass         ( const char        * device_class,
                                       int                 verbose )
{
  icSignature profile_class = icSigDisplayClass;
      oyConfDomain_s * d = oyConfDomain_FromReg( device_class, 0 );
      const char * icc_profile_class = oyConfDomain_GetText( d,
                                             "icc_profile_class", oyNAME_NICK );
      if(icc_profile_class && strcmp(icc_profile_class,"display") == 0)
        profile_class = icSigDisplayClass;
      else if(icc_profile_class && strcmp(icc_profile_class,"output") == 0)
        profile_class = icSigOutputClass;
      else if(icc_profile_class && strcmp(icc_profile_class,"input") == 0)
        profile_class = icSigInputClass;
      else if(strcmp(device_class,"monitor") == 0)
        profile_class = icSigDisplayClass;
      else if(strcmp(device_class,"printer") == 0)
        profile_class = icSigOutputClass;
      else if(strcmp(device_class,"camera") == 0)
        profile_class = icSigInputClass;
      else if(strcmp(device_class,"scanner") == 0)
        profile_class = icSigInputClass;

      if(verbose)
        fprintf( stderr, "icc_profile_class: %s\n", icc_profile_class ? icc_profile_class : device_class );
      oyConfDomain_Release( &d );
  return profile_class;
}

char **  listProfiles                ( oyConfig_s        * c, /* device */
                                       icSignature         profile_class,
                                       const char        * device_class,
                                       int                 flags,
                                       int                 verbose,
                                       int                 show_non_device_related,
                                       oyOptions_s       * options,
                                       int               * count )
{
      int size, i, current = -1, current_tmp = 0, pos = 0;
      oyProfile_s * profile = 0, * temp_profile = 0;
      oyProfiles_s * patterns = 0, * iccs = 0;
      const char * profile_file_name = 0;
      int32_t * rank_list;
      int empty_added;
      char ** list = NULL;

      *count = 0;
      profile = oyProfile_FromSignature( profile_class, oySIGNATURE_CLASS, 0 );
      patterns = oyProfiles_New( 0 );
      oyProfiles_MoveIn( patterns, &profile, -1 );

      iccs = oyProfiles_Create( patterns, flags, 0 );
      oyProfiles_Release( &patterns );


      size = oyProfiles_Count(iccs);
      if(verbose)
      {
        oyOptions_s * options = NULL;
        char * json = NULL;
        char * t = oyjlBT(0);
        int error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/options/source",
                                         "backend_core", OY_CREATE_NEW );
        error = oyDeviceToJSON( c, options, &json, oyAllocFunc );
        fprintf( stderr, OY_DBG_FORMAT_ "%s%s %s: %d\n%s\n", OY_DBG_ARGS_, t,
                 _("found profiles for device class"), device_class, size, json );
        free(t);
        if(json) free(json);
      }
      rank_list = (int32_t*) malloc( size * sizeof(int32_t) );
      oyProfiles_DeviceRank( iccs, c, rank_list );

      size = oyProfiles_Count(iccs);

      oyDeviceGetProfile( c, options, &profile );
      profile_file_name = oyProfile_GetFileName( profile, 0 );

      empty_added = -1;

      for( i = 0; i < size; ++i)
      {
        const char * temp_profile_file_name, * description;
        {
           temp_profile = oyProfiles_Get( iccs, i );
           description = oyProfile_GetText( temp_profile, oyNAME_DESCRIPTION );
           temp_profile_file_name = oyProfile_GetFileName( temp_profile, 0);

           current_tmp = -1;

           if(profile_file_name && temp_profile_file_name &&
              strcmp( profile_file_name, temp_profile_file_name ) == 0)
             current_tmp = pos;

           if(current == -1 && current_tmp != -1)
             current = current_tmp;

           if(empty_added == -1 &&
              rank_list[i] < 1)
           {
             //fprintf(stdout, "automatic\n");
             oyjlStringListPush( &list, count, "automatic", malloc,free );
             empty_added = pos;
             if(current != -1 &&
                current == pos)
               ++current;
             ++pos;
           }

           if(show_non_device_related == 1 ||
              rank_list[i] > 0 ||
              current_tmp != -1)
           {
             char * t = NULL;
             oyjlStringAdd( &t, 0,0, "[%d-%d] %s (%s)",
              pos+1, rank_list[i], description, temp_profile_file_name);
             oyjlStringListPush( &list, count, t, malloc,free );
             free(t);
 
             ++pos;
           }
        }
        oyProfile_Release( &temp_profile );
      }
      if(empty_added == -1)
      {
        ++pos;
        if(current == -1 && current_tmp != -1)
          current = pos;
      }
      if(verbose)
        fprintf( stderr, "current: %d\n", current );

      oyProfile_Release( &profile );
      oyProfiles_Release( &iccs );
      free( rank_list );
      *count = pos;

  return list;
}

oyjlOptionChoice_s * getDeviceProfileChoices    ( oyjlOption_s      * o OYJL_UNUSED,
                                                  int               * selected,
                                                  oyjlOptions_s     * opts )
{
  oyjlOptionChoice_s * c = NULL;
  oyConfigs_s * devices = 0;
  oyOptions_s * options = 0;
  int i,n,
      flags = 0,
      choices = 0;
  oyjlOption_s * skip_x = oyjlOptions_GetOptionL( opts, "r", 0 );
  int skip_x_color_region_target = *skip_x->variable.i;
  OYJL_GET_RESULT_INT( opts, "2", 0, icc_version_2 );
  OYJL_GET_RESULT_INT( opts, "4", 0, icc_version_4 );
  OYJL_GET_RESULT_STRING( opts, "c", NULL, device_class );
  OYJL_GET_RESULT_INT( opts, "d", -1, device_pos );
  OYJL_GET_RESULT_STRING( opts, "device-name", NULL, device_name );
  OYJL_GET_RESULT_INT( opts, "v", 0, verbose );
  OYJL_GET_RESULT_INT( opts, "show-non-device-related", 0, show_non_device_related );

  icSignature profile_class;
  char ** list;
  oyConfig_s * d = NULL;

  if(!device_class) return c;
  if(device_pos == -1 && !device_name) return c;

  profile_class = getProfileClass( device_class, verbose );
  if(verbose)
  {
    fprintf( stderr, OY_DBG_FORMAT_ "-c=%s -d=%d --device-name=%s\n", OY_DBG_ARGS_,
             device_class, device_pos, device_name?device_name:"" );
  }

  if(!skip_x_color_region_target)
    oyOptions_SetFromString( &options,
              "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target", "yes",
                                     OY_CREATE_NEW );
  oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/command",
                                     "list", OY_CREATE_NEW );
  if(icc_version_2) flags |= OY_ICC_VERSION_2;
  if(icc_version_4) flags |= OY_ICC_VERSION_4;
  oyOptions_SetFromInt( &options,
                                    "//" OY_TYPE_STD "/icc_profile_flags",
                                    flags, 0, OY_CREATE_NEW );
  if(device_name)
    oyDeviceGet( OY_TYPE_STD, device_class, device_name, options, &d );
  else
  {
    oyDevicesGet( OY_TYPE_STD, device_class, options, &devices );
    d = oyConfigs_Get( devices, device_pos );
  }

  list = listProfiles( d, profile_class, device_class, flags, verbose, show_non_device_related, options, &n );
  choices = n;

  c = calloc(choices+1, sizeof(oyjlOptionChoice_s));

  if(c)
  {
    for(i = 0; i < choices; ++i)
    {
      c[i].nick = strdup(list[i]);
      c[i].name = strdup(list[i]);
      c[i].description = strdup("");
      c[i].help = strdup("");
    }
  }
  if(selected)
    *selected = -1;

  oyConfigs_Release( &devices );
  oyOptions_Release( &options );

  return c;
}

/* This function is called the
 * * first time for GUI generation and then
 * * for executing the tool.
 */
int myMain( int argc, const char ** argv )
{
  int error = 0;
  int state = 0;
  int assign = 0;
  const char * device_class = 0;
  int device_pos = -1;
  const char * profile_name = 0;
  const char * new_profile_name = 0;
  int system_wide = 0;
  int erase = 0;
  int setup = 0;
  int unset = 0;
  int list = 0;
  int verbose = 0;
  int short_var = 0;
  int print_class = 0;
  const char * device_name = 0;
  int path = 0;
  int skip_x_color_region_target = 0;
  int list_profiles = 0;
  int show_non_device_related = 0;
  int list_taxi_profiles = 0;
  const char * format = 0;
  const char * device_json = 0;
  const char * rank_json = 0;
  const char * output = 0;
  int icc_version_2 = 0;
  int icc_version_4 = 0;
  int only_db = 0;
  int device_meta_tag = 0;
  const char * export = 0;
  const char * render = 0;

  int list_rank_paths = 0;

  /* handle options */
  /* declare the option choices  *   nick,          name,               description,                  help */
  oyjlOptionChoice_s c_choices[] = {{"monitor",     _("Monitor"),       "",                           ""},
                                    {"printer",     _("Printer"),       "",                           ""},
                                    {"camera",      _("Camera"),        "",                           ""},
                                    {"scanner",     _("Scanner"),       "",                           ""},
                                    {NULL,NULL,NULL,NULL}};
  oyjlOptionChoice_s f_choices[] = {{"icc",         _("write assigned ICC profile"),"",                        ""},
                                    {"fallback-icc",_("create fallback ICC profile"),"",                        ""},
                                    {"openicc+rank-map",_("create OpenICC device color state JSON including the rank map"),"",                        ""},
                                    {"openicc",     _("create OpenICC device color state JSON"),"",                        ""},
                                    {"openicc-rank-map",_("create OpenICC device color state rank map JSON"),"",                        ""},
                                    {NULL,NULL,NULL,NULL}};

  oyjlOptionChoice_s E_choices[] = {{"OY_DEBUG", _("set the Oyranos debug level. Alternatively the -v option can be used."),NULL,                         NULL},
                                    {"XDG_DATA_HOME XDG_DATA_DIRS",_("route Oyranos to top directories containing resources. The derived paths for ICC profiles have a \"color/icc\" appended."),_("http://www.oyranos.com/wiki/index.php?title=OpenIccDirectoryProposal"),NULL},
                                    {NULL,NULL,NULL,NULL}};

  oyjlOptionChoice_s S_choices[] = {{_("oyranos-policy(1) oyranos-config(1) oyranos-profiles(1) oyranos-profile(1) oyranos(3)"),NULL,               NULL,                         NULL},
                                    {"http://www.oyranos.org",NULL,               NULL,                         NULL},
                                    {NULL,NULL,NULL,NULL}};

  /* declare options - the core information; use previously declared choices */
  oyjlOption_s oarray[] = {
  /* type,   flags,                      o,  option,          key,      name,          description,                  help, value_name,         
        value_type,              values,             variable_type, variable_name */
    {"oiwi", 0,                          "a","assign",        NULL,     _("Assign"),   _("add configuration to OpenICC DB"),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&assign},NULL},
    {"oiwi", OYJL_OPTION_FLAG_IMMEDIATE, "c","device-class",  NULL,     _("Device Class"),_("use device class. Useful device classes are monitor, scanner, printer, camera."),NULL, _("CLASS"),         
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)c_choices, sizeof(c_choices), malloc ), 0}}, oyjlSTRING,    {.s=&device_class},NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE|OYJL_OPTION_FLAG_IMMEDIATE,  "d","device-pos",    NULL,     _("Device Pos"),  _("device position start from zero"),NULL, _("NUMBER"),        
        oyjlOPTIONTYPE_FUNCTION, {.getChoices = getDevicePosChoices},                oyjlINT,       {.i=&device_pos},NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE|OYJL_OPTION_FLAG_ACCEPT_NO_ARG,  "p","profile-name",  NULL,     _("Profile Name"),_("profile file name"),    _("Can be \"\" empty string or \"automatic\" or a real profile name."), _("ICC_FILE_NAME"), 
        oyjlOPTIONTYPE_FUNCTION, {.getChoices = getDeviceProfileChoices},                oyjlSTRING,    {.s=&profile_name},NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "n","new-profile-name",  NULL, _("New Profile Name"),_("profile file name"),   NULL, _("ICC_FILE_NAME"), 
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&new_profile_name},NULL},
    {"oiwi", 0,                          NULL,"system-wide",   NULL,     _("System Wide"),_("add computer wide"),       NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&system_wide},NULL},
    {"oiwi", 0,                          "e","erase",         NULL,     _("Erase"),    _("remove configuration from OpenICC DB"),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&erase},NULL},
    {"oiwi", 0,                          "s","setup",         NULL,     _("Setup"),    _("configure session from OpenICC DB"),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&setup},NULL},
    {"oiwi", 0,                          "u","unset",         NULL,     _("Unset"),    _("reset session configuration to zero"),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&unset},NULL},
    {"oiwi", 0,                          "l","list",          NULL,     _("List"),     _("List device classes"),     NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&list},NULL},
    {"oiwi", OYJL_OPTION_FLAG_IMMEDIATE, "v","verbose",       NULL,     _("Verbose"),  _("verbose"),                 NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&verbose},NULL},
    {"oiwi", OYJL_OPTION_FLAG_IMMEDIATE, NULL,"short",        NULL,     _("Short"),    _("print module ID or profile name"),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&short_var},NULL},
    {"oiwi", OYJL_OPTION_FLAG_IMMEDIATE, NULL,"print-class",  NULL,     _("Print Class"),_("print the modules device class"),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&print_class},NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  NULL,"device-name",  NULL,     _("Device Name"),_("alternatively specify the name of a device"),NULL, _("NAME"),          
        oyjlOPTIONTYPE_FUNCTION, {0},                oyjlSTRING,    {.s=&device_name},NULL},
    {"oiwi", 0,                          NULL,"path",         NULL,     _("Path"),     _("print the full file name"),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&path},NULL},
    {"oiwi", 0,                          "r","skip-x-color-region-target",NULL,     _("Skip X Color Region Target"),_("skip X Color Management device profile"),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&skip_x_color_region_target},NULL},
    {"oiwi", 0,                          NULL,"list-profiles", NULL,     _("List Profiles"),_("List local DB profiles for selected device"),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&list_profiles},NULL},
    {"oiwi", 0,                          NULL,"show-non-device-related",NULL,     _("Show Non Device Related"),_("show as well non matching profiles"),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&show_non_device_related},NULL},
    {"oiwi", 0,                          NULL,"list-taxi-profiles",NULL,     _("List Taxi Profiles"),_("List Taxi DB profiles for selected device"),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&list_taxi_profiles},NULL},
    {"oiwi", 0,                          "f","format",        NULL,     _("Format"),   _("dump configuration data"), NULL, _("icc|openicc+rank-map|openicc|openicc-rank-map"),
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)f_choices, sizeof(f_choices), malloc ), 0}}, oyjlSTRING,    {.s=&format},NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "j","device-json",   NULL,     _("Device Json"),_("use device JSON alternatively to -c and -d options"),NULL, _("FILENAME"),      
        oyjlOPTIONTYPE_FUNCTION, {0},                oyjlSTRING,    {.s=&device_json},NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "k","rank-json",     NULL,     _("Rank Json"),_("use rank map JSON alternatively to -c and -d options"),NULL, _("FILENAME"),      
        oyjlOPTIONTYPE_FUNCTION, {0},                oyjlSTRING,    {.s=&rank_json},NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "o","output",        NULL,     _("Output"),   _("write to specified file"), NULL, _("FILENAME"),      
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&output},NULL},
    {"oiwi", 0,                          NULL,"only-db",       NULL,     _("Only Db"),  _("use only DB keys for -f=openicc"),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&only_db},NULL},
    {"oiwi", 0,                          "m","device-meta-tag",NULL,     _("Device Meta Tag"),_("embedd device and driver information into ICC meta tag"),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&device_meta_tag},NULL},
    {"oiwi", 0,                          "h","help",          NULL,     NULL,          NULL,                    NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlNONE,      {0},NULL},
    {"oiwi", 0, "X", "export", NULL, NULL, NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices.list = NULL}, oyjlSTRING, {.s=&export},NULL},
    {"oiwi", 0,                          "V","version",       NULL,     _("Version"),  _("Version"),                 NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlNONE,      {0},NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "R", "render", NULL, NULL,  NULL,  NULL, NULL, oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s=&render},NULL},
    {"oiwi", 0,                          "2","icc-version-2", NULL,     _("Icc Version 2"),_("Select ICC v2 Profiles"),  NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&icc_version_2},NULL},
    {"oiwi", 0,                          "4","icc-version-4", NULL,     _("Icc Version 4"),_("Select ICC v4 Profiles"),  NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&icc_version_4},NULL},
    {"oiwi", 0,                          "E","man-environment",NULL,    NULL,          NULL,                         NULL, NULL,               
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)E_choices, sizeof(E_choices), malloc ), 0}}, oyjlNONE,      {},NULL},
    {"oiwi", 0,                          "S","man-see_also",  NULL,     NULL,          NULL,                         NULL, NULL,               
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)S_choices, sizeof(S_choices), malloc ), 0}}, oyjlNONE,      {},NULL},
    /* default option template -X|--export */
    {"",0,0,NULL,NULL,NULL,NULL,NULL, NULL, oyjlOPTIONTYPE_END, {0},0,{0},0}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups[] = {
  /* type,   flags, name,               description,                  help,               mandatory,     optional,      detail */
    {"oiwg", 0,     NULL,               _("Set basic parameters"),    NULL,               NULL,          NULL,          "c,d,device-name,2,4,r",NULL},
    {"oiwg", 0,     NULL,               _("Assign profile to device"),NULL,               "a,c,d",       "p,system-wide", "a,p,system-wide",NULL},
    {"oiwg", 0,     NULL,               _("Unassign profile from device"),NULL,           "e,c,d",       "system-wide", "e" ,NULL},
    {"oiwg", 0,     NULL,               _("Setup device"),            NULL,               "s,c,d",       NULL,          "s" ,NULL},
    {"oiwg", 0,     NULL,               _("Unset device"),            NULL,               "u,c,d",       NULL,          "u" ,NULL},
    {"oiwg", 0,     NULL,               _("List device classes"),     NULL,               "l",           "v,short,print-class","l,v,short,print-class",NULL},
    {"oiwg", 0,     NULL,               _("List devices"),            _("Needs -c option."),               "l",         "c,d|device-name,short,path,r,v","l,c,short,path",NULL},
    {"oiwg", 0,     NULL,               _("List local DB profiles for selected device"),_("Needs -c and -d options."),               "list-profiles","c,d,show-non-device-related","list-profiles,show-non-device-related",NULL},
    {"oiwg", 0,     NULL,               _("List Taxi DB profiles for selected device"),_("Needs -c and -d options."),               "list-taxi-profiles","c,d,show-non-device-related","list-taxi-profiles,show-non-device-related",NULL},
    {"oiwg", 0,     NULL,               _("Dump device color state"), _("Needs -c and -d options."),               "f,c,d,j,k",   "o,only-db,m", "f,o,j,k,only-db,m",NULL},
    {"oiwg", OYJL_GROUP_FLAG_GENERAL_OPTS, NULL, _("General options"),NULL,               "h|X|V|R",     "v",           "h,X,V,R,v",NULL},
    {"",0,0,0,0,0,0,0,0}
  };

  oyjlUiHeaderSection_s * sections = oyUiInfo(_("The oyranos-device program shows and administrates ICC profiles for color devices."),
                  "2021-05-13T12:00:00", "May 13, 2021");
  oyjlUi_s * ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oyranos-device", _("Oyranos Color Devices"), _("The Tool handles Oyranos CMS Color Devices."),
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
    fprintf( stderr, "%s\n\tman oyranos-device\n\n", _("For more information read the man page:") );
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
  \"command_set\": \"oyranos-device\",\n\
  \"comment\": \"command_set_delimiter - build key:value; default is '=' key=value\",\n\
  \"comment\": \"command_set_option - use \\\"-s\\\" \\\"key\\\"; skip \\\"--\\\" direct in front of key\",\n\
  \"command_get\": \"oyranos-device\",\n\
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
    oyjlArgsRender( argc, argv, NULL, jcommands, NULL, debug, ui, myMain );
#else
    fprintf( stderr, "No render support compiled in. For a GUI use -X json and load into oyjl-args-render viewer." );
#endif
  } else if(ui)
  {
    /* ... working code goes here ... */
    const char * prof_name = profile_name;
    int simple = 0;
    int flags = 0;
    oySCOPE_e scope = oySCOPE_USER;

    oyProfile_s * prof = 0;
    oyConfigs_s * devices = 0;
    oyOptions_s * options = 0;
    oyConfig_s * c = 0,
               * dt = 0;
    size_t size = 0;
    const char * filename = 0,
               * device_name = 0;
    char * data = 0, *t;
    int n = 0;

    if(print_class) simple = 2;
    if(path)
    { simple = 2; list_rank_paths = 1;}
    if(icc_version_2) flags |= OY_ICC_VERSION_2;
    if(icc_version_4) flags |= OY_ICC_VERSION_4;
    if(short_var)
      simple = 1;
    if(system_wide)
      scope = oySCOPE_SYSTEM;
    char ** results = oyjlOptions_ResultsToList( ui->opts, "d", &n );
    if(n && strcmp(results[0],"oyjl-list") == 0)
    {
      device_pos = -2;
      if(verbose)
        fprintf( stderr, "device_pos: %d %d \"%s\"\n", device_pos, n, results?results[0]:"---" );
    }
    oyjlStringListRelease( &results, n, free );
    results = oyjlOptions_ResultsToList( ui->opts, "device-name", &n );
    if(n && strcmp(results[0],"oyjl-list") == 0)
    {
      device_pos = -3;
      if(verbose)
        fprintf( stderr, "device_pos: %d %d \"%s\"\n", device_pos, n, results?results[0]:"---" );
    }
    oyjlStringListRelease( &results, n, free );
    if( profile_name && strcmp(profile_name,"oyjl-list") == 0 )
    {
      assign = 0;
      list_profiles = 1;
      if(verbose)
      {
        results = oyjlOptions_ResultsToList( ui->opts, "device-name", &n );
        fprintf( stderr, "profile_name: \"%s\" assign: %d list_profiles: %d\n", profile_name, assign, list_profiles );
        oyjlStringListRelease( &results, n, free );
        prof_name = NULL;
      }
    } else if(profile_name && profile_name[0])
      assign = 1;
    n = 0;


#define TEST_CAT_FILE( string, fn ) {\
  size_t size = 0; \
  char * text = oyReadFileToMem_( fn, &size, oyAllocateFunc_ ); \
  if( !string || (text && strstr(text, string) != NULL)) \
  { \
    if(text) \
      fprintf( stdout, "%s \n", text ); \
    else \
      fprintf( stderr, "%s %s\n", _("File not loaded!"), fn ); \
    done = 1; \
  } \
  oyFree_m_( text ); \
  }

  if(format && strcmp(format, "openicc-rank-map") == 0 && 
     (device_class || list_rank_paths))
  {
     int files_n = 0, i, done = 0;
     const char * subdir = "color/rank-map";
     int data = oyYES,
         owner = oySCOPE_USER_SYS;
     const char * dir_string = NULL,
                * string = "config.icc_profile",
                * suffix = "json";

     char ** files = oyDataFilesGet_( &files_n, subdir, data, owner,
                                      dir_string, string, suffix,
                                      oyAllocateFunc_ );
     if(list_rank_paths)
     {
       int path_names_n = 0;
       char ** path_names = oyDataPathsGet_( &path_names_n, subdir, oyALL,
                                             owner, oyAllocateFunc_ );
       for( i = 0; i < path_names_n; ++i )
         fprintf(stdout, "%s\n", path_names[i] );

       oyStringListRelease_( &path_names, path_names_n, free );

       return 0;
     }

     if(verbose)
       fprintf(stderr, "found rank maps: %d\n", files_n);
     for( i = 0; i < files_n; ++i )
     {
       const char * file = files[i];

       if(verbose)
         fprintf(stderr, "%d: %s\n", i, files[i]);

       if(strstr(file, device_class) != NULL)
       {
         char * no = NULL;
         TEST_CAT_FILE( no, file )
         done = 1;
         break;
       }
     }

     if(!done)
     for( i = 0; i < files_n; ++i )
     {
       const char * file = files[i];
       {
         TEST_CAT_FILE( device_class, file )
         if(done)
           break;
       }
     }
     return 0;
  }

  /* resolve device_class */
  if(device_class)
  {
    /* get XCM_ICC_COLOR_SERVER_TARGET_PROFILE_IN_X_BASE */
    if(!skip_x_color_region_target)
      error = oyOptions_SetFromString( &options,
              "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target", "yes",
                                     OY_CREATE_NEW );
    if(verbose)
      error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/command",
                                     "properties", OY_CREATE_NEW );
    else
      error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/command",
                                     "list", OY_CREATE_NEW );
    error = oyOptions_SetFromInt( &options,
                                    "//" OY_TYPE_STD "/icc_profile_flags",
                                    flags, 0, OY_CREATE_NEW );

    if(device_name && strcmp(device_name, "oyjl-list") != 0)
      error = oyDeviceGet( OY_TYPE_STD, device_class, device_name, options, &c );
    else
      error = oyDevicesGet( OY_TYPE_STD, device_class, options, &devices );

    oyOptions_Release( &options );

    n = oyConfigs_Count( devices );
    if( device_pos >= 0 &&
        (device_pos >= (int)n || device_pos < -3) )
    {
      device_name = 0;
      fprintf( stderr, "%s %s\n  device_class: \"%s\" device_pos: \"%d\"  %s: %d\n", _("!!! ERROR"), _("Could not resolve device."),
               device_class?device_class:"????", device_pos,
               _("Available devices"), n);
      return 1;
    }
  }

  /* resolve device_name */
  if(device_pos >= 0)
  {
    char * t;

    c = oyConfigs_Get( devices, device_pos );
    if(!c)
    {
      fprintf( stderr, "%s %s\n  device_class: \"%s\" device_pos: \"%d\"  %s: %d\n", _("!!! ERROR"), _("Could not resolve device."),
               device_class?device_class:"????", device_pos,
               _("Available devices"), n);
      return 1;
    }

    device_name = oyConfig_FindString( c, "device_name", 0 );
    if(device_name)
    {
      t = strdup(device_name);
      device_name = t;
    }
    else
    {
      fprintf( stderr, "%s %s: %s %d. %s: %d\n", _("!!! ERROR"), _("Could not resolve device_name"),
               device_class?device_class:"????", device_pos,
               _("Available devices"), n);
      return 1;
    }
  } else
    if(device_pos == -2) /* "oyjl-list" */
  {
    int i;
    for(i = 0; i < n; ++i)
      printf( "%d\n", i );
  }

  if(device_json)
  {
    size_t json_size = 0;
    char * json_text = oyReadFileToMem_( device_json, &json_size, oyAllocateFunc_ );
    if(json_text)
    {
      error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/underline_key_suffix",
                                     "xxx", OY_CREATE_NEW );
      error = oyDeviceFromJSON( json_text, options, &c );
      oyDeAllocateFunc_( json_text );
      oyOptions_Release( &options );
    }
    if(!c)
    {
      fprintf( stderr, OY_DBG_FORMAT_ "%s %s: %s  %d\n", OY_DBG_ARGS_, _("!!! ERROR"), _("Could not resolve device_json"),
               device_json, error);
      return 1;
    }
    device_class = oyStringCopy_( oyConfig_FindString( c, "device_class", 0 ), NULL );
    if(!device_class)
      device_class = oyStringCopy_( oyConfig_FindString( c, "device_type", 0 ), NULL );
    device_name = oyConfig_FindString( c, "device_name", 0 );
    if(!device_name)
      device_name = device_json;

    if(rank_json || !oyConfig_GetRankMap(c))
    {
      oyRankMap * rank_map = NULL;
      const char * rankj = rank_json ? rank_json : device_json;

      json_size = 0;
      json_text = oyReadFileToMem_( rankj, &json_size, oyAllocateFunc_ );
      error = oyRankMapFromJSON ( json_text, NULL, &rank_map, oyAllocateFunc_ );
      oyDeAllocateFunc_( json_text );
      if(!rank_map || error || !rank_map[0].key)
      {
        char ** list = NULL;
        fprintf( stderr, OY_DBG_FORMAT_ "%s: %s: %s  %d\n", OY_DBG_ARGS_, _("WARNING"), _("Creation of rank_map failed from"), rankj, error );
        error = oyRankMapList( device_class, NULL, &list, oyAllocateFunc_ );
        if(error)
          fprintf( stderr, OY_DBG_FORMAT_ "%s: %s: %s  %d\n", OY_DBG_ARGS_, _("WARNING"), _("Creation of rank_map failed from"), device_class, error );
        if(list && list[0])
        { json_size = 0;
          json_text = oyReadFileToMem_( list[0], &json_size, oyAllocateFunc_ );
          error = oyRankMapFromJSON ( json_text, NULL, &rank_map, oyAllocateFunc_ );
          if(error)
            fprintf( stderr, OY_DBG_FORMAT_ "%s: %s: %s  %d\n", OY_DBG_ARGS_, _("WARNING"), _("Creation of rank_map failed from"), device_class, error );
        }
      }

      if(rank_map)
        oyConfig_SetRankMap( c, rank_map );

      if( (list_profiles || list_taxi_profiles) && rank_json )
      {
        fprintf( stderr, "%s: %s  %d\n", _("!!! ERROR"),
                 _("Could not resolve rank_json"), error );
        return 1;
      }
    }

    if(verbose)
    {
      fprintf( stderr, "%s: %s %s\n", _("Using JSON"), device_class, device_name );
      fprintf( stderr, "%s\n", oyOptions_GetText( *oyConfig_GetOptions( c, "db"), oyNAME_NAME ) );
    }
  }

  if(list && device_class)
  {
    char * text = NULL,
         * report = NULL;
    int i,n;

    if(!skip_x_color_region_target)
      error = oyOptions_SetFromString( &options,
              "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target", "yes",
                                     OY_CREATE_NEW );
    error = oyOptions_SetFromInt( &options,
                                    "//" OY_TYPE_STD "/icc_profile_flags",
                                    flags, 0, OY_CREATE_NEW );
    n = oyConfigs_Count( devices );
    if(error <= 0)
    {
      for(i = 0; i < n || (n == 0 && c); ++i)
      {
        if(n != 0)
          c = oyConfigs_Get( devices, i );

        if( device_pos >= 0 && device_pos != i )
        {
          oyConfig_Release( &c );
          continue;
        }

        if(verbose)
        printf("------------------------ %d ---------------------------\n",i);

        error = oyDeviceGetInfo( c, oyNAME_NICK, options, &text,
                                 oyAllocFunc );
        oyDeviceAskProfile2( c, options, &prof );

        if(simple == 0)
        {
          if(device_pos >= -1)
            oyStringAddPrintf_( &report, oyAllocFunc, oyDeAllocFunc,
                              "%d: ", i );
          if(device_pos >= -1)
          oyStringAddPrintf_( &report, oyAllocFunc, oyDeAllocFunc,
                              "\"" );
          oyStringAddPrintf_( &report, oyAllocFunc, oyDeAllocFunc,
                              "%s", text ? text : "???" );
          if(device_pos >= -1)
          oyStringAddPrintf_( &report, oyAllocFunc, oyDeAllocFunc,
                              "\" " );
          if(device_pos >= -1)
          error = oyDeviceGetInfo( c, oyNAME_NAME, options, &text,
                                   oyAllocFunc );
          if(device_pos >= -1)
          oyStringAddPrintf_( &report, oyAllocFunc, oyDeAllocFunc,
                              "%s%s", text ? text : "???",
                              (i+1 == n) || device_pos != -1 ? "" : "\n" );
          else if(i < n-1)
          oyStringAddPrintf_( &report, oyAllocFunc, oyDeAllocFunc,
                              "\n" );
        } else
        {
          data = oyProfile_GetMem( prof, &size, 0, oyAllocFunc);
          if(size && data)
            oyDeAllocFunc( data );
          data = 0;
          filename = oyProfile_GetFileName( prof, -1 );
          oyStringAddPrintf_( &report, oyAllocFunc, oyDeAllocFunc,
                              "%s%s", filename ? (simple == 1)?(strrchr(filename,OY_SLASH_C) ? strrchr(filename,OY_SLASH_C)+1:filename):filename : OY_PROFILE_NONE,
                              (i+1 == n) || device_pos != -1 ? "" : "\n" );
        }
        if(verbose)
        {
          error = oyDeviceGetInfo( c, oyNAME_DESCRIPTION, options, &text,
                                   oyAllocFunc );
          printf( "%s\n", text ? text : "???" );
        }

        if(text)
          free( text );

        /* verbose adds */
        if(verbose)
        {
          data = oyProfile_GetMem( prof, &size, 0, oyAllocFunc);
          if(size && data)
            oyDeAllocFunc( data );
          data = 0;
          filename = oyProfile_GetFileName( prof, -1 );
          printf( " server profile \"%s\" size: %d\n",
                  filename?filename:OY_PROFILE_NONE, (int)size );

          text = 0;
          oyDeviceProfileFromDB( c, &text, oyAllocFunc );
          printf( " DB profile \"%s\"\n  keys: %s\n",
                  text?text:OY_PROFILE_NONE,
                  oyConfig_FindString( c, "key_set_name", 0 ) ?
                      oyConfig_FindString( c, "key_set_name", 0 ) :
                      OY_PROFILE_NONE );

          oyDeAllocFunc( text );
          text = 0;

          if(oyConfig_FindString( c, "supported_devices_info", NULL ))
          {
            oyOption_s * o = oyConfig_Find( c, "supported_devices_info" );
            int j = 0;
            const char * t = NULL;
            oyStringAddPrintf_( &report, oyAllocFunc, oyDeAllocFunc, "\n\n" );
            while((t = oyOption_GetValueString( o, j++ )) != NULL)
            {
              oyStringAddPrintf_( &report, oyAllocFunc, oyDeAllocFunc,
                                  "%s\n", t );
            }
          }
        }

        oyProfile_Release( &prof );
        if(n != 0)
          oyConfig_Release( &c );
        else
          break;
      }

      if(report)
        fprintf( stdout, "%s\n", report );
      oyDeAllocFunc( report ); report = 0;
    }
    oyConfigs_Release( &devices );
    oyOptions_Release( &options );

    return 0;

  } else
  if( list )
  {
    uint32_t count = 0,
           * rank_list = 0;
    char ** texts = 0,
          * temp = 0,
         ** attributes = 0,
          * device_class = 0;
    int i,j, attributes_n;
    char separator;

    /* get all configuration filters */
    oyConfigDomainList("//"OY_TYPE_STD"/device", &texts, &count,&rank_list ,0 );
    for( i = 0; i < (int)count; ++i )
    {
      attributes_n = 0;
 
      /* pick the filters name and remove the common config part */
      temp = oyFilterRegistrationToText( texts[i], oyFILTER_REG_APPLICATION,
                                         malloc );
      attributes = oyStringSplit( temp, '.', &attributes_n, malloc );
      free(temp);
      temp = malloc(1024); temp[0] = 0;
      for(j = 0; j < attributes_n; ++j)
      {
        if(strcmp(attributes[j], "config") == 0)
          continue;

        if(j && temp[0])
          sprintf( &temp[strlen(temp)], "." );
        sprintf( &temp[strlen(temp)], "%s", attributes[j]);
      }

      /*  The string in temp can be passed as the device_class argument to
       *  oyDevicesGet().
       */
      if(verbose)
        fprintf( stdout, "%d: %s \"%s\"\n", i, texts[i], temp);
      else
      {
        if(simple)
          separator = '.';
        else
          separator = '/';

        if(simple == 2 && strrchr(texts[i],separator))
        {
          oyConfDomain_s * domain = oyConfDomain_FromReg( texts[i], 0 );
          const char * device_class = oyConfDomain_GetText( domain, "device_class", oyNAME_NICK );
          oyConfDomain_Release( &domain );
		
          fprintf( stdout, "%s\n", device_class );
        }
        else if(strrchr(texts[i],separator))
          fprintf( stdout, "%s\n", strrchr(texts[i],separator) + 1);
        else
          fprintf( stdout, "%s\n", texts[i]);
      }

      oyStringListRelease_( &attributes, attributes_n, free );
      free (device_class);
      free(temp);
    }

    return 0;

  } else
  if( (setup || unset || erase || assign) &&
      c )
  {
    oyProfile_s * profile = 0;
    const char * tmp = 0;

    error = oyOptions_SetFromInt( &options,
                                    "//" OY_TYPE_STD "/icc_profile_flags",
                                    flags, 0, OY_CREATE_NEW );
    error = oyDeviceAskProfile2( c, options, &profile );
    oyOptions_Release( &options );

    if(profile)
      tmp = oyProfile_GetFileName( profile, -1 );

    if(verbose)
      fprintf( stdout, "%s \"%s\" %s %s%s%s\n",
            device_class, device_name, prof_name?prof_name:"", error?"wrong":"good",
            tmp?"\n  has already a profile: ":"", tmp?tmp:"" );

    if(assign && prof_name)
    {
      if(strcmp(prof_name,"") == 0 ||
         strcmp(prof_name,"automatic") == 0)
      {
        if(!device_json)
        {
          /* start with complete device info */
          oyConfig_Release( &c );
          if(!skip_x_color_region_target)
            oyOptions_SetFromString( &options,
                   "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target",
                         "yes", OY_CREATE_NEW );
          error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/command",
                                   "properties", OY_CREATE_NEW );  
          error = oyOptions_SetFromInt( &options,
                                    "//" OY_TYPE_STD "/icc_profile_flags",
                                    flags, 0, OY_CREATE_NEW );
          error = oyDeviceGet( 0, device_class, device_name, options, &dt );
          if(dt)
          {
            oyConfig_Release( &c );
            c = dt; dt = 0;
          }
          oyOptions_Release( &options );
        }

        /*error = oyDeviceSetProfile( c, scope, NULL ); no profile name not supported*/
        error = oyDeviceUnset( c );
        error = oyConfig_EraseFromDB( c, scope );

        if(!device_json)
        {
          oyConfig_Release( &c );
          if(!skip_x_color_region_target)
            oyOptions_SetFromString( &options,
                   "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target",
                         "yes", OY_CREATE_NEW );
          error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/command",
                                   "properties", OY_CREATE_NEW );  
          error = oyOptions_SetFromInt( &options,
                                    "//" OY_TYPE_STD "/icc_profile_flags",
                                    flags, 0, OY_CREATE_NEW );
          error = oyDeviceGet( 0, device_class, device_name, options, &dt );
          if(dt)
          {
            oyConfig_Release( &c );
            c = dt; dt = 0;
          }
          oyOptions_Release( &options );
        }
      } else
      {
        char * file_name = NULL;
        if(prof_name && strchr(prof_name, '(') != NULL && strchr(prof_name, '(') < strchr(prof_name, ')') )
        {
          char * t;
          file_name = oyjlStringCopy( strchr(prof_name,'(') + 1, 0 );
          t = strchr(file_name, ')');
          t[0] = '\000';
        } else
          file_name = oyjlStringCopy( prof_name, 0 );
        error = oyDeviceSetProfile( c, scope, file_name );
        if(error)
          fprintf( stdout, "profile assignment failed\n" );
        error = oyDeviceUnset( c );
        free(file_name);
      }

      error = oyOptions_SetFromInt( &options,
                                    "//" OY_TYPE_STD "/icc_profile_flags",
                                    flags, 0, OY_CREATE_NEW );
      error = oyOptions_SetFromString( &options,
                                      "//"OY_TYPE_STD"/config/skip_ask_for_profile", "yes", OY_CREATE_NEW );
      error = oyDeviceSetup( c, options );
    }
    else if(assign)
    {
      return 1;
    }

    if(unset || erase)
      oyDeviceUnset( c );

    if(setup)
    {
      error = oyOptions_SetFromInt( &options,
                                    "//" OY_TYPE_STD "/icc_profile_flags",
                                    flags, 0, OY_CREATE_NEW );
      error = oyOptions_SetFromString( &options,
                                      "//"OY_TYPE_STD"/config/skip_ask_for_profile", "yes", OY_CREATE_NEW );
      oyDeviceSetup( c, options );
    }

    if(erase)
      oyConfig_EraseFromDB( c, scope );

    oyConfig_Release( &c );
    return 0;

  } else
  if((list_profiles || list_taxi_profiles) && (c || (device_class && device_name)))
  {
    oyProfile_s * profile = 0;
    const char * tmp = 0;
    icSignature profile_class = icSigDisplayClass;
    oyOptions_s * options = 0;

    {
      oyConfDomain_s * d = oyConfDomain_FromReg( device_class, 0 );
      const char * icc_profile_class = oyConfDomain_GetText( d,
                                             "icc_profile_class", oyNAME_NICK );
      if(icc_profile_class && strcmp(icc_profile_class,"display") == 0)
        profile_class = icSigDisplayClass;
      else if(icc_profile_class && strcmp(icc_profile_class,"output") == 0)
        profile_class = icSigOutputClass;
      else if(icc_profile_class && strcmp(icc_profile_class,"input") == 0)
        profile_class = icSigInputClass;
      else if(strcmp(device_class,"monitor") == 0)
        profile_class = icSigDisplayClass;
      else if(strcmp(device_class,"printer") == 0)
        profile_class = icSigOutputClass;
      else if(strcmp(device_class,"camera") == 0)
        profile_class = icSigInputClass;
      else if(strcmp(device_class,"scanner") == 0)
        profile_class = icSigInputClass;

      if(verbose)
        fprintf( stderr, "icc_profile_class: %s\n", icc_profile_class ? icc_profile_class : device_class );
      oyConfDomain_Release( &d );
    }

    if(!device_json)
    {
      /* get all device informations from the module */
      oyConfig_Release( &c );
      if(!skip_x_color_region_target)
        oyOptions_SetFromString( &options,
                   "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target",
                         "yes", OY_CREATE_NEW );
      error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/command",
                                     "properties", OY_CREATE_NEW );  
      error = oyOptions_SetFromInt( &options,
                                    "//" OY_TYPE_STD "/icc_profile_flags",
                                    flags, 0, OY_CREATE_NEW );
      error = oyDeviceGet( 0, device_class, device_name, options, &c );
      oyOptions_Release( &options );
    }
    if(!c)
    {
      fprintf( stderr, "%s %s\n  device_class: \"%s\" device_name: \"%s\"  %s: %d\n", _("!!! ERROR"), _("Could not resolve device."),
               device_class?device_class:"????", device_name,
               _("Available devices"), n);
      
      return 1;
    }

    if(!skip_x_color_region_target)
    oyOptions_SetFromString( &options,
                   "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target",
                         "yes", OY_CREATE_NEW );
    error = oyOptions_SetFromInt( &options,
                                    "//" OY_TYPE_STD "/icc_profile_flags",
                                    flags, 0, OY_CREATE_NEW );

    error = oyDeviceGetProfile( c, options, &profile );

    if(profile)
      tmp = oyProfile_GetFileName( profile, -1 );

    if(verbose)
      fprintf( stderr, "%s %s %s%s%s\n",
               device_class, device_name, error?"":"good",
               tmp?"\nassigned profile: ":"", tmp?tmp:"" );

    oyProfile_Release( &profile );

    if(list_profiles)
    {
      char ** list = listProfiles( c, profile_class, device_class, flags, verbose, show_non_device_related, options, &n );
      int i;
      for(i = 0; i < n; ++i)
        puts( list[i] );
      oyjlStringListRelease( &list, n, free );

    } else if(list_taxi_profiles)
    {
      int size, i, current = -1, current_tmp = 0, pos = 0;
      oyProfile_s * profile = 0;
      oyConfigs_s * taxi_devices = 0;
      oyConfig_s * device = c;

      if(verbose)
        fprintf( stderr, "%s [%s] \"%s\"\n", _("Taxi ID"),
                 _("match value"), _("description") );

      oyDevicesFromTaxiDB( device, 0, &taxi_devices, 0 );

      size = oyConfigs_Count( taxi_devices );

      error = oyDeviceGetProfile( device, options, &profile );

      for( i = 0; i < size; ++i)
      {
        {
           oyConfig_s * taxi_dev = oyConfigs_Get( taxi_devices, i );
           int32_t rank = 0;
           error = oyDeviceCompare( device, taxi_dev, &rank );

           current_tmp = -1;

           if(current == -1 && current_tmp != -1)
             current = current_tmp;


           if(show_non_device_related == 1 ||
              rank > 0 ||
              current_tmp != -1)
           {
             fprintf(stdout, "%s/0 [%d] ", oyNoEmptyString_m_(
                    oyConfig_FindString(taxi_dev, "TAXI_id", 0)), rank);
             fprintf(stdout, "\"%s\"\n", oyNoEmptyString_m_(
                    oyConfig_FindString(taxi_dev, "TAXI_profile_description", 0)));
             ++pos;
           }

           oyConfig_Release( &taxi_dev );
        }  
      } 
      oyProfile_Release( &profile );                                                
      oyConfigs_Release( &taxi_devices );
      oyOptions_Release( &options );
    }

    oyConfig_Release( &c );
    return 0;


  } else if(format && c)
  {
    oyConfDomain_s * d = oyConfDomain_FromReg( device_class, 0 );
    char * json = 0;
    char * profile_name = 0;
    char * out_name = 0;
    oyjl_val rank_root = 0,
             rank_map = 0,
             device = 0;

    if(!device_json)
    {
      /* get all device informations from the module */
      if(!skip_x_color_region_target)
        oyOptions_SetFromString( &options,
                   "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target",
                         "yes", OY_CREATE_NEW );
      error = oyOptions_SetFromInt( &options,
                                    "//" OY_TYPE_STD "/icc_profile_flags",
                                    flags, 0, OY_CREATE_NEW );
      error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/command",
                                     "properties", OY_CREATE_NEW );  
      error = oyDeviceGet( 0, device_class, device_name, options, &dt );
      if(dt)
      {
        oyConfig_Release( &c );
        c = dt; dt = 0;
      }
      oyOptions_Release( &options );
    }
    if(!c)
    {
      fprintf( stderr, "%s %s\n  device_class: \"%s\" device_name: \"%s\"  %s: %d\n", _("!!! ERROR"), _("Could not resolve device."),
               device_class?device_class:"????", device_name,
               _("Available devices"), n);
      
      return 1;
    }

    /* query the full device information from DB */
    error = oyDeviceProfileFromDB( c, &profile_name, oyAllocFunc );
    if(profile_name) { oyDeAllocFunc( profile_name ); profile_name = 0; }

    if(strcmp(format,"openicc") == 0 ||
       strcmp(format,"openicc+rank-map") == 0 ||
       strcmp(format,"openicc-rank-map") == 0)
    {
      if(strcmp(format,"openicc") == 0 ||
         strcmp(format,"openicc+rank-map") == 0)
      {
        oyOptions_s * parsed = NULL;
        char * path = NULL;
        oyStringAddPrintf( &path, 0,0, "org/freedesktop/openicc/device/%s", device_class );
        error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/options/source",
                                   "db", OY_CREATE_NEW );  
        error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/key_path",
                                       path, OY_CREATE_NEW );  
        error = oyDeviceToJSON( c, options, &json, oyAllocFunc );
        if(oyOptions_FromJSON( json, options, &parsed, "org/freedesktop/openicc/device/%s/[0]", device_class ))
          WARNc1_S( "%s\n", _("found issues parsing JSON") );
        if(json) oyFree_m_( json );

        /* it is possible that no DB keys are available; use all others */
        if(!only_db)
        {
          error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/options/source",
                                         "backend_core", OY_CREATE_NEW );
          error = oyDeviceToJSON( c, options, &json, oyAllocFunc );
          if(oyOptions_FromJSON( json, options, &parsed, "org/freedesktop/openicc/device/%s/[0]", device_class ))
            WARNc1_S( "%s\n", _("found issues parsing JSON") );
          if(json) oyFree_m_( json );
        }
        oyOptions_Release( &options );

        json = oyStringCopy( oyOptions_GetText( parsed, oyNAME_JSON ), 0 );

        if(!json)
        {
          fprintf( stderr, "no DB data available\n" );
          return 0;
        }

        device = oyJsonParse( json, NULL );
      }
      if(strcmp(format,"openicc-rank-map") == 0 ||
         strcmp(format,"openicc+rank-map") == 0)
      {
        const oyRankMap * map = oyConfig_GetRankMap( c );
        oyConfDomain_s * domain = oyConfDomain_FromReg( device_class, 0 );
        const char * device_class = oyConfDomain_GetText( domain, "device_class", oyNAME_NICK );
        oyConfDomain_Release( &domain );

        if(!map)
        { fprintf( stderr, "no RankMap found\n" ); return 0;
        }

        error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/options/device_class",
                                       device_class, OY_CREATE_NEW );  
        oyRankMapToJSON( map, options, &json, oyAllocFunc );
        oyOptions_Release( &options );
        if(!json)
        { fprintf( stderr, "no JSON from RankMap available\n" ); return 0;
        }

        rank_root = oyJsonParse( json, NULL );
      }

      if(strcmp(format,"openicc+rank-map") == 0)
      {
        const char * xpath = "org/freedesktop/openicc/rank_map";

        /*oyjl_message_func_set( (oyjl_message_f)oyMessageFunc );*/

        rank_map = oyjlTreeGetValue( rank_root, 0, xpath );
        if(rank_map && rank_map->type == oyjl_t_object)
        {
          oyjl_val openicc = oyjlTreeGetValue( device, 0, "org/freedesktop/openicc" );
          /* copy the rank_map into the openicc node */
          if(openicc && openicc->type == oyjl_t_object)
          {
            oyjl_val * values;
            int level = 0;
            char ** keys;

            oyDeAllocFunc( json ); json = 0;
            oyjlTreeToJson( rank_map, &level, &json );
            rank_map = oyJsonParse( json, 0 );
            if(json){ free(json); json = 0; }

            keys = openicc->u.object.keys;
            values = openicc->u.object.values;

            openicc->u.object.keys = malloc( sizeof(char*) * openicc->u.object.len + 1 );
            openicc->u.object.values = malloc( sizeof(oyjl_val) * openicc->u.object.len + 1 );

            if(rank_map && rank_map->type == oyjl_t_object &&
               openicc->u.object.values && openicc->u.object.keys)
            {
              openicc->u.object.values[0] = rank_map;
              memcpy( &openicc->u.object.values[1], values, sizeof(oyjl_val) * openicc->u.object.len );
              openicc->u.object.keys[0] = oyStringCopy_("rank_map", oyAllocFunc);
              memcpy( &openicc->u.object.keys[1], keys, sizeof(char*) * openicc->u.object.len );

              ++openicc->u.object.len;

              /* level = 0; */
              oyjlTreeToJson( device, &level, &json );
            }
          }
        }

        oyjlTreeFree( rank_root ); rank_root = 0;
        oyjlTreeFree( device ); device = 0;
      }

      if(output)
        error = oyWriteMemToFile2_( output,
                                    json, strlen(json), 0x01,
                                    &out_name, oyAllocFunc );
      else
        fprintf( stdout, "%s", json );

      if(json)
        size = strlen(json);
      oyDeAllocFunc( json ); json = 0;

    } else
    if(strcmp(format,"icc") == 0 ||
       strcmp(format,"fallback-icc") == 0)
    {
      if(strcmp(format,"fallback-icc") == 0)
        oyOptions_SetFromString( &options,
                   "//"OY_TYPE_STD"/config/icc_profile.fallback",
                         "yes", OY_CREATE_NEW );
      if(!skip_x_color_region_target)
        oyOptions_SetFromString( &options,
                   "//"OY_TYPE_STD"/config/icc_profile.x_color_region_target",
                         "yes", OY_CREATE_NEW );
      error = oyOptions_SetFromInt( &options,
                                    "//" OY_TYPE_STD "/icc_profile_flags",
                                    flags, 0, OY_CREATE_NEW );
      error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "/config/command",
                                     "properties", OY_CREATE_NEW );  
      error = oyDeviceGet( 0, device_class, device_name, options, &dt );
      if(dt)
      {
        oyConfig_Release( &c );
        c = dt; dt = 0;
      }
      if(strcmp(format,"fallback-icc") == 0)
      {
        icHeader * header;
        oyOption_s * o;

        o = oyOptions_Find( *oyConfig_GetOptions(c, "data"),
                            "icc_profile.fallback", oyNAME_PATTERN );
        if( o )
        {
          prof = (oyProfile_s*) oyOption_GetStruct( o, oyOBJECT_PROFILE_S );
          oyOption_Release( &o );
        } else
          error = oyDeviceAskProfile2( c, options, &prof );

        if(prof)
        {
          uint32_t model_id = 0;
          const char * t = 0;
          error = oyProfile_AddTagText( prof, icSigProfileDescriptionTag,
                                        (char*) output );
          t = oyConfig_FindString( c, "manufacturer", 0 );
          if(t)
            error = oyProfile_AddTagText( prof, icSigDeviceMfgDescTag, t );
          t =  oyConfig_FindString( c, "model", 0 );
          if(t)
            error = oyProfile_AddTagText( prof, icSigDeviceModelDescTag, t);

          if(device_meta_tag)
          {
            oyOptions_s * opts = 0;
            t = oyConfig_FindString( c, "prefix", 0 );
            error = oyOptions_SetFromString( &opts, "///key_prefix_required",
                                                  t, OY_CREATE_NEW );
            oyProfile_AddDevice( prof, c, opts );
            oyOptions_Release( &opts );
          }

          data = oyProfile_GetMem( prof, &size, 0, oyAllocFunc );
          header = (icHeader*) data;
          t = oyConfig_FindString( c, "mnft", 0 );
          if(t)
            sprintf( (char*)&header->manufacturer, "%s", t );
          t = oyConfig_FindString( c, "model_id", 0 );
          if(t)
            model_id = atoi( t );
          model_id = oyValueUInt32( model_id );
          memcpy( &header->model, &model_id, 4 );
          oyOption_Release( &o );
        }
      } else /* strcmp(format,"icc") == 0 */
        oyDeviceAskProfile2( c, options, &prof );

      oyOptions_Release( &options );

      data = oyProfile_GetMem( prof, &size, 0, oyAllocFunc);
      if(size && data)
      {
        if(output)
          error = oyWriteMemToFile2_( output,
                                      data, size, 0x01,
                                      &out_name, oyAllocFunc );
        else
          fwrite( data, sizeof(char), size, stdout );

        oyDeAllocFunc( data ); data = 0;

      }
      oyOptions_Release( &options );

    } else {
      fprintf( stderr, "%s unsupported format: %s\n", _("!!! ERROR"), format );
      return 1;
    }

    if(verbose || (error && !size) || !size)
      fprintf( stderr, "  written %d bytes to %s\n", (int)size,
               out_name ? out_name : "stdout" );

    if(out_name){ oyDeAllocFunc(out_name); out_name = 0; }
    oyConfDomain_Release( &d );

    return 0;
  }


  /* This point should not be reached */
  return 1;

  /* device profile */
  if(0)
  {
    int i,n, pos = 0;
    oyProfileTag_s * tag_ = 0;
    oyConfig_s * oy_device = 0;
    oyOption_s * o = 0;
    char * text = 0, * name = 0;
    icSignature vs;
    char * v = 0;
    icTagTypeSignature texttype;
    oyProfile_s * p = 0;
    oyConfig_s * device;
    oyProfiles_s * p_list;
    int32_t * rank_list;

    error = oyDeviceGet( 0, device_class, device_name, 0, &oy_device );
    /* pick expensive informations */
    oyDeviceGetInfo( oy_device, oyNAME_DESCRIPTION, 0, &text, oyAllocateFunc_);
    oyDeAllocateFunc_( text );
    error = oyDeviceGetProfile( oy_device, 0, &p );

    vs = oyValueUInt32( oyProfile_GetSignature(p,oySIGNATURE_VERSION) );
    v = (char*)&vs;
    if(v[0] <= 2)
      texttype = icSigTextDescriptionType;
    else
      texttype = (icTagTypeSignature) icSigMultiLocalizedUnicodeType;

    n = oyOptions_Count( *oyConfig_GetOptions( oy_device,"backend_core") );

    for(i = 0; i < n; ++i)
    {
      o = oyOptions_Get( *oyConfig_GetOptions( oy_device,"backend_core"), i );

      text = oyOption_GetValueText( o, oyAllocateFunc_ );
      if(!text) continue;

      name = oyFilterRegistrationToText( oyOption_GetRegistration(o),
                                         oyFILTER_REG_MAX, oyAllocateFunc_ );
      if(strstr(name, "manufacturer"))
      {
        /* add a Manufacturer desc tag */
        tag_ = oyProfileTag_CreateFromText( text, texttype,
                                            icSigDeviceMfgDescTag, 0 );
        error = !tag_;
        if(tag_)
          error = oyProfile_TagMoveIn ( p, &tag_, -1 );

        oyDeAllocateFunc_( name );
        oyDeAllocateFunc_( text );
        continue;

      } else if(strstr(name, "model"))
      {

        /* add a Device Model desc tag */
        tag_ = oyProfileTag_CreateFromText( text, texttype,
                                            icSigDeviceModelDescTag, 0 );
        error = !tag_;
        if(tag_)
          error = oyProfile_TagMoveIn ( p, &tag_, -1 );

        oyDeAllocateFunc_( name );
        oyDeAllocateFunc_( text );
        continue;

      }

      oyDeAllocateFunc_( text );

      text = oyFilterRegistrationToText( oyOption_GetRegistration(o),
                                         oyFILTER_REG_MAX, oyAllocateFunc_ );
      oyDeAllocateFunc_( text );
      ++pos;
    }

    oyProfile_Release( &p );

    device = oyConfig_FromRegistration( "//" OY_TYPE_STD "/config", 0 );
    oyProfile_GetDevice( p, device );

    printf("following key/values are in the devices backend_core set:\n");
    n = oyOptions_Count( *oyConfig_GetOptions( oy_device,"backend_core") );
    for(i = 0; i < n; ++i)
    {
      o = oyOptions_Get( *oyConfig_GetOptions( oy_device,"backend_core"), i );

      text = oyOption_GetValueText( o, oyAllocateFunc_ );
      if(!text) continue;

      printf("%s: %s\n", oyOption_GetRegistration(o), text);

      oyOption_Release( &o );
    }

    printf("\ngoing to rank installed profiles according to the device[\"%s\",\"%s\"]:\n", device_class, device_name );
    p_list = oyProfiles_ForStd( oyASSUMED_RGB, flags, 0,0 );
    rank_list = (int32_t*) oyAllocateFunc_( oyProfiles_Count(p_list)
                                                        * sizeof(int32_t) );
    oyProfiles_DeviceRank( p_list, oy_device, rank_list );
    n = oyProfiles_Count( p_list );
    for(i = 0; i < n; ++i)
    {
      prof = oyProfiles_Get( p_list, i );
      printf("%d %d: \"%s\" %s\n", rank_list[i], i,
             oyProfile_GetText( prof, oyNAME_DESCRIPTION ),
             oyProfile_GetFileName(prof, 0));
      oyProfile_Release( &prof );
    }
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
#ifdef HAVE_LOCALE_H
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


