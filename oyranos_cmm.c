/** @file oyranos_cmm.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2008-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    CMM core APIs
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2009/04/28
 *
 *  basic and / or common functions for modules
 */


#include "oyranos_alpha.h"
#include "oyranos_alpha_internal.h"
#include "oyranos_object_internal.h"
#include "oyranos_cmm.h"
#include "oyranos_elektra.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_io.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"
#if !defined(WIN32)
#include <dlfcn.h>
#endif
#include <math.h>
#include <stdarg.h> /* va_list */

#define OY_ERR if(l_error != 0) error = l_error;

#ifdef DEBUG_
#define DEBUG_OBJECT 1
#endif

/** @func    oyFilterPlug_ImageRootRun
 *  @brief   implement oyCMMFilter_GetNext_f()
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/10 (Oyranos: 0.1.8)
 *  @date    2009/05/01
 */
int      oyFilterPlug_ImageRootRun   ( oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket )
{
  int x_pix = 0, y_pix = 0, n = 0;
  int result = 0, error = 0;
  int is_allocated = 0;
  oyPointer * ptr = 0;
  oyFilterSocket_s * socket = requestor_plug->remote_socket_;
  oyImage_s * image = (oyImage_s*)socket->data;
  int width;

  DBGs_NUM2_S( ticket, "%s[%d]", _("Work on remote socket image"),
               oyStruct_GetId( (oyStruct_s*)image ) );

  /* Do not work on non existent data. */
  if(!image || !ticket->output_image)
    return result;

  /* Set a unknown output image dimension to something appropriate. */
  if(!ticket->output_image->width && !ticket->output_image->height)
  {
    DBGs_NUM7_S( ticket, "%s[%d] %s %.04gx%.04g %.04gx%.04g",
                 _("Set dimensions on ticket->output_image"),
                 oyStruct_GetId( (oyStruct_s*)ticket->output_image ),
                 oyRectangle_Show( ticket->output_image_roi ),
                 ticket->output_image->width, ticket->output_image->height,
                 image->width, image->height );
    ticket->output_image->width = image->width;
    ticket->output_image->height = image->height;
    oyImage_SetCritical( ticket->output_image, image->layout_[0], 0, 0 );
  }

  width = ticket->output_image->width;

  x_pix = ticket->start_xy[0] * width;
  y_pix = ticket->start_xy[1] * width;

  if(x_pix < image->width &&
     y_pix < image->height &&
     ticket->pixels_n)
  {
    n = ticket->pixels_n;
    if(n == 1)
      ptr = image->getPoint( image, x_pix, y_pix, 0, &is_allocated );

    result = !ptr;

  } else {
    char * t = 0;

    /* adapt the rectangle of interesst to the new image dimensions */
    oyRectangle_s image_roi = {oyOBJECT_RECTANGLE_S,0,0,0},
                  output_image_roi = {oyOBJECT_RECTANGLE_S,0,0,0};
    double correct = ticket->output_image->width / (double) image->width;
    oyRectangle_SetByRectangle( &image_roi, ticket->output_image_roi );
    oyRectangle_SetByRectangle( &output_image_roi, ticket->output_image_roi );
    /* x and y source image offset */
    image_roi.x = x_pix / (double) image->width;
    image_roi.y = y_pix / (double) image->width;
    image_roi.width *= correct;
    image_roi.height *= correct;
    STRING_ADD( t, oyRectangle_Show( &image_roi ) );
    DBGs_NUM4_S( ticket, "%s[%d] %s %s", _("Fill ticket->array from image"),
                 oyStruct_GetId( (oyStruct_s*)image ),
                 oyRectangle_Show( ticket->output_image_roi ),
                 t );
    oyRectangle_Scale( &output_image_roi, correct );
    error = oyImage_FillArray( image, &image_roi, 1,
                               &ticket->array, &output_image_roi, 0 );
    if(error)
      result = error;
    oyFree_m_( t );
  }

  return result;
}


/** Function oyFilterSocket_MatchImagingPlug
 *  @brief   verify connectors matching each other
 *  @ingroup module_api
 *  @memberof oyCMMapi5_s
 *
 *  @param         socket              a filter socket
 *  @param         plug                a filter plug
 *  @return                            1 on success, otherwise 0
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/04/20 (Oyranos: 0.1.10)
 *  @date    2009/04/28
 */
