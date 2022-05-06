/** @file oyranos_convert.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2012-2021 (C) Kai-Uwe Behrmann
 *
 *  @brief    ICC conversion - on the command line
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2012/02/19
 *
 *  The program uses ICC profiles to perform color transforms.
 */

#include "oyConversion_s.h"
#include "oyProfiles_s.h"

#include "oyranos.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_helper_macros.h"
#include "oyranos_internal.h"
#include "oyranos_io.h"
#include "oyranos_config.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyranos_version.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "oyjl.h"
#include "oyjl_version.h"
extern char **environ;



static oyjlOptionChoice_s * listInput ( oyjlOption_s * o OYJL_UNUSED, int * y OYJL_UNUSED, oyjlOptions_s * opts OYJL_UNUSED )
{
  oyjlOptionChoice_s * c = NULL;

  int size = 0, i,n = 0;
  char * result = oyjlReadCommandF( &size, "r", malloc, "ls -1 *.[P,p][N,n][G,g] *.[P,p][P,p][M,m] *.[J,j][P,p][G,g] *.[T,t][I,i][F,f]" );
  char ** list = oyjlStringSplit( result, '\n', &n, 0 );

  if(list)
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
    free(list);
  }

  return c;
}

static oyjlOptionChoice_s * listProfiles ( icSignature profile_class, icSignature profile_class2, icSignature profile_class3 )
{
  oyProfile_s * pattern = 0;
  oyProfiles_s * patterns = oyProfiles_New( 0 ),
               * profiles = 0;
  uint32_t icc_profile_flags = oyICCProfileSelectionFlagsFromOptions( 
                                    OY_CMM_STD, "//" OY_TYPE_STD "/icc_color",
                                                                   NULL, 0 );

  pattern = oyProfile_FromSignature( profile_class, oySIGNATURE_CLASS, 0 );
  oyProfiles_MoveIn( patterns, &pattern, -1 );
  if(profile_class2)
  {
    pattern = oyProfile_FromSignature( profile_class2, oySIGNATURE_CLASS, 0 );
    oyProfiles_MoveIn( patterns, &pattern, -1 );
  }
  if(profile_class3)
  {
    pattern = oyProfile_FromSignature( profile_class3, oySIGNATURE_CLASS, 0 );
    oyProfiles_MoveIn( patterns, &pattern, -1 );
  }

  profiles = oyProfiles_Create( patterns, icc_profile_flags, 0 );
  oyProfiles_Release( &patterns );
  int n = oyProfiles_Count( profiles ), i;
  oyjlOptionChoice_s * cs = (oyjlOptionChoice_s*) calloc( (unsigned int)n+20, sizeof(oyjlOptionChoice_s) );
  for(i = 0; i < n; ++i)
  {
    oyProfile_s * p = oyProfiles_Get(profiles, i);
    const char * desc = oyProfile_GetText(p, oyNAME_DESCRIPTION);
    const char * fn = oyProfile_GetFileName(p, -1);
    if(desc)
      cs[i].name = oyjlStringCopy( desc, 0 );
    if(fn)
      cs[i].nick = oyjlStringCopy( fn, 0 );
    oyProfile_Release( &p );
  }
  oyProfiles_Release( &profiles );

  return cs;
}

static oyjlOptionChoice_s * listDeviceLinkProfiles ( oyjlOption_s * x OYJL_UNUSED, int * y OYJL_UNUSED, oyjlOptions_s * z OYJL_UNUSED )
{ return listProfiles(icSigLinkClass, 0,0); }

static oyjlOptionChoice_s * listEffectProfiles ( oyjlOption_s * x OYJL_UNUSED, int * y OYJL_UNUSED, oyjlOptions_s * z OYJL_UNUSED )
{ return listProfiles(icSigAbstractClass, 0,0); }

static oyjlOptionChoice_s * listSimulationProfiles ( oyjlOption_s * x OYJL_UNUSED, int * y OYJL_UNUSED, oyjlOptions_s * z OYJL_UNUSED )
{ return listProfiles(icSigOutputClass, 0,0); }

static oyjlOptionChoice_s * listOutputProfiles ( oyjlOption_s * x OYJL_UNUSED, int * y OYJL_UNUSED, oyjlOptions_s * z OYJL_UNUSED )
{ return listProfiles(icSigOutputClass, icSigDisplayClass, icSigColorSpaceClass); }

