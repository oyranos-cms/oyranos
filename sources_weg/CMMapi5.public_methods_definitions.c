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
  int coff = 0;
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
      coff = oyToColourOffset_m( image->layout_[oyLAYOUT] );

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
