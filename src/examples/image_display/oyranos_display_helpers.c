/**
 *  Oyranos is an open source Color Management System 
 * 
 *  @par Copyright:
 *            2009-2013 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2010/09/05
 *
 *  Oyranos helpers for handling on screen display of images.
 */

#include <oyranos.h>
#include <oyConversion_s.h>
#include <oyObject_s.h>
#include "oyranos_display_helpers.h"
#include <oyranos_image.h>

#ifndef _DBG_FORMAT_
#define _DBG_UHR_ (double)clock()/(double)CLOCKS_PER_SEC
#define _DBG_FORMAT_ " %f %s:%d %s() "
#define _DBG_ARGS_ _DBG_UHR_,(strrchr(__FILE__,'/') ? strrchr(__FILE__,'/')+1 : __FILE__),__LINE__,__func__
#endif
int oy_display_verbose = 0;
extern "C" { int oyWriteMemToFile_(const char* name, const void* mem, size_t size); }

oyConversion_s * oyConversion_FromImageForDisplay_ (
                                       oyImage_s         * image_in,
                                       oyImage_s         * image_out,
                                       oyFilterNode_s   ** cc_node,
                                       uint32_t            flags,
                                       oyDATATYPE_e        data_type,
                                       oyOptions_s       * cc_options,
                                       oyObject_s          obj )
{
  oyFilterNode_s * in = 0, * out = 0, * icc = 0;
  int error = 0;
  oyConversion_s * conversion = 0;
  oyOptions_s * options = 0;
  oyOption_s * option = 0;
  const char * sv = 0;
  double scale = 0;

  if(!image_in || !image_out)
    return NULL;

  /* start with an empty conversion object */
  conversion = oyConversion_New( obj );
  /* create a filter node */
  in = oyFilterNode_NewWith( "//" OY_TYPE_STD "/root", 0, obj );
  /* set the above filter node as the input */
  oyConversion_Set( conversion, in, 0 );
  /* set the image buffer */
  oyFilterNode_SetData( in, (oyStruct_s*)image_in, 0, 0 );


  /* add a scale node */
  out = oyFilterNode_NewWith( "//" OY_TYPE_STD "/scale", 0, obj );
  options = oyFilterNode_GetOptions( out, OY_SELECT_FILTER );
  /* scale factor from DB */
  option = oyOption_FromRegistration( OY_INTERNAL "/scale/scale", 0 );
  error = oyOption_SetFromText( option, 0, 0 );
  error = oyOption_SetValueFromDB( option );
  scale = 1.0;
  if(!error)
  {
    sv = oyOption_GetValueString( option, 0 );
    if(sv)
      scale = strtod( sv, 0 );
  }
  oyOption_Release( &option );
  error = oyOptions_SetFromDouble( &options,
                                   OY_INTERNAL "/scale/scale",
                                   scale, 0, OY_CREATE_NEW );
  oyOptions_Release( &options );
  /* append the node */
  error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                out, "//" OY_TYPE_STD "/data", 0 );
  if(error > 0)
    fprintf( stderr, "could not add  filter: %s\n", "//" OY_TYPE_STD "/scale" );
  in = out;


  /* add a expose node */
  out = oyFilterNode_NewWith( "//" OY_TYPE_STD "/expose", 0, obj );
  options = oyFilterNode_GetOptions( out, OY_SELECT_FILTER );
  /* expose factor */
  error = oyOptions_SetFromDouble( &options,
                                   "//" OY_TYPE_STD "/expose/expose",
                                   1.0, 0, OY_CREATE_NEW );
  oyOptions_Release( &options );
  /* append the node */
  error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                out, "//" OY_TYPE_STD "/data", 0 );
  if(error > 0)
    fprintf( stderr, "could not add  filter: %s\n", "//" OY_TYPE_STD "/expose" );
  in = out;


  /* add a channel node */
  out = oyFilterNode_NewWith( "//" OY_TYPE_STD "/channel", 0, obj );
  options = oyFilterNode_GetOptions( out, OY_SELECT_FILTER );
  /* channel option*/
  error = oyOptions_SetFromText( &options,
                                   "//" OY_TYPE_STD "/channel/channel",
                                   "", OY_CREATE_NEW );
  oyOptions_Release( &options );
  /* append the node */
  error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                out, "//" OY_TYPE_STD "/data", 0 );
  if(error > 0)
    fprintf( stderr, "could not add  filter: %s\n", "//" OY_TYPE_STD "/channel" );
  in = out;


  /* create a new filter node */
  {
    icc = out = oyFilterNode_FromOptions( OY_CMM_STD, "//" OY_TYPE_STD "/icc_color",
                                     cc_options, NULL );
    /* append the new to the previous one */
    error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                  out, "//" OY_TYPE_STD "/data", 0 );
    if(error > 0)
      fprintf( stderr, "could not add  filter: %s\n", OY_CMM_STD );

    /* Set the image to the first/only socket of the filter node.
     * oyFilterNode_Connect() has now no chance to copy it it the other nodes.
     * We rely on resolving the image later.
     */
    error = oyFilterNode_SetData( out, (oyStruct_s*)image_out, 0, 0 );
    if(error != 0)
      fprintf( stderr, "could not add data\n" );
  }

  /* swap in and out */
  if(out)
    in = out;


  /* create a node for preparing the image for displaying */
  {
    out = oyFilterNode_NewWith( "//" OY_TYPE_STD "/display", 0, obj );
    options = oyFilterNode_GetOptions( out, OY_SELECT_FILTER );
    /* data type for display */
    error = oyOptions_SetFromInt( &options,
                                  "//" OY_TYPE_STD "/display/datatype",
                                  data_type, 0, OY_CREATE_NEW );
    /* alpha might be support once by FLTK? */
    error = oyOptions_SetFromInt( &options,
                                  "//" OY_TYPE_STD "/display/preserve_alpha",
                                  1, 0, OY_CREATE_NEW );
    oyOptions_Release( &options );
    /* append the node */
    error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                  out, "//" OY_TYPE_STD "/data", 0 );
    if(error > 0)
      fprintf( stderr, "could not add  filter: %s\n", "//" OY_TYPE_STD "/display" );
    oyFilterNode_SetData( out, (oyStruct_s*)image_out, 0, 0 );
    in = out;
  }


  /* add a closing node */
  out = oyFilterNode_NewWith( "//" OY_TYPE_STD "/output", 0, obj );
  error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                out, "//" OY_TYPE_STD "/data", 0 );
  if(error > 0)
    fprintf( stderr, "could not add  filter: %s\n", "//" OY_TYPE_STD "/output" );

  /* set the output node of the conversion */
  oyConversion_Set( conversion, 0, out );

  /* apply policies */
  /*error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "//verbose",
                                 "true", OY_CREATE_NEW );*/
  oyConversion_Correct( conversion, "//" OY_TYPE_STD "/icc_color", flags,
                        options );
  oyOptions_Release( &options );

  if(cc_node)
    *cc_node = icc;

  return conversion;
}
/** Function oyConversion_FromImageForDisplay
 *  @brief   generate a Oyranos graph from a image file name
 *
 *  @param[in]     image_in            input
 *  @param[in]     image_out           output
 *  @param[out]    cc_node             used icc node, owned by caller
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
 *  @version Oyranos: 0.9.6
 *  @since   2012/01/21 (Oyranos: 0.4.0)
 *  @date    2014/05/16
 */
