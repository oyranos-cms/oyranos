/** @file oyranos_profile_graph.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  Copyright (C) 2012-2019  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    2D ICC profile graph
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de> and others
 *  @par License:
 *            BSD-3-Clause <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2012/06/12
 *
 *  A tool to generate a 2D graph of ICC profiles for fast visualising.
 *  
 */

# include <stddef.h>
#include <regex.h>

#define __USE_POSIX2 1
#include <stdio.h>                /* popen() */
#include <math.h>                 /* NAN */
#include <string.h>
#include <cairo.h>                /* Cairo headers */
#include <cairo-svg.h>            /* Cairo SVG headers */

#include "oyConversion_s.h"
#include "oyProfiles_s.h"
#include "oyImage_s.h"
#include "oyranos_alpha_internal.h"

#ifdef __cplusplus
#if defined(USE_CLIPPER)
#include <cpp_cairo/cairo_clipper.hpp>
#include <clipper.hpp>
#include <cmath>
#include <iostream>
#endif
#endif /* __cplusplus */

#include <oyjl.h>


#include "oyranos_color.h"
#include "oyranos_color_internal.h"
#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_helper_macros.h"
#include "oyranos_internal.h"
#include "oyranos_config.h"
#include "oyranos_version.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"

#include "ciexyz31_2.h" /* cieXYZ_31_2 1931 2° */
#include "ciexyz64_1.h"
#include "spd_A_5.h"
#include "spd_D65_5.h"
#include "spd_S1S2S3_5.h"

# define x_xyY cieXYZ_31_2[i][0]/(cieXYZ_31_2[i][0]+cieXYZ_31_2[i][1]+cieXYZ_31_2[i][2])
# define y_xyY cieXYZ_31_2[i][1]/(cieXYZ_31_2[i][0]+cieXYZ_31_2[i][1]+cieXYZ_31_2[i][2])

#define xToImage(val) (((float)xO + (val-min_x)*width \
                                              /(max_x-min_x)) + 0.5)
#define yToImage(val) (((float)yO - (val-min_y)*height \
                                              /(max_y-min_y)) + 0.5)
#define imageToX(val) (((val-min_x-xO)*(max_x-min_x)/width))
#define imageToY(val) (((yO-val-min_y)*(max_y-min_y)/height))

#define MIN(a,b) ((a<b)?(a):(b))
#define MAX(a,b) ((a>b)?(a):(b))


double * getSaturationLine_(oyProfile_s * profile, int intent, size_t * size_, oyProfile_s * outspace);
double   bb_spectrum( double wavelength, double bbTemp );
int      oyDXXCIEfromTemperature( double kelvin, float ** SPD );
int      oyXYZtoSRGB ( double * rgb );

typedef struct { /* oyF3 three floats */
  float f[3];
} oyF3;

oyImage_s * oySpectrumCreateEmpty ( int min, int max, int lambda, int columns );
float    oySpectrumFillFromArrayF  ( oyImage_s * spec, float * curve, int pos );
float    oySpectrumFillFromArrayF3 ( oyImage_s * spec, float curves[][3] );
void     oySpectrumNormalise ( oyImage_s * spec, float scale );
typedef enum {
  oySPECTRUM_CHANNELS,
  oySPECTRUM_START,
  oySPECTRUM_END,
  oySPECTRUM_LAMBDA,
  oySPECTRUM_COLUMNS
} oySPECTRUM_PARAM_e;
double   oySpectrumGetParam  ( oyImage_s * spec, oySPECTRUM_PARAM_e mode );
const char* oySpectrumGetString ( oyImage_s * spec, oySPECTRUM_PARAM_e mode );
double   oySpectrumGet( oyImage_s * spec, int column, int channel );
void        oySpectrumSetString ( oyImage_s * spec, oySPECTRUM_PARAM_e mode, char * names );

oyImage_s * oySpectrumFromTree    ( oyjl_val root );
void        oySpectrumToPpm( oyImage_s * spectra, const char * input, const char * output );

oyjl_val    oyTreeFromCxf( const char * text );
oyjl_val    oyTreeFromCsv( const char * text );
oyjl_val    oyTreeFromCgats( const char * text );

int         oyTreeToCgats( oyjl_val root, int * level OYJL_UNUSED, char ** text );
int         oyTreeToCsv( oyjl_val root, int * level, char ** text );

void        oyTreeFilterColors( oyjl_val root, const char * pattern );

typedef enum {
  COLOR_COLOR,
  COLOR_GRAY,
  COLOR_SPECTRAL
} COLOR_MODE;
void drawIlluminant ( cairo_t * cr,
                      oyImage_s * spec, int column,
                      float xO, float yO, float width, float height,
                      float min_x, float max_x, float min_y, float max_y,
                      COLOR_MODE mode, double color[4],
                      uint32_t icc_profile_flags, const char * id );

int verbose = 0;
/* implement cairo_write_func_t */
cairo_status_t oyCairoToStdout(void *closure OYJL_UNUSED,
                       const unsigned char *data,
                       unsigned int length)
{
  fwrite( data, sizeof(unsigned char), length, stdout );
  return CAIRO_STATUS_SUCCESS;
}

static oyjlOptionChoice_s * listProfiles ( oyjlOption_s * x OYJL_UNUSED, int * y OYJL_UNUSED, oyjlOptions_s * z OYJL_UNUSED )
{
  oyProfiles_s * ps = oyProfiles_Create( NULL, 0, 0 );
  int n = oyProfiles_Count( ps ), i;
  oyjlOptionChoice_s * cs = (oyjlOptionChoice_s*) calloc( n+1, sizeof(oyjlOptionChoice_s) );
  for(i = 0; i < n; ++i)
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
  return cs;
}

