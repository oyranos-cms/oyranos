/** @file oyranos_profile_graph.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  Copyright (C) 2012-2021  Kai-Uwe Behrmann
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

#include <stddef.h>

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
#include <oyjl_macros.h>

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

#ifdef SKIP_GETTEXT
# ifdef OYJL_USE_GETTEXT
# undef OYJL_USE_GETTEXT
# endif
#endif


#include "ciexyz31_2.h" /* cieXYZ_31_2 1931 2° */
#include "ciexyz64_1.h"
#include "spd_A_5.h"
#include "spd_D65_5.h"
#include "spd_S1S2S3_5.h"
#include "HLC_EPV_M0_V2-3.h"

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
int      oyXYZ2sRGB ( double * rgb );
void     oyLCh2Lab                   ( const double        i[],
                                       double              o[],
                                       void              * none OY_UNUSED );
int      oyLabGamutCheck             ( double            * lab,
                                       int                 count,
                                       oyProfiles_s      * proofing,
                                       int               * is_in_gamut,
                                       double            * lab_tested );

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
int         oySpectrumComputeWhite( oyImage_s * spectra, int stdobs, float * illu_300_830_5nm, oyF3 *white, double * spectra_rect );
int         oySpectrumComputeXYZ( oyImage_s * spectra, int index, float * illu_300_830_5nm, int stdobs, oyF3 *xyz );

oyImage_s * oySpectrumFromTree    ( oyjl_val root );
void        oySpectrumToPpm( oyImage_s * spectra, const char * input, const char * output );

void        oySpecCompute ( int start, int lambda, int channel, int pos, oyF3 *xyz, oyF3 *white, double y, int stdobs, float * illu_spd );

oyjl_val    oyTreeFromCxf( const char * text );
oyjl_val    oyTreeFromCsv( const char * text );
oyjl_val    oyTreeFromCgats( const char * text );

int         oyTreeToCgats( oyjl_val root, int * level OYJL_UNUSED, char ** text );
int         oyTreeToCsv( oyjl_val root, int * level, char ** text );
int         oyTreeToIccXml( oyjl_val root, int * level OYJL_UNUSED, char ** text );

void        oyTreeFilterColors( oyjl_val root, const char * pattern );

typedef enum {
  COLOR_COLOR,
  COLOR_GRAY,
  COLOR_SPECTRAL
} COLOR_MODE;
#define OY_DRAW_ID 0x01
void drawIlluminant ( cairo_t * cr,
                      oyImage_s * spec, int column,
                      float xO, float yO, float width, float height,
                      float min_x, float max_x, float min_y, float max_y,
                      COLOR_MODE mode, double color[4],
                      uint32_t icc_profile_flags, const char * id, int flags,
                      int frame, double thickness );
#define OY_NO_DRAW 0x1
void drawTextTopLeft( cairo_t * cr,
                      double background_lightness, int frame,
                      double xO, double yO, double width, double height,
                      double min_x, double max_x, double min_y, double max_y,
                      int flags, double * text_width,
                      char * format, ... );

int         oyjlCopyNode             ( oyjl_val            src,
                                       const char        * in,
                                       oyjl_val            dst,
                                       const char        * out,
                                       int                 out_pos);
oyjl_val    oyjlTreeGetValueFilteredF( oyjl_val root, int flag, const char * filter, const char * format, ... );

static int verbose = 0;
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
  oyjlOptionChoice_s * cs = (oyjlOptionChoice_s*) calloc( (unsigned int)n+20, sizeof(oyjlOptionChoice_s) );
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

static oyjlOptionChoice_s * listInput ( oyjlOption_s * o OYJL_UNUSED, int * y OYJL_UNUSED, oyjlOptions_s * opts OYJL_UNUSED )
{
  oyjlOptionChoice_s * c = NULL;

  int size = 0, i,n = 0;
  char * result = oyjlReadCommandF( &size, "r", malloc, "ls -1 *.[N,n][C,c][C,c]" );
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

/* find ncc/cgats/json/cxf files */
static oyjlOptionChoice_s * listSpectral ( oyjlOption_s * o OYJL_UNUSED, int * y OYJL_UNUSED, oyjlOptions_s * opts OYJL_UNUSED )
{
  oyjlOptionChoice_s * c = NULL;

  if(oy_debug)
    fputs("listSpectral", stderr);
  int size = 0, i = 0, n = 0;
  char * result = oyjlReadCommandF( &size, "r", malloc, "ls -1 *.[N,n][C,c][C,c] *.[C,c][G,g][A,a][T,t][S,s] *.[J,j][S,s][O,o][N,n] *.[C,c][X,x][F,f]" );
  char ** list = oyjlStringSplit( result, '\n', &n, 0 );

  c = calloc(n+2, sizeof(oyjlOptionChoice_s));
  if(c)
  {
    if(list)
    {
      for(i = 0; i < n; ++i)
      {
        c[i].nick = strdup( list[i] );
        c[i].name = strdup("");
        c[i].description = strdup("");
        c[i].help = strdup("");
      }

      free(list);
      --i;
    }

    c[i].nick = strdup( "HLC_EPV_M0_V2_3" );
    c[i].name = strdup("");
    c[i].description = strdup("");
    c[i].help = strdup("");
  }

  return c;
}

static oyjlOptionChoice_s * listPages ( oyjlOption_s * x OYJL_UNUSED, int * y OYJL_UNUSED, oyjlOptions_s * opts )
{
  OYJL_GET_RESULT_STRING( opts, "import", NULL, input );
  oyjlOptionChoice_s * cs = NULL;

  if(input)
  {
    char * fn = oyMakeFullFileDirName_( input );
    size_t size = 0;
    char * text = oyReadFileToMem_( fn, &size, NULL );
    int data_format = oyjlDataFormat( text );
    oyjl_val specT = NULL;
    if(data_format == oyNAME_JSON)
    {
      specT = oyJsonParse( text, NULL );
      if(specT)
      {
        int i, pages;
        char ** paths = oyjlTreeToPaths( oyjlTreeGetValue( specT, 0, "collection/[0]/pages" ), 1, NULL, OYJL_PATH, &pages );
        cs = (oyjlOptionChoice_s*) calloc( (unsigned)pages+1, sizeof(oyjlOptionChoice_s) );
        for(i = 0; i < pages; ++i)
        {
          cs[i].name = oyjlStringCopy( paths[i], 0 );
          cs[i].nick = oyjlStringCopy( paths[i], 0 );
        }
        oyjlTreeFree(specT);
      }
    }
    if(text) free(text);
    if(fn) free(fn);
  }
  return cs;
}

static const char * jcommands = "{\n\
  \"command_set\": \"oyranos-profile-graph\"\n\
}";
/* This function is called the
 * * first time for GUI generation and then
 * * for executing the tool.
 */
int myMain( int argc, const char ** argv )
{
  /* the functional switches */
  const char * format = NULL;
  const char * output = NULL;
  const char * input = NULL;
  const char * page = NULL;
  int no_spectral = 0;
  int no_blackbody = 0;
  double thickness = 1.0;
  double change_thickness = 0.7;
  int no_border = 0;
  int raster = 0;
  int standardobs = 0, observer64 = 0;
  double kelvin = 0.0;
  const char * illuminant = NULL;
  double hue = -1.0;
  double lightness = -1.0;
  double chroma = -1.0;
  double background_lightness = -1;
  int no_color = 0;
  uint32_t flags = 0;
  int v2 = 0, v4 = 0, no_repair = 0;

  double max_x,max_y,min_x,min_y;
  oyImage_s * spectra = NULL;
  oyF3 * spectra_XYZ = NULL;
  oyF3 white = {{0.0,0.0,0.0}};
  double CIE_Y_max = 1.0;
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
  int scale_spectrum = 0;
  double pixel_width = 128.0;
  int pixel_w=(int)pixel_width, pixel_h=(int)pixel_width,  /* size in pixel */
      x,y,w=0,h=0;                               /* image dimensions */
  oyProfile_s * p_xyz = oyProfile_FromStd(oyASSUMED_XYZ, flags, 0),
              * p_lab = oyProfile_FromStd(oyASSUMED_LAB, flags, 0),
              * proj = p_lab; /* xyy_plane=0 */
  int xyy_plane = 0;
  double xs_xyz = 1.2,                           /* scaling of CIE*xy graph */
         ys_xyz = 1.2;

#if defined(__ANDROID__) || !defined(OYJL_USE_GETTEXT)
  fprintf(stderr, OYJL_DBG_FORMAT "%s\n", OYJL_DBG_ARGS, argv[0] );
#endif

  /* spectal variables */
  int nano_min = 64; /* 420 nm */
  int nano_max = 342; /* 700 nm */

  int tab_border_x=0;
  int tab_border_y=0;
  int left_text_border  = 0;
  int lower_text_border = 0;

  float xO,yO,width,height,height_;
  double rgba[4] = {.45,.45,.45,.7};
  double bg_rgba[4] = {.7,.7,.7,.5};
  char * top_left_text = NULL;

  /* value range */
  min_x=min_y=0.0;
  max_x=max_y=1.0;

#if defined(__ANDROID__)
  pixel_width = 1024.0;
#endif

  oyjlOptions_s * opts;
  oyjlUi_s * ui;
  oyjlUiHeaderSection_s * info, * info_tmp;
  const char * export = NULL;
  const char * render = NULL;
  int help = 0;
  int verbose = 0;
  int version = 0;
  int state = 0;

  opts = oyjlOptions_New( argc, (const char**)argv );
  /* nick, name, description, help */
  oyjlOptionChoice_s env_vars[]={ {"OY_DEBUG", _("set the Oyranos debug level."), _("Alternatively the -v option can be used."), _("Valid integer range is from 1-20.")},
                                  {"XDG_DATA_HOME XDG_DATA_DIRS", _("route Oyranos to top directories containing resources. The derived paths for ICC profiles have a \"color/icc\" appended. http://www.openicc.org/index.php%3Ftitle=OpenIccDirectoryProposal.html"), "", ""},
                                  {NULL,NULL,NULL,NULL}};
  oyjlOptionChoice_s examples[]={ {_("Show graph of a ICC profile"), "oyranos-profile-graph ICC_PROFILE", "", ""},
                                  {_("Show the saturation lines of two profiles in CIE*ab 256 pixel width, without spectral line and with thicker lines:"), "oyranos-profile-graph -w 256 -n -t 3 sRGB.icc ProPhoto-RGB.icc", "", ""},
                                  {_("Show HLC Color Atlas patches"),"oyranos-profile-graph -H=90 -o HLC_H090.png cmyk web",_("Color patches are only shown, if they are in gamut of the default CMYK and web profile."), ""},
                                  {_("Show the standard observer spectral function as curves:"),"oyranos-profile-graph --standard-observer -o CIE-StdObserver.png","", ""},
                                  {NULL,NULL,NULL,NULL}};
  oyjlOptionChoice_s illu_dxx[]={ {"A",  _("Illuminant A"),  "", _("CIE A spectral power distribution")},
                                  {"D50",_("Illuminant D50"),"", _("CIE D50 spectral power distribution (computed)")},
                                  {"D55",_("Illuminant D55"),"", _("CIE D55 spectral power distribution (computed)")},
                                  {"D65",_("Illuminant D65"),"", _("CIE D65 spectral power distribution (computed)")},
                                  {"D65T",_("Illuminant D65 T"),"",_("CIE D65 spectral power distribution")},
                                  {"D75",_("Illuminant D75"),"", _("CIE D75 spectral power distribution (computed)")},
                                  {"D93",_("Illuminant D93"),"", _("CIE D93 spectral power distribution (computed)")},
                                  {NULL,NULL,NULL,NULL}};
  oyjlOptionChoice_s out_form[]={ {"png",_("PNG"),"",_("PNG Raster")},
                                  {"svg",_("SVG"),"",_("SVG Vector")},
                                  {NULL,NULL,NULL,NULL}};
  oyjlOptionChoice_s spe_form[]={ {"png",_("PNG"),"",_("PNG Raster")},
                                  {"svg",_("SVG"),"",_("SVG Vector")},
                                  {"csv",_("CSV"),"",_("CSV Values")},
                                  {"ncc",_("NCC"),"",_("Named Color Collection")},
                                  {"cgats",_("CGATS"),"",_("CGATS Values")},
                                  {"icc-xml",_("Icc XML"),"",_("ICC Named Color Values")},
                                  {"ppm",_("PPM"),"",_("Spectral PAM Image")},
                                  {NULL,NULL,NULL,NULL}};
  oyjlOptionChoice_s p_format[]={ {"png",_("PNG"),"",_("PNG Raster")},
                                  {"svg",_("SVG"),"",_("SVG Vector")},
                                  {"ncc",_("NCC"),"",_("Named Color Collection")},
                                  {NULL,NULL,NULL,NULL}};
  oyjlOptionChoice_s see_as_well[]={{"oyranos-profile(1) oyranos-config(1) oyranos-policy(1) oyranos(3)", "", "", ""},
                                    {"http://www.oyranos.org","","",""},
                                    {NULL,NULL,NULL,NULL}};
  oyjlOption_s oarray[] = {
  /* type,   flags, o, option, key, name, description, help, value_name, value_type, values, var_type, variable */
    {"oiwi", 0,                         "2", "icc-version-2", NULL, _("ICC Version 2"), _("Select ICC v2 Profiles"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&v2},NULL},
    {"oiwi", 0,                         "4", "icc-version-4", NULL, _("ICC Version 4"), _("Select ICC v4 Profiles"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&v4},NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE|OYJL_OPTION_FLAG_REPETITION, "@", NULL,            NULL, _("Input"),         _("ICC Profile"),            NULL, "l|rgb|cmyk|gray|lab|xyz|web|effect|proof|FILE", oyjlOPTIONTYPE_FUNCTION, {.getChoices = listProfiles}, oyjlINT, {.i=&profile_count},NULL},
    {"oiwi", 0, "b", "no-border",     NULL, _("Omit border"),   _("Omit border in graph"),   NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&no_border},NULL},
    {"oiwi", 0, "c", "no-blackbody",  NULL, _("No black body"), _("Omit white line of lambert light emitters"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&no_blackbody},NULL},
    {"oiwi", 0, "d", "change-thickness",NULL,_("Thickness increase"),_("Specify increase of the thickness of the graph lines"), NULL, _("NUMBER"), oyjlOPTIONTYPE_DOUBLE,
      {.dbl.start = 0.0, .dbl.end = 2.0, .dbl.tick = 0.05, .dbl.d = 0.7}, oyjlDOUBLE, {.d=&change_thickness},NULL},
    {"oiwi", 0, "f", "format",        NULL, _("Format"),        _("Specify output file format png or svg, default is png"), NULL, _("FORMAT"), oyjlOPTIONTYPE_CHOICE,
      {.choices.list = (oyjlOptionChoice_s*)oyjlStringAppendN( NULL, (const char*)out_form, sizeof(out_form), 0 )}, oyjlSTRING, {.s=&format},NULL},
    {"oiwi", 0, "g", "no-color",      NULL, _("Gray"),          _("Draw Gray"),              NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&no_color},NULL},
    {"oiwi", 0, "H", "hue",           NULL, _("Hue"),           _("HLC Color Atlas"),   _("Select a page by hue color angle in the HLC Color Atlas. -H=365 will output all hues."), _("NUMBER"), oyjlOPTIONTYPE_DOUBLE,
      {.dbl.start = 0.0, .dbl.end = 365.0, .dbl.tick = 5, .dbl.d = 0.0}, oyjlDOUBLE, {.d=&hue},NULL},
    {"oiwi", 0, "L", "lightness",     NULL, _("Lightness"),     _("HLC Color Atlas"),   _("Select a page by lightness in the HLC Color Atlas."), _("NUMBER"), oyjlOPTIONTYPE_DOUBLE,
      {.dbl.start = -5.0, .dbl.end = 100.0, .dbl.tick = 5, .dbl.d = -5.0}, oyjlDOUBLE, {.d=&lightness},NULL},
    {"oiwi", 0, "C", "chroma",        NULL, _("Chroma"),        _("HLC Color Atlas"),   _("Select a page by chroma (saturation) in the HLC Color Atlas."), _("NUMBER"), oyjlOPTIONTYPE_DOUBLE,
      {.dbl.start = -5.0, .dbl.end = 130.0, .dbl.tick = 5, .dbl.d = -5.0}, oyjlDOUBLE, {.d=&chroma},NULL},
    {"oiwi", 0, "l", "background-lightness",     NULL, _("Background"),     _("Background Lightness"),   NULL, _("NUMBER"), oyjlOPTIONTYPE_DOUBLE,
      {.dbl.start = -1.0, .dbl.end = 100.0, .dbl.tick = 1.0, .dbl.d = -1.0}, oyjlDOUBLE, {.d=&background_lightness},NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE, "i", "import",         NULL, _("Input"),        _("Color Page Input"),       _("Supported is a color page in NCC format, which contains pages layout with referenced rgb values. Those are placed on a sheed. Such pages are created by e.g. oyranos-profile-graph --hlc=NUMBER -f ncc"), _("FILE"), oyjlOPTIONTYPE_FUNCTION, {.getChoices = listInput}, oyjlSTRING, {.s=&input},NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE, "I", "index",          NULL, _("Index"),        _("Page Selection"),         _("Specify a page name as string or page index as number. -1 will list all page names of the imported file."), _("PAGE"), oyjlOPTIONTYPE_FUNCTION, {.getChoices = listPages}, oyjlSTRING, {.s=&page},NULL},
    {"oiwi", 0, "u", "illuminant",    NULL, _("Illuminant"),    _("Illuminant Spectrum"),    NULL, _("STRING"), oyjlOPTIONTYPE_CHOICE,
      {.choices.list = (oyjlOptionChoice_s*)oyjlStringAppendN( NULL, (const char*)illu_dxx, sizeof(illu_dxx), 0 )}, oyjlSTRING, {.s=&illuminant},NULL},
    {"oiwi", 0, "k", "kelvin",        NULL, _("Kelvin"),        _("Blackbody Radiator"),     NULL, _("NUMBER"), oyjlOPTIONTYPE_DOUBLE,
      {.dbl.start = 0.0, .dbl.end = 25000.0, .dbl.tick = 100, .dbl.d = 0.0}, oyjlDOUBLE, {.d=&kelvin},NULL},
    {"oiwi", 0, "n", "no-spectral-line",NULL,_("No spectral"),  _("Omit the spectral line"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&no_spectral},NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE, "o", "output",        NULL, _("Output"),        _("Specify output file name, default is stdout"), NULL, _("-|FILE"), oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s=&output},NULL},
    {"oiwi", 0, "p", "spectral-format",NULL,_("Spectral Output"),_("Specify spectral output file format"), NULL, _("FORMAT"), oyjlOPTIONTYPE_CHOICE,
      {.choices.list = (oyjlOptionChoice_s*)oyjlStringAppendN( NULL, (const char*)spe_form, sizeof(spe_form), 0 )}, oyjlSTRING, {.s=&format},NULL},
    {"oiwi", 0, "m", "swatch-format", NULL, _("Format"),      _("Specify output file format"), NULL, _("FORMAT"), oyjlOPTIONTYPE_CHOICE,
      {.choices.list = (oyjlOptionChoice_s*)oyjlStringAppendN( NULL, (const char*)p_format, sizeof(p_format), 0 )}, oyjlSTRING, {.s=&format},NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE, "P", "pattern",       NULL, _("Pattern"),       _("Filter of Color Names"),  NULL, _("STRING"), oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s=&pattern},NULL},
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE, "s", "spectral",      NULL, _("Spectral"),      _("Spectral Input"),         NULL, _("FILE"), oyjlOPTIONTYPE_FUNCTION, {.getChoices = listSpectral}, oyjlSTRING, {.s=&input},NULL},
    {"oiwi", 0, "S", "standard-observer",NULL,_("Standard Observer"),_("CIE Standard Observer 1931 2°"), NULL,NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&standardobs},NULL},
    {"oiwi", 0, "O", "observer-64",   NULL, _("10° Observer"),  _("CIE Observer 1964 10°"),  NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&observer64},NULL},

    {"oiwi", 0, "r", "no-repair",     NULL, _("No repair"),     _("No Profile repair of ICC profile ID"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&no_repair},NULL},
    {"oiwi", 0, "T", "raster",        NULL, _("Raster"),        _("Draw Raster"),            NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&raster},NULL},
    {"oiwi", 0, "t", "thickness",     NULL, _("Thickness"),     _("Specify the thickness of the graph lines"), NULL, _("NUMBER"), oyjlOPTIONTYPE_DOUBLE,
      {.dbl.start = 0.0, .dbl.end = 10.0, .dbl.tick = 0.05, .dbl.d = 1.0}, oyjlDOUBLE, {.d=&thickness},NULL},
    {"oiwi", 0, "w", "width",         NULL, _("Width"),         _("Specify output image width in pixel"), NULL, _("NUMBER"), oyjlOPTIONTYPE_DOUBLE,
      {.dbl.start = 64.0, .dbl.end = 4096.0, .dbl.tick = 1, .dbl.d = pixel_width}, oyjlDOUBLE, {.d=&pixel_width},NULL},
    {"oiwi", 0, "x", "xyy",           NULL, _("xyY"),           _("Use CIE*xyY *x*y plane for saturation line projection"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&xyy_plane},NULL},
    {"oiwi", 0, "z", "scale",         NULL, _("Scale"),         _("Scale the height of the spectrum graph"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&scale_spectrum},NULL},

    /* default options -h, -v and -V */
    {"oiwi", OYJL_OPTION_FLAG_ACCEPT_NO_ARG, "h", "help",NULL,NULL,NULL,NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&help},NULL},
    {"oiwi", 0, NULL,"synopsis",NULL, NULL,         NULL,         NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlNONE, {0},NULL},
    {"oiwi", 0, "v", "verbose", NULL, _("verbose"), _("verbose"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&verbose},NULL},
    {"oiwi", 0, "V", "version", NULL, _("version"), _("Version"), NULL, NULL, oyjlOPTIONTYPE_NONE, {0}, oyjlINT, {.i=&version},NULL},
    /* default option template -X|--export */
    {"oiwi", 0, "X", "export", NULL, NULL, NULL, NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices.list = NULL}, oyjlSTRING, {.s=&export},NULL},
    /* The --render option can be hidden and used only internally. */
    {"oiwi", OYJL_OPTION_FLAG_EDITABLE, "R", "render", NULL, NULL,  NULL,  NULL, NULL, oyjlOPTIONTYPE_CHOICE, {0}, oyjlSTRING, {.s=&render},NULL},
    /* blind options, useful only for man page generation */
    {"oiwi", 0, "E", "man-environment_variables", NULL, "", "", NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*)oyjlStringAppendN( NULL, (const char*)env_vars, sizeof(env_vars), 0 )}, oyjlNONE, {.i=0},NULL},
    {"oiwi", 0, "A", "man-examples", NULL, "", "", NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*)oyjlStringAppendN( NULL, (const char*)examples, sizeof(examples), 0 )}, oyjlNONE, {.i=0},NULL},
    {"oiwi", 0, "W", "man-see_as_well", NULL, "", "", NULL, NULL, oyjlOPTIONTYPE_CHOICE, {.choices.list = (oyjlOptionChoice_s*)oyjlStringAppendN( NULL, (const char*)see_as_well, sizeof(see_as_well), 0 )}, oyjlNONE, {.i=NULL},NULL},
    {"",0,0,0,0,0,0,0, NULL, oyjlOPTIONTYPE_END, {0},0,{0},NULL}
  };
  opts->array = (oyjlOption_s*)oyjlStringAppendN( NULL, (const char*)oarray, sizeof(oarray), 0 );

  oyjlOptionGroup_s groups[] = {
  /* type,   flags, name, description, help, mandatory, optional, detail */
    {"oiwg", 0, _("Saturation"), _("2D Graph from profiles"), _("Create a 2D Graph containing the saturation line from a ICC Profile."), "@", "t,b,l,g,w,o,f,c,x,d,n,2,4,r,v", "@,d,x,c,n,2,4,r",NULL},
    {"oiwg", 0, _("HLC"), _("HLC Color Atlas Pages"), _("Create a 2D Graph containing the possible color patches inside the ICC Profile gamut. More information about HLC Color Atlas can be found on www.freiefarbe.de"), "H", "L,C,@,t,b,l,g,w,m,o,p,2,4,r,v", "H,L,C,@,m",NULL},
    {"oiwg", 0, _("StdObs2°"), _("Standard Observer 1931 2° Graph"), NULL, "S", "t,b,l,g,w,T,o,f,v", "S",NULL},
    {"oiwg", 0, _("Obs10°"), _("1964 10° Observer Graph"), NULL, "O", "t,b,l,g,w,T,o,f,v", "O",NULL},
    {"oiwg", 0, _("Blackbody Radiator"), _("Blackbody Radiator Spectrum Graph"), NULL, "k", "t,b,l,g,w,T,o,f,v", "k",NULL},
    {"oiwg", 0, _("Illuminant Spectrum"), _("Illuminant Spectrum Graph"), NULL, "u", "t,b,l,g,w,T,o,f,v", "u",NULL},
    {"oiwg", 0, _("Spectral Input"), _("Spectral Input Graph"), NULL, "s,p", "t,b,l,g,w,T,P,o,v,z", "s,p,P,z",NULL},
    {"oiwg", 0, _("Color Page"), _("Render Color Page"), NULL, "i", "I,t,b,l,g,w,T,f,o,v", "i,I",NULL},
    {"oiwg", OYJL_GROUP_FLAG_GENERAL_OPTS, _("Misc"), _("General options"), NULL, "X|h|V|R", "v", "t,b,l,g,w,T,o,f,h,X,R,V,v",NULL},
    {"",0,0,0,0,0,0,0,0}
  };
  opts->groups = (oyjlOptionGroup_s*)oyjlStringAppendN( NULL, (const char*)groups, sizeof(groups), 0);

  info_tmp = oyUiInfo(_("The  oyranos-profile-graph programm converts ICC profiles or embedded ICC profiles from images to a graph image. By default the program shows the saturation line of the specified profiles and writes to stdout."),
                  "2020-06-02T12:00:00", "June 2, 2020");
  /* use newline separated permissions in name + write newline separated list in description; both lists need to match in count */
  info = oyjlUiHeaderSection_Append( info_tmp, "permissions", NULL,
                                     "android.permission.READ_EXTERNAL_STORAGE\nandroid.permission.WRITE_EXTERNAL_STORAGE",
                                     _("Read external storage for global data access, like downloads, music ...\nWrite external storage to create and modify global data.") );
  free(info_tmp); info_tmp = NULL;
  const char * attr = "{\n\
  \"org\": {\n\
    \"freedesktop\": {\n\
      \"oyjl\": {\n\
        \"ui\": {\n\
          \"options\": {\n\
            \"type\": \"oiws\",\n\
            \"array\": [{},{},{\n\
                \"type\": \"oiwi\",\n\
                \"o\": \"@\",\n\
                \"values\": {\n\
                  \"getChoicesCompletionBash\": \"oyranos-profiles -l 2>/dev/null\"\n\
                }\n\
              }]\n\
            }\n\
          }\n\
        }\n\
      }\n\
    }\n\
  }";
  oyjl_val root;
  root = oyJsonParse( attr, NULL );
  oyjlOptions_SetAttributes( opts, &root );
  ui = oyjlUi_FromOptions( "oyranos-profile-graph", _("Oyranos Profile Graph"), _("The tool is a ICC color profile grapher."),
#ifdef __ANDROID__
                                       ":/images/oyPG-logo.svg", // use qrc
#else
                                       "oyPG-logo",
#endif
      info, opts, &state );
  if( state & oyjlUI_STATE_EXPORT &&
      export &&
      strcmp(export,"json+command") != 0)
    return 0;

  if(ui && (!export && !input && !profile_count && !standardobs && !observer64 && kelvin == 0.0 && !illuminant && !render && hue == -1.0))
  {
    oyjlUiHeaderSection_s * version = oyjlUi_GetHeaderSection( ui,
                                                               "version" );
    FILE * f = NULL;
    char * t = oyjlOptions_PrintHelp( ui->opts, ui, verbose, &f, "%s v%s - %s", argv[0],
                              version && version->name ? version->name : "",
                              ui->description ? ui->description : "" );
    fputs( t, f ); free(t);
    state = oyjlUI_STATE_HELP;
  }

  if(state & oyjlUI_STATE_HELP)
  {
    fprintf( stderr, "%s\n\tman oyranos-profile-graph\n\n", _("For more information read the man page:"));
    return 0;
  }
  if(!ui) return 1;

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

  if(export && strcmp(export,"json+command") == 0)
  {
    char * json = NULL,
         * json_commands = strdup(jcommands);
    json = oyjlUi_ToJson( ui, 0 );
    json_commands[strlen(json_commands)-2] = ',';
    json_commands[strlen(json_commands)-1] = '\000';
    oyjlStringAdd( &json_commands, malloc, free, "%s", &json[1] );
    free(json); json = NULL;
    puts( json_commands );
    free(json_commands); json_commands = NULL;
    return 0;
  }

