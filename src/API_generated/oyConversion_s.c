/** @file oyConversion_s.c

   [Template file inheritance graph]
   +-> oyConversion_s.template.c
   |
   +-> Base_s.c
   |
   +-- oyStruct_s.template.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


  
#include "oyConversion_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"


#include "oyConversion_s_.h"

#include "oyranos_module_internal.h"
#include "oyCMMapiFilters_s.h"
#include "oyFilterGraph_s.h"
#include "oyFilterPlug_s_.h"
#include "oyFilterNode_s_.h"
#include "oyImage_s.h"
#include "oyImage_s_.h"
#include "oyCMMapi9_s_.h"
#include "oyPixelAccess_s_.h"
  


/** Function oyConversion_New
 *  @memberof oyConversion_s
 *  @brief   allocate a new Conversion object
 */
OYAPI oyConversion_s * OYEXPORT
  oyConversion_New( oyObject_s object )
{
  oyObject_s s = object;
  oyConversion_s_ * conversion = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  conversion = oyConversion_New_( s );

  return (oyConversion_s*) conversion;
}

/** Function  oyConversion_Copy
 *  @memberof oyConversion_s
 *  @brief    Copy or Reference a Conversion object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     conversion                 Conversion struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyConversion_s* OYEXPORT
  oyConversion_Copy( oyConversion_s *conversion, oyObject_s object )
{
  oyConversion_s_ * s = (oyConversion_s_*) conversion;

  if(s)
  {
    oyCheckType__m( oyOBJECT_CONVERSION_S, return NULL )
  }
  else
    return NULL;

  s = oyConversion_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyConversion_s" );

  return (oyConversion_s*) s;
}
 
/** Function oyConversion_Release
 *  @memberof oyConversion_s
 *  @brief   release and possibly deallocate a oyConversion_s object
 *
 *  @param[in,out] conversion                 Conversion struct object
 */
OYAPI int OYEXPORT
  oyConversion_Release( oyConversion_s **conversion )
{
  oyConversion_s_ * s = 0;

  if(!conversion || !*conversion)
    return 0;

  s = (oyConversion_s_*) *conversion;

  oyCheckType__m( oyOBJECT_CONVERSION_S, return 1 )

  *conversion = 0;

  return oyConversion_Release_( &s );
}



/* Include "Conversion.public_methods_definitions.c" { */
/** Function  oyConversion_Correct
 *  @memberof oyConversion_s
 *  @brief    Check for correctly adhering to policies
 *
 *  Without any options the module selected with the @ref registration 
 *  policy filter shall perform graph analysis and correct the graph.
 *
 *  @code
    // use the output
    error = oyConversion_Correct( conversion, "//" OY_TYPE_STD "/icc_color", 0, 0 );
    if(error > 0) error_msg();
    else if(error < 0) debug_msg();
    @endcode

 *  @par Typical Options:
 *  - "command"-"help" - a string option issuing a help text as message
 *  - "verbose" - reporting changes as message
 *
 *  TODO: display and selection of policy modules
 *
 *  @see defaults_apis
 *
 *  @param   conversion                the to be checked configuration
 *  @param   registration              the to be used policy module
 *  @param[in]     flags               select particular options:
 *                                     - for inbuild defaults
 *                                       @ref oyOPTIONSOURCE_FILTER
 *                                     - for options marked as advanced
 *                                       @ref oyOPTIONATTRIBUTE_ADVANCED
 *                                     - use inbuild options from filter type
 *                                       @ref OY_SELECT_FILTER
 *                                     - cover policy options
 *                                       @ref OY_SELECT_COMMON
 *  @param   options                   options to the policy module
 *  @return                            status
 *                                     - 0 : indifferent
 *                                     - >= 1 : error
 *                                     - < 0 : issue like, no module found from registration arg
 *                                     + a message should be sent
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/01/25
 *  @since   2009/07/24 (Oyranos: 0.1.10)
 */
