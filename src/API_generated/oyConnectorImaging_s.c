/** @file oyConnectorImaging_s.c

   [Template file inheritance graph]
   +-> oyConnectorImaging_s.template.c
   |
   +-> oyConnector_s.template.c
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


  
#include "oyConnectorImaging_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"


#include "oyConnectorImaging_s_.h"

#ifdef oyCheckCType__m
#undef oyCheckCType__m
#endif
#define oyCheckCType__m( type, action ) \
  if( !s || !(oyOBJECT_CONNECTOR_S <= s->type_ && s->type_ < oyOBJECT_CONNECTOR_MAX_S)) \
  { \
    WARNc3_S( "%s %s(%s)", _("Unexpected object type:"), \
              oyStructTypeToText( s ? s->type_ : oyOBJECT_NONE ), \
              oyStructTypeToText( type )) \
    action; \
  }
  


/** Function oyConnectorImaging_New
 *  @memberof oyConnectorImaging_s
 *  @brief   allocate a new ConnectorImaging object
 */
OYAPI oyConnectorImaging_s * OYEXPORT
  oyConnectorImaging_New( oyObject_s object )
{
  oyObject_s s = object;
  oyConnectorImaging_s_ * connectorimaging = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  connectorimaging = oyConnectorImaging_New_( s );

  return (oyConnectorImaging_s*) connectorimaging;
}

/** Function  oyConnectorImaging_Copy
 *  @memberof oyConnectorImaging_s
 *  @brief    Copy or Reference a ConnectorImaging object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     connectorimaging                 ConnectorImaging struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyConnectorImaging_s* OYEXPORT
  oyConnectorImaging_Copy( oyConnectorImaging_s *connectorimaging, oyObject_s object )
{
  oyConnectorImaging_s_ * s = (oyConnectorImaging_s_*) connectorimaging;

  if(s)
  {
    oyCheckCType__m( oyOBJECT_CONNECTOR_IMAGING_S, return NULL )
  }
  else
    return NULL;

  s = oyConnectorImaging_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyConnectorImaging_s" );

  return (oyConnectorImaging_s*) s;
}
 
/** Function oyConnectorImaging_Release
 *  @memberof oyConnectorImaging_s
 *  @brief   release and possibly deallocate a oyConnectorImaging_s object
 *
 *  @param[in,out] connectorimaging                 ConnectorImaging struct object
 */
OYAPI int OYEXPORT
  oyConnectorImaging_Release( oyConnectorImaging_s **connectorimaging )
{
  oyConnectorImaging_s_ * s = 0;

  if(!connectorimaging || !*connectorimaging)
    return 0;

  s = (oyConnectorImaging_s_*) *connectorimaging;

  oyCheckCType__m( oyOBJECT_CONNECTOR_IMAGING_S, return 1 )

  *connectorimaging = 0;

  return oyConnectorImaging_Release_( &s );
}




/* Include "ConnectorImaging.public_methods_definitions.c" { */
/** Function  oyConnectorImaging_SetDataTypes
 *  @memberof oyConnectorImaging_s
 *  @brief    Set data types capability of oyConnectorImaging_s
 *
 *  @param         c                   the image connector
 *  @param         data_types          array of supported data types, const
 *  @param         data_types_n        number of elements in data_types
 *  @return                            error
 *
 *  @version  Oyranos: 0.9.5
 *  @date     2013/06/14
 *  @since    2013/06/14 (Oyranos: 0.9.5)
 */
OYAPI int  OYEXPORT
                 oyConnectorImaging_SetDataTypes (
                                       oyConnectorImaging_s  * c,
                                       oyDATATYPE_e          * data_types,
                                       int                     data_types_n )
{
  oyConnectorImaging_s_ * s = (oyConnectorImaging_s_*)c;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_CONNECTOR_IMAGING_S, return 1 )

  s->data_types = (oyDATATYPE_e*) oyjlStringAppendN( NULL, (const char*)data_types, sizeof(oyDATATYPE_e) * data_types_n, 0 );
  s->data_types_n = data_types_n;

  return 0;
}

