
/** @file oyranos_image.h

   [Template file inheritance graph]
   +-- oyranos_image.template.h

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


#ifndef OYRANOS_IMAGE_H
#define OYRANOS_IMAGE_H

#include <oyranos_icc.h>
#include <oyranos_types.h>

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


/** \addtogroup objects_image Image Containers
 *  @brief Pixel buffer abstraction
 *
 *  @{ *//* objects_image */

/** @enum    oyDATATYPE_e
 *  @brief   Basic data types for anonymous pointers 
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

size_t             oyDataTypeGetSize ( oyDATATYPE_e        data_type ); 
const char *       oyDataTypeToText  ( oyDATATYPE_e        data_type );

/** @enum    oyCHANNELTYPE_e
 *  @brief   Channels types of Colors 
 *
 *  This type is useful to present color manipulation UI elements in a 
 *  sensible way.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/00/00 (Oyranos: 0.1.8)
 *  @date    2009/08/03
 */
typedef enum oyCHANNELTYPE_e {
  oyCHANNELTYPE_UNDEFINED,            /*!< as well for the list end */
  oyCHANNELTYPE_OTHER,                /*!< other */
  oyCHANNELTYPE_LIGHTNESS,            /*!< pure lightness */
  oyCHANNELTYPE_COLOR,               /*!< pure color */
  oyCHANNELTYPE_COLOR_LIGHTNESS,     /*!< lightness and color */
  oyCHANNELTYPE_COLOR_LIGHTNESS_PREMULTIPLIED, /*!< premultiplied lightness and color */
  oyCHANNELTYPE_DEPTH,                /*!< depth field */
  oyCHANNELTYPE_ALPHA1,               /*!< alpha */
  oyCHANNELTYPE_ALPHA2,               /*!< alpha2 */
  oyCHANNELTYPE_ALPHA3,               /*!< alpha3 */
  oyCHANNELTYPE_U,                    /*!< u */
  oyCHANNELTYPE_V                     /*!< v */
} oyCHANNELTYPE_e;

oyCHANNELTYPE_e oyICCColorSpaceToChannelLayout (
                                       icColorSpaceSignature sig,
                                       int                 pos );


/** @enum    oyPIXEL_ACCESS_TYPE_e
 *  @brief   Pixel access types 
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/00/00 (Oyranos: 0.1.8)
 *  @date    2008/00/00
 */
typedef enum {
  oyPIXEL_ACCESS_IMAGE,                /*!< image accessor */
  oyPIXEL_ACCESS_POINT,                /*!< dont use */
  oyPIXEL_ACCESS_LINE                  /*!< dont use */
} oyPIXEL_ACCESS_TYPE_e;

/** @typedef oyPixel_t
    parametric type as shorthand for the channel layout in bitmaps \n

    should fit into a 32bit type, usual unsigned int or uint32_t \n

    X F P S TTTT OOOOOOOO CCCCCCCCCCCCCCCC \n

    C  channels count per pixel (3 for RGB); max 65535 \n
    O  color channel offset (0 for RGB, 1 for ARGB) \n
    P  Planar bit: 0 - interwoven, 1 - one channel after the other \n
    S  Swap color channels bit (BGR) \n
    T  Type oyDATATYPE_e \n
    X  non host byte order bit \n
    F  Revert bit: 0 - MinIsBlack(Chocolate) 1 - MinIsWhite(Vanilla); \n
       exchange min and max : (1-x)

 */
typedef uint32_t oyPixel_t;

char   *           oyPixelPrint      ( oyPixel_t           pixel_layout,
                                       oyAlloc_f           allocateFunc );


/** @enum    oyLAYOUT_e
 *  @brief   Pixel mask descriptions.
 */
typedef enum {
  oyLAYOUT = 0, /*!< remembering the layout @see oyPixel_t */
  oyPOFF_X,     /*!< pixel count x offset */
  oyPOFF_Y,     /*!< pixel count y offset */
  oyCOFF,       /*!< channel offset */
  oyDATA_SIZE,  /*!< sample size in byte */
  oyCHANS,      /*!< number of channels */
  oyCHAN0       /*!< first color channel position */
} oyLAYOUT_e;

/** Set channels count in @ref oyPixel_t */
#define oyChannels_m(c)             (c)
/** Set offset count to first color channel in  @ref oyPixel_t */
#define oyColorOffset_m(o)         ((o) << 16)
/** Set @ref oyDATATYPE_e in  @ref oyPixel_t */
#define oyDataType_m(t)             ((t) << 24)
/** Set swapping of color channels in @ref oyPixel_t , e.g. BGR */
#define oySwapColorChannels_m(s)   ((s) << 28)
#define OY_SWAP_COLOR_CHANNELS      0x10000000
/** Set planar instead of interwoven color channels in @ref oyPixel_t */
#define oyPlanar_m(p)               ((p) << 29)
#define OY_PLANAR                   0x20000000
/** Set min is white in @ref oyPixel_t */
#define oyFlavor_m(p)               ((f) << 30)
#define OY_FLAVOR                   0x40000000
/** Set byte swapping for integers larger than 8 byte in @ref oyPixel_t */
#define oyByteswap_m(x)             ((x) << 31)
#define OY_BYTESWAPED               0x80000000