int                oyConversion_Correct (
                                       oyConversion_s    * conversion,
                                       const char        * registration,
                                       uint32_t            flags,
                                       oyOptions_s       * options )
{
  int error = -1;
  oyConversion_s_ * s = (oyConversion_s_*)conversion;
  const char * pattern = registration;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_CONVERSION_S, return 1 )

  if(!s->input && !s->out_)
  {
    WARNc1_S( "%s",_("Found no node in conversion. give up") );
    return 1;
  }

  if(error <= 0)
  {
    oyCMMapiFilters_s * apis;
    int apis_n = 0, i;
    oyCMMapi9_s_ * cmm_api9_ = 0;
    char * class_name, * api_reg;

    class_name = oyFilterRegistrationToText( pattern, oyFILTER_REG_APPLICATION, 0 );
    api_reg = oyStringCopy_("///", oyAllocateFunc_ );
    STRING_ADD( api_reg, class_name );
    oyFree_m_( class_name );

    apis = oyCMMsGetFilterApis_( api_reg, oyOBJECT_CMM_API9_S,
                                 oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR,
                                 0,0 );
    oyFree_m_( api_reg );
    apis_n = oyCMMapiFilters_Count( apis );
    for(i = 0; i < apis_n; ++i)
    {
      cmm_api9_ = (oyCMMapi9_s_*) oyCMMapiFilters_Get( apis, i );

      if(oyFilterRegistrationMatch( cmm_api9_->pattern, pattern, 0 ))
      {
        if(cmm_api9_->oyConversion_Correct)
          error = cmm_api9_->oyConversion_Correct( conversion, flags, options );
        if(error)
        {
          WARNc2_S( "%s %s",_("error in module:"), cmm_api9_->registration );
          return 1;
        }
      }

      if(cmm_api9_->release)
        cmm_api9_->release( (oyStruct_s**)&cmm_api9_ );
    }
    oyCMMapiFilters_Release( &apis );
  }

  if(error == -1)
    WARNc3_S( "%s: %s flags: %d",_("Found no policy module"), registration, flags );

  return error;
}

/** Function  oyConversion_CreateBasicPixels
 *  @memberof oyConversion_s
 *  @brief    Allocate and initialise a basic oyConversion_s object
 *
 *  @code
    // create a image graph
    oyConversion_CreateBasicPixels( input_image, output_image, 0, 0 );
    @endcode

 *  Provided options will be passed to oyFilterNode_NewWith(). There for no
 *  options defaults will be selected.
 *
 *  @param[in]     input               the input image data
 *  @param[in]     output              the output image data
 *  @param[in]     options             options to be passed to oyFilterCore_s
 *                                     Each option should be carefully named
 *                                     to be correctly selected by the intented
 *                                     filter (root, icc and output).
 *                                     The available options are particial
 *                                     defined by plugable policy modules,
 *                                     e.g. "oicc", and a filters own options.
 *                                     OY_DEFAULT_CMM_CONTEXT and 
 *                                     OY_DEFAULT_CMM_RENDERER can contain a 
 *                                     string to explicitely select the icc 
 *                                     module.
 *  @param         object              the optional object
 *  @return                            the conversion context
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/06/29
 *  @since   2008/06/26 (Oyranos: 0.1.8)
 */
oyConversion_s   * oyConversion_CreateBasicPixels (
                                       oyImage_s         * input,
                                       oyImage_s         * output,
                                       oyOptions_s       * options,
                                       oyObject_s          object )
{
  oyConversion_s * s = 0;
  int error = !input || !output;
  oyFilterNode_s * in = 0, * out = 0, * a[3] = {NULL,NULL,NULL};

  if(error <= 0)
  {
    s = oyConversion_New ( 0 );
    error = !s;

    if(error <= 0)
      a[0] = in = oyFilterNode_NewWith( "//" OY_TYPE_STD "/root", options, 0 );
    if(error <= 0)
      error = oyConversion_Set( s, in, 0 );
    if(error <= 0)
      error = oyFilterNode_SetData( in, (oyStruct_s*)input, 0, 0 );

    if(error <= 0)
      a[2] = out = oyFilterNode_FromOptions( OY_CMM_STD, "//" OY_TYPE_STD "/icc_color", options, object );
    if(error <= 0)
      error = oyFilterNode_SetData( out, (oyStruct_s*)output, 0, 0 );
    if(error <= 0)
      error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                    out, "//" OY_TYPE_STD "/data", 0 );

    in = out; out = 0;

    if(error <= 0)
      a[2] = out = oyFilterNode_NewWith( "//" OY_TYPE_STD "/output", options, 0 );
    if(error <= 0)
    {
      error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                    out, "//" OY_TYPE_STD "/data", 0 );
      if(error)
        WARNc1_S( "could not add  filter: %s\n", "//" OY_TYPE_STD "/output" );
    }
    if(error <= 0)
      error = oyConversion_Set( s, 0, out );
  }

  if(error)
  {
    int i;
    for( i = 2; i >= 0; --i )
      oyFilterNode_Release( &a[i] );
    oyConversion_Release ( &s );
  }

  return s;
}

