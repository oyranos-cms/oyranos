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
#include <math.h>
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

#include "ciexyz31_2.h"
#include "ciexyz64_1.h"
#include "bb_100K.h"
#include "spd_A_5.h"
#include "spd_D65_5.h"

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
double   oySpectrumGet( oyImage_s * spec, int column, int channel );
oyImage_s * oySpectrumFromCSV( char * text );

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
  fprintf( stderr, "      --illuminant D65\t%s\n", _("CIE D65 spectral power distribution"));
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
  fprintf( stderr, _("For more informations read the man page:"));
  fprintf( stderr, "\n");
  fprintf( stderr, "      man oyranos-profile-graph\n");
}

int main( int argc , char** argv )
{
  /* the functional switches */
  char * format = 0;
  char * output = 0;
  char * input = NULL;
  oyImage_s * spectra = NULL;
  int spectral_channels = 0;
  int spectral_count = 0;
  double spectra_rect[4] = {1000000.,-1000000.,1000000.,-1000000.}; /* x_min,x_max, y_min,y_max */
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
    spectra = oySpectrumFromCSV( text );
    if(spectra)
    {
      spectral_channels = oySpectrumGetParam( spectra, oySPECTRUM_CHANNELS );
      spectral_count = oySpectrumGetParam( spectra, oySPECTRUM_COLUMNS );
      for(i = 0; i < spectral_channels; ++i)
      {
        float x = oySpectrumGet( spectra, -1, i );
        if(x < spectra_rect[0]) spectra_rect[0] = x;
        if(x > spectra_rect[1]) spectra_rect[1] = x;
        for(j = 0; j < spectral_count; ++j)
        {
          double y = oySpectrumGet( spectra, j, i );
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
      fprintf( stderr, "channels: %d (%d) count: %d (%f %f - %f %f / %f %f)\n",
               (int)spectral_channels, (int)size, spectral_count,
               spectra_rect[0], spectra_rect[1], spectra_rect[2], spectra_rect[3], min_y, max_y );
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
      oySpectrumNormalise ( a, 1.0/max ); max = 1.0;
      drawIlluminant( cr,
                      a, 0,
                      xO, yO, width, height,
                      min_x, max_x,  min_y < 0 ? min_y : 0.0, max,
                      color ? COLOR_SPECTRAL : COLOR_GRAY, rgba,
                      flags, "A" );
      oyImage_Release( &a );
    }
    if(oyStringCaseCmp_(illuminant,"D65") == 0)
    {
      oyImage_s * a = oySpectrumCreateEmpty ( 300, 830, 5, 1 );
      float max = oySpectrumFillFromArrayF( a, spd_D65_5, 0 );
      oySpectrumNormalise ( a, 1.0/max ); max = 1.0;
      drawIlluminant( cr,
                      a, 0,
                      xO, yO, width, height,
                      min_x, max_x,  min_y < 0 ? min_y : 0.0, max,
                      color ? COLOR_SPECTRAL : COLOR_GRAY, rgba,
                      flags, "D65" );
      oyImage_Release( &a );
    }
  }
  if(spectra)
  {
    int j;
#define setColor(r_,g_,b_,a_) \
                 rgba[0] = r_; rgba[1] = g_; rgba[2] = b_; rgba[3] = a_;
    for(j = 0; j < spectral_count; ++j)
    {
      switch(j)
      {
        case 0: setColor( .8,.0,.0,1. ) break;
        case 1: setColor( .0,.8,.0,1. ) break;
        case 2: setColor( .0,.0,.8,1. ) break;
        case 3: setColor( .0,.8,.8,1. ) break;
        case 4: setColor( .8,.8,.0,1. ) break;
        case 5: setColor( .8,.0,.8,1. ) break;
        case 6: setColor( .5,.0,.0,1. ) break;
        case 7: setColor( .0,.5,.0,1. ) break;
        case 8: setColor( .0,.0,.5,1. ) break;
        case 9: setColor( .0,.5,.5,1. ) break;
        case 10: setColor( .5,.5,.0,1. ) break;
        case 11: setColor( .5,.0,.5,1. ) break;
        case 12: setColor( 1.,1.,1.,1. ) break;
        case 13: setColor( .8,.8,.8,1. ) break;
        case 14: setColor( .6,.6,.6,1. ) break;
        case 15: setColor( .4,.4,.4,1. ) break;
        case 16: setColor( .2,.2,.2,1. ) break;
        case 17: setColor( .0,.0,.0,1. ) break;
      }
      drawIlluminant( cr,
                      spectra, j,
                      xO, yO, width, height,
                      min_x, max_x, min_y < 0 ? min_y : 0.0, max_y,
                      spectral_count == 1 && color ? COLOR_SPECTRAL : COLOR_COLOR, rgba,
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
  double * dbl = (double*) oyImage_GetPointF(spec)( spec, column,0,-1, &is_allocated );

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

  if(verbose)
  fprintf( stderr, "drawing*sprectrum %d nm - %d nm %d nm precission %f", start, end, lambda, max_y );

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
      if(verbose) fprintf( stderr, " x0: %g y0: %g/%g\n", x0, y0, y1 );
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

  if(verbose)
  fprintf( stderr, " (%f) %d %s\n", max, channels, id );

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

int oyCSVglines( char * text )
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
float oyCSVparseDouble( char * text )
{
  double v = -1;
  if(text)
  {
    int r = oyStringToDouble( text, &v );
    if(r == 0) return v;
  }
  return v;
}
int oyCSVgetColumns( char * text )
{
  int len = oyStrlen_(text), i;
  int pixels = 0;
  for(i = 0; i < len; ++i)
  {
    char c = text[i];
    int is_line_break = 0;
    if(((int)'0' <= (int)c &&
          (int)c <= (int)'9') ||
       c == '.' ||
       c == '-' ||
       c == 'e' ||
       c == 'E' ||
       c == ' '
      )
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
oyImage_s * oySpectrumFromCSV   ( char * text )
{
  oyImage_s * spec = NULL;
  int pixels = 0,
      lines = 0;
  float nm, start, end, lambda;
  if(!text) return spec;
  pixels = oyCSVgetColumns( text );
  lines = oyCSVglines( text );
  if(verbose)
    fprintf( stderr, "lines: %d\n", lines );

  if(pixels >= 1)
  {
    int is_allocated = 0;
    double * dbl;;
    int i,index;
    spec = oySpectrumCreateEmpty( 1, lines, 1, pixels );
    for(i = 0; i < lines; ++i)
    {
      char * endl = strchr( text, '\n' );
      nm = oyCSVparseDouble(text);
      if(i == 0) start = nm;
      else if(i == 1) lambda = nm - start;
      else if(i == lines-1) end = nm;
      if(verbose > 1)
        fprintf( stderr, "%d lamda: %f ", i, nm );
      for(index = 0; index < pixels; ++index)
      {
        is_allocated = 0;
        dbl = (double*) oyImage_GetPointF(spec)( spec, index,0,-1, &is_allocated );
        text = strchr( text, ',' );
        if(text && text < endl && text[0] != '\r' && text[1] != '\r')
          dbl[i] = oyCSVparseDouble(++text);
        else
          dbl[i] = NAN;
        if(verbose > 1)
          fprintf( stderr, "%d: %f ", i, dbl[i] );
      }
      if(text)
      {
        text = strchr( text, '\n' );
        if(text) ++text;
        if(verbose > 1)
          fprintf( stderr, "\n" );
      }
    }
    oySpectrumSetRange( spec, start, end, lambda );
  }

  return spec;
}
