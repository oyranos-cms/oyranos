/** @file oyranos_profile_graph.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  Copyright (C) 2012-2017  Kai-Uwe Behrmann
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

#define __USE_POSIX2 1
#include <stdio.h>                /* popen() */
#include <math.h>                 /* NAN */
#include <string.h>
#include <cairo.h>                /* Cairo headers */
#include <cairo-svg.h>            /* Cairo SVG headers */

#include "oyConversion_s.h"
#include "oyProfile_s.h"
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
#include "bb_100K.h"
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

void  printfHelp (int argc, char** argv)
{
  int i;
  fprintf( stderr, "\n");
  for(i = 0; i < argc; ++i)
    fprintf( stderr, "%s ", argv[i]);
  fprintf( stderr, "\n");
  fprintf( stderr, "%s v%d.%d.%d %s\n", argv[0],
                        OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C,
                                _("is a ICC color profile grapher"));
  fprintf( stderr, "%s:\n",               _("Usage"));
  fprintf( stderr, "  %s\n",              _("2D Graph from profiles:"));
  fprintf( stderr, "      %s [-o %s] [-x [-c]] [-s] [-vbowt] %s\n", argv[0],
                          _("FILE"),_("PROFILENAMES") );
  fprintf( stderr, "      -x \t%s\n",     _("use CIE*xyY *x*y plane for saturation line projection"));
  fprintf( stderr, "      \t-c\t%s\n",    _("omit white line of lambert light emitters"));
  fprintf( stderr, "      -s \t%s\n",     _("omit the spectral line"));
  fprintf( stderr, "      -d %s\t%s\n",   _("NUMBER"), _("specify incremental increase of the thickness of the graph lines"));
  fprintf( stderr, "\n");
  fprintf( stderr,  "  %s\n",             _("Standard Observer 1931 2° Graph:"));
  fprintf( stderr,  "      %s --standard-observer [-vbowtr]\n", argv[0]);
  fprintf( stderr, "      \t--no-color\t%s\n",    _("draw gray"));
  fprintf( stderr, "\n");
  fprintf( stderr,  "  %s\n",             _("1964 10° Observer Graph:"));
  fprintf( stderr,  "      %s --standard-observer-64 [-vbowtr]\n", argv[0]);
  fprintf( stderr, "      \t--no-color\t%s\n",    _("draw gray"));
  fprintf( stderr, "\n");
  fprintf( stderr,  "  %s\n",             _("Blackbody Radiator Spectrum Graph:"));
  fprintf( stderr,  "      %s --kelvin %s [-vbowtr]\n", argv[0], _("NUMBER"));
  fprintf( stderr, "      \t--no-color\t%s\n",    _("draw gray"));
  fprintf( stderr, "\n");
  fprintf( stderr,  "  %s\n",             _("Illuminant Spectrum Graph:"));
  fprintf( stderr,  "      %s --illuminant A|D65 [-vbowtr]\n", argv[0]);
  fprintf( stderr, "      --illuminant A\t%s\n",   _("CIE A spectral power distribution"));
  fprintf( stderr, "      --illuminant D50\t%s\n", _("CIE D50 spectral power distribution (computed)"));
  fprintf( stderr, "      --illuminant D65\t%s\n", _("CIE D65 spectral power distribution"));
  fprintf( stderr, "      --illuminant %s\t%s\n", _("NUMBER"), _("Kelvin derived CIE spectral power distribution"));
  fprintf( stderr, "      \t--no-color\t%s\n",    _("draw gray"));
  fprintf( stderr, "\n");
  fprintf( stderr,  "  %s\n",             _("CSV Spectrum Graph:"));
  fprintf( stderr,  "      %s --input %s [-vbowtr]\n", argv[0], _("FILE"));
  fprintf( stderr, "      \t--no-color\t%s\n",    _("draw gray"));
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",              _("General options:"));
  fprintf( stderr, "      -v \t%s\n",     _("verbose"));
  fprintf( stderr, "      -w %s\t%s\n",   _("NUMBER"), _("specify output image width in pixel"));
  fprintf( stderr, "      -o %s\t%s\n",   _("FILE"),   _("specify output file name, default is output.png"));
  fprintf( stderr, "      -f %s\t%s\n",   _("FORMAT"),   _("specify output file format png or svg, default is png"));
  fprintf( stderr, "      -b \t%s\n",     _("omit border"));
  fprintf( stderr, "      -t %s\t%s\n",   _("NUMBER"), _("specify increase of the thickness of the graph lines"));
  fprintf( stderr, "      -r \t%s\n",     _("draw grid"));
  fprintf( stderr, "      -2 \t%s\n",     _("select a ICC v2 profile"));
  fprintf( stderr, "      -4 \t%s\n",     _("select a ICC v4 profile"));
  fprintf( stderr, "\n");
  fprintf( stderr, _("For more information read the man page:"));
  fprintf( stderr, "\n");
  fprintf( stderr, "      man oyranos-profile-graph\n");
}