/** Function  oyConversion_CreateBasicPixelsFromBuffers
 *  @memberof oyConversion_s
 *  @brief    One dimensional color conversion context
 *
 *  The options are passed to oyConversion_CreateBasicPixels();
 *  The function does the lookups for the profiles and the modules contexts
 *  in the Oyranos cache on the fly.
 *
 *  @param[in]     p_in                the input profile
 *  @param[in]     buf_in              the input channels
 *  @param[in]     buf_type_in         the input pixel type; channel count can be omitted
 *  @param[in]     p_out               the output profile
 *  @param[in]     buf_out             the output channels
 *  @param[in]     buf_type_out        the output pixel type; channel count can be omitted
 *  @param[in]     options             see the same option in  oyConversion_CreateBasicPixels()
 *  @param[in]     count               the pixel count to convert in buf_in and buf_out
 *  @return                            the conversion context
 *
 *  @version Oyranos: 0.9.5
 *  @date    2014/02/05
 *  @since   2011/02/22 (Oyranos: 0.3.0)
 */
oyConversion_s *   oyConversion_CreateBasicPixelsFromBuffers (
                                       oyProfile_s       * p_in,
                                       oyPointer           buf_in,
                                       oyPixel_t           buf_type_in,
                                       oyProfile_s       * p_out,
                                       oyPointer           buf_out,
                                       oyPixel_t           buf_type_out,
                                       oyOptions_s       * options,
                                       int                 count )
{
  oyImage_s * in  = NULL,
            * out = NULL;
  oyConversion_s * conv = NULL;
  int chan, cchan;

  if(count <= 0)
  {
    WARNc1_S("buffer requested with size of pixels: %d", count);
    return NULL;
  }

  if(!buf_in || !buf_out)
  {
    WARNc1_S("buffer%s missed", (!buf_in && !buf_out) ? "s" : "");
    return NULL;
  }

  chan  = oyToChannels_m( buf_type_in );
  cchan = oyProfile_GetChannelsCount( p_in );
  if(!chan && cchan)
  {
    chan = cchan;
    buf_type_in |= oyChannels_m(cchan);
  }
  in   = oyImage_Create( count, 1,
                         buf_in ,
                         buf_type_in,
                         p_in,
                         0 );

  
  chan  = oyToChannels_m( buf_type_out );
  cchan = oyProfile_GetChannelsCount( p_out );
  if(!chan && cchan)
  {
    chan = cchan;
    buf_type_out |= oyChannels_m(cchan);
  }
  out  = oyImage_Create( count, 1,
                         buf_out ,
                         buf_type_out,
                         p_out,
                         0 );

  conv   = oyConversion_CreateBasicPixels( in,out, options, 0 );

  oyImage_Release( &in );
  oyImage_Release( &out );

  return conv;
}

/** Function oyConversion_CreateFromImage
 *  @memberof oyConversion_s
 *  @brief   generate a Oyranos graph from a image file name
 *
 *  @param[in]     image_in            input
 *  @param[in]     module_options      options for icc node
 *  @param[in]     output_profile      profile to convert colors to;
 *  @param[in]     buf_type_out        the desired data type for output
 *  @param[in]     flags               select particular options:
 *                                     - for inbuild defaults
 *                                       @ref oyOPTIONSOURCE_FILTER
 *                                     - for options marked as advanced
 *                                       @ref oyOPTIONATTRIBUTE_ADVANCED
 *                                     - use inbuild options from filter type
 *                                       @ref OY_SELECT_FILTER
 *                                     - cover policy options
 *                                       @ref OY_SELECT_COMMON
 *  @param[in]     obj                 Oyranos object (optional)
 *  @return                            generated new graph, owned by caller
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/07/01
 *  @since   2012/04/21 (Oyranos: 0.5.0)
 */
