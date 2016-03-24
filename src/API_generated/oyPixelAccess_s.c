/** @file oyPixelAccess_s.c

   [Template file inheritance graph]
   +-> oyPixelAccess_s.template.c
   |
   +-> Base_s.c
   |
   +-- oyStruct_s.template.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2016 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


  
#include "oyPixelAccess_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"


#include "oyPixelAccess_s_.h"

#include "oyRectangle_s_.h"
#include "oyFilterSocket_s_.h"
#include "oyFilterPlug_s_.h"
  


/** Function oyPixelAccess_New
 *  @memberof oyPixelAccess_s
 *  @brief   allocate a new PixelAccess object
 */
OYAPI oyPixelAccess_s * OYEXPORT
  oyPixelAccess_New( oyObject_s object )
{
  oyObject_s s = object;
  oyPixelAccess_s_ * pixelaccess = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  pixelaccess = oyPixelAccess_New_( s );

  return (oyPixelAccess_s*) pixelaccess;
}

/** Function oyPixelAccess_Copy
 *  @memberof oyPixelAccess_s
 *  @brief   copy or reference a PixelAccess object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     pixelaccess                 PixelAccess struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyPixelAccess_s* OYEXPORT
  oyPixelAccess_Copy( oyPixelAccess_s *pixelaccess, oyObject_s object )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*) pixelaccess;

  if(s)
    oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 0 )

  s = oyPixelAccess_Copy_( s, object );

  return (oyPixelAccess_s*) s;
}
 
/** Function oyPixelAccess_Release
 *  @memberof oyPixelAccess_s
 *  @brief   release and possibly deallocate a oyPixelAccess_s object
 *
 *  @param[in,out] pixelaccess                 PixelAccess struct object
 */
OYAPI int OYEXPORT
  oyPixelAccess_Release( oyPixelAccess_s **pixelaccess )
{
  oyPixelAccess_s_ * s = 0;

  if(!pixelaccess || !*pixelaccess)
    return 0;

  s = (oyPixelAccess_s_*) *pixelaccess;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 1 )

  *pixelaccess = 0;

  return oyPixelAccess_Release_( &s );
}



/* Include "PixelAccess.public_methods_definitions.c" { */
/** Function  oyPixelAccess_ChangeRectangle
 *  @memberof oyPixelAccess_s
 *  @brief    Change the ticket for a conversion graph
 *
 *  @param[in,out] pixel_access        optional pixel iterator configuration
 *  @param[in]     start_x             x position relative to virtual source
 *                                     image
 *  @param[in]     start_y             y position relative to virtual source
 *                                     image
 *  @param[in]     output_rectangle    the region in the output image, optional
 *  @return                            0 on success, else error
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/04/17 (Oyranos: 0.3.0)
 *  @date    2011/05/05
 */
int                oyPixelAccess_ChangeRectangle (
                                       oyPixelAccess_s   * pixel_access,
                                       double              start_x,
                                       double              start_y,
                                       oyRectangle_s     * output_rectangle )
{
  oyPixelAccess_s_ ** pixel_access_ = (oyPixelAccess_s_**)&pixel_access;
  int error = 0;
  oyRectangle_s_ * roi = (oyRectangle_s_*)oyRectangle_New(0);

  if(!pixel_access)
    error = 1;

  if(error <= 0 && output_rectangle)
    oyRectangle_SetByRectangle( (oyRectangle_s*)(*pixel_access_)->output_image_roi,
                                output_rectangle );

  if(error <= 0)
  {
    oyRectangle_SetByRectangle( (oyRectangle_s*)roi, (oyRectangle_s*)(*pixel_access_)->output_image_roi );
    (*pixel_access_)->start_xy[0] = roi->x = start_x;
    (*pixel_access_)->start_xy[1] = roi->y = start_y;
  }
  oyRectangle_Release( (oyRectangle_s**)&roi );

  return error;
}

/** Function  oyPixelAccess_Create
 *  @memberof oyPixelAccess_s
 *  @brief    Allocate iand initialise a basic oyPixelAccess_s object
 *
 *  @verbatim
  // conversion->out_ has to be linear, so we access only the first plug
  node = oyConversion_GetNode( conversion, OY_OUTPUT );
  plug = oyFilterNode_GetPlug( node, 0 );
  oyFilterNode_Release( &node );

  // create a very simple pixel iterator
  if(plug)
    pixel_access = oyPixelAccess_Create( 0,0, plug,
                                         oyPIXEL_ACCESS_IMAGE, 0 );
@endverbatim
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/07/07 (Oyranos: 0.1.8)
 *  @date    2009/06/10
 */
