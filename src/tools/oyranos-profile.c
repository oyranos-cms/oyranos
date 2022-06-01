/** @file oyranos_profile.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2011-2022 (C) Kai-Uwe Behrmann
 *
 *  @brief    ICC profile informations - on the command line
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2011/06/24
 *
 *  The program informs about a ICC profile.
 */


#include "oyProfile_s.h"
#include "oyProfile_s_.h"           /* oyProfile_ToFile_() */
#include "oyProfileTag_s.h"
#include "oyStructList_s.h"

#include "oyranos.h"
#include "oyranos_debug.h"
#include "oyranos_devices.h"
#include "oyranos_helper.h"
#include "oyranos_icc.h"
#include "oyranos_internal.h"
#include "oyranos_io.h"
#include "oyranos_config.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyranos_version.h"
#include "oyProfiles_s.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define OPENICC_DEVICE_JSON_HEADER \
  "{\n" \
  "  \"org\": {\n" \
  "    \"freedesktop\": {\n" \
  "      \"openicc\": {\n" \
  "        \"device\": {\n" \
  "          \"%s\": [{\n"
#define OPENICC_DEVICE_JSON_FOOTER \
  "            }\n" \
  "          ]\n" \
  "        }\n" \
  "      }\n" \
  "    }\n" \
  "  }\n" \
  "}\n"
#define OPENICC_DEVICE_MONITOR "monitor"
#define OPENICC_DEVICE_SCANNER "scanner"
#define OPENICC_DEVICE_PRINTER "printer"
#define OPENICC_DEVICE_CAMERA  "camera"

#include "oyjl.h"
#include "oyjl_macros.h"
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

static oyjlOptionChoice_s * listProfiles ( oyjlOption_s * x OYJL_UNUSED, int * y OYJL_UNUSED, oyjlOptions_s * opts )
{
  OYJL_GET_RESULT_STRING( opts, "@", NULL, profile_name );
  oyjlOptionChoice_s * cs = NULL;
  uint32_t i = 0;
  if(profile_name && strcmp(profile_name,"oyjl-list") == 0)
  {
    uint32_t n = 0;
    char ** profile_names = oyProfileListGet(0, &n, malloc);
    cs = (oyjlOptionChoice_s*) calloc( (unsigned int)n+20, sizeof(oyjlOptionChoice_s) );
    for(i = 0; i < n; ++i)
    {
      const char * fn = profile_names[i];
      if(fn)
        cs[i].nick = oyjlStringCopy( fn, 0 );
    }
    oyjlStringListRelease( &profile_names, n, free );

  } else
  {
    oyProfiles_s * ps = oyProfiles_Create( NULL, 0, 0 );
    int n = oyProfiles_Count( ps );
    cs = (oyjlOptionChoice_s*) calloc( (unsigned int)n+20, sizeof(oyjlOptionChoice_s) );
    for(i = 0; (int)i < n; ++i)
    {
      oyProfile_s * p = oyProfiles_Get(ps, i);
      const char * desc = oyProfile_GetText(p, oyNAME_DESCRIPTION);
      const char * fn = oyProfile_GetFileName(p, -1);
      if(desc)
        cs[i].name = oyjlStringCopy( desc, 0 );
      if(fn)
        cs[i].nick = oyjlStringCopy( fn, 0 );
      oyProfile_Release( &p );
    }
    oyProfiles_Release( &ps );
  }

  cs[i].nick = cs[i].name = "rgb"; ++i;
  cs[i].nick = cs[i].name = "cmyk"; ++i;
  cs[i].nick = cs[i].name = "gray"; ++i;
  cs[i].nick = cs[i].name = "lab"; ++i;
  cs[i].nick = cs[i].name = "xyz"; ++i;
  cs[i].nick = cs[i].name = "web"; ++i;
  cs[i].nick = cs[i].name = "effect"; ++i;
  cs[i].nick = cs[i].name = "proof"; ++i;

  return cs;
}

oyjlOptionChoice_s * listTagPos                 ( oyjlOption_s      * o OYJL_UNUSED,
                                                  int               * selected OYJL_UNUSED,
                                                  oyjlOptions_s     * opts )
{
  oyjlOptionChoice_s * c = NULL;
  int i,
      choices = 0;
  oyProfile_s * p = NULL;
  OYJL_GET_RESULT_STRING( opts, "@", NULL, profile_name );
  fputs( "listTagPos\n", stderr );
  if(!profile_name) return c;

  p = oyProfile_FromName( profile_name, 0,0 );
  choices = oyProfile_GetTagCount( p );
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
  /*if(selected)
    *selected = current;*/

  oyProfile_Release( &p );

  return c;
}

oyjlOptionChoice_s * listTagName                ( oyjlOption_s      * o OYJL_UNUSED,
                                                  int               * selected OYJL_UNUSED,
                                                  oyjlOptions_s     * opts )
{
  oyjlOptionChoice_s * c = NULL;
  int i,
      choices = 0;
  oyProfile_s * p = NULL;
  OYJL_GET_RESULT_STRING( opts, "@", NULL, profile_name );
  if(!profile_name) return c;

  p = oyProfile_FromName( profile_name, 0,0 );
  choices = oyProfile_GetTagCount( p );
  c = calloc(choices+1, sizeof(oyjlOptionChoice_s));

  if(c)
  {
    for(i = 0; i < choices; ++i)
    {
      char * v;
      oyProfileTag_s * tag = oyProfile_GetTagByPos( p, i );
      icTagSignature ts = oyProfileTag_GetUse( tag );
      const char * tn = oyICCTagName( ts );

      v = oyjlStringCopy( tn, 0 );

      c[i].nick = v;
      c[i].name = strdup(v);
      c[i].description = strdup("");
      c[i].help = strdup("");

      oyProfileTag_Release( &tag );
    }
  }
  /*if(selected)
    *selected = current;*/

  oyProfile_Release( &p );

  return c;
}

