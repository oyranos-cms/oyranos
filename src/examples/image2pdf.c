/** @file image2pdf.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  Copyright (C) 2008-2012  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    public Oyranos API's
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2008/09/26
 *
 *  A sample application on how to compose a set of camera raw files into a pdf,
 *  while keeping attention to user color settings.
 *  Beside Cairo and Oyranos you need DCraw during runtime.
 *  
 *  Compile: cc -pedantic -Wall -g `oyranos-config --cflags` `oyranos-config --ld_x_flags` `pkg-config --cflags --libs cairo` image2pdf.c -o image2pdf
 */

# include <stddef.h>

#define __USE_POSIX2 1
#include <stdio.h>                /* popen() */
FILE *popen ( const char *__command, const char *__modes);
#include <math.h>
#include <string.h>
#include <oyConversion_s.h>
#include <oyProfile_s.h>          /* Oyranos headers */
#include <oyranos_devices.h>
#include <cairo.h>                /* Cairo headers */
#include <cairo-pdf.h>

#define MIN(a,b) ((a<b)?(a):(b))
#define MAX(a,b) ((a>b)?(a):(b))

int main (int argc, char ** argv)
{
  int result = 0;
  int i,j,o, error;
  cairo_t * cr = 0;
  cairo_surface_t * surface = NULL, * image_surf = NULL;
  cairo_status_t status;
  double page_w = 210.0,         /* page width in mm */
         page_h = 297.0,
         resolution = 72.0,      /* Cairo PDF surface resolution */
         scale = 1.0,
         frame = 0;
  int pixel_w, pixel_h,          /* page size in pixel */
      x,y,w=0,h=0,               /* image dimensions */
      to_moni = 0;
  size_t size = 0;
  unsigned char * image_data = 0,
                  rgba[4] = {127,127,127,255};
  oyProfile_s * monitor, * print, * output;
  oyConversion_s * to_output = 0;
  oyConfig_s * device = 0;
  oyFilterNode_s * node;
  const char * reg;
  uint32_t icc_profile_flags;

  if(argc < 2)
  {
    printf("Merge some CamerRAW images into one output image and use\n");
    printf("Oyranos CMS settings to obtain the result.\n");
    printf("\n");
    printf("Usage of the image2pdf example application:");
    printf("  To obtain a PDF (test.pdf):\n");
    printf("    image2pdf imageA.raw imageB.raw\n");
    printf("  To obtain a monitor preview (test.png):\n");
    printf("    image2pdf --monitor imageA.raw imageB.raw\n");
    return 1;
  }

  o = 1;
  if(strcmp(argv[o],"--monitor") == 0 || strcmp(argv[o],"-m") == 0)
  {
    ++o;
    to_moni = 1;
    resolution = 96;
  }

  pixel_w = page_w / 25.4 * resolution;
  pixel_h = page_h / 25.4 * resolution;

  /* create a surface to place our images on */
  if(to_moni)
    surface = cairo_image_surface_create( CAIRO_FORMAT_ARGB32, pixel_w,pixel_h);
  else
    surface = cairo_pdf_surface_create( "test.pdf",
                                        pixel_w,
                                        pixel_h );

  status = cairo_surface_status( surface );
  if(status) return 1;

  /* select profiles matching actual capabilities */
  node = oyFilterNode_FromOptions( OY_CMM_STD, "//" OY_TYPE_STD "/icc_color", NULL, NULL );
  reg = oyFilterNode_GetRegistration( node );
  icc_profile_flags = oyICCProfileSelectionFlagsFromRegistration( reg );
  oyFilterNode_Release( &node );

  /*  The monitor profile is located in the Xserver. For details see:
   *  http://www.freedesktop.org/wiki/Specifications/icc_profiles_in_x_spec
   */
  error = oyDeviceGet( OY_TYPE_STD, "monitor", 0, 0,
                       &device );
  error = oyDeviceGetProfile( device, 0, &monitor );
  if(error > 0)
    fprintf(stderr, "oyDeviceGetProfile error: %d\n", error);
  
  printf("monitor:  %s\n", oyProfile_GetText( monitor, oyNAME_DESCRIPTION ) );

  /*  The output profile is equal to sRGB, as output profiles are curently not
   *  supported in Cairo. 
   */
  print = oyProfile_FromStd( oyASSUMED_WEB, icc_profile_flags, 0 );
  printf("print:    %s\n", oyProfile_GetText( print, oyNAME_DESCRIPTION ));

  cr = cairo_create( surface );

  cairo_set_source_rgba( cr, rgba[0]/255., rgba[1]/255., rgba[2]/255., 1.0 );
  cairo_rectangle( cr, 0, 0, pixel_w, pixel_h );
  cairo_fill( cr );

  for ( i=0; i < argc-o; ++i )
  {
    const char * filename = argv[i+o];
    oyOptions_s * options = NULL;

    oyImage_s * in = NULL, * out = NULL;
    error = oyImage_FromFile( filename, icc_profile_flags, &in, NULL );

    w = oyImage_GetWidth( in );
    h = oyImage_GetHeight( in );

    /* create a Cairo image */
    image_surf = cairo_image_surface_create( CAIRO_FORMAT_ARGB32, w, h );
    status = cairo_surface_status( image_surf ); if(status) return 1;

    /* write our dcraw stream on the Cairo image */
    image_data = cairo_image_surface_get_data( image_surf );
    size = w*h;

    /* build the color context */
    if( to_moni )
      output = oyProfile_Copy( monitor, 0 );
    else
      output = oyProfile_Copy( print, 0 );
    out   = oyImage_Create( w, h,
                         image_data,
                         oyChannels_m(oyProfile_GetChannelsCount(output)+1) |
                          oyDataType_m(oyUINT8),
                         output,
                         0 );

    /* create a processing graph to convert from one image to an other */
    to_output = oyConversion_CreateBasicPixels( in, out, options, 0 );

    /* tell Oyranos to apply defaults */
    oyConversion_Correct( to_output, "//" OY_TYPE_STD "/icc_color",
                          oyOPTIONATTRIBUTE_ADVANCED, 0 );

    /* transform colors */
    oyConversion_RunPixels( to_output, NULL );
    oyConversion_Release( &to_output );

    /*oyImage_WritePPM( out, "test_out.ppm", "out" );*/
    oyImage_Release( &in );
    oyImage_Release( &out );

    /* Cairo uses a Blue Green Red Alpha channel layout */
#pragma omp parallel for
    for(j = 0; j < size; ++j)
    {
      unsigned char t = image_data[j*4+2];
      image_data[j*4+2] = image_data[j*4+0];
      /*image_data[j*4+1] = 0;*/
      image_data[j*4+0] = t;
      image_data[j*4+3] = 255;
    }
    cairo_surface_mark_dirty( image_surf );

    /* place our images on a sheet */
    if(argc-o > 1)
    {
      /* place in contact sheed style */
      scale = (pixel_w - pixel_w/10.0)/4.0/(double)MAX(w,h);
      cairo_save( cr );
      x = i%4 * (pixel_w - pixel_w/20.0)/4.0 + pixel_w/30.0;
      y = i/4 * (pixel_w - pixel_w/20.0)/4.0
          + ((pixel_w - pixel_w/20.0)/4.0 - MIN(w,h)*scale)/2.0
          + pixel_w/30.0;
    } else {
      /* single image */
      scale = (pixel_w - pixel_w/10.0)/(double)MAX(w,h);
      x = pixel_w/20.0;
      y = ((pixel_w - pixel_w/20.0) - MIN(w,h)*scale)/2.0
          + pixel_w/20.0;
    }

    /* draw a frame around the image */
    frame = pixel_w/20.0 * scale;
    cairo_set_source_rgba( cr, .0, .0, .0, 1.0);
    cairo_set_line_width (cr, 1.);
    cairo_rectangle( cr, x - frame, y - frame,
                         w*scale + 2*frame, h*scale + 2*frame);
    cairo_stroke(cr);

    /* draw the image */
    cairo_translate( cr, x, y );
    cairo_scale( cr, scale, scale );
    cairo_set_source_surface( cr, image_surf, 0,0 );
    cairo_paint( cr );
    cairo_restore( cr );

    /* small clean */
    cairo_surface_destroy( image_surf );
  }

  if(to_moni)
    cairo_surface_write_to_png( surface, "test.png" );

  cairo_surface_finish( surface );

  /* clean */
  cairo_surface_destroy( surface );
  oyProfile_Release( &monitor );
  oyProfile_Release( &print );

  return result;
}