/** Function  oyConnectorImaging_GetDataTypes
 *  @memberof oyConnectorImaging_s
 *  @brief    Get capabilities of oyConnectorImaging_s
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/06
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
OYAPI int  OYEXPORT
                 oyConnectorImaging_GetDataTypes (
                                       oyConnectorImaging_s  * c,
                                       const oyDATATYPE_e   ** data_types,
                                       int                   * data_types_n )
{
  oyConnectorImaging_s_ * s = (oyConnectorImaging_s_*)c;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_CONNECTOR_IMAGING_S, return 1 )

  if(data_types)
    *data_types = s->data_types;
  if(data_types_n)
    *data_types_n = s->data_types_n;

  return 0;
}
/** Function  oyConnectorImaging_GetChannelTypes
 *  @memberof oyConnectorImaging_s
 *  @brief    Get capabilities of oyConnectorImaging_s
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/06
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
OYAPI int  OYEXPORT
                 oyConnectorImaging_GetChannelTypes (
                                       oyConnectorImaging_s  * c,
                                       const oyCHANNELTYPE_e** channel_types,
                                       int                   * channel_types_n )
{
  oyConnectorImaging_s_ * s = (oyConnectorImaging_s_*)c;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_CONNECTOR_IMAGING_S, return 1 )

  if(channel_types)
    *channel_types = s->channel_types;
  if(channel_types_n)
    *channel_types_n = s->channel_types_n;

  return 0;
}

/** Function  oyConnectorImaging_GetDataTypes
 *  @memberof oyConnectorImaging_s
 *  @brief    Get capabilities of oyConnectorImaging_s
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/06
 *  @since    2012/09/06 (Oyranos: 0.5.0)
 */
OYAPI int  OYEXPORT
                 oyConnectorImaging_GetCapability (
                                       oyConnectorImaging_s    * c,
                                       oyCONNECTOR_IMAGING_CAP_e type )
{
  oyConnectorImaging_s_ * s = (oyConnectorImaging_s_*)c;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_CONNECTOR_IMAGING_S, return 0 )

  switch(type)
  {
    case oyCONNECTOR_IMAGING_CAP_IS_PLUG: return s->is_plug;
    case oyCONNECTOR_IMAGING_CAP_MAX_COLOR_OFFSET: return s->max_color_offset;
    case oyCONNECTOR_IMAGING_CAP_MIN_CHANNELS_COUNT: return s->min_channels_count;
    case oyCONNECTOR_IMAGING_CAP_MAX_CHANNELS_COUNT: return s->max_channels_count;
    case oyCONNECTOR_IMAGING_CAP_MIN_COLOR_COUNT: return s->min_color_count;
    case oyCONNECTOR_IMAGING_CAP_MAX_COLOR_COUNT: return s->max_color_count;
    case oyCONNECTOR_IMAGING_CAP_CAN_PLANAR: return s->can_planar;
    case oyCONNECTOR_IMAGING_CAP_CAN_INTERWOVEN: return s->can_interwoven;
    case oyCONNECTOR_IMAGING_CAP_CAN_SWAP: return s->can_swap;
    case oyCONNECTOR_IMAGING_CAP_CAN_SWAP_BYTES: return s->can_swap_bytes;
    case oyCONNECTOR_IMAGING_CAP_CAN_REVERT: return s->can_revert;
    case oyCONNECTOR_IMAGING_CAP_CAN_PREMULTIPLIED_ALPHA: return s->can_premultiplied_alpha;
    case oyCONNECTOR_IMAGING_CAP_CAN_NONPREMULTIPLIED_ALPHA: return s->can_nonpremultiplied_alpha;
    case oyCONNECTOR_IMAGING_CAP_CAN_SUBPIXEL: return s->can_subpixel;
    case oyCONNECTOR_IMAGING_CAP_ID: return s->id;
    case oyCONNECTOR_IMAGING_CAP_IS_MANDATORY: return s->is_mandatory;
    default: return 0; 
  }
  return 0;
}