static oyjlOptionChoice_s * listIccNodes ( oyjlOption_s * o OYJL_UNUSED, int * y OYJL_UNUSED, oyjlOptions_s * opts OYJL_UNUSED )
{
  oyjlOptionChoice_s * c = NULL;

  int i, current = -1, n = 0;
  const char ** list = NULL, ** nicks = NULL;
  int icc_profile_flags = oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                            "//" OY_TYPE_STD "/icc_color", NULL, 0 );
  oyOptionChoicesGet2( oyWIDGET_CMM_CONTEXT, icc_profile_flags, oyNAME_NICK, &n, &nicks, &current );
  oyOptionChoicesGet2( oyWIDGET_CMM_CONTEXT, icc_profile_flags, oyNAME_NAME, &n, &list, &current );

  if(list)
  {
    c = calloc(n+1, sizeof(oyjlOptionChoice_s));
    if(c)
    {
      for(i = 0; i < n; ++i)
      {
        c[i].nick = strdup( nicks[i] );
        c[i].name = strdup( list[i] );
        c[i].description = strdup("");
        c[i].help = strdup("");
      }
    }
  }

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
  const char * output_profile = 0;
  const char * device_link = 0;
  const char * input = 0;
  const char * output = 0;
  const char * node_name = 0;
  int effect_profile_count = 0;
  int simulation_profile_count = 0;
  int other_args_n = 0;
  int levels = 0;
  const char * format = 0;
  int uint8_var = 0,
      uint16_var = 0,
      half_var = 0,
      float_var = 0,
      double_var = 0;
  int icc_defaults_simple = 0;
  const char * export_var = 0;
  const char * help = 0;
  const char * render = 0;
  int verbose = 0;
  int version = 0;

  /* handle options */
  /* declare the option choices  *   nick,          name,               description,                  help */
  oyjlOptionChoice_s f_choices[] = {{"clut",        _("CLUT"),          _("Generate a 3D LUT"),       _("Look Up Table")},
                                    {"icc",         _("ICC"),           _("Profile"),                 _("Device Link")},
                                    {"hald",        _("HALD"),          "",                        ""},
                                    {"slice",       _("Slice"),         "",                        ""},
                                    {"lab",         _("Lab"),           "",                        ""},
                                    {NULL,NULL,NULL,NULL}};

  oyjlOptionChoice_s l_choices[] = {{"4",           _("4"),             "",                        ""},
                                    {"8",           _("8"),             "",                        ""},
                                    {"12",          _("12"),            "",                        ""},
                                    {"16",          _("16"),            "",                        ""},
                                    {"64",          _("64"),            _("Default for CLUT"),                        ""},
                                    {NULL,NULL,NULL,NULL}};

  oyjlOptionChoice_s E_choices[] = {{"OY_DEBUG", _("set the Oyranos debug level."),_("Alternatively the -v option can be used."),_("Valid integer range is from 1-20.")},
                                    {"OY_MODULE_PATH",_("route Oyranos to additional directories containing modules."),NULL,                         NULL},
                                    {"XDG_CONFIG_HOME",_("search paths are influenced by the XDG_CONFIG_HOME shell variable."),NULL,                         NULL},
                                    {NULL,NULL,NULL,NULL}};

  oyjlOptionChoice_s A_choices[] = {{"",_("Get ICC profile"),_("oyranos-icc -f icc -i image.png | iccexamin -g -i"),                          NULL},
                                    {"",_("Convert image to ICC Color Space"),_("oyranos-icc -i image.png -n lcm2 -p Lab.icc -o image.ppm"),  NULL},
                                    {"",_("Convert image through ICC device link profile"),_("oyranos-icc -i image.png --device-link deviceLink.icc -o image.ppm"),NULL},
                                    {"",_("Get Conversion"),_("oyranos-icc -f icc -i input.icc -n lcm2 -p sRGB.icc -o device_link.icc"),      NULL},
                                    {"",_("Create 3D CLUT"),_("oyranos-icc -f clut -i Lab.icc -n lcm2 -p sRGB.icc -o clut.ppm"),              NULL},
                                    {NULL,NULL,NULL,NULL}};

  oyjlOptionChoice_s S_choices[] = {{_("oyranos-profile(1) oyranos-profiles(1) oyranos(3)"),NULL,               NULL,                         NULL},
                                    {"http://www.oyranos.org",NULL,               NULL,                         NULL},
                                    {NULL,NULL,NULL,NULL}};

  /* declare options - the core information; use previously declared choices */
  oyjlOption_s oarray[] = {
  /* type,   flags,                      o,  option,          key,      name,          description,                  help, value_name,         
        value_type,              values,             variable_type, variable_name */
//    {"oiwi", OYJL_OPTION_FLAG_EDITABLE|OYJL_OPTION_FLAG_REPETITION,"@",NULL,NULL,_("Module Options"),_("Options dependent to the -n=MODULE_NAME option."),NULL, _("MODULE_OPTIONS"),
//        oyjlOPTIONTYPE_FUNCTION, {.getChoices = listOptions}, oyjlINT, {.i=&other_args_n} },
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "p","output-profile",NULL,     _("Output Profile"),_("Output Color Space"),      NULL, _("ICC_FILE_NAME"),
        oyjlOPTIONTYPE_FUNCTION, {.getChoices = listOutputProfiles},                oyjlSTRING,    {.s=&output_profile}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  NULL,"device-link",  NULL,     _("Device Link"),_("Conversion"),              NULL, _("ICC_FILE_NAME"),
        oyjlOPTIONTYPE_FUNCTION, {.getChoices = listDeviceLinkProfiles},                oyjlSTRING,    {.s=&device_link}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "i","input",         NULL,     _("Input"),    _("Read from file"),          NULL, _("FILE_NAME"),
        oyjlOPTIONTYPE_FUNCTION, {.getChoices = listInput},                oyjlSTRING,{.s=&input}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "o","output",        NULL,     _("Output"),   _("Write to file"),           _("For Convert Image currently only PPM and PNG formats. For -f clut currently only PPM format, CLUT is a levels x levels*levels sized PPM."),_("FILE_NAME"),     
        oyjlOPTIONTYPE_CHOICE,   {0},                oyjlSTRING,    {.s=&output}},
    {"oiwi", 0,                          "n","node-name",     NULL,     _("Node Name"),_("Module name"),             NULL, _("MODULE_NAME"),
        oyjlOPTIONTYPE_FUNCTION, {.getChoices = listIccNodes},                oyjlSTRING,    {.s=&node_name}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE|OYJL_OPTION_FLAG_REPETITION,"e","effect-profile",NULL,     _("Effect Profile"),_("Effect abtract Color Space"),NULL, _("ICC_FILE_NAME"), 
        oyjlOPTIONTYPE_FUNCTION, {.getChoices = listEffectProfiles},                oyjlINT,       {.i=&effect_profile_count}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE|OYJL_OPTION_FLAG_REPETITION,"s","simulation-profile",NULL,     _("Simulation Profile"),_("Simulation/Proof Color Space"),NULL, _("ICC_FILE_NAME"), 
        oyjlOPTIONTYPE_FUNCTION, {.getChoices = listSimulationProfiles},                oyjlINT,       {.i=&simulation_profile_count}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  NULL,"levels",        NULL,     _("Levels"),   _("Defaults for -f clut to 64. For -f hald|slice|lab levels from 4-16 make sense."),NULL, _("NUMBER"),        
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)l_choices, sizeof(l_choices), malloc ), 0}}, oyjlINT,       {.i=&levels}},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE,  "f","format",        NULL,     _("Format"),   _("Select format"),           NULL, _("FORMAT"),
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)f_choices, sizeof(f_choices), malloc ), 0}}, oyjlSTRING,    {.s=&format}},
    {"oiwi", 0,                          NULL,"uint8",         NULL,     NULL,          _("select unsigned integer 8-bit precision data format"),NULL, NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlNONE,      {.i=&uint8_var}},
    {"oiwi", 0,                          NULL,"uint16",        NULL,     NULL,          _("select unsigned integer 16-bit precision data format"),NULL, NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlNONE,      {.i=&uint16_var}},
    {"oiwi", 0,                          NULL,"half",          NULL,     NULL,          _("select floating point 16-bit precision data format"),NULL, NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlNONE,      {.i=&half_var}},
    {"oiwi", 0,                          NULL,"float",         NULL,     NULL,          _("select floating point 32-bit precision data format"),NULL, NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlNONE,      {.i=&float_var}},
    {"oiwi", 0,                          NULL,"double",        NULL,     NULL,          _("select floating point 64-bit precision data format"),NULL, NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlNONE,      {.i=&double_var}},
    {"oiwi", 0,                          "d","icc-defaults-simple",NULL,     _("ICC Defaults Simple"),_("enable simple defaults"),  NULL, NULL,
        oyjlOPTIONTYPE_NONE,     {0},                oyjlINT,       {.i=&icc_defaults_simple}},
    {"oiwi", 0,                          "E","man-environment_variables",NULL,     NULL,NULL,                         NULL, NULL,
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)E_choices, sizeof(E_choices), malloc ), 0}}, oyjlNONE,      {}},
    {"oiwi", 0,                          "A","man-examples",  NULL,      NULL,          NULL,                         NULL, NULL,               
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)A_choices, sizeof(A_choices), malloc ), 0}}, oyjlNONE,      {}},
    {"oiwi", 0,                          "S","man-see_as_well",NULL,     NULL,          NULL,                         NULL, NULL,               
        oyjlOPTIONTYPE_CHOICE,   {.choices = {(oyjlOptionChoice_s*) oyjlStringAppendN( NULL, (const char*)S_choices, sizeof(S_choices), malloc ), 0}}, oyjlNONE,      {}},

    /* default options -h and -v */
    {"oiwi", OYJL_OPTION_FLAG_ACCEPT_NO_ARG, "h", "help",NULL,NULL,NULL,NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlSTRING, {.s=&help} },
    {"oiwi", 0, NULL,"synopsis",NULL, NULL,         NULL,         NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlNONE, {0} },
    {"oiwi", 0, "v", "verbose", NULL, _("verbose"), _("verbose"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&verbose} },
    {"oiwi", 0, "V", "version", NULL, _("version"), _("Version"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&version} },
    /* default option template -X|--export */
    {"oiwi", 0, "X", "export", NULL, NULL, NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices.list = NULL}, oyjlSTRING, {.s=&export_var} },
    /* The --render option can be hidden and used only internally. */
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE|OYJL_OPTION_FLAG_MAINTENANCE, "R", "render", NULL, NULL,  NULL,  NULL, NULL, oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s=&render} },
    {"",0,0,NULL,NULL,NULL,NULL,NULL, NULL, oyjlOPTIONTYPE_END, {0},oyjlNONE,{0}}
  };

  /* declare option groups, for better syntax checking and UI groups */
  oyjlOptionGroup_s groups[] = {
  /* type,   flags, name,               description,                  help,               mandatory,     optional,      detail */
    {"oiwg", 0,     NULL,               _("Convert Image"),           NULL,               "p|device-link,i","o,n,e,s,levels","i,p,device-link,s,e,o"},
    {"oiwg", 0,     NULL,               _("Generate CLUT Image"),     _("Use the -f clut argument. -i shall provide the Input Color Space."),"f,p",         "i,o,n,e,s,levels","f,i,p,s,e,o,levels"},
    {"oiwg", 0,     NULL,               _("Generate Device Link Profile"),_("Use the -f icc argument."),"f,p,i",       "o,n",         "f,uint8,uint16,half,float,double"},
    {"oiwg", 0,     NULL,               _("Extract ICC profile"),     _("Use the -f icc argument."),"f,i",         "o,n",         "f,o"},
    {"oiwg", 0,     NULL,               _("Generate Image"),          _("Use the -f hald|slice|lab arguments."),"f",           "o,levels",    "f,o,levels"},
    {"oiwg", OYJL_GROUP_FLAG_GENERAL_OPTS,NULL,               _("General options"),         NULL,               "X|h|V|R",     "v",           "n,d,h,X,R,V,v"},
    {"",0,0,0,0,0,0,0}
  };

  oyjlUiHeaderSection_s * info = oyUiInfo(_("The oyranos-icc programm converts images by ICC profiles or embedded ICC profiles from within images to a new image. It helps to create these color conversion and writes them into various formats. It can extract a embedded ICC profile from a image."),
                  "2020-09-23T12:00:00", "September 23, 2020");
  oyjlUi_s * ui = oyjlUi_Create( argc, argv, /* argc+argv are required for parsing the command line options */
                                       "oyranos-icc", _("ICC color conversion tool"), NULL,
#ifdef __ANDROID__
                                       ":/images/logo.svg", // use qrc
#else
                                       NULL,
#endif
                                       info, oarray, groups, &state );
  if( state & oyjlUI_STATE_EXPORT &&
      export_var &&
      strcmp(export_var,"json+command") != 0)
    goto clean_main;
  if(state & oyjlUI_STATE_HELP)
  {
    fprintf( stderr, "%s\n\tman oyranos-icc\n\n", _("For more information read the man page:") );
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

  if(ui && (export_var && strcmp(export_var,"json+command") == 0))
  {
    char * json = oyjlUi_ToJson( ui, 0 ),
         * json_commands = NULL;
    oyjlStringAdd( &json_commands, malloc, free, "{\n  \"command_set\": \"%s\",", argv[0] );
    oyjlStringAdd( &json_commands, malloc, free, "%s", &json[1] ); /* skip opening '{' */
    puts( json_commands );
    goto clean_main;
  }

  /* Render boilerplate */
  if(ui && render)
  {
#if !defined(NO_OYJL_ARGS_RENDER)
    int debug = verbose;
    oyjlArgsRender( argc, argv, NULL, NULL,NULL, debug, ui, myMain );
#else
    fprintf( stderr, "No render support compiled in. For a GUI use -X json and load into oyjl-args-render viewer." );
#endif
  } else if(ui)
  {
#if 1
    /* ... working code goes here ... */
  oyDATATYPE_e data_type = oyUINT16;
  char * simulation_profile = 0;
  char * effect_profile = 0;
  uint32_t icc_profile_flags = 0;
  oyProfiles_s * proofing = oyProfiles_New(0),
               * effects = oyProfiles_New(0);
  oyProfile_s * p = NULL;
  oyOptions_s * module_options = 0;
  char ** list = 0;

  int i, n;

  char ** other_args = 0;
  char * text = 0, * t = 0;
  oyOptions_s * opts = 0;
  oyImage_s * image = 0;

    if(uint8_var)
      data_type = oyUINT8;
    if(uint16_var)
      data_type = oyUINT16;
    if(half_var)
      data_type = oyHALF;
    if(float_var)
      data_type = oyFLOAT;
    if(double_var)
      data_type = oyDOUBLE;

    if(node_name)
    {
      oyOptions_SetFromString( &module_options, OY_DEFAULT_CMM_CONTEXT,
                                               node_name, OY_CREATE_NEW );

      icc_profile_flags = oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                                            "//" OY_TYPE_STD "/icc_color", module_options, 0 );
    }

    if(simulation_profile_count)
    {
      n = 0;
      list = oyjlOptions_ResultsToList( ui->opts, "s", &n );
      for(i = 0; i < n; ++i)
      {
        t = list[i];
        p = oyProfile_FromName( t, icc_profile_flags, 0 );
        oyProfiles_MoveIn( proofing, &p, -1 );
      }
      oyjlStringListRelease( &list, n, 0 );
    }

    if( effect_profile_count )
    {
      n = 0;
      list = oyjlOptions_ResultsToList( ui->opts, "e", &n );
      for(i = 0; i < n; ++i)
      {
        t = list[i];
        p = oyProfile_FromName( t, icc_profile_flags, 0 );
        oyProfiles_MoveIn( effects, &p, -1 );
      }
      oyjlStringListRelease( &list, n, 0 );
    }

    if( other_args_n )
    {
      n = 0;
      list = oyjlOptions_ResultsToList( ui->opts, "@", &n );
      for(i = 0; i < n; ++i)
      {
        t = oyjlStringCopy( list[i], oyAllocateFunc_ );
        text = oyStrrchr_(t, '=');
        /* get the key only */
        if(text)
          text[0] = 0;
        oyStringListAddStaticString( &other_args,&other_args_n,
                                            t,
                                            oyAllocateFunc_,oyDeAllocateFunc_ );
        if(text)
          oyStringListAddStaticString( &other_args,&other_args_n,
                                            text+1,
                                            oyAllocateFunc_,oyDeAllocateFunc_ );
        else
        {
          if(i+1 < n)
          {
            ++i;
            oyStringListAddStaticString( &other_args,
                                            &other_args_n,
                                            list[i],
                                            oyAllocateFunc_,oyDeAllocateFunc_ );
          }// else wrong_arg = t;
        }
        oyjlStringListRelease( &list, n, 0 );
      }
    }

  if(help)
  {
    if(node_name)
    {
      int r OY_UNUSED;
      char * t = 0;
      STRING_ADD( t, "oyranos-xforms-modules -n " );
      STRING_ADD( t, node_name );
      if(!icc_defaults_simple)
        STRING_ADD( t, " -f" );
      if(verbose)
        STRING_ADD( t, " -v" );
      STRING_ADD( t, " | oyranos-xforms" );
      if(verbose)
        STRING_ADD( t, " -lh" );
      if(oy_debug)
        fprintf(stderr, "%s\n", t);
      r = system(t);
      return;
    }
  }

  if(verbose)
    fprintf( stderr, "  Oyranos v%s\n",
                  oyNoEmptyName_m_(oyVersionString(1,0)));

#if 1
  if(other_args)
  {
    const char * result_xml = 0;
    const char * opt_names = 0;
    //oyFormsArgs_s * forms_args = oyFormsArgs_New( 0 );
    const char * data = 0, * ct = 0;
    int i;
    oyOption_s * o = 0;

    //forms_args->print = 0;

    /* TODO */
    //error = oyXFORMsRenderUi( text, oy_ui_cmd_line_handlers, forms_args );
    //result_xml = oyFormsArgs_ModelGet( forms_args );

    if(result_xml)
    {
      opts = oyOptions_FromText( result_xml, 0,0 );

      data = oyOptions_GetText( opts, oyNAME_NAME );
      opt_names = oyOptions_GetText( opts, oyNAME_DESCRIPTION );

      for( i = 0; i < other_args_n; i += 2 )
      {
        /* check for wrong args */
        if(opt_names && strstr( opt_names, other_args[i] ) == NULL)
        {
          fprintf(stderr, "Unknown option: %s", other_args[i]);
          exit( 1 );

        } else
        {
          o = oyOptions_Find( opts, other_args[i], oyNAME_PATTERN );
          if(i + 1 < other_args_n)
          {
            ct = oyOption_GetText( o, oyNAME_NICK );
            if(oy_debug)
            fprintf( stderr, "%s => ",
                    ct?ct:"---" ); ct = 0;
            oyOption_SetFromString( o, other_args[i + 1], 0 );
            data = oyOption_GetText( o, oyNAME_NICK );

            if(oy_debug)
            fprintf( stderr, "%s\n",
                    data?oyStrchr_(data, ':') + 1:"" ); data = 0;
          }
          else
          {
            fprintf( stderr,
                    "%s: --%s  argument missed\n", _("Option"), other_args[i] );
            exit( 1 );
          }
          oyOption_Release( &o );
        }
      }
    } else
      /* handle the options as if they are commandline switches */
      for( i = 0; i+1 < other_args_n; i += 2 )
      {
        oyOptions_SetFromString( &module_options, other_args[i],
                               other_args[i+1], OY_CREATE_NEW );
        
      }
  }
#endif


  icc_profile_flags = oyICCProfileSelectionFlagsFromOptions( OY_CMM_STD,
                            "//" OY_TYPE_STD "/icc_color", module_options, 0 );

  if(output_profile || device_link)
  {
    uint32_t flags = 0;
    oyPixel_t pixel_layout;
    oyConversion_s * cc;

    if(!output)
      WARNc_S("No output file name provided");
    if(!icc_defaults_simple)
      flags |= oyOPTIONATTRIBUTE_ADVANCED;
    if(oyProfiles_Count(effects))
    {
      error = oyOptions_MoveInStruct( &module_options,
                                      OY_PROFILES_EFFECT,
                                       (oyStruct_s**) &effects,
                                       OY_CREATE_NEW );
      oyOptions_SetFromString( &module_options, OY_DEFAULT_EFFECT,
                             "1", OY_CREATE_NEW );
    }
    if(oyProfiles_Count(proofing))
    {
      error = oyOptions_MoveInStruct( &module_options,
                                      OY_PROFILES_SIMULATION,
                                       (oyStruct_s**) &proofing,
                                       OY_CREATE_NEW );
      oyOptions_SetFromString( &module_options, OY_DEFAULT_PROOF_SOFT,
                             "1", OY_CREATE_NEW );
    }


    if(format && oyStringCaseCmp_(format,"clut") == 0)
    {
      int width = levels,
          size, l,a,b,j;
      uint16_t * buf = 0;
      uint16_t in[3];
      char comment[80];

      if(!width)
        width = 64;

      size = width*width;

      if(!output)
        WARNc_S("No output file name provided");

      buf = calloc(sizeof(uint16_t), size*width*3);

#pragma omp parallel for private(in,a,b,j)
      for(l = 0; l < width; ++l)
      {
        in[0] = floor((double) l / (width - 1) * 65535.0 + 0.5);
        for(a = 0; a < width; ++a) {
          in[1] = floor((double) a / (width - 1) * 65535.0 + 0.5);
          for(b = 0; b < width; ++b)
          {
            in[2] = floor((double) b / (width - 1) * 65535.0 + 0.5);

            for(j = 0; j < 3; ++j)
              /* BGR */
              buf[b*size*3+a*+width*3+l*3+j] = in[j];
          }
        }
      }
      if(input)
      {
        p = oyProfile_FromName( input, icc_profile_flags, 0 );
        if(!p)
          WARNc1_S("Could not open profile: %s", input);
          error = 1;
      } else
        p = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, 0 );
      image = oyImage_Create( width,width*width, buf, OY_TYPE_123_16,
                              p, 0 );
      oyProfile_Release( &p );
      sprintf( comment, "clut with %d levels", width );

      pixel_layout = oyImage_GetPixelLayout( image, oyLAYOUT );
      data_type = oyToDataType_m(pixel_layout);
      p = oyProfile_FromName(output_profile, icc_profile_flags, 0);
      cc = oyConversion_CreateFromImage (
                                image, module_options,
                                p, data_type, flags, 0 );

      error = oyConversion_RunPixels( cc, 0 );
      image = oyConversion_GetImage( cc, OY_OUTPUT );

      error = oyImage_WritePPM( image, output, comment);

      oyImage_Release( &image );
    } else
    if(format && oyStringCaseCmp_(format,"icc") == 0)
    {
      double buf[24];
      oyImage_s * in;
      oyFilterGraph_s * graph = NULL;
      oyFilterNode_s * icc = NULL;
      oyBlob_s * blob = NULL;
      int error = 0;
      int n=0;
      const char* node_name = "///icc_color";

      if(input)
      {
        p = oyProfile_FromName( input, icc_profile_flags, 0 );
        if(!p)
        {
          WARNc1_S("Could not open profile: %s", input);
          error = 1;
        }
      } else
        p = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, 0 );
      n = oyProfile_GetChannelsCount(p);
      pixel_layout = oyChannels_m(n) | oyDataType_m(data_type);
      in = oyImage_Create( 2, 2, buf, pixel_layout, p, 0 );
      oyProfile_Release( &p );

      p = oyProfile_FromName(output_profile, icc_profile_flags, 0);

      cc = oyConversion_CreateFromImage (
                                in, module_options, 
                                p, data_type, 0, 0 );
      oyProfile_Release( &p );

      memset( buf, 0, sizeof(double)*24);

      if(cc)
        graph = oyConversion_GetGraph( cc );
      if(graph)
        icc = oyFilterGraph_GetNode( graph, -1, node_name, NULL );
      if(icc)
      {
        int old_oy_debug = oy_debug;
        oy_debug = 1;
        blob = oyFilterNode_ToBlob( icc, 0 );
        oy_debug = old_oy_debug;
        if(blob && oyBlob_GetSize( blob ))
        {
          size_t size = oyBlob_GetSize( blob);
          char * data = oyBlob_GetPointer( blob );

          if(output)
          {
            error = oyWriteMemToFile_ ( output, data, size );
            if(error)
            {
              WARNc_S("Could not write to profile");
            }
          } else
          {
            fwrite( data, sizeof(char), size, stdout );
          }
        }
        oyBlob_Release( &blob );
        oyFilterNode_Release( &icc );
      } else
        WARNc1_S("Could not open node: %s", node_name);
      oyFilterGraph_Release( &graph );

    } else
    {
      char * comment = 0;
      error = oyImage_FromFile( input, icc_profile_flags, &image, NULL );
      pixel_layout = oyImage_GetPixelLayout( image,oyLAYOUT );
      if(device_link)
      {
        p = oyProfile_FromName(device_link, icc_profile_flags, 0);
        if(!p)
        {
          WARNc1_S("Could not open profile: %s", device_link);
          error = 1;
        } else
        {
          const char * t;
          char * dln = NULL;
          oyProfile_s * dl = NULL;
          oyProfileTag_s * psid = oyProfile_GetTagById( p, icSigProfileSequenceIdentifierTag );
          int32_t texts_n = 0;
          char ** texts = oyProfileTag_GetText( psid, &texts_n, 0,0,0,0);
          int count = (texts_n-1)/5;

          oyProfileTag_Release( &psid );
          oyStringListRelease_( &texts, texts_n, oyDeAllocateFunc_ );

          oyImage_SetCritical( image, 0, p, 0, -1,-1 );
          t = oyProfile_GetFileName( p, count - 1 );
          if(t)
          {
            output_profile = dln = strdup( t );
            dl = oyProfile_FromName(t, icc_profile_flags, 0);
          }
          if(dl && strcmp(t,dln) != 0)
          {
            oyProfile_Release( &p );
            WARNc2_S("Set output profile %s from %s", t,dln);
            p = dl;
          } else if(!output_profile)
          {
            fprintf( stderr, "No output profile found in: %s - use the -p option", t );
            return 1;
          } else
            oyProfile_Release( &p );
        }
      }
      if(!p)
        p = oyProfile_FromName(output_profile, icc_profile_flags, 0);
      if(!p)
      {
          WARNc1_S("Could not open output profile: %s", output_profile);
          error = 1;
      }
      data_type = oyToDataType_m(pixel_layout);
      cc = oyConversion_CreateFromImage (
                                image, module_options,
                                p, data_type, flags, 0 );
      oyImage_Release( &image );

      error = oyConversion_RunPixels( cc, 0 );
      image = oyConversion_GetImage( cc, OY_OUTPUT );
      if(verbose)
      {
        oyFilterGraph_s * cc_graph = oyConversion_GetGraph( cc );
        oyFilterNode_s * icc = oyFilterGraph_GetNode( cc_graph, -1, node_name, 0 );
        oyFilterGraph_Release( &cc_graph );
        fprintf( stderr, "node hash:\n%s\n", oyFilterNode_GetText( icc, oyNAME_NAME ) );
        oyFilterNode_Release( &icc );
      }
      oyConversion_Release( &cc );

      STRING_ADD( comment, "source image was " );
      STRING_ADD( comment, input );
      oyOptions_SetFromString( &opts, "//" OY_TYPE_STD "/file_write/comment",
                             comment, OY_CREATE_NEW );
      error = oyImage_ToFile( image, output, opts );

      oyImage_Release( &image );
      oyFree_m_( comment );
    }
    
  } else
  if(format && oyStringCaseCmp_(format,"icc") == 0)
  {
    
    oyProfile_s * prof = 0;
    size_t size = 0;
    char * data = 0;
    fprintf(stderr, "%s\n", input);
    error = oyImage_FromFile( input, icc_profile_flags, &image, NULL );
    prof = oyImage_GetProfile( image );
    data = oyProfile_GetMem( prof, &size, 0, oyAllocateFunc_);
    if(size)
    {
      if(output)
      {
        error = oyWriteMemToFile_ ( output, data, size );
        if(error)
        {
          WARNc_S("Could not write to profile");
        }
      } else
      {
        fwrite( data, sizeof(char), size, stdout );
      }
      oyDeAllocateFunc_(data); size = 0; data = 0;
    } else
      WARNc_S("No profile found");

    oyImage_Release( &image );
    oyProfile_Release( &prof );
    
  } else
  if(format && (oyStringCaseCmp_(format,"hald") == 0 ||
                oyStringCaseCmp_(format,"slice") == 0 ||
                oyStringCaseCmp_(format,"lab") == 0))
  {
    int width = levels,
        size = width*width,
        l,a,b,j;
    uint16_t * buf = 0;
    uint16_t in[3];
    char comment[80];

    if(!output)
      WARNc_S("No output file name provided");

    p = oyProfile_FromStd( oyEDITING_LAB, icc_profile_flags, 0 );
    if(oyStringCaseCmp_(format,"hald") == 0)
    {
      if(!width)
        width = 8;
      buf = calloc(sizeof(uint16_t), size*width*size*width*3);

#pragma omp parallel for private(in,a,b,j)
      for(l = 0; l < size; ++l)
      {
        in[0] = floor((double) l / (size - 1) * 65535.0 + 0.5);
        for(a = 0; a < size; ++a) {
          in[1] = floor((double) a / (size - 1) * 65535.0 + 0.5);
          for(b = 0; b < size; ++b)
          {
            in[2] = floor((double) b / (size - 1) * 65535.0 + 0.5);
            for(j = 0; j < 3; ++j)
              buf[l*size*size*3+b*size*3+a*3+j] = in[j];
          }
        }
      }

      image = oyImage_Create( size*width, size*width, buf, OY_TYPE_123_16,
                              p, 0 );
      sprintf( comment, "CIE*Lab Hald with %d levels", width );

    } else if(oyStringCaseCmp_(format,"lab") == 0)
    {
      if(!width)
        width = 64;
      buf = calloc(sizeof(uint16_t), size*width*3);

#pragma omp parallel for private(in,a,b,j)
      for(l = 0; l < width; ++l)
      {
        in[0] = floor((double) l / (width - 1) * 65535.0 + 0.5);
        for(a = 0; a < width; ++a) {
          in[1] = floor((double) a / (width - 1) * 65535.0 + 0.5);
          for(b = 0; b < width; ++b)
          {
            in[2] = floor((double) b / (width - 1) * 65535.0 + 0.5);
            for(j = 0; j < 3; ++j)
              buf[a*size*3+b*+width*3+l*3+j] = in[j];
          }
        }
      }
      image = oyImage_Create( width,width*width, buf, OY_TYPE_123_16,
                              p, 0 );
      sprintf( comment, "CIE*Lab LUT with %d levels", width );
    } else if(oyStringCaseCmp_(format,"slice") == 0)
    {
      if(!width)
        width = 17;
      buf = calloc(sizeof(uint16_t), size*width*3);

#pragma omp parallel for private(in,a,b,j)
      for(l = 0; l < width; ++l)
      {
        in[1] = floor((double) l / (width - 1) * 65535.0 + 0.5);
        for(a = 0; a < width; ++a) {
          in[0] = floor((double) a / (width - 1) * 65535.0 + 0.5);
          for(b = 0; b < width; ++b)
          {
            in[2] = floor((double) b / (width - 1) * 65535.0 + 0.5);
            for(j = 0; j < 3; ++j)
              buf[a*size*3+b*+width*3+l*3+j] = in[j];
          }
        }
      }
      image = oyImage_Create( width,width*width, buf, OY_TYPE_123_16,
                              p, 0 );
      sprintf( comment, "CIE*Lab slice with %d levels", width );
    } else
      WARNc1_S("format is not supported %s", format);

    error = oyImage_WritePPM( image, output, comment);
    if(error)
    {
      WARNc_S("Could not write to file");
    }
  }
  if(!output_profile && !device_link && format && oyStringCaseCmp_(format,"clut") == 0)
  {
      WARNc1_S("format \"%s\" is missing --output or --device-link argument", format);
                        return 0;
  }
  /* format conversion */
  else if(input && output)
  {
    char * comment = NULL;
    STRING_ADD( comment, "source image was " );
    STRING_ADD( comment, input );
    oyOptions_SetFromString( &opts, "//" OY_TYPE_STD "/file_write/comment",
                             comment, OY_CREATE_NEW );
    oyFree_m_( comment );
    error = oyImage_FromFile( input, icc_profile_flags, &image, NULL );
    error = oyImage_ToFile( image, output, opts );

    oyImage_Release( &image );
  }
  else
  {
      WARNc1_S("format is not supported \"%s\" with this arguments", format);
                        return 0;
  }

  oyProfile_Release( &p );
  oyProfiles_Release( &effects );
  oyProfiles_Release( &proofing );
  oyOptions_Release( &opts );

  oyFinish_( FINISH_IGNORE_I18N | FINISH_IGNORE_CACHES );
#endif
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
  oyjlLibRelease();

  return error;
}

extern int * oyjl_debug;
char ** environment = NULL;
int main( int argc_, char**argv_, char ** envv )
{
  int argc = argc_;
  char ** argv = argv_;


#ifdef __ANDROID__
  setenv("COLORTERM", "1", 0); /* show rich text format on non GNU color extension environment */

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
  oyFinish_( FINISH_IGNORE_I18N | FINISH_IGNORE_CACHES );

  return 0;
}


