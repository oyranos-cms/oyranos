{% include "source_file_header.txt" %}

#include "oyranos_debug.h"
#include "oyranos_sentinel.h"
#include <oyranos_string.h>

#include "oyranos_image.h"
#include "oyranos_image_internal.h"

/** \addtogroup objects_image
 *
 *  @{ *//* objects_image */


char *         oyPixelLayoutPrint_   ( oyPixel_t           pixel_layout )
{
  return oyPixelPrint(pixel_layout, oyAllocateFunc_);
}

char   *           oyPixelPrint      ( oyPixel_t           pixel_layout,
                                       oyAlloc_f           allocateFunc )
{
  oyDATATYPE_e t = oyToDataType_m( pixel_layout );
  char * text = 0;

#define oyPixelLayoutPrint_FORMAT "channels: %d channel_offset: %d sample_type[%luByte]: %s planar: %d byte_swap %d color_swap: %d flawor: %d"
#define oyPixelLayoutPrint_ARGS \
  oyToChannels_m( pixel_layout ), \
  oyToColorOffset_m( pixel_layout ), \
  oyDataTypeGetSize( t ), \
  oyDataTypeToText(t), \
  oyToPlanar_m( pixel_layout ), \
  oyToByteswap_m( pixel_layout), \
  oyToSwapColorChannels_m( pixel_layout ), \
  oyToFlavor_m( pixel_layout )

  /* describe the pixel layout and access */
  oyStringAddPrintf_(&text, allocateFunc, 0, 
                     oyPixelLayoutPrint_FORMAT, oyPixelLayoutPrint_ARGS);

  /*printf(oyPixelLayoutPrint_FORMAT,oyPixelLayoutPrint_ARGS);*/

#undef oyPixelLayoutPrint_FORMAT
#undef oyPixelLayoutPrint_ARGS
  return text;
}



/** @} *//* objects_image */ 
