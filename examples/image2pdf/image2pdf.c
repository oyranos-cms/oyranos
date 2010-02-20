/** @file image2pdf.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2008  Kai-Uwe Behrmann
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
 *  while keeping attention to user colour settings.
 *  Beside Cairo, lcms and Oyranos you need DCraw during runtime.
 *  
 *  Compile: cc -pedantic -Wall -g `oyranos-config --cflags` `oyranos-config --ld_x_flags` `pkg-config --cflags cairo` `pkg-config --libs cairo` `pkg-config --libs lcms` image2pdf.c -o image2pdf
 */

# include <stddef.h>

#define __USE_POSIX2 1
#include <stdio.h>                /* popen() */
FILE *popen ( const char *__command, const char *__modes);
#include <math.h>
#include <icc34.h>
#include <lcms.h>                 /* littleCMS – typical CMM on Linux */
#include <oyranos_alpha.h>        /* Oyranos headers */
#include <cairo.h>                /* Cairo headers */
#include <cairo-pdf.h>

#define MIN(a,b) ((a<b)?(a):(b))
#define MAX(a,b) ((a>b)?(a):(b))
double       readShort(FILE * fp);
char *       createProfile           ( float               gamma,
                                       float               rx,
                                       float               ry,
                                       float               gx,
                                       float               gy,
                                       float               bx,
                                       float               by,
                                       const char        * name,
                                       size_t            * size );