#if !defined(NO_OYJL_ARGS_RENDER)
  /* Render boilerplate */
  if(render)
  { 
    int debug = verbose;
# define RENDER_I18N NULL
    oyjlTermColorInit( OYJL_RESET_COLORTERM | OYJL_FORCE_COLORTERM );
    oyjlArgsRender( argc, argv, RENDER_I18N, jcommands,NULL, debug, ui, myMain );
#ifndef __ANDROID__
    oyjlUi_Release( &ui);
#endif
    return 0;
  }
#endif

  if(verbose > 1)
    oy_debug = verbose - 1;

  {
    double Lab[3] = { background_lightness == -1.0 ? 70 : background_lightness, 0.0, 0.0 }, rgb[3];
    oyLab2XYZ( Lab, rgb );
    if(verbose) fprintf(stderr, "Background Lab: %.2f %.2f %.2f XYZ: %.2f %.2f %.2f ", Lab[0], Lab[1], Lab[2], rgb[0], rgb[1], rgb[2] );
    oyXYZ2sRGB( rgb );
    if(verbose) fprintf(stderr, "RGB: %.2f %.2f %.2f\n", rgb[0], rgb[1], rgb[2] );
    bg_rgba[0] = rgb[0]; bg_rgba[1] = rgb[1]; bg_rgba[2] = rgb[2];
    if(background_lightness >= 0.0)
      bg_rgba[3] = 1.0;
  }

  /* detect all anonymous arguments for saturation */
  profile_names = oyjlOptions_ResultsToList( ui->opts, "@", &profile_count );
  pixel_w = (int)(pixel_width + 0.5);
  if(xyy_plane) proj = p_xyz;

  if(profile_count && profile_names && profile_names[0][0] == 'l' && profile_names[0][1] == '\000')
  {
    system("oyranos-profiles -le");
    return 0;
  }


  if(oy_debug)
    fprintf( stderr, "  Oyranos v%s\n",
                  oyNoEmptyName_m_(oyVersionString(1)));