const char * jcommands = "{\n\
  \"command_set\": \"oyranos-profile-graph\",\n\
  \"comment\": \"command_set_delimiter - build key:value; default is '=' key=value\",\n\
  \"comment\": \"command_set_option - use \\\"-s\\\" \\\"key\\\"; skip \\\"--\\\" direct in front of key\"\n\
}";
int main( int argc , char** argv )
{
  /* the functional switches */
  const char * format = NULL;
  const char * sformat = NULL;
  const char * output = NULL;
  const char * input = NULL;
  int no_spectral = 0;
  int no_blackbody = 1;
  double thickness = 1.0;
  double change_thickness = 0.7;
  int no_border = 0;
  int raster = 0;
  int standardobs = 0, observer64 = 0;
  double kelvin = 0.0;
  const char * illuminant = NULL;
  int no_color = 0;
  int flags = 0;
  int v2 = 0, v4 = 0, no_repair = 0;

  double max_x,max_y,min_x,min_y;
  oyImage_s * spectra = NULL;
  oyF3 * spectra_XYZ = NULL;
  oyF3 white = {{0.0,0.0,0.0}};
  oyjl_val specT = NULL;
  char * string = NULL;
  int spectral_channels = 0;
  int spectral_count = 0;
  double spectra_rect[4] = {1000000.,-1000000.,1000000.,-1000000.}; /* x_min,x_max, y_min,y_max */

  oyImage_s * image = NULL;
  char ** profile_names = 0;
  int profile_count = 0;
  const char * pattern = NULL;

  int i,j;
  cairo_t * cr = 0;
  cairo_surface_t * surface = NULL;
  cairo_status_t status;
  double scale = 1.0,
         frame = 0;
  double pixel_width = 128.0;
  int pixel_w=pixel_width, pixel_h=pixel_width,  /* size in pixel */
      x,y,w=0,h=0;                               /* image dimensions */
  oyProfile_s * p_xyz = oyProfile_FromStd(oyASSUMED_XYZ, flags, 0),
              * p_lab = oyProfile_FromStd(oyASSUMED_LAB, flags, 0),
              * proj = p_lab; /* xyy_plane=0 */
  int xyy_plane = 0;
  double xs_xyz = 1.2,                           /* scaling of CIE*xy graph */
         ys_xyz = 1.2;

  /* spectal variables */
  int nano_min = 64; /* 420 nm */
  int nano_max = 342; /* 700 nm */

  int tab_border_x=0;
  int tab_border_y=0;
  int left_text_border  = 0;
  int lower_text_border = 0;

  float xO,yO,width,height,height_;
  double rgba[4] = {.5,.5,.5,.7};
  double bg_rgba[4] = {.7,.7,.7,.5};

  /* value range */
  min_x=min_y=0.0;
  max_x=max_y=1.0;


#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif
  oyI18NInit_();

  oyjlOptions_s * opts;
  oyjlUi_s * ui;
  oyjlUiHeaderSection_s * info;
  const char * export = NULL;
  int help = 0;
  int verbose = 0;
  int state = 0;

  opts = oyjlOptions_New( argc, (const char**)argv );
  /* nick, name, description, help */
  oyjlOptionChoice_s env_vars[]={ {"OY_DEBUG", _("set the Oyranos debug level."), _("Alternatively the -v option can be used."), _("Valid integer range is from 1-20.")},
                                  {"XDG_DATA_HOME XDG_DATA_DIRS", _("route Oyranos to top directories containing resources. The derived paths for ICC profiles have a \"color/icc\" appended. http://www.oyranos.com/wiki/index.php?title=OpenIccDirectoryProposal"), "", ""},
                                  {"","","",""}};
  oyjlOptionChoice_s examples[]={ {_("Show graph of a ICC profile"), "oyranos-profile-graph", "ICC_PROFILE", ""},
                                  {_("Show the saturation lines of two profiles in CIE*ab 256 pixel width, without spectral line and with thicker lines:"), "oyranos-profile-graph", "-w 256 -s -t 3 sRGB.icc ProPhoto-RGB.icc", ""},
                                  {_("Show the standard observer spectral function as curves:"),"oyranos-profile-graph","--standard-observer -o CIE-StdObserver.png",""},
                                  {"","","",""}};
  oyjlOptionChoice_s illu_dxx[]={ {"A",  _("Illuminant A"),  "", _("CIE A spectral power distribution")},
                                  {"D50",_("Illuminant D50"),"", _("CIE D50 spectral power distribution (computed)")},
                                  {"D55",_("Illuminant D55"),"", _("CIE D55 spectral power distribution (computed)")},
                                  {"D65",_("Illuminant D65"),"", _("CIE D65 spectral power distribution")},
                                  {"D75",_("Illuminant D75"),"", _("CIE D75 spectral power distribution (computed)")},
                                  {"D93",_("Illuminant D93"),"", _("CIE D93 spectral power distribution (computed)")},
                                  {"","","",""}};
  oyjlOptionChoice_s out_form[]={ {"png",_("PNG"),"",_("PNG Raster")},
                                  {"svg",_("SVG"),"",_("SVG Vector")},
                                  {"","","",""}};
  oyjlOptionChoice_s spe_form[]={ {"png",_("PNG"),"",_("PNG Raster")},
                                  {"svg",_("SVG"),"",_("SVG Vector")},
                                  {"csv",_("CSV"),"",_("CSV Values")},
                                  {"ncc",_("NCC"),"",_("Named Color Collection")},
                                  {"cgats",_("CGATS"),"",_("CGATS Values")},
                                  {"ppm",_("PPM"),"",_("Spectral PAM Image")},
                                  {"","","",""}};
  oyjlOption_s oarray[] = {
  /* type,   flags, o, option, key, name, description, help, value_name, value_type, values, var_type, variable */
    {"oiwi", 0,                         '2', "icc-version-2", NULL, _("ICC Version 2"), _("Select ICC v2 Profiles"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&v2} },
    {"oiwi", 0,                         '4', "icc-version-4", NULL, _("ICC Version 4"), _("Select ICC v4 Profiles"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&v4} },
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE, '@', NULL,            NULL, _("Input"),         _("ICC Profile"),            NULL, "l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE", oyjlOPTIONTYPE_FUNCTION, {.getChoices = listProfiles}, oyjlINT, {.i=&profile_count} },
    {"oiwi", 0, 'b', "no-border",     NULL, _("Omit border"),   _("Omit border in graph"),   NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&no_border} },
    {"oiwi", 0, 'c', "no-blackbody",  NULL, _("No black body"), _("Omit white line of lambert light emitters"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&no_blackbody} },
    {"oiwi", 0, 'd', "change-thickness",NULL,_("Thickness increase"),_("Specify increase of the thickness of the graph lines"), NULL, _("NUMBER"), oyjlOPTIONTYPE_DOUBLE,
      {.dbl.start = -1000.0, .dbl.end = 1000.0, .dbl.tick = 0.05, .dbl.d = 0.7}, oyjlDOUBLE, {.d=&change_thickness} },
    {"oiwi", 0, 'f', "format",        NULL, _("Format"),        _("Specify output file format png or svg, default is png"), NULL, _("FORMAT"), oyjlOPTIONTYPE_CHOICE,
      {.choices.list = (oyjlOptionChoice_s*)oyjlStringAppendN( NULL, (const char*)out_form, sizeof(out_form), 0 )}, oyjlSTRING, {.s=&format} },
    {"oiwi", 0, 'g', "no-color",      NULL, _("Gray"),          _("Draw Gray"),              NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&no_color} },
    {"oiwi", 0, 'i', "illuminant",    NULL, _("Illuminant"),    _("Illuminant Spectrum"),    NULL, _("STRING"), oyjlOPTIONTYPE_CHOICE,
      {.choices.list = (oyjlOptionChoice_s*)oyjlStringAppendN( NULL, (const char*)illu_dxx, sizeof(illu_dxx), 0 )}, oyjlSTRING, {.s=&illuminant} },
    {"oiwi", 0, 'k', "kelvin",        NULL, _("Kelvin"),        _("Blackbody Radiator"),     NULL, _("NUMBER"), oyjlOPTIONTYPE_DOUBLE,
      {.dbl.start = 0.0, .dbl.end = 25000.0, .dbl.tick = 100, .dbl.d = 0.0}, oyjlDOUBLE, {.d=&kelvin} },
    {"oiwi", 0, 'n', "no-spectral-line",NULL,_("No spectral"),  _("Omit the spectral line"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&no_spectral} },
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE, 'o', "output",        NULL, _("Output"),        _("Specify output file name, default is stdout"), NULL, _("-|FILE"), oyjlOPTIONTYPE_CHOICE, {}, oyjlSTRING, {.s=&output} },
    {"oiwi", 0, 'p', "spectral-format",NULL,_("Spectral Output"),_("Specify spectral output file format"), NULL, _("FORMAT"), oyjlOPTIONTYPE_CHOICE,
      {.choices.list = (oyjlOptionChoice_s*)oyjlStringAppendN( NULL, (const char*)spe_form, sizeof(spe_form), 0 )}, oyjlSTRING, {.s=&sformat} },
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE, 'P', "pattern",       NULL, _("Pattern"),       _("Filter of Color Names"),  NULL, _("STRING"), oyjlOPTIONTYPE_CHOICE, {}, oyjlSTRING, {.s=&pattern} },
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE, 's', "spectral",      NULL, _("Spectral"),      _("Spectral Input"),         NULL, _("FILE"), oyjlOPTIONTYPE_CHOICE, {}, oyjlSTRING, {.s=&input} },
    {"oiwi", 0, 'S', "standard-observer",NULL,_("Standard Observer"),_("CIE Standard Observer 1931 2°"), NULL,NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&standardobs} },
    {"oiwi", 0, 'O', "observer-64",   NULL, _("10° Observer"),  _("CIE Observer 1964 10°"),  NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&observer64} },

    {"oiwi", 0, 'r', "no-repair",     NULL, _("No repair"),     _("No Profile repair of ICC profile ID"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&no_repair} },
    {"oiwi", 0, 'R', "raster",        NULL, _("Raster"),        _("Draw Raster"),            NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&raster} },
    {"oiwi", 0, 't', "thickness",     NULL, _("Thickness"),     _("Specify the thickness of the graph lines"), NULL, _("NUMBER"), oyjlOPTIONTYPE_DOUBLE,
      {.dbl.start = 0.0001, .dbl.end = 10.0, .dbl.tick = 0.01, .dbl.d = 1.0}, oyjlDOUBLE, {.d=&thickness} },
    {"oiwi", 0, 'w', "width",         NULL, _("Width"),         _("Specify output image width in pixel"), NULL, _("NUMBER"), oyjlOPTIONTYPE_DOUBLE,
      {.dbl.start = 64.0, .dbl.end = 4096.0, .dbl.tick = 1, .dbl.d = 128.0}, oyjlDOUBLE, {.d=&pixel_width} },
    {"oiwi", 0, 'x', "xyy",           NULL, _("xyY"),           _("Use CIE*xyY *x*y plane for saturation line projection"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&xyy_plane} },

    /* default options -h and -v */
    {"oiwi", 0, 'h', "help", NULL, _("help"), _("Help"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&help} },
    {"oiwi", 0, 'v', "verbose", NULL, _("verbose"), _("verbose"), NULL, NULL, oyjlOPTIONTYPE_NONE, {}, oyjlINT, {.i=&verbose} },
    /* default option template -X|--export */
    {"oiwi", 0, 'X', "export", NULL, NULL, NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices.list = NULL}, oyjlSTRING, {.s=&export} },
    /* blind options, useful only for man page generation */
    {"oiwi", 0, '.', "man-environment_variables", NULL, "", "", NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*)oyjlStringAppendN( NULL, (const char*)env_vars, sizeof(env_vars), 0 )}, oyjlNONE, {.i=0} },
    {"oiwi", 0, ',', "man-examples", NULL, "", "", NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*)oyjlStringAppendN( NULL, (const char*)examples, sizeof(examples), 0 )}, oyjlNONE, {.i=0} },
    {"",0,0,0,0,0,0,0, NULL, oyjlOPTIONTYPE_END, {},0,{}}
  };
  opts->array = (oyjlOption_s*)oyjlStringAppendN( NULL, (const char*)oarray, sizeof(oarray), 0 );

  oyjlOptionGroup_s groups[] = {
  /* type,   flags, name, description, help, mandatory, optional, detail */
    {"oiwg", 0, _("Saturation"), _("2D Graph from profiles"), _("Create a 2D Graph containing the saturation line from a ICC Profile."), "@", "tbgwofcxdn24rv", "@dxcn24r" },
    {"oiwg", 0, _("StdObs2°"), _("Standard Observer 1931 2° Graph"), NULL, "S", "tbgwRofv", "S" },
    {"oiwg", 0, _("Obs10°"), _("1964 10° Observer Graph"), NULL, "O", "tbgwRofv", "O" },
    {"oiwg", 0, _("Blackbody Radiator"), _("Blackbody Radiator Spectrum Graph"), NULL, "k", "tbgwRofv", "k" },
    {"oiwg", 0, _("Illuminant Spectrum"), _("Illuminant Spectrum Graph"), NULL, "i", "tbgwRofv", "i" },
    {"oiwg", 0, _("Spectral Input"), _("Spectral Input Graph"), NULL, "s", "tbgwRopv", "spP" },
    {"oiwg", 0, _("Misc"), _("General options"), NULL, "", "", "tbgwRofXvh" },
    {"",0,0,0,0,0,0,0}
  };
  opts->groups = (oyjlOptionGroup_s*)oyjlStringAppendN( NULL, (const char*)groups, sizeof(groups), 0);

  info = oyUiInfo(_("The  oyranos-profile-graph programm converts ICC profiles or embedded ICC profiles from images to a graph image. By default the program shows the saturation line of the specified profiles and writes to stdout."),
                  "2019-03-24T12:00:00", "March 24, 2019");
  ui = oyjlUi_Create( argc, (const char**)argv,
      "oyranos-profile-graph", _("Oyranos Profile Graph"), _("The tool is a ICC color profile grapher."),
      "oyPG-logo",
      info, opts->array, opts->groups, &state );
  if( state & oyjlUI_STATE_EXPORT &&
      export &&
      strcmp(export,"json+command") != 0)
    return 0;
  if(!ui) return 1;

  if(!export && !input && !profile_count && !standardobs && !observer64 && !kelvin && !illuminant)
  {
    oyjlUiHeaderSection_s * version = oyjlUi_GetHeaderSection( ui,
                                                               "version" );
    oyjlOptions_PrintHelp( ui->opts, ui, verbose, "%s v%s - %s", argv[0],
                              version && version->name ? version->name : "",
                              ui->description ? ui->description : "" );
    state = oyjlUI_STATE_HELP;
  }

  if(state & oyjlUI_STATE_HELP)
  {
    fprintf( stderr, "%s\n\tman oyranos-profile-graph\n\n", _("For more information read the man page:"));
    return 0;
  }

  if(verbose)
  {
    char * json = oyjlOptions_ResultsToJson( opts );
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
    exit(0);
  }

  /* detect all anonymous arguments for saturation */
  profile_names = oyjlOptions_ResultsToList( ui->opts, '@', &profile_count );
  pixel_w = pixel_width+0.5;
  if(xyy_plane) proj = p_xyz;
  if(sformat) format = sformat;

  if(profile_count && profile_names && profile_names[0] && profile_names[0][0] == 'l' && profile_names[0][1] == '\000')
  {
    system("oyranos-profiles -le");
    exit(0);
  }


  if(oy_debug)
    fprintf( stderr, "  Oyranos v%s\n",
                  oyNoEmptyName_m_(oyVersionString(1,0)));