oyConversion_s * oyConversion_FromImageForDisplay  (
                                       oyImage_s         * image_in,
                                       oyImage_s         * image_out,
                                       oyFilterNode_s   ** cc_node,
                                       uint32_t            flags,
                                       oyDATATYPE_e        data_type,
                                       oyOptions_s       * cc_options,
                                       oyObject_s          obj )
{
  oyConversion_s * conversion =
     oyConversion_FromImageForDisplay_( image_in, image_out, cc_node, flags,
                                        data_type, cc_options, obj );

  return conversion;
}


/** Function oyDrawScreenImage
 *  @brief   generate a Oyranos image from a given context for display
 *
 *  The function asks the 'oydi' node to provide parameters to render a
 *  oyImage_s from a prepared oyConversion_s context.
 *
 *  @param[in]     context             the Oyranos graph
 *  @param[in]     display_rectangle   absolute coordinates of visible image
 *                                     in relation to display
 *  @param[in,out] old_display_rectangle
 *                                     rembering of display_rectangle
 +  @param[in,out] old_roi_rectangle   remembering of ticket's ROI (optional)
 *  @param[in]     system_type         the system dependent type specification
 *  @param[in]     display             the system display with system_type:
 *                                     - "X11": a Display object
 *  @param[in]     window              the system window with system_type:
 *                                     - "X11": a Window ID
 *  @param[in]     dirty               explicite redraw
 *  @param[out]    image               the image from graph to display
 *  @return                            0 - success, -1 - issue, >=  1 - error
 *
 *  @version Oyranos: 0.4.0
 *  @since   2010/09/05 (Oyranos: 0.1.11)
 *  @date    2012/01/24
 */
