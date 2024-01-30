#include "oyranos_image_internal.h"

/**
 *  @internal
 *  Function oyImage_CombinePixelLayout2Mask_
 *  @memberof oyImage_s
 *  @brief   describe a images channel and pixel layout
 *
 *  - gather informations about the pixel layout
 *  - describe the color channels characteristic into oyImage_s::channel_layout
 *  - store some text in the images nick name as a ID
 *
 *  @version Oyranos: 0.5.0
 *  @date    2012/09/06
 *  @since   2007/11/26 (Oyranos: 0.1.8)
 */
int oyImage_CombinePixelLayout2Mask_ ( oyImage_s_        * image,
                                       oyPixel_t           pixel_layout )
{
  int n     = oyToChannels_m( pixel_layout );
  oyProfile_s * profile = image->profile_;
  int cchan_n = oyProfile_GetChannelsCount( profile );
  int coff_x = oyToColorOffset_m( pixel_layout );
  oyDATATYPE_e t = oyToDataType_m( pixel_layout );
  int swap  = oyToSwapColorChannels_m( pixel_layout );
  /*int revert= oyT_FLAVOR_M( pixel_layout );*/
  oyPixel_t *mask = image->oy_->allocateFunc_( sizeof(oyPixel_t) * (oyCHAN0 + 
                    OY_MAX(n,cchan_n) + 1));
  int error = !mask;
  int so = oyDataTypeGetSize( t );
  int w = image->width;
  int h = image->height;
  int i;
  char * text,
       * hash_text = NULL;
  oyImage_s_ * s = image;
  oyCHANNELTYPE_e * clayout = 0; /* non profile described channels */

  if(!mask) return error;
  text = oyAllocateFunc_(512);
  if(!text) { image->oy_->deallocateFunc_(mask); return 1; }

  if(!n && cchan_n)
    n = cchan_n;

  /* describe the pixel layout and access */
  if(error <= 0)
  {
    error = !memset( mask, 0, sizeof(mask) * sizeof(oyPixel_t));
    if(oyToPlanar_m( pixel_layout ))
    {
      mask[oyPOFF_X] = 1;
      mask[oyCOFF] = w*h*n;
    } else {
      mask[oyPOFF_X] = n;
      mask[oyCOFF] = 1;
    }
    mask[oyPOFF_Y] = mask[oyPOFF_X] * w;
    mask[oyDATA_SIZE] = so;
    mask[oyLAYOUT] = pixel_layout;
    mask[oyCHANS] = n;

    if(swap)
      for(i = 0; i < cchan_n; ++i)
        mask[oyCHAN0 + i] = coff_x + cchan_n - i - 1;
    else
      for(i = 0; i < cchan_n; ++i)
        mask[oyCHAN0 + i] = coff_x + i;
  }

  /* describe the channels characters */
  if(!s->channel_layout)
  {
    clayout = image->oy_->allocateFunc_( sizeof(int) * ( OY_MAX(n,cchan_n)+ 1));
    if(!clayout) { image->oy_->deallocateFunc_(mask); oyDeAllocateFunc_(text); return 1; }
    /* we dont know about the content */
    for(i = 0; i < n; ++i)
      clayout[i] = oyCHANNELTYPE_OTHER;
    /* describe profile colors */
    for(i = coff_x; i < coff_x + cchan_n; ++i)
      clayout[i] = oyICCColorSpaceToChannelLayout( 
                     oyProfile_GetSignature(profile, oySIGNATURE_COLOR_SPACE),
                     i - coff_x );
    /* place a end marker */
    clayout[n] = oyCHANNELTYPE_UNDEFINED;
      s->channel_layout = clayout;
  }

  /* describe the image */
  oySprintf_( text, 
                  "{ \"oyImage_s\": { \"id\": \"%d\", \"width\": \"%d\", \"height\": \"%d\", \"resolution\": [\"%.02f\", \"%.02f\"],\n",
                  oyObject_GetId(image->oy_),
                  image->width,
                  image->height,
                  image->resolution_x,
                  image->resolution_y);
  hashTextAdd_m( text );
  i = strlen(text);
  if(profile)
    oySprintf_(text, "\"icc_profile\": %s\n", oyjlTermColorToPlain(oyProfile_GetText(profile, oyNAME_JSON), 0));
  if(text[16] == '\n') text[16] = ' ';
  i = strlen(text);
  if(text[i-3] == '\n') text[i-3] = ' ';
  if(text[i-1] == '\n') text[i-1] = ',';
  hashTextAdd_m( text );
  oySprintf_( text, "\n \"channels\": { \"all\": \"%d\", \"color\": \"%d\" }, ", n, cchan_n );
  hashTextAdd_m( text );
  oySprintf_( text,
              "\"offsets\": { \"first_color_sample\": \"%d\", \"next_pixel\": \"%d\"}"
              /*"  next line = %d\n"*/,
              coff_x, mask[oyPOFF_X]/*, mask[oyPOFF_Y]*/ );
  hashTextAdd_m( text );

  if(swap || oyToByteswap_m( pixel_layout ))
  {
    hashTextAdd_m( ", \"swap\": {" );
    if(swap)
    {
      if(oyToByteswap_m( pixel_layout ))
        hashTextAdd_m( " \"colorswap\": \"yes\"," );
      else
        hashTextAdd_m( " \"colorswap\": \"yes\"" );
    }
    if( oyToByteswap_m( pixel_layout ) )
      hashTextAdd_m( " \"byteswap\": \"yes\"" );
    hashTextAdd_m( " }" );
  }

  if( oyToFlavor_m( pixel_layout ) )
  {
    oySprintf_( text, ", \"flawor value\": \"yes\"" );
    hashTextAdd_m( text );
  }
  oySprintf_( text, ",\n\"sample_type\": { \"value\": \"%s\", \"byte\": \"%d\" }",
                    oyDataTypeToText(t), so );
  hashTextAdd_m( text );
  oySprintf_( text, "}}");
  hashTextAdd_m( text );

  if(error <= 0)
    error = oyObject_SetName( s->oy_, hash_text, oyNAME_NICK );


  oyDeAllocateFunc_(text);
  oySTRUCT_FREE_m(s, hash_text);

  if(s->oy_->deallocateFunc_)
  {
    if(s->layout_)
      s->oy_->deallocateFunc_(s->layout_);
    s->layout_ = 0;
  }
  s->layout_ = mask;


  return 0;
}