/** Function  oyConnectorImaging_SetCapability
 *  @memberof oyConnectorImaging_s
 *  @brief    Set capabilities of oyConnectorImaging_s
 *
 *  @version  Oyranos: 0.9.5
 *  @date     2013/06/14
 *  @since    2013/06/14 (Oyranos: 0.9.5)
 */
OYAPI int  OYEXPORT
                 oyConnectorImaging_SetCapability (
                                       oyConnectorImaging_s    * c,
                                       oyCONNECTOR_IMAGING_CAP_e type,
                                       int                       value )
{
  oyConnectorImaging_s_ * s = (oyConnectorImaging_s_*)c;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_CONNECTOR_IMAGING_S, return 1 )

  switch(type)
  {
    case oyCONNECTOR_IMAGING_CAP_IS_PLUG: s->is_plug = value; break;
    case oyCONNECTOR_IMAGING_CAP_MAX_COLOR_OFFSET:  s->max_color_offset = value; break;
    case oyCONNECTOR_IMAGING_CAP_MIN_CHANNELS_COUNT:  s->min_channels_count = value; break;
    case oyCONNECTOR_IMAGING_CAP_MAX_CHANNELS_COUNT:  s->max_channels_count = value; break;
    case oyCONNECTOR_IMAGING_CAP_MIN_COLOR_COUNT:  s->min_color_count = value; break;
    case oyCONNECTOR_IMAGING_CAP_MAX_COLOR_COUNT:  s->max_color_count = value; break;
    case oyCONNECTOR_IMAGING_CAP_CAN_PLANAR:  s->can_planar = value; break;
    case oyCONNECTOR_IMAGING_CAP_CAN_INTERWOVEN:  s->can_interwoven = value; break;
    case oyCONNECTOR_IMAGING_CAP_CAN_SWAP:  s->can_swap = value; break;
    case oyCONNECTOR_IMAGING_CAP_CAN_SWAP_BYTES:  s->can_swap_bytes = value; break;
    case oyCONNECTOR_IMAGING_CAP_CAN_REVERT:  s->can_revert = value; break;
    case oyCONNECTOR_IMAGING_CAP_CAN_PREMULTIPLIED_ALPHA:  s->can_premultiplied_alpha = value; break;
    case oyCONNECTOR_IMAGING_CAP_CAN_NONPREMULTIPLIED_ALPHA:  s->can_nonpremultiplied_alpha = value; break;
    case oyCONNECTOR_IMAGING_CAP_CAN_SUBPIXEL:  s->can_subpixel = value; break;
    case oyCONNECTOR_IMAGING_CAP_ID:  s->id = value; break;
    case oyCONNECTOR_IMAGING_CAP_IS_MANDATORY:  s->is_mandatory = value; break;
    default: return 1;
  }
  return 0;
}

/* } Include "ConnectorImaging.public_methods_definitions.c" */

#define oyCheckConnectorImagingType__m( type, action ) \
if(!(oyOBJECT_CONNECTOR_S <= s->type_ && s->type_ < oyOBJECT_CONNECTOR_MAX_S)) \
    { action; }

/** Function  oyConnectorImaging_SetTexts
 *  @memberof oyConnectorImaging_s
 *  @brief    set the texts in a connector
 *
 *  Set UI strings.
 *
 *  @param[in]     obj                 ConnectorImaging object
 *  @param[in]     getText             the name function
 *  @param[in]     text_classes        zero terminated list of classes, 
 *                                     e.g. {"name",NULL}
 *                                     owned by the user, but has to live the
 *                                     lifetime of the object
 *  @return                            status
 *
 *  @version  Oyranos: 0.9.5
 *  @date     2013/06/10
 *  @since    2013/06/10 (Oyranos: 0.9.5)
 */