int main( int argc , char** argv )
{
  /* the functional switches */
  char * format = 0;
  char * output = 0;
  char * input = NULL;
  int spectral = 1;
  int blackbody = 1;
  double thickness = 1.0;
  double change_thickness = 0.7;
  int border = 1;
  int raster = 0;
  int standardobs = 0;
  int saturation = 1;
  double kelvin = 0.0;
  char * illuminant = 0;
  int color = 1;
  int flags = 0;

  double max_x,max_y,min_x,min_y;
  oyImage_s * spectra = NULL;
  oyF3 * spectra_XYZ = NULL;
  oyF3 white = {{0.0,0.0,0.0}};
  oyjl_val specT = NULL;
  char * string = NULL;
  int spectral_channels = 0;
  int spectral_count = 0;
  double spectra_rect[4] = {1000000.,-1000000.,1000000.,-1000000.}; /* x_min,x_max, y_min,y_max */

  const char * filename = 0;

  int i,j,
      o = 0;
  cairo_t * cr = 0;
  cairo_surface_t * surface = NULL;
  cairo_status_t status;
  double scale = 1.0,
         frame = 0;
  int pixel_w=128, pixel_h=128,  /* size in pixel */
      x,y,w=0,h=0;               /* image dimensions */
  oyProfile_s * p_xyz = oyProfile_FromStd(oyASSUMED_XYZ, flags, 0),
              * p_lab = oyProfile_FromStd(oyASSUMED_LAB, flags, 0),
              * proj = p_lab;
  double xs_xyz = 1.2,           /* scaling of CIE*xy graph */
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


  if(argc != 1)
  {
    int pos = 1;
    const char *wrong_arg = 0;
    while(pos < argc)
    {
      switch(argv[pos][0])
      {
        case '-':
            for(i = 1; i < (int)strlen(argv[pos]); ++i)
            switch (argv[pos][i])
            {
              case '2': flags |= OY_ICC_VERSION_2; break;
              case '4': flags |= OY_ICC_VERSION_4; break;
              case 'b': border = 0; break;
              case 'c': blackbody = 0; break;
              case 'd': i=0; OY_PARSE_FLOAT_ARG2( change_thickness, "d", -1000.0, 1000.0, .7 ); break;
              case 'f': OY_PARSE_STRING_ARG(format); break;
              case 'o': OY_PARSE_STRING_ARG(output); break;
              case 'r': raster = 1; break;
              case 'w': OY_PARSE_INT_ARG( pixel_w ); break;
              case 'v': if(verbose++) oy_debug += 1; break;
              case 'x': proj = p_xyz; break;
              case 's': spectral = 0; break;
              case 't': i=0; OY_PARSE_FLOAT_ARG2( thickness, "t", 0.001, 10.0 , 1.0 ); break;
              case 'h':
              case '-':
                        if(i == 1)
                        {
                             if(OY_IS_ARG("format"))
                        { OY_PARSE_STRING_ARG2(format, "format"); break; }
                        else if(OY_IS_ARG("output"))
                        { OY_PARSE_STRING_ARG2(output, "output"); break; }
                        else if(OY_IS_ARG("profile"))
                        { OY_PARSE_STRING_ARG2(filename, "profile"); break; }
                        else if(OY_IS_ARG("standard-observer"))
                        { blackbody = spectral = saturation = 0; standardobs = 1; i=100; break;}
                        else if(OY_IS_ARG("standard-observer-64"))
                        { blackbody = spectral = saturation = 0; standardobs = 2; i=100; break;}
                        else if(OY_IS_ARG("kelvin"))
                        { blackbody = spectral = saturation = 0;
                          OY_PARSE_FLOAT_ARG2(kelvin, "kelvin", 800.0,15000.0,5000.0); i=100; break;}
                        else if(OY_IS_ARG("illuminant"))
                        { blackbody = spectral = saturation = 0;
                          OY_PARSE_STRING_ARG2(illuminant, "illuminant"); break; }
                        else if(OY_IS_ARG("input"))
                        { blackbody = spectral = saturation = 0;
                          OY_PARSE_STRING_ARG2(input, "input"); break; }
                        else if(OY_IS_ARG("no-color"))
                        { color = 0; i=100; break;}
                        else if(OY_IS_ARG("verbose"))
                        { if(verbose++) oy_debug += 1; i=100; break;}
                        } OY_FALLTHROUGH
              default:
                        printfHelp(argc, argv);
                        exit (0);
                        break;
            }
            break;
        default: ++o; break;
      }
      if( wrong_arg )
      {
        fprintf( stderr, "%s %s\n", _("wrong argument to option:"), wrong_arg);
        printfHelp(argc, argv);
        exit(1);
      }
      ++pos;
    }
    if(oy_debug) fprintf( stderr,  "%s\n", argv[1] );
  }
  else
  {
                        printfHelp(argc, argv);
                        exit (0);
  }

  if(oy_debug)
    fprintf( stderr, "  Oyranos v%s\n",
                  oyNoEmptyName_m_(oyVersionString(1,0)));

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
      spectra = oySpectrumFromTree( specT );
      if(verbose && spectra) fprintf( stderr, "JSON parsed\n" );
      if(oy_debug) fprintf( stderr, "%s", oyStruct_GetText((oyStruct_s*)spectra, oyNAME_NAME, 0));
    }
    else
    if(data_format == oyNAME_JSON + 1) /* XML */
    {
      specT = oyTreeFromCxf( text );
      if(verbose)
        fprintf( stderr, "XML detected: %s\n", input );

      spectra = oySpectrumFromTree( specT );
      if(verbose && spectra) fprintf( stderr, "CxF parsed\n" );
      if(oy_debug) fprintf( stderr, "%s", oyStruct_GetText((oyStruct_s*)spectra, oyNAME_NAME, 0));
    }
    if(!spectra && text)
    {
      specT = oyTreeFromCsv( text );
      oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, input, "description" );
      spectra = oySpectrumFromTree( specT );
      if(verbose && spectra)
      if(verbose && spectra) fprintf( stderr, "CSV parsed\n" );
      if(oy_debug) fprintf( stderr, "%s", oyStruct_GetText((oyStruct_s*)spectra, oyNAME_NAME, 0));
    }
    if(!spectra && text)
    {
      specT = oyTreeFromCgats( text );
      spectra = oySpectrumFromTree( specT );
      if(verbose && spectra) fprintf( stderr, "CGATS parsed\n" );
      if(oy_debug) fprintf( stderr, "%s", oyStruct_GetText((oyStruct_s*)spectra, oyNAME_NAME, 0));
    }

    if(spectra)
    {
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
          if(verbose)
          {
            int lambda = oySpectrumGetParam( spectra, oySPECTRUM_LAMBDA );
            int start = oySpectrumGetParam( spectra, oySPECTRUM_START );
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
          oyXYZtoSRGB( rgb );
          if(j < 10)
          fprintf( stderr, "%d\t%f %f %f -> %f %f %f\n", j, spectra_XYZ[j].f[0]/white.f[0]*100.0, spectra_XYZ[j].f[1]/white.f[0]*100.0, spectra_XYZ[j].f[2]/white.f[0]*100.0,
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
    surface = cairo_svg_surface_create( output?output:"output.svg",
                                        (double)pixel_w, (double)pixel_h);
  else
  {
    if(oyStringCaseCmp_(format, "csv") == 0)
    {
      if(specT)
      {
        int level = 0;
        oyTreeToCsv( specT, &level, &string );
        oyjlWriteFile( output?output:"output.csv", string, strlen(string) );
      } else
        oyMessageFunc_p(oyMSG_ERROR,NULL,"no input tree found");
    }
    else if(oyStringCaseCmp_(format, "ncc") == 0)
    {
      if(specT)
      {
        int level = 0;
        oyjlTreeToJson( specT, &level, &string );
        oyjlWriteFile( output?output:"output.ncc", string, strlen(string) );
      } else
        oyMessageFunc_p(oyMSG_ERROR,NULL,"no input tree found");
    }
    else if(oyStringCaseCmp_(format, "ppm") == 0)
    {
      oySpectrumToPpm( spectra, input, output?output:"output.ppm" );
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
          oyjlWriteFile( output?output:"output.cgats", string, strlen(string) );
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

  if(spectral)
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
          cairo_move_to(cr, xToImage(Lab[1]/256.0+.5),
                            yToImage(Lab[2]/256.0+0.5));
        else
          cairo_line_to(cr, xToImage(Lab[1]/256.0+.5),
                            yToImage(Lab[2]/256.0+0.5));
      }
      cairo_close_path(cr);
    }
    cairo_stroke(cr);
  }

  if(blackbody && proj == p_xyz)
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
          cairo_move_to(cr, xToImage(Lab[1]/256.0+.5),
                            yToImage(Lab[2]/256.0+0.5));
        else
          cairo_line_to(cr, xToImage(Lab[1]/256.0+.5),
                            yToImage(Lab[2]/256.0+0.5));
      }
    }
    cairo_stroke(cr);
  }

  /* draw a frame around the image */
  frame = pixel_w/40.0;
  cairo_set_source_rgba( cr, .0, .0, .0, 1.0);
  cairo_set_line_width (cr, 0.7*thickness);
  if(border)
  cairo_rectangle( cr, x - frame, y - frame,
                         w*scale + 2*frame, h*scale + 2*frame);
  cairo_stroke(cr);
  if(border && proj == p_lab && saturation)
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

  if(saturation)
  {
    float t = thickness;
    cairo_set_line_width (cr, 1.*thickness);
    if(proj == p_lab && !spectral)
    {
      cairo_set_source_rgba( cr, 1., 1., 1., 1.0);
      cairo_move_to(cr, xToImage(.0), yToImage(.5));
      cairo_line_to(cr, xToImage(1.0), yToImage(.5));
      cairo_move_to(cr, xToImage(.5), yToImage(0));
      cairo_line_to(cr, xToImage(.5), yToImage(1.0));
      cairo_stroke(cr);
    }

    cairo_set_line_width (cr, 3.*t);
    for ( j=argc-o; j < argc; ++j )
    {
      const char * filename = argv[j];
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

      t = pow( change_thickness, j-(argc-o)) * thickness;
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


  /* default spectral range to draw */
  min_x = 300.;
  max_x = 780.;

  if(raster &&
     ( standardobs || illuminant || kelvin > 0.0 || spectra ) )
  {
    /* draw some coordinate system hints */
    cairo_set_line_width (cr, 0.35*thickness);
    cairo_set_source_rgba( cr, bg_rgba[0], bg_rgba[1], bg_rgba[2], bg_rgba[3]);
    /* 25 nm */
    for(i = 300; i < max_x; i += 25)
    {
      cairo_move_to(cr, xToImage(i), yToImage(min_y));
      cairo_line_to(cr, xToImage(i), yToImage(max_y));
    }
    cairo_stroke(cr);

    /* 100 nm */
    cairo_set_line_width (cr, 0.7*thickness);
    for(i = 300; i < max_x; i += 100)
    {
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
                      color ? COLOR_COLOR : COLOR_GRAY, rgba, \
                      flags, id ); \
    }
  cairo_set_line_width (cr, 3.*thickness);
  if(standardobs == 1)
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
  if(standardobs == 2)
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
                      color ? COLOR_SPECTRAL : COLOR_GRAY, rgba,
                      flags, "kelvin" );
    oyImage_Release( &a );
  }
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
                      color ? COLOR_SPECTRAL : COLOR_GRAY, rgba,
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
                      color ? COLOR_SPECTRAL : COLOR_GRAY, rgba,
                      flags, "S1" );
      drawIlluminant( cr, a, 1, xO, yO, width, height,
                      min_x, max_x,  min_y < 0 ? min_y : 0.0, max,
                      color ? COLOR_SPECTRAL : COLOR_GRAY, rgba,
                      flags, "S2" );
      drawIlluminant( cr, a, 2, xO, yO, width, height,
                      min_x, max_x,  min_y < 0 ? min_y : 0.0, max,
                      color ? COLOR_SPECTRAL : COLOR_GRAY, rgba,
                      flags, "S3" );
      oyImage_Release( &a );
    } else
    if(oyStringCaseCmp_(illuminant,"D50") == 0)
    {
      oyImage_s * a = oySpectrumCreateEmpty ( 300, 830, 5, 1 );
      float * spd_5 = NULL;
      double kelvin = 5000;
      int error = oyDXXCIEfromTemperature( kelvin, &spd_5 );
      if(error)
        oyMessageFunc_p(oyMSG_ERROR,(oyStruct_s*)a,"not CIE illuminant for %g", kelvin);
      float max = oySpectrumFillFromArrayF( a, spd_5, 0 );
      if(verbose) fprintf( stderr, "max: %f\n", max );
      oySpectrumNormalise ( a, 1.0/max ); max = 1.0;
      drawIlluminant( cr, a, 0, xO, yO, width, height,
                      min_x, max_x,  min_y < 0 ? min_y : 0.0, max,
                      color ? COLOR_SPECTRAL : COLOR_GRAY, rgba,
                      flags, "D50" );
      oyImage_Release( &a );
    } else
    if(oyStringCaseCmp_(illuminant,"D65") == 0)
    {
      oyImage_s * a = oySpectrumCreateEmpty ( 300, 830, 5, 1 );
      float max = oySpectrumFillFromArrayF( a, spd_D65_5, 0 );
      if(verbose) fprintf( stderr, "max: %f\n", max );
      oySpectrumNormalise ( a, 1.0/max ); max = 1.0;
      drawIlluminant( cr,
                      a, 0,
                      xO, yO, width, height,
                      min_x, max_x,  min_y < 0 ? min_y : 0.0, max,
                      color ? COLOR_SPECTRAL : COLOR_GRAY, rgba,
                      flags, "D65" );
      oyImage_Release( &a );
    } else
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
                      color ? COLOR_SPECTRAL : COLOR_GRAY, rgba,
                      flags, "D50" );
      oyImage_Release( &a );
    }
    }
  }
  if(spectra)
  {
    int j;
#define setColor(r_,g_,b_,a_) \
                 rgba[0] = r_; rgba[1] = g_; rgba[2] = b_; rgba[3] = a_;
    for(j = 0; j < spectral_count; ++j)
    {
      double rgb[4] = { spectra_XYZ[j].f[0]/white.f[0], spectra_XYZ[j].f[1]/white.f[0], spectra_XYZ[j].f[2]/white.f[0], 1.0 };
      oyXYZtoSRGB( rgb );
      drawIlluminant( cr,
                      spectra, j,
                      xO, yO, width, height,
                      min_x, max_x, min_y < 0 ? min_y : 0.0, max_y,
                      spectral_count == 1 && color ? COLOR_SPECTRAL : COLOR_COLOR, rgb,
                      flags, input );
    }
    oyImage_Release( &spectra );

    min_x=min_y=0.0;
    max_x=max_y=1.0;
  }

  cairo_restore( cr );

  if(format == NULL || oyStringCaseCmp_(format, "png") == 0)
    status = cairo_surface_write_to_png( surface, output?output:"output.png" );
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
  fprintf( stderr, "drawing*sprectrum %d %d nm - %d nm %d nm precission %f", index, start, end, lambda, max_y );

  cairo_move_to(cr, xToImage( oySpectrumGetParam( spec, oySPECTRUM_START )), yToImage( oySpectrumGet(spec, index, 0)));

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
          oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, d, "collection/[0]/colors/[%d]/spectral/[%d]", index, i );
        }
      }

      if(text)
      {
        text = strchr( text, '\n' );
        if(text) ++text;

        if(verbose > 1) fprintf( stderr, "\n" );
      }
    }
    oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, start, "collection/[0]/spectral/startNM" );
    oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, end, "collection/[0]/spectral/endNM" );
    oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, lambda, "collection/[0]/spectral/lambda" );
    oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, (end-start+lambda)/lambda, "collection/[0]/spectral/steps" );
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