#define flags (v2?OY_ICC_VERSION_2:0 | v4?OY_ICC_VERSION_4:0 | no_repair?OY_NO_REPAIR:0)

  if(input)
  {
    size_t size = 0;
    char * text = oyReadFileToMem_( input, &size, NULL );
    double scale = 1.0;
    int data_format = oyjlDataFormat( text );
    char error_buffer[128];
    size_t error_buffer_size = 128;
    if(data_format == oyNAME_JSON)
    {
      specT = oyjlTreeParse( text, error_buffer, error_buffer_size ),
      oyTreeFilterColors( specT, pattern );
      spectra = oySpectrumFromTree( specT );
      if(verbose && spectra) fprintf( stderr, "NCC parsed\n" );
      if(oy_debug) fprintf( stderr, "%s", oyStruct_GetText((oyStruct_s*)spectra, oyNAME_NAME, 0));
    }
    else
    if(data_format == oyNAME_JSON + 1) /* XML */
    {
      specT = oyTreeFromCxf( text );
      if(verbose)
        fprintf( stderr, "XML detected: %s\n", input );

      oyTreeFilterColors( specT, pattern );
      spectra = oySpectrumFromTree( specT );
      if(verbose && spectra) fprintf( stderr, "CxF parsed\n" );
      if(oy_debug) fprintf( stderr, "%s", oyStruct_GetText((oyStruct_s*)spectra, oyNAME_NAME, 0));
    }
    if(!spectra && text)
    {
      specT = oyTreeFromCsv( text );
      oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, input, "description" );
      oyTreeFilterColors( specT, pattern );
      spectra = oySpectrumFromTree( specT );
      if(verbose && spectra)
      if(verbose && spectra) fprintf( stderr, "CSV parsed\n" );
      if(oy_debug) fprintf( stderr, "%s", oyStruct_GetText((oyStruct_s*)spectra, oyNAME_NAME, 0));
    }
    if(!spectra && text)
    {
      specT = oyTreeFromCgats( text );
      oyTreeFilterColors( specT, pattern );
      spectra = oySpectrumFromTree( specT );
      if(verbose && spectra) fprintf( stderr, "CGATS parsed\n" );
      if(oy_debug) fprintf( stderr, "%s", oyStruct_GetText((oyStruct_s*)spectra, oyNAME_NAME, 0));
    }
    if(!spectra && text)
      oyImage_FromFile( input, flags, &image, NULL );

    if(spectra)
    {
      int lambda = oySpectrumGetParam( spectra, oySPECTRUM_LAMBDA );
      int start = oySpectrumGetParam( spectra, oySPECTRUM_START );
      spectral_channels = oySpectrumGetParam( spectra, oySPECTRUM_CHANNELS );
      spectral_count = oySpectrumGetParam( spectra, oySPECTRUM_COLUMNS );
      spectra_XYZ = calloc( spectral_count, sizeof(oyF3) );
      float * illu_5 = NULL; /* 300-830@5nm */
      double kelvin = 5000;
      int error = oyDXXCIEfromTemperature( kelvin, &illu_5 );
      if(error)
        oyMessageFunc_p(oyMSG_ERROR,0,"not CIE illuminant for %g", kelvin);
      for(i = 0; i < spectral_channels; ++i)
      {
        float x = oySpectrumGet( spectra, -1, i );
        if(x < spectra_rect[0]) spectra_rect[0] = x;
        if(x > spectra_rect[1]) spectra_rect[1] = x;
        for(j = 0; j < spectral_count; ++j)
        {
          double y = oySpectrumGet( spectra, j, i );
          {
            int cieXYZ_31_2_startNM = cieXYZ_31_2[0][0];
            int cieXYZ_31_2_start_pos = start - cieXYZ_31_2_startNM;
            int illu_startNM = 300;
            int illu_start_pos = (start - illu_startNM) / 5;
            if(lambda%5 == 0 && start >= cieXYZ_31_2_startNM)
            {
              double illu = illu_5[illu_start_pos + i*lambda/5] / 100.0; /* D50 max ~103 */
              int k;
              for(k = 0; k < 3; ++k)
              {
                double spd = cieXYZ_31_2[1 + cieXYZ_31_2_start_pos + i*lambda][k];
                double weigthed = y * illu * spd;
                spectra_XYZ[j].f[k] += weigthed;
                if(j == 0)
                {
                  weigthed = 1.0 * illu * spd;
                  white.f[k] += weigthed;
                }
              }
            } else
              oyMessageFunc_p( oyMSG_ERROR,0, "need lambda of %5, got: %d; start: %d spd_start: %d", lambda%5, start, cieXYZ_31_2_startNM );
          }
          if(isnan(y))
            continue;
          if(y < spectra_rect[2]) spectra_rect[2] = y;
          if(y > spectra_rect[3]) spectra_rect[3] = y;
        }
      }
      if(fabs(spectra_rect[3]) > fabs(spectra_rect[2]))
        scale = spectra_rect[3];
      else if(spectra_rect[2])
        scale = - spectra_rect[2];
      oySpectrumNormalise( spectra, fabs(1.0/scale) );
      if(spectra_rect[2] < 0.0)
        min_y = spectra_rect[2] / scale;
      max_y = 1.0;

      if(verbose)
      fprintf( stderr, "channels: %d (%d) count: %d (%f %f - %f %f / %f %f) scale: %f\n",
               (int)spectral_channels, (int)size, spectral_count,
               spectra_rect[0], spectra_rect[1], spectra_rect[2], spectra_rect[3], min_y, max_y, scale );
      if(verbose)
      {
        fprintf( stderr, "%s\n", oySpectrumGetString( spectra, oySPECTRUM_CHANNELS ) );
        fprintf( stderr, "white:\t%f %f %f\n", white.f[0], white.f[1], white.f[2] );
        for(j = 0; j < spectral_count; ++j)
        {
          double rgb[4] = { spectra_XYZ[j].f[0]/white.f[0], spectra_XYZ[j].f[1]/white.f[0], spectra_XYZ[j].f[2]/white.f[0], 1.0 };
          oyjl_val v = oyjlTreeGetValueF(specT, 0, "collection/[0]/colors/[%d]/name", j);
          const char * name = OYJL_GET_STRING(v);

          oyXYZtoSRGB( rgb );
          if(j < 10)
          fprintf( stderr, "%d %s\t%f %f %f -> %f %f %f\n", j, name, spectra_XYZ[j].f[0]/white.f[0]*100.0, spectra_XYZ[j].f[1]/white.f[0]*100.0, spectra_XYZ[j].f[2]/white.f[0]*100.0,
                   rgb[0], rgb[1], rgb[2] );
          else if(j == 10)
          fprintf( stderr, "%d\t... more might follow\n", j );
        }
      }
    }
  }

  pixel_h = pixel_w;

  thickness *= pixel_w/128.0;

  /* create a surface to place our images on */
  if(format == NULL || oyStringCaseCmp_(format, "png") == 0)
    surface = cairo_image_surface_create( CAIRO_FORMAT_ARGB32, pixel_w,pixel_h);
  else if(oyStringCaseCmp_(format, "svg") == 0)
  {
    if(!output || strcmp(output,"-") == 0)
      surface = cairo_svg_surface_create_for_stream(
                                          oyCairoToStdout, NULL,
                                          (double)pixel_w, (double)pixel_h);
    else
      surface = cairo_svg_surface_create( output,
                                          (double)pixel_w, (double)pixel_h);
  }
  else
  {
    if(oyStringCaseCmp_(format, "csv") == 0)
    {
      if(specT)
      {
        int level = 0;
        oyTreeToCsv( specT, &level, &string );
        if(!output || strcmp(output,"-") == 0)
          fwrite( string, sizeof(char), strlen(string), stdout );
        else
          oyjlWriteFile( output, string, strlen(string) );
      } else
        oyMessageFunc_p(oyMSG_ERROR,NULL,"no input tree found");
    }
    else if(oyStringCaseCmp_(format, "ncc") == 0)
    {
      if(specT)
      {
        int level = 0;
        oyjlTreeToJson( specT, &level, &string );
        if(!output || strcmp(output,"-") == 0)
          fwrite( string, sizeof(char), strlen(string), stdout );
        else
          oyjlWriteFile( output, string, strlen(string) );
      } else
        oyMessageFunc_p(oyMSG_ERROR,NULL,"no input tree found");
    }
    else if(oyStringCaseCmp_(format, "ppm") == 0)
    {
      oySpectrumToPpm( spectra, input, output );
    }
    else if(oyStringCaseCmp_(format, "cgats") == 0)
    {
      if(specT)
      {
        int level = 0;
        oyTreeToCgats( specT, &level, &string );
        if(!string)
          oyMessageFunc_p(oyMSG_ERROR,NULL,"error in oyTreeToCgats()");
        else
        {
          if(!output || strcmp(output,"-") == 0)
            fwrite( string, sizeof(char), strlen(string), stdout );
          else
            oyjlWriteFile( output, string, strlen(string) );
        }
      } else
        oyMessageFunc_p(oyMSG_ERROR,NULL,"no input tree found");
    }

    oyFinish_( FINISH_IGNORE_I18N | FINISH_IGNORE_CACHES );

    return 0;
  }

  status = cairo_surface_status( surface );
  if(status) return 1;

  cr = cairo_create( surface );

  cairo_set_source_rgba( cr, bg_rgba[0], bg_rgba[1], bg_rgba[2], bg_rgba[3] );
  cairo_rectangle( cr, 0, 0, pixel_w, pixel_h );
  cairo_fill( cr );

  /* single image */
  w = h = 128;
  scale = (pixel_w - pixel_w/10.0)/(double)MAX(w,h);
  x = pixel_w/20.0;
  y = pixel_w/20.0;

  tab_border_x *= scale/2;
  tab_border_y *= scale/2;
  left_text_border *= scale/2;
  lower_text_border *= scale/2;

  /* diagram variables in image points */
  xO = (float)( x +           tab_border_x + left_text_border);     /* origin */
  yO = (float)(-y + pixel_h - tab_border_y - lower_text_border);    /* origin */
  width= (float)(pixel_w - 2*x - 2*tab_border_x - left_text_border); /* width of diagram */
  height_=(float)(pixel_h- 2*y - 2*tab_border_y - lower_text_border); /* height of diagram */
  height = MAX( 0, height_ );

  if(profile_count && no_spectral == 0)
  {
    cairo_set_source_rgba( cr, .0, .0, .0, 1.0);
    if(proj == p_xyz)
    {
      i = nano_min;
      cairo_move_to(cr, xToImage(x_xyY*xs_xyz), yToImage(y_xyY*ys_xyz));
      for(i = nano_min; i<=(int)(nano_max); ++i)
        cairo_line_to(cr, xToImage(x_xyY*xs_xyz), yToImage(y_xyY*ys_xyz));
      i = nano_min;
      cairo_line_to(cr, xToImage(x_xyY*xs_xyz), yToImage(y_xyY*ys_xyz));
      cairo_stroke(cr);
    }

    if(proj == p_lab)
    {
      cairo_new_path(cr);
      for(i = 0; i<=371; ++i)
      {
        double XYZ[3];
        double Lab[3];
        XYZ[0] = cieXYZ_64_10[i][0]; XYZ[1] = cieXYZ_64_10[i][1]; XYZ[2] = cieXYZ_64_10[i][2];
        oyXYZ2Lab( XYZ, Lab);
        if(i == 0)
          cairo_move_to(cr, xToImage(Lab[1]/256.0+0.5),
                            yToImage(Lab[2]/256.0+0.5));
        else
          cairo_line_to(cr, xToImage(Lab[1]/256.0+0.5),
                            yToImage(Lab[2]/256.0+0.5));
      }
      cairo_close_path(cr);
    }
    cairo_stroke(cr);
  }

  if(profile_count && no_blackbody == 0 && proj == p_xyz)
  {
    cairo_set_source_rgba( cr, .0, .0, .0, 1.0);
    if(proj == p_xyz)
    {
      cairo_move_to(cr, xToImage(bb_100K[1][0]*xs_xyz), yToImage(bb_100K[1][1]*ys_xyz));
      for(i = 2; i<bb_100K[0][2]; ++i)
        cairo_line_to(cr, xToImage(bb_100K[i][0]*xs_xyz), yToImage(bb_100K[i][1]*ys_xyz));
      cairo_stroke(cr);
    }

    if(proj == p_lab)
    {
      cairo_new_path(cr);
      for(i = 1; i<bb_100K[0][2]; ++i)
      {
        double xyz[3] = { bb_100K[i][0], bb_100K[i][1], bb_100K[i][2] };
        double XYZ[3] = { xyz[0]/xyz[1], 1.0, xyz[2]/xyz[1] };
        double Lab[3];
        oyXYZ2Lab( XYZ, Lab);
        if(i == 0)
          cairo_move_to(cr, xToImage(Lab[1]/256.0+0.5),
                            yToImage(Lab[2]/256.0+0.5));
        else
          cairo_line_to(cr, xToImage(Lab[1]/256.0+0.5),
                            yToImage(Lab[2]/256.0+0.5));
      }
    }
    cairo_stroke(cr);
  }

  /* draw a frame around the image */
  frame = pixel_w/40.0;
  cairo_set_source_rgba( cr, .0, .0, .0, 1.0);
  cairo_set_line_width (cr, 0.7*thickness);
  if(!no_border)
  cairo_rectangle( cr, x - frame, y - frame,
                         w*scale + 2*frame, h*scale + 2*frame);
  cairo_stroke(cr);
  if(profile_count && no_border == 0 && proj == p_lab)
  {
    /* draw cross */
    cairo_move_to(cr, xToImage(.0), yToImage(.5));
    cairo_line_to(cr, xToImage(1.0), yToImage(.5));
    cairo_move_to(cr, xToImage(.5), yToImage(0));
    cairo_line_to(cr, xToImage(.5), yToImage(1.0));
    cairo_stroke(cr);
  }

#ifdef DEBUG_
  cairo_set_source_rgba( cr, 1.0, 1.0, 1.0, 1.0);
  cairo_rectangle( cr, xO, yO, width, -height );
  cairo_stroke( cr );
