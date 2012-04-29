/** @file oyranos_convert.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2012 (C) Kai-Uwe Behrmann
 *
 *  @brief    ICC conversion - on the command line
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2012/02/19
 *
 *  The program uses ICC profiles to perform colour transforms.
 *
 *  cc -Wall -g oyranos_convert.c -o oyranos-icc `pkg-config --libs --cflags oyranos` -I./ -I../build_11.4 -I API_generated/ -I oforms/
 */


#include "oyranos.h"
#include "oyranos_alpha.h"
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

#include "oyranos_forms.h"
#ifdef USE_LCMS
#include "lcms2.h"
#endif

void* oyAllocFunc(size_t size) {return malloc (size);}
int    oyImage_FromFile              ( const char        * file_name,
                                       oyImage_s        ** image,
                                       oyObject_s          obj );
oyConversion_s * oyConversion_FromImage (
                                       oyImage_s         * image_in,
                                       const char        * module,
                                       oyOptions_s       * module_options,
                                       oyProfile_s       * output_profile,
                                       oyDATATYPE_e        buf_type_out,
                                       uint32_t            flags,
                                       oyObject_s          obj );

void  printfHelp (int argc, char** argv)
{
  char * version = oyVersionString(1,0);
  char * id = oyVersionString(2,0);
  char * cfg_date =  oyVersionString(3,0);
  char * devel_time = oyVersionString(4,0);

  fprintf( stderr, "\n");
  fprintf( stderr, "oyranos-icc %s\n",
                                _("is a ICC color conversion tool"));
  fprintf( stderr, "  Oyranos v%s config: %s devel period: %s\n",
                  oyNoEmptyName_m_(version),
                  oyNoEmptyName_m_(cfg_date), oyNoEmptyName_m_(devel_time) );
  if(id)
  fprintf( stderr, "  Oyranos git id %s\n", oyNoEmptyName_m_(id) );
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",
                                           _("Hint: search paths are influenced by the XDG_CONFIG_HOME shell variable."));
  fprintf( stderr, "\n");
  fprintf( stderr, "%s\n",                 _("Usage"));
  fprintf( stderr, "  %s\n",               _("Convert Image:"));
  fprintf( stderr, "      %s -p PROFILE [-o FILENAME] [-n MODULE] -i FILENAME\n", argv[0]);
  fprintf( stderr, "      -p PROFILE   %s\n", _("Output Color Space"));
  fprintf( stderr, "      -s PROFILE   %s\n", _("Simulation/Proof Color Space"));
  fprintf( stderr, "      -e PROFILE   %s\n", _("Effect abtract Color Space"));
  fprintf( stderr, "      -o FILENAME  %s\n", _("write to file, currently only PPM format"));
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("Extract ICC profile:"));
  fprintf( stderr, "      %s -f icc [-o FILENAME] [-n MODULE] -i FILENAME\n", argv[0]);
  fprintf( stderr, "      -o FILENAME  %s\n", _("write to file"));
  fprintf( stderr, "  %s\n",               _("Print a help text:"));
  fprintf( stderr, "      %s -h [-n MODULE] [-d]\n",        argv[0]);
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s\n",               _("General options:"));
  fprintf( stderr, "      -v           %s\n",  _("verbose"));
  fprintf( stderr, "      -n MODULE    %s\n",  _("module name"));
  fprintf( stderr, "      -d           %s\n",  _("enable simple defaults"));
  fprintf( stderr, "\n");
  fprintf( stderr, "  %s:\n",               _("Example"));
  fprintf( stderr, "    %s:\n",             _("Get ICC profile"));
  fprintf( stderr, "      oyranos-icc -f icc -i image.png | iccexamin -g -i\n");
  fprintf( stderr, "\n");
  fprintf( stderr, "    %s:\n",             _("Convert image to ICC Color Space"));
  fprintf( stderr, "      oyranos-icc -i image.png -n lcm2 -p Lab.icc -o image.ppm\n");
  fprintf( stderr, "\n");
  fprintf( stderr, "\n");

  if(version) oyDeAllocateFunc_(version);
  if(id) oyDeAllocateFunc_(id);
  if(cfg_date) oyDeAllocateFunc_(cfg_date);
  if(devel_time) oyDeAllocateFunc_(devel_time);
}


