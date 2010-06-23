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
#include "oyranos_cmm.h"
#include "oyranos_cmms.h"
#include "oyranos_elektra.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_io.h"
#include "oyranos_sentinel.h"
#include "oyranos_texts.h"
#if !defined(WIN32)
#include <dlfcn.h>
#include <inttypes.h>
#endif
#include <math.h>
#include <stdarg.h> /* va_list */

#define OY_ERR if(l_error != 0) error = l_error;

#ifdef DEBUG_
#define DEBUG_OBJECT 1
#endif


/** Function oyConnectorImaging_New
 *  @memberof oyConnector_s
 *  @brief   allocate a new Connector object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/27 (Oyranos: 0.1.8)
 *  @date    2008/07/27
 */
OYAPI oyConnectorImaging_s * OYEXPORT
             oyConnectorImaging_New  ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CONNECTOR_S;
# define STRUCT_TYPE oyConnectorImaging_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = 0;
  
  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );
  if(!error)
  {
    s->type_ = type;
    s->copy = (oyStruct_Copy_f) oyConnectorImaging_Copy;
    s->release = (oyStruct_Release_f) oyConnectorImaging_Release;

    s->oy_ = s_obj;

    error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
  }
  if(error)
    return 0;
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  s->name.type = oyOBJECT_NAME_S;

  s->is_plug = -1;
  s->max_colour_offset = -1;
  s->min_channels_count = -1;
  s->max_channels_count = -1;
  s->min_colour_count = -1;
  s->max_colour_count = -1;
  s->can_planar = -1;
  s->can_interwoven = -1;
  s->can_swap = -1;
  s->can_swap_bytes = -1;
  s->can_revert = -1;
  s->can_premultiplied_alpha = -1;
  s->can_nonpremultiplied_alpha = -1;
  s->can_subpixel = -1;

  return s;
}

/**
 *  @internal
 *  Function oyConnectorImaging_Copy_
 *  @memberof oyConnector_s
 *  @brief   real copy a Connector object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/27 (Oyranos: 0.1.8)
 *  @date    2008/07/27
 */
oyConnectorImaging_s *
             oyConnectorImaging_Copy_( oyConnectorImaging_s * obj,
                                       oyObject_s          object )
{
  oyConnectorImaging_s * s = 0;
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;

  if(!obj || !object)
    return s;

  s = oyConnectorImaging_New( object );
  error = !s;

  if(error <= 0)
  {
    allocateFunc_ = s->oy_->allocateFunc_;

    s->name.nick = oyStringCopy_( obj->name.nick, allocateFunc_);
    s->name.name = oyStringCopy_( obj->name.name, allocateFunc_);
    s->name.description = oyStringCopy_( obj->name.description, allocateFunc_);

    s->connector_type = obj->connector_type;
    s->is_plug = obj->is_plug;
    if(obj->data_types_n)
    {
      s->data_types = allocateFunc_( obj->data_types_n * sizeof(oyDATATYPE_e) );
      error = !s->data_types;
      if(!error)
        error = !memcpy( s->data_types, obj->data_types,
                         obj->data_types_n * sizeof(oyDATATYPE_e) );
      if(error <= 0)
        s->data_types_n = obj->data_types_n;
    }
    s->max_colour_offset = obj->max_colour_offset;
    s->min_channels_count = obj->min_channels_count;
    s->max_channels_count = obj->max_channels_count;
    s->min_colour_count = obj->min_colour_count;
    s->max_colour_count = obj->max_colour_count;
    s->can_planar = obj->can_planar;
    s->can_interwoven = obj->can_interwoven;
    s->can_swap = obj->can_swap;
    s->can_swap_bytes = obj->can_swap_bytes;
    s->can_revert = obj->can_revert;
    s->can_premultiplied_alpha = obj->can_premultiplied_alpha;
    s->can_nonpremultiplied_alpha = obj->can_nonpremultiplied_alpha;
    s->can_subpixel = obj->can_subpixel;
    if(obj->channel_types_n)
    {
      int n = obj->channel_types_n;

      s->channel_types = allocateFunc_( n * sizeof(oyCHANNELTYPE_e) );
      error = !s->channel_types;
      if(!error)
        error = !memcpy( s->channel_types, obj->channel_types,
                         n * sizeof(oyCHANNELTYPE_e) );
      if(error <= 0)
        s->channel_types_n = n;
    }
    s->is_mandatory = obj->is_mandatory;
  }

  if(error)
    oyConnectorImaging_Release( &s );

  return s;
}

/** Function oyConnectorImaging_Copy
 *  @memberof oyConnector_s
 *  @brief   copy or reference a Connector object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/27 (Oyranos: 0.1.8)
 *  @date    2008/07/27
 */