#endif

  if(profile_count)
  {
    float t = thickness;
    cairo_set_line_width (cr, 1.*thickness);
    if(proj == p_lab && no_spectral == 0)
    {
      cairo_set_source_rgba( cr, 1., 1., 1., 1.0);
      cairo_move_to(cr, xToImage(.0), yToImage(.5));
      cairo_line_to(cr, xToImage(1.0), yToImage(.5));
      cairo_move_to(cr, xToImage(.5), yToImage(0));
      cairo_line_to(cr, xToImage(.5), yToImage(1.0));
      cairo_stroke(cr);
    }

    cairo_set_line_width (cr, 3.*t);
    for ( j=0; j < profile_count; ++j )
    {
      const char * filename = profile_names[j];
      size_t size = 0;

      oyProfile_s * p = oyProfile_FromName( filename, flags, NULL );
      double * saturation;

      if(!p)
      {
        oyImage_s * image = 0;
        oyImage_FromFile( filename, flags, &image, NULL );
        p = oyImage_GetProfile( image );
      }

      saturation = getSaturationLine_( p, 3, &size, p_lab );

      t = pow( change_thickness, j ) * thickness;
      cairo_set_line_width (cr, 3.*t);

      cairo_new_path(cr);
      if(saturation)
      {
        i = 0;
        if(proj == p_lab)
          cairo_move_to(cr, xToImage(saturation[i*3+1]/1.0),
                            yToImage(saturation[i*3+2]/1.0));
        else
        {
          double XYZ[3];
          double Lab[3];
          Lab[0] = saturation[i*3+0]*100.0;
          Lab[1] = saturation[i*3+1]*256.0-127.0;
          Lab[2] = saturation[i*3+2]*256.0-127.0;
          oyLab2XYZ( Lab, XYZ);
          cairo_line_to(cr, xToImage(XYZ[0]/(XYZ[0]+XYZ[1]+XYZ[2])*xs_xyz),
                            yToImage(XYZ[1]/(XYZ[0]+XYZ[1]+XYZ[2])*ys_xyz));
        }
        for(i = 0; i<(int)size; ++i)
        {
          if(proj == p_lab)
            cairo_line_to(cr, xToImage(saturation[i*3+1]/1.0),
                              yToImage(saturation[i*3+2]/1.0));
          else
          {
            double XYZ[3];
            double Lab[3];
            Lab[0] = saturation[i*3+0]*100.0;
            Lab[1] = saturation[i*3+1]*256.0-127.0;
            Lab[2] = saturation[i*3+2]*256.0-127.0;
            oyLab2XYZ( Lab, XYZ);
            cairo_line_to(cr, xToImage(XYZ[0]/(XYZ[0]+XYZ[1]+XYZ[2])*xs_xyz),
                              yToImage(XYZ[1]/(XYZ[0]+XYZ[1]+XYZ[2])*ys_xyz));
          }
        }
      }
      cairo_close_path(cr);
      cairo_set_source_rgba( cr, 1., 1., 1., 1.0);
      cairo_stroke(cr);

      oyProfile_Release( &p );
    }
  }
  if(image) /* support first line RGB and draw each channel */
  {
    float t = thickness;
    cairo_set_line_width (cr, 0.5*t);
    int channels = oyImage_GetPixelLayout( image, oyCHANS );
    int layout = oyImage_GetPixelLayout( image, oyLAYOUT);
    oyDATATYPE_e data_type = oyToDataType_m( layout );
    int planar = oyToPlanar_m( layout );
    double byteps = oyDataTypeGetSize( data_type );
    double image_width = oyImage_GetWidth(image);
    int iheight = 0, is_allocated = 0;
    char * pixels = oyImage_GetLineF(image)( image, 0, &iheight, -1, &is_allocated );
    uint16_t * line16 = (uint16_t*) pixels;
    uint8_t * line8 = (uint8_t*) pixels;
    printf("byteps: %d\n", (int)byteps);
    for( j=0; j < channels; ++j )
    {
      cairo_new_path(cr);
      if(line16 || line8)
      {
        for(i = 0; i < image_width; ++i)
        {
          double pos = i/image_width;
          int index = planar ? (i + j*image_width) : (i*channels + j);
          double v = byteps == 1 ? line8[index]/256.0 : line16[index]/65536.0;
          if(oy_debug)
            printf("(%d %d)->%d: %g %d\n", i, j, index, v, line16[index]);
          if(i == 0)
          cairo_move_to(cr, xToImage(pos),
                            yToImage(v));
          else
          cairo_line_to(cr, xToImage(pos),
                            yToImage(v));
        }
      }
      /* set RGB channel colors */
      if(j == 0) cairo_set_source_rgba( cr, 1.0, 0.0, 0.0, 1.0);
      if(j == 1) cairo_set_source_rgba( cr, 0.0, 1.0, 0.0, 1.0);
      if(j == 2) cairo_set_source_rgba( cr, 0.0, 0.0, 1.0, 1.0);
      if(j > 2) cairo_set_source_rgba( cr, 1.0, 1.0, 1.0, 1.0);
      cairo_stroke(cr);
    }
  }

  /* default spectral range to draw */
  min_x = 300.;
  max_x = 780.;
  if(standardobs || observer64)
    min_x = 360;
  if(spectra)
  {
    int startNM = oySpectrumGetParam( spectra, oySPECTRUM_START );
    int endNM = oySpectrumGetParam( spectra, oySPECTRUM_END );
    min_x = startNM;
    max_x = endNM;
  }

  if(raster &&
     ( standardobs || observer64 || illuminant || kelvin > 0.0 || spectra ) )
  {
    /* draw some coordinate system hints */
    cairo_set_line_width (cr, 0.35*thickness);
    cairo_set_source_rgba( cr, bg_rgba[0], bg_rgba[1], bg_rgba[2], bg_rgba[3]);
    /* 25 nm */
    for(i = 300; i < max_x; i += 25)
    {
      if(i < min_x) continue;
      cairo_move_to(cr, xToImage(i), yToImage(min_y));
      cairo_line_to(cr, xToImage(i), yToImage(max_y));
    }
    cairo_stroke(cr);

    /* 100 nm */
    cairo_set_line_width (cr, 0.7*thickness);
    for(i = 300; i < max_x; i += 100)
    {
      if(i < min_x) continue;
      cairo_move_to(cr, xToImage(i), yToImage(min_y));
      cairo_line_to(cr, xToImage(i), yToImage(max_y));
    }
    cairo_stroke(cr);

    if(min_y < 0.0)
    {
      /* zero intensity */
      cairo_move_to(cr, xToImage(min_x), yToImage(0.0));
      cairo_line_to(cr, xToImage(max_x), yToImage(0.0));
      cairo_stroke(cr);
    }
  }


#define drawSpectralCurve(spectra, index, r,g,b,a_) \
    { \
      double rgba[4] = {r,g,b,a_}; \
      drawIlluminant( cr, \
                      a, index, \
                      xO, yO, width, height, \
                      min_x, max_x,  min_y < 0 ? min_y : 0.0, max, \
                      no_color ? COLOR_GRAY : COLOR_COLOR, rgba, \
                      flags, id ); \
    }
  cairo_set_line_width (cr, 3.*thickness);
  if(standardobs)
  {
    const char * id = "StdObs 31";
    oyImage_s * a = oySpectrumCreateEmpty ( 360, 830, 1, 3 );
    float max = oySpectrumFillFromArrayF3 ( a, cieXYZ_31_2 );
    oySpectrumNormalise ( a, 1.0/max ); max = 1.0;
    /* draw spectral sensitivity curves from 1931 standard observer */
    drawSpectralCurve(cieXYZ_31_2, 0, 1.,.0,.0, 1.)
    drawSpectralCurve(cieXYZ_31_2, 1, .0,1.,.0,1.)
    drawSpectralCurve(cieXYZ_31_2, 2, .0,.0,1.,1.)
    cairo_stroke(cr);
    oyImage_Release( &a );
  }
  if(observer64)
  {
    oyImage_s * a = oySpectrumCreateEmpty ( 360, 830, 1, 3 );
    float max = oySpectrumFillFromArrayF3 ( a, cieXYZ_64_10 );
    const char * id = "StdObs 64";
    oySpectrumNormalise ( a, 1.0/max ); max = 1.0;
    /* draw spectral sensitivity curves from 1964 standard observer */
    drawSpectralCurve(cieXYZ_64_10, 0, 1.0, .0, .0, 1.)
    drawSpectralCurve(cieXYZ_64_10, 1, .0, 1.0, .0, 1.)
    drawSpectralCurve(cieXYZ_64_10, 2, .0, .0, 1.0, 1.)
    cairo_stroke(cr);
    oyImage_Release( &a );
  }
#undef drawSpectralCurve
  if(kelvin > 0.0)
  {
    /* draw black body spectrum */
    const int lambda = 5,
              max_x_ = 1000,
              len = (max_x_ - 300) / lambda + 2;
    float bb[len], max = 0;
    oyImage_s * a = oySpectrumCreateEmpty ( 300, max_x_, lambda, 1 );
    for(i=0; 300+i*lambda <= max_x_; ++i) bb[i] = bb_spectrum(300+i*lambda, kelvin);
    for(i=0; 300+i*lambda <= max_x_ && 300+i*lambda <= max_x; ++i)
      if(max < bb[i]) max = bb[i];
    oySpectrumFillFromArrayF( a, bb, 0 );
    oySpectrumNormalise ( a, 1.0/max ); max = 1.0;
    drawIlluminant  ( cr,
                      a, 0,
                      xO, yO, width, height,
                      min_x, max_x,  min_y < 0 ? min_y : 0.0, max,
                      no_color ? COLOR_GRAY : COLOR_SPECTRAL, rgba,
                      flags, "kelvin" );
    oyImage_Release( &a );
  }
