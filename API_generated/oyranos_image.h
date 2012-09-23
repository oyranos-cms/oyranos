
/** @file oyranos_image.h

   [Template file inheritance graph]
   +-- oyranos_image.template.h

 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2004-2012 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @date     2012/09/23
 */


#ifndef OYRANOS_IMAGE_H
#define OYRANOS_IMAGE_H

#include <oyranos_helper.h>
#include <oyranos_icc.h>
#include <oyranos_string.h>
#include <oyranos_types.h>

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


/** @enum    oyDATATYPE_e
 *  @brief   basic data types for anonymous pointers
 *  @ingroup objects_image
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/00/00 (Oyranos: 0.1.8)
 *  @date    2007/00/00
 */
typedef enum oyDATATYPE_e {
  oyUINT8,     /*!<  8-bit integer */
  oyUINT16,    /*!< 16-bit integer */
  oyUINT32,    /*!< 32-bit integer */
  oyHALF,      /*!< 16-bit floating point number */
  oyFLOAT,     /*!< IEEE floating point number */
  oyDOUBLE     /*!< IEEE double precission floating point number */
} oyDATATYPE_e;

/** @enum    oyCHANNELTYPE_e
 *  @brief   channels types
 *  @ingroup objects_image
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/00/00 (Oyranos: 0.1.8)
 *  @date    2009/08/03
 */
typedef enum oyCHANNELTYPE_e {
  oyCHANNELTYPE_UNDEFINED,            /**< as well for the list end */
  oyCHANNELTYPE_OTHER,
  oyCHANNELTYPE_LIGHTNESS,
  oyCHANNELTYPE_COLOUR,
  oyCHANNELTYPE_COLOUR_LIGHTNESS,
  oyCHANNELTYPE_COLOUR_LIGHTNESS_PREMULTIPLIED,
  oyCHANNELTYPE_DEPTH,
  oyCHANNELTYPE_ALPHA1,
  oyCHANNELTYPE_ALPHA2,
  oyCHANNELTYPE_ALPHA3,
  oyCHANNELTYPE_U,
  oyCHANNELTYPE_V
} oyCHANNELTYPE_e;

oyCHANNELTYPE_e oyICCColourSpaceToChannelLayout (
                                       icColorSpaceSignature sig,
                                       int                 pos );


/** @enum    oyPIXEL_ACCESS_TYPE_e
 *  @brief   pixel access types
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/00/00 (Oyranos: 0.1.8)
 *  @date    2008/00/00
 */
typedef enum {
  oyPIXEL_ACCESS_IMAGE,
  oyPIXEL_ACCESS_POINT,                /**< dont use */
  oyPIXEL_ACCESS_LINE                  /**< dont use */
} oyPIXEL_ACCESS_TYPE_e;

/** @typedef oyPixel_t
 *  @ingroup objects_image
    parametric type as shorthand for the channel layout in bitmaps \n

    should fit into a 32bit type, usual unsigned int or uint32_t \n

    X F P S TTTT OOOOOOOO CCCCCCCC \n

    C  channels count per pixel (3 for RGB); max 255 \n
    O  colour channel offset (0 for RGB, 1 for ARGB) \n
    P  Planar bit: 0 - interwoven, 1 - one channel after the other \n
    S  Swap colour channels bit (BGR) \n
    T  Type oyDATATYPE_e \n
    X  non host byte order bit \n
    F  Revert bit: 0 - MinIsBlack(Chocolate) 1 - MinIsWhite(Vanilla); \n
       exchange min and max : (1-x)

 */
typedef uint32_t oyPixel_t;

char   *           oyPixelPrint      ( oyPixel_t           pixel_layout,
                                       oyAlloc_f           allocateFunc );


/** pixel mask description 
 */
typedef enum {
  oyLAYOUT = 0, /* remembering the layout */
  oyPOFF_X,     /* pixel count x offset */
  oyPOFF_Y,     /* pixel count y offset */
  oyCOFF,       /* channel offset */
  oyDATA_SIZE,  /* sample size in byte */
  oyCHANS,      /* number of channels */
  oyCHAN0       /* first colour channel */
} oyLAYOUT_e;

#define oyChannels_m(c)             (c)
#define oyColourOffset_m(o)         ((o) << 8)
#define oyDataType_m(t)             ((t) << 16)
#define oySwapColourChannels_m(s)   ((s) << 20)
#define oyPlanar_m(p)               ((p) << 21)
#define oyFlavor_m(p)               ((f) << 22)
#define oyByteSwap_m(x)             ((x) << 23)

/** define some common types */
#define OY_TYPE_123_8       (oyChannels_m(3)|oyDataType_m(oyUINT8))
#define OY_TYPE_123_16      (oyChannels_m(3)|oyDataType_m(oyUINT16))
#define OY_TYPE_123_HALF    (oyChannels_m(3)|oyDataType_m(oyHALF))
#define OY_TYPE_123_FLOAT   (oyChannels_m(3)|oyDataType_m(oyFLOAT))
#define OY_TYPE_123_DBL     (oyChannels_m(3)|oyDataType_m(oyDOUBLE))