oyPixelAccess_s *  oyPixelAccess_Create (
                                       int32_t             start_x,
                                       int32_t             start_y,
                                       oyFilterPlug_s    * plug,
                                       oyPIXEL_ACCESS_TYPE_e type,
                                       oyObject_s          object )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)oyPixelAccess_New( object );
  oyFilterSocket_s_ * sock = 0;
  oyFilterPlug_s_ ** plug_ = (oyFilterPlug_s_**)&plug;
  int error = !s || !plug || !(*plug_)->remote_socket_;
  int w = 0;
  oyImage_s * image = 0;
  int32_t n = 0;

  if(error <= 0)
  {
    sock = (*plug_)->remote_socket_;
    image = (oyImage_s*)sock->data;

    s->start_xy[0] = s->start_xy_old[0] = start_x;
    s->start_xy[1] = s->start_xy_old[1] = start_y;

    /* make shure the filter->image_ is set, e.g.
       error = oyFilterCore_ImageSet ( filter, image );

    s->data_in = filter->image_->data; */
    if(image)
      w = oyImage_GetWidth( image );

    /** The filters have no obligation to pass end to end informations.
        The ticket must hold all pices of interesst.
     */
    s->output_image_roi->width = 1.0;
    if(image)
      s->output_image_roi->height = oyImage_GetHeight( image ) / (double)oyImage_GetWidth( image );
    s->output_image = oyImage_Copy( image, 0 );
    s->graph = (oyFilterGraph_s_*)oyFilterGraph_FromNode( (oyFilterNode_s*)sock->node, 0 );

    if(type == oyPIXEL_ACCESS_POINT)
    {
      s->array_xy = s->oy_->allocateFunc_(sizeof(int32_t) * 2);
      s->array_xy[0] = s->array_xy[1] = 0;
      s->array_n = 1;
      s->pixels_n = 1;
    } else
    if(type == oyPIXEL_ACCESS_LINE)
    {
      s->array_xy = s->oy_->allocateFunc_(sizeof(int32_t) * 2);
      /* set relative advancements from one pixel to the next */
      s->array_xy[0] = 1;
      s->array_xy[1] = 0;
      s->array_n = 1;
      s->pixels_n = w;       /* the total we want */
    } else
    /* if(type == oyPIXEL_ACCESS_IMAGE) */
    {
      /** @todo how can we know about the various module capabilities
       *  - back report the processed number of pixels in the passed pointer
       *  - restrict for a line interface only, would fit to oyArray2d_s
       *  - + handle inside an to be created function oyConversion_RunPixels()
       */
    }

    /* Copy requests, which where attached to the node, to the ticket. */
    if((*plug_)->node->core->options_)
      error = oyOptions_Filter( &s->request_queue, &n, 0,
                                oyBOOLEAN_INTERSECTION,
                                "////resolve", (*plug_)->node->core->options_ );
  }

  if(error)
    oyPixelAccess_Release ( (oyPixelAccess_s**)&s );

  return (oyPixelAccess_s*)s;
}

/** Function  oyPixelAccess_GetOutputImage
 *  @memberof oyPixelAccess_s
 *  @brief    Access oyPixelAccess_s::output_image
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/06
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
oyImage_s *        oyPixelAccess_GetOutputImage (
                                       oyPixelAccess_s   * pixel_access )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 0 )

  return oyImage_Copy( s->output_image, 0 );
}
/** Function  oyPixelAccess_SetOutputImage
 *  @memberof oyPixelAccess_s
 *  @brief    Set oyPixelAccess_s::output_image
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/10/04
 *  @since    2012/10/04 (Oyranos: 0.5.0)
 */