#define pflags (v2?OY_ICC_VERSION_2:0 | v4?OY_ICC_VERSION_4:0 | no_repair?OY_NO_REPAIR:0)

  if(input)
  {
    char * fn = oyMakeFullFileDirName_( input );
    size_t size = 0;
    char * text = oyReadFileToMem_( fn, &size, NULL );
    double scale = 1.0;
    int data_format = oyjlDataFormat( text );

    if(strcmp(input, "HLC_EPV_M0_V2_3") == 0)
    {
      text = HLC_EPV_M0_V2_3_json;
      data_format = oyNAME_JSON;
    }

    if(data_format == oyNAME_JSON)
    {
      specT = oyJsonParse( text, NULL );
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
    if(!specT && text)
    {
      specT = oyTreeFromCsv( text );
      oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, strchr(input,'/')?strrchr(input,'/')+1:input, "description" );
      oyTreeFilterColors( specT, pattern );
      spectra = oySpectrumFromTree( specT );
      if(verbose && spectra)
      if(verbose && spectra) fprintf( stderr, "CSV parsed\n" );
      if(oy_debug) fprintf( stderr, "%s", oyStruct_GetText((oyStruct_s*)spectra, oyNAME_NAME, 0));
    }
    if(!specT && text && data_format <= 0)
    {
      specT = oyTreeFromCgats( text );
      oyTreeFilterColors( specT, pattern );
      spectra = oySpectrumFromTree( specT );
      if(verbose && spectra) fprintf( stderr, "CGATS parsed\n" );
      if(oy_debug) fprintf( stderr, "%s", oyStruct_GetText((oyStruct_s*)spectra, oyNAME_NAME, 0));
    }
    if(!spectra && text && data_format != oyNAME_JSON)
      oyImage_FromFile( fn, pflags, &image, NULL );

    if(spectra)
    {
      int luminance_spec = 0;
      char * device_class = oyjlValueText( oyjlTreeGetValueF( specT, 0, "collection/[0]/spectral/[0]/measurement/device/class" ), 0 );
      if( (device_class && oyStringCaseCmp_(device_class, "display") == 0) ||
          oyjlValueText( oyjlTreeGetValueF( specT, 0, "luminance" ), 0 ) )
        luminance_spec = 1;

      char * D = oyjlValueText( oyjlTreeGetValueF( specT, 0, "collection/[0]/spectral/[0]/tristimulus/illuminant" ), 0 );
      float * illu_300_830_5nm = NULL; /* 300-830@5nm */
      double kelvin = 5000.0;
      if(D && oyStringCaseCmp_(D,"d50") == 0)
        kelvin = 5000.0;
      else if(D && oyStringCaseCmp_(D,"d65") == 0)
        kelvin = 6500.0;
      //kelvin = 1.0; // ignore illuminant
      if(luminance_spec == 0)
      {
        int error = oyDXXCIEfromTemperature( kelvin, &illu_300_830_5nm );
        if(error)
          oyMessageFunc_p(oyMSG_ERROR,0,"not CIE illuminant for %g", kelvin);
      }

      if(D && verbose)
        fprintf( stderr, "custom illuminant %s for %g Kelvin\n", D, kelvin);

      int stdobs = 2; /* convert spectral data always to 2°CMF as used by ICC */
#if 0 // ignore custom CMF in XYZ/Lab values
      char * so = oyjlValueText( oyjlTreeGetValueF( specT, 0, "collection/[0]/spectral/[0]/tristimulus/observer" ), 0 );
      if(so && ((oyStringCaseCmp_(so,"10_Degree") == 0) ||
                (oyStringCaseCmp_(so,"10 Degree") == 0) ||
                (oyStringCaseCmp_(so,"10°") == 0)) )
        stdobs = 10;
#endif
      spectral_channels = (int)oySpectrumGetParam( spectra, oySPECTRUM_CHANNELS );
      spectral_count = (int)oySpectrumGetParam( spectra, oySPECTRUM_COLUMNS );
      spectra_XYZ = (oyF3*) calloc( (unsigned)spectral_count, sizeof(oyF3) );

      oySpectrumComputeWhite( spectra, stdobs, illu_300_830_5nm, &white, spectra_rect );
      for(j = 0; j < spectral_count; ++j)
      {
        oySpectrumComputeXYZ ( spectra, j, illu_300_830_5nm, stdobs, &spectra_XYZ[j] );
        double computedXYZ_Y = spectra_XYZ[j].f[1]/white.f[0];
        if(luminance_spec && CIE_Y_max < computedXYZ_Y)
          CIE_Y_max = computedXYZ_Y;
      }

      if(fabs(spectra_rect[3]) > fabs(spectra_rect[2]))
        scale = spectra_rect[3];
      else if(spectra_rect[2])
        scale = - spectra_rect[2];

      if(scale_spectrum || luminance_spec)
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
        fprintf( stderr, OY_DBG_FORMAT_ "%s\n", OY_DBG_ARGS_, oySpectrumGetString( spectra, oySPECTRUM_CHANNELS ) );
        fprintf( stderr, "white:\t%f %f %f\n", white.f[0], white.f[1], white.f[2] );
        for(j = 0; j < spectral_count; ++j)
        {
          double computedXYZ[3] = { spectra_XYZ[j].f[0]/white.f[0], spectra_XYZ[j].f[1]/white.f[0], spectra_XYZ[j].f[2]/white.f[0] };
          /* scale luminance white point to match Y = 1.0 */
          if(luminance_spec) for(i = 0; i < 3; ++i) computedXYZ[i] /= CIE_Y_max;

          double rgb[4] = { computedXYZ[0], computedXYZ[1], computedXYZ[2], 1.0 },
                 lab[3], XYZ[3] = {-1,-1,-1}, Lab[3], srgb[3] = {-1,-1,-1};
          char * L = oyjlValueText( oyjlTreeGetValueF( specT, 0, "collection/[0]/colors/[%d]/lab/[0]/data/[%d]", j, 0 ), 0 ),
               * a = oyjlValueText( oyjlTreeGetValueF( specT, 0, "collection/[0]/colors/[%d]/lab/[0]/data/[%d]", j, 1 ), 0 ),
               * b = oyjlValueText( oyjlTreeGetValueF( specT, 0, "collection/[0]/colors/[%d]/lab/[0]/data/[%d]", j, 2 ), 0 ),
               * R = oyjlValueText( oyjlTreeGetValueF( specT, 0, "collection/[0]/colors/[%d]/rgb/[0]/data/[%d]", j, 0 ), 0 ),
               * G = oyjlValueText( oyjlTreeGetValueF( specT, 0, "collection/[0]/colors/[%d]/rgb/[0]/data/[%d]", j, 1 ), 0 ),
               * B = oyjlValueText( oyjlTreeGetValueF( specT, 0, "collection/[0]/colors/[%d]/rgb/[0]/data/[%d]", j, 2 ), 0 ),
               * X = oyjlValueText( oyjlTreeGetValueF( specT, 0, "collection/[0]/colors/[%d]/xyz/[0]/data/[%d]", j, 0 ), 0 ),
               * Y = oyjlValueText( oyjlTreeGetValueF( specT, 0, "collection/[0]/colors/[%d]/xyz/[0]/data/[%d]", j, 1 ), 0 ),
               * Z = oyjlValueText( oyjlTreeGetValueF( specT, 0, "collection/[0]/colors/[%d]/xyz/[0]/data/[%d]", j, 2 ), 0 );
          int r = -10;
          if(L) r = oyjlStringToDouble( L, &lab[0], 0,0 );
          if(a) r = oyjlStringToDouble( a, &lab[1], 0,0 );
          if(b) r = oyjlStringToDouble( b, &lab[2], 0,0 );
          Lab[0] = lab[0]*100.0;
          Lab[1] = lab[1]*256.0-128.0;
          Lab[2] = lab[2]*256.0-128.0;
          if(r <= 0 && L && a && b && !X)
            oyLab2XYZ( Lab, XYZ );

          if(X) r = oyjlStringToDouble( X, &XYZ[0], 0,0 );
          if(Y) r = oyjlStringToDouble( Y, &XYZ[1], 0,0 );
          if(Z) r = oyjlStringToDouble( Z, &XYZ[2], 0,0 );
          if(r <= 0 && X && Y && Z && !L)
            oyXYZ2Lab( XYZ, Lab );

          if(R) r = oyjlStringToDouble( R, &srgb[0], 0,0 );
          if(G) r = oyjlStringToDouble( G, &srgb[1], 0,0 );
          if(B) r = oyjlStringToDouble( B, &srgb[2], 0,0 );

          oyjl_val v = oyjlTreeGetValueF(specT, 0, "collection/[0]/colors/[%d]/name", j);
          const char * name = OYJL_GET_STRING(v);
          if(!name) name = "";

          oyXYZ2sRGB( rgb );
          {
            double computedLab[3];
            oyXYZ2Lab( computedXYZ, computedLab ); /* no white point -> D50 conversion, as that is already our state */
            if(!L && !a && !b)
            {
              oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, computedLab[0]/100.0,     "collection/[0]/colors/[%d]/lab/[0]/data/[0]", j );
              oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, computedLab[1]/256.0+0.5, "collection/[0]/colors/[%d]/lab/[0]/data/[1]", j );
              oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, computedLab[2]/256.0+0.5, "collection/[0]/colors/[%d]/lab/[0]/data/[2]", j );
            }
            if(j < 10)
            {
              double DE = OY_HYP3(fabs(computedLab[0]-Lab[0]),fabs(computedLab[1]-Lab[1]),fabs(computedLab[2]-Lab[2]));
              fprintf( stderr, "%d \"%s\"%s  white/scale: %f computedScaledXYZ:\t%f %f %f ",
                       j, name, strlen(name) < 10 ? "\t": "", white.f[0], spectra_XYZ[j].f[0]/white.f[0]/CIE_Y_max, spectra_XYZ[j].f[1]/white.f[0]/CIE_Y_max, spectra_XYZ[j].f[2]/white.f[0]/CIE_Y_max );
              fprintf( stderr, "-> Lab:%d %d %d\t%d %d %d (rgb)\n",
                       (int)(computedLab[0]+.5), (int)(computedLab[1]+.5), (int)(computedLab[2]+.5), (int)(rgb[0]*255.), (int)(rgb[1]*255.), (int)(rgb[2]*255.) );
              if(strlen(name) > 8) fprintf( stderr, "\t" );
              fprintf( stderr, "\t\t\t\t\tfound XYZ:\t%f %f %f ",
                       XYZ[0], XYZ[1], XYZ[2] );
              fprintf( stderr, ":: Lab:%d %d %d\t%d %d %d (srgb) DE: %f\n",
                       (int)(Lab[0]+.5), (int)(Lab[1]+.5), (int)(Lab[2]+.5), (int)(srgb[0]*255.+.5), (int)(srgb[1]*255.+.5), (int)(srgb[2]*255.+.5), DE );
            }
            else if(j == 10)
              fprintf( stderr, "%d\t... more might follow\n", j );
          }
        }
      }
    }
  }

  pixel_h = pixel_w;

  thickness *= pixel_w/128.0;