oyConversion_s * oyConversion_CreateFromImage (
                                       oyImage_s         * image_in,
                                       oyOptions_s       * module_options,
                                       oyProfile_s       * output_profile,
                                       oyDATATYPE_e        buf_type_out,
                                       uint32_t            flags,
                                       oyObject_s          obj )
{
  oyFilterNode_s * in, * out;
  int error = 0;
  oyConversion_s * conversion = 0;
  oyOptions_s * options = 0;
  oyImage_s * image_out = 0;
  int layout_out = 0;
  oyProfile_s * profile_in;
  int chan_in;
  int cchan_in;

  if(!image_in)
    return NULL;

  /* start with an empty conversion object */
  conversion = oyConversion_New( obj );
  /* create a filter node */
  in = oyFilterNode_NewWith( "//" OY_TYPE_STD "/root", 0, obj );
  /* set the above filter node as the input */
  oyConversion_Set( conversion, in, 0 );
  /* set the image buffer */
  oyFilterNode_SetData( in, (oyStruct_s*)image_in, 0, 0 );

  /* create a new CMM filter node */
  out = oyFilterNode_FromOptions( OY_CMM_STD, "//" OY_TYPE_STD "/icc_color", module_options, obj );
  /* append the new to the previous one */
  error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                out, "//" OY_TYPE_STD "/data", 0 );
  if(error > 0)
    fprintf( stderr, "could not add  filter: %s\n", "//" OY_TYPE_STD "/icc_color" );

  layout_out = oyDataType_m(buf_type_out);
  profile_in = oyImage_GetProfile( image_in );
  chan_in     = oyToChannels_m( oyImage_GetPixelLayout(image_in, oyLAYOUT ) );
  cchan_in = oyProfile_GetChannelsCount( profile_in );
  oyProfile_Release( &profile_in );

  if(!chan_in && cchan_in)
    chan_in = cchan_in;
  /* preserve alpha */
  layout_out |= oyChannels_m( oyProfile_GetChannelsCount(output_profile)
                              + chan_in - cchan_in );

  /* Create a output image with supplied channel depth and profile */
  image_out   = oyImage_Create( oyImage_GetWidth( image_in ),
                                oyImage_GetHeight( image_in ),
                         0,
                         layout_out,
                         output_profile,
                         0 );
  oyProfile_Release( &output_profile );

  /* swap in and out */
  in = out;

  /* add a closing node */
  out = oyFilterNode_NewWith( "//" OY_TYPE_STD "/output", 0, obj );
  error = oyFilterNode_Connect( in, "//" OY_TYPE_STD "/data",
                                out, "//" OY_TYPE_STD "/data", 0 );
  oyFilterNode_SetData( in, (oyStruct_s*)image_out, 0, 0 );
  oyImage_Release( &image_out );

  /* set the output node of the conversion */
  oyConversion_Set( conversion, 0, out );

  /* apply policies */
  /*error = oyOptions_SetFromString( &options, "//" OY_TYPE_STD "//verbose",
                                 "true", OY_CREATE_NEW );*/
  oyConversion_Correct( conversion, "//" OY_TYPE_STD "/icc_color", flags,
                        options );
  oyOptions_Release( &options );

  return conversion;
}

/** Function  oyConversion_GetGraph
 *  @memberof oyConversion_s
 *  @brief    Get the filter graph from a conversion context
 *
 *  @see oyFilterGraph_GetNode() to obtain a node.
 *
 *  @param[in,out] conversion          conversion object
 *  @return                            the filter graph containing all nodes
 *
 *  @version Oyranos: 0.2.1
 *  @since   2011/01/02 (Oyranos: 0.2.1)
 *  @date    2011/01/02
 */
oyFilterGraph_s  * oyConversion_GetGraph (
                                       oyConversion_s    * conversion )
{
  if(conversion)
    return oyFilterGraph_FromNode(
                  (oyFilterNode_s*)(((oyConversion_s_*)conversion)->input), 0 );
  else
    return NULL;
}

/** Function  oyConversion_GetImage
 *  @memberof oyConversion_s
 *  @brief    Get a image copy at the desired position
 *
 *  @param[in,out] conversion          conversion object
 *  @param[in]     flags               OY_INPUT or OY_OUTPUT
 *  @return                            the image
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/19 (Oyranos: 0.1.10)
 *  @date    2009/02/19
 */