int                oyPixelAccess_SetOutputImage (
                                       oyPixelAccess_s   * pixel_access,
                                       oyImage_s         * image )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 1 )

  oyImage_Release( &s->output_image );
  s->output_image = oyImage_Copy( image, 0 );

  return 0;
}
/** Function  oyPixelAccess_GetOutputROI
 *  @memberof oyPixelAccess_s
 *  @brief    Access oyPixelAccess_s::output_image_roi
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/06
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
oyRectangle_s *    oyPixelAccess_GetOutputROI (
                                       oyPixelAccess_s   * pixel_access )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 0 )

  return oyRectangle_Copy( (oyRectangle_s*)s->output_image_roi, 0 );
}
/** Function  oyPixelAccess_GetGraph
 *  @memberof oyPixelAccess_s
 *  @brief    Access oyPixelAccess_s::graph
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/06
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
oyFilterGraph_s *  oyPixelAccess_GetGraph (
                                       oyPixelAccess_s   * pixel_access )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 0 )

  return oyFilterGraph_Copy( (oyFilterGraph_s*)s->graph, 0 );
}
/** Function  oyPixelAccess_GetRequestQueue
 *  @memberof oyPixelAccess_s
 *  @brief    Access oyPixelAccess_s::request_queue
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/06
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
oyOptions_s *      oyPixelAccess_GetRequestQueue (
                                       oyPixelAccess_s   * pixel_access )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 0 )

  return oyOptions_Copy( s->request_queue, 0 );
}
/** Function  oyPixelAccess_GetArray
 *  @memberof oyPixelAccess_s
 *  @brief    Access oyPixelAccess_s::array
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/06
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
oyArray2d_s *      oyPixelAccess_GetArray (
                                       oyPixelAccess_s   * pixel_access )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 0 )

  return oyArray2d_Copy( s->array, 0 );
}
/** Function  oyPixelAccess_SetArray
 *  @memberof oyPixelAccess_s
 *  @brief    Set oyPixelAccess_s::array
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/06
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
int                oyPixelAccess_SetArray (
                                       oyPixelAccess_s   * pixel_access,
                                       oyArray2d_s       * array )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 1 )

  if(s->array != array)
  {
    if(s->array)
      oyArray2d_Release( &s->array );
    s->array = oyArray2d_Copy( array, 0 );
  }

  return 0;
}
/** Function  oyPixelAccess_GetUserData
 *  @memberof oyPixelAccess_s
 *  @brief    Access oyPixelAccess_s::user_data
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/06
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
oyStruct_s *       oyPixelAccess_GetUserData (
                                       oyPixelAccess_s   * pixel_access )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 0 )

  if(s->user_data->copy)
    return s->user_data->copy( s->user_data, 0 );
  else 
    return s->user_data;
}
/** Function  oyPixelAccess_SetUserData
 *  @memberof oyPixelAccess_s
 *  @brief    Set oyPixelAccess_s::user_data
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/06
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
int                oyPixelAccess_SetUserData (
                                       oyPixelAccess_s   * pixel_access,
                                       oyStruct_s        * user_data )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 1 )

  if(user_data->copy)
    s->user_data = user_data->copy( user_data, 0 );
  else 
    s->user_data = user_data;

  return 0;
}
/** Function  oyPixelAccess_GetWorkspaceID
 *  @memberof oyPixelAccess_s
 *  @brief    Access oyPixelAccess_s::workspace_id
 *
 *  A ID for distinct resources.
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/06
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
int32_t            oyPixelAccess_GetWorkspaceID (
                                       oyPixelAccess_s   * pixel_access )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 0 )

  return s->workspace_id;
}
/** Function  oyPixelAccess_SWorkspaceID
 *  @memberof oyPixelAccess_s
 *  @brief    Set oyPixelAccess_s::workspace_id
 *
 *  A ID for distinct resources.
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/06
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
int                oyPixelAccess_SetWorkspaceID (
                                       oyPixelAccess_s   * pixel_access,
                                       int32_t             workspace_id )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 1 )

  return s->workspace_id = workspace_id;
}
/** Function  oyPixelAccess_GetStart
 *  @memberof oyPixelAccess_s
 *  @brief    Access oyPixelAccess_s::start_xy
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/06
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
double             oyPixelAccess_GetStart (
                                       oyPixelAccess_s   * pixel_access,
                                       int                 vertical )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 0 )

  if(vertical == 0)
    return s->start_xy[0];
  else
    return s->start_xy[1];
}
/** Function  oyPixelAccess_GetOldStart
 *  @memberof oyPixelAccess_s
 *  @brief    Access oyPixelAccess_s::start_xy_old
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/10
 *  @since    2012/09/10 (Oyranos: 0.5.0)
 */
double             oyPixelAccess_GetOldStart (
                                       oyPixelAccess_s   * pixel_access,
                                       int                 vertical )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 0 )

  if(vertical == 0)
    return s->start_xy_old[0];
  else
    return s->start_xy_old[1];
}
/** Function  oyPixelAccess_SetOldStart
 *  @memberof oyPixelAccess_s
 *  @brief    Access oyPixelAccess_s::start_xy_old
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/10
 *  @since    2012/09/10 (Oyranos: 0.5.0)
 */
int                oyPixelAccess_SetOldStart (
                                       oyPixelAccess_s   * pixel_access,
                                       int                 vertical,
                                       double              start )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*)pixel_access;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PIXEL_ACCESS_S, return 1 )

  if(vertical == 0)
    s->start_xy_old[0] = start;
  else
    s->start_xy_old[1] = start;
  return 0;
}

/* } Include "PixelAccess.public_methods_definitions.c" */

