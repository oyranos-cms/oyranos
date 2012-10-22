/** @file oyranos_file.cpp
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2011 (C) Kai-Uwe Behrmann
 *
 *  @brief    image file handler
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2011/04/06
 *
 */
/* c++ -Wall -g oyranos_file.cpp `pkg-config --cflags --libs oyranos` -o oyranos-file */

#include <oyConversion_s.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern "C" {
typedef struct oyProfile_s_ oyProfile_s_;
int          oyProfile_ToFile_       ( oyProfile_s_      * profile,
                                       const char        * file_name );
}

/** Function oyConversion_FromImageFileName
 *  @brief   generate a Oyranos graph from a image file name
 *
 *  @param[in]     file_name           name of image file
 *  @param[in]     profile_name        name of ICC profile file
 *  @param[in]     flags               set options
 *                                     - 0x01 - request device
 *                                     - 0x02 - no data processing
 *  @param[in]     data_type           the desired data type for output
 *  @param[in]     obj                 Oyranos object (optional)
 *  @return                            generated new graph, owned by caller
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/04/06 (Oyranos: 0.3.0)
 *  @date    2011/04/06
 */
oyConversion_s * oyConversion_FromImageFileName  (
                                       const char        * file_name,
                                       const char        * profile_name,
                                       uint32_t            flags,
                                       oyDATATYPE_e        data_type,
                                       oyObject_s          obj )
{
  oyFilterNode_s * in, * out;
  int error = 0;
  oyConversion_s * conversion = 0;
  oyOptions_s * options = 0;

  if(!file_name)
    return NULL;

  /* start with an empty conversion object */
  conversion = oyConversion_New( obj );
  /* create a filter node */
  in = oyFilterNode_NewWith( "//" OY_TYPE_STD "/file_read.meta", 0, obj );
  /* set the above filter node as the input */
  oyConversion_Set( conversion, in, 0 );

  /* add a file name argument */
  /* get the options of the input node */
  if(in)
  options = oyFilterNode_GetOptions( in, OY_SELECT_FILTER );
  /* add a new option with the appropriate value */
  error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/file_read/filename",
                                 file_name, OY_CREATE_NEW );
  /* set image process options */
  if(flags & 0x01)
    error = oyOptions_SetFromText( &options,
                                   "//" OY_TYPE_STD "/file_read/device",
                                   "1", OY_CREATE_NEW );

  if(profile_name)
  {
    oyProfile_s * prof = oyProfile_FromFile( profile_name, 0, 0 );
    error = oyOptions_MoveInStruct( &options,
                                "//" OY_TYPE_STD "/config/icc_profile.add_meta",
                                    (oyStruct_s**)&prof, OY_CREATE_NEW );
  }
  if(flags & 0x02)
    error = oyOptions_SetFromText( &options,
                                   "//" OY_TYPE_STD "/file_read/render",
                                   "0", OY_CREATE_NEW );
  /* release the options object, this means its not any more refered from here*/
  oyOptions_Release( &options );

  /* add a closing node */
  out = oyFilterNode_NewWith( "//" OY_TYPE_STD "/output", 0, obj );
  error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                out, "//" OY_TYPE_STD "/data", 0 );
  /* set the output node of the conversion */
  if(!error)
    oyConversion_Set( conversion, 0, out );

  return conversion;
}


int main(int argc, char ** argv)
{
  const char * profile_name = argc > 2 ? argv[2] : NULL;

  if(argc == 1)
  {
    fprintf(stderr, "Usage:\t%s camerRaw_filename [profile_name]\n\n", argv[0]);
    return 1;
  }

  oyConversion_s * c = oyConversion_FromImageFileName( argv[1], profile_name,
                                  0x01 | 0x04, oyUINT16, 0 );

  oyImage_s * image = oyConversion_GetImage( c, OY_OUTPUT );

  oyConfig_s * device = 0;
  oyOptions_s * image_tags = oyImage_GetTags( image );
  if(image)
    device = (oyConfig_s*)oyOptions_GetType( image_tags, 0,
                                           "device",
                                           oyOBJECT_CONFIG_S );
  oyOption_s * opt = oyConfig_Find( device, "icc_profile.add_meta" );
  oyProfile_s * profile = (oyProfile_s*) oyOption_GetStruct( opt,
                                                         oyOBJECT_PROFILE_S );
  oyProfile_ToFile_( (oyProfile_s_*)profile, "test_image_profile.icc" );
  oyProfile_Release( &profile );
  oyOption_Release( &opt );
  oyConfig_Release( &device );
  oyConversion_Release( &c );
  oyOptions_Release( &image_tags );

  char * nfn = (char*)malloc(strlen(argv[1])+12);
  sprintf( nfn, "%s", argv[1] );
  char * t = strrchr( nfn, '.' );
  t[0] = '\000';
  sprintf( t, "%s", ".ppm" );

  int error = oyImage_WritePPM( image, nfn, argv[1] );
  if(!error)
    printf( "wrote file to %s\n", nfn );
  oyImage_Release( &image );

  return 0;
}