int  oyDrawScreenImage               ( oyConversion_s    * context,
                                       oyPixelAccess_s   * ticket,
                                       oyRectangle_s     * display_rectangle,
                                       oyRectangle_s     * old_display_rectangle,
                                       oyRectangle_s     * old_roi_rectangle,
                                       const char        * system_type,
                                       oyDATATYPE_e        data_type_request,
                                       void              * display,
                                       void              * window,
                                       int                 dirty,
                                       oyImage_s         * image )
{
  int result = 0;

    if(context)
    {
      double X,Y,W,H;
      int channels = 0;
      oyFilterNode_s * node_out = 0;
      oyRectangle_s * disp_rectangle = 0,
                    * ticket_roi = 0;
      oyOptions_s * image_tags = 0;
      oyDATATYPE_e data_type = oyUINT8;
      oyPixel_t pt = 0;

      oyRectangle_GetGeo( display_rectangle, &X, &Y, &W, &H );

      if(!image)
        return 1;
      if( W <= 0 || H <= 0)
        return -1;

      image_tags = oyImage_GetTags( image );

    if(window && strcmp("X11", system_type) == 0)
    {
#if defined(XCM_HAVE_X11)
      /* add X11 window and display identifiers to output image */
      oyOption_s * o = 0;
      Display *disp = (Display*) display;
      Window  w = (Window) window;
      int count = oyOptions_CountType( image_tags,
                                       "//" OY_TYPE_STD "/display/window_id",
                                       oyOBJECT_BLOB_S );
      if(!count && w)
      {
        oyBlob_s * win_id = oyBlob_New(0),
                 * display_id = oyBlob_New(0);
        if(win_id)
        {
          oyBlob_SetFromStatic( win_id, (oyPointer)w, 0, 0 );
          o = oyOption_FromRegistration( "//" OY_TYPE_STD "/display/window_id",
                                         0 );
          oyOption_MoveInStruct( o, (oyStruct_s**)&win_id );
          oyOptions_MoveIn( image_tags, &o, -1 );

          oyBlob_SetFromStatic( display_id, (oyPointer)disp, 0, 0 );
          o = oyOption_FromRegistration( "//" OY_TYPE_STD "/display/display_id",
                                         0 );
          oyOption_MoveInStruct( o, (oyStruct_s**)&display_id );
          oyOptions_MoveIn( image_tags, &o, -1 );

          oyOptions_SetFromText( &image_tags,
                                 "//" OY_TYPE_STD "/display/display_name",
                                 DisplayString(disp), OY_CREATE_NEW );

        } else
          printf("%s:%d WARNING: no X11 Window obtained or\n"
                 "   no oyBlob_s allocateable\n", __FILE__,__LINE__);

      }
#endif
   }
      /* check if the actual data can be displayed */
      pt = oyImage_GetPixelLayout( image, oyLAYOUT );
      data_type = oyToDataType_m( pt );
      channels = oyToChannels_m( pt );
      if(pt != 0 &&
         ((channels != 4 && channels != 3) || data_type != data_type_request))
      {
        printf( "%s:%d WARNING: wrong image data format: %s\n%s\n"
                "need 4 or 3 channels with %s\n", __FILE__,__LINE__,
                oyOptions_FindString( image_tags, "filename", 0 ),
                image ? oyObject_GetName( image->oy_, oyNAME_NICK ) : "",
                oyDataTypeToText( data_type_request ) );
        return 1;
      }


      /* Inform about the images display coverage.  */
      disp_rectangle = (oyRectangle_s*) oyOptions_GetType( image_tags, -1,
                                    "display_rectangle", oyOBJECT_RECTANGLE_S );
      oyRectangle_SetGeo( disp_rectangle, X,Y,W,H );


      node_out = oyConversion_GetNode( context, OY_OUTPUT );
      ticket_roi = oyPixelAccess_GetArrayROI( ticket );
      /* decide wether to refresh the cached rectangle of our static image */
      if( node_out &&
          /* Did the window area move? */
         ((!oyRectangle_IsEqual( disp_rectangle, old_display_rectangle ) ||
           /* Something explicite to update? */
           (old_roi_rectangle &&
            !oyRectangle_IsEqual( ticket_roi, old_roi_rectangle ))||
           /* Did the image move? */
           oyPixelAccess_GetStart( ticket,0 ) !=
           oyPixelAccess_GetOldStart( ticket,0 ) ||
           oyPixelAccess_GetStart( ticket,1 ) !=
           oyPixelAccess_GetOldStart( ticket,1 )) ||
           dirty > 0))
      {
#ifdef DEBUG_
        printf( "%s:%d new display rectangle: %s +%d+%d\n", __FILE__,__LINE__,
                oyRectangle_Show(disp_rectangle), X, Y ),
#endif

        /* convert the image data */
        oyConversion_RunPixels( context, ticket );

        if(oy_debug && getenv("OY_DEBUG_WRITE"))
        {
          oyImage_s * out = oyConversion_GetImage( context, OY_OUTPUT );
          oyImage_WritePPM( out, "debug_image_out.ppm", "image_display output image");
          oyImage_Release( &out );
        }

        /* remember the old rectangle */
        oyRectangle_SetByRectangle( old_display_rectangle, disp_rectangle );
        oyRectangle_SetByRectangle( old_roi_rectangle, ticket_roi );
        oyPixelAccess_SetOldStart(ticket,0, oyPixelAccess_GetStart(ticket,0));
        oyPixelAccess_SetOldStart(ticket,1, oyPixelAccess_GetStart(ticket,1));
      } else
        result = -1;

      oyFilterNode_Release( &node_out );
      oyOptions_Release( &image_tags );
      oyRectangle_Release( &disp_rectangle );
      oyRectangle_Release( &ticket_roi );
    }

  if(oy_debug >= 4)
    fprintf( stderr, "%s:%d %s() result: %d\n", strrchr(__FILE__,'/'),__LINE__,__func__, result );
  return result;
}