#define ILLUMINANT( DXX ) \
    if(oyStringCaseCmp_(illuminant, "D" #DXX) == 0) \
    { \
      oyImage_s * a = oySpectrumCreateEmpty ( 300, 830, 5, 1 ); \
      float * spd_5 = NULL; \
      double kelvin = DXX*100; \
      int error = oyDXXCIEfromTemperature( kelvin, &spd_5 ); \
      if(error) \
        oyMessageFunc_p(oyMSG_ERROR,(oyStruct_s*)a,"not CIE illuminant for %g", kelvin); \
      float max = oySpectrumFillFromArrayF( a, spd_5, 0 ); \
      if(verbose) fprintf( stderr, "max: %f\n", max ); \
      oySpectrumNormalise ( a, 1.0/max ); max = 1.0; \
      drawIlluminant( cr, a, 0, xO, yO, width, height, \
                      min_x, max_x,  min_y < 0 ? min_y : 0.0, max, \
                      no_color ? COLOR_GRAY : COLOR_SPECTRAL, rgba, \
                      flags, "D" #DXX ); \
      oyImage_Release( &a ); \
    } else
  if(illuminant != 0)
  {
    if(oyStringCaseCmp_(illuminant,"A") == 0)
    {
      oyImage_s * a = oySpectrumCreateEmpty ( 300, 830, 5, 1 );
      float max = oySpectrumFillFromArrayF( a, spd_A_5, 0 );
      if(verbose) fprintf( stderr, "max: %f\n", max );
      oySpectrumNormalise ( a, 1.0/max ); max = 1.0;
      drawIlluminant( cr,
                      a, 0,
                      xO, yO, width, height,
                      min_x, max_x,  min_y < 0 ? min_y : 0.0, max,
                      no_color ? COLOR_GRAY : COLOR_SPECTRAL, rgba,
                      flags, "A" );
      oyImage_Release( &a );
    } else
    if(oyStringCaseCmp_(illuminant,"SPD") == 0)
    {
      oyImage_s * a = oySpectrumCreateEmpty ( 300, 830, 5, 3 );
      float max = oySpectrumFillFromArrayF3( a, spd_S1S2S3_5 );
      if(verbose) fprintf( stderr, "max: %f\n", max );
      oySpectrumNormalise ( a, 1.0/max ); max = 1.0;
      drawIlluminant( cr, a, 0, xO, yO, width, height,
                      min_x, max_x,  min_y < 0 ? min_y : 0.0, max,
                      no_color ? COLOR_GRAY : COLOR_SPECTRAL, rgba,
                      flags, "S1" );
      drawIlluminant( cr, a, 1, xO, yO, width, height,
                      min_x, max_x,  min_y < 0 ? min_y : 0.0, max,
                      no_color ? COLOR_GRAY : COLOR_SPECTRAL, rgba,
                      flags, "S2" );
      drawIlluminant( cr, a, 2, xO, yO, width, height,
                      min_x, max_x,  min_y < 0 ? min_y : 0.0, max,
                      no_color ? COLOR_GRAY : COLOR_SPECTRAL, rgba,
                      flags, "S3" );
      oyImage_Release( &a );
    } else
    ILLUMINANT( 50 )
    ILLUMINANT( 55 )
    ILLUMINANT( 65 )
    ILLUMINANT( 75 )
    ILLUMINANT( 93 )
    {
    long kelvin = 0, err;
    if((err = oyjlStringToLong(illuminant,&kelvin)) == 0)
    {
      oyImage_s * a = oySpectrumCreateEmpty ( 300, 830, 5, 1 );
      float * spd_5 = NULL;
      int error = oyDXXCIEfromTemperature( kelvin, &spd_5 );
      if(error)
      {
        oyMessageFunc_p(oyMSG_ERROR,(oyStruct_s*)a,"not CIE illuminant for %ld K (valid range: 4000-25000 K)", kelvin);
        return 1;
      }
      float max = oySpectrumFillFromArrayF( a, spd_5, 0 );
      if(verbose) fprintf( stderr, "max: %f\n", max );
      oySpectrumNormalise ( a, 1.0/max ); max = 1.0;
      drawIlluminant( cr, a, 0, xO, yO, width, height,
                      min_x, max_x,  min_y < 0 ? min_y : 0.0, max,
                      no_color ? COLOR_GRAY : COLOR_SPECTRAL, rgba,
                      flags, "D50" );
      oyImage_Release( &a );
    }
    }
  }
  if(spectra)
  {
    int j;
    for(j = 0; j < spectral_count; ++j)
    {
      double rgb[4] = { spectra_XYZ[j].f[0]/white.f[0], spectra_XYZ[j].f[1]/white.f[0], spectra_XYZ[j].f[2]/white.f[0], 1.0 };
      oyXYZtoSRGB( rgb );
      drawIlluminant( cr,
                      spectra, j,
                      xO, yO, width, height,
                      min_x, max_x, min_y < 0 ? min_y : 0.0, max_y,
                      no_color ? COLOR_GRAY : spectral_count == 1 ? COLOR_SPECTRAL : COLOR_COLOR, rgb,
                      flags, input );
    }
    oyImage_Release( &spectra );

    min_x=min_y=0.0;
    max_x=max_y=1.0;
  }

  cairo_restore( cr );

  if(format == NULL || oyStringCaseCmp_(format, "png") == 0)
  {
    if(!output || strcmp(output,"-") == 0)
      status = cairo_surface_write_to_png_stream( surface, oyCairoToStdout,
                                                  NULL);
    else
      status = cairo_surface_write_to_png( surface, output );
  }
  if(status != CAIRO_STATUS_SUCCESS)
  {
    fprintf( stderr, "%s\n", cairo_status_to_string( status ));
    return 1;
  }

  cairo_surface_finish( surface );

  /* clean */
  cairo_surface_destroy( surface );
  oyProfile_Release( &p_lab );
  oyProfile_Release( &p_xyz );


  oyFinish_( FINISH_IGNORE_I18N | FINISH_IGNORE_CACHES );

  return 0;
}

double * getSaturationLine_(oyProfile_s * profile, int intent, size_t * size_, oyProfile_s * outspace)
{
  int size,
      precision = 20;
  oyOption_s * o = NULL;
  oyOptions_s * opts = oyOptions_New(0),
              * result = NULL;
  oyProfile_s * p = oyProfile_Copy( profile, NULL );
  oyOptions_MoveInStruct( &opts, OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "icc_profile.input",
                          (oyStruct_s**) &p,
                          OY_CREATE_NEW );
  p = oyProfile_Copy( outspace, NULL );
  oyOptions_MoveInStruct( &opts, OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "icc_profile.output",
                          (oyStruct_s**) &p,
                          OY_CREATE_NEW );
  oyOptions_SetFromInt( &opts,   OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "rendering_intent",
                          intent, 0, OY_CREATE_NEW );
  oyOptions_SetFromInt( &opts,   OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "precision",
                          precision, 0, OY_CREATE_NEW );

  oyOptions_Handle( "//"OY_TYPE_STD"/graph2d.saturation_line",
                    opts,"saturation_line",
                    &result );

  o = oyOptions_Find( result, "saturation_line.output.double",
                                       oyNAME_PATTERN );
  *size_ = 0;
  size = (int) oyOption_GetValueDouble( o, -1 );
  if(size > 1)
  {
    double * values = oyAllocateFunc_( (size+1) * sizeof(double) );
    int i;
    for(i = 0; i < size; ++i)
      values[i] = oyOption_GetValueDouble( o, i );
    *size_ = size/3;
    return values;
  } else
    fprintf( stderr, "saturation_line contains no lines: %d\n", size );
  return NULL;
}

const char* oySpectrumGetString ( oyImage_s * spec, oySPECTRUM_PARAM_e mode )
{
  oyOptions_s * tags;
  const char * text = NULL;

  if(mode == oySPECTRUM_CHANNELS)
  {
    tags = oyImage_GetTags( spec );
    text = oyOptions_FindString( tags, "SPECTRAL_CHANNEL_NAMES", NULL );
    oyOptions_Release( &tags );
  }

  return text;
}
void        oySpectrumSetString( oyImage_s * spec, oySPECTRUM_PARAM_e mode, char * names )
{
  oyOptions_s * tags = oyImage_GetTags( spec );
  if(mode == oySPECTRUM_CHANNELS)
    oyOptions_SetFromString( &tags,   OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "SPECTRAL_CHANNEL_NAMES",
                             names, OY_CREATE_NEW );
  oyOptions_Release( &tags );
}

void        oySpectrumSetRange( oyImage_s * spec, double min, double max, double lambda )
{
  int           spp  = (max - min + lambda) / lambda;
  oyOptions_s * tags = oyImage_GetTags( spec );
  oyOptions_SetFromDouble( &tags,   OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "SPECTRAL_START_NM",
                        min, 0, OY_CREATE_NEW );
  oyOptions_SetFromDouble( &tags,   OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "SPECTRAL_END_NM",
                        max, 0, OY_CREATE_NEW );
  /* this is redundant */
  oyOptions_SetFromDouble( &tags,   OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "SPECTRAL_BANDS",
                        spp, 0, OY_CREATE_NEW );
  oyOptions_Release( &tags );
}
oyImage_s * oySpectrumCreateEmpty ( int min, int max, int lambda, int columns )
{
  oyPixel_t     pixel_layout = oyDataType_m( oyDOUBLE );
  oyImage_s   * spec = NULL;
  oyPointer     pixels = NULL;
  oyProfile_s * p = oyProfile_FromStd(oyASSUMED_XYZ, 0, 0); 
  int           spp  = (max - min + lambda) / lambda;

  pixel_layout |= oyChannels_m( spp );
  spec = oyImage_Create ( columns, 1, pixels, pixel_layout, p, NULL );
  oySpectrumSetRange( spec, min, max, lambda );

  return spec;
}
float    oySpectrumFillFromArrayF  ( oyImage_s * spec, float * curve, int column )
{
  int channels = oySpectrumGetParam( spec, oySPECTRUM_CHANNELS );
  int i;
  float max = -1000000.0;
  int is_allocated = 0;
  double * dbl;
  if(!spec || !curve) return 0.0;

  dbl = (double*) oyImage_GetPointF(spec)( spec, column,0,-1, &is_allocated );

  for(i = 0; i < channels; ++i)
  {
    float v = curve[i];
    dbl[i] = v;
    if(!isnan(v) && v > max) max = v;
  }
  return max;
}
float    oySpectrumFillFromArrayF3 ( oyImage_s * spec, float curves[][3] )
{
  int channels = oySpectrumGetParam( spec, oySPECTRUM_CHANNELS );
  int i, index;
  float max = -1000000.0;

  for(index = 0; index < 3; ++index)
  {
    int is_allocated = 0;
    double * dbl = (double*) oyImage_GetPointF(spec)( spec, index,0,-1, &is_allocated );
    for(i = 1; i <= channels; ++i)
    {
      float v = curves[i][index];
      dbl[i] = v;
      if(v > max)
        max = v;
    }
  }

  return max;
}
void     oySpectrumNormalise ( oyImage_s * spec, float scale )
{
  int channels = oySpectrumGetParam( spec, oySPECTRUM_CHANNELS );
  int pixels = oySpectrumGetParam( spec, oySPECTRUM_COLUMNS );
  int i, pixel;

  if(scale)
  for(pixel = 0; pixel < pixels; ++pixel)
  {
    int is_allocated = 0;
    double * dbl = (double*) oyImage_GetPointF(spec)( spec, pixel,0,-1, &is_allocated );
    for(i = 0; i < channels; ++i)
      if(!isnan(dbl[i]))
        dbl[i] *= scale;
  }
}
double   oySpectrumGetParam  ( oyImage_s * spec, oySPECTRUM_PARAM_e mode )
{
  double nm = 0;
  oyOptions_s * tags;

  switch(mode)
  {
    case oySPECTRUM_CHANNELS:
         return oyImage_GetPixelLayout( spec, oyCHANS );
    case oySPECTRUM_START: OY_FALLTHROUGH
    case oySPECTRUM_END:
         tags = oyImage_GetTags( spec );
         oyOptions_FindDouble( tags, mode == oySPECTRUM_START ? "SPECTRAL_START_NM" : "SPECTRAL_END_NM",
                               0, &nm );
         oyOptions_Release( &tags );
         break;
    case oySPECTRUM_LAMBDA:
         {
           float start    = oySpectrumGetParam(spec, oySPECTRUM_START),
                 end      = oySpectrumGetParam(spec, oySPECTRUM_END),
                 channels = oySpectrumGetParam(spec, oySPECTRUM_CHANNELS);
           nm = (end - start) / (channels - 1);
         }
         break;
    case oySPECTRUM_COLUMNS:
         nm = oyImage_GetWidth( spec );
         break;
  }

  return nm;
}
double   oySpectrumGet( oyImage_s * spec, int pos, int channel )
{
  int is_allocated = 0;
  double * dbl;

  if(pos < 0)
  {
    /* wave length mode */
    double nm = 0;
    double lambda = oySpectrumGetParam( spec, oySPECTRUM_LAMBDA ),
           start = oySpectrumGetParam( spec, oySPECTRUM_START );

    nm = start + channel*lambda;
    return nm;
  }

  dbl = (double*) oyImage_GetPointF(spec)( spec, pos,0,-1, &is_allocated );
  return dbl[channel];
}
oyConversion_s * oy_xyz_srgb = NULL;
int oyXYZtoSRGB ( double * rgb )
{
  int error = 0;
  int icc_profile_flags = 0;

  if(!oy_xyz_srgb)
  {
    oyProfile_s * pXYZ = oyProfile_FromStd( oyASSUMED_XYZ, icc_profile_flags, 0 ),
                * sRGB = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, 0 );
    oy_xyz_srgb = oyConversion_CreateBasicPixelsFromBuffers (
                                       pXYZ, rgb, oyDataType_m(oyDOUBLE),
                                       sRGB, rgb, oyDataType_m(oyDOUBLE),
                                       0, 1 );
    oyProfile_Release( &sRGB );
    oyProfile_Release( &pXYZ );
  }
  error = !oy_xyz_srgb;
  rgb[3] = 1.0;
  oyConversion_RunPixels( oy_xyz_srgb, 0 );
  return error;
}

void drawIlluminant( cairo_t * cr,
                      oyImage_s * spec, int index,
                      float xO, float yO, float width, float height,
                      float min_x, float max_x, float min_y, float max_y,
                      COLOR_MODE mode, double color[4],
                      uint32_t icc_profile_flags, const char * id )
{
  int channels = oySpectrumGetParam( spec, oySPECTRUM_CHANNELS );
  int start = oySpectrumGetParam( spec, oySPECTRUM_START ),
      end = oySpectrumGetParam( spec, oySPECTRUM_END );
  float max = -1000000.0;

  int lambda = (end - start) / (channels - 1);
  /*  draw spectral power distribution
   *  start and end are not very precise,
   *  just some illustration
   */
  double line_width = cairo_get_line_width( cr );
  int i;
  /* float precission avoids clamping in CIE*XYZ space on input */
  double rgb[4] = {0.0,0.0,0.0,1.0};

  for(i = 0; i < 4; ++i) rgb[i] = color[i];
  cairo_set_source_rgba( cr, rgb[0],rgb[1],rgb[2], rgb[3]);

  if(mode == COLOR_GRAY)
  {
#define setGrayColor(r,g,b,a) \
    cairo_set_source_rgba( cr, (r*0.2+g*0.7+b*0.1)/3.0,(r*0.2+g*0.7+b*0.1)/3.0,(r*0.2+g*0.7+b*0.1)/3.0,a);
    setGrayColor( rgb[0],rgb[1],rgb[2], rgb[3] )
  }

  oyProfile_s * pLab = oyProfile_FromStd( oyASSUMED_LAB, icc_profile_flags, 0 ),
              * sRGB = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, 0 );
  oyConversion_s * lab_srgb = NULL;

  if(mode == COLOR_SPECTRAL)
  {
    lab_srgb = oyConversion_CreateBasicPixelsFromBuffers (
                                       pLab, rgb, oyDataType_m(oyDOUBLE),
                                       sRGB, rgb, oyDataType_m(oyDOUBLE),
                                       0, 1 );
    rgb[3] = 1.0;
  }

  if(verbose && index < 10)
    fprintf( stderr, "drawing*sprectrum %s %d %d nm - %d nm %d nm precission %f", id, index, start, end, lambda, max_y );

  cairo_move_to(cr, xToImage( OY_MAX(start, min_x) ), yToImage( oySpectrumGet(spec, index, 0)));

  for(i = 0; i < channels-1; ++i)
  {
    double XYZ[3] = {0.0,0.0,0.0};
    int j;
    double x0 = oySpectrumGet(spec, -1, i ),
           x1 = oySpectrumGet(spec, -1, i+1 ),
           y0 = oySpectrumGet(spec, index, i),
           y1 = oySpectrumGet(spec, index, i+1);
    cairo_pattern_t * g;

    if(min_x > x0 || x1 > max_x ||
       isnan(y0) || isnan(y1))
    {
      if(verbose > 1) fprintf( stderr, " x0: %g y0: %g/%g\n", x0, y0, y1 );
      if(isnan(y0) || isnan(y1)) break;
      continue;
    }

    if(y0 > max) max = y0;
    if(y1 > max) max = y1;

    if(mode != COLOR_SPECTRAL)
    {
      cairo_line_to(cr, xToImage(x0), yToImage(y0));
      continue;
    }

    g = cairo_pattern_create_linear( xToImage(x0),yToImage(y0),
                                     xToImage(x1),yToImage(y1));

    /* start with previous color */
    cairo_pattern_add_color_stop_rgba(g, 0, rgb[0],rgb[1],rgb[2], rgb[3]);
    if(lab_srgb)
    {
      /* get spectral color from color matching function (CMF) */
      if(360 <= x0 && x0 <= 830)
        for(j = 0; j<3; ++j)
          XYZ[j] = cieXYZ_31_2[(int)x0 - 360][j];
      oyXYZ2Lab( XYZ, rgb );
      rgb[0] /= 100.0; rgb[1] = rgb[1]/256.0+0.5; rgb[2] = rgb[2]/256.0+0.5;
      oyConversion_RunPixels( lab_srgb, 0 );
      /* add different stop */
      cairo_pattern_add_color_stop_rgba(g, 1, rgb[0],rgb[1],rgb[2], rgb[3]);
      /* only one color pattern can be drawn at each cairo_stroke;
       * appears to be a cairo limitation */
      cairo_set_source(cr, g);
    }

    cairo_move_to(cr, xToImage(x0), yToImage(y0));
    /* draw a bit further and avoid empty space between lines */
    cairo_line_to( cr, xToImage(x1), yToImage(y1) );
    /* draw a disconnected single line segment with actual gradient pattern */
    cairo_stroke(cr);

    /* end with a half circle to cover empty areas toward the following line segment */
    if(lab_srgb)
      cairo_set_source_rgba( cr, rgb[0],rgb[1],rgb[2], rgb[3]);

    {
      float yi = (y0-y1) / (max_y - min_y),
            xi = lambda / (max_x - min_x);
      double a_b = atan( yi / xi );
      if(verbose > 1)
        fprintf( stderr, "x0,x1 y0,y1 %g,%g %g,%g  %g %g => %g\n", x0,x1,y0,y1, xi, yi, a_b );
      cairo_arc_negative( cr, xToImage(x1), yToImage(y1), line_width/2.0, a_b+M_PI/2.0, a_b-M_PI/2.0);
    }

    cairo_fill(cr);
  }

  if(mode != COLOR_SPECTRAL)
    cairo_stroke(cr);

  if(verbose && index < 10)
  fprintf( stderr, " (%f) %d %s\n", max, channels, id );
  if(verbose && index == 10)
  fprintf( stderr, "drawing*sprectrum %d ... more might follow\n", index );

  oyProfile_Release( &sRGB );
  oyProfile_Release( &pLab );
  oyConversion_Release( &lab_srgb );
}