OYAPI oyConnectorImaging_s * OYEXPORT
             oyConnectorImaging_Copy ( oyConnectorImaging_s * obj,
                                       oyObject_s          object )
{
  oyConnectorImaging_s * s = 0;

  if(!obj)
    return s;

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyConnectorImaging_Copy_( obj, object );

  return s;
}
 
/** Function oyConnectorImaging_Release
 *  @memberof oyConnector_s
 *  @brief   release and possibly deallocate a Connector object
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/27 (Oyranos: 0.1.8)
 *  @date    2008/07/27
 */
OYAPI int  OYEXPORT
           oyConnectorImaging_Release( oyConnectorImaging_s ** obj )
{
  /* ---- start of common object destructor ----- */
  oyConnectorImaging_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  oyCheckType__m( oyOBJECT_CONNECTOR_S, return 1 )

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    if(s->name.nick) deallocateFunc( s->name.nick );
    if(s->name.name) deallocateFunc( s->name.name );
    if(s->name.description) deallocateFunc( s->name.description );

    if(s->data_types)
      deallocateFunc( s->data_types ); s->data_types = 0;
    s->data_types_n = 0;

    if(s->channel_types)
      deallocateFunc( s->channel_types ); s->channel_types = 0;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}

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
  int x = 0, y = 0, n = 0;
  int result = 0, error = 0;
  int is_allocated = 0;
  oyPointer * ptr = 0;
  oyFilterSocket_s * socket = requestor_plug->remote_socket_;
  oyImage_s * image = (oyImage_s*)socket->data;

  /* Do not work on non existent data. */
  if(!image || !ticket->output_image)
    return result;

  /* Set a unknown output image dimension to something appropriate. */
  if(!ticket->output_image->width && !ticket->output_image->height)
  {
    ticket->output_image->width = image->width;
    ticket->output_image->height = image->height;
    oyImage_SetCritical( ticket->output_image, image->layout_[0], 0, 0 );
  }

  x = ticket->start_xy[0];
  y = ticket->start_xy[1];

  result = oyPixelAccess_CalculateNextStartPixel( ticket, requestor_plug);

  if(result != 0)
    return result;

  if(x < image->width &&
     y < image->height &&
     ticket->pixels_n)
  {
    n = ticket->pixels_n;
    if(n == 1)
      ptr = image->getPoint( image, x, y, 0, &is_allocated );

    result = !ptr;

  } else {

    /* adapt the rectangle of interesst to the new image dimensions */
    oyRectangle_s new_roi = {oyOBJECT_RECTANGLE_S,0,0,0};
    double correct = ticket->output_image->width / (double) image->width;
    oyRectangle_SetByRectangle( &new_roi, ticket->output_image_roi );
    /* x and y offset */
    new_roi.x += x / (double) image->width;
    new_roi.y += y / (double) image->width;
    new_roi.width *= correct;
    new_roi.height *= correct;
    error = oyImage_FillArray( image, &new_roi, 1, &ticket->array, 0, 0 );
    if(error)
      result = error;
  }

  return result;
}

/* private definition */
int      oyMessageFunc_              ( int                 code,
                                       const oyStruct_s  * context,
                                       const char        * format,
                                       ... );

int      oyFilterMessageFunc         ( int                 code,
                                       const oyStruct_s  * context,
                                       const char        * format,
                                       ... )
{
  va_list ap;
  return oyMessageFunc_( code, context, format, ap );
}


/** @func  oyCMMWarnFunc
 *  @brief message handling
 *
 *  @version Oyranos: 0.1.10
 *  @date    2008/01/02
 *  @since   2008/01/02 (Oyranos: 0.1.8)
 */
int oyCMMWarnFunc( int code, const oyStruct_s * context, const char * format, ... )
{
  char* text = (char*)calloc(sizeof(char), 4096);
  va_list list;
  const char * type_name = "";
  int id = -1;

  if(context && oyOBJECT_NONE < context->type_)
  {
    type_name = oyStructTypeToText( context->type_ );
    id = oyObject_GetId( context->oy_ );
  }

  va_start( list, format);
  vsprintf( text, format, list);
  va_end  ( list );

  switch(code)
  {
    case oyMSG_WARN:
         fprintf( stderr, "WARNING"); fprintf( stderr, ": " );
         break;
    case oyMSG_ERROR:
         fprintf( stderr, "!!! ERROR"); fprintf( stderr, ": " );
         break;
  }

  fprintf( stderr, "%s[%d] ", type_name, id );

  fprintf( stderr, "%s", text ); fprintf( stderr, "\n" );
  free( text );

  return 0;
}