oyjlOptionChoice_s * listPpmcie                 ( oyjlOption_s      * o OYJL_UNUSED,
                                                  int               * selected OYJL_UNUSED,
                                                  oyjlOptions_s     * opts )
{
  oyjlOptionChoice_s * c = NULL;
  int can_ppmcie = 0, has_xyz = 0, i, choices;
  oyProfile_s * p = NULL;
  oyProfileTag_s * tags[4] = {0,0,0,0};
  OYJL_GET_RESULT_INT( opts, "v", 0, verbose );
  OYJL_GET_RESULT_STRING( opts, "@", NULL, profile_name );
  if(verbose)
  {
    char * t = oyBT(0);
    fprintf( stderr, "%slistPpmcie\n", t );
    free(t);
  }
  if(!profile_name) return c;

  p = oyProfile_FromName( profile_name, 0,0 );
  tags[0] = oyProfile_GetTagById( p, icSigRedColorantTag );
  tags[1] = oyProfile_GetTagById( p, icSigGreenColorantTag );
  tags[2] = oyProfile_GetTagById( p, icSigBlueColorantTag );
  tags[3] = oyProfile_GetTagById( p, icSigMediaWhitePointTag );
  if(tags[0] && tags[1] && tags[2] && tags[3])
    ++has_xyz;
  if(has_xyz && oyjlHasApplication("ppmcie") && oyjlHasApplication("pamtopng"))
    ++can_ppmcie;
  choices = can_ppmcie + has_xyz;
  if(choices)
    c = calloc(choices+1, sizeof(oyjlOptionChoice_s));

  if(c)
  {
    i = 0;
    c[i].nick = strdup("TEXT");
    c[i].name = strdup(_("TEXT"));
    c[i].description = strdup("");
    c[i].help = strdup("");
    if(can_ppmcie)
    {
      ++i;
      c[i].nick = strdup("PNG");
      c[i].name = strdup(_("PNG"));
      c[i].description = strdup("");
      c[i].help = strdup("");
    }
  }

  oyProfile_Release( &p );
  for(i = 0; i < 4; ++i)
    oyProfileTag_Release( &tags[i] );

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
  int path = 0;
  int short_var = 0;
  const char * file_name = NULL,
             * profile_desc = 0;
  int list_tags = 0;
  int tag_pos = -1;
  const char * tag_name = 0;
  int remove_tag = 0;
  int list_hash = 0;
  const char * profile_name = 0;
  const char * ppmcie = NULL;
  int verbose = 0;
  const char * dump_openicc_json = 0;
  const char * output = NULL;
  const char * device_class = 0;
  const char * format = 0;
  const char * json_name = 0;
  const char * name_space = 0;
  int icc_version_2 = 0;
  int icc_version_4 = 0;
  int help = 0;
  int version = 0;
  const char * render = NULL;
  const char * export = 0;

#if defined(__ANDROID__)
  oy_debug = 2;
  fprintf(stderr, OYJL_DBG_FORMAT "%s\n", OYJL_DBG_ARGS, argv[0] );
#endif

  /* handle options */
  /* declare the option choices  *   nick,          name,               description,                  help */
  oyjlOptionChoice_s E_choices[] = {{"OY_DEBUG", _("set the Oyranos debug level. Alternatively the -v option can be used."),NULL,                         NULL},
                                    {"XDG_DATA_HOME XDG_DATA_DIRS",_("route Oyranos to top directories containing resources. The derived paths for ICC profiles have a \"color/icc\" appended."),_("http://www.oyranos.com/wiki/index.php?title=OpenIccDirectoryProposal"),NULL},
                                    {NULL,NULL,NULL,NULL}};

  oyjlOptionChoice_s A_choices[] = {{"",_("Show overview and header of profile"),_("oyranos-profile sRGB.icc"),                         NULL},
                                    {"",_("Show first tags content of profile"),_("oyranos-profile -lv -p=1 sRGB.icc"),                         NULL},
                                    {"",_("Show the profile hash sum"),_("oyranos-profile -m sRGB.icc"),                         NULL},
                                    {"",_("Show the RGB primaries of a matrix profile inside a CIE*xy diagram"),_("ppmcie `oyranos-profile --ppmcie sRGB.icc` > sRGB_cie-xy.ppm"),NULL},
                                    {"",_("Add calibration data to meta tag of a device profile"),_("oyranos-profile -w my_profile -j my_device.json my_profile.icc"),NULL},
                                    {"",_("Pass the profile to a external tool"),_("iccdump \"`oyranos-profile --path cmyk`\""),NULL},
                                    {NULL,NULL,NULL,NULL}};

  oyjlOptionChoice_s S_choices[] = {{_("oyranos-profiles(1) oyranos-profile-graph(1) oyranos-config-fltk(1) oyranos-config(1) oyranos(3) ppmcie(1)"),NULL,               NULL,                         NULL},
                                    {"http://www.oyranos.org",NULL,               NULL,                         NULL},
                                    {NULL,NULL,NULL,NULL}};

  /* declare options - the core information; use previously declared choices */
  oyjlOption_s oarray[] = {
  /* type,   flags,                      o,  option,          key,      name,          description,                  help, value_name,         
        value_type,              values,             variable_type, variable_name */
    {"oiwi", 0,                          NULL,"path",         NULL,     _("Path"),     _("show the full ICC profile path and file name"),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&path}},
    {"oiwi", 0,                          NULL,"short",        NULL,     _("Short"),    _("show only the ICC profiles file name"),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&short_var}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "@", NULL,           NULL,     _("Input"),    _("ICC Profile"),             _("can  be  file  name,  internal  description  string,  ICC profile ID or wildcard \"rgb\", \"cmyk\", \"gray\", \"lab\", \"xyz\", \"web\", \"rgbi\", \"cmyki\", \"grayi\", \"labi\", \"xyzi\".  Wildcards ending with \"i\" are assumed profiles. \"web\" is a sRGB profile. The other wildcards are editing profiles."), "l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE",
        oyjlOPTIONTYPE_FUNCTION, {.getChoices = listProfiles}, oyjlSTRING,    {.s=&file_name}},
    {"oiwi", 0,                          "l","list-tags",     NULL,     _("List Tags"),_("list contained tags additional to overview and header."),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&list_tags}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "p","tag-pos",       NULL,     _("Tag Pos"),  _("select tag"),              NULL, _("NUMBER"),        
        oyjlOPTIONTYPE_FUNCTION, {.getChoices = listTagPos},   oyjlINT,       {.i=&tag_pos}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "n","tag-name",      NULL,     _("Tag Name"), _("select tag"),              NULL, _("NAME"),          
        oyjlOPTIONTYPE_FUNCTION, {.getChoices = listTagName},  oyjlSTRING,    {.s=&tag_name}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "r","remove-tag",    NULL,     _("Remove Tag"),NULL,                         _("remove selected tag number."),_("NUMBER"),        
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlINT,       {.i=&remove_tag}},
    {"oiwi", 0,                          "m","list-hash",     NULL,     _("List Hash"),_("show internal hash value."),NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&list_hash}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "w","profile-name",  NULL,     _("Profile Name"),_("write profile with correct ID hash"),_("The -w option specifies the new internal and external profile name. PROFILENAME specifies the source profile."),_("ICC_FILE_NAME"),
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&profile_name}},
    {"oiwi", OYJL_OPTION_FLAG_ACCEPT_NO_ARG,NULL,"ppmcie",    NULL,     _("Ppmcie"), _("show CIE*xy chromaticities, if available, for use with ppmcie."), NULL, _("FORMAT"),          
        oyjlOPTIONTYPE_FUNCTION, {.getChoices = listPpmcie},  oyjlSTRING,    {.s=&ppmcie}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "o","output",        NULL,     _("Dump Openicc Json"),NULL,                 _("write device informations to OpenICC JSON."),_("FILENAME"),      
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&output}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "c","device-class",  NULL,     _("Device Class"),_("use device class. Useful device classes are monitor, scanner, printer, camera."),NULL, _("CLASS"),         
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&device_class}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "f","format",        NULL,     _("Format"),   _("use IccXML format"),       NULL, _("xml"),        
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&format}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "j","json-name",     NULL,     _("Json Name"),_("embed OpenICC JSON device from file"),NULL, _("FILENAME"),      
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&json_name}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "s","name-space",    NULL,     _("Name Space"),_("add prefix"),              NULL, _("NAME"),          
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&name_space}},
    {"oiwi", 0,                          "2","icc-version-2", NULL,     _("Icc Version 2"),_("Select ICC v2 Profiles"),  NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&icc_version_2}},
    {"oiwi", 0,                          "4","icc-version-4", NULL,     _("Icc Version 4"),_("Select ICC v4 Profiles"),  NULL, NULL,               
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&icc_version_4}},
    {"oiwi", 0,                          "E","man-environment",NULL,    NULL,          NULL,                         NULL, NULL,               
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)E_choices, sizeof(E_choices), malloc ), 0}}, oyjlNONE,      {}},
    {"oiwi", 0,                          "A","man-examples",  NULL,     NULL,          NULL,                         NULL, NULL,               
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)A_choices, sizeof(A_choices), malloc ), 0}}, oyjlNONE,      {}},
    {"oiwi", 0,                          "S","man-see_also",  NULL,     NULL,          NULL,                         NULL, NULL,               
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)S_choices, sizeof(S_choices), malloc ), 0}}, oyjlNONE,      {}},
    /* default options -h and -v */
    {"oiwi", OYJL_OPTION_FLAG_ACCEPT_NO_ARG, "h", "help",NULL,NULL,NULL,NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&help} },
    {"oiwi", 0, NULL,"synopsis",NULL, NULL,         NULL,         NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlNONE, {0} },
    {"oiwi", 0, "v", "verbose", NULL, _("verbose"), _("verbose"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&verbose} },
    {"oiwi", 0, "V", "version", NULL, _("version"), _("Version"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&version} },
    /* default option template -X|--export */
    {"oiwi", 0, "X", "export", NULL, NULL, NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices = {NULL, 0}}, oyjlSTRING, {.s=&export} },
    /* The --render option can be hidden and used only internally. */
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE|OYJL_OPTION_FLAG_MAINTENANCE, "R", "render", NULL, NULL,  NULL,  NULL, NULL, oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s=&render} },
    {"",0,0,NULL,NULL,NULL,NULL,NULL, NULL, oyjlOPTIONTYPE_END, {0},0,{0}}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups[] = {
  /* type,   flags, name,               description,                  help,               mandatory,     optional,      detail */
    {"oiwg", 0,     NULL,               _("Show ICC Profile"),        NULL,               "@",           "path,short,v",  "@,path,short"},
    {"oiwg", 0,     NULL,               _("List included ICC tags"),  NULL,               "l,@",         "p,n,v",         "l,p,n"},
    {"oiwg", 0,     NULL,               _("Remove included ICC tag"), NULL,               "r,@",         NULL,          "r"},
    {"oiwg", 0,     NULL,               _("Show Profile ID"),         NULL,               "m,@",         "w",           "m,w"},
    {"oiwg", 0,     NULL,               _("Show CIE*xy chromaticities"),NULL,             "ppmcie,@",    "v",           "ppmcie"},
    {"oiwg", 0,     NULL,               _("Dump Device Infos to OpenICC device JSON"),NULL,"o,@",        "c,f",         "o,c,f"},
    {"oiwg", 0,     NULL,               _("Write to ICC profile"),    NULL,               "w,@",         "j|m,s,2,4",   "w,j|m,s,2,4"},
    {"oiwg", OYJL_GROUP_FLAG_GENERAL_OPTS, NULL, _("General options"),NULL,               "h|X|V|R",     "v",           "h,X,V,R,v"},
    {"",0,0,0,0,0,0,0}
  };

  oyjlUiHeaderSection_s * sections = oyUiInfo(_("The oyranos-profile programm shows informations about a ICC profile and allows some modifications."),
                  "2015-03-06T12:00:00", "March 06, 2015");
  oyjlUi_s * ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oyranos-profile", _("Oyranos Profile"), _("The Tool gives information of a ICC color profile."),