int main (int argc, char ** argv)
{
  int result = 0;
  FILE * fp = 0;
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
      depth=255,                 /* the used ppm levels of gray */
      to_moni = 0,
      do_proof = 0,
      proof_intent = 0,
      rendering_intent = 0,
      bpc = 0;
  size_t size = 0;
  char * command,
       * info[10],
       * image_profile_name,
         c,
       * ptr,
       * data, * profile_name;
  unsigned char * image_data = 0,
                  rgba[4] = {127,127,127,255};
  const char * dcraw_icc_space=0;
  cmsHPROFILE input, proof, editing, monitor, print;
  cmsHTRANSFORM to_output = 0;
  oyConfig_s * device = 0;
  oyProfile_s * prof = 0;

  for( i = 0; i < 10; ++i )
    info[i] = malloc( 2048 );
  command = malloc( 2048 );

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
    printf("  To obtain a proofed monitor preview (test_proof.png):\n");
    printf("    image2pdf --monitor --proof imageA.raw imageB.raw\n");
    return 1;
  }

  o = 1;
  if(strcmp(argv[o],"--monitor") == 0 || strcmp(argv[o],"-m") == 0)
  {
    ++o;
    to_moni = 1;
    resolution = 96;
  }

  /* A advanced application provides the means to let the user decide, whether
   * she/he wants proofing on or off according to Oyranos settings.
   */
  if(strcmp(argv[o],"--proof") == 0 || strcmp(argv[o],"-p") == 0)
  {
    if(to_moni)
    {
      ++o;
      do_proof = 1;
    } else {
      printf("The --proof option is only allowed after --monitor\n");
    }
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

  /*  The monitor profile is located in the Xserver. For details see:
   *  http://www.freedesktop.org/wiki/Specifications/icc_profiles_in_x_spec
   */
  error = oyDeviceGet( OY_TYPE_STD, "monitor", 0, 0,
                       &device );
  error = oyDeviceGetProfile( device, &prof );
  data = oyProfile_GetMem( prof, &size, 0, malloc );

  if(!size || !data)
  {
    profile_name = oyGetDefaultProfileName( oyASSUMED_WEB, malloc );
    data = oyGetProfileBlock( profile_name, &size, malloc );
  }
  monitor = cmsOpenProfileFromMem( data, size );
  printf( "monitor:  %d\n", (int)size );
  free( data ); size = 0;

  /*  The editing profile can be obtained by Oyranos.
   *  It could be used as blending colour space.
   */
  profile_name = oyGetDefaultProfileName( oyEDITING_RGB, malloc );
  data = oyGetProfileBlock( profile_name, &size, malloc );
  editing = cmsOpenProfileFromMem( data, size );
  printf("editing:  %s\n", profile_name);
  free( data ); free( profile_name ); size = 0;

  /*  The output profile is equal to sRGB, as output profiles are curently not
   *  supported in Cairo. 
   */
  profile_name = oyGetDefaultProfileName( oyASSUMED_WEB, malloc );
  data = oyGetProfileBlock( profile_name, &size, malloc );
  print = cmsOpenProfileFromMem( data, size );
  printf("print:    %s\n", profile_name);
  free( data ); free( profile_name ); size = 0;

  /*  The printer profile is not easily available on Linux.
   *  Better maintain a own database for profile to queue profile assignment.
   *  We use here just the Oyranos proofing profile.
   */
  profile_name = oyGetDefaultProfileName( oyPROFILE_PROOF, malloc );
  data = oyGetProfileBlock( profile_name, &size, malloc );
  proof = cmsOpenProfileFromMem( data, size );
  printf("proofing: %s\n", profile_name);
  free( data ); free( profile_name ); size = 0;

  /* collect some more parameters */
  rendering_intent = oyGetBehaviour( oyBEHAVIOUR_RENDERING_INTENT );
  bpc = oyGetBehaviour( oyBEHAVIOUR_RENDERING_BPC );
  proof_intent = oyGetBehaviour( oyBEHAVIOUR_RENDERING_INTENT_PROOF ) ?
                         INTENT_ABSOLUTE_COLORIMETRIC :
                         INTENT_RELATIVE_COLORIMETRIC ;
  printf("intent:   %d, bpc: %d, proofing intent %d\n",
          rendering_intent, bpc, proof_intent);

  cr = cairo_create( surface );

  cairo_set_source_rgba( cr, rgba[0]/255., rgba[1]/255., rgba[2]/255., 1.0 );
  cairo_rectangle( cr, 0, 0, pixel_w, pixel_h );
  cairo_fill( cr );

  for ( i=0; i < argc-o; ++i )
  {
    const char * filename = argv[i+o];

    dcraw_icc_space = 0;

    /* obtain manufacturer and camera model from dcraw */
    sprintf (command, "PATH=.:$PATH ; dcraw -i '%s'\n", filename );
    fp = popen( command, "rb" );
    fscanf( fp, "%s is a ", info[0]);
    fscanf( fp, "%s ", info[0] );
    fgets( info[1], 2048, fp );
    fclose( fp );

    ptr = strstr( info[1], "image." );
    if(ptr)
    {
      ptr--;
      *ptr = 0;
    }

    printf(" %s: Manufacturer: \"%s\" Model: \"%s\"\n", filename,
             info[0], info[1]);


    /*  We can follow a hierarchie of profile sources to obtain a best match
     *  for our given device.
     *  First of all its important to obtain a useful description of the device.
     *  DCraw provides this, and we write into the info[0] and info[1] strings.
     *  The following hierarchy is numbered step by step:
     */

    /*  1. Embedded profile: We do not know about how to get from DCraw.
     *  2. Exif profile infos: DCraw does not provide as it would make not much
     *     sense. We would have to search for a external exif parser.
     *  3. Device database for a ICC profile: which fits to info[0] + info[1].
     *     We do not implement this here.
     */
    image_profile_name = 0;

    if(!dcraw_icc_space && image_profile_name)
      dcraw_icc_space = "-o 0"; /* raw */

    /*  4. Popup? Does the user want some vote, as there is no profile found 
     *     so far? We can ask the Oyranos CMS and decide upon user settings. */
    if(!dcraw_icc_space)
    switch( oyGetBehaviour (oyBEHAVIOUR_ACTION_UNTAGGED_ASSIGN ))
    { case 0: /* Ignore CM and profiles. Or, in case you cant, say here sRGB. */
              dcraw_icc_space = "-o 1"; /* dcraw linear matrix to gamma sRGB */
              profile_name = oyGetDefaultProfileName( oyASSUMED_WEB, malloc );
              data = oyGetProfileBlock( profile_name, &size, malloc );
              input = cmsOpenProfileFromMem( data, size );
              free( data ); free( profile_name );
              break;
      case 1: /* 4a. Use the Oyranos standard assumed input Rgb profile.
               *     This does not apply, as the Cairo output is untagged.
                     Thus sRGB is the only option.
               */
      case 2: /* 4b. let the user select a input profile */
            /*dcraw_icc_space = "-o 0";*/ /* linear raw RGB,
                     For simplicity here is no interactive mode provided. */
              ;
    }

    /* 4a. Oyranos assumed Rgb profile: our last possibility. */
    if(!dcraw_icc_space)
    {
      /*  Tell DCraw to do the conversion with the build in matrices.
       */
      dcraw_icc_space = "-o 2"; /* dcraw linear matrix to Adobe Rgb */

      data = createProfile( 1.0, 0.64, 0.33, 0.21, 0.71, 0.15, 0.06,
                            "linearAdobeRGB1998", &size );
      input = cmsOpenProfileFromMem( data, size );
      free( data );
    }

    /* build the colour context with littleCMS */
    to_output = cmsCreateProofingTransform( input, TYPE_BGRA_8,
                            to_moni ? monitor : print,
                         /* Cairo uses a Blue Green Red Alpha channel layout */
                            TYPE_BGRA_8,
                            proof,
                            rendering_intent, proof_intent,
                            do_proof ? 
                              cmsFLAGS_SOFTPROOFING : 0 |
                             (bpc && rendering_intent == 1) ?
                               cmsFLAGS_WHITEBLACKCOMPENSATION : 0 );


    /* decode with camera white balance and pipe half size, 16-bit, be verbose*/
    sprintf (command, "PATH=.:$PATH ; dcraw -w -c -h -4 -v %s '%s'\n",
                      dcraw_icc_space, filename );

    fp = popen (command, "rb");
    fscanf (fp, "P6 %d %d %d%c", &w, &h, &depth, &c);

    /* create a Cairo image */
    image_surf = cairo_image_surface_create( CAIRO_FORMAT_ARGB32, w, h );
    status = cairo_surface_status( image_surf ); if(status) return 1;

    /* write our dcraw stream on the Cairo image */
    image_data = cairo_image_surface_get_data( image_surf );
    size = w*h;
    /* Cairo uses a Blue Green Red Alpha channel layout */
    for(j = 0; j < size; ++j)
    {
      if(depth == 255)
      {
        image_data[j*4+2] = fgetc( fp );
        image_data[j*4+1] = fgetc( fp );
        image_data[j*4+0] = fgetc( fp );
      } else { /* assuming 16-bit */
        image_data[j*4+2] = readShort( fp )*255.;
        image_data[j*4+1] = readShort( fp )*255.;
        image_data[j*4+0] = readShort( fp )*255.;
      }
      image_data[j*4+3] = 255;

      if(feof(fp))
        printf("at: %d\n", j);
    }

    /* With the no ICC limitation in Cairo (as of v1.8), it is useful to convert
     * to the output colour right here and avoid the editing space completely.
     */
    cmsDoTransform( to_output, image_data, image_data, size );

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
    cmsCloseProfile( input );
    cairo_surface_destroy( image_surf );
    fclose( fp );
  }

  /* flush data to disk */
  if(do_proof)
    cairo_surface_write_to_png( surface, "test_proof.png" );
  else if(to_moni)
    cairo_surface_write_to_png( surface, "test.png" );

  cairo_surface_finish( surface );

  /* clean */
  cairo_surface_destroy( surface );
  cmsDeleteTransform( to_output );
  cmsCloseProfile( monitor );
  cmsCloseProfile( proof );
  cmsCloseProfile( print );
  cmsCloseProfile( editing );

  return result;
}

