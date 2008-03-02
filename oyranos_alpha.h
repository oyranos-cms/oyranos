/**
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2004-2007  Kai-Uwe Behrmann
 *
 * @autor: Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * -----------------------------------------------------------------------------
 */

/** @file @internal
 *  @brief misc alpha API's
 */

/** @date      25. 11. 2004 */


#ifndef OYRANOS_ALPHA_H
#define OYRANOS_ALPHA_H

#include "oyranos.h"
#include "oyranos_icc.h"
#include "oyranos_version.h"

#ifdef __cplusplus
namespace oyranos {
extern "C" {
#endif /* __cplusplus */


#define oyInPlaning
#ifdef oyInPlaning


void                    oyLab2XYZ       (const double * CIElab, double * XYZ);
void                    oyXYZ2Lab       (const double * XYZ, double * CIElab);


#define oyPointer void*

/** @internal
 *  @brief Oyranos name structure
 *
 *  since: (Oyranos: version 0.1.x)
 */
typedef enum {
  oyOBJECT_TYPE_NONE,
  oyOBJECT_TYPE_OBJECT_S,
  oyOBJECT_TYPE_DISPLAY_S,
  oyOBJECT_TYPE_NAMED_COLOUR_S,       /*!< @see oyNamedColour_s */
  oyOBJECT_TYPE_NAMED_COLOURS_S,      /*!< @see oyNamedColours_s */
  oyOBJECT_TYPE_PROFILE_S,            /*!< @see oyProfile_s */
  oyOBJECT_TYPE_PROFILES_S,           /*!< @see oyProfiles_s */
  oyOBJECT_TYPE_OPTION_S,             /*!< @see oyOption_s */
  oyOBJECT_TYPE_OPTIONS_S,            /*!< @see oyOptions_s */
  oyOBJECT_TYPE_REGION_S,             /*!< @see oyRegion_s */
  oyOBJECT_TYPE_IMAGE_S,              /*!< @see oyImage_s */
  oyOBJECT_TYPE_COLOUR_CONVERSION_S,  /*!< @see oyColourConversion_s */
  oyOBJECT_TYPE_CMM_POINTER_S,        /*!< @see oyCMMptr_s */
  oyOBJECT_TYPE_ICON_S,               /*!< @see oyIcon_s */
  oyOBJECT_TYPE_CMM_INFO_S,           /*!< @see oyCMMInfo_s */
  oyOBJECT_TYPE_NAME_S                /*!< @see oyName_s */
} oyOBJECT_TYPE_e;

typedef enum {
  oyNAME_NAME,
  oyNAME_NICK,
  oyNAME_DESCRIPTION
} oyNAME_e;

/** @internal
 *  @brief Oyranos name structure
 *
 *  since: (Oyranos: version 0.1.8)
 */
typedef struct {
  oyOBJECT_TYPE_e      type;           /*!< internal struct type oyOBJECT_TYPE_NAME_S */
  char               * nick;    /*!< few letters for mass representation (A1) */
  char               * name;           /*!< normal user visible name (A1-MySys)*/
  char               * description;    /*!< full user description (A1-MySys from Oyranos) */
} oyName_s;

void         oyNameRelease_           ( oyName_s       ** name,
                                        oyDeAllocFunc_t   deallocateFunc );

/** @internal
 *  @brief CMM pointer
 *
 *  since: (Oyranos: version 0.1.8)
 */
typedef struct {
  oyOBJECT_TYPE_e      type;           /*!< internal struct type oyOBJECT_TYPE_CMM_POINTER_S */
  char               * cmm;            /*!< the CMM the option is targeting */
  oyPointer            ptr;            /*!< a CMM's data pointer */
  oyDeAllocFunc_t      deallocateFunc; /*!< CMM's deallocation function */
} oyCMMptr_s;


/** @internal
 *  @brief Oyranos structure base
 *
 *  since: (Oyranos: version 0.1.8)
 */
typedef struct {
  oyOBJECT_TYPE_e      type;          /*!< internal struct type oyOBJECT_TYPE_OBJECT_S */
  oyAllocFunc_t        allocateFunc;
  oyDeAllocFunc_t      deallocateFunc;
  oyPointer            parent;        /*!< parent struct of parent_type */
  oyOBJECT_TYPE_e      parent_type;   /*!< parents struct type */
  oyPointer            backdoor;      /*!< allow non breaking extensions */
  oyCMMptr_s        ** cmm_ptr;       /*!< CMM's data pointers */
  oyName_s           * name;          /*!< naming feature */
  int                  ref;           /*!< reference counter */
  int                  version;       /*!< OYRANOS_VERSION */
  char               * hash;          /*!< has_sum */
} oyObject_s;

oyObject_s * oyObjectNew              ( void );
oyObject_s * oyObjectNewWithAllocators( oyAllocFunc_t     allocateFunc,
                                        oyDeAllocFunc_t   deallocateFunc );
oyObject_s * oyObjectCopy             ( oyObject_s      * object );
oyObject_s * oyObjectSetAllocators    ( oyObject_s      * object,
                                        oyAllocFunc_t     allocateFunc,
                                        oyDeAllocFunc_t   deallocateFunc );
void         oyObjectRelease          ( oyObject_s     ** oy );

oyObject_s * oyObjectSetParent        ( oyObject_s      * object,
                                        oyOBJECT_TYPE_e   type,
                                        oyPointer         ptr );
/*oyPointer    oyObjectAlign            ( oyObject_s      * oy,
                                        size_t          * size,
                                        oyAllocFunc_t     allocateFunc );*/

oyObject_s * oyObjectSetNames         ( oyObject_s      * object,
                                        const char      * nick,
                                        const char      * name,
                                        const char      * description );
oyObject_s * oyObjectSetName          ( oyObject_s      * object,
                                        const char      * name,
                                        oyNAME_e          type );
const char * oyObjectGetName          ( const oyObject_s* object,
                                        oyNAME_e          type );
oyPointer    oyObjectGetCMMPtr        ( oyObject_s      * object,
                                        const char      * cmm );
oyObject_s * oyObjectSetCMMPtr        ( oyObject_s      * object,
                                        const char      * cmm,
                                        oyPointer         ptr,
                                        oyDeAllocFunc_t   deallocateFunc );

/* --- colour conversions --- */


/** @brief Option for rendering

    @todo include the oyOptions_t_ type for gui elements
    should be used in a list oyColourTransformOptions_s to form a options set
 */
typedef struct {
  oyOBJECT_TYPE_e      type;           /*!< internal struct type oyOBJECT_TYPE_OPTION_S */
  oyObject_s         * oy;
  oyWIDGET_e           opt;            /*!< registred option */
  int             supported_by_chain;  /*!< 1 for supporting; 0 if one fails */
  double               value_d;        /*!< value of option; unset with nan; */ 
} oyOption_s;

/** @brief Options for rendering
    Options can be any flag or rendering intent and other informations needed to
    configure a process. It contains variables for colour transforms.
 */
typedef struct {
  oyOBJECT_TYPE_e      type;           /*!< internal struct type oyOBJECT_TYPE_OPTIONS_S */
  oyObject_s         * oy;
  int                  n;              /*!< number of options */
  oyOption_s*          opts;
} oyOptions_s;
#if 0
/** allocate n oyOption_s */
oyOptions_s*   oyOptionsCreate        ( int n,
                                        oyObject_s      * object);
/** allocate oyOption_s for a 4 char CMM identifier obtained by oyModulsGetNames
 */
oyOptions_s*   oyOptionsCreateFor     ( const char      * cmm,
                                        oyObject_s      * object);
void           oyOptionsRelease       ( oyOptions_s     * options );
oyPointer      oyOptionsAlign         ( oyOptions_s     * options,
                                        size_t          * size,
                                        oyAllocFunc_t     allocateFunc );

/** confirm if all is ok

  @param[in]   opts      the options to verify
  @param[in]   cmm       the CMM to check for
  @return                NULL for no error, or non conforming options
 */
oyOptions_s*   oyOptionsVerifyForCMM  ( oyOptions_s     * opts,
                                        const char      * cmm,
                                        oyObject_s      * object);
#endif

/** @brief a profile and its attributes
 */
typedef struct {
  oyOBJECT_TYPE_e      type;           /*!< internal struct type oyOBJECT_TYPE_COLOUR_PROFILE_S */
  oyObject_s         * oy;
  oyChar             * file_name;      /*!< file name for loading on request */
  size_t               size;           /*!< ICC profile size */
  void               * block;          /*!< ICC profile data */
  icColorSpaceSignature sig;           /*!< ICC profile signature */
  char               * hash;           /*!< hash sum */
  oyPROFILE_e          use_default;    /*!< if > 0 : take from settings */
  oyObject_s        ** names_chan;     /*!< user visible channel description */
  int                  channels_n;     /*!< number of channels */
} oyProfile_s;

OYAPI oyProfile_s * OYEXPORT 
                    oyProfileFromStd  ( oyPROFILE_e       colour_space,
                                        oyObject_s      * object);
OYAPI oyProfile_s * OYEXPORT
                    oyProfileFromFile ( const char*       file_name,
                                        int               flags,
                                        oyObject_s      * object);
OYAPI oyProfile_s * OYEXPORT
                    oyProfileFromMem  ( size_t            buf_size,
                                        const oyPointer   buf,
                                        int               flags,
                                        oyObject_s      * object);
OYAPI oyProfile_s * OYEXPORT
                    oyProfileCopy     ( oyProfile_s     * profile,
                                        oyObject_s      * object);
OYAPI void OYEXPORT oyProfileRelease  ( oyProfile_s ** profile );
#if 0
OYAPI oyPointer OYEXPORT
                    oyProfileAlign    ( oyProfile_s     * profile,
                                        size_t          * size,
                                        oyAllocFunc_t     allocateFunc );
#endif
OYAPI int OYEXPORT  oyProfileGetChannelsCount ( oyProfile_s * colour);

OYAPI icColorSpaceSignature OYEXPORT
                    oyProfileGetSignature ( oyProfile_s * profile );
OYAPI void OYEXPORT oyProfileSetChannelNames( oyProfile_s * colour,
                                        oyObject_s     ** names_chan );
OYAPI const oyObject_s ** OYEXPORT
                    oyProfileGetChannelNames( oyProfile_s * colour);
OYAPI int OYEXPORT  oyProfileGetChannelsCount(oyProfile_s * profile_ref );
OYAPI const oyChar * OYEXPORT
                    oyProfileGetChannelName ( oyProfile_s * profile,
                                        int               channel_pos,
                                        oyNAME_e          type );


/** @brief tell about the conversion profiles
 */
typedef struct {
  oyOBJECT_TYPE_e      type;           /*!< internal struct type oyOBJECT_TYPE_PROFILES_S */
  oyObject_s         * oy;
  int                  n;              /*!< number of profiles */
  oyProfile_s        * profiles;
} oyProfileList_s;

/** @brief start with a simple rectangle */
typedef struct {
  oyOBJECT_TYPE_e      type;           /*!< internal struct type oyOBJECT_TYPE_REGION_S */
  oyObject_s         * oy;
  int x;
  int y;
  int width;
  int height;
} oyRegion_s;

typedef enum {
  oyUINT8,     /*!<  8-bit integer */
  oyUINT16,    /*!< 16-bit integer */
  oyUINT32,    /*!< 32-bit integer */
  oyHALF,      /*!< 16-bit floating point number */
  oyFLOAT,     /*!< IEEE floating point number */
  oyDOUBLE     /*!< IEEE double precission floating point number */
} oyDATATYPE_e;


/** parametric enum as shorthand for the channel layout in bitmaps

    should fit into a 32bit type, usual unsigned int or uint32_t

    F P S TTTT OOOOOOOO CCCCCCCC

    C  channels count per pixel (3 for RGB); max 255
    O  colour channel offset (0 for RGB, 1 for ARGB)
    P  Planar bool: 0 - interwoven, 1 - one channel after the other
    S  Swap colour channels bool (BGR)
    T  Type oyDATATYPE_e
    X  non host byte order
    F  Revert bool: 0 - MinIsBlack(Chocolate) 1 - MinIsWhite(Vanilla);
       exchange min and max : (1-x)

 */

#define oyCHANNELS_M(c)             (c)
#define oyCOLOUR_OFFSET_M(o)        ((o) << 8)
#define oyDATATYPE_M(t)             ((t) << 16)
#define oySWAP_COLOUR_CHANNELS_M(s) ((s) << 20)
#define oyPLANAR_M(p)               ((p) << 21)
#define oyFLAVOR_M(p)               ((f) << 22)

/** define some common types */
#define oyTYPE_123_8       (oyCHANNELS_M(3)|oyDATATYPE_M(oyUINT8))
#define oyTYPE_123_16      (oyCHANNELS_M(3)|oyDATATYPE_M(oyUINT16))
#define oyTYPE_123_HALF    (oyCHANNELS_M(3)|oyDATATYPE_M(oyHALF))
#define oyTYPE_123_FLOAT   (oyCHANNELS_M(3)|oyDATATYPE_M(oyFLOAT))
#define oyTYPE_123_DBL     (oyCHANNELS_M(3)|oyDATATYPE_M(oyDOUBLE))

#define oyTYPE_123A_8      (oyCHANNELS_M(4)|oyDATATYPE_M(oyUINT8))
#define oyTYPE_123A_16     (oyCHANNELS_M(4)|oyDATATYPE_M(oyUINT16))
#define oyTYPE_123A_HALF   (oyCHANNELS_M(4)|oyDATATYPE_M(oyHALF))
#define oyTYPE_123A_FLOAT  (oyCHANNELS_M(4)|oyDATATYPE_M(oyFLOAT))
#define oyTYPE_123A_DBL    (oyCHANNELS_M(4)|oyDATATYPE_M(oyDOUBLE))

#define oyTYPE_123AZ_HALF  (oyCHANNELS_M(5)|oyDATATYPE_M(oyHALF))
#define oyTYPE_123AZ_FLOAT (oyCHANNELS_M(5)|oyDATATYPE_M(oyFLOAT))
#define oyTYPE_123AZ_DBL   (oyCHANNELS_M(5)|oyDATATYPE_M(oyDOUBLE))

#define oyTYPE_A123_8      (oyCHANNELS_M(4)|oyCOLOUR_OFFSET_M(1)|oyDATATYPE_M(oyUINT8))
#define oyTYPE_A123_16     (oyCHANNELS_M(4)|oyCOLOUR_OFFSET_M(1)|oyDATATYPE_M(oyUINT16))

#define oyTYPE_A321_8      (oyCHANNELS_M(4)|oyCOLOUR_OFFSET_M(1)|oyDATATYPE_M(oyUINT8)|oySWAP_COLOUR_CHANNELS_M(oyYES))
#define oyTYPE_A321_16     (oyCHANNELS_M(4)|oyCOLOUR_OFFSET_M(1)|oyDATATYPE_M(oyUINT16)|oySWAP_COLOUR_CHANNELS_M(oyYES))

/* some intermixed types will work as well */
#define oyTYPE_123A_HALF_Z_FLOAT (oyCHANNELS_M(4+1*2)|oyDATATYPE_M(oyHALF))

#define oyTYPE_1234_8      (oyCHANNELS_M(4)|oyDATATYPE_M(oyUINT8))
#define oyTYPE_1234A_8     (oyCHANNELS_M(5)|oyDATATYPE_M(oyUINT8))
#define oyTYPE_1234_16     (oyCHANNELS_M(4)|oyDATATYPE_M(oyUINT16))
#define oyTYPE_1234A_16    (oyCHANNELS_M(5)|oyDATATYPE_M(oyUINT16))

#define oyTYPE_1234_8_REV  (oyCHANNELS_M(4)|oyDATATYPE_M(oyUINT8)|oyFLAVOR_M(oyYes))
#define oyTYPE_1234A_8_REV (oyCHANNELS_M(5)|oyDATATYPE_M(oyUINT8)|oyFLAVOR_M(oyYes))
#define oyTYPE_1234_16_REV (oyCHANNELS_M(4)|oyDATATYPE_M(oyUINT16)|oyFLAVOR_M(oyYes))
#define oyTYPE_1234A_16_REV (oyCHANNELS_M(5)|oyDATATYPE_M(oyUINT16)|oyFLAVOR_M(oyYes))


#define oyTYPE_1_8         (oyCHANNELS_M(1)|oyDATATYPE_M(oyUINT8))
#define oyTYPE_1_16        (oyCHANNELS_M(1)|oyDATATYPE_M(oyUINT16))
#define oyTYPE_1_HALF      (oyCHANNELS_M(1)|oyDATATYPE_M(oyHALF))
#define oyTYPE_1_FLOAT     (oyCHANNELS_M(1)|oyDATATYPE_M(oyFLOAT))
#define oyTYPE_1A_8        (oyCHANNELS_M(2)|oyDATATYPE_M(oyUINT8))
#define oyTYPE_1A_16       (oyCHANNELS_M(2)|oyDATATYPE_M(oyUINT16))
#define oyTYPE_1A_HALF     (oyCHANNELS_M(2)|oyDATATYPE_M(oyHALF))
#define oyTYPE_1A_FLOAT    (oyCHANNELS_M(2)|oyDATATYPE_M(oyFLOAT))

#define oyTYPE_1_8_REV     (oyCHANNELS_M(1)|oyDATATYPE_M(oyUINT8)|oyFLAVOR_M(oyYes))
#define oyTYPE_1_16_REV    (oyCHANNELS_M(1)|oyDATATYPE_M(oyUINT16)|oyFLAVOR_M(oyYes))
#define oyTYPE_1_HALF_REV  (oyCHANNELS_M(1)|oyDATATYPE_M(oyHALF)|oyFLAVOR_M(oyYes))
#define oyTYPE_1_FLOAT_REV (oyCHANNELS_M(1)|oyDATATYPE_M(oyFLOAT))|oyFLAVOR_M(oyYes)
#define oyTYPE_1A_8_REV    (oyCHANNELS_M(2)|oyDATATYPE_M(oyUINT8)|oyFLAVOR_M(oyYes))
#define oyTYPE_1A_16_REV   (oyCHANNELS_M(2)|oyDATATYPE_M(oyUINT16)|oyFLAVOR_M(oyYes))
#define oyTYPE_1A_HALF_REV (oyCHANNELS_M(2)|oyDATATYPE_M(oyHALF)|oyFLAVOR_M(oyYes))
#define oyTYPE_1A_FLOAT_REV (oyCHANNELS_M(2)|oyDATATYPE_M(oyFLOAT)|oyFLAVOR_M(oyYes))

/* decode */
#define oyT_CHANNELS_M(c)           ((c)&255)
#define oyT_COLOUR_OFFSET_M(o)      (((o) >> 8)&255)
#define oyT_DATATYPE_M(t)           (((t) >> 16)&15)
#define oyT_SWAP_COLOUR_CHANNELS_M(s) (((s) >> 20)&1)
#define oyT_PLANAR_M(p)             (((p) >> 21)&1)
#define oyT_FLAVOR_M(f)             (((f) >> 22)&1)


/** @brief a reference struct to gather information for image transformation

    as we dont target a complete imaging solution, only raster is supported

    oyImage_s should hold image dimensions,
    oyDisplayRegion_s information and
    a reference to the data for conversion

    As well referencing of itself would be nice, to allow light copies.

    Should oyImage_s become internal and we provide a user interface?
 */
typedef struct {
  oyOBJECT_TYPE_e      type;           /*!< internal struct type oyOBJECT_TYPE_IMAGE_S */
  oyObject_s         * oy;
  int                  width;          /*!< data width */
  int                  height;         /*!< data height */
  oyPointer            data;           /*!< image data */
  const oyOptions_s  * options;        /*!< for instance channel layout */
  oyProfile_s        * profile;        /*!< image profile */
  const oyRegion_s   * region;   /*!< region to render, if zero render all */
  int                  display_pos_x;  /*!< upper position on display of image*/
  int                  display_pos_y;  /*!< left position on display of image */
  int                * layout_;  /*!< internal samples mask (3,384,2,1,0 BGR) */
} oyImage_s;


oyImage_s *    oyImageCreate          ( int               width,
                                        int               height, 
                                        oyPointer         channels,
                                        int               pixel_layout,
                                        oyProfile_s     * profile,
                                        oyObject_s      * object);
oyImage_s *    oyImageCreateForDisplay( int               width,
                                        int               height, 
                                        oyPointer         channels,
                                        int               pixel_layout,
                                        const char      * display_name,
                                        int               display_pos_x,
                                        int               display_pos_y,
                                        oyObject_s      * object);
oyImage_s *    oyImageCopy            ( oyImage_s       * image,
                                        oyObject_s      * object );
void           oyImageRelease         ( oyImage_s      ** image );


/** In case where
      a option indicates monitor output, or
      the out image struct has no profile set, 
    the conversion will route to monitor colours, honouring the oyImage_s screen
    position.
 */
typedef struct {
  oyOBJECT_TYPE_e      type;           /*!< internal struct type oyOBJECT_TYPE_COLOUR_CONVERSION_S */
  oyObject_s         * oy;
  oyProfileList_s    * profiles;       /*!< effect / simulation profiles */ 
  oyOptions_s        * options;        /*!< conversion opts */
  oyImage_s          * image_in;       /*!< input */
  oyImage_s          * image_out;      /*!< output */
} oyColourConversion_s;

oyColourConversion_s* oyColourConversionCreate (
                                        oyProfileList_s *list,
                                        oyOptions_s     * opts,
                                        oyImage_s       * in,
                                        oyImage_s       * out,
                                        oyObject_s      * object);
void        oyColourConversionRelease ( oyColourConversion_s ** cc );

int             oyColourConversionRun ( oyColourConversion_s *colour /*!< object*/
                                     );                  /*!< return: error */

/** @brief colour patch with meta informations
 *
 *  Data management on library side.
 *  User can control memory management at creation time.
 *
 *  It has the complexity of a object, and should not be accessed directly.
 *
 *  since: (ICC Examin: version 0.45)
 *
 *  TODO: needs to be Xatom compatible
 */
typedef struct {
  oyOBJECT_TYPE_e      type;           /*!< internal struct type oyOBJECT_TYPE_NAMED_COLOUR_S */
  oyObject_s         * oy;             /*!< internal struct type */
  double             * channels;       /*!< eigther parsed or calculated otherwise */
  double               XYZ[3];         /*!< CIE*XYZ representation */
  char               * blob;           /*!< advanced : CGATS / ICC ? */
  size_t               blob_len;       /*!< advanced : CGATS / ICC ? */
  oyProfile_s        * profile;        /*!< ICC */
} oyNamedColour_s;

oyNamedColour_s*  oyNamedColourCreate ( oyObject_s        * object,
                                        const double      * chan,
                                        const char        * blob,
                                        int                 blob_len,
                                        oyProfile_s       * profile_ref);
oyNamedColour_s*  oyNamedColourCreateWithName (
                                        const char        * nick,
                                        const char        * name,
                                        const char        * description,
                                        const double      * chan,
                                        const double      * XYZ,
                                        const char        * blob,
                                        int                 blob_len,
                                        oyProfile_s       * profile_ref,
                                        oyObject_s        * object );
oyNamedColour_s*  oyNamedColourCopy   ( const oyNamedColour_s  * colour,
                                        oyObject_s        * object );
void              oyNamedColourRelease( oyNamedColour_s  ** colour );

oyProfile_s *     oyNamedColourGetSpace( oyNamedColour_s  * colour );
int               oyNamedColourSetSpace( oyNamedColour_s  * colour,
                                        oyProfile_s       * profile_ref );
void              oyNamedColourSetChannels( oyNamedColour_s * colour,
                                        const double      * channels );
void              oyNamedColourSetColourStd( oyNamedColour_s * colour,
                                        oyPROFILE_e         profile_ref,
                                        const double      * channels );
const double *    oyNamedColourGetChannelsConst(oyNamedColour_s * colour);
const double *    oyNamedColourGetXYZConst    ( oyNamedColour_s * colour);
int               oyNamedColourConvertStd( oyNamedColour_s* colour,
                                        oyPROFILE_e         colour_space,
                                        oyPointer           buf,
                                        oyDATATYPE_e        buf_type );

/** @brief list of colour patches
 *
 *  Data management on library side.
 *  User can control memory management at creation time.
 *
 *  It has the complexity of a object, and should not be accessed directly.
 *
 *  since: (ICC Examin: version 0.45)
 *
 *  TODO: make the object non visible
 */
typedef struct {
  oyOBJECT_TYPE_e      type;           /*!< internal struct type oyOBJECT_TYPE_NAMED_COLOURS_S */
  oyObject_s         * oy;             /*!< internal struct type */
  int                  size;           /*!< list entries */
  oyNamedColour_s   ** colours;        /*!< colour list */
} oyNamedColours_s;

oyNamedColours_s* oyNamedColoursCreate( const char  * nick,
                                        const char  * name,
                                        const char  * name_long,
                                        int           n,
                                        oyObject_s  * object );
oyNamedColours_s* oyNamedColoursCopy  ( const oyNamedColours_s * colours,
                                        oyObject_s  * object );
void              oyNamedColoursRelease(oyNamedColours_s** colours );

int               oyNamedColoursCount ( oyNamedColours_s * swatch );
oyNamedColour_s*  oyNamedColoursGet   ( oyNamedColours_s * swatch,
                                        int           position);
int               oyNamedColoursAdd   ( oyNamedColours_s * swatch,
                                        oyNamedColour_s  * patch,
                                        int           position );
int               oyNamedColoursRemovePatch( oyNamedColours_s * swatch,
                                        int           position );


void              oyCopyColour        ( const double      * from,
                                        double            * to,
                                        int                 n,
                                        oyProfile_s * profile_ref,
                                        int                 channels_n );




/* --- CMM API --- */

int    oyModulRegisterXML             ( oyGROUP_e         group,
                                        const char      * xml );

/** obtain 4 char CMM identifiers and count of CMM's */
char** oyModulsGetNames               ( int             * count,
                                        oyAllocFunc_t     allocateFunc );
/** Query for available options for a cmm

    @param[in] cmm      the 4 char CMM ID or zero for the current CMM
    @return             available options
 */
oyOptions_s * oyModulGetOptions       ( const char      * cmm,
                                        oyObject_s      * object);
const char *  oyCMMGetActual          ( int               flags );


/* --- Image Colour Profile API --- */
/* needs extra libraries liboyranos_png liboyranos_tiff ... */
#ifdef OY_HAVE_PNG
#include <png.h>
#define OY_PNG_s    png_infop
oyProfile_s* oyImagePNGgetICC   ( OY_PNG_s info, int flags );
#endif
#ifdef OY_HAVE_TIFF
#include <tiffio.h>
#define OY_TIFF_s   TIFF*
oyProfile_s* oyImageTIFFgetICC  ( OY_TIFF_s dir, int flags );
int                oyImageTIFFsetICC  ( OY_TIFF_s         dir,
                                        oyProfile_s * profile,
                                        int               flags);
#endif
#ifdef OY_HAVE_EXR
#include <OpenEXR/OpenEXR.h>
#define OY_EXR_s    ImfHeader
oyProfile_s*       oyImageEXRgetICC   ( OY_EXR_s          header,
                                        int               flags );
#ifdef __cplusplus
#define OY_EXRpp_s  Imf::Header*
oyProfile_s*       oyImageEXRgetICC   ( OY_EXRpp_s        header,
                                        int               flags );
#endif
#endif
/*
with flags something like:
oyIMAGE_EMBED_ICC_MINIMAL  ...
oyIMAGE_EMBED_ICC_FULL (while for OpenEXR this would not make sense) */

#if 0
typedef struct {
  oyNamedColour_s        oyNamedColour;
  oyNamedColours_s       oyNamedColours;
  oyProfile_s            oyProfile;
  oyOption_s             oyOption;
  oyOptions_s            oyOptions;
  oyRegion_s             oyRegion;
  oyImage_s              oyImage;
  oyColourConversion_s   oyColourConversion;
  oyName_s              oyNames;
} oyObject_u;
#endif

#endif /* oyInPlaning */

#ifdef __cplusplus
} /* extern "C" */
} /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OYRANOS_ALPHA_H */