/*                            BB_SPECTRUM

    Calculate, by Planck's radiation law, the emittance of a black body
    of temperature bbTemp at the given wavelength (in metres).
    
    source: http://www.fourmilab.ch/documents/specrend/
 */ 
double bb_spectrum(double wavelength, double bbTemp)
{     
    double wlm = wavelength * 1e-9;   /* Wavelength in meters */

    /*  c1 = 3.74183e-16  from 2*M_PI*h*pow(2,c)  2*pi*6.62606957e-34*299792458^2
     *
     *  h - Planck constant                       6.62606957(29)×10−34 J*s
     *  c - light speed                           299 792 458          m/s
     *
     *  c2 = 1.4388e-2    from hc/k               6.62606957e-34*299792458/1.3806488e-23
     *
     *  k - Boltzmann contant                     1.3806488(13)×10−23  J/K
     */
    return (3.74183e-16/*c1*/ * pow(wlm, -5.0)) / (exp(1.4388e-2/*c2*/ / (wlm * bbTemp)) - 1.0); /* W / m² / m */
}

/* xD yD calculation for CIE DXX illuminant */
int      oyxDyDCIEfromTemperature( double kelvin, double * xD, double * yD )
{
  int error = -1;
  double xd = 0.0;
  double yd = 0.0;
  double T = kelvin;

  if(4000 <= kelvin && kelvin <=7000)
  {
    xd = -4.6070*1000000000/(T*T*T) + 2.9678*1000000/(T*T) + 0.09911*1000/T + 0.244063;
    error = 0;
  } else if(7000 < kelvin && kelvin <= 25000)
  {
    xd = -2.0064*1000000000/(T*T*T) + 1.9018*1000000/(T*T) + 0.24748*1000/T + 0.237040;
    error = 0;
  }
  yd = -3.000*xd*xd + 2.870*xd - 0.275;

  *xD = xd;
  *yD = yd;
  
  return error;
}
/* M1 M2 calculation for CIE DXX illuminant */
int      oyM1M2CIEfromTemperature( double kelvin, double * M1, double * M2 )
{
  double xD = 0, yD = 0;
  int error = oyxDyDCIEfromTemperature( kelvin, &xD, &yD );
  double M = 0.0241 + 0.2562*xD - 0.7341*yD;
  *M1 = (-1.3515 - 1.7703*xD + 5.9114*yD) / M;
  *M2 = (0.0300 - 31.4424*xD + 30.0717*yD) / M;
  return error;
}
/* standard CIE DXX illuminant computation, (300-830nm 107step 5nm) */
int      oyDXXCIEfromTemperature( double kelvin, float ** SPD )
{
  int i;
  double M1,M2;
  int error = oyM1M2CIEfromTemperature( kelvin, &M1, &M2 );
  float * spd = NULL;

  if(!error)
    oyAllocHelper_m_( spd, float, 108, oyAllocateFunc_, return 1 )

  if(spd)
    for(i = 0; i < 107; ++i)
      spd[i] = spd_S1S2S3_5[i][0] + M1 * spd_S1S2S3_5[i][1] + M2* spd_S1S2S3_5[i][2];

  *SPD = spd;

  return error;
}

int oyCSVglines( const char * text )
{
  int lines = 0;
  if(text)
  {
    int len = strlen(text), i;
    int last_line_break = 0;
    for(i = 0; i < len; ++i)
      if(text[i] == '\n')
      {
        ++lines;
        last_line_break = i;
      }
    if(last_line_break < len - 3)
      ++lines;
  }
  return lines;
}
double oyCSVparseDouble( const char * text )
{
  double v = -1;
  if(text)
  {
    int r = oyStringToDouble( text, &v );
    if(r == 0) return v;
  }
  return v;
}
int oyIsNumber( const char c )
{
  if(((int)'0' <= (int)c &&
        (int)c <= (int)'9') ||
       c == '.' ||
       c == '-' ||
       c == 'e' ||
       c == 'E' ||
       c == ' '
      )
    return 1;
  return 0;
}
int oyCSVgetColumns( const char * text )
{
  int len = oyStrlen_(text), i;
  int pixels = 0;
  for(i = 0; i < len; ++i)
  {
    char c = text[i];
    int is_line_break = 0;
    if(oyIsNumber(c))
      continue; /* is a number */
    else if(c == ',')
      ++pixels;
    else if(c == '\n')
      is_line_break = 1;
    else
      return pixels;
    if(is_line_break)
      return pixels ? pixels : i?1:0;
  }
  return pixels;
}
const char * oySkipHeaderComment   ( const char * text, const char ** table_header )
{
  if(oyIsNumber( text[0] )) return text;
  *table_header = text;
  while((text = strchr(text, '\n')) != 0)
  {
    ++text;
    if(oyIsNumber( text[0] ))
      return text;
    *table_header = text;
  }
  return NULL;
}
oyjl_val    oyTreeFromCsv( const char * text )
{
  oyjl_val specT = NULL;
  int pixels = 0,
      lines = 0,
      cols = 0;
  float nm, start, end, lambda;
  double d;
  const char * header_start = NULL;
  char * header = NULL, * t, ** hlist = NULL;
  if(!text) return specT;
  text = oySkipHeaderComment( text, &header_start );
  if(header_start)
  {
    header = oyjlStringCopy( header_start, 0 );
    t = strchr( header, '\n' );
    if(t)
      t[0] = '\000';
    hlist = oyjlStringSplit( header, ',', &cols, 0 );
  }
  pixels = oyCSVgetColumns( text );
  lines = oyCSVglines( text );
  if(verbose)
    fprintf( stderr, "lines: %d\n", lines );

  if(pixels >= 1)
  {
    int i,index;
    specT = oyjlTreeNew( "" );
    oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, "ncc1", "type" );
    oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, "Named Color Collection v1", "comment" );
    for(index = 0; index < pixels; ++index)
      if(cols-1 == pixels)
      {
        int len;
        /* remove guarding quotes */
        t = hlist[index + 1];
        if(t[0] == '\"')
          ++t;
        len = strlen(t);
        if(t[len-1] == '\"')
          t[len-1] = '\000';
        oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, t, "collection/[0]/colors/[%d]/name", index + 1 );
      } else
        oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, "", "collection/[0]/colors/[%d]/name", index + 1 );

    for(i = 0; i < lines; ++i)
    {
      const char * endl;

      if(!text) continue;
      endl = strchr( text, '\n' );
      if(!endl)
      {
        int len = strlen(text);
        if(len > 2)
          endl = text + len;
      }
      nm = oyCSVparseDouble(text);
      if(i == 0) start = nm;
      else if(i == 1) lambda = nm - start;
      else if(i == lines-1) end = nm;

      if(verbose > 1) fprintf( stderr, "%d lamda: %f ", i, nm );

      for(index = 0; index < pixels; ++index)
      {
        if(text)
          text = strchr( text, ',' );
        if(text && text < endl && text[0] != '\r' && text[1] != '\r')
        {
          d = oyCSVparseDouble(++text);
          oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, d, "collection/[0]/colors/[%d]/spectral/0/data/[%d]", index, i );
        }
      }

      if(text)
      {
        text = strchr( text, '\n' );
        if(text) ++text;

        if(verbose > 1) fprintf( stderr, "\n" );
      }
    }
    oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, start, "collection/[0]/spectral/[0]/startNM" );
    oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, end, "collection/[0]/spectral/[0]/endNM" );
    oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, lambda, "collection/[0]/spectral/[0]/lambda" );
    oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, (end-start+lambda)/lambda, "collection/[0]/spectral/[0]/steps" );
  }

  return specT;
}