int          oyFilterSocket_MatchImagingPlug (
                                       oyFilterSocket_s  * socket,
                                       oyFilterPlug_s    * plug )
{
  int match = 0;
  oyConnectorImaging_s * a = 0,  * b = 0;
  oyImage_s * image = 0;
  int colours_n = 0, n, i, j;
  oyDATATYPE_e data_type = 0;

  if(socket && socket->type_ == oyOBJECT_FILTER_SOCKET_S &&
     socket->pattern && socket->pattern->type_ == oyOBJECT_CONNECTOR_IMAGING_S)
    a = (oyConnectorImaging_s*)socket->pattern;

  if(plug && plug->type_ == oyOBJECT_FILTER_PLUG_S &&
     plug->pattern && plug->pattern->type_ == oyOBJECT_CONNECTOR_IMAGING_S)
    b = (oyConnectorImaging_s*) plug->pattern;

  if(a && b)
  {
    match = 1;
    image = oyImage_Copy( (oyImage_s*)socket->data, 0 );

    if(!b->is_plug)
      match = 0;

    /** For a zero set pixel layout we skip most tests and assume it will be
        checked later. */
    if(image && image->layout_[oyLAYOUT] && match)
    {
      /* channel counts */
      colours_n = oyProfile_GetChannelsCount( image->profile_ );
      if(image->layout_[oyCHANS] < b->min_channels_count ||
         image->layout_[oyCHANS] > b->max_channels_count ||
         colours_n < b->min_colour_count ||
         colours_n > b->max_colour_count)
        match = 0;

      /* data types */
      if(match)
      {
        data_type = oyToDataType_m( image->layout_[oyLAYOUT] );
        n = b->data_types_n;
        match = 0;
        for(i = 0; i < n; ++i)
          if(b->data_types[i] == data_type)
            match = 1;
      }

      /* planar and interwoven capabilities */
      if(b->max_colour_offset < image->layout_[oyCOFF] ||
         (!b->can_planar && oyToPlanar_m(image->layout_[oyCOFF])) ||
         (!b->can_interwoven && !oyToPlanar_m(image->layout_[oyCOFF])))
        match = 0;

      /* swap and byteswapping capabilities */
      if((!b->can_swap && oyToSwapColourChannels_m(image->layout_[oyCOFF])) ||
         (!b->can_swap_bytes && oyToByteswap_m(image->layout_[oyCOFF])))
        match = 0;

      /* revert or chockolat and vanilla */
      if((!b->can_revert && oyToFlavor_m(image->layout_[oyCOFF])))
        match = 0;

      /* channel types */
      if(match && b->channel_types)
      {
        n = image->layout_[oyCHANS];
        for(i = 0; i < b->channel_types_n; ++i)
        {
          match = 0;
          for(j = 0; j < n; ++j)
            if(b->channel_types[i] == image->channel_layout[j] &&
               !(!b->can_nonpremultiplied_alpha &&
                 image->channel_layout[j] == oyCHANNELTYPE_COLOUR_LIGHTNESS) &&
               !(!b->can_premultiplied_alpha &&
                 image->channel_layout[j] == oyCHANNELTYPE_COLOUR_LIGHTNESS_PREMULTIPLIED))
              match = 1;
          if(!match)
            break;
        }
      }

      /* subpixels */
      if(image->sub_positioning && !b->can_subpixel)
        match = 0;
    }
  }

  oyImage_Release( &image );
  oyConnectorImaging_Release( &a );

  return match;
}

/**
 *  This function implements oyCMMInfoGetText_f.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2010/09/06
 */
const char * oyCMMgetText             ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
         if(strcmp(select, "name")==0)
  {
         if(type == oyNAME_NICK)
      return _("NoNo");
    else if(type == oyNAME_NAME)
      return _("Oyranos modules");
    else
      return _("Oyranos supplied modules");
  } else if(strcmp(select, "manufacturer")==0)
  {
         if(type == oyNAME_NICK)
      return _("Kai-Uwe");
    else if(type == oyNAME_NAME)
      return _("Kai-Uwe Behrmann");
    else
      return _("Oyranos project; www: http://www.oyranos.com; support/email: ku.b@gmx.de; sources: http://www.oyranos.com/wiki/index.php?title=Oyranos/Download");
  } else if(strcmp(select, "copyright")==0)
  {
         if(type == oyNAME_NICK)
      return _("newBSD");
    else if(type == oyNAME_NAME)
      return _("Copyright (c) 2005-2010 Kai-Uwe Behrmann; newBSD");
    else
      return _("new BSD license: http://www.opensource.org/licenses/bsd-license.php");
  }
  return 0;
}
const char *oyCMM_texts[4] = {"name","copyright","manufacturer",0};

/**
 *  This function implements oyCMMInfoGetText_f for image connector plugs.
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
const char * oyCMMgetImageConnectorPlugText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
  if(strcmp(select, "name")==0)
  {
    if(type == oyNAME_NICK)
      return _("Img");
    else if(type == oyNAME_NAME)
      return _("Image");
    else if(type == oyNAME_DESCRIPTION)
      return _("Image PNG Plug");
  }
  return NULL;
}
const char * oy_image_connector_texts[] = {"name", 0};
/**
 *  This function implements oyCMMInfoGetText_f for image connector sockets.
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
const char * oyCMMgetImageConnectorSocketText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context )
{
  if(strcmp(select, "name")==0)
  {
    if(type == oyNAME_NICK)
      return _("Img");
    else if(type == oyNAME_NAME)
      return _("Image");
    else if(type == oyNAME_DESCRIPTION)
      return _("Image PNG Socket");
  }
  return NULL;
}

oyObjectInfoStatic_s oy_connector_imaging_static_object = {
  oyOBJECT_INFO_STATIC_S, 0,0,0,
  {2,oyOBJECT_CONNECTOR_S,oyOBJECT_CONNECTOR_IMAGING_S,0,0,0,0,0}
};