int main( int argc , char** argv )
{
  int error = 0;
  char * format = 0;
  char * output = 0;
  char * input = 0;
  char * node_name = 0;
  int help = 0;
  int verbose = 0;
  int icc_defaults_simple = 0;
  char * output_profile = 0;
  char * simulation_profile = 0;
  char * effect_profile = 0;
  oyProfiles_s * proofing = oyProfiles_New(0),
               * effects = oyProfiles_New(0);
  oyProfile_s * p = 0;
  oyOptions_s * module_options = 0;

  int output_model = 0;
  const char * input_xml_file = 0;
  const char * output_model_file = 0,
             * result_xml = 0;
  char * text = 0, * t = 0;
  const char * opt_names = 0;
  oyFormsArgs_s * forms_args = oyFormsArgs_New( 0 );
  const char * data = 0, * ct = 0;
  char ** other_args = 0;
  int other_args_n = 0;
  int i;
  int print = 1;
  oyOptions_s * opts = 0;
  oyOption_s * o = 0;
  oyImage_s * image = 0;

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif
  oyExportStart_(EXPORT_CHECK_NO);

  if(argc >= 2)
  {
    int pos = 1, i;
    char *wrong_arg = 0;
    DBG_PROG1_S("argc: %d\n", argc);
    while(pos < argc)
    {
      switch(argv[pos][0])
      {
        case '-':
            for(i = 1; pos < argc && i < strlen(argv[pos]); ++i)
            switch (argv[pos][i])
            {
              case 'd': icc_defaults_simple = 1; break;
              case 'f': OY_PARSE_STRING_ARG(format); break;
              case 'o': OY_PARSE_STRING_ARG(output); break;
              case 'p': OY_PARSE_STRING_ARG(output_profile); break;
              case 's': OY_PARSE_STRING_ARG(simulation_profile);
                        p = oyProfile_FromFile( simulation_profile, 0,0 );
                        oyProfiles_MoveIn( proofing, &p, -1 );
                        break;
              case 'e': OY_PARSE_STRING_ARG(effect_profile);
                        p = oyProfile_FromFile( effect_profile, 0,0 );
                        oyProfiles_MoveIn( effects, &p, -1 );
                        break;
              case 'i': OY_PARSE_STRING_ARG(input); break;
              case 'n': OY_PARSE_STRING_ARG(node_name); break;
              case 'v': if(verbose) oy_debug += 1; verbose = 1; break;
              case 'h': help = 1; break;
              case '-':
                        if(OY_IS_ARG("help"))
                        { help = 1; i=100; break; }
                        else if(strcmp(&argv[pos][2],"verbose") == 0)
                        { oy_debug += 1; i=100; break;
                        } else if(argv[pos][2])
                        {
                          STRING_ADD( t, &argv[pos][2] );
                          text = oyStrrchr_(t, '=');
                          /* get the key only */
                          if(text)
                            text[0] = 0;
                          oyStringListAddStaticString_( &other_args,&other_args_n,
                                                        t,
                                            oyAllocateFunc_,oyDeAllocateFunc_ );
                          if(text)
                            oyStringListAddStaticString_(
                                            &other_args,&other_args_n,
                                            oyStrrchr_(&argv[pos][2], '=') + 1,
                                            oyAllocateFunc_,oyDeAllocateFunc_ );
                          else {
                            if(argv[pos+1])
                            {
                              oyStringListAddStaticString_( &other_args,
                                                            &other_args_n,
                                                            argv[pos+1],
                                            oyAllocateFunc_,oyDeAllocateFunc_ );
                              ++pos;
                            } else wrong_arg = argv[pos];
                          }
                          if(t) oyDeAllocateFunc_( t );
                          t = 0;
                          i=100; break;
                        } else
                        {
                          wrong_arg = argv[pos];
                        }
                        break;
              default:
                        printfHelp(argc, argv);
                        exit (0);
                        break;
            }
            break;
        default:
                        printfHelp(argc, argv);
                        exit (0);
                        break;
      }
      if( wrong_arg )
      {
       fprintf(stderr, "%s %s\n", _("wrong argument to option:"), wrong_arg);
       printfHelp(argc, argv);
       exit(1);
      }
      ++pos;
    }
  } else
  {
                        printfHelp(argc, argv);
                        exit (0);
  }

  if(help)
  {
    printfHelp(argc, argv);
    if(node_name)
    {
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
      system(t);
      exit(0);
    }
  }

  if(other_args)
  {
    forms_args->print = 0;

    error = oyXFORMsRenderUi( text, oy_ui_cmd_line_handlers, forms_args );
    result_xml = oyFormsArgs_ModelGet( forms_args );

    opts = oyOptions_FromText( result_xml, 0,0 );

    data = oyOptions_GetText( opts, oyNAME_NAME );
    opt_names = oyOptions_GetText( opts, oyNAME_DESCRIPTION );

      for( i = 0; i < other_args_n; i += 2 )
      {
        /* check for wrong args */
        if(opt_names && strstr( opt_names, other_args[i] ) == NULL)
        {
          fprintf(stderr, "Unknown option: %s", other_args[i]);
          printfHelp( argc, argv );
          exit( 1 );

        } else
        {
          o = oyOptions_Find( opts, other_args[i] );
          if(i + 1 < other_args_n)
          {
            ct = oyOption_GetText( o, oyNAME_NICK );
            if(oy_debug)
            fprintf( stderr, "%s => ",
                    ct?ct:"---" ); ct = 0;
            oyOption_SetFromText( o, other_args[i + 1], 0 );
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
      print = 0;
  }

  if(output_profile)
  {
    uint32_t flags = 0;
    oyPixel_t pixel_layout;
    oyDATATYPE_e data_type = oyUINT8;

    if(!output)
      WARNc_S("No output file name provided");
    if(!icc_defaults_simple)
      flags |= oyOPTIONATTRIBUTE_ADVANCED;
    error = oyImage_FromFile( input, &image, NULL );
    pixel_layout = oyImage_PixelLayoutGet( image );
    data_type = oyToDataType_m(pixel_layout);
    p = oyProfile_FromFile(output_profile, 0,0);
    if(oyProfiles_Count(effects))
      error = oyOptions_MoveInStruct( &module_options,
                                     "//" OY_TYPE_STD "/config/profiles_effect",
                                       (oyStruct_s**) &effects,
                                       OY_CREATE_NEW );
    if(oyProfiles_Count(proofing))
      error = oyOptions_MoveInStruct( &module_options,
                                      "//" OY_TYPE_STD "/config/profiles_proof",
                                       (oyStruct_s**) &proofing,
                                       OY_CREATE_NEW );
    oyConversion_s * cc = oyConversion_FromImage (
                                image, node_name, module_options, 
                                p, data_type, flags, 0 );
    error = oyConversion_RunPixels( cc, 0 );
    oyImage_Release( &image );
#ifdef USE_LCMS
    if(0)
    {
      image = oyConversion_GetImage( cc, OY_INPUT );
      oyProfile_s * p_in = oyImage_ProfileGet(image);
      const char * pfn_in = oyProfile_GetFileName(p_in, -1);
      oyImage_Release( &image );
      cmsHPROFILE in = cmsOpenProfileFromFile( pfn_in, "rb"),
                  out = cmsOpenProfileFromFile(output_profile, "rb");
      image = oyConversion_GetImage( cc, OY_OUTPUT );
      oyPixel_t pixel_layout_out = oyImage_PixelLayoutGet( image );
      oyDATATYPE_e data_type_out = oyToDataType_m(pixel_layout_out);
      uint32_t flags_in = cmsFormatterForColorspaceOfProfile(in, 
                       data_type == oyFLOAT ? 4 : data_type == oyUINT16 ? 2 : 1,
                                                    data_type == oyFLOAT ? 1:0),
               flags_out = cmsFormatterForColorspaceOfProfile( out,
               data_type_out == oyFLOAT ? 4 : data_type_out == oyUINT16 ? 2 : 1,
                                               data_type_out == oyFLOAT ? 1:0) ;
      printf("input %s -> %s %d->%d\n", pfn_in, output_profile, TYPE_XYZ_FLT, TYPE_RGB_FLT );
      cmsHTRANSFORM xform = cmsCreateTransform(in, flags_in, out, flags_out, 0,
                                               cmsFLAGS_NOOPTIMIZE);
      char * image_data = image->getLine( image, 0, 0, -1, 0 );
      cmsDoTransform(xform, image_data, image_data, image->height * image->width);
      //memset(image_data,0,image->height * image->width*3*4);
    }
#endif
    image = oyConversion_GetImage( cc, OY_OUTPUT );
    error = oyImage_PpmWrite( image, output, input );
    
  } else
  if(format && strcmp(format,"icc") == 0)
  {
    
    oyProfile_s * prof = 0;
    size_t size = 0;
    char * data = 0;
    fprintf(stderr, "%s\n", input);
    error = oyImage_FromFile( input, &image, NULL );
    prof = oyImage_ProfileGet( image );
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
    
  } else
  {
                        printfHelp(argc, argv);
                        exit (0);
  }

  return error;
}

int    oyImage_FromFile              ( const char        * file_name,
                                       oyImage_s        ** image,
                                       oyObject_s          obj )
{
  oyFilterNode_s * in, * out;
  int error = 0;
  oyConversion_s * conversion = 0;
  oyOptions_s * options = 0;

  if(!file_name)
    return 1;

  /* start with an empty conversion object */
  conversion = oyConversion_New( obj );
  /* create a filter node */
  in = oyFilterNode_NewWith( "//" OY_TYPE_STD "/file_read.meta", 0, obj );
  /* set the above filter node as the input */
  oyConversion_Set( conversion, in, 0 );

  /* add a file name argument */
  /* get the options of the input node */
  if(in)
  options = oyFilterNode_OptionsGet( in, OY_SELECT_FILTER );
  /* add a new option with the appropriate value */
  error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/file_read/filename",
                                 file_name, OY_CREATE_NEW );
  /* release the options object, this means its not any more refered from here*/
  oyOptions_Release( &options );

  /* add a closing node */
  out = oyFilterNode_NewWith( "//" OY_TYPE_STD "/output", 0, obj );
  error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                out, "//" OY_TYPE_STD "/data", 0 );
  /* set the output node of the conversion */
  oyConversion_Set( conversion, 0, out );

  *image = oyConversion_GetImage( conversion, OY_OUTPUT );
  oyImage_Release( image );
  *image = oyConversion_GetImage( conversion, OY_INPUT );

  oyConversion_Release( &conversion );

  return error;
}

/** Function oyConversion_FromImage
 *  @brief   generate a Oyranos graph from a image file name
 *
 *  @param[in]     image_in            input
 *  @param[in]     module              tobe ussed icc node
 *  @param[in]     flags               for inbuild defaults |
 *                                     oyOPTIONSOURCE_FILTER;
 *                                     for options marked as advanced |
 *                                     oyOPTIONATTRIBUTE_ADVANCED |
 *                                     OY_SELECT_FILTER |
 *                                     OY_SELECT_COMMON
 *  @param[in]     data_type           the desired data type for output
 *  @param[in]     obj                 Oyranos object (optional)
 *  @return                            generated new graph, owned by caller
 *
 *  @version Oyranos: 0.4.1
 *  @since   2012/04/21 (Oyranos: 0.4.1)
 *  @date    2012/04/21
 */
oyConversion_s * oyConversion_FromImage (
                                       oyImage_s         * image_in,
                                       const char        * module,
                                       oyOptions_s       * module_options,
                                       oyProfile_s       * output_profile,
                                       oyDATATYPE_e        buf_type_out,
                                       uint32_t            flags,
                                       oyObject_s          obj )
{
  oyFilterNode_s * in, * out, * icc;
  int error = 0;
  oyConversion_s * conversion = 0;
  oyOptions_s * options = 0;
  char * module_reg = 0;
  oyImage_s * image_out = 0;

  if(!image_in)
    return NULL;

  /* start with an empty conversion object */
  conversion = oyConversion_New( obj );
  /* create a filter node */
  in = oyFilterNode_NewWith( "//" OY_TYPE_STD "/root", 0, obj );
  /* set the above filter node as the input */
  oyConversion_Set( conversion, in, 0 );
  /* set the image buffer */
  oyFilterNode_DataSet( in, (oyStruct_s*)image_in, 0, 0 );


  STRING_ADD( module_reg, "//" OY_TYPE_STD "/" );
  if(module)
    STRING_ADD( module_reg, module );
  else
    STRING_ADD( module_reg, "icc" );

  /* create a new CMM filter node */
  icc = out = oyFilterNode_NewWith( module_reg, module_options, obj );
  /* append the new to the previous one */
  error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                out, "//" OY_TYPE_STD "/data", 0 );
  if(error > 0)
    fprintf( stderr, "could not add  filter: %s\n", "//" OY_TYPE_STD "/icc" );

  /* Create a output image with supplied channel depth and profile */
  image_out   = oyImage_Create( image_in->width, image_in->height,
                         0,
                      oyChannels_m(oyProfile_GetChannelsCount(output_profile)) |
                          oyDataType_m(buf_type_out),
                         output_profile,
                         0 );

  /* swap in and out */
  in = out;

  /* add a closing node */
  out = oyFilterNode_NewWith( "//" OY_TYPE_STD "/output", 0, obj );
  error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                out, "//" OY_TYPE_STD "/data", 0 );
  oyFilterNode_DataSet( in, (oyStruct_s*)image_out, 0, 0 );
  /* set the output node of the conversion */
  oyConversion_Set( conversion, 0, out );

  /* apply policies */
  /*error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "//verbose",
                                 "true", OY_CREATE_NEW );*/
  oyConversion_Correct( conversion, "//" OY_TYPE_STD "/icc", flags,
                        options );
  oyOptions_Release( &options );

  return conversion;
}