OYAPI int  OYEXPORT
                 oyConnectorImaging_SetTexts( oyConnectorImaging_s     * obj,
                                       oyCMMGetText_f      getText,
                                       const char       ** text_classes )
{
  oyConnectorImaging_s_ * s = (oyConnectorImaging_s_*) obj;

  if(!obj)
    return 1;

  oyCheckConnectorImagingType__m( oyOBJECT_CONNECTOR_S, return 1 )

  s->getText = getText;
  s->texts = text_classes;

  return 0;
}

/** Function  oyConnectorImaging_GetTexts
 *  @memberof oyConnectorImaging_s
 *  @brief    get the text classes in a connector
 *
 *  Set UI strings.
 *
 *  @param[in]     obj                 ConnectorImaging object
 *  @return        text_classes        zero terminated list of classes, 
 *                                     e.g. {"name",NULL}
 *
 *  @version  Oyranos: 0.9.5
 *  @date     2013/06/10
 *  @since    2013/06/10 (Oyranos: 0.9.5)
 */
OYAPI const char **  OYEXPORT
                 oyConnectorImaging_GetTexts( oyConnectorImaging_s     * obj )
{
  oyConnectorImaging_s_ * s = (oyConnectorImaging_s_*) obj;

  if(!obj)
    return 0;

  oyCheckConnectorImagingType__m( oyOBJECT_CONNECTOR_S, return 0 )

  return s->texts;
}

/** Function  oyConnectorImaging_GetText
 *  @memberof oyConnectorImaging_s
 *  @brief    get the names in a connector
 *
 *  Get UI strings.
 *
 *  @param[in]     obj                 ConnectorImaging object
 *  @param[in]     name_class          the names class, e.g. "name", "help" ...
 *  @param[in]     type                the names type
 *  @return                            the name string
 *
 *  @version  Oyranos: 0.9.5
 *  @since    2011/01/31 (Oyranos: 0.3.0)
 *  @date     2013/06/10
 */
OYAPI const char *  OYEXPORT
                 oyConnectorImaging_GetText ( oyConnectorImaging_s     * obj,
                                       const char        * name_class,
                                       oyNAME_e            type )
{
  oyConnectorImaging_s_ * s = (oyConnectorImaging_s_*) obj;
  const char * string = 0;

  if(!obj)
    return 0;

  oyCheckConnectorImagingType__m( oyOBJECT_CONNECTOR_S, return 0 )

  string = s->getText( name_class, type, (oyStruct_s*)s );

  return string;
}

/** Function  oyConnectorImaging_IsPlug
 *  @memberof oyConnectorImaging_s
 *  @brief    Is this connector a plug or a socket
 *
 *  @param[in]     obj                 ConnectorImaging object
 *  @return                            boolean; 0 - socket; 1 - plug
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/31 (Oyranos: 0.3.0)
 *  @date    2011/01/31
 */
int              oyConnectorImaging_IsPlug  ( oyConnectorImaging_s     * obj )
{
  oyConnectorImaging_s_ * s = (oyConnectorImaging_s_*)obj;

  if(!obj)
    return 0;

  oyCheckConnectorImagingType__m( oyOBJECT_CONNECTOR_S, return 0 )

  return s->is_plug;
}

/** Function  oyConnectorImaging_SetIsPlug
 *  @memberof oyConnectorImaging_s
 *  @brief    Set this connector as a plug or a socket
 *
 *  @param[in,out] obj                 ConnectorImaging object
 *  @param[in]     is_plug             boolean; 0 - socket; 1 - plug
 *  @return                            1 - error; 0 - success; -1 - otherwise
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/31 (Oyranos: 0.3.0)
 *  @date    2011/01/31
 */
int              oyConnectorImaging_SetIsPlug(oyConnectorImaging_s     * obj,
                                       int                 is_plug )
{
  oyConnectorImaging_s_ * s = (oyConnectorImaging_s_*)obj;

  if(!obj)
    return 0;

  oyCheckConnectorImagingType__m( oyOBJECT_CONNECTOR_S, return 1 )

  s->is_plug = is_plug;

  return 0;
}

