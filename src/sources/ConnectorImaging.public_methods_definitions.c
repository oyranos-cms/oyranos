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
