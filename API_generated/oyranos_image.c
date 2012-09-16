/** @file oyranos_image.c

   [Template file inheritance graph]
   +-- oyranos_image.template.c

 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2004-2012 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @date     2012/09/15
 */


#include "oyranos_debug.h"
#include "oyranos_sentinel.h"

#include "oyranos_image.h"
#include "oyranos_image_internal.h"

/** \addtogroup objects_image Image API
 *
 *  @{
 */


char *         oyPixelLayoutPrint_   ( oyPixel_t           pixel_layout )
{
  return oyPixelPrint(pixel_layout, oyAllocateFunc_);
}

char   *           oyPixelPrint      ( oyPixel_t           pixel_layout,
                                       oyAlloc_f           allocateFunc )
{
  oyDATATYPE_e t = oyToDataType_m( pixel_layout );
  char * text = 0;

#define oyPixelLayoutPrint_FORMAT "channels: %d channel_offset: %d sample_type[%dByte]: %s planar: %d byte_swap %d colour_swap: %d flawor: %d"
#define oyPixelLayoutPrint_ARGS \
  oyToChannels_m( pixel_layout ), \
  oyToColourOffset_m( pixel_layout ), \
  oySizeofDatatype( t ), \
  oyDatatypeToText(t), \
  oyToPlanar_m( pixel_layout ), \
  oyToByteswap_m( pixel_layout), \
  oyToSwapColourChannels_m( pixel_layout ), \
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