#ifdef __ANDROID__
                                       ":/images/oyranos-logo", // use qrc
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
    fprintf( stderr, "%s\n\tman oyranos-profile\n\n", _("For more information read the man page:") );
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
  \"command_set\": \"oyranos-profile\",\n\
  \"comment\": \"command_set_delimiter - build key:value; default is '=' key=value\",\n\
  \"comment\": \"command_set_option - use \\\"-s\\\" \\\"key\\\"; skip \\\"--\\\" direct in front of key\",\n\
  \"command_get\": \"oyranos-profile\",\n\
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
    int read_stdin = 0;
    oyProfile_s * p = NULL;
    oyProfileTag_s * tag = NULL;
    int simple = 0;
    int flags = OY_NO_REPAIR,
        dump_chromaticities = 0;
    const char * prefixes[24] = {0}; int pn = 0;
    const char * device_class = "unknown";

    dump_openicc_json = output;
    if(ppmcie)
    {
      if(strcasecmp(ppmcie,"TEXT") == 0)
        dump_chromaticities = 1;
      else
      if(strcasecmp(ppmcie,"PNG") == 0)
        dump_chromaticities = 2;
      else if(strcmp(ppmcie,"1") == 0)
        dump_chromaticities = 1;
      else
      {
        oyjlOptionChoice_s * choices = listPpmcie( NULL, NULL, ui->opts );
        int i, n = oyjlOptionChoice_Count( choices );
        if(strcmp(ppmcie,"oyjl-list") == 0)
        {
          for(i = 0; i < n; ++i)
            printf( "%s\n", choices[i].nick );
          return 0;
        } 
        else
        {
          fprintf(stderr, "%s %s", oyjlTermColor(oyjlRED, _("Usage Error")), _("Allowed option choices are:"));
          for(i = 0; i < n; ++i)
            fprintf(stderr, " --ppmcie=%s", choices[i].nick );
          fprintf(stderr, " %s\n", _("Exit!"));
          return 1;
        }
      }
    }
    if(path)
      simple = 2;
    if(short_var)
      simple = 1;
    if(remove_tag)
    {
      if(tag_pos != -1)
        remove_tag = tag_pos;
      else
        tag_pos = remove_tag;
    }

  if(file_name && strcmp(file_name,"-") == 0)
    read_stdin = 1;
  if(read_stdin)
  {
    size_t size = 0;
    void * data = oyReadStdinToMem_(&size, oyAllocateFunc_);
    if(size > 128 && oyCheckProfileMem( data, size, 0 ) == 0)
    {
      fprintf(stderr, "%s: %s\n", _("read input stream"), _("ok"));
      p = oyProfile_FromMem( size, data, 0, 0 );
    } else
    {
      fprintf(stderr, "%s: %s %s\n", _("read input stream"), _("failed!"), _("Exit!"));
      return 1;
    }
    oyFree_m_( data );
  } else if(file_name)
    p = oyProfile_FromName( file_name, (verbose?OY_COMPUTE:0) | flags, 0 );

  if(p)
  {
    if(file_name)
      profile_desc = file_name;
    else
      profile_desc = oyProfile_GetText( p, oyNAME_DESCRIPTION );
  }

  if((profile_name || list_hash || list_tags || remove_tag || dump_chromaticities || dump_openicc_json) && !p)
  {
    fprintf(stderr, "%s %s %s\n", oyjlTermColor(oyjlRED, _("Usage Error")), _("Need a ICC profile argument."), _("Exit!"));
    return 1;
  }

  if(p && profile_name)
  {
    oyConfig_s * device;
    oyOptions_s * opts = NULL;
    char * json_text = NULL;
    char * data = 0;
    size_t size = 0;
    char * pn = 0;
    char * ext = 0;
    const char * t = strrchr(profile_name, '.');
    int i;

    if(remove_tag)
    {
      uint32_t id[4];
      printf("tags: %d\n", oyProfile_GetTagCount( p ));
      error = oyProfile_TagReleaseAt( p, tag_pos );
      printf("tags: %d\n", oyProfile_GetTagCount( p ));
      oyProfile_GetMD5( p, OY_COMPUTE, id );
    }

    if(error <= 0 && list_hash)
    {
      uint32_t id[4];
      oyProfile_GetMD5( p, OY_COMPUTE, id );

      fprintf( stderr, "%s %s\n", _("Write to ICC profile:"), profile_name);
      error = oyProfile_ToFile_( (oyProfile_s_*)p, profile_name);
      return error;
    }

    STRING_ADD( pn, profile_name );
    if(t)
    {
      ++t;
      STRING_ADD( ext, t );
      i = 0;
      while(ext[i])
      {
        ext[i] = tolower(ext[i]);
        ++i;
      }
      if(strcmp(ext,"icc") != 0 &&
         strcmp(ext,"icm") != 0)
      {
        oyFree_m_(ext);
        ext = 0;
      }
    }
    if(!ext)
      STRING_ADD( pn, ".icc" );
    else
      oyFree_m_(ext);

    {
      oyProfile_s * test = oyProfile_FromName( pn, (verbose?OY_COMPUTE:0) | flags, 0 );
      if(test)
      {
        fprintf(stderr, "%s: \"%s\" - %s\n", _("Profile exists already"), pn, _("Exit!"));
        return 1;
      }
    }

    if(!p)
      return 1;

    error = oyProfile_AddTagText( p, icSigProfileDescriptionTag, profile_name );

    if(json_name)
    {
      size_t json_size = 0;
      json_text = oyReadFileToMem_( json_name, &json_size, oyAllocateFunc_ );
      oyDeviceFromJSON( json_text, NULL, &device );
      error = oyOptions_SetFromString( &opts, "///set_device_attributes",
                                     "true", OY_CREATE_NEW );
      oyProfile_AddDevice( p, device, opts );
      oyOptions_Release( &opts );
      oyConfig_Release( &device );
      oyDeAllocateFunc_(json_text);
    }
      
    data = oyProfile_GetMem( p, &size, 0, oyAllocateFunc_ );

    if(data && size)
    {
      uint32_t id[4];

      oyFree_m_(data);

      oyProfile_GetMD5( p, OY_COMPUTE, id );
      oyProfile_ToFile_( (oyProfile_s_*)p, pn );
      oyFree_m_( pn );
    }

    oyProfile_Release( &p );

  } else
  if(p)
  {
    char ** texts = NULL;
    int32_t texts_n = 0, i,j,k, count;

    if(simple)
    {
      const char * filename = oyProfile_GetFileName( p, -1 );
      char * report = NULL;
      oyStringAddPrintf( &report, 0,0,
                         "%s", filename ? (simple == 1)?(strrchr(filename,OY_SLASH_C) ? strrchr(filename,OY_SLASH_C)+1:filename):filename : OY_PROFILE_NONE );
      fprintf( stdout, "%s\n", report );
      return 0;
    }

    /* print header infos */
    if(error <= 0 && !dump_chromaticities && !dump_openicc_json && !list_hash && !list_tags)
    if(tag_name == NULL && tag_pos == -1)
    {
      uint32_t id[4],
               sig;
      uint8_t * f = (uint8_t*) &sig;
      size_t size = 0;
      icSignature vs = oyValueUInt32( oyProfile_GetSignature(p,oySIGNATURE_VERSION) );
      char * v = (char*)&vs;
      const char   ** names = NULL;
      int count;
      int32_t text_n;
      char** tag_text = 0;
      const char * fn;
      oyProfileTag_s * tag;

      /* keep total number of chars equal to original for cli print */
      fprintf( stdout, "%s %s\n",_("Description:     "), oyProfile_GetText(p,oyNAME_DESCRIPTION) );
      fn = oyProfile_GetFileName(p,-1);
      fprintf( stdout, "%s %s\n",_("File:            "), fn?fn:"----" );

      size = oyProfile_GetSize( p, 0 );
      /* keep total number of chars equal to original for cli print */
      fprintf( stdout, "%s %lu bytes\n",_("Size:            "), (long unsigned int)size );

      tag = oyProfile_GetTagById( p, icSigCopyrightTag );
      tag_text = oyProfileTag_GetText( tag, &text_n, 0,0, 0, 0 );
      /* keep total number of chars equal to original for cli print */
      if(text_n)
      fprintf( stdout, "%s %s\n",_("Copyright:       "), tag_text[0] );
      oyProfileTag_Release( &tag );

      fprintf( stdout, "\n" );
      fprintf( stdout, "%s\n",_("Header") );
      fprintf( stdout, "------\n" );

      sig = oyProfile_GetSignature(p,oySIGNATURE_SIZE);
      /* keep total number of chars equal to original for cli print */
      fprintf( stdout, "%s %u bytes\n",_("Size:            "), (unsigned)sig );

      sig = oyProfile_GetSignature(p,oySIGNATURE_CMM);
      /* keep total number of chars equal to original for cli print */
      fprintf( stdout, "%s %s\n",_("Cmm:             "),
               oyICCCmmDescription(sig));

      /* keep total number of chars equal to original for cli print */
      fprintf( stdout, "%s %d.%d.%d\n",_("Version:         "),
             (int)v[0], (int)v[1]/16, (int)v[1]%16 );

      /* keep total number of chars equal to original for cli print */
      fprintf( stdout, "%s %s\n",_("Type:            "),
             oyICCDeviceClassDescription( (icProfileClassSignature)
                         oyProfile_GetSignature(p,oySIGNATURE_CLASS) ) );

      /* keep total number of chars equal to original for cli print */
      fprintf( stdout, "%s %s\n",_("Data Color Space:"),
             oyICCColorSpaceGetName( (icColorSpaceSignature)
                         oyProfile_GetSignature(p,oySIGNATURE_COLOR_SPACE) ) );

      /* keep total number of chars equal to original for cli print */
      fprintf( stdout, "%s %s\n",_("PCS Color Space: "),
             oyICCColorSpaceGetName( (icColorSpaceSignature)
                         oyProfile_GetSignature(p,oySIGNATURE_PCS) ) );

      /* keep total number of chars equal to original for cli print */
      fprintf( stdout, "%s %d-%d-%d %d:%d:%d\n",_("Creation Date:   "),
             oyProfile_GetSignature(p,oySIGNATURE_DATETIME_YEAR),
             oyProfile_GetSignature(p,oySIGNATURE_DATETIME_MONTH),
             oyProfile_GetSignature(p,oySIGNATURE_DATETIME_DAY),
             oyProfile_GetSignature(p,oySIGNATURE_DATETIME_HOURS),
             oyProfile_GetSignature(p,oySIGNATURE_DATETIME_MINUTES),
             oyProfile_GetSignature(p,oySIGNATURE_DATETIME_SECONDS) );

      sig = oyValueUInt32(oyProfile_GetSignature(p,oySIGNATURE_MAGIC));
      /* keep total number of chars equal to original for cli print */
      fprintf( stdout, "%s %c%c%c%c\n",_("Magic:           "),
               f[0]?f[0]:' ',f[1]?f[1]:' ',f[2]?f[2]:' ',f[3]?f[3]:' ');

      /* keep total number of chars equal to original for cli print */
      fprintf( stdout, "%s %s\n",_("Platform:        "),
             oyICCPlatformDescription( (icPlatformSignature)
                         oyProfile_GetSignature(p,oySIGNATURE_PLATFORM) ) );

      sig = oyValueUInt32(oyProfile_GetSignature(p,oySIGNATURE_OPTIONS));
      /* keep total number of chars equal to original for cli print */
      fprintf( stdout, "%s %s | %s\n",_("Flags:           "),
               f[0]&0x80?"Profile is embedded":"Profile is not embedded",
               f[0]&0x40 ? "Use not Anywhere" : "Use Anywhere" );

      sig = oyValueUInt32(oyProfile_GetSignature(p,oySIGNATURE_MANUFACTURER));
      /* keep total number of chars equal to original for cli print */
      fprintf( stdout, "%s %c%c%c%c\n",_("Manufacturer:    "),
               f[0]?f[0]:' ',f[1]?f[1]:' ',f[2]?f[2]:' ',f[3]?f[3]:' ');

      sig = oyValueUInt32(oyProfile_GetSignature(p,oySIGNATURE_MODEL));
      /* keep total number of chars equal to original for cli print */
      fprintf( stdout, "%s %c%c%c%c\n", _("Model:           "),
               f[0]?f[0]:' ',f[1]?f[1]:' ',f[2]?f[2]:' ',f[3]?f[3]:' ');

      sig = oyValueUInt32(oyProfile_GetSignature(p,oySIGNATURE_ATTRIBUTES));
      /* keep total number of chars equal to original for cli print */
      fprintf( stdout, "%s %s | %s | %s | %s\n",_("Attributes:      "),
               f[0]&0x80?"Transparent":"Reflective", f[0]&0x40 ? "Matte" : "Glossy",
               f[0]&0x20?"Negative":"Positive", f[0]&0x10 ? "BW" : "Color" );

      oyOptionChoicesGet( oyWIDGET_RENDERING_INTENT, &count, &names, 0 );
      j = oyProfile_GetSignature(p,oySIGNATURE_INTENT);
      /* keep total number of chars equal to original for cli print */
      fprintf( stdout, "%s %s\n", _("Rendering Intent:"),
               (j < count && names && names[j]) ? names[j] : _("unknown"));

      /* keep total number of chars equal to original for cli print */
      fprintf( stdout, "%s %f %f %f\n", _("Illuminant:      "),
             (float)oyProfile_GetSignature(p,oySIGNATURE_ILLUMINANT)/65535.0,
             (float)oyProfile_GetSignature(p,oySIGNATURE_ILLUMINANT_Y)/65535.0,
             (float)oyProfile_GetSignature(p,oySIGNATURE_ILLUMINANT_Z)/65535.0 );
      sig = oyValueUInt32(oyProfile_GetSignature(p,oySIGNATURE_CREATOR));
      fprintf( stdout, "%s %c%c%c%c\n", _("Creator:         "),
               f[0]?f[0]:' ',f[1]?f[1]:' ',f[2]?f[2]:' ',f[3]?f[3]:' ');

      oyProfile_GetMD5(p, OY_FROM_PROFILE, id);
      /* keep total number of chars equal to original for cli print */
      fprintf( stdout, "%s %08x%08x%08x%08x\n",_("Profile ID:      "),id[0],id[1],id[2],id[3] );
    }

    if(error <= 0 && list_tags)
    {
      FILE * out = stdout;

      if(verbose && (tag_name || tag_pos != -1))
        out = stderr;
      fprintf( out, "\n" );
      fprintf( out, "%s\n", _("Profile Tags") );
      fprintf( out, "------------\n" );
      count = oyProfile_GetTagCount( p );
      for(i = 0; i < count; ++i)
      {
        tag = oyProfile_GetTagByPos( p, i );

        if(tag &&
           ((tag_name == NULL && (tag_pos == -1 || tag_pos == i)) ||
            (tag_name != NULL && (strcmp(oyICCTagName(oyProfileTag_GetUse(tag)),
                                         tag_name) == 0)))
          )
        {
          int32_t tag_size = 0;
          texts = oyProfileTag_GetText( tag, &texts_n, NULL, NULL,
                                        &tag_size, malloc );

          fprintf( out, "%s/%s[%d]\t%d\t@ %d\t%s",
                   oyICCTagName(oyProfileTag_GetUse(tag)),
                   oyICCTagTypeName(oyProfileTag_GetType(tag)), i,
                   (int)tag_size, (int)oyProfileTag_GetOffset( tag ),
                   oyICCTagDescription(oyProfileTag_GetUse(tag)));
          if((verbose || oy_debug) && texts)
          {
            fprintf( out, ":\n" );
              for(j = 0; j < texts_n; ++j)
                if(texts[j])
                {
                  fprintf( stdout, "%s", texts[j] );
                  if(!(tag_name && texts_n == 1))
                    fprintf( stdout, "\n" );
                }
          } else
          {
            fprintf( out, "\n" );
          }
        }

        oyProfileTag_Release( &tag );
      }
    } else
    if( error <= 0 && dump_openicc_json )
    {
      FILE * fp = stdout;
      oyjl_val root = oyjlTreeNew("");
      char * json = NULL;
      if(strcmp( dump_openicc_json, "-" ) != 0)
        fp = fopen( dump_openicc_json, "wb" );
      tag = oyProfile_GetTagById( p, icSigMetaDataTag );
      if(tag)
      {
        int32_t tag_size = 0;
        int size = 0;
        int has_prefix = 0;
        char * prefix = NULL;

        if(!format) format = "openicc";
        if(!(strcmp(format,"openicc") == 0 || 
             strcmp(format,"xml") == 0))
        {
          fprintf( stderr, "%s %s\n%s\n",
                   _("Allowed option values are -f openicc and -f xml. unknown format:"),
                   format, _("Exit!") );
          return 1;
        }
        texts = oyProfileTag_GetText( tag, &texts_n, NULL, NULL,
                                      &tag_size, malloc );

        if(texts_n > 2)
          size = atoi(texts[0]);

        /* collect key prefixes and detect device class */
        if(size == 2)
        for(j = 2; j < texts_n; j += 2)
          if(texts[j] && strcmp(texts[j],"prefix") == 0)
            has_prefix = 1;

        if(size == 2 && strcmp(device_class,"unknown") == 0)
        for(j = 2; j < texts_n; j += 2)
        {
          int len = texts[j] ? strlen( texts[j] ) : 0;

          #define CHECK_PREFIX( name_, device_class_ ) { \
            int plen = strlen(name_), n=pn-1, found = 0; \
            while(n >= 0) if(strcmp( prefixes[n--], name_ ) == 0) found = 1; \
          if( !found && len >= plen && memcmp( texts[j], name_, plen) == 0 ) \
          { \
            device_class = device_class_; \
            prefixes[pn++] = name_; \
          }}
          CHECK_PREFIX( "EDID_", OPENICC_DEVICE_MONITOR );
          CHECK_PREFIX( "EXIF_", OPENICC_DEVICE_CAMERA );
          CHECK_PREFIX( "lRAW_", OPENICC_DEVICE_CAMERA );
          CHECK_PREFIX( "PPD_", OPENICC_DEVICE_PRINTER );
          CHECK_PREFIX( "CUPS_", OPENICC_DEVICE_PRINTER );
          CHECK_PREFIX( "GUTENPRINT_", OPENICC_DEVICE_PRINTER );
          CHECK_PREFIX( "SANE_", OPENICC_DEVICE_SCANNER );
        }

        /* add device class */
        if(strcmp(format,"openicc") != 0)
          fprintf( fp, "    <dictType>\n      <TagSignature>meta</TagSignature>\n" );

        /* add prefix key */
        if(pn && !has_prefix &&
           strcmp(format,"openicc") == 0)
        {
          for(j = 0; j < pn; ++j)
          {
            oyjlStringAdd( &prefix, 0,0, "%s", prefixes[j] );
            if(pn > 1 && j < pn-1)
              oyjlStringAdd( &prefix, 0,0, "," );
            if(j == pn-1)
              oyjlTreeSetStringF( root, OYJL_CREATE_NEW, prefix, "org/freedesktop/openicc/device/%s/prefix", device_class );
          }
        }

        /* add device and driver calibration properties */
        for(j = 2; j < texts_n; j += 2)
        {
          int vals_n = 0;
          char ** vals = 0, * val = 0;

          if(texts[j] && texts[j+1] && texts_n > j+1)
          {
            if(texts[j+1][0] == '<')
            {
              if(strcmp(format,"openicc") == 0)
                oyjlTreeSetStringF( root, OYJL_CREATE_NEW, texts[j+1], "org/freedesktop/openicc/device/%s/%s", device_class, texts[j] );
              else
                fprintf( fp, "       <DictEntry Name=\"%s\" Values\"%s\"/>",
                         texts[j], texts[j+1] );
            }
            else
            {
              /* split into a array with a useful delimiter */
              vals = oyStringSplit_( texts[j+1], ':', &vals_n, malloc );
              if(vals_n > 1)
              {
                STRING_ADD( val, "              \"");
                STRING_ADD( val, texts[j] );
                STRING_ADD( val, ": [" );
                for(k = 0; k < vals_n; ++k)
                {
                  if(strcmp(format,"openicc") == 0)
                    oyjlTreeSetStringF( root, OYJL_CREATE_NEW, vals[k], "org/freedesktop/openicc/device/%s/%s/[%d]", device_class, texts[j], k );
                  if(k != 0)
                  STRING_ADD( val, "," );
                  STRING_ADD( val, "\"" );
                  STRING_ADD( val, vals[k] );
                  STRING_ADD( val, "\"" );
                }
                STRING_ADD( val, "]");
                if(strcmp(format,"openicc") != 0)
                  fprintf( fp, "%s", val );
                if(val) free( val );
              } else
                if(strcmp(format,"openicc") == 0)
                  oyjlTreeSetStringF( root, OYJL_CREATE_NEW, texts[j+1], "org/freedesktop/openicc/device/%s/%s", device_class, texts[j] );
                else
                  fprintf( fp, "       <DictEntry Name=\"%s\" Value=\"%s\"/>",
                     texts[j], texts[j+1] );

              oyStringListRelease_( &vals, vals_n, free );
            }
          }
          if(j < (texts_n - 2))
          {
            if(strcmp(format,"openicc") != 0)
              fprintf( fp, "\n" );
          }
        }

        if(strcmp(format,"openicc") != 0)
          fprintf( fp, "\n    </dictType>\n" );
      }
      if(oyjlValueCount(root))
        json = oyjlTreeToText( root, strcmp(format,"openicc") == 0 ? OYJL_JSON : OYJL_XML );
      if(json)
      {
        fputs( json, fp );
        free(json);
        json = NULL;
      }
      if(fp && fp != stdout)
      {
        fclose(fp);
        fp = NULL;
      }
    } else
    if( error <= 0 && dump_chromaticities )
    {
      oyProfileTag_s * tags[4] = {0,0,0,0};
      tags[0] = oyProfile_GetTagById( p, icSigRedColorantTag );
      tags[1] = oyProfile_GetTagById( p, icSigGreenColorantTag );
      tags[2] = oyProfile_GetTagById( p, icSigBlueColorantTag );
      tags[3] = oyProfile_GetTagById( p, icSigMediaWhitePointTag );
      if(!tags[0] || !tags[1] || !tags[2] || !tags[3])
      {
        fprintf(stderr, "%s: \"%s\" - %s\n", _("RGB primaries missed"), profile_desc, _("Exit!"));
        return 1;
      }

#ifdef USE_GETTEXT
      setlocale(LC_NUMERIC,"C");
#endif

      if(dump_chromaticities == 1)
      for(i = 0; i < 4; ++i)
      {
        oyStructList_s * s = oyProfileTag_Get( tags[i] );
        count = oyStructList_Count( s );
        for(j = 0; j < count; ++j)
        {
          oyOption_s * opt = (oyOption_s*) oyStructList_GetType( s, j,
                                                    oyOBJECT_OPTION_S );
          if(opt && strstr( oyOption_GetRegistration( opt ), "icSigXYZType" ) != NULL)
          {
            if(i == 0)
              fprintf( stdout, "-red " );
            if(i == 1)
              fprintf( stdout, "-green " );
            if(i == 2)
              fprintf( stdout, "-blue " );
            if(i == 3)
              fprintf( stdout, "-white " );
            fprintf( stdout, "%g %g ",
              oyOption_GetValueDouble( opt, 0 ) /
                  (oyOption_GetValueDouble( opt, 0 )+oyOption_GetValueDouble( opt, 1 )+oyOption_GetValueDouble( opt, 2 )),
              oyOption_GetValueDouble( opt, 1 ) /
                  (oyOption_GetValueDouble( opt, 0 )+oyOption_GetValueDouble( opt, 1 )+oyOption_GetValueDouble( opt, 2 ))
                   );
            if(i == 3)
              fprintf( stdout, "\n" );
          }
        }
        oyProfileTag_Release( &tags[i] );
      } else
      {
        char * command = NULL;
        oyjlStringAdd( &command, 0,0, "ppmcie `oyranos-profile \"%s\" --ppmcie=TEXT` | pamtopng", profile_desc );
        system( command );
        if(verbose)
          fprintf( stderr, "command: %s\n", command );
        free(command);
      }
    } else
    if(remove_tag)
      oyProfile_TagReleaseAt( p, tag_pos );


    if(error <= 0 && list_hash)
    {
      uint32_t * i;
      uint32_t md5[4],
               id[4];
      oyProfile_GetMD5(p, OY_FROM_PROFILE, id);
      error = oyProfile_GetMD5(p, OY_COMPUTE, md5);

      i = (uint32_t*)md5;
      if(!verbose)
        fprintf( stdout, "%08x%08x%08x%08x\n",
            i[0],i[1],i[2],i[3] );
      else
        fprintf( stdout, "%08x%08x%08x%08x[%08x%08x%08x%08x] %s\n",
            i[0],i[1],i[2],i[3], id[0],id[1],id[2],id[3], profile_desc );
    }

    oyProfile_Release( &p );
  } else
  if(!p)
    error = 1;

  }
  else error = 1;

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

extern int * oyjl_debug;
char ** environment = NULL;
int main( int argc_, char**argv_, char ** envv OYJL_UNUSED )
{
  int argc = argc_;
  char ** argv = argv_;

#ifdef __ANDROID__
  setenv("COLORTERM", "1", 0); /* show rich text format on non GNU color extension environment */
  //setenv("OY_DEBUG", "2", 0);

  argv = calloc( argc + 2, sizeof(char*) );
  memcpy( argv, argv_, (argc + 2) * sizeof(char*) );
  argv[argc++] = "--render=gui"; /* start Renderer (e.g. QML) */
  environment = environ;
#else
  environment = envv;
#endif

  /* language needs to be initialised before setup of data structures */
#ifdef OYJL_HAVE_LOCALE_H
  setlocale(LC_ALL,"");
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