oyImage_s        * oyConversion_GetImage (
                                       oyConversion_s    * conversion,
                                       uint32_t            flags )
{
  oyImage_s * image = 0;
  oyFilterPlug_s_ * plug = 0;
  oyFilterSocket_s_ * sock = 0;
  int error = 0;
  oyConversion_s_ * s = (oyConversion_s_*)conversion;
  oyPixelAccess_s * pixel_access = 0;

  oyCheckType__m( oyOBJECT_CONVERSION_S, return 0 )

  if(!error)
  {
    if(oyToInput_m(flags))
    {
      sock = (oyFilterSocket_s_*) oyFilterNode_GetSocket(
                                                 (oyFilterNode_s*)s->input, 0 );
      if(sock)
      {
        image = oyImage_Copy( (oyImage_s*) sock->data, 0 );

        if(!image)
        {
          plug = (oyFilterPlug_s_*)oyFilterNode_GetPlug((oyFilterNode_s*)s->out_,0);
          pixel_access = oyPixelAccess_Create( 0,0, (oyFilterPlug_s*)plug,
                                               oyPIXEL_ACCESS_IMAGE, 0 );
          s->out_->api7_->oyCMMFilterPlug_Run( (oyFilterPlug_s*) plug,
                                               pixel_access );
          oyPixelAccess_Release( &pixel_access );
          oyFilterPlug_Release( (oyFilterPlug_s**)&plug );

          image = oyImage_Copy( (oyImage_s*) sock->data, 0 );
        }
      }
      oyFilterSocket_Release( (oyFilterSocket_s**)&sock );

    } else
    if(oyToOutput_m(flags))
    {
      plug = (oyFilterPlug_s_*)oyFilterNode_GetPlug((oyFilterNode_s*)s->out_,0);
      if(plug && plug->remote_socket_)
      {
        image = oyImage_Copy( (oyImage_s*) plug->remote_socket_->data, 0);

        if(!image)
        {
          oyFilterPlug_Release( (oyFilterPlug_s**)&plug );
          /* Run the graph to set up processing image data. */
          plug = (oyFilterPlug_s_*) oyFilterNode_GetPlug(
                                               (oyFilterNode_s*)s->out_, 0 );
          pixel_access = oyPixelAccess_Create( 0,0, (oyFilterPlug_s*)plug,
                                               oyPIXEL_ACCESS_IMAGE, 0 );
          s->out_->api7_->oyCMMFilterPlug_Run( (oyFilterPlug_s*)plug,
                                               pixel_access );

          {
            oyFilterPlug_s * root_plug = oyFilterNode_GetPlug(
                                               (oyFilterNode_s*)s->input, 0 );
            oyFilterNode_s * reader = oyFilterPlug_GetRemoteNode( root_plug );
            if(reader)
            {
              oyObject_UnRef( s->input->oy_ );
              oyFilterNode_Copy( reader, NULL );
              s->input = (oyFilterNode_s_*) reader;
            }
            oyFilterPlug_Release( &root_plug );
            oyFilterNode_Release( &reader );
          }
          /* Link the tickets image. It should be real copied in a plug-in. */
          /* error = oyFilterNode_SetData( s->out_,
                                        (oyStruct_s*)pixel_access->output_image,
                                        0, 0 ); */
          oyPixelAccess_Release( &pixel_access );

          image = oyImage_Copy( (oyImage_s*) plug->remote_socket_->data, 0 );
        }
      }
      oyFilterPlug_Release( (oyFilterPlug_s**)&plug );
    }
  }

  return image;
}

/** Function  oyConversion_GetNode
 *  @memberof oyConversion_s
 *  @brief    Get the filter node copy
 *
 *  @param[in,out] conversion          conversion object
 *  @param[in]     flags               OY_INPUT or OY_OUTPUT
 *  @return                            the filter node as defined by flags
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/03/27 (Oyranos: 0.3.0)
 *  @date    2011/03/29
 */
oyFilterNode_s   * oyConversion_GetNode (
                                       oyConversion_s    * conversion,
                                       uint32_t            flags )
{
  oyFilterNode_s * node = 0;
  int error = 0;
  oyConversion_s_ * s = (oyConversion_s_*)conversion;

  oyCheckType__m( oyOBJECT_CONVERSION_S, return 0 )

  if(!error)
  {
    if(oyToInput_m(flags))
    {
      node = oyFilterNode_Copy( (oyFilterNode_s*)s->input, 0 );
    } else
    if(oyToOutput_m(flags))
    {
      node = oyFilterNode_Copy( (oyFilterNode_s*)s->out_, 0 );
    }
  }

  return node;
}

/** Function  oyConversion_GetOnePixel
 *  @memberof oyConversion_s
 *  @brief    Compute one pixel at the given position
 *
 *  @param[in,out] conversion          conversion object
 *  @param[in]     x                   position x
 *  @param[in]     y                   position y
 *  @param[in,out] pixel_access        pixel iterator configuration
 *  @return                            -1 end; 0 on success; error > 1
 *
 *  @version Oyranos: 0.3.0
 *  @since   2008/07/14 (Oyranos: 0.1.8)
 *  @date    2011/04/11
 */
int          oyConversion_GetOnePixel( oyConversion_s    * conversion,
                                       double              x,
                                       double              y,
                                       oyPixelAccess_s   * pixel_access )
{
  oyFilterPlug_s_ * plug = 0;
  oyFilterSocket_s_ * sock = 0;
  int error = 0;

  /* conversion->out_ has to be linear, so we access only the first socket */
  plug = (oyFilterPlug_s_*)oyFilterNode_GetPlug( (oyFilterNode_s *)oyConversionPriv_m(conversion)->out_, 0 );
  sock = plug->remote_socket_;

  oyPixelAccessPriv_m(pixel_access)->start_xy[0] = x;
  oyPixelAccessPriv_m(pixel_access)->start_xy[1] = y;

  /* @todo */
  error = sock->node->api7_->oyCMMFilterPlug_Run( (oyFilterPlug_s*)plug, pixel_access );

  oyFilterPlug_Release( (oyFilterPlug_s**) &plug );
  return error;
}