/* define some common types */
/** 8-bit RGB/YUV/XYZ/... */
#define OY_TYPE_123_8       (oyChannels_m(3)|oyDataType_m(oyUINT8))
/** 16-bit RGB/... */
#define OY_TYPE_123_16      (oyChannels_m(3)|oyDataType_m(oyUINT16))
/** 16-bit float RGB/... */
#define OY_TYPE_123_HALF    (oyChannels_m(3)|oyDataType_m(oyHALF))
/** 32-bit float RGB/... */
#define OY_TYPE_123_FLOAT   (oyChannels_m(3)|oyDataType_m(oyFLOAT))
/** 64-bit double RGB/... */
#define OY_TYPE_123_DBL     (oyChannels_m(3)|oyDataType_m(oyDOUBLE))

/** 8-bit RGB/... alpha */
#define OY_TYPE_123A_8      (oyChannels_m(4)|oyDataType_m(oyUINT8))
/** 16-bit RGB/... alpha */
#define OY_TYPE_123A_16     (oyChannels_m(4)|oyDataType_m(oyUINT16))
/** 16-bit float RGB/... alpha */
#define OY_TYPE_123A_HALF   (oyChannels_m(4)|oyDataType_m(oyHALF))
/** 32-bit float RGB/... alpha */
#define OY_TYPE_123A_FLOAT  (oyChannels_m(4)|oyDataType_m(oyFLOAT))
/** 64-bit double RGB/... alpha */
#define OY_TYPE_123A_DBL    (oyChannels_m(4)|oyDataType_m(oyDOUBLE))

/** 16-bit float RGB/... alpha + Z */
#define OY_TYPE_123AZ_HALF  (oyChannels_m(5)|oyDataType_m(oyHALF))
/** 32-bit float RGB/... alpha + Z */
#define OY_TYPE_123AZ_FLOAT (oyChannels_m(5)|oyDataType_m(oyFLOAT))
/** 64-bit double RGB/... alpha + Z */
#define OY_TYPE_123AZ_DBL   (oyChannels_m(5)|oyDataType_m(oyDOUBLE))

#define OY_TYPE_A123_8      (oyChannels_m(4)|oyColorOffset_m(1)|oyDataType_m(oyUINT8))
#define OY_TYPE_A123_16     (oyChannels_m(4)|oyColorOffset_m(1)|oyDataType_m(oyUINT16))

#define OY_TYPE_A321_8      (oyChannels_m(4)|oyColorOffset_m(1)|oyDataType_m(oyUINT8)|oySwapColorChannels_m(oyYES))
#define OY_TYPE_A321_16     (oyChannels_m(4)|oyColorOffset_m(1)|oyDataType_m(oyUINT16)|oySwapColorChannels_m(oyYES))

/* some intermixed types will work as well */
#define OY_TYPE_123A_HALF_Z_FLOAT (oyChannels_m(4+1*2)|oyDataType_m(oyHALF))

/** 8-bit Cmyk */
#define OY_TYPE_1234_8      (oyChannels_m(4)|oyDataType_m(oyUINT8))
/** 8-bit Cmyk alpha */
#define OY_TYPE_1234A_8     (oyChannels_m(5)|oyDataType_m(oyUINT8))
#define OY_TYPE_1234_16     (oyChannels_m(4)|oyDataType_m(oyUINT16))
#define OY_TYPE_1234A_16    (oyChannels_m(5)|oyDataType_m(oyUINT16))

#define OY_TYPE_1234_8_REV  (oyChannels_m(4)|oyDataType_m(oyUINT8)|oyFlavor_m(oyYes))
#define OY_TYPE_1234A_8_REV (oyChannels_m(5)|oyDataType_m(oyUINT8)|oyFlavor_m(oyYes))
#define OY_TYPE_1234_16_REV (oyChannels_m(4)|oyDataType_m(oyUINT16)|oyFlavor_m(oyYes))
#define OY_TYPE_1234A_16_REV (oyChannels_m(5)|oyDataType_m(oyUINT16)|oyFlavor_m(oyYes))


/** 8-bit Gray */
#define OY_TYPE_1_8         (oyChannels_m(1)|oyDataType_m(oyUINT8))
/** 16-bit Gray */
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
/** Get channels count in @ref oyPixel_t */
#define oyToChannels_m(c)           ((c)&65535)
/** Get offset count to first color channel in  @ref oyPixel_t */
#define oyToColorOffset_m(o)       (((o) >> 16)&255)
/** Get @ref oyDATATYPE_e in  @ref oyPixel_t */
#define oyToDataType_m(t)           ((oyDATATYPE_e)(((t) >> 24)&15))
/** Get swapping of color channels in @ref oyPixel_t , e.g. BGR */
#define oyToSwapColorChannels_m(s) (((s) >> 28)&1)
/** Get planar instead of interwoven color channels in @ref oyPixel_t */
#define oyToPlanar_m(p)             (((p) >> 29)&1)
/** Get min is white in @ref oyPixel_t */
#define oyToFlavor_m(f)             (((f) >> 30)&1)
/** Get byte swapping for integers larger than 8 byte in @ref oyPixel_t */
#define oyToByteswap_m(x)           (((x) >> 31)&1)

/** @} *//* objects_image */

#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */


#endif /* OYRANOS_IMAGE_H */