oyjl_val    oyTreeFromCgats( const char * text )
{
  oyOptions_s * opts = oyOptions_New(0),
              * result = NULL;
  oyOption_s * o = oyOption_FromRegistration(OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH "cgats",0);
  oyOption_SetFromString( o, text, 0 );
  oyOptions_MoveIn( opts, &o, -1 );
  oyOption_Release( &o );

  oyOptions_Handle( "//"OY_TYPE_STD"/parse_cgats",
                    opts, "parse_cgats",
                    &result );

  o = oyOptions_Find( result, "cgats.data", oyNAME_PATTERN );
  oyPointer_s * ptr = (oyPointer_s *) oyOption_GetStruct( o, oyOBJECT_POINTER_S );
  oyjl_val specT = (oyjl_val) oyPointer_GetPointer( ptr );

  return specT;
}

/** @brief copy nodes
 *
 *  @param[in]     src                 source tree
 *  @param[in]     in                  source path
 *  @param[out]    dst                 destination tree
 *  @param[in]     out                 destination path
 *  @return                            error: 0 - success; 1 - not found; 2 - no text; 3 - JSON error
 */
int         oyjlCopyNode             ( oyjl_val            src,
                                       const char        * in,
                                       oyjl_val            dst,
                                       const char        * out)
{
  oyjl_val v = oyjlTreeGetValue(src, 0, in);
  char * value;
  if(!v) return 1;
  value = oyjlValueText( v, 0 );
  if(!value) return 2;
  if(oyjlTreeSetStringF( dst, OYJL_CREATE_NEW, value, out ))
    return 3;
  free(value);
  return 0;
}

/* CxF NCC conversion table */
const char * oy_cxf_json[][3] = {
  {"cc:CxF/cc:FileInformation/cc:Description",  "description",  "DESCRIPTOR"},
  {"cc:CxF/cc:FileInformation/cc:CreationDate", "date",         "CREATED"},
  {"cc:CxF/cc:FileInformation/cc:Creator",      "creator",      "ORIGINATOR"},
  {"cc:CxF/cc:Resources/cc:ColorSpecificationCollection/cc:ColorSpecification/@Id",                               "collection/[0]/spectral/[0]/id", ""},
  {"cc:CxF/cc:Resources/cc:ColorSpecificationCollection/cc:ColorSpecification/cc:TristimulusSpec/cc:Illuminant",  "collection/[0]/spectral/[0]/tristimulus/illuminant", ""},
  {"cc:CxF/cc:Resources/cc:ColorSpecificationCollection/cc:ColorSpecification/cc:TristimulusSpec/cc:Observer",    "collection/[0]/spectral/[0]/tristimulus/observer", ""},
  {"cc:CxF/cc:Resources/cc:ColorSpecificationCollection/cc:ColorSpecification/cc:TristimulusSpec/cc:Method",      "collection/[0]/spectral/[0]/tristimulus/method", ""},
  {"cc:CxF/cc:Resources/cc:ColorSpecificationCollection/cc:ColorSpecification/cc:MeasurementSpec/cc:GeometryChoice/cc:SingleAngle/cc:IlluminationAngle", "collection/[0]/spectral/[0]/measurement/illumination_angle", ""},
  {"cc:CxF/cc:Resources/cc:ColorSpecificationCollection/cc:ColorSpecification/cc:MeasurementSpec/cc:GeometryChoice/cc:SingleAngle/cc:MeasurementAngle", "collection/[0]/spectral/[0]/measurement/observer_angle", ""},
  {"cc:CxF/cc:Resources/cc:ColorSpecificationCollection/cc:ColorSpecification/cc:MeasurementSpec/cc:CalibrationStandard", "collection/[0]/spectral/[0]/measurement/calibration", ""},
  {"cc:CxF/cc:Resources/cc:ColorSpecificationCollection/cc:ColorSpecification/cc:MeasurementSpec/cc:Aperture",    "collection/[0]/spectral/[0]/measurement/aperture", ""},
  {"cc:CxF/cc:Resources/cc:ColorSpecificationCollection/cc:ColorSpecification/cc:MeasurementSpec/cc:Device/cc:Model", "collection/[0]/spectral/[0]/measurement/device/model", ""},
  {"cc:CxF/cc:Resources/cc:ColorSpecificationCollection/cc:ColorSpecification/cc:MeasurementSpec/cc:Device/cc:DeviceFilter", "collection/[0]/spectral/[0]/measurement/device/filter", ""},
  {"cc:CxF/cc:Resources/cc:ColorSpecificationCollection/cc:ColorSpecification/cc:MeasurementSpec/cc:Device/cc:DeviceIllumination", "collection/[0]/spectral/[0]/measurement/device/illumination", ""},
  {NULL, NULL, NULL}
};
oyjl_val    oyTreeFromCxf( const char * text )
{
  char error_buffer[128];
  size_t error_buffer_size = 128;
  oyjl_val root = oyjlTreeParseXml( text, 0, error_buffer, error_buffer_size ),
           collection = oyjlTreeGetValue( root, 0, "cc:CxF/cc:Resources/cc:ObjectCollection/cc:Object" ),
           specT = oyjlTreeNew("");
  int count = oyjlValueCount( collection ), i;
  oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, "ncc1", "type" );
  oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, "Named Color Collection v1", "comment" );
  char * startWL = oyjlValueText( oyjlTreeGetValue(root, 0, "cc:CxF/cc:Resources/cc:ColorSpecificationCollection/cc:ColorSpecification/cc:MeasurementSpec/cc:WavelengthRange/@StartWL"), 0 );
  char * increment = oyjlValueText( oyjlTreeGetValue(root, 0, "cc:CxF/cc:Resources/cc:ColorSpecificationCollection/cc:ColorSpecification/cc:MeasurementSpec/cc:WavelengthRange/@Increment"), 0 );
  long startNM = 0, lambda = 0, endNM = 0, n_max = 0;
  i = 0;
  while(oy_cxf_json[i][0])
  {
    int error = 0;
    const char * in = oy_cxf_json[i][0], * out = oy_cxf_json[i][1];
    error = oyjlCopyNode(root, in, specT, out);
    if(i <= 2 && error)
      oyMessageFunc_p( oyMSG_WARN, NULL, "copy from CxF node to NCC node failed: %s %s", in, out );
    ++i;
  }; 

  if(startWL)
  {
    oyjlStringToLong(startWL, &startNM);
    oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, startNM, "collection/[0]/spectral/[0]/startNM" );
    free(startWL);
    startWL = NULL;
  }
  if(increment)
  {
    oyjlStringToLong(increment, &lambda);
    oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, lambda, "collection/[0]/spectral/[0]/lambda" );
    free(increment);
    increment = NULL;
  }


#ifdef USE_GETTEXT
  char * old_loc = strdup(setlocale(LC_ALL,NULL));
  setlocale(LC_ALL,"C");
#endif
  for(i = 0; i < count; ++i)
  {
    oyjl_val obj = oyjlTreeGetValueF( collection, 0, "[%d]", i );
    char * name = oyjlValueText( oyjlTreeGetValue( obj, 0, "@Name" ), 0 ),
         * startWl = oyjlValueText( oyjlTreeGetValue( obj, 0, "cc:ColorValues//@StartWL" ), 0 ),
         * reflSpec_text = oyjlValueText( oyjlTreeGetValue( obj, 0, "cc:ColorValues//@text" ), 0 );
    int n = 0, j;
    long startNm = 0;
    double * list = NULL;
    char * colorSpec = oyjlValueText( oyjlTreeGetValue( obj, 0, "cc:ColorValues//@ColorSpecification" ), 0 );
    if(oyjlStringsToDoubles( reflSpec_text, ' ', &n, 0, &list ))
      fprintf( stderr, "ERROR parsing: %d %s ", i, name );

    if(colorSpec)
      oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, colorSpec, "collection/[0]/colors/[%d]/spectral/[0]/id" );
    if(name)
      oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, name, "collection/[0]/colors/[%d]/name", i );

    if(startWl)
      oyjlStringToLong(startWl, &startNm);
    //fprintf( stderr, "  %d: %s - %ld-%ld ", i, name, startNM, startNM+n*lambda );
    for(j = 0; j < n; ++j)
      oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, list[j], "collection/[0]/colors/[%d]/spectral/[0]/data/[%d]", i, j );
    if(n_max < j-1) n_max = j-1;
  }
  endNM = startNM + lambda * n_max;
  oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, endNM, "collection/[0]/spectral/[0]/endNM" );
  if(lambda)
    oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, (endNM-startNM+lambda)/lambda, "collection/[0]/spectral/[0]/steps" );

#ifdef USE_GETTEXT
  setlocale(LC_ALL,old_loc);
  free(old_loc);
#endif

  oyjlTreeFree(root);
  return specT;
}

/* return data section */
oyjl_val oyTreeGetParam( oyjl_val root, double *lambda, double *startNM, double *endNM, int *pixels, int *spp, char ** creator, char ** creation_date, char ** description )
{
  oyjl_val v;
  int is_ncc1 = 0;

  if(!root) return NULL;

  v = oyjlTreeGetValueF( root, 0, "type" );
  if(v) { char * name = OYJL_GET_STRING(v); if(strcmp(name,"ncc1") == 0) is_ncc1 = 1; }

  v = oyjlTreeGetValue( root, 0, "collection/[0]/spectral/[0]/startNM" );
  if(!v) { oyMessageFunc_p(oyMSG_ERROR,NULL,"startNM missed"); return NULL; }
  else *startNM = v->u.number.d;

  v = oyjlTreeGetValue( root, 0, "collection/[0]/spectral/[0]/endNM" );
  if(!v) { oyMessageFunc_p(oyMSG_ERROR,NULL,"endNM missed"); return NULL; }
  else *endNM = v->u.number.d;

  v = oyjlTreeGetValue( root, 0, "collection/[0]/spectral/[0]/lambda" );
  if(!v) { oyMessageFunc_p(oyMSG_ERROR,NULL,"lambda missed"); return NULL; }
  else *lambda = v->u.number.d;

  v = oyjlTreeGetValue( root, 0, "creator" );
  if(!v) { oyMessageFunc_p(oyMSG_WARN,NULL,"creator missed"); *creator = NULL; }
  else *creator = oyjlStringCopy( v->u.string, 0 );

  v = oyjlTreeGetValue( root, 0, "date" );
  if(!v) { oyMessageFunc_p(oyMSG_WARN,NULL,"date missed"); *creation_date = NULL; }
  else *creation_date = oyjlStringCopy( v->u.string, 0 );

  v = oyjlTreeGetValue( root, 0, "description" );
  if(!v) { oyMessageFunc_p(oyMSG_WARN,NULL,"description missed"); *description = NULL; }
  else *description = oyjlStringCopy( v->u.string, 0 );

  *spp = (*endNM - *startNM) / *lambda + 1;

  v = oyjlTreeGetValue( root, 0, "collection/[0]/colors" );
  if(!v) { oyMessageFunc_p(oyMSG_WARN,NULL,"colors missed"); return NULL; }
  *pixels = oyjlValueCount( v );

  if(verbose)
    oyMessageFunc_p( oyMSG_DBG,NULL, "colors: %d samples: %d isncc1: %d", pixels, spp, is_ncc1 );

  return oyjlTreeGetValue(root, 0, "collection/[0]/colors");
}