/** Function  oyConversion_RunPixels
 *  @memberof oyConversion_s
 *  @brief    Process a pixel conversion graph
 *
 *  @param[in,out] conversion          conversion object
 *  @param[in,out] pixel_access        optional custom pixel iterator
 *                                     configuration;
 *                                     The ticket can be passed in for
 *                                     specifying only regions of the source
 *                                     data.
 *  @return                            0 on success, else error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/07/06 (Oyranos: 0.1.8)
 *  @date    2009/03/06
 *
 *  Here a very basic code snippet:
 *  @code
    // use the output
    oyConversion_RunPixels( conversion, NULL );
    oyImage_s * image = oyConversion_GetImage( conversion, OY_OUTPUT );
    // get the data and draw the image
    for(i = 0; i < image->height; ++i)
    {
      image_data = oyImage_GetLineF(image)( image, i, &height, -1, &is_allocated );

      // ...

      if(is_allocated)
        free( image_data );
    } @endcode
 *
 *  Here a example from test2.cpp for creating a job ticket from the DAG:
 *  @dontinclude test2.cpp
    @skip create a basic job ticket for faster repeats of oyConversion_RunPixels()
    @until oyFilterPlug_Release
 *
 *  oyFilterGraph_PrepareContexts() uses the internal oyFilterNode_SetContext_()
 *  for automatic resources resolving during DAG processing. Both oyCMMapi4_s
 *  and oyCMMapi7_s contexts are checked for if declared by oyCMMapi4_Create()
 *  context_type argument.
 */