oyProfile_s * getEditingProfile      ( )
{
  static oyProfile_s * editing = NULL;

  if(!editing)
  {
    oyOption_s *matrix = oyOption_FromRegistration("///color_matrix."
              "from_primaries."
              "redx_redy_greenx_greeny_bluex_bluey_whitex_whitey_gamma", NULL );
    /* http://www.color.org/chardata/rgb/rommrgb.xalter
     * original gamma is 1.8, we adapt to typical LCD gamma of 2.2 */
    oyOption_SetFromDouble( matrix, 0.7347, 0, 0);
    oyOption_SetFromDouble( matrix, 0.2653, 1, 0);
    oyOption_SetFromDouble( matrix, 0.1596, 2, 0);
    oyOption_SetFromDouble( matrix, 0.8404, 3, 0);
    oyOption_SetFromDouble( matrix, 0.0366, 4, 0);
    oyOption_SetFromDouble( matrix, 0.0001, 5, 0);
    oyOption_SetFromDouble( matrix, 0.3457, 6, 0);
    oyOption_SetFromDouble( matrix, 0.3585, 7, 0);
    oyOption_SetFromDouble( matrix, 2.2, 8, 0);

    oyOptions_s * opts = oyOptions_New(0),
                * result = 0;

    oyOptions_MoveIn( opts, &matrix, -1 );
    oyOptions_Handle( "//"OY_TYPE_STD"/create_profile.icc",
                                opts,"create_profile.icc_profile.color_matrix",
                                &result );

    editing = (oyProfile_s*)oyOptions_GetType( result, -1, "icc_profile",
                                               oyOBJECT_PROFILE_S );
    oyOptions_Release( &result );

    oyProfile_AddTagText( editing, icSigProfileDescriptionTag,
                                            "ICC Examin ROMM gamma 2.2" );

    if(oy_debug)
    {
      size_t size = 0;
      char * data = (char*) oyProfile_GetMem( editing, &size, 0, malloc );
      oyWriteMemToFile_( "ICC Examin ROMM gamma 2.2.icc", data, size );
    }
  }

  return editing;
}