#define OY_TYPE_123A_8      (oyChannels_m(4)|oyDataType_m(oyUINT8))
#define OY_TYPE_123A_16     (oyChannels_m(4)|oyDataType_m(oyUINT16))
#define OY_TYPE_123A_HALF   (oyChannels_m(4)|oyDataType_m(oyHALF))
#define OY_TYPE_123A_FLOAT  (oyChannels_m(4)|oyDataType_m(oyFLOAT))
#define OY_TYPE_123A_DBL    (oyChannels_m(4)|oyDataType_m(oyDOUBLE))

#define OY_TYPE_123AZ_HALF  (oyChannels_m(5)|oyDataType_m(oyHALF))
#define OY_TYPE_123AZ_FLOAT (oyChannels_m(5)|oyDataType_m(oyFLOAT))
#define OY_TYPE_123AZ_DBL   (oyChannels_m(5)|oyDataType_m(oyDOUBLE))

#define OY_TYPE_A123_8      (oyChannels_m(4)|oyColourOffset_m(1)|oyDataType_m(oyUINT8))
#define OY_TYPE_A123_16     (oyChannels_m(4)|oyColourOffset_m(1)|oyDataType_m(oyUINT16))

#define OY_TYPE_A321_8      (oyChannels_m(4)|oyColourOffset_m(1)|oyDataType_m(oyUINT8)|oySwapColourChannels_m(oyYES))
#define OY_TYPE_A321_16     (oyChannels_m(4)|oyColourOffset_m(1)|oyDataType_m(oyUINT16)|oySwapColourChannels_m(oyYES))

/* some intermixed types will work as well */
#define OY_TYPE_123A_HALF_Z_FLOAT (oyChannels_m(4+1*2)|oyDataType_m(oyHALF))

#define OY_TYPE_1234_8      (oyChannels_m(4)|oyDataType_m(oyUINT8))
#define OY_TYPE_1234A_8     (oyChannels_m(5)|oyDataType_m(oyUINT8))
#define OY_TYPE_1234_16     (oyChannels_m(4)|oyDataType_m(oyUINT16))
#define OY_TYPE_1234A_16    (oyChannels_m(5)|oyDataType_m(oyUINT16))

#define OY_TYPE_1234_8_REV  (oyChannels_m(4)|oyDataType_m(oyUINT8)|oyFlavor_m(oyYes))
#define OY_TYPE_1234A_8_REV (oyChannels_m(5)|oyDataType_m(oyUINT8)|oyFlavor_m(oyYes))
#define OY_TYPE_1234_16_REV (oyChannels_m(4)|oyDataType_m(oyUINT16)|oyFlavor_m(oyYes))
#define OY_TYPE_1234A_16_REV (oyChannels_m(5)|oyDataType_m(oyUINT16)|oyFlavor_m(oyYes))


#define OY_TYPE_1_8         (oyChannels_m(1)|oyDataType_m(oyUINT8))
#define OY_TYPE_1_16        (oyChannels_m(1)|oyDataType_m(oyUINT16))
#define OY_TYPE_1_HALF      (oyChannels_m(1)|oyDataType_m(oyHALF))
#define OY_TYPE_1_FLOAT     (oyChannels_m(1)|oyDataType_m(oyFLOAT))
#define OY_TYPE_1A_8        (oyChannels_m(2)|oyDataType_m(oyUINT8))
#define OY_TYPE_1A_16       (oyChannels_m(2)|oyDataType_m(oyUINT16))
#define OY_TYPE_1A_HALF     (oyChannels_m(2)|oyDataType_m(oyHALF))
#define OY_TYPE_1A_FLOAT    (oyChannels_m(2)|oyDataType_m(oyFLOAT))

#define OY_TYPE_1_8_REV     (oyChannels_m(1)|oyDataType_m(oyUINT8)|oyFlavor_m(oyYes))
#define OY_TYPE_1_16_REV    (oyChannels_m(1)|oyDataType_m(oyUINT16)|oyFlavor_m(oyYes))
#define OY_TYPE_1_HALF_REV  (oyChannels_m(1)|oyDataType_m(oyHALF)|oyFlavor_m(oyYes))
#define OY_TYPE_1_FLOAT_REV (oyChannels_m(1)|oyDataType_m(oyFLOAT))|oyFlavor_m(oyYes)
#define OY_TYPE_1A_8_REV    (oyChannels_m(2)|oyDataType_m(oyUINT8)|oyFlavor_m(oyYes))
#define OY_TYPE_1A_16_REV   (oyChannels_m(2)|oyDataType_m(oyUINT16)|oyFlavor_m(oyYes))
#define OY_TYPE_1A_HALF_REV (oyChannels_m(2)|oyDataType_m(oyHALF)|oyFlavor_m(oyYes))
#define OY_TYPE_1A_FLOAT_REV (oyChannels_m(2)|oyDataType_m(oyFLOAT)|oyFlavor_m(oyYes))

/* decode */
#define oyToChannels_m(c)           ((c)&255)
#define oyToColourOffset_m(o)       (((o) >> 8)&255)
#define oyToDataType_m(t)           ((oyDATATYPE_e)(((t) >> 16)&15))
#define oyToSwapColourChannels_m(s) (((s) >> 20)&1)
#define oyToPlanar_m(p)             (((p) >> 21)&1)
#define oyToFlavor_m(f)             (((f) >> 22)&1)
#define oyToByteswap_m(x)           (((x) >> 23)&1)

#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */


#endif /* OYRANOS_IMAGE_H */