int                oyConversion_RunPixels (
                                       oyConversion_s    * conversion,
                                       oyPixelAccess_s   * pixel_access )
{
  oyConversion_s_ * s = (oyConversion_s_*)conversion;
  oyFilterPlug_s * plug = 0;
  oyFilterNode_s * node_out = 0;
  oyImage_s * image_out = 0,
            * image_input = 0;
  int error = 0, result = 0, dirty = 0;
  oyRectangle_s_ roi = {oyOBJECT_RECTANGLE_S, 0,0,0, 0,0,0,0};
  double clck;

  oyPixelAccess_s_ * pixel_access_ = (oyPixelAccess_s_*)oyPixelAccess_Copy(pixel_access,NULL);

  oyCheckType__m( oyOBJECT_CONVERSION_S, return 1 )

  /* should be the same as conversion->out_->filter */
  node_out = oyConversion_GetNode( conversion, OY_OUTPUT );
  plug = oyFilterNode_GetPlug( node_out, 0 );

  /* basic checks */
  if(!plug)
  {
    WARNc1_S("graph incomplete [%d]", s ? oyObject_GetId( s->oy_ ) : -1)
    return 1;
  }

  /* conversion->out_ has to be linear, so we access only the first plug */

  if(!pixel_access_)
  {
    /** The function creates a very simple pixel iterator if no job ticket
        is passed in as __pixel_access__. */
    if(plug)
    {
      if(oy_debug) clck = oyClock();
      pixel_access_ = (oyPixelAccess_s_*)oyPixelAccess_Create( 0,0, plug,
                                                      oyPIXEL_ACCESS_IMAGE, 0 );
      if(oy_debug)
      { clck = oyClock() - clck;
        DBG_PROG1_S("oyPixelAccess_Create(): %g", clck/1000000.0 );
      }
    }
  }

  if(!pixel_access_)
    error = 1;

  image_out = oyConversion_GetImage( conversion, OY_OUTPUT );

  if(error <= 0)
    oyRectangle_SetByRectangle( (oyRectangle_s*)&roi,
                                (oyRectangle_s*)pixel_access_->output_array_roi );

  if(error <= 0 && !pixel_access_->array)
  {
    if(oy_debug) clck = oyClock();
    result = oyImage_FillArray( image_out, (oyRectangle_s*)&roi, 0,
                                &pixel_access_->array,
                                (oyRectangle_s*)pixel_access_->output_array_roi, 0 );
    if(oy_debug)
    { clck = oyClock() - clck;
      DBGs_PROG1_S( pixel_access_,"oyImage_FillArray(): %g", clck/1000000.0 );
    }
    error = ( result != 0 );
  }

  /* run on the graph */
  if(error <= 0)
  {
    DBGs_PROG2_S( pixel_access_, "Run: node_out[%d] image_out[%d]",
                 oyStruct_GetId((oyStruct_s*)node_out),
                 oyStruct_GetId((oyStruct_s*)image_out) );
    if(oy_debug) clck = oyClock();
    error = oyFilterNodePriv_m(node_out)->api7_->oyCMMFilterPlug_Run( plug,
                                             (oyPixelAccess_s*)pixel_access_ );
    if(oy_debug)
    { clck = oyClock() - clck;
      DBG_PROG1_S( "conversion->out_->api7_->oyCMMFilterPlug_Run(): %g",
                clck/1000000.0 );
      DBGs_PROG1_S( pixel_access_, 
         "conversion->out_->api7_->oyCMMFilterPlug_Run(): %g", clck/1000000.0 );
    }
  }

  if(error != 0 && pixel_access_)
  {
    dirty = oyOptions_FindString( (oyOptions_s*)pixel_access_->graph->options, "dirty", "true")
            ? 1 : 0;

    /* refresh the graph representation */
    if(oy_debug) clck = oyClock();
    oyFilterGraph_SetFromNode( (oyFilterGraph_s*)pixel_access_->graph, (oyFilterNode_s*)s->input, 0, 0 );
    if(oy_debug)
    { clck = oyClock() - clck;
      DBGs_PROG1_S(pixel_access_,"oyFilterGraph_SetFromNode(): %g",clck/1000000.0 );
    }

    /* resolve missing data */
    if(oy_debug) clck = oyClock();
    image_input = oyFilterPlug_ResolveImage( plug, (oyFilterSocket_s*)((oyFilterPlug_s_*)plug)->remote_socket_,
                                             (oyPixelAccess_s*)pixel_access_ );
    if(oy_debug)
    { clck = oyClock() - clck;
      DBGs_PROG1_S(pixel_access_,"oyFilterPlug_ResolveImage(): %g",clck/1000000.0 );
    }
    DBGs_PROG1_S( pixel_access_, "image_input: %s",
                  oyStruct_GetText((oyStruct_s*)image_input, oyNAME_NAME, 0) );
    oyImage_Release( &image_input );

    {
#if 0
      clck = oyClock();
      l_error = oyArray2d_Release( &(*pixel_access_)->array ); OY_ERR
      l_error = oyImage_FillArray( image_out, &roi, 0,
                                   &(*pixel_access_)->array,
                                   (*pixel_access_)->output_array_roi, 0 ); OY_ERR
      clck = oyClock() - clck;
      DBG_PROG1_S("oyImage_FillArray(): %g", clck/1000000.0 );
#endif

      if(error != 0 &&
         dirty)
      {
        if(pixel_access_->start_xy[0] != pixel_access_->start_xy_old[0] ||
           pixel_access_->start_xy[1] != pixel_access_->start_xy_old[1])
        {
          /* set back to previous values, at least for the simplest case */
          pixel_access_->start_xy[0] = pixel_access_->start_xy_old[0];
          pixel_access_->start_xy[1] = pixel_access_->start_xy_old[1];
        }

        if(oy_debug) clck = oyClock();
        oyFilterGraph_PrepareContexts( (oyFilterGraph_s*)pixel_access_->graph, 0 );
        if(oy_debug)
        { clck = oyClock() - clck;
          DBGs_PROG1_S( pixel_access_,
                     "oyFilterGraph_PrepareContexts(): %g", clck/1000000.0 );
          clck = oyClock();
        }
        error = s->out_->api7_->oyCMMFilterPlug_Run( plug,
                                              (oyPixelAccess_s*)pixel_access_);
        if(oy_debug)
        { clck = oyClock() - clck;
          DBGs_PROG1_S( pixel_access_,
          "conversion->out_->api7_->oyCMMFilterPlug_Run(): %g",clck/1000000.0 );
        }
      }
    }
  }

  DBGs_PROG1_S( pixel_access_, "image_out: %s",
                oyStruct_GetText((oyStruct_s*)image_out, oyNAME_NAME, 0) );
  oyPixelAccess_SetArrayFocus( (oyPixelAccess_s*)pixel_access_, 0 );
  if(oy_debug)
  {
    oyRectangle_s * pix = NULL; oyImage_RoiToSamples( image_out,
                                (oyRectangle_s*)&roi, &pix );
    oyImage_SamplesToPixels( image_out, pix, pix );
    char * t = NULL; STRING_ADD( t, oyRectangle_Show( pix ) );
    oyRectangle_Release( &pix );
    DBGs_PROG2_S( pixel_access_,
          "roi:%s %s", t,
          oyPixelAccess_Show( (oyPixelAccess_s*)pixel_access_ ) );
  }

  /** Write the pixel data possibly to the output image.
   * \n
   * The oyPixelAccess_s job ticket contains a oyArray2d_s object called array
   * holding the in memory data. After the job is done the output images
   * pixel_data pointer is compared with the job tickets array pointer. If
   * they are the same it is assumed that a observer of the output image will
   * see the same processed data, otherwise oyPixelAccess_s::array must be
   * copied to the output image. Here a example from test2.cpp for a
   * optimisation by setting the same array in the DAG output image and the
   * __ticket__:
   *  @dontinclude test2.cpp
      @skip oyImage_GetPixelData
      @until oyPixelAccess_SetArray
   *  @see oyPixelAccess_Create() oyImage_GetPixelData() oyPixelAccess_GetOutputImage() oyPixelAccess_GetArray() oyPixelAccess_SetArray()
   *
   * While the design of having whatever data storage in a oyImage_s is very
   * flexible, the oyPixelAccess_s::array's in memory buffer is not.
   * Users with very large data sets have to process the data in chunks and
   * the oyPixelAccess_s::array allocation can remain constant.
   */
  if(image_out && oyImagePriv_m(image_out)->pixel_data->type_ == oyOBJECT_ARRAY2D_S && pixel_access_ &&
     ((oyPointer)oyImagePriv_m(image_out)->pixel_data != (oyPointer)pixel_access_->array ||
      image_out != pixel_access_->output_image))
  {
    /* move the array to the top left place
     * same as : roi.x = roi.y = 0; */
    /*roi.x = (*pixel_access_)->start_xy[0];
    roi.y = (*pixel_access_)->start_xy[1];*/
    result = oyImage_ReadArray( image_out, (oyRectangle_s*)&roi,
                                pixel_access_->array, 0 );
  }

  oyPixelAccess_Release( (oyPixelAccess_s**)&pixel_access_ );

  oyImage_Release( &image_out );
  oyFilterPlug_Release( &plug );
  oyFilterNode_Release( &node_out );

  return error;
}