oyImage_s * oySpectrumFromTree       ( oyjl_val root )
{
  oyImage_s * spec = NULL;
  int pixels = 0,
      samples = 0;
  oyjl_val v, data;
  double  start, end, lambda;
  char * channel_names = NULL,
       * name, * creator, * creation_date, * description;
  int is_ncc1 = 0;

  if(!root) return spec;
  data = oyTreeGetParam( root, &lambda, &start, &end, &pixels, &samples, &creator, &creation_date, &description );
  v = oyjlTreeGetValueF( root, 0, "type" );
  if(v) { name = OYJL_GET_STRING(v); if(strcmp(name,"ncc1") == 0) is_ncc1 = 1; }
  if(!is_ncc1) return spec;
  else fprintf(stderr, "is_ncc1 = %d\n", is_ncc1);

  if(data && pixels >= 1)
  {
    int is_allocated = 0;
    double * dbl, d;
    int i,index;
    spec = oySpectrumCreateEmpty( start, end, lambda, pixels );
    for(index = 0; index < pixels; ++index)
    {
      v = oyjlTreeGetValueF( data, 0, "[%d]/name", index );
      name = NULL;
      if(v) name = OYJL_GET_STRING(v);
      oyjlStringAdd( &channel_names, 0,0, "%s\"%s\"", index?",":"", name?name:"" );
      is_allocated = 0;
      dbl = (double*) oyImage_GetPointF(spec)( spec, index,0,-1, &is_allocated );
      for(i = 0; i < samples; ++i)
      {
        v = oyjlTreeGetValueF( data, 0, "[%d]/spectral/[0]/data/[%d]", index, i );
        if(!v)
        {
          fprintf(stderr, "ERROR with i = %d index = %d\n", i, index);
          d = NAN;
        } else
          d = OYJL_GET_DOUBLE(v);
        dbl[i] = d;
      }
    }
    oySpectrumSetString( spec, oySPECTRUM_CHANNELS, channel_names );
  }

  return spec;
}

int oyTreeToCgats( oyjl_val root, int * level OYJL_UNUSED, char ** text )
{
  oyjl_str t = NULL;
  int i,index;

  int pixels = 0,
      samples = 0;
  oyjl_val v, data;
  double  start, end, lambda;
  char * name, * creator, *creation_date, * description;
  double d;

  if(!root) return 1;
  data = oyTreeGetParam( root, &lambda, &start, &end, &pixels, &samples, &creator, &creation_date, &description );

#ifdef USE_GETTEXT
  char * old_loc = strdup(setlocale(LC_ALL,NULL));
  setlocale(LC_ALL,"C");
#endif

  if(data)
  {
    char * tmp = NULL;
    oyjlStringAdd( &tmp, 0,0, "CGATS\n\nDESCRIPTOR \"%s\"\nORIGINATOR \"%s\"\n",
      description?description:"Spectral Data", creator?creator:"Oyranos CMS" );
    if(creation_date)
      oyjlStringAdd( &tmp, 0,0, "CREATED \"%s\"\n", creation_date );

    /* compile a INSTRUMENTATION line */
    oyjl_val v = oyjlTreeGetValue( root, 0, "collection/[0]/spectral/[0]/tristimulus/illuminant" );
    const char  * illuminant = v ? v->u.string : NULL;
    v = oyjlTreeGetValue( root, 0, "collection/[0]/spectral/[0]/tristimulus/observer" );
    const char  * observer = v ? v->u.string : NULL;
    v = oyjlTreeGetValue( root, 0, "collection/[0]/spectral/[0]/measurement/illumination_angle" );
    const char  * illumination_angle = v ? v->u.string : NULL;
    v = oyjlTreeGetValue( root, 0, "collection/[0]/spectral/[0]/measurement/observer_angle" );
    const char  * observer_angle = v ? v->u.string : NULL;
    v = oyjlTreeGetValue( root, 0, "collection/[0]/spectral/[0]/measurement/device/model" );
    const char  * model = v ? v->u.string : NULL;
    v = oyjlTreeGetValue( root, 0, "collection/[0]/spectral/[0]/measurement/device/filter" );
    const char  * filter = v ? v->u.string : NULL;
    if(illuminant || observer || (illumination_angle && observer_angle) || model)
      oyjlStringAdd( &tmp, 0,0, "INSTRUMENTATION \"%s %s %s %s%s%s%s %s\"\n",
          model?model:"", illuminant?illuminant:"", observer?observer:"",
          illumination_angle?"geometry_":"", illumination_angle?illumination_angle:"", observer_angle?"/":"", observer_angle?observer_angle:"",
          filter?filter:"" );
    /* compile a MEASUREMENT_SOURCE line */
    v = oyjlTreeGetValue( root, 0, "collection/[0]/spectral/[0]/measurement/device/illumination" );
    const char  * illumination = v ? v->u.string : NULL;
    if(illumination)
      oyjlStringAdd( &tmp, 0,0, "MEASUREMENT_SOURCE \"%s\"\n", illumination );

    oyjlStringAdd( &tmp, 0,0, "SPECTRAL_BANDS \"%d\"\nSPECTRAL_START_NM \"%f\"\nSPECTRAL_END_NM \"%f\"\nSPECTRAL_NORM \"%f\"\n\nKEYWORD \"SAMPLE_NAME\"\nNUMBER_OF_FIELDS %d\nBEGIN_DATA_FORMAT\nSAMPLE_NAME ",
      samples, start, end, lambda, 1 + samples );

    for(i = 0; i < samples; ++i)
    {
      int nm = start + i*lambda;
      oyjlStringAdd( &tmp, 0,0, "%sSPEC_%d", i?" ":"", nm );
    }
    oyjlStringAdd( &tmp, 0,0, "\nEND_DATA_FORMAT\n\nNUMBER_OF_SETS %d\nBEGIN_DATA\n", (int)pixels );
    t = oyjlStrNewFrom(&tmp,0,0,0);
  }

  if(t && pixels >= 1)
  {
    for(index = 0; index < pixels; ++index)
    {
      v = oyjlTreeGetValueF( data, 0, "[%d]/name", index );
      name = OYJL_GET_STRING(v);
      oyjlStrAppendN( t, "\"", 1 );
      oyjlStrAppendN( t, name, strlen(name) );
      oyjlStrAppendN( t, "\" ", 2 );
      for(i = 0; i < samples; ++i)
      {
        char f[32];
        v = oyjlTreeGetValueF( data, 0, "[%d]/spectral/[0]/data/[%d]", index, i );
        if(!v)
        {
          fprintf(stderr, "ERROR with i = %d index = %d (spp = %d pixels = %d)\n", i, index, samples, pixels);
          d = NAN;
        } else
          d = OYJL_GET_DOUBLE(v);
        if(i)
          oyjlStrAppendN( t, " ", 1 );
        sprintf(f, "%f", d);
        oyjlStrAppendN( t, f, strlen(f) );
      }
      oyjlStrAppendN( t, "\n", 1 );
    }
    oyjlStrAppendN( t, "END_DATA\n", 9 );
  }

#ifdef USE_GETTEXT
  setlocale(LC_ALL,old_loc);
  free(old_loc);
#endif

  if(!t) return 1;

  *text = oyjlStrPull( t );
  oyjlStrRelease( &t );
  return 0;
}

void oySpectrumToPpm( oyImage_s * spectra, const char * input, const char * output )
{
  if(spectra)
  {
    char * t = oyStringCopy( output?output:"output", 0 );
    char * comment = input?oyStringCopy( input, 0 ):NULL;
    double lambda = oySpectrumGetParam( spectra, oySPECTRUM_LAMBDA ),
           start = oySpectrumGetParam( spectra, oySPECTRUM_START ),
           end = oySpectrumGetParam( spectra, oySPECTRUM_END ),
           channels = oySpectrumGetParam( spectra, oySPECTRUM_CHANNELS );
    const char * channel_names = oySpectrumGetString( spectra, oySPECTRUM_CHANNELS);

    oyStringAddPrintf( &comment, 0,0, "\n SPECTRAL_NORM: %g", lambda );
    oyStringAddPrintf( &comment, 0,0, "\n SPECTRAL_START_NM: %g", start );
    oyStringAddPrintf( &comment, 0,0, "\n SPECTRAL_END_NM: %g", end );
    oyStringAddPrintf( &comment, 0,0, "\n SPECTRAL_BANDS: %g", channels );
    oyStringAddPrintf( &comment, 0,0, "\n SPECTRAL_CHANNEL_NAMES: %s", channel_names );

    oyImage_WritePPM( spectra, t, comment );
    oyFree_m_(t);
    oyFree_m_(comment);
  }
  else
    oyMessageFunc_p(oyMSG_ERROR,NULL,"no input spectra found");
}

/* convert two dimensional array to CSV */
int oyTreeToCsv( oyjl_val root, int * level OYJL_UNUSED, char ** text )
{
  oyjl_str t = NULL;
  int i,index;

  int pixels = 0,
      samples = 0;

  oyjl_val v, data;
  double  start, end, lambda;
  char * name, * creator, *creation_date, * description;
  double d;

  if(!root) return 1;
  data = oyTreeGetParam( root, &lambda, &start, &end, &pixels, &samples, &creator, &creation_date, &description );

#ifdef USE_GETTEXT
  char * old_loc = strdup(setlocale(LC_ALL,NULL));
  setlocale(LC_ALL,"C");
#endif

  if(data && pixels >= 1)
    t = oyjlStrNew(0,0,0);
  if(t)
  {
    oyjlStrAppendN( t, "\"Wavelength (nm)/Name\"", 22 );
    for(index = 0; index < pixels; ++index)
    {
      v = oyjlTreeGetValueF( data, 0, "[%d]/name", index );
      name = OYJL_GET_STRING(v);
      oyjlStrAppendN( t, ",\"", 2 );
      if(name)
        oyjlStrAppendN( t, name, strlen(name) );
      oyjlStrAppendN( t, "\"", 1 );
    }
    oyjlStrAppendN( t, "\n", 1 );
    for(i = 0; i < samples; ++i)
    {
      for(index = 0; index < pixels; ++index)
      {
        char f[32];
        if(index == 0)
        {
          sprintf(f, "%d", (int)(start + i*lambda));
          oyjlStrAppendN( t, f, strlen(f) );
        }

        v = oyjlTreeGetValueF( data, 0, "[%d]/spectral/[0]/data/[%d]", index, i );
        if(!v)
        {
          fprintf(stderr, "ERROR with i = %d index = %d (spp = %d pixels = %d)\n", i, index, samples, pixels);
          d = NAN;
        } else
          d = OYJL_GET_DOUBLE(v);
        sprintf(f, "%f", d);
        oyjlStrAppendN( t, ",", 1 );
        oyjlStrAppendN( t, f, strlen(f) );
      }
      oyjlStrAppendN( t, "\n", 1 );
    }
  }

#ifdef USE_GETTEXT
  setlocale(LC_ALL,old_loc);
  free(old_loc);
#endif

  if(!t) return 1;

  *text = oyjlStrPull( t );
  oyjlStrRelease( &t );
  return 0;
}

int oyRegExpMatch( const char * text, const char * pattern )
{
  int status = 0;
  regex_t re;
  if(regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0)
    return 0;
  status = regexec( &re, text, (size_t)0, NULL, 0 );
  regfree( &re );
  if(status != 0)
    return 0;
  return 1;
}

void        oyTreeFilterColors( oyjl_val root, const char * pattern )
{
  oyjl_val data, v;
  int index, pixels, match;
  const char * name;
  char * num;

  if(!pattern) return;
  data = oyjlTreeGetValue(root, 0, "collection/[0]/colors");
  if(!data) { oyMessageFunc_p(oyMSG_WARN,NULL,"colors missed"); return; }
  pixels = oyjlValueCount( data );

  num = oyjlStringCopy( "10000000000000000", 0 );
  if(!num) return;
  for(index = pixels - 1; index >= 0; --index)
  {

    v = oyjlTreeGetValueF( data, 0, "[%d]/name", index );
    name = OYJL_GET_STRING(v);
    if(!name) continue;
    match = oyRegExpMatch( name, pattern );
    if(pattern && !match )
    {
      sprintf( num, "[%d]", index );
      oyjlTreeClearValue(data, num);
    }
  }
  free(num);
}

