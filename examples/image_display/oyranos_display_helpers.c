/**
 *  Oyranos is an open source Colour Management System 
 * 
 *  @par Copyright:
 *            2009-2010 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2010/09/05
 *
 *  Oyranos helpers for handling on screen display of images.
 */

#include <oyranos.h>
#include <oyranos_alpha.h>
#include <oyranos_cmm.h>   /* for hacking into module API */

#ifdef HAVE_X11
#include <X11/Xlib.h>
#endif


/** Function oyConversion_FromImageFileName
 *  @brief   generate a Oyranos graph from a image file name
 *
 *  @param[in]     file_name           name of image file
 *  @param[out]    icc_node            used icc node, owned by caller
 *  @param[in]     flags               for inbuild defaults |
 *                                     oyOPTIONSOURCE_FILTER;
 *                                     for options marked as advanced |
 *                                     oyOPTIONATTRIBUTE_ADVANCED |
 *                                     OY_SELECT_FILTER |
 *                                     OY_SELECT_COMMON
 *  @param[in]     obj                 Oyranos object (optional)
 *  @return                            generated new graph, owned by caller
 *
 *  @version Oyranos: 0.1.13
 *  @since   2010/09/05 (Oyranos: 0.1.11)
 *  @date    2010/11/27
 */
oyConversion_s * oyConversion_FromImageFileName  (
                                       const char        * file_name,
                                       oyFilterNode_s   ** icc_node,
                                       uint32_t            flags,
                                       oyObject_s          obj )
{
  oyFilterNode_s * in, * out, * icc;
  int error = 0;
  oyConversion_s * conversion = 0;
  oyOptions_s * options = 0;
  oyImage_s * image_in = 0;

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
  options = oyFilterNode_OptionsGet( in, OY_SELECT_FILTER );
  /* add a new option with the appropriate value */
  error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/file_read/filename",
                                 file_name, OY_CREATE_NEW );
  /* release the options object, this means its not any more refered from here*/
  oyOptions_Release( &options );

  /* create a new filter node */
  icc = out = oyFilterNode_NewWith( "//" OY_TYPE_STD "/icc", options, obj );
  /* append the new to the previous one */
  error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                out, "//" OY_TYPE_STD "/data", 0 );
  if(error > 0)
    fprintf( stderr, "could not add  filter: %s\n", "//" OY_TYPE_STD "/icc" );

  /* Set the image to the first/only socket of the filter node.
   * oyFilterNode_Connect() has now no chance to copy it it the other nodes.
   * We rely on resolving the image later.
   */
  oyFilterNode_DataSet( in, (oyStruct_s*)image_in, 0, 0 );

  /* swap in and out */
  in = out;


  /* create a node for preparing the image for displaying */
  out = oyFilterNode_NewWith( "//" OY_TYPE_STD "/display", 0, obj );
  options = oyFilterNode_OptionsGet( out, OY_SELECT_FILTER );
  /* 8 bit data for FLTK */
  error = oyOptions_SetFromInt( &options,
                                "//" OY_TYPE_STD "/display/datatype",
                                oyUINT8, 0, OY_CREATE_NEW );
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
  in = out;


  /* add a closing node */
  out = oyFilterNode_NewWith( "//" OY_TYPE_STD "/output", 0, obj );
  error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                out, "//" OY_TYPE_STD "/data", 0 );
  /* set the output node of the conversion */
  oyConversion_Set( conversion, 0, out );

  /* apply policies */
  /*error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "//verbose",
                                 "true", OY_CREATE_NEW );*/
  oyConversion_Correct( conversion, "//" OY_TYPE_STD "/icc", flags,
                        options );
  oyOptions_Release( &options );


  *icc_node = icc;

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
 +  @param[in,out] old_roi_rectangle   remembering of ticket's ROI
 *  @param[in]     system_type         the system dependent type specification
 *  @param[in]     display             the system display with system_type:
 *                                     - "X11": a Display object
 *  @param[in]     window              the system window with system_type:
 *                                     - "X11": a Window ID
 *  @param[in]     dirty               expicite redraw
 *  @param[out]    image               the image from graph to display
 *  @return                            0 - success, >=  1 - error
 *
 *  @version Oyranos: 0.1.11
 *  @since   2010/09/05 (Oyranos: 0.1.11)
 *  @date    2010/09/10
 */