/** Function  oyConversion_Set
 *  @memberof oyConversion_s
 *  @brief    Set input and output of a conversion graph
 *
 *  @param[in,out] conversion          conversion object
 *  @param[in]     input               input node
 *  @param[in]     output              output node
 *  @return                            0 on success, else error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/05 (Oyranos: 0.1.10)
 *  @date    2009/03/05
 */
int                oyConversion_Set  ( oyConversion_s    * conversion,
                                       oyFilterNode_s    * input,
                                       oyFilterNode_s    * output )
{
  oyConversion_s_ * s = (oyConversion_s_*)conversion;
  int error = 0;

  oyCheckType__m( oyOBJECT_CONVERSION_S, return 1 )

  if(input)
    s->input = (oyFilterNode_s_*)oyFilterNode_Copy(input,NULL);

  if(output)
    s->out_ = (oyFilterNode_s_*)oyFilterNode_Copy(output,NULL);

  return error;
}

/** Function  oyConversion_ToText
 *  @memberof oyConversion_s
 *  @brief    Text description of a conversion graph
 *
 *  @todo Should this function generate XFORMS compatible output? How?
 *
 *  @param[in]     conversion          conversion object
 *  @param[in]     head_line           text for inclusion
 *  @param[in]     reserved            future format selector (dot, xml ...)
 *  @param[in]     allocateFunc        allocation function
 *  @return                            the graph description
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/10/04 (Oyranos: 0.1.8)
 *  @date    2009/03/05
 */
char             * oyConversion_ToText (
                                       oyConversion_s    * conversion,
                                       const char        * head_line,
                                       int                 reserved,
                                       oyAlloc_f           allocateFunc )
{
  oyConversion_s_ * s = (oyConversion_s_*)conversion;
  char * text = 0;
  oyFilterGraph_s * adjacency_list = 0;

  oyCheckType__m( oyOBJECT_CONVERSION_S, return 0 )

  adjacency_list = oyFilterGraph_New( 0 );
  oyFilterGraph_SetFromNode( adjacency_list, (oyFilterNode_s*)s->input, 0, 0 );

  text = oyFilterGraph_ToText( adjacency_list,
                               (oyFilterNode_s*)s->input, (oyFilterNode_s*)s->out_,
                               head_line, reserved, allocateFunc );

  oyFilterGraph_Release( &adjacency_list );

  return text;
}

/* } Include "Conversion.public_methods_definitions.c" */