#define HLC_PARAM_SETUP \
    int dist = 5, l,c, c_max = dist==10 ? 130 : 125; \
    int lcount = (100-2)/(double)dist + 2, \
        ccount = c_max/(double)dist + 1;
  if((int)(hue-.5) != -1)
  {
    HLC_PARAM_SETUP
    int h = 0;
    char * t = NULL;
    time_t cutime;         /* Time since epoch */
    struct tm * gmt;
    oyStructList_s * ccs = oyStructList_New(0); /* color conversions */
    oyProfiles_s * ps = oyProfiles_New(0);
    oyProfile_s * pLab = oyProfile_FromStd( oyASSUMED_LAB, 0, 0 );
    double color[16];
    int * outside = NULL, index = 0;
    double * lab = NULL;
    int count = lcount * ccount;
    oyOptions_s * module_options = NULL;

    oyOptions_SetFromString( &module_options, OY_DEFAULT_RENDERING_INTENT, "1", OY_CREATE_NEW );
    oyOptions_SetFromString( &module_options, OY_DEFAULT_RENDERING_BPC, "0", OY_CREATE_NEW );

    specT = oyjlTreeNew("");
    oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, "ncc1", "type" );
    oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, "Named Color Collection v1", "comment" );

    if(lightness >= 0.0 && chroma >= 0.0)
        count = 1;

    if((int)(hue+.5) == 365)
      oyjlStringAdd( &t, 0,0, "HLC ALL" );
    else
      oyjlStringAdd( &t, 0,0, "HLC H%03d", (int)hue );
    oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, t, "description" );
    free( t ); t = NULL;

    oyjlStringAdd( &t, 0,0, "Oyranos CMS" );
    oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, t, "creator" );
    free( t ); t = NULL;

    t = calloc(48,sizeof(char));
    cutime = time(NULL); /* time right NOW */
    gmt = localtime( &cutime );
    strftime( t, 48, "%FT%H:%M:%S%z", gmt );
    oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, t, "date" );
    free( t ); t = NULL;

    if(profile_count)
    {
      lab = calloc( count, sizeof(double) * 3 );
      outside = calloc( count, sizeof(int) );
      int error = !outside || !lab;
      if(error) return error;
      if(verbose)
        fprintf(stderr, "ccount: %d c_max: %d  lcount: %d\n", ccount, c_max, lcount );

      for(i = 0; i < profile_count; ++i)
      {
        const char * filename = profile_names[i];
        oyProfile_s * p = oyProfile_FromName( filename, pflags, NULL );
        oyConversion_s * cc;
        const char * desc = oyProfile_GetText( p, oyNAME_DESCRIPTION );
        const char * fn = oyProfile_GetFileName(p, -1);
        const char * hash = oyProfile_GetText( p, oyNAME_REGISTRATION );
        int error = 0;
        oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, desc, "collection/[0]/profiles/[%d]/id", i );
        oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, fn,   "collection/[0]/profiles/[%d]/filename", i );
        oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, hash, "collection/[0]/profiles/[%d]/hash", i );
        cc = oyConversion_CreateBasicPixelsFromBuffers (
                                       pLab, color, oyDataType_m(oyDOUBLE),
                                       p,    color, oyDataType_m(oyDOUBLE),
                                       module_options, 1 );
        error = !cc;
        if(error)
          fprintf( stderr, "No color conversion for profile: %s\n", filename );

        error = oyStructList_MoveIn( ccs, (oyStruct_s**)&cc, i, 0 );
        if(error)
          oyMessageFunc_p( oyMSG_ERROR, NULL, "in oyStructList_MoveIn( cc ): %s", oyProfile_GetFileName( p, -1 ) );
        oyProfiles_MoveIn( ps, &p, i );
      }
    }
    oyOptions_Release( &module_options );
    oyProfile_Release( &pLab );

    /* allocate color leave before pages */
    oyjlTreeGetValue( specT, OYJL_CREATE_NEW, "collection/[0]/colors" );
    for(h = 0; h < 360; h += dist)
    {
      char page_id[16];
      char id[24];
      char * color_desc = NULL;
      if((int)(hue+.5) != 365)
        h = hue;
      if(count == 1)
      {
        l = lightness/dist;
        c = chroma/dist;
        double LCh[3] = { l/(double)(lcount-1), c/(double)(ccount-1)*c_max/128.0, h/360.0 };
        sprintf( page_id, "H%03d_L%03d_C%03d", (int)(LCh[2]*360.0+0.5), (int)(LCh[0]*100.0+0.5), (int)(LCh[1]*128.0+0.5) );
      } else
        sprintf( page_id, "H%03d", h );
      oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, h/dist,  "collection/[0]/pages/%s/index", page_id );

      if(profile_count)
      {
        memset( outside, 0, sizeof(int) * count );
        if(count > 1)
        {
          for(c = 0; c < ccount; ++c)
          {
            for(l = 0; l < lcount; ++l)
            {
              double LCh[3] = { l/(double)lcount, c/(double)ccount*c_max/128.0, h/360.0 };
              oyLCh2Lab( LCh, &lab[ (c + l * ccount) * 3 ], NULL );
            }
          }
        } else
        {
          double LCh[3] = { lightness/100.0, chroma/128.0, h/360.0 };
          oyLCh2Lab( LCh, &lab[0], NULL );
        }
        for(i = 0; i < profile_count; ++i)
        {
          const char * filename = profile_names[i];
          oyProfile_s * p = oyProfile_FromName( filename, pflags, NULL );
          oyProfiles_s * proofing = oyProfiles_New(0);
          int error;
          if(verbose)
            fprintf(stderr, "proofing: %s\n", filename);
          oyProfiles_MoveIn( proofing, &p, -1 );
          error = oyLabGamutCheck( lab, count, proofing, outside, NULL );
          if(error)
          {
            oyMessageFunc_p( oyMSG_ERROR, NULL, "in oyLabGamutCheck( ): %s", oyProfile_GetFileName( p, -1 ) );
            return error;
          }
          oyProfiles_Release( &proofing );
        }
      }

      if(h == 0 || h == (int)(hue+.5))
        oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, "The page consists of a array of rows, each containing a array of columns. Each column references a color index number or null for no color at this position in the row.",  "collection/[0]/pages/%s/comment", page_id );
      /* provide axis texts */
      if(count > 1)
      {
        oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, "L",  "collection/[0]/pages/%s/rows_marker", page_id );
        for(l = 0; l < lcount; l++)
        {
          sprintf( id, "%03d", (int)(l/(double)(lcount-1)*100) );
          oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, id,  "collection/[0]/pages/%s/rows_markers/[%d]", page_id, l );
        }
        oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, "C",  "collection/[0]/pages/%s/columns_marker", page_id );
        for(c = 0; c < ccount; c++)
        {
          sprintf( id, "%03d", (int)(c/(double)(ccount-1)*c_max) );
          oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, id,  "collection/[0]/pages/%s/columns_markers/[%d]", page_id, c );
        }
        oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, "a*",  "collection/[0]/pages/%s/columns_marker2", page_id );
        oyjlTreeGetValueF( specT, OYJL_CREATE_NEW,         "collection/[0]/pages/%s/columns_markers2/[%d]", page_id, 0 );
        oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, "b*",  "collection/[0]/pages/%s/columns_marker3", page_id );
        oyjlTreeGetValueF( specT, OYJL_CREATE_NEW,         "collection/[0]/pages/%s/columns_markers3/[%d]", page_id, 0 );
      }
      if(count == 1)
      {
        oyjlStringAdd( &color_desc, 0,0, "HLC: %.1f %.1f %.1f", hue, lightness, chroma );
        oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, color_desc,  "collection/[0]/pages/%s/columns_marker", page_id );
      }
      for(l = lcount - 1; l >= 0; l--)
      {
        char row_id[8];
        c = 0;
        if(count == 1)
        {
          l = lightness/dist;
          c = chroma/dist;
        }
        sprintf( row_id, "L%03d", (int)(l/(double)(lcount-1)*100) );
        for(; c < ccount; c++)
        {
          double LCh[3] = { l/(double)(lcount-1), c/(double)(ccount-1)*c_max/128.0, h/360.0 };
          double Lab[3], XYZ[3];
          int error;

          oyLCh2Lab(LCh, Lab, NULL);

          if(l == 0)
          {
            sprintf( id, "%.1f", Lab[1]*256.0-128.0 );
            oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, id,  "collection/[0]/pages/%s/columns_markers2/[%d]", page_id, c );
            sprintf( id, "%.1f", Lab[2]*256.0-128.0 );
            oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, id,  "collection/[0]/pages/%s/columns_markers3/[%d]", page_id, c );
          }

          if(count == 1)
            i = 0;
          else
            i = l * ccount + c;
          if(outside && outside[i])
          {
            oyjlTreeGetValueF( specT, OYJL_CREATE_NEW, "collection/[0]/pages/%s/rows/%s/[%d]", page_id, row_id, count == 1 ? 0 : c );
            if(count == 1) break;
            continue;
          }

          sprintf( id, "H%03d_L%03d_C%03d", (int)(LCh[2]*360.0+0.5), (int)(LCh[0]*100.0+0.5), (int)(LCh[1]*128.0+0.5) );
          error = oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, id,     "collection/[0]/colors/[%d]/id", index );
          if(error) fprintf( stderr, "trouble with: HLC: %s\n", id );
          error = oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, index,     "collection/[0]/colors/[%d]/index", index );
          if(error) fprintf( stderr, "trouble with: HLC: %s\n", id );

          oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, "HLC",  "collection/[0]/colors/[%d]/lch/[0]/id", index );
          oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, LCh[0], "collection/[0]/colors/[%d]/lch/[0]/data/[0]", index );
          oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, LCh[1], "collection/[0]/colors/[%d]/lch/[0]/data/[1]", index );
          oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, LCh[2], "collection/[0]/colors/[%d]/lch/[0]/data/[2]", index );
          oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, Lab[0], "collection/[0]/colors/[%d]/lab/[0]/data/[0]", index );
          oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, Lab[1], "collection/[0]/colors/[%d]/lab/[0]/data/[1]", index );
          oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, Lab[2], "collection/[0]/colors/[%d]/lab/[0]/data/[2]", index );
          oyIcc2CIELab( Lab, Lab, NULL );
          oyLab2XYZ( Lab, XYZ );
          double rgb[4] = { XYZ[0], XYZ[1], XYZ[2], 1.0 };
          if(verbose) fprintf(stderr, "%d %s: HLC: %.2f %.2f %.2f Lab: %.2f %.2f %.2f XYZ: %.2f %.2f %.2f ", index, id, LCh[2], LCh[0], LCh[1], Lab[0], Lab[1], Lab[2], rgb[0], rgb[1], rgb[2] );
          oyXYZ2sRGB( rgb );
          if(verbose) fprintf(stderr, "RGB: %.5f %.5f %.5f\n", rgb[0], rgb[1], rgb[2] );
          if(count == 1)
          {
            free(color_desc); color_desc = NULL;
            oyjlStringAdd( &color_desc, 0,0, "Lab: %.1f %.1f %.1f  XYZ: %.3f %.3f %.3f  sRGB: %.3f %.3f %.3f", Lab[0], Lab[1], Lab[2], XYZ[0], XYZ[1], XYZ[2], rgb[0], rgb[1], rgb[2] );
            oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, color_desc,  "collection/[0]/pages/%s/columns_marker", page_id );
          }
          oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, XYZ[0], "collection/[0]/colors/[%d]/xyz/[0]/data/[0]", index );
          oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, XYZ[1], "collection/[0]/colors/[%d]/xyz/[0]/data/[1]", index );
          oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, XYZ[2], "collection/[0]/colors/[%d]/xyz/[0]/data/[2]", index );
          oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, "sRGB", "collection/[0]/colors/[%d]/rgb/[0]/id", index );
          oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, rgb[0], "collection/[0]/colors/[%d]/rgb/[0]/data/[0]", index );
          oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, rgb[1], "collection/[0]/colors/[%d]/rgb/[0]/data/[1]", index );
          oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, rgb[2], "collection/[0]/colors/[%d]/rgb/[0]/data/[2]", index );

          if(profile_count)
          {
            for(i = 0; i < profile_count; ++i)
            {
              oyProfile_s * p = oyProfiles_Get( ps, i );
              int channel_count = oyProfile_GetChannelsCount( p ), j, pos;
              icColorSpaceSignature csp = (icColorSpaceSignature) oyProfile_GetSignature( p, oySIGNATURE_COLOR_SPACE);
              const char * desc = oyProfile_GetText( p, oyNAME_DESCRIPTION );
              oyConversion_s * cc = (oyConversion_s*) oyStructList_GetRefType( ccs, i, oyOBJECT_CONVERSION_S );
              if(!p)
              {
                oyMessageFunc_p( oyMSG_ERROR, NULL, "no profile: %d", i );
                return 1;
              }
              if(!cc)
              {
                oyMessageFunc_p( oyMSG_ERROR, NULL, "no color conversion for profile: %s", oyProfile_GetFileName( p, -1 ) );
                return 1;
              }
              oyLCh2Lab( LCh, color, NULL );
              const char * json_cn = NULL;
              switch(csp)
              {
                case icSigLabData:    json_cn = "lab"; break;
                case icSigXYZData:    json_cn = "xyz"; break;
                case icSigRgbData:    json_cn = "rgb"; break;
                case icSigCmykData:   json_cn = "cmyk"; break;
                case icSigYCbCrData:  json_cn = "ycbcr"; break;
                default:              json_cn = "color"; break;
              }
              error = oyConversion_RunPixels( cc, 0 );
              if(error) fprintf( stderr, "error in oyConversion_RunPixels() HLC: %s\n", id );
              else if(verbose) fprintf( stderr, "fine oyConversion_RunPixels() HLC: %s\n", id );
              oyConversion_Release( &cc );
              oyProfile_Release( &p );

              {
                oyjl_val v = oyjlTreeGetValueF(specT, 0, "collection/[0]/colors/[%d]/%s", index, json_cn );
                pos = oyjlValueCount( v );
              }
              oyjlTreeSetStringF(   specT, OYJL_CREATE_NEW, desc,     "collection/[0]/colors/[%d]/%s/[%d]/id",        index, json_cn, pos );
              if(count == 1)
              {
                free(color_desc); color_desc = NULL;
                oyjlStringAdd( &color_desc, 0,0, "%s:", desc );
              }
              for(j = 0; j < channel_count; ++j)
              {
                oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, color[j], "collection/[0]/colors/[%d]/%s/[%d]/data/[%d]", index, json_cn, pos, j );
                if(count == 1)
                  oyjlStringAdd( &color_desc, 0,0, " %.3f", color[j] );
              }
              if(count == 1)
                oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, color_desc,  "collection/[0]/pages/%s/columns_marker2", page_id );
            }
          }

          /* position in colors array */
          oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, index, "collection/[0]/pages/%s/rows/%s/[%d]", page_id, row_id, count == 1 ? 0 : c );

          ++index;
          if(count == 1)
            break;
        }
        if(count == 1)
          break;
      }
      if((int)(hue+.5) != 365)
        break;
      if(color_desc) { free(color_desc); } color_desc = NULL;
    }

    oyStructList_Release( &ccs );
    oyProfiles_Release( &ps );
    if(lab) free(lab);
    if(outside) free(outside);
  }

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
        if(!string)
          oyMessageFunc_p(oyMSG_ERROR,NULL,"error in oyTreeToCsv()");
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
    else if(oyStringCaseCmp_(format, "ncc") == 0)
    {
      if(specT)
      {
        string = oyjlTreeToText( specT, OYJL_JSON | OYJL_NO_MARKUP );
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
    else if(oyStringCaseCmp_(format, "icc-xml") == 0)
    {
      if(specT)
      {
        int level = 0;
        oyTreeToIccXml( specT, &level, &string );
        if(!string)
          oyMessageFunc_p(oyMSG_ERROR,NULL,"error in oyTreeToIccXml()");
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
    else if(format)
      oyMessageFunc_p( oyMSG_ERROR, NULL, "export format not supported: %s", format );

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

  if(0.0 <= background_lightness && background_lightness < 5.0)
    cairo_set_source_rgba( cr, 1.0, 1.0, 1.0, 1.0 );
  else
    cairo_set_source_rgba( cr, .0, .0, .0, 1.0);
  /* draw spectral gamut line */
  cairo_set_line_width (cr, thickness);
  if(profile_count && no_spectral == 0 && (int)(hue-.5) == -1)
  {
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

  /* draw white point line */
  cairo_set_line_width (cr, 0.5*thickness);
  if(profile_count && no_blackbody == 0 && proj == p_xyz && (int)(hue-.5) == -1)
  {
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
  if(0.0 <= background_lightness && background_lightness < 50.0)
    cairo_set_source_rgba( cr, 1.0, 1.0, 1.0, 1.0 );
  else
    cairo_set_source_rgba( cr, .0, .0, .0, 1.0);
  cairo_set_line_width (cr, 0.7*thickness);
  if(!no_border)
  cairo_rectangle( cr, x - frame, y - frame,
                         w*scale + 2*frame, h*scale + 2*frame);
  cairo_stroke(cr);

  /* draw cross */
  if(0.0 <= background_lightness && background_lightness < 5.0)
    cairo_set_source_rgba( cr, 1.0, 1.0, 1.0, 1.0 );
  else
    cairo_set_source_rgba( cr, .0, .0, .0, 1.0);
  if(profile_count && no_border == 0 && proj == p_lab && (int)(hue-.5) == -1 )
  {
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

  /* draw gamut saturation */
  if(profile_count && (int)(hue-.5) == -1)
  {
    double t = thickness;

    cairo_set_line_width (cr, 3.*t);
    for ( j = profile_count - 1; j >= 0; --j )
    {
      const char * filename = profile_names[j];
      size_t size = 0;

      oyProfile_s * p = oyProfile_FromName( filename, pflags, NULL );
      double * saturation = NULL;

      if(!p)
      {
        oyImage_s * image = 0;
        oyImage_FromFile( filename, pflags, &image, NULL );
        p = oyImage_GetProfile( image );
      }

      if((int)(hue-.5) == -1)
        saturation = getSaturationLine_( p, 3, &size, p_lab );

      t = pow( change_thickness, j ) * thickness;
      cairo_set_line_width (cr, 3.*t);

      if(saturation && !no_color && j == 0)
      {
        /* background circles */
        for(i = 1; i<(int)size; ++i)
        {
          double XYZ[3];
          double Lab[3];
          double x0, y0;

          Lab[0] = saturation[(i-1)*3+0]*100.0;
          Lab[1] = saturation[(i-1)*3+1]*256.0-128.0;
          Lab[2] = saturation[(i-1)*3+2]*256.0-128.0;
          oyLab2XYZ( Lab, XYZ);
          if(proj == p_lab)
          {
            x0 = saturation[(i-1)*3+1]/1.0;
            y0 = saturation[(i-1)*3+2]/1.0;
          }
          else
          {
            x0 = XYZ[0]/(XYZ[0]+XYZ[1]+XYZ[2])*xs_xyz;
            y0 = XYZ[1]/(XYZ[0]+XYZ[1]+XYZ[2])*ys_xyz;
          }
          oyXYZ2sRGB( XYZ );

          /* end with a half circle to cover empty areas toward the following line segment */
          cairo_set_source_rgba( cr, XYZ[0],XYZ[1],XYZ[2], 1.0);
          cairo_arc( cr, xToImage(x0), yToImage(y0), t*3.0/2.0, 0, 2*M_PI);
          cairo_fill(cr);

        }

        /* colord saturation line */
        for(i = 1; i<(int)size; ++i)
        {
          double XYZ[3],XYZ1[3];
          double Lab[3];
          cairo_pattern_t * g;
          double x0, x1, y0, y1;

          Lab[0] = saturation[(i-1)*3+0]*100.0;
          Lab[1] = saturation[(i-1)*3+1]*256.0-128.0;
          Lab[2] = saturation[(i-1)*3+2]*256.0-128.0;
          oyLab2XYZ( Lab, XYZ);
          if(proj == p_lab)
          {
            x0 = saturation[(i-1)*3+1]/1.0;
            y0 = saturation[(i-1)*3+2]/1.0;
          }
          else
          {
            x0 = XYZ[0]/(XYZ[0]+XYZ[1]+XYZ[2])*xs_xyz;
            y0 = XYZ[1]/(XYZ[0]+XYZ[1]+XYZ[2])*ys_xyz;
          }
          oyXYZ2sRGB( XYZ );

          Lab[0] = saturation[i*3+0]*100.0;
          Lab[1] = saturation[i*3+1]*256.0-128.0;
          Lab[2] = saturation[i*3+2]*256.0-128.0;
          oyLab2XYZ( Lab, XYZ1);
          if(proj == p_lab)
          {
            x1 = saturation[i*3+1]/1.0;
            y1 = saturation[i*3+2]/1.0;
          }
          else
          {
            x1 = XYZ1[0]/(XYZ1[0]+XYZ1[1]+XYZ1[2])*xs_xyz;
            y1 = XYZ1[1]/(XYZ1[0]+XYZ1[1]+XYZ1[2])*ys_xyz;
          }

          cairo_new_path(cr);
          oyXYZ2sRGB( XYZ1 );
          g = cairo_pattern_create_linear( xToImage(x0),yToImage(y0),
                                           xToImage(x1),yToImage(y1));
          cairo_pattern_add_color_stop_rgba(g, 0, XYZ[0],XYZ[1],XYZ[2], 1.0);
          /* add different stop */
          cairo_pattern_add_color_stop_rgba(g, 1, XYZ1[0],XYZ1[1],XYZ1[2], 1.0);
          /* only one color pattern can be drawn at each cairo_stroke;
           * appears to be a cairo limitation */
          cairo_set_source(cr, g);
          cairo_move_to(cr, xToImage(x0), yToImage(y0));
          cairo_line_to(cr, xToImage(x1), yToImage(y1));
          cairo_stroke(cr);
          cairo_pattern_destroy(g);

        }
      } else
      if(saturation && (no_color || j > 0))
      {
        cairo_new_path(cr);
        i = 0;
        if(proj == p_lab)
          cairo_move_to(cr, xToImage(saturation[i*3+1]/1.0),
                            yToImage(saturation[i*3+2]/1.0));
        else
        {
          double XYZ[3];
          double Lab[3];
          Lab[0] = saturation[i*3+0]*100.0;
          Lab[1] = saturation[i*3+1]*256.0-128.0;
          Lab[2] = saturation[i*3+2]*256.0-128.0;
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
            Lab[1] = saturation[i*3+1]*256.0-128.0;
            Lab[2] = saturation[i*3+2]*256.0-128.0;
            oyLab2XYZ( Lab, XYZ);
            cairo_line_to(cr, xToImage(XYZ[0]/(XYZ[0]+XYZ[1]+XYZ[2])*xs_xyz),
                              yToImage(XYZ[1]/(XYZ[0]+XYZ[1]+XYZ[2])*ys_xyz));
          }
        }
        cairo_close_path(cr);
        if(-1.0 <= background_lightness && background_lightness < 50.0)
          cairo_set_source_rgba( cr, 1.0, 1.0, 1.0, 1.0 );
        else
          cairo_set_source_rgba( cr, .0, .0, .0, 1.0);
        cairo_stroke(cr);
      }

      oyProfile_Release( &p );
    }
  } else
  /* expect a page to draw */
  if(oyjlTreeGetValue( specT, 0, "collection/[0]/pages/[0]/" ))
  {
    int pages = 0, rows = 0, columns = 0, x,y;
    oyjl_val v = oyjlTreeGetValue( specT, 0, "collection/[0]/pages/[0]/[0]" );
    char * page_id = NULL;
    char * pt = NULL;

    pages = oyjlValueCount( oyjlTreeGetValue( specT, 0, "collection/[0]/pages" ) );
    if(page)
    {
      long pos = 0;
      if(oyjlStringToLong( page, &pos, 0 ) <= 0 && pos == -1)
      {
        int i;
        char ** paths = oyjlTreeToPaths( oyjlTreeGetValue( specT, 0, "collection/[0]/pages" ), 1, NULL, OYJL_PATH, &pages );
        for(i = 0; i < pages; ++i)
          fprintf( stdout, "%s\n", paths[i] );
        return 0;
      }
      v = oyjlTreeGetValueF( specT, 0, "collection/[0]/pages/%s/[0]", page );
      if(v)
        page_id = oyjlStringCopy( page, 0 );
      if(!page_id && oyjlStringToLong( page, &pos, 0 ) <= 0)
      {
        v = oyjlTreeGetValueF( specT, 0, "collection/[0]/pages/[%ld]/[0]", pos );
        page_id = oyjlTreeGetPath( oyjlTreeGetValue( specT, 0, "collection/[0]/pages" ), v );
        if(page_id && strchr(page_id, '/'))
        {
          pt = strchr(page_id,'/');
          pt[0] = '\000';
          pt = NULL;
        }
      }
    }
    else
    {
      page_id = oyjlTreeGetPath( oyjlTreeGetValue( specT, 0, "collection/[0]/pages" ), v );
      if(page_id && strchr(page_id, '/'))
      {
        pt = strchr(page_id,'/');
        pt[0] = '\000';
        pt = NULL;
      }
    }

    v = oyjlTreeGetValueF( specT, 0, "collection/[0]/pages/%s/rows", page_id );
    rows = oyjlValueCount( v );
    v = oyjlTreeGetValueF( specT, 0, "collection/[0]/pages/%s/rows/", page_id );
    columns = oyjlValueCount( v );
    fprintf( stderr, "page_title: %s (%s) %d x %dx%d\n", page_id ? page_id : "----", pt?pt:"----", pages, rows, columns );

    double ratio = rows / (double)columns;
    double off = frame/20.0;
    cairo_set_line_width (cr, off * 2.0);
    char * utf8 = NULL;
    const char * t;
    if(0.0 <= background_lightness && background_lightness < 50.0)
      /* set font color */
      cairo_set_source_rgba( cr, 1.0, 1.0, 1.0, 1.0 );
    cairo_select_font_face(cr, "Sans",
                           CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size (cr, frame);
    x = 0; y = rows;
    if((t = OYJL_GET_STRING(oyjlTreeGetValueF( specT, 0, "collection/[0]/pages/%s/rows_marker", page_id ))) != NULL)
    {
      oyjlStringAdd( &utf8, 0,0, t );
      cairo_move_to (cr, xToImage(0) - frame * 0.8,
                         yToImage((double)(y  )/(double)rows * ratio + (1-ratio) / 2.0 + (ratio==1.0?0.0:0.01)) - off);
      cairo_show_text (cr, utf8);
      free(utf8); utf8 = NULL;
    }
    cairo_set_font_size (cr, frame/2.0);
    for(y = 0; y < rows; ++y)
      if((t = OYJL_GET_STRING(oyjlTreeGetValueF( specT, 0, "collection/[0]/pages/%s/rows_markers/[%d]", page_id, y ))) != NULL)
      {
        oyjlStringAdd( &utf8, 0,0, t );
        cairo_move_to (cr, xToImage(0) - frame * 0.8,
                           yToImage((double)(y  )/(double)rows * ratio + (1-ratio) / 2.0 + 0.01) - off);
        cairo_show_text (cr, utf8);
        free(utf8); utf8 = NULL;
      }

    x = columns - 1;
    cairo_set_font_size (cr, frame);
    oyjlStringAdd( &utf8, 0,0, "%s", page_id ? page_id : "----" );
    cairo_move_to (cr, xToImage(1.0) - frame * (strlen(utf8)>5 ? 8.0 : 2.5),
                       yToImage((double)(y  )/(double)rows * ratio + (1-ratio) / 2.0 + (ratio==1.0?0.0:0.01)) - off);
    cairo_show_text (cr, utf8);
    free(utf8); utf8 = NULL;

    x = y = 0;
    cairo_set_font_size (cr, frame);
    if((t = OYJL_GET_STRING(oyjlTreeGetValueF( specT, 0, "collection/[0]/pages/%s/columns_marker", page_id ))) != NULL)
    {
      oyjlStringAdd( &utf8, 0,0, t );
      cairo_move_to (cr, xToImage(0) - frame * 0.8,
                         yToImage((double)(y  )/(double)rows * ratio + (1-ratio) / 2.0 - (columns == 1?0.023:0.05)) - off);
      cairo_show_text (cr, utf8);
      free(utf8); utf8 = NULL;
    }

    if(columns > 1)
      cairo_set_font_size (cr, frame/2.0);
    if((t = OYJL_GET_STRING(oyjlTreeGetValueF( specT, 0, "collection/[0]/pages/%s/columns_marker2", page_id ))) != NULL)
    {
      oyjlStringAdd( &utf8, 0,0, t );
      cairo_move_to (cr, xToImage(0) - frame * 0.8,
                         yToImage((double)(y  )/(double)rows * ratio + (1-ratio) / 2.0 - (columns == 1?0.053:0.07)) - off);
      cairo_show_text (cr, utf8);
      free(utf8); utf8 = NULL;
    }

    if((t = OYJL_GET_STRING(oyjlTreeGetValueF( specT, 0, "collection/[0]/pages/%s/columns_marker3", page_id ))) != NULL)
    {
      oyjlStringAdd( &utf8, 0,0, t );
      cairo_move_to (cr, xToImage(0) - frame * 0.8, yToImage((double)(y  )/(double)rows * ratio + (1-ratio) / 2.0 - 0.09) - off);
      cairo_show_text (cr, utf8);
      free(utf8); utf8 = NULL;
    }

    for(x = 0; x < columns; ++x)
    {
      if((t = OYJL_GET_STRING(oyjlTreeGetValueF( specT, 0, "collection/[0]/pages/%s/columns_markers/[%d]", page_id, x ))) != NULL)
      {
        oyjlStringAdd( &utf8, 0,0, t );
        cairo_move_to (cr, xToImage((((double)x)+0.5)/(double)columns) - 0.6*frame, yToImage((double)(y  )/(double)rows * ratio + (1-ratio) / 2.0 - 0.05) - off);
        cairo_show_text (cr, utf8);
        free(utf8); utf8 = NULL;
      }
      if((t = OYJL_GET_STRING(oyjlTreeGetValueF( specT, 0, "collection/[0]/pages/%s/columns_markers2/[%d]", page_id, x ))) != NULL)
      {
        oyjlStringAdd( &utf8, 0,0, t );
        cairo_move_to (cr, xToImage((((double)x)+0.5)/(double)columns) - 0.6*frame, yToImage((double)(y  )/(double)rows * ratio + (1-ratio) / 2.0 - 0.07) - off);
        cairo_show_text (cr, utf8);
        free(utf8); utf8 = NULL;
      }
      if((t = OYJL_GET_STRING(oyjlTreeGetValueF( specT, 0, "collection/[0]/pages/%s/columns_markers3/[%d]", page_id, x ))) != NULL)
      {
        oyjlStringAdd( &utf8, 0,0, t );
        cairo_move_to (cr, xToImage((((double)x)+0.5)/(double)columns) - 0.6*frame, yToImage((double)(y  )/(double)rows * ratio + (1-ratio) / 2.0 - 0.09) - off);
        cairo_show_text (cr, utf8);
        free(utf8); utf8 = NULL;
      }
    }
    {
      char * utf8 = NULL;
      int j;
      for( j = 0; j < profile_count; ++j)
        oyjlStringAdd( &utf8, 0,0, " %s", profile_names[j] );
      drawTextTopLeft( cr, background_lightness, frame, xO, yO, width, height, min_x, max_x, min_y, max_y, 0,0, "%s %s %s", OYJL_E(input,""), OYJL_E(pattern,""), OYJL_E(utf8,""));
      if(utf8) { free(utf8); utf8 = NULL; }
    }

    for(x = 0; x < columns; ++x)
    {
      for(y = 0; y < rows; ++y)
      {
        int found = 0;
        double rgb[4] = { 1, 1, 1, 1.0 };
        double d;
        int index = 0, i;
        v = oyjlTreeGetValueF( specT, 0, "collection/[0]/pages/%s/rows/[%d]/[%d]", page_id, y, x );
        if(OYJL_IS_DOUBLE(v))
        {
          d = OYJL_GET_DOUBLE(v);
          index = (int)(d+0.5);
          for(i = 0; i < 3; ++i)
          {
            v = oyjlTreeGetValueF( specT, 0, "collection/[0]/colors/[%d]/rgb/[0]/data/[%d]", index, i );
            if(OYJL_IS_DOUBLE(v))
              rgb[i] = OYJL_GET_DOUBLE(v);
          }
          found = 1;
        }

        if(found)
        {
          cairo_new_path(cr);
          cairo_move_to(cr, xToImage((double)(x  )/(double)columns) + off, yToImage((double)(rows-1-y  )/(double)rows * ratio + (1-ratio) / 2.0) - off);
          cairo_line_to(cr, xToImage((double)(x+1)/(double)columns) - off, yToImage((double)(rows-1-y  )/(double)rows * ratio + (1-ratio) / 2.0) - off);
          cairo_line_to(cr, xToImage((double)(x+1)/(double)columns) - off, yToImage((double)(rows-1-y+1)/(double)rows * ratio + (1-ratio) / 2.0) + off);
          cairo_line_to(cr, xToImage((double)(x  )/(double)columns) + off, yToImage((double)(rows-1-y+1)/(double)rows * ratio + (1-ratio) / 2.0) + off);
          cairo_close_path(cr);
          cairo_set_source_rgba( cr, rgb[0], rgb[1], rgb[2], 1.0);
          cairo_fill(cr);
        } else
        /* draw cross for not found single patch */
        if(columns == 1 && rows == 1)
        {
          cairo_new_path(cr);
          cairo_set_line_width (cr, off * 10.0);
          cairo_move_to(cr, xToImage((double)(x  )/(double)columns) + off, yToImage((double)(rows-1-y  )/(double)rows * ratio + (1-ratio) / 2.0) - off);
          cairo_line_to(cr, xToImage((double)(x+1)/(double)columns) - off, yToImage((double)(rows-1-y+1)/(double)rows * ratio + (1-ratio) / 2.0) + off);
          cairo_stroke(cr);
          cairo_move_to(cr, xToImage((double)(x+1)/(double)columns) - off, yToImage((double)(rows-1-y  )/(double)rows * ratio + (1-ratio) / 2.0) - off);
          cairo_line_to(cr, xToImage((double)(x  )/(double)columns) + off, yToImage((double)(rows-1-y+1)/(double)rows * ratio + (1-ratio) / 2.0) + off);
          cairo_stroke(cr);
        }

        cairo_set_source_rgba( cr, 0, 0, 0, 1.0 );
      }
    }
    cairo_set_source_rgba( cr, 1, 1, 1, 1.0);
  }

  if(image) /* support first line RGB and draw each channel */
  {
    double t = thickness;
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
    fprintf(stderr, "byteps: %d\n", (int)byteps);
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

  if(standardobs)
    oyjlStringAdd( &top_left_text, 0,0, "%s", "StdObs 31" );
  else if(observer64)
    oyjlStringAdd( &top_left_text, 0,0, "%s", "StdObs 64" );
  if(kelvin > 0.0)
    oyjlStringAdd( &top_left_text, 0,0, "%d K", (int) kelvin );
  else if(illuminant != NULL)
    oyjlStringAdd( &top_left_text, 0,0, "%s", illuminant );
  if(spectra)
    oyjlStringAdd( &top_left_text, 0,0, "%s    %s", OYJL_E(input,""), OYJL_E(pattern,"") );

  if(raster &&
     ( standardobs || observer64 || illuminant || kelvin > 0.0 || spectra ) )
  {
    double text_width = 0;
    if(top_left_text)
      drawTextTopLeft( cr, background_lightness, frame, xO, yO, width, height, min_x, max_x, min_y, max_y, OY_NO_DRAW, &text_width, top_left_text );

    /* draw some coordinate system hints */
    cairo_set_line_width (cr, 0.35*thickness);
    cairo_set_source_rgba( cr, rgba[0], rgba[1], rgba[2], 1.0 );

    /* 25 nm */
    for(i = 300; i < max_x; i += 25)
    {
      double x = xToImage(i),
             w = xToImage(min_x) - frame * 0.8 + text_width;
      double y = max_y - 0.0025;
      if(i < min_x) continue;
      if(text_width > 0 && x <= w)
        y = max_y - 0.03;
      cairo_move_to(cr, xToImage(i), yToImage(min_y));
      cairo_line_to(cr, xToImage(i), yToImage(y));
    }
    cairo_stroke(cr);
    if(verbose)
      fprintf( stderr, "raster color: %f %f %f\n", rgba[0], rgba[1], rgba[2] );

    /* 100 nm */
    char utf8[32];
    cairo_select_font_face(cr, "Sans",
                           CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size (cr, frame);
    cairo_set_line_width (cr, 0.7*thickness);
    for(i = 300; i < max_x; i += 100)
    {
      double x = xToImage(i),
             w = xToImage(min_x) - frame * 0.8 + text_width;
      double y = max_y - 0.0025;
      if(i < min_x) continue;
      if(text_width > 0 && x <= w)
        y = max_y - 0.03;
      cairo_move_to(cr, xToImage(i), yToImage(min_y));
      cairo_line_to(cr, xToImage(i), yToImage(y));
      sprintf( utf8, "%d", i );
      cairo_move_to (cr, xToImage(i) - frame * 0.8,
                         yToImage(max_y + 0.0025));
      cairo_show_text (cr, utf8);
    }
    cairo_stroke(cr);

    if(min_y < 0.0)
    {
      /* zero intensity */
      cairo_move_to(cr, xToImage(min_x), yToImage(0.0));
      cairo_line_to(cr, xToImage(max_x), yToImage(0.0));
      cairo_stroke(cr);
      sprintf( utf8, "%d", 0 );
      cairo_move_to (cr, xToImage(min_x) - frame * 0.8,
                         yToImage(max_y + 0.0025));
      cairo_show_text (cr, utf8);
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
                      pflags, top_left_text, 0,0,0 ); \
    }
  cairo_set_line_width (cr, 3.*thickness);

  if(standardobs)
  {
    oyImage_s * a = oySpectrumCreateEmpty ( 360, 830, 1, 3 );
    float max = oySpectrumFillFromArrayF3 ( a, cieXYZ_31_2 );
    oySpectrumNormalise ( a, 1.0/max ); max = 1.0;
    /* draw spectral sensitivity curves from 1931 standard observer */
    drawSpectralCurve(cieXYZ_31_2, 0, 1.,.0,.0, 1.)
    drawSpectralCurve(cieXYZ_31_2, 1, .0,1.,.0,1.)
    drawSpectralCurve(cieXYZ_31_2, 2, .0,.0,1.,1.)
    cairo_stroke(cr);
    oyImage_Release( &a );
    drawTextTopLeft( cr, background_lightness, frame, xO, yO, width, height, min_x, max_x, min_y, max_y, 0,0, top_left_text );
  }
  if(observer64)
  {
    oyImage_s * a = oySpectrumCreateEmpty ( 360, 830, 1, 3 );
    float max = oySpectrumFillFromArrayF3 ( a, cieXYZ_64_10 );
    oySpectrumNormalise ( a, 1.0/max ); max = 1.0;
    /* draw spectral sensitivity curves from 1964 standard observer */
    drawSpectralCurve(cieXYZ_64_10, 0, 1.0, .0, .0, 1.)
    drawSpectralCurve(cieXYZ_64_10, 1, .0, 1.0, .0, 1.)
    drawSpectralCurve(cieXYZ_64_10, 2, .0, .0, 1.0, 1.)
    cairo_stroke(cr);
    oyImage_Release( &a );
    drawTextTopLeft( cr, background_lightness, frame, xO, yO, width, height, min_x, max_x, min_y, max_y, 0,0, top_left_text );
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
                      pflags, "kelvin", 0,0,0 );
    oyImage_Release( &a );

    drawTextTopLeft( cr, background_lightness, frame, xO, yO, width, height, min_x, max_x, min_y, max_y, 0,0, top_left_text );
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
                      pflags, "D" #DXX, 0,0,0 ); \
      oyImage_Release( &a ); \
    } else
  if(illuminant != 0)
  {
    drawTextTopLeft( cr, background_lightness, frame, xO, yO, width, height, min_x, max_x, min_y, max_y, 0,0, top_left_text );
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
                      pflags, "A", 0,0,0 );
      oyImage_Release( &a );
    } else
    if(oyStringCaseCmp_(illuminant,"D65T") == 0)
    {
      oyImage_s * a = oySpectrumCreateEmpty ( 300, 830, 5, 1 );
      float max = oySpectrumFillFromArrayF( a, spd_D65_5, 0 );
      if(verbose) fprintf( stderr, "max: %f\n", max );
      oySpectrumNormalise ( a, 1.0/max ); max = 1.0;
      drawIlluminant( cr,
                      a, 0,
                      xO, yO, width, height,
                      min_x, max_x,  min_y < 0 ? min_y : 0.0, max,
                      no_color ? COLOR_GRAY : COLOR_SPECTRAL, rgba,
                      pflags, "A", 0,0,0 );
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
                      pflags, "S1", OY_DRAW_ID,frame,thickness );
      drawIlluminant( cr, a, 1, xO, yO, width, height,
                      min_x, max_x,  min_y < 0 ? min_y : 0.0, max,
                      no_color ? COLOR_GRAY : COLOR_SPECTRAL, rgba,
                      pflags, "S2", OY_DRAW_ID,frame,thickness );
      drawIlluminant( cr, a, 2, xO, yO, width, height,
                      min_x, max_x,  min_y < 0 ? min_y : 0.0, max,
                      no_color ? COLOR_GRAY : COLOR_SPECTRAL, rgba,
                      pflags, "S3", OY_DRAW_ID,frame,thickness );
      oyImage_Release( &a );
    } else
    ILLUMINANT( 50 )
    ILLUMINANT( 55 )
    ILLUMINANT( 65 )
    ILLUMINANT( 75 )
    ILLUMINANT( 93 )
    /* else */ if(illuminant)
    {
      oyMessageFunc_p( oyMSG_ERROR, 0, "illuminant not supported: %s", oyjlTermColor( oyjlBOLD, illuminant ));
      return 0;
    }

    {
    long kelvin = 0, err;
    if((err = oyjlStringToLong(illuminant,&kelvin, 0)) <= 0)
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
                      pflags, "D50", 0,0,0 );
      oyImage_Release( &a );
      if(spd_5) free(spd_5);
    }
    }
  }
  if(spectra)
  {
    int j;
    for(j = 0; j < spectral_count; ++j)
    {
      double rgb[4] = { spectra_XYZ[j].f[0]/white.f[0]/CIE_Y_max, spectra_XYZ[j].f[1]/white.f[0]/CIE_Y_max, spectra_XYZ[j].f[2]/white.f[0]/CIE_Y_max, 1.0 };
      const char * name = NULL;
      oyjl_val val = oyjlTreeGetValueF( specT, 0, "collection/[0]/colors/[%d]/name", j );
      if(val)
        name = OYJL_GET_STRING(val);
      oyXYZ2sRGB( rgb );
      drawIlluminant( cr,
                      spectra, j,
                      xO, yO, width, height,
                      min_x, max_x, min_y < 0 ? min_y : 0.0, max_y,
                      no_color ? COLOR_GRAY : spectral_count == 1 ? COLOR_SPECTRAL : COLOR_COLOR, rgb,
                      pflags, name, spectral_count < 10 ? OY_DRAW_ID : 0, frame, thickness );
    }
    oyImage_Release( &spectra );
    drawTextTopLeft( cr, background_lightness, frame, xO, yO, width, height, min_x, max_x, min_y, max_y, 0,0, top_left_text );

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
  free(info);
  oyjlUi_Release( &ui );

  return 0;
}
#undef pflags

extern int * oyjl_debug;
static char ** environment = NULL;
#ifdef __ANDROID__
extern char **environ;
#endif
int main( int argc_, char**argv_, char ** envv OYJL_UNUSED )
{
  int argc = argc_;
  char ** argv = argv_;

#ifdef __ANDROID__
  argv = calloc( argc + 2, sizeof(char*) );
  memcpy( argv, argv_, argc * sizeof(char*) );
  argv[argc++] = "--render=gui"; /* start QML */
  argv[argc] = NULL;
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

/** @brief    CIE*LCh -> CIE*Lab, all in PCS*Lab range
 *
 *  @param[in]     i                   input LCh triple in normalised range of 0.0 ... 1.0 each channel
 *  @param[out]    o                   output Lab triple in normalised range of 0.0 ... 1.0 each channel
 *  @param         none                unused
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/12/05
 *  @since   2016/13/13 (Oyranos: 0.9.6)
 */
void         oyLCh2Lab               ( const double        i[],
                                       double              o[],
                                       void              * none OY_UNUSED )
{
  /* CIE*L */
  o[0] = i[0];
  /* CIE*a = C * cos(h) */
  o[1] = i[1] * cos(M_PI*2.0*i[2])/2.0 + 0.5;
  /* CIE*b = C * sin(h) */
  o[2] = i[1] * sin(M_PI*2.0*i[2])/2.0 + 0.5;
}
int  oyIsInRange                     ( const double        i,
                                       const double        reference,
                                       const double        delta )
{ 
  if(fabs(reference - i) < delta) return 1;
  return 0;
}
int  oyColorIsProofingMarker         ( const double        i[],
                                       double              delta )
{ 
  if( oyIsInRange(i[0], 0.5, delta) &&
      oyIsInRange(i[1], 0.5, delta) &&
      oyIsInRange(i[2], 0.5, delta) )
    return 1;
  return 0;
}
/** @brief   Lab in PCS*Lab range -> ICC profile -> inGamutTest
 *
 *  While it is possible to add multiple proofing profiles at once,
 *  the results will be washed out. It is much more precise to run
 *  this function for each proofing profile separately.
 *
 *  @param[in]     lab                 input Lab values
 *  @param[in]     count               count of 'lab' values
 *  @param[in]     space               color space for gamut boundary check; optional
 *  @param[in,out] is_outside_gamut    array with result of out of gamut test
 *  @param[in,out] lab_tested          the resulting colors
 *  @return                            results
 *                                     - -1 : no operation
 *                                     -  0 : success
 *                                     -  1 : found error
 *
 *  @version Oyranos: 0.9.7
 *  @date    2020/05/29
 *  @since   2020/03/11 (Oyranos: 0.9.7)
 */
int      oyLabGamutCheck             ( double            * lab,
                                       int                 count,
                                       oyProfiles_s      * proofing,
                                       int               * is_outside_gamut,
                                       double            * lab_tested )
{
  int error = -1;
  int icc_profile_flags = 0, i, found_outsider = 0;
  oyProfile_s * pLab = oyProfile_FromStd( oyASSUMED_LAB, icc_profile_flags, 0 )/*,
              * proof1 = oyProfiles_Get(proofing, 0);
  icColorSpaceSignature csp = (icColorSpaceSignature) oyProfile_GetSignature( proof1, oySIGNATURE_COLOR_SPACE)*/;
  double * tmp = lab_tested ? NULL : calloc( 3*count, sizeof(double) ),
         delta = 0.01;
  oyOptions_s * module_options = NULL;
  oyProfiles_s * profs = oyProfiles_Copy( proofing, NULL );
  if(!lab_tested) lab_tested = tmp;
  if(!lab_tested) return error;
  oyOptions_SetFromString( &module_options, OY_DEFAULT_RENDERING_INTENT, "1", OY_CREATE_NEW );
  oyOptions_SetFromString( &module_options, OY_DEFAULT_RENDERING_BPC, "0", OY_CREATE_NEW );
  if(oyProfiles_Count(proofing))
  {
    oyOptions_MoveInStruct( &module_options,
                                       OY_PROFILES_SIMULATION,
                                       (oyStruct_s**) &profs,
                                       OY_CREATE_NEW );
    oyOptions_SetFromString( &module_options, OY_DEFAULT_PROOF_SOFT, "1", OY_CREATE_NEW );
    oyOptions_SetFromString( &module_options, OY_DEFAULT_RENDERING_GAMUT_WARNING, "1", OY_CREATE_NEW ); /* This option expands the grid size and slows the initial context creation down. However, it is needed for precise enough output. */
    oyOptions_SetFromString( &module_options, OY_DEFAULT_RENDERING_INTENT_PROOF, "3", OY_CREATE_NEW );
  }
  oyConversion_s * cc = oyConversion_CreateBasicPixelsFromBuffers (
                                       pLab, lab, oyDataType_m(oyDOUBLE),
                                       pLab, lab_tested, oyDataType_m(oyDOUBLE),
                                       module_options, count );
  error = !cc;
  oyConversion_RunPixels( cc, 0 );

  for(i = 0; i < count; ++i)
  {
    int is_outside = is_outside_gamut[i];
    if(is_outside) continue;
    if(oyColorIsProofingMarker( &lab[i*3], delta + 0.02 )) /* test source in greater range */
      is_outside = 0;
    else
      is_outside = oyColorIsProofingMarker(&lab_tested[i*3], delta);
    if(is_outside)
    {
      ++found_outsider;
      is_outside_gamut[i] = is_outside;
    }
  }
  /*if((csp == icSigCmykData || csp == icSigRgbData) && found_outsider == 0)
    error = 1;*/

  oyProfile_Release( &pLab );
  oyOptions_Release( &module_options );
  oyConversion_Release( &cc );
  if(tmp)
    free(tmp);

  return error;
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

  pixel_layout |= oyChannels_m( (unsigned)spp );
  spec = oyImage_Create ( columns, 1, pixels, pixel_layout, p, NULL );
  oySpectrumSetRange( spec, min, max, lambda );

  return spec;
}
float    oySpectrumFillFromArrayF  ( oyImage_s * spec, float * curve, int column )
{
  int channels = (int)oySpectrumGetParam( spec, oySPECTRUM_CHANNELS );
  int i;
  float max = -1000000.0;
  int is_allocated = 0;
  double * dbl;
  if(!spec || !curve) return 0.0;

  dbl = (double*) oyImage_GetPointF(spec)( spec, column,0,-1, &is_allocated );

  for(i = 0; i < channels; ++i)
  {
    float v = curve[i];
    dbl[i] = (double)v;
    if(!isnan(v) && v > max) max = v;
  }
  return max;
}
float    oySpectrumFillFromArrayF3 ( oyImage_s * spec, float curves[][3] )
{
  int channels = (int)oySpectrumGetParam( spec, oySPECTRUM_CHANNELS );
  int i, index;
  float max = -1000000.0;

  for(index = 0; index < 3; ++index)
  {
    int is_allocated = 0;
    double * dbl = (double*) oyImage_GetPointF(spec)( spec, index,0,-1, &is_allocated );
    for(i = 1; i <= channels; ++i)
    {
      float v = curves[i][index];
      dbl[i] = (double)v;
      if(v > max)
        max = v;
    }
  }

  return max;
}
void     oySpectrumNormalise ( oyImage_s * spec, float scale )
{
  int channels = (int)oySpectrumGetParam( spec, oySPECTRUM_CHANNELS );
  int pixels = (int)oySpectrumGetParam( spec, oySPECTRUM_COLUMNS );
  int i, pixel;

  if(scale != 0.0)
  for(pixel = 0; pixel < pixels; ++pixel)
  {
    int is_allocated = 0;
    double * dbl = (double*) oyImage_GetPointF(spec)( spec, pixel,0,-1, &is_allocated );
    for(i = 0; i < channels; ++i)
      if(!isnan(dbl[i]))
        dbl[i] *= (double)scale;
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
           double start    = oySpectrumGetParam(spec, oySPECTRUM_START),
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

/** compute accumulative CIE*XYZ for single spectral value using the Std 2° CMF and custom illuminant
 *
 *  @param[in]     start               spectrum start
 *  @param[in]     lambda              spectral step size
 *  @param[in]     channel             spectral step to use for computing
 *  @param[in]     pos                 the position of the pixel
 *  @parem[in,out] xyz                 the accumulated XYZ triple
 *  @param[out]    white               the white point
 *  @param[in]     y                   spectral value in nm
 *  @param[in]     stdobs              standard observer; typical 2 for 2°, or 10 for 10°
 *  @param[in]     illu_spd            the illuminant spectrum, with start @300nm and 5nm lambda
 */
void        oySpecCompute ( int start, int lambda, int channel, int pos, oyF3 *xyz, oyF3 *white, double y, int stdobs, float * illu_spd )
{
  int cmf_startNM = 360;
  int cmf_start_pos = start - cmf_startNM;
  int illu_startNM = 300;
  int illu_start_pos = (start - illu_startNM) / 5;

  if( lambda%5 == 0 && start >= cmf_startNM )
  {
    double illu = 1.0;
    if(illu_spd)
      illu = illu_spd[illu_start_pos + channel*lambda/5] / 100.0; /* D50 max ~103 */
    int k;
    for(k = 0; k < 3; ++k)
    {
      double spd = 0.0;
      double weigthed = 0.0;

      if(stdobs == 2)
        spd = cieXYZ_31_2[1 + cmf_start_pos + channel*lambda][k];
      else if(stdobs == 10)
        spd = cieXYZ_64_10[cmf_start_pos + channel*lambda][k];
      weigthed = y * illu * spd;

      if(xyz)
        xyz->f[k] += weigthed;
      if(pos == 0)
      {
        weigthed = 1.0 * illu * spd;
        if(white)
          white->f[k] += weigthed;
      }
    }
  } else
    oyMessageFunc_p( oyMSG_ERROR,0, "need lambda of %5, got: %d; start: %d spd_start: %d", lambda%5, start, cmf_startNM );
}

int         oySpectrumComputeWhite( oyImage_s * spectra, int stdobs, float * illu_300_830_5nm, oyF3 *white, double * spectra_rect )
{
  int error = 0, i,j;

  if(spectra)
  {
    int lambda = oySpectrumGetParam( spectra, oySPECTRUM_LAMBDA );
    int start = oySpectrumGetParam( spectra, oySPECTRUM_START );
    int spectral_channels = oySpectrumGetParam( spectra, oySPECTRUM_CHANNELS );
    int spectral_count = oySpectrumGetParam( spectra, oySPECTRUM_COLUMNS );
    for(i = 0; i < spectral_channels; ++i)
    {
      float x = oySpectrumGet( spectra, -1, i );
      if(x < spectra_rect[0]) spectra_rect[0] = x;
      if(x > spectra_rect[1]) spectra_rect[1] = x;
      for(j = 0; j < spectral_count; ++j)
      {
        double y = oySpectrumGet( spectra, j, i );
        oySpecCompute ( start, lambda, i, j, NULL, white, y, stdobs, illu_300_830_5nm );
        if(isnan(y))
          continue;
        if(y < spectra_rect[2]) spectra_rect[2] = y;
        if(y > spectra_rect[3]) spectra_rect[3] = y;
      }
    }
  }

  return error;
}

/** compute CIE*XYZ for spectral color using the Std 2° CMF and custom illuminant
 *
 *  @param[in]     spectra             spectral image
 *  @param[in]     index               position in color list
 *  @param[in]     illu_300_830_5nm    illuminant spd 300-830nm lambda 5nm
 *  @param[out]    xyz                 ICC*XYZ triple
 */
int         oySpectrumComputeXYZ( oyImage_s * spectra, int index, float * illu_300_830_5nm, int stdobs, oyF3 *xyz )
{
  int error = 0, i, j = index;

  if(spectra)
  {
    int lambda = oySpectrumGetParam( spectra, oySPECTRUM_LAMBDA );
    int start = oySpectrumGetParam( spectra, oySPECTRUM_START );
    int spectral_channels = oySpectrumGetParam( spectra, oySPECTRUM_CHANNELS );
    int spectral_count = oySpectrumGetParam( spectra, oySPECTRUM_COLUMNS );

    if(j >= spectral_count)
    {
      error = 1;
      oyMessageFunc_p(oyMSG_ERROR,0,"Index out of range %d max: %d", j, spectral_count);
      return error;
    }

    for(i = 0; i < spectral_channels; ++i)
    {
      double y = oySpectrumGet( spectra, j, i );
      oySpecCompute ( start, lambda, i, j, xyz, NULL, y, stdobs, illu_300_830_5nm );
      if(isnan(y))
      {
        error = 1;
        continue;
      }
    }
  }

  return error;
}

static oyConversion_s * oy_xyz_srgb = NULL;
int oyXYZ2sRGB ( double * rgb )
{
  int error = 0;
  int icc_profile_flags = 0, i;
  static double rgb_[3];
 
  if(!oy_xyz_srgb)
  {
    oyProfile_s * pXYZ = oyProfile_FromStd( oyASSUMED_XYZ, icc_profile_flags, 0 ),
                * sRGB = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, 0 );
    oyOptions_s * options = NULL;
    oyOptions_SetFromString( &options, OY_DEFAULT_RENDERING_INTENT, "1", OY_CREATE_NEW );
    oy_xyz_srgb = oyConversion_CreateBasicPixelsFromBuffers (
                                       pXYZ, rgb_, oyDataType_m(oyDOUBLE),
                                       sRGB, rgb_, oyDataType_m(oyDOUBLE),
                                       options, 1 );
    oyProfile_Release( &sRGB );
    oyProfile_Release( &pXYZ );
    oyOptions_Release( &options );
  }
  error = !oy_xyz_srgb;

  for(i = 0; i < 3; ++i) rgb_[i] = rgb[i];
  oyConversion_RunPixels( oy_xyz_srgb, 0 );
  for(i = 0; i < 3; ++i) rgb[i] = rgb_[i];

  return error;
}

void drawIlluminant ( cairo_t * cr,
                      oyImage_s * spec, int index,
                      float xO, float yO, float width, float height,
                      float min_x, float max_x, float min_y, float max_y,
                      COLOR_MODE mode, double color[4],
                      uint32_t icc_profile_flags, const char * id, int flags,
                      int frame, double thickness )
{
  int channels = oySpectrumGetParam( spec, oySPECTRUM_CHANNELS );
  int start = oySpectrumGetParam( spec, oySPECTRUM_START ),
      end = oySpectrumGetParam( spec, oySPECTRUM_END );
  float max = -1000000.0;
  int max_pos = -1;

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

    if(y0 > max) { max = y0; max_pos = i; }
    if(y1 > max) { max = y1; max_pos = i+1; }

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
    cairo_pattern_destroy(g);

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

  if(id && flags & OY_DRAW_ID)
  {
    double x = oySpectrumGet(spec, -1, max_pos ),
           y = oySpectrumGet(spec, index, max_pos),
           x_pos = xToImage(x);
    cairo_text_extents_t e;
    cairo_set_font_size (cr, frame);
    cairo_text_extents (cr, id, &e);
    cairo_move_to (cr, x_pos > width - e.width + e.x_bearing ? width - e.width + e.x_bearing : x_pos,
                       yToImage(y + 0.003 * thickness));
    cairo_show_text (cr, id);
    if(verbose > 1)
      fprintf( stderr, "draw[%f %f %f %f] id: %s\n", x, y, x_pos, width, id );
  }

  if(verbose && index < 10)
  fprintf( stderr, " (%f) %d %s\n", max, channels, id );
  if(verbose && index == 10)
  fprintf( stderr, "drawing*sprectrum %d ... more might follow\n", index );

  oyProfile_Release( &sRGB );
  oyProfile_Release( &pLab );
  oyConversion_Release( &lab_srgb );
}

void drawTextTopLeft( cairo_t * cr,
                      double background_lightness, int frame,
                      double xO, double yO, double width, double height,
                      double min_x, double max_x, double min_y, double max_y,
                      int flags, double * text_width,
                      char * format, ... )
{
  char * tmp = NULL;
  const char * text = format;

  if(strchr(format, '%'))
  { OYJL_CREATE_VA_STRING(format, tmp, malloc, return)
    text = tmp;
  }

  cairo_set_source_rgba( cr, 0, 0, 0, 1.0 );
  if(0.0 <= background_lightness && background_lightness < 50.0)
    /* set font color */
    cairo_set_source_rgba( cr, 1.0, 1.0, 1.0, 1.0 );
  cairo_select_font_face(cr, "Sans",
                         CAIRO_FONT_SLANT_NORMAL,
                         CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size (cr, frame);
  cairo_move_to (cr, xToImage(min_x) - frame * 0.8,
                     yToImage(max_y - 0.025));

  if(text_width)
  {
    cairo_text_extents_t e;
    cairo_text_extents (cr, text, &e);
    *text_width = e.width;
  }

  if(!(flags & OY_NO_DRAW))
  cairo_show_text (cr, text);

  if(tmp) { free(tmp); tmp = NULL; }
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

  if(kelvin <= 10.0) error = 0;

  if(!error)
    oyAllocHelper_m_( spd, float, 108, oyAllocateFunc_, return 1 )

  if(spd)
  {
    for(i = 0; i < 107; ++i)
      if(kelvin > 10)
        spd[i] = spd_S1S2S3_5[i][0] + M1 * spd_S1S2S3_5[i][1] + M2* spd_S1S2S3_5[i][2];
      else
        spd[i] = kelvin;
  }

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
    int r = oyjlStringToDouble( text, &v, 0,0 );
    if(r <= 0) return v;
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

oyjl_val    oyjlTreeGetValueFilteredF( oyjl_val root, int flag, const char * filter, const char * format, ... )
{
  oyjl_val value = NULL;
  char * text = NULL;

  OYJL_CREATE_VA_STRING(format, text, malloc, return value)

  value = oyjlTreeGetValue( root, flag, text );

  if(!value)
  {
    char * t = oyjlStringCopy( text, 0 );
    if(t)
    {
      oyjlStringReplace( &t, filter, "", 0,0 );
      value = oyjlTreeGetValue( root, 0, t );
      free(t);
    }
  }

  if(text) free(text);

  return value;
}
/** @brief copy nodes
 *
 *  @param[in]     src                 source tree
 *  @param[in]     in                  source path
 *  @param[out]    dst                 destination tree
 *  @param[in]     out                 destination path
 *  @param[in]     out_pos             destination path position specified by %d within 'out'
 *  @return                            error: 0 - success; 1 - not found; 2 - no text; 3 - JSON error
 */
int         oyjlCopyNode             ( oyjl_val            src,
                                       const char        * in,
                                       oyjl_val            dst,
                                       const char        * out,
                                       int                 out_pos )
{
  oyjl_val v = oyjlTreeGetValueFilteredF(src, 0, "cc:", in);
  char * value;
  if(!v) return 1;
  value = oyjlValueText( v, 0 );
  if(!value) return 2;
  if(oyjlTreeSetStringF( dst, OYJL_CREATE_NEW, value, out, out_pos ))
    return 3;
  free(value);
  return 0;
}

/* CxF NCC conversion table */
const char * oy_cxf_json[][3] = {
  {"cc:CxF/cc:FileInformation/cc:Description",  "description",  "DESCRIPTOR"},
  {"cc:CxF/cc:FileInformation/cc:CreationDate", "date",         "CREATED"},
  {"cc:CxF/cc:FileInformation/cc:Creator",      "creator",      "ORIGINATOR"},
  {NULL, NULL, NULL}
};
const char * oy_cxf_color_spec_json[][3] = {
  {"@Id",                               "collection/[0]/spectral/[%d]/id", ""},
  {"cc:TristimulusSpec/cc:Illuminant",  "collection/[0]/spectral/[%d]/tristimulus/illuminant", ""},
  {"cc:TristimulusSpec/cc:Observer",    "collection/[0]/spectral/[%d]/tristimulus/observer", ""},
  {"cc:ColorSpecification/cc:TristimulusSpec/cc:Method",      "collection/[0]/spectral/[%d]/tristimulus/method", ""},
  {"cc:ColorSpecification/cc:MeasurementSpec/cc:GeometryChoice/cc:SingleAngle/cc:IlluminationAngle", "collection/[0]/spectral/[%d]/measurement/illumination_angle", ""},
  {"cc:ColorSpecification/cc:MeasurementSpec/cc:GeometryChoice/cc:SingleAngle/cc:MeasurementAngle", "collection/[0]/spectral/[%d]/measurement/observer_angle", ""},
  {"cc:ColorSpecification/cc:MeasurementSpec/cc:CalibrationStandard", "collection/[0]/spectral/[%d]/measurement/calibration", ""},
  {"cc:ColorSpecification/cc:MeasurementSpec/cc:Aperture",    "collection/[0]/spectral/[%d]/measurement/aperture", ""},
  {"cc:ColorSpecification/cc:MeasurementSpec/cc:Device/cc:Model", "collection/[0]/spectral/[%d]/measurement/device/model", ""},
  {"cc:ColorSpecification/cc:MeasurementSpec/cc:Device/cc:DeviceFilter", "collection/[0]/spectral/[%d]/measurement/device/filter", ""},
  {"cc:ColorSpecification/cc:MeasurementSpec/cc:Device/cc:DeviceIllumination", "collection/[0]/spectral/[%d]/measurement/device/illumination", ""},
  {NULL, NULL, NULL}
};
oyjl_val    oyTreeFromCxf( const char * text )
{
  char error_buffer[128] = {0};
  size_t error_buffer_size = 128;
  oyjl_val root = oyjlTreeParseXml( text, 0, error_buffer, error_buffer_size ),
           specT = oyjlTreeNew(""),
           collection = oyjlTreeGetValueFilteredF( root, 0, "cc:", "cc:CxF/cc:Resources/cc:ObjectCollection/cc:Object//cc:ColorValues" ); /* detect the array or single */
  int count = oyjlValueCount( collection ), i;
  oyjl_val color_spec = oyjlTreeGetValueFilteredF( root, 0, "cc:", "cc:CxF/cc:Resources/cc:ColorSpecificationCollection/cc:ColorSpecification/cc:MeasurementSpec/cc:WavelengthRange" );
  int color_spec_count = oyjlValueCount( color_spec );
  long startNM = 0, lambda = 0, endNM = 0, n_max = 0;
  char * startWL, * increment;

  oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, "ncc1", "type" );
  oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, "Named Color Collection v1", "comment" );

  if(count == 0)
  { /* single color */
    collection = oyjlTreeGetValueFilteredF( root, 0, "cc:", "cc:CxF/cc:Resources/cc:ObjectCollection" );
    count = 1;
  } else
  { /* array of colors */
    collection = oyjlTreeGetValueFilteredF( root, 0, "cc:", "cc:CxF/cc:Resources/cc:ObjectCollection/cc:Object" );
    count = oyjlValueCount( collection );
  }

  /* search for spectral parameters in more than one MeasurementSpec */
  if(color_spec_count < 1)
  {
    color_spec  = oyjlTreeGetValueFilteredF( root, 0, "cc:", "cc:CxF/cc:Resources/cc:ColorSpecificationCollection/cc:ColorSpecification" );
    color_spec_count = oyjlValueCount( color_spec );
    for(i = 0; i < color_spec_count; ++i)
    {
      color_spec = oyjlTreeGetValueFilteredF( root, 0, "cc:", "cc:CxF/cc:Resources/cc:ColorSpecificationCollection/cc:ColorSpecification/[%i]", i );
      int j = 0;
      while(oy_cxf_color_spec_json[j][0])
      {
        int error = 0;
        const char * in = oy_cxf_color_spec_json[j][0], * out = oy_cxf_color_spec_json[j][1];
        error = oyjlCopyNode(color_spec, in, specT, out, i);
        if(error)
          oyMessageFunc_p( oyMSG_WARN, NULL, "copy from CxF node to NCC node failed: %s %s", in, out );
        ++j;
      }
    }
    color_spec  = oyjlTreeGetValueFilteredF( root, 0, "cc:", "cc:CxF/cc:Resources/cc:ColorSpecificationCollection/cc:ColorSpecification" );
    color_spec_count = oyjlValueCount( color_spec );
    for(i = 0; i < color_spec_count; ++i)
    {
      color_spec = oyjlTreeGetValueFilteredF( root, 0, "cc:", "cc:CxF/cc:Resources/cc:ColorSpecificationCollection/cc:ColorSpecification/[%i]/cc:MeasurementSpec/cc:WavelengthRange/@StartWL", i );
      if(color_spec)
      {
        color_spec = oyjlTreeGetValueFilteredF( root, 0, "cc:", "cc:CxF/cc:Resources/cc:ColorSpecificationCollection/cc:ColorSpecification/[%i]", i );
        break;
      }
    }
  } else
    color_spec = oyjlTreeGetValueFilteredF( root, 0, "cc:", "cc:CxF/cc:Resources/cc:ColorSpecificationCollection/cc:ColorSpecification" );

  startWL = oyjlValueText( oyjlTreeGetValueFilteredF( color_spec, 0, "cc:", "cc:MeasurementSpec/cc:WavelengthRange/@StartWL"), 0 );
  increment = oyjlValueText( oyjlTreeGetValueFilteredF( color_spec, 0, "cc:", "cc:MeasurementSpec/cc:WavelengthRange/@Increment"), 0 );

  i = 0;
  while(oy_cxf_json[i][0])
  {
    int error = 0;
    const char * in = oy_cxf_json[i][0], * out = oy_cxf_json[i][1];
    error = oyjlCopyNode(root, in, specT, out, 0);
    if(i <= 2 && error)
      oyMessageFunc_p( oyMSG_WARN, NULL, "copy from CxF node to NCC node failed: %s %s", in, out );
    ++i;
  }; 

  if(startWL)
  {
    oyjlStringToLong(startWL, &startNM, 0);
    oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, startNM, "collection/[0]/spectral/[0]/startNM" );
    free(startWL);
    startWL = NULL;
  }
  if(increment)
  {
    oyjlStringToLong(increment, &lambda, 0);
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
         * startWl = oyjlValueText( oyjlTreeGetValueFilteredF( obj, 0, "cc:", "cc:ColorValues//@StartWL" ), 0 ),
         * reflSpec_text = oyjlValueText( oyjlTreeGetValueFilteredF( obj, 0, "cc:", "cc:ColorValues//@text" ), 0 ),
         * x,*y,*z,*L,*a,*b,*R,*G,*B,*C,*M,*Y,*K;
    oyjl_val xyz = oyjlTreeGetValueFilteredF( obj, 0, "cc:", "cc:ColorValues/cc:ColorCIEXYZ" ),
             lab = oyjlTreeGetValueFilteredF( obj, 0, "cc:", "cc:ColorValues/cc:ColorCIELab" ),
             rgb = oyjlTreeGetValueFilteredF( obj, 0, "cc:", "cc:ColorValues/cc:ColorSRGB" ),
             cmyk = oyjlTreeGetValueFilteredF( obj, 0, "cc:", "cc:ColorValues/cc:ColorCMYK" );
    int n = 0, j, r = 0;
    long startNm = 0;
    double * list = NULL, d = 0.0;
    char * colorSpec = oyjlValueText( oyjlTreeGetValueFilteredF( obj, 0, "cc:", "cc:ColorValues//@ColorSpecification" ), 0 );

    if(!reflSpec_text)
    {
      n = oyjlValueCount(oyjlTreeGetValueFilteredF( obj, 0, "cc:", "cc:ColorValues" ));
      for(j = 0; j < n; ++j)
      {
        if(!reflSpec_text) reflSpec_text = oyjlValueText( oyjlTreeGetValueFilteredF( obj, 0, "cc:", "cc:ColorValues/[%d]/@text", j ), 0 );
        if(!xyz) xyz = oyjlTreeGetValueFilteredF( obj, 0, "cc:", "cc:ColorValues/[%d]/cc:ColorCIEXYZ", j );
        if(!lab) lab = oyjlTreeGetValueFilteredF( obj, 0, "cc:", "cc:ColorValues/[%d]/cc:ColorCIELab", j );
        if(!rgb) rgb = oyjlTreeGetValueFilteredF( obj, 0, "cc:", "cc:ColorValues/[%d]/cc:ColorSRGB", j );
        if(!cmyk) cmyk = oyjlTreeGetValueFilteredF( obj, 0, "cc:", "cc:ColorValues/[%d]/cc:ColorCMYK", j );
      }
    }

    if(!reflSpec_text && !xyz && !lab && !rgb && !cmyk)
    {
      fprintf( stderr, "ERROR parsing: %d %s - no ColorValues//", i, name );
      continue;
    }

    if(name)
      oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, name, "collection/[0]/colors/[%d]/name", i );

    if(reflSpec_text)
    {
      n = 0;
      if(oyjlStringsToDoubles( reflSpec_text, 0, &n, 0, &list ))
        fprintf( stderr, "ERROR parsing: %d %s %s n=%d ", i, name, reflSpec_text, n );

      if(colorSpec)
        oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, colorSpec, "collection/[0]/colors/[%d]/spectral/[0]/id", i );

      if(startWl)
        oyjlStringToLong(startWl, &startNm, 0);
      //fprintf( stderr, "  %d: %s - %ld-%ld ", i, name, startNM, startNM+n*lambda );
      for(j = 0; j < n; ++j)
        oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, list[j], "collection/[0]/colors/[%d]/spectral/[0]/data/[%d]", i, j );
      if(n_max < j-1) n_max = j-1;
    }

    if(xyz)
    {
      x = oyjlValueText( oyjlTreeGetValueFilteredF( xyz, 0, "cc:", "cc:X" ), 0 );
      y = oyjlValueText( oyjlTreeGetValueFilteredF( xyz, 0, "cc:", "cc:Y" ), 0 );
      z = oyjlValueText( oyjlTreeGetValueFilteredF( xyz, 0, "cc:", "cc:Z" ), 0 );
      if(x) r = oyjlStringToDouble( x, &d, 0,0 );
      if(r <= 0) oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, d/100.0, "collection/[0]/colors/[%d]/xyz/[0]/data/[0]", i );
      if(y) r = oyjlStringToDouble( y, &d, 0,0 );
      if(r <= 0) oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, d/100.0, "collection/[0]/colors/[%d]/xyz/[0]/data/[1]", i );
      if(z) r = oyjlStringToDouble( z, &d, 0,0 );
      if(r <= 0) oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, d/100.0, "collection/[0]/colors/[%d]/xyz/[0]/data/[2]", i );
    }
    if(lab)
    {
      L = oyjlValueText( oyjlTreeGetValueFilteredF( lab, 0, "cc:", "cc:L" ), 0 );
      a = oyjlValueText( oyjlTreeGetValueFilteredF( lab, 0, "cc:", "cc:A" ), 0 );
      b = oyjlValueText( oyjlTreeGetValueFilteredF( lab, 0, "cc:", "cc:B" ), 0 );
      if(L) r = oyjlStringToDouble( L, &d, 0,0 );
      if(r <= 0) oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, d/100.0,       "collection/[0]/colors/[%d]/lab/[0]/data/[0]", i );
      if(a) r = oyjlStringToDouble( a, &d, 0,0 );
      if(r <= 0) oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, d/255.0 + 0.5, "collection/[0]/colors/[%d]/lab/[0]/data/[1]", i );
      if(b) r = oyjlStringToDouble( b, &d, 0,0 );
      if(r <= 0) oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, d/255.0 + 0.5, "collection/[0]/colors/[%d]/lab/[0]/data/[2]", i );
    }
    if(rgb)
    {
      R = oyjlValueText( oyjlTreeGetValueFilteredF( rgb, 0, "cc:", "cc:R" ), 0 );
      G = oyjlValueText( oyjlTreeGetValueFilteredF( rgb, 0, "cc:", "cc:G" ), 0 );
      B = oyjlValueText( oyjlTreeGetValueFilteredF( rgb, 0, "cc:", "cc:B" ), 0 );
      if(R) r = oyjlStringToDouble( R, &d, 0,0 );
      if(r <= 0) oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, d/255.0, "collection/[0]/colors/[%d]/rgb/[0]/data/[0]", i );
      if(G) r = oyjlStringToDouble( G, &d, 0,0 );
      if(r <= 0) oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, d/255.0, "collection/[0]/colors/[%d]/rgb/[0]/data/[1]", i );
      if(B) r = oyjlStringToDouble( B, &d, 0,0 );
      if(r <= 0) oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, d/255.0, "collection/[0]/colors/[%d]/rgb/[0]/data/[2]", i );
    }
    if(cmyk)
    {
      C = oyjlValueText( oyjlTreeGetValueFilteredF( cmyk, 0, "cc:", "cc:C" ), 0 );
      M = oyjlValueText( oyjlTreeGetValueFilteredF( cmyk, 0, "cc:", "cc:M" ), 0 );
      Y = oyjlValueText( oyjlTreeGetValueFilteredF( cmyk, 0, "cc:", "cc:Y" ), 0 );
      K = oyjlValueText( oyjlTreeGetValueFilteredF( cmyk, 0, "cc:", "cc:K" ), 0 );
      if(C) r = oyjlStringToDouble( C, &d, 0,0 );
      if(r <= 0) oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, d/100.0, "collection/[0]/colors/[%d]/cmyk/[0]/data/[0]", i );
      if(M) r = oyjlStringToDouble( M, &d, 0,0 );
      if(r <= 0) oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, d/100.0, "collection/[0]/colors/[%d]/cmyk/[0]/data/[1]", i );
      if(Y) r = oyjlStringToDouble( Y, &d, 0,0 );
      if(r <= 0) oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, d/100.0, "collection/[0]/colors/[%d]/cmyk/[0]/data/[2]", i );
      if(K) r = oyjlStringToDouble( K, &d, 0,0 );
      if(r <= 0) oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, d/100.0, "collection/[0]/colors/[%d]/cmyk/[0]/data/[3]", i );
    }
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

  v = oyjlTreeGetValue( root, 0, "creator" );
  if(!v) { oyMessageFunc_p(oyMSG_WARN,NULL,"creator missed"); *creator = NULL; }
  else *creator = oyjlStringCopy( v->u.string, 0 );

  v = oyjlTreeGetValue( root, 0, "date" );
  if(!v) { oyMessageFunc_p(oyMSG_WARN,NULL,"date missed"); *creation_date = NULL; }
  else *creation_date = oyjlStringCopy( v->u.string, 0 );

  v = oyjlTreeGetValue( root, 0, "description" );
  if(!v) { oyMessageFunc_p(oyMSG_WARN,NULL,"description missed"); *description = NULL; }
  else *description = oyjlStringCopy( v->u.string, 0 );

  v = oyjlTreeGetValue( root, 0, "collection/[0]/colors" );
  if(!v) { oyMessageFunc_p(oyMSG_WARN,NULL,"colors missed"); return NULL; }
  *pixels = oyjlValueCount( v );

  v = oyjlTreeGetValue( root, 0, "collection/[0]/spectral/[0]/startNM" );
  if(!v) { oyMessageFunc_p(oyMSG_ERROR,NULL,"startNM missed"); return NULL; }
  else *startNM = v->u.number.d;

  v = oyjlTreeGetValue( root, 0, "collection/[0]/spectral/[0]/endNM" );
  if(!v) { oyMessageFunc_p(oyMSG_ERROR,NULL,"endNM missed"); return NULL; }
  else *endNM = v->u.number.d;

  v = oyjlTreeGetValue( root, 0, "collection/[0]/spectral/[0]/lambda" );
  if(!v) { oyMessageFunc_p(oyMSG_ERROR,NULL,"lambda missed"); return NULL; }
  else *lambda = v->u.number.d;

  *spp = (*endNM - *startNM) / *lambda + 1;

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
      samples = 0,
      channels = 0;
  oyjl_val v, data;
  double  start, end, lambda;
  char * name, * creator, *creation_date, * description;
  double d;

  if(!root) return 1;
  data = oyTreeGetParam( root, &lambda, &start, &end, &pixels, &samples, &creator, &creation_date, &description );
  if(!data)
    data = oyjlTreeGetValue( root, 0, "collection/[0]/colors" );

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
    else
    {
    /* copy INSTRUMENTATION line */
      v = oyjlTreeGetValue( root, 0, "collection/[0]/spectral/[0]/measurement/device/instrumentation" );
      const char  * instrumentation = v ? v->u.string : NULL;
      if(instrumentation)
        oyjlStringAdd( &tmp, 0,0, "INSTRUMENTATION \"%s\"\n", instrumentation );
    }

    /* copy MEASUREMENT_SOURCE line */
    v = oyjlTreeGetValue( root, 0, "collection/[0]/spectral/[0]/measurement/device/illumination" );
    const char  * illumination = v ? v->u.string : NULL;
    if(illumination)
      oyjlStringAdd( &tmp, 0,0, "MEASUREMENT_SOURCE \"%s\"\n", illumination );
    /* copy DEVICE_CLASS line */
    char * device_class = oyjlValueText( oyjlTreeGetValueF( root, 0, "collection/[0]/spectral/[0]/measurement/device/class" ), 0 );
    if(device_class)
      oyjlStringAdd( &tmp, 0,0, "KEYWORD \"DEVICE_CLASS\"\nDEVICE_CLASS \"%s\"\n", device_class );

    if(oyjlTreeGetValueF( data, 0, "[0]/lab/[0]/data" )) channels += 3;
    if(oyjlTreeGetValueF( data, 0, "[0]/rgb/[0]/data" )) channels += 3;
    if(oyjlTreeGetValueF( data, 0, "[0]/cmyk/[0]/data" )) channels += 4;

    if(start)
      oyjlStringAdd( &tmp, 0,0, "SPECTRAL_BANDS \"%d\"\nSPECTRAL_START_NM \"%f\"\nSPECTRAL_END_NM \"%f\"\nSPECTRAL_NORM \"%f\"\n\nNUMBER_OF_FIELDS %d\nBEGIN_DATA_FORMAT\nSAMPLE_ID",
      samples, start, end, lambda, 1 + samples + channels );
    else
      oyjlStringAdd( &tmp, 0,0, "\nNUMBER_OF_FIELDS %d\nBEGIN_DATA_FORMAT\nSAMPLE_ID", 1 + samples + channels );

    v = oyjlTreeGetValueF( data, 0, "[0]/lab/[0]/data" );
    if(v)
    {
      oyjlStringAdd( &tmp, 0,0, "\tLAB_L" );
      oyjlStringAdd( &tmp, 0,0, "\tLAB_A" );
      oyjlStringAdd( &tmp, 0,0, "\tLAB_B" );
    }

    v = oyjlTreeGetValueF( data, 0, "[0]/rgb/[0]/data" );
    if(v)
    {
      oyjlStringAdd( &tmp, 0,0, "\tRGB_R" );
      oyjlStringAdd( &tmp, 0,0, "\tRGB_G" );
      oyjlStringAdd( &tmp, 0,0, "\tRGB_B" );
    }

    v = oyjlTreeGetValueF( data, 0, "[0]/cmyk/[0]/data" );
    if(v)
    {
      oyjlStringAdd( &tmp, 0,0, "\tCMYK_C" );
      oyjlStringAdd( &tmp, 0,0, "\tCMYK_M" );
      oyjlStringAdd( &tmp, 0,0, "\tCMYK_Y" );
      oyjlStringAdd( &tmp, 0,0, "\tCMYK_K" );
    }

    for(i = 0; i < samples; ++i)
    {
      int nm = start + i*lambda;
      oyjlStringAdd( &tmp, 0,0, "\tSPECTRAL_%d", nm );
    }
    oyjlStringAdd( &tmp, 0,0, "\nEND_DATA_FORMAT\n\nNUMBER_OF_SETS %d\nBEGIN_DATA\n", (int)pixels );
    t = oyjlStr_NewFrom(&tmp,0,0,0);
  }

  if(t && pixels >= 1)
  {
    for(index = 0; index < pixels; ++index)
    {
      char f[32];
      v = oyjlTreeGetValueF( data, 0, "[%d]/name", index );
      if(!v)
        v = oyjlTreeGetValueF( data, 0, "[%d]/id", index );
      name = OYJL_GET_STRING(v);
      if(name)
      {
        oyjlStr_AppendN( t, "\"", 1 );
        oyjlStr_AppendN( t, name, strlen(name) );
        oyjlStr_AppendN( t, "\"", 1 );
      }

      v = oyjlTreeGetValueF( data, 0, "[%d]/lab/[0]/data", index );
      if(v)
      {
        for(i = 0; i < 3; ++i)
        {
          v = oyjlTreeGetValueF( data, 0, "[%d]/lab/[0]/data/[%d]", index, i );
          if(!v)
          {
            fprintf(stderr, "ERROR with i = %d index = %d (spp = %d pixels = %d)\n", i, index, 3, pixels);
            d = NAN;
          } else
            d = OYJL_GET_DOUBLE(v);
          oyjlStr_AppendN( t, "\t", 1 );
          if(i == 0)
            sprintf(f, "%f", d*100.0);
          else
            sprintf(f, "%f", (d-0.5)*256.0);
          oyjlStr_AppendN( t, f, strlen(f) );
        }
      }

      v = oyjlTreeGetValueF( data, 0, "[%d]/rgb/[0]/data", index );
      if(v)
      {
        for(i = 0; i < 3; ++i)
        {
          v = oyjlTreeGetValueF( data, 0, "[%d]/rgb/[0]/data/[%d]", index, i );
          if(!v)
          {
            fprintf(stderr, "ERROR with i = %d index = %d (spp = %d pixels = %d)\n", i, index, 3, pixels);
            d = NAN;
          } else
            d = OYJL_GET_DOUBLE(v);
          oyjlStr_AppendN( t, "\t", 1 );
          sprintf(f, "%f", d*255.0);
          oyjlStr_AppendN( t, f, strlen(f) );
        }
      }

      v = oyjlTreeGetValueF( data, 0, "[%d]/cmyk/[0]/data", index );
      if(v)
      {
        for(i = 0; i < 4; ++i)
        {
          v = oyjlTreeGetValueF( data, 0, "[%d]/cmyk/[0]/data/[%d]", index, i );
          if(!v)
          {
            fprintf(stderr, "ERROR with i = %d index = %d (spp = %d pixels = %d)\n", i, index, 3, pixels);
            d = NAN;
          } else
            d = OYJL_GET_DOUBLE(v);
          oyjlStr_AppendN( t, "\t", 1 );
          sprintf(f, "%f", d*100.0);
          oyjlStr_AppendN( t, f, strlen(f) );
        }
      }

      for(i = 0; i < samples; ++i)
      {
        v = oyjlTreeGetValueF( data, 0, "[%d]/spectral/[0]/data/[%d]", index, i );
        if(!v)
        {
          fprintf(stderr, "ERROR with i = %d index = %d (spp = %d pixels = %d)\n", i, index, samples, pixels);
          d = NAN;
        } else
          d = OYJL_GET_DOUBLE(v);
        oyjlStr_AppendN( t, "\t", 1 );
        sprintf(f, "%f", d);
        oyjlStr_AppendN( t, f, strlen(f) );
      }
      oyjlStr_AppendN( t, "\n", 1 );
    }
    oyjlStr_AppendN( t, "END_DATA\n", 9 );
  }