/** Function  oyConnectorImaging_GetReg
 *  @memberof oyConnectorImaging_s
 *  @brief    Get the registration for the connection type
 *
 *  This is use as a rough check, if connections are possible.
 *
 *  @param[in]     obj                 ConnectorImaging object
 *  @return                            registration string
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/31 (Oyranos: 0.3.0)
 *  @date    2011/01/31
 */
const char *     oyConnectorImaging_GetReg  ( oyConnectorImaging_s     * obj )
{
  oyConnectorImaging_s_ * s = (oyConnectorImaging_s_*)obj;

  if(!obj)
    return 0;

  oyCheckConnectorImagingType__m( oyOBJECT_CONNECTOR_S, return 0 )

  return s->connector_type;
}

/** Function  oyConnectorImaging_SetReg
 *  @memberof oyConnectorImaging_s
 *  @brief    Set this connectors type string
 *
 *  This is use as a rough check, if connections are possible.
 *
 *  @param[in,out] obj                 ConnectorImaging object
 *  @param[in]     type_registration   the registration string to describe the
 *                                     type
 *  @return                            1 - error; 0 - success; -1 - otherwise
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/31 (Oyranos: 0.3.0)
 *  @date    2011/01/31
 */
int              oyConnectorImaging_SetReg  ( oyConnectorImaging_s     * obj,
                                       const char        * type_registration )
{
  oyConnectorImaging_s_ * s = (oyConnectorImaging_s_*)obj;

  if(!obj)
    return 0;

  oyCheckConnectorImagingType__m( oyOBJECT_CONNECTOR_S, return 1 )

  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;
    oyAlloc_f allocateFunc = s->oy_->allocateFunc_;

    if(s->connector_type)
    { deallocateFunc( s->connector_type ); s->connector_type = 0; }

    s->connector_type = oyStringCopy_( type_registration, allocateFunc );
  }

  return 0;
}

/** Function  oyConnectorImaging_SetMatch
 *  @memberof oyConnectorImaging_s
 *  @brief    Set this connectors type check function
 *
 *  This is use as a check, if connections are possible.
 *  This allowes for a more fine grained control than the type registration.
 *
 *  @param[in,out] obj                 ConnectorImaging object
 *  @param[in]     func                the check function
 *  @return                            1 - error; 0 - success; -1 - otherwise
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/31 (Oyranos: 0.3.0)
 *  @date    2011/01/31
 */
int              oyConnectorImaging_SetMatch( oyConnectorImaging_s     * obj,
                                       oyCMMFilterSocket_MatchPlug_f func )
{
  oyConnectorImaging_s_ * s = (oyConnectorImaging_s_*)obj;

  if(!obj)
    return 0;

  oyCheckConnectorImagingType__m( oyOBJECT_CONNECTOR_S, return 1 )

  s->filterSocket_MatchPlug = func;

  return 0;
}

/** Function  oyConnectorImaging_GetMatch
 *  @memberof oyConnectorImaging_s
 *  @brief    Set this connectors type check function
 *
 *  This is use as a check, if connections are possible.
 *  This allowes for a more fine grained control than the type registration.
 *
 *  @param[in]     obj                 ConnectorImaging object
 *  @return                            the check function
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/31 (Oyranos: 0.3.0)
 *  @date    2011/01/31
 */
oyCMMFilterSocket_MatchPlug_f oyConnectorImaging_GetMatch (
                                       oyConnectorImaging_s     * obj )
{
  oyConnectorImaging_s_ * s = (oyConnectorImaging_s_*)obj;

  if(!obj)
    return 0;

  oyCheckConnectorImagingType__m( oyOBJECT_CONNECTOR_S, return 0 )

  return s->filterSocket_MatchPlug;
}