int  oyDrawScreenImage               ( oyConversion_s    * context,
                                       oyPixelAccess_s   * ticket,
                                       oyRectangle_s     * display_rectangle,
                                       oyRectangle_s     * old_display_rectangle,
                                       oyRectangle_s     * old_roi_rectangle,
                                       const char        * system_type,
                                       void              * display,
                                       void              * window,
                                       int                 dirty,
                                       oyImage_s         * image )
{
    if(context)
    {
      int X = display_rectangle->x;
      int Y = display_rectangle->y;
      int W = display_rectangle->width;
      int H = display_rectangle->height;
      int channels = 0;
      oyRectangle_s * disp_rectangle = 0;
      oyOptions_s * image_tags = 0;
      oyDATATYPE_e data_type = oyUINT8;
      oyPixel_t pt = 0;

      if(!image)
        return 1;

      image_tags = oyImage_TagsGet( image );

    if(window && strcmp("X11", system_type) == 0)
    {
#if defined(HAVE_X11)
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
          oyOption_StructMoveIn( o, (oyStruct_s**)&win_id );
          oyOptions_MoveIn( image_tags, &o, -1 );

          oyBlob_SetFromStatic( display_id, (oyPointer)disp, 0, 0 );
          o = oyOption_FromRegistration( "//" OY_TYPE_STD "/display/display_id",
                                         0 );
          oyOption_StructMoveIn( o, (oyStruct_s**)&display_id );
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
      pt = oyImage_PixelLayoutGet( image );
      data_type = oyToDataType_m( pt );
      channels = oyToChannels_m( pt );
      if(pt != 0 &&
         ((channels != 4 && channels != 3) || data_type != oyUINT8))
      {
        printf( "WARNING: wrong image data format: %s\n%s\n"
                "need 4 or 3 channels with 8-bit\n",
                oyOptions_FindString( image_tags, "filename", 0 ),
                image ? oyObject_GetName( image->oy_, oyNAME_NICK ) : "" );
        return 1;
      }


      /* Inform about the images display coverage.  */
      disp_rectangle = (oyRectangle_s*) oyOptions_GetType( image_tags, -1,
                                    "display_rectangle", oyOBJECT_RECTANGLE_S );
      oyRectangle_SetGeo( disp_rectangle, X,Y,W,H );

      oyOptions_Release( &image_tags );

      /* decide wether to refresh the cached rectangle of our static image */
      if( context->out_ &&
          /* Did the window area move? */
         ((!oyRectangle_IsEqual( disp_rectangle, old_display_rectangle ) ||
           /* Something explicite to update? */
           !oyRectangle_IsEqual( ticket->output_image_roi, old_roi_rectangle )||
           /* Did the image move? */
           ticket->start_xy[0] != ticket->start_xy_old[0] ||
           ticket->start_xy[1] != ticket->start_xy_old[1]) ||
           dirty ))
      {
#ifdef DEBUG
        printf( "%s:%d new display rectangle: %s +%d+%d\n", __FILE__,__LINE__,
                oyRectangle_Show(disp_rectangle), x(), y() ),
#endif

        /* convert the image data */
        oyConversion_RunPixels( context, ticket );

        /* remember the old rectangle */
        oyRectangle_SetByRectangle( old_display_rectangle, disp_rectangle );
        oyRectangle_SetByRectangle( old_roi_rectangle,ticket->output_image_roi);
        ticket->start_xy_old[0] = ticket->start_xy[0];
        ticket->start_xy_old[1] = ticket->start_xy[1];
      }
    }
  return 0;
}


