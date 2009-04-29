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

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyConnectorImaging_Copy;
  s->release = (oyStruct_Release_f) oyConnectorImaging_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
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