oyjl_val    oyTreeFromCxf( const char * text )
{
  char error_buffer[128];
  size_t error_buffer_size = 128;
  oyjl_val root = oyjlTreeParseXml( text, 0, error_buffer, error_buffer_size ),
           node = oyjlTreeGetValue( root, 0, "cc:CxF/cc:FileInformation/cc:Description" ),
           collection = oyjlTreeGetValue( root, 0, "cc:CxF/cc:Resources/cc:ObjectCollection/cc:Object" ),
           specT = oyjlTreeNew("");
  int count = oyjlValueCount( collection ), i;
  oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, "ncc1", "type" );
  oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, "Named Color Collection v1", "comment" );
  char * v = oyjlValueText( node, 0 );
  if(v) oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, v, "description" ); /* DESCRIPTOR */
  node = oyjlTreeGetValue( root, 0, "cc:CxF/cc:FileInformation/cc:CreationDate" );
  v = oyjlValueText( node, 0 );
  if(v) oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, v, "date" ); /*  */
  node = oyjlTreeGetValue( root, 0, "cc:CxF/cc:FileInformation/cc:Creator" );
  v = oyjlValueText( node, 0 );
  if(v) oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, v, "creator" ); /* ORIGINATOR */
  char * startWL = oyjlValueText( oyjlTreeGetValue(root, 0, "cc:CxF/cc:Resources/cc:ColorSpecificationCollection/cc:ColorSpecification/cc:MeasurementSpec/cc:WavelengthRange/@StartWL"), 0 );
  long startNM = 0, lambda = 0, endNM = 0, n_max = 0;
  char * increment = oyjlValueText( oyjlTreeGetValue(root, 0, "cc:CxF/cc:Resources/cc:ColorSpecificationCollection/cc:ColorSpecification/cc:MeasurementSpec/cc:WavelengthRange/@Increment"), 0 );
  oyjlStringToLong(startWL, &startNM);
  oyjlStringToLong(increment, &lambda);
  oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, startNM, "collection/[0]/spectral/startNM" );
  oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, lambda, "collection/[0]/spectral/lambda" );

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
    if(oyjlStringsToDoubles( reflSpec_text, ' ', &n, 0, &list ))
      fprintf( stderr, "ERROR parsing: %d %s ", i, name );

    oyjlTreeSetStringF( specT, OYJL_CREATE_NEW, name, "collection/[0]/colors/[%d]/name", i );

    oyjlStringToLong(startWl, &startNm);
    //fprintf( stderr, "  %d: %s - %ld-%ld ", i, name, startNM, startNM+n*lambda );
    for(j = 0; j < n; ++j)
      oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, list[j], "collection/[0]/colors/[%d]/spectral/[%d]", i, j );
    if(n_max < j-1) n_max = j-1;
  }
  endNM = startNM + lambda * n_max;
  oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, endNM, "collection/[0]/spectral/endNM" );
  oyjlTreeSetDoubleF( specT, OYJL_CREATE_NEW, (endNM-startNM+lambda)/lambda, "collection/[0]/spectral/steps" );

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

  v = oyjlTreeGetValue( root, 0, "collection/[0]/spectral/startNM" );
  if(!v) { oyMessageFunc_p(oyMSG_ERROR,NULL,"startNM missed"); return NULL; }
  else *startNM = v->u.number.d;

  v = oyjlTreeGetValue( root, 0, "collection/[0]/spectral/endNM" );
  if(!v) { oyMessageFunc_p(oyMSG_ERROR,NULL,"endNM missed"); return NULL; }
  else *endNM = v->u.number.d;

  v = oyjlTreeGetValue( root, 0, "collection/[0]/spectral/lambda" );
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
        v = oyjlTreeGetValueF( data, 0, "[%d]/spectral/[%d]", index, i );
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
  oyjl_str t;
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
      oyjlStringAdd( &tmp, 0,0, "CREATED \"%s\"\n", creation_date?creation_date:"" );
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

  if(data && pixels >= 1)
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
        v = oyjlTreeGetValueF( data, 0, "[%d]/spectral/[%d]", index, i );
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
  oyjl_str t;
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
  {
    t = oyjlStrNew(0,0,0);
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

        v = oyjlTreeGetValueF( data, 0, "[%d]/spectral/[%d]", index, i );
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

  *text = oyjlStrPull( t );
  oyjlStrRelease( &t );
  return 0;
}