#ifdef USE_GETTEXT
  setlocale(LC_ALL,old_loc);
  free(old_loc);
#endif

  if(!t) return 1;

  *text = oyjlStr_Pull( t );
  oyjlStr_Release( &t );
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
    t = oyjlStr_New(0,0,0);
  if(t)
  {
    oyjlStr_AppendN( t, "\"Wavelength (nm)/Name\"", 22 );
    for(index = 0; index < pixels; ++index)
    {
      v = oyjlTreeGetValueF( data, 0, "[%d]/name", index );
      name = OYJL_GET_STRING(v);
      oyjlStr_AppendN( t, ",\"", 2 );
      if(name)
        oyjlStr_AppendN( t, name, strlen(name) );
      oyjlStr_AppendN( t, "\"", 1 );
    }
    oyjlStr_AppendN( t, "\n", 1 );
    for(i = 0; i < samples; ++i)
    {
      for(index = 0; index < pixels; ++index)
      {
        char f[32];
        if(index == 0)
        {
          sprintf(f, "%d", (int)(start + i*lambda));
          oyjlStr_AppendN( t, f, strlen(f) );
        }

        v = oyjlTreeGetValueF( data, 0, "[%d]/spectral/[0]/data/[%d]", index, i );
        if(!v)
        {
          fprintf(stderr, "ERROR with i = %d index = %d (spp = %d pixels = %d)\n", i, index, samples, pixels);
          d = NAN;
        } else
          d = OYJL_GET_DOUBLE(v);
        sprintf(f, "%f", d);
        oyjlStr_AppendN( t, ",", 1 );
        oyjlStr_AppendN( t, f, strlen(f) );
      }
      oyjlStr_AppendN( t, "\n", 1 );
    }
  }