int isBigEndian(void)
{
  int big = 0;
  char testc[2] = {0,0};
  uint16_t *testu = (uint16_t*)testc;
  *testu = 1;
  big = testc[1];
  return big;
}

double readShort(FILE * fp)
{
  uint16_t s;
  uint8_t * byte = (uint8_t*)&s;

  if(isBigEndian())
  {
    byte[0] = fgetc( fp );
    byte[1] = fgetc( fp );
  } else {
    byte[1] = fgetc( fp );
    byte[0] = fgetc( fp );
  }

  return s/65535.0;
}

char *       createProfile           ( float               gamma,
                                       float               rx,
                                       float               ry,
                                       float               gx,
                                       float               gy,
                                       float               bx,
                                       float               by,
                                       const char        * name,
                                       size_t            * size )
{
  cmsCIExyYTRIPLE p = {{0,0,0},{0,0,0},{0,0,0}};
  LPGAMMATABLE g[3];
  cmsHPROFILE lp;
  cmsCIExyY D65xyY = {0.31271, 0.32902, 1.0};
  char * data;

  p.Red.x = rx; p.Red.y = ry;
  p.Green.x = gx; p.Green.y = gy;
  p.Blue.x = bx; p.Blue.y = by;
  g[0] = g[1] = g[2] = cmsBuildGamma(1, gamma);
  lp = cmsCreateRGBProfile( &D65xyY, &p, g);
  cmsAddTag( lp, icSigProfileDescriptionTag,  (char*)name );
  cmsAddTag( lp, icSigCopyrightTag,
             (char*)"2008 Kai-Uwe Behrmann, use freely");

  *size = 0;
  _cmsSaveProfileToMem( lp, 0, size );
  data = malloc( *size );
  _cmsSaveProfileToMem( lp, data, size );
  cmsCloseProfile( lp );

  return data;
}