#ifdef USE_GETTEXT
  setlocale(LC_ALL,old_loc);
  free(old_loc);
#endif

  if(!t) return 1;

  *text = oyjlStr_Pull( t );
  oyjlStr_Release( &t );
  return 0;
}


int         oyTreeToIccXml( oyjl_val root, int * level OYJL_UNUSED, char ** text )
{
  int i,index,
      error = 0;
  char * json = NULL;

  int pixels = 0,
      samples = 0;
  oyjl_val v, data, icc, nmcl;
  double  start, end, lambda;
  char * name, * creator, *creation_date, * description;
  double d;

  if(!root) return 1;
  data = oyTreeGetParam( root, &lambda, &start, &end, &pixels, &samples, &creator, &creation_date, &description );
  if(!creator) creator = "Oyranos CMS";
#ifdef USE_GETTEXT
  char * old_loc = strdup(setlocale(LC_ALL,NULL));
  setlocale(LC_ALL,"C");
#endif

  if(data)
  {
    icc = oyjlTreeNew("");

    oyjlTreeGetValue( icc, OYJL_CREATE_NEW,                     "IccProfile/Header/PreferredCMMType" );
    oyjlTreeSetStringF( icc, OYJL_CREATE_NEW, "5.00",           "IccProfile/Header/ProfileVersion" );
    oyjlTreeSetStringF( icc, OYJL_CREATE_NEW, "nmcl",           "IccProfile/Header/ProfileDeviceClass" );
    oyjlTreeGetValue( icc, OYJL_CREATE_NEW,                     "IccProfile/Header/DataColourSpace" );
    v = oyjlTreeGetValueF( data, 0, "[%d]/xyz", 0 );
    if(v)
      oyjlTreeSetStringF(icc,OYJL_CREATE_NEW, "XYZ ",           "IccProfile/Header/PCS" );
    else
      oyjlTreeSetStringF(icc,OYJL_CREATE_NEW, "Lab ",           "IccProfile/Header/PCS" );
    oyjlTreeSetStringF( icc, OYJL_CREATE_NEW, "now",            "IccProfile/Header/CreationDateTime" );
    oyjlTreeSetStringF( icc, OYJL_CREATE_NEW, "true",           "IccProfile/Header/ProfileFlags/@EmbeddedInFile" );
    oyjlTreeSetStringF( icc, OYJL_CREATE_NEW, "false",          "IccProfile/Header/ProfileFlags/@UseWithEmbeddedDataOnly" );
    oyjlTreeSetStringF( icc, OYJL_CREATE_NEW, "reflective",     "IccProfile/Header/DeviceAttributes/@ReflectiveOrTransparency" );
    oyjlTreeSetStringF( icc, OYJL_CREATE_NEW, "glossy",         "IccProfile/Header/DeviceAttributes/@GlossyOrMatte" );
    oyjlTreeSetStringF( icc, OYJL_CREATE_NEW, "positive",       "IccProfile/Header/DeviceAttributes/@MediaPolarity" );
    oyjlTreeSetStringF( icc, OYJL_CREATE_NEW, "colour",         "IccProfile/Header/DeviceAttributes/@MediaColour" );
    oyjlTreeSetStringF( icc, OYJL_CREATE_NEW, "Absolute Coloriemtric", "IccProfile/Header/RenderingIntent" );
    oyjlTreeSetStringF( icc, OYJL_CREATE_NEW, "0.96420288",     "IccProfile/Header/PCSIlluminant/XYZNumber/@X" );
    oyjlTreeSetStringF( icc, OYJL_CREATE_NEW, "1.00000000",     "IccProfile/Header/PCSIlluminant/XYZNumber/@Y" );
    oyjlTreeSetStringF( icc, OYJL_CREATE_NEW, "0.82490540",     "IccProfile/Header/PCSIlluminant/XYZNumber/@Z" );
    oyjlTreeGetValue( icc, OYJL_CREATE_NEW,                     "IccProfile/Header/ProfileCreator" );
    oyjlTreeSetStringF( icc, OYJL_CREATE_NEW, "1",              "IccProfile/Header/ProfileID" );
    oyjlTreeGetValue( icc, OYJL_CREATE_NEW,                     "IccProfile/Header/SpectralPCS" );
    oyjlTreeSetDoubleF( icc, OYJL_CREATE_NEW, start,            "IccProfile/Header/SpectralRange/Wavelengths/@start" );
    oyjlTreeSetDoubleF( icc, OYJL_CREATE_NEW, end,              "IccProfile/Header/SpectralRange/Wavelengths/@end" );
    oyjlTreeSetDoubleF( icc, OYJL_CREATE_NEW, samples,          "IccProfile/Header/SpectralRange/Wavelengths/@steps" );
    oyjlTreeSetStringF( icc, OYJL_CREATE_NEW, "desc",           "IccProfile/Tags/multiLocalizedUnicodeType/[0]/TagSignature" );
    oyjlTreeSetStringF( icc, OYJL_CREATE_NEW, "enUS",           "IccProfile/Tags/multiLocalizedUnicodeType/[0]/LocalizedText/@LanguageCountry" );
    oyjlTreeSetStringF( icc, OYJL_CREATE_NEW, description,      "IccProfile/Tags/multiLocalizedUnicodeType/[0]/LocalizedText/@cdata" );
    oyjlTreeSetStringF( icc, OYJL_CREATE_NEW, "cprt",           "IccProfile/Tags/multiLocalizedUnicodeType/[1]/TagSignature" );
    oyjlTreeSetStringF( icc, OYJL_CREATE_NEW, "enUS",           "IccProfile/Tags/multiLocalizedUnicodeType/[1]/LocalizedText/@LanguageCountry" );
    oyjlTreeSetStringF( icc, OYJL_CREATE_NEW, creator,          "IccProfile/Tags/multiLocalizedUnicodeType/[1]/LocalizedText/@cdata" );
    oyjlTreeSetStringF( icc, OYJL_CREATE_NEW, "ncl2",           "IccProfile/Tags/tagArrayType/TagSignature" );
    oyjlTreeSetStringF( icc, OYJL_CREATE_NEW, "nmcl",           "IccProfile/Tags/tagArrayType/ArraySignature" );
    oyjlTreeSetStringF( icc, OYJL_CREATE_NEW, "wtpt",           "IccProfile/Tags/XYZType/TagSignature" );
    oyjlTreeSetStringF( icc, OYJL_CREATE_NEW, "0.96420288",     "IccProfile/Tags/XYZType/XYZNumber/@X" );
    oyjlTreeSetStringF( icc, OYJL_CREATE_NEW, "1.00000000",     "IccProfile/Tags/XYZType/XYZNumber/@Y" );
    oyjlTreeSetStringF( icc, OYJL_CREATE_NEW, "0.82490540",     "IccProfile/Tags/XYZType/XYZNumber/@Z" );

    nmcl = oyjlTreeGetValue( icc, OYJL_CREATE_NEW,              "IccProfile/Tags/tagArrayType/ArrayTags/tagStructureType" );
    for(index = 0; index < pixels; ++index)
    {
      char * spec = NULL;
      v = oyjlTreeGetValueF( data, 0, "[%d]/name", index );
      name = OYJL_GET_STRING(v);
      oyjlTreeSetStringF( nmcl, OYJL_CREATE_NEW, "nmcl",        "[%d]/StructureSignature", index );
      oyjlTreeSetStringF( nmcl, OYJL_CREATE_NEW, "name",        "[%d]/MemberTags/utf8TextType/TagSignature", index );
      oyjlTreeSetStringF( nmcl, OYJL_CREATE_NEW, name,          "[%d]/MemberTags/utf8TextType/TextData", index );


      for(i = 0; i < samples; ++i)
      {
        v = oyjlTreeGetValueF( data, 0, "[%d]/spectral/[0]/data/[%d]", index, i );
        if(!v)
        {
          fprintf(stderr, "ERROR with i = %d index = %d (spp = %d pixels = %d)\n", i, index, samples, pixels);
          d = NAN;
        } else
          d = OYJL_GET_DOUBLE(v);
        if(i)
          oyjlStringAdd( &spec, 0,0, "\t" );
        oyjlStringAdd( &spec, 0,0, "%f", d );
      }


      oyjlTreeSetStringF( nmcl, OYJL_CREATE_NEW, "pcs ",        "[%d]/MemberTags/float16NumberType/[0]/TagSignature", index );
      v = oyjlTreeGetValueF( data, 0, "[%d]/lab/[0]/data", index );
      if(v)
      {
        char * pcs = NULL;
        int j;
        for(j = 0; j < 3; ++j)
        {
          v = oyjlTreeGetValueF( data, 0, "[%d]/lab/[0]/data/[%d]", index, j );
          d = OYJL_GET_DOUBLE(v);
          if(j)
            oyjlStringAdd( &pcs, 0,0, "\t" );
          oyjlStringAdd( &pcs, 0,0, "%f", d );
        }
        oyjlTreeSetStringF( nmcl, OYJL_CREATE_NEW, pcs,         "[%d]/MemberTags/float16NumberType/[0]/Data", index );
        if(pcs) free( pcs );
      }
      else
        oyjlTreeSetStringF( nmcl, OYJL_CREATE_NEW, "50 0 0",    "[%d]/MemberTags/float16NumberType/[0]/Data", index );
      oyjlTreeSetStringF( nmcl, OYJL_CREATE_NEW, "spec",        "[%d]/MemberTags/float16NumberType/[1]/TagSignature", index );
      oyjlTreeSetStringF( nmcl, OYJL_CREATE_NEW, spec,          "[%d]/MemberTags/float16NumberType/[1]/Data", index );
      if(spec) free( spec );
    }
    int level = 0;
    oyjlTreeToXml( icc, &level, &json );
  }
  else
    error = 1;

#ifdef USE_GETTEXT
  setlocale(LC_ALL,old_loc);
  free(old_loc);
#endif

  *text = json;
  return error;
}


void        oyTreeFilterColors( oyjl_val root, const char * pattern )
{
  oyjl_val data, v;
  int index, pixels, match;
  char * name;
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
    match = oyjlRegExpFind( name, pattern, NULL ) != NULL;
    if(pattern && !match )
    {
      sprintf( num, "[%d]", index );
      oyjlTreeClearValue(data, num);
    }
  }
  free(num);
}

