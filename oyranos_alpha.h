/** @file oyranos_alpha.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2004-2011 (C) Kai-Uwe Behrmann
 *
 *  @brief    object APIs
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @since    2004/11/25
 */

/** @date      25. 11. 2004 */


#ifndef OYRANOS_ALPHA_H
#define OYRANOS_ALPHA_H

#include "oyranos.h"
#include "oyranos_icc.h"
#include "oyranos_object.h"
#include "oyBlob_s.h"
#include "oyPointer_s.h"
#include "oyHash_s.h"
#include "oyName_s.h"
#include "oyObject_s.h"
#include "oyObserver_s.h"
#include "oyOption_s.h"
#include "oyOptions_s.h"
#include "oyConfigs_s.h"
#include "oyProfile_s.h"
#include "oyProfiles_s.h"
#include "oyStruct_s.h"
#include "oyStructList_s.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define OY_IN_PLANING
#ifdef OY_IN_PLANING


void                    oyLab2XYZ       (const double * CIElab, double * XYZ);
void                    oyXYZ2Lab       (const double * XYZ, double * CIElab);
void         oyCIEabsXYZ2ICCrelXYZ   ( const double      * CIEXYZ,
                                       double            * ICCXYZ,
                                       const double      * XYZmin,
                                       const double      * XYZmax,
                                       const double      * XYZwhite );
void         oyICCXYZrel2CIEabsXYZ   ( const double      * ICCXYZ,
                                       double            * CIEXYZ,
                                       const double      * XYZmin,
                                       const double      * XYZmax,
                                       const double      * XYZwhite );
int                oyBigEndian       ( void );

typedef struct oyImage_s oyImage_s;



/** param[out]     is_allocated          are the points always newly allocated*/
typedef oyPointer (*oyImage_GetPoint_f)( oyImage_s       * image,
                                         int               point_x,
                                         int               point_y,
                                         int               channel,
                                         int             * is_allocated );
/** param[out]     is_allocated          are the lines always newly allocated */
typedef oyPointer (*oyImage_GetLine_f) ( oyImage_s       * image,
                                         int               line_y,
                                         int             * height,
                                         int               channel,
                                         int             * is_allocated );
/** param[out]     is_allocated          are the tiles always newly allocated */
typedef oyPointer*(*oyImage_GetTile_f) ( oyImage_s       * image,
                                         int               tile_x,
                                         int               tile_y,
                                         int               channel,
                                         int             * is_allocated );
typedef int       (*oyImage_SetPoint_f)( oyImage_s       * image,
                                         int               point_x,
                                         int               point_y,
                                         int               channel,
                                         oyPointer         data );
typedef int       (*oyImage_SetLine_f) ( oyImage_s       * image,
                                         int               point_x,
                                         int               point_y,
                                         int               pixel_n,
                                         int               channel,
                                         oyPointer         data );
typedef int       (*oyImage_SetTile_f) ( oyImage_s       * image,
                                         int               tile_x,
                                         int               tile_y,
                                         int               channel,
                                         oyPointer         data );




#define oyNAME_ID oyNAME_NICK


oyPointer    oyStruct_Allocate       ( oyStruct_s        * st,
                                       size_t              size );
int          oyStruct_GetId          ( oyStruct_s        * st );



const char *       oySignalToString  ( oySIGNAL_e          signal_type );


OYAPI oyObserver_s * OYEXPORT
           oyObserver_New            ( oyObject_s          object );
OYAPI oyObserver_s * OYEXPORT
           oyObserver_Copy           ( oyObserver_s      * obj,
                                       oyObject_s          object);
OYAPI int  OYEXPORT
           oyObserver_Release        ( oyObserver_s     ** obj );

OYAPI int  OYEXPORT
           oyObserver_SignalSend     ( oyObserver_s      * observer,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * user_data );
OYAPI int  OYEXPORT
           oyStruct_ObserverAdd      ( oyStruct_s        * model,
                                       oyStruct_s        * observer,
                                       oyStruct_s        * user_data,
                                       oyObserver_Signal_f signalFunc );
OYAPI int  OYEXPORT
           oyStruct_ObserverRemove   ( oyStruct_s        * model,
                                       oyStruct_s        * observer,
                                       oyObserver_Signal_f signalFunc );
OYAPI int  OYEXPORT
           oyStruct_ObserverSignal   ( oyStruct_s        * model,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data );
OYAPI int  OYEXPORT
           oyStruct_DisableSignalSend( oyStruct_s        * model );
OYAPI int  OYEXPORT
           oyStruct_EnableSignalSend ( oyStruct_s        * model );
OYAPI int  OYEXPORT
           oyStruct_ObserversCopy    ( oyStruct_s        * object,
                                       oyStruct_s        * pattern,
                                       uint32_t            flags );
OYAPI int  OYEXPORT
           oyStruct_ObserverCopyModel( oyStruct_s        * model,
                                       oyStruct_s        * pattern,
                                       uint32_t            flags );
OYAPI int  OYEXPORT
           oyStruct_ObserverCopyObserver (
                                       oyStruct_s        * observer,
                                       oyStruct_s        * pattern,
                                       uint32_t            flags );
OYAPI int  OYEXPORT
           oyStruct_IsObserved       ( oyStruct_s        * model,
                                       oyStruct_s        * observer );

#define OY_SIGNAL_BLOCK                0x01 /**< do not send new signals */
#define oyToSignalBlock_m(r)           ((r)&1)
OYAPI uint32_t OYEXPORT
           oySignalFlagsGet          ( void );
OYAPI int  OYEXPORT
           oySignalFlagsSet          ( uint32_t            flags );
int      oyStructSignalForward_      ( oyObserver_s      * observer,
                                       oySIGNAL_e          signal_type,
                                       oyStruct_s        * signal_data );


/** @struct  oyCallback_s
 *  @brief   a Callback object
 *  @extends oyStruct_s
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/18 (Oyranos: 0.1.10)
 *  @date    2009/11/18
 */
typedef struct oyCallback_s {
  oyOBJECT_e           type_;          /**< struct type oyOBJECT_CALLBACK_S */ 
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  void               (*callback)();    /**< a generic callback function */
  oyPointer            data;           /**< generic data to pass to the above
                                            callback */
} oyCallback_s;

OYAPI oyCallback_s * OYEXPORT
           oyCallback_New            ( oyObject_s          object );
OYAPI oyCallback_s * OYEXPORT
           oyCallback_Copy           ( oyCallback_s      * obj,
                                       oyObject_s          object);
OYAPI int  OYEXPORT
           oyCallback_Release        ( oyCallback_s     ** obj );




int          oyName_boolean          ( oyName_s          * name_a,
                                       oyName_s          * name_b,
                                       oyNAME_e            name_type,
                                       oyBOOLEAN_e         type );


#define OY_HASH_SIZE 16

#define OY_OBSERVE_AS_WELL 0x01

oyHash_s *   oyCMMCacheListGetEntry_ ( const char        * hash_text );
oyStructList_s** oyCMMCacheList_     ( void );
char   *     oyCMMCacheListPrint_    ( void );


/* --- colour conversion --- */



/** 
 *  @ingroup objects_conversion
 *
 *  @version Oyranos: 0.1.11
 *  @since   2010/08/12 (Oyranos: 0.1.11)
 *  @date    2010/08/12
 */
typedef enum {
  oyFILTER_REG_MODE_NONE = 0,
  /** segments starting with underbar are implementation attributes */
  oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR = 0x0100
} oyFILTER_REG_MODE_e;
char   oyFilterRegistrationModify    ( const char        * registration,
                                       oyFILTER_REG_MODE_e mode,
                                       char             ** result,
                                       oyAlloc_f           allocateFunc );
int          oyRegistrationEraseFromDB(const char        * registration );


oyOption_s *   oyOption_FromDB       ( const char        * registration,
                                       oyObject_s          object );


#define OY_STRING_LIST                 0x01 /** create a oyVAL_STRING_LIST */
/* decode */
#define oyToStringList_m(r)            ((r)&1)


#define OY_CREATE_NEW                  0x02        /** create */
#define OY_ADD_ALWAYS                  0x04        /** do not use for configurations */
/* decode */
#define oyToCreateNew_m(r)             (((r) >> 1)&1)
#define oyToAddAlways_m(r)             (((r) >> 2)&1)
OYAPI int  OYEXPORT
               oyOptions_SaveToDB    ( oyOptions_s       * options,
                                       const char        * key_base_name,
                                       char             ** new_reg,
                                       oyAlloc_f           allocateFunc );

OYAPI int  OYEXPORT
               oyConfig_Set          ( oyConfig_s        * config,
                                       const char        * registration_domain,
                                       oyOptions_s       * options,
                                       oyBlob_s          * data );


OYAPI int  OYEXPORT
                 oyConfigs_SelectSimiliars (
                                       oyConfigs_s       * list,
                                       const char        * pattern[][2],
                                       oyConfigs_s      ** filtered );



/** @struct  oyConfDomain_s
 *  @brief   a ConfDomain object
 *  @extends oyStruct_s
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
typedef struct oyConfDomain_s {
  oyOBJECT_e           type_;          /**< struct type oyOBJECT_CONF_DOMAIN_S */ 
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */
} oyConfDomain_s;

OYAPI oyConfDomain_s * OYEXPORT
           oyConfDomain_FromReg      ( const char        * registration_domain,
                                       oyObject_s          object );
OYAPI oyConfDomain_s * OYEXPORT
           oyConfDomain_Copy         ( oyConfDomain_s    * obj,
                                       oyObject_s          object );
OYAPI int  OYEXPORT
           oyConfDomain_Release      ( oyConfDomain_s    **obj );

OYAPI const char * OYEXPORT
           oyConfDomain_GetText      ( oyConfDomain_s    * obj,
                                       const char        * name,
                                       oyNAME_e            type );
OYAPI const char ** OYEXPORT
           oyConfDomain_GetTexts     ( oyConfDomain_s    * obj );


/** @typedef oyIO_t
    parametric type as shorthand for IO flags \n

    should fit into a 32bit type, usual unsigned int or uint32_t \n

    ------ W R \n

    R  no cache read \n
    W  no cache write \n

 */
#define OY_NO_CACHE_READ            0x01        /** read not from cache */
#define OY_NO_CACHE_WRITE           0x02        /** write not from cache */
/* decode */
#define oyToNoCacheRead_m(r)        ((r)&1)
#define oyToNoCacheWrite_m(w)       (((w) >> 1)&1)
#if 0
OYAPI oyPointer OYEXPORT
                   oyProfile_Align   ( oyProfile_s     * profile,
                                       size_t          * size,
                                       oyAlloc_f         allocateFunc );
#endif

#define OY_COMPUTE      0x01
#define OY_FROM_PROFILE 0x04


/** @enum    oyDATALAYOUT_e
 *  @brief   ideas for basic data arrangements
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/00/00 (Oyranos: 0.1.8)
 *  @date    2008/00/00
 */
typedef enum oyDATALAYOUT_e {
  oyDATALAYOUT_NONE,
  oyDATALAYOUT_CURVE,                 /**< equally spaced curve, oyDATALAYOUT_e[0], size[1], min[2], max[3], elements[4]... */
  oyDATALAYOUT_MATRIX,                /**< 3x3 matrix, oyDATALAYOUT_e[0], a1[1],a2[2],a3,b1,b2,b3,c1,c2,c3 */
  oyDATALAYOUT_TABLE,                 /**< CLUT, oyDATALAYOUT_e[0], table dimensions in[1], array out[2], size of first dimension[3], size of second [4], ... size of last[n], elements[n+1]... */
  oyDATALAYOUT_ARRAY,                 /**< value array, oyDATALAYOUT_e[0], size[1], elements[2]... */
  /*oyDATALAYOUT_PICEWISE_CURVE,*/       /**< paired curve, layout as in oyDATA_LAYOUT_CURVE but with elements grouped to two */
  /*oyDATALAYOUT_HULL,*/              /**< oyDATALAYOUT_e[0], triangle count[1], indixes[2], XYZ triples[3..5]..., followed by interwoven index + normale + midpoint triples[n]... */
  /*oyDATALAYOUT_,*/                /**< */
  oyDATALAYOUT_MAX                    /**< */
} oyDATALAYOUT_e;

/** @brief start with a simple rectangle
 *  @ingroup objects_rectangle
 *  @extends oyStruct_s
 */
typedef struct oyRectangle_s {
  oyOBJECT_e           type_;          /**< @private internal struct type oyOBJECT_RECTANGLE_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< @private */

  double x;
  double y;
  double width;
  double height;

} oyRectangle_s;

oyRectangle_s* oyRectangle_New_      ( oyObject_s          object );
oyRectangle_s* oyRectangle_NewWith   ( double              x,
                                       double              y,
                                       double              width,
                                       double              height,
                                       oyObject_s          object );
oyRectangle_s* oyRectangle_NewFrom   ( oyRectangle_s     * ref,
                                       oyObject_s          object );
oyRectangle_s* oyRectangle_Copy      ( oyRectangle_s     * rectangle,
                                       oyObject_s          object );
int            oyRectangle_Release   ( oyRectangle_s    ** rectangle );

void           oyRectangle_SetGeo    ( oyRectangle_s     * edit_rectangle,
                                       double              x,
                                       double              y,
                                       double              width,
                                       double              height );
void           oyRectangle_SetByRectangle (
                                       oyRectangle_s     * edit_rectangle,
                                       oyRectangle_s     * ref );
void           oyRectangle_Trim      ( oyRectangle_s     * edit_rectangle,
                                       oyRectangle_s     * ref );
void           oyRectangle_MoveInside( oyRectangle_s     * edit_rectangle,
                                       oyRectangle_s     * ref );
void           oyRectangle_Scale     ( oyRectangle_s     * edit_rectangle,
                                       double              factor );
void           oyRectangle_Normalise ( oyRectangle_s     * edit_rectangle );
void           oyRectangle_Round     ( oyRectangle_s     * edit_rectangle );
int            oyRectangle_IsEqual   ( oyRectangle_s     * rectangle1,
                                       oyRectangle_s     * rectangle2 );
int            oyRectangle_IsInside  ( oyRectangle_s     * test,
                                       oyRectangle_s     * ref );
int            oyRectangle_PointIsInside( oyRectangle_s  * rectangle,
                                       double              x,
                                       double              y );
double         oyRectangle_CountPoints(oyRectangle_s     * rectangle );
int            oyRectangle_Index     ( oyRectangle_s     * rectangle,
                                       double              x,
                                       double              y );
int            oyRectangle_SamplesFromImage (
                                       oyImage_s         * image,
                                       oyRectangle_s     * image_rectangle,
                                       oyRectangle_s     * pixel_rectangle );
const char *   oyRectangle_Show      ( oyRectangle_s     * rectangle );


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

int      oySizeofDatatype            ( oyDATATYPE_e        t );


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
#if 0
char   *           oyPixelPrint      ( oyPixel_t           pixel_layout,
                                       oyAlloc_f           allocateFunc );
#endif

typedef struct oyArray2d_s oyArray2d_s;

/** @struct  oyArray2d_s
 *  @brief   2d data array
 *  @ingroup objects_image
 *  @extends oyStruct_s
 *
 *  oyArray2d_s is a in memory data view. The array2d holds pointers to lines in
 *  the original memory blob. The arrays contained in array2d represent the 
 *  samples. There is no information in which order the samples appear. No pixel
 *  layout or meaning is provided. Given the coordinates x and y, a samples 
 *  memory adress can be accessed by &array2d[y][x] . This adress must be
 *  converted to the data type provided in oyArray2d_s::t.
 *
 *  The oyArray2d_s::data pointer should be observed in order to be signaled
 *  about its invalidation.
 *
  \dot
  digraph a {
  bgcolor="transparent";
  nodesep=.05;
  rankdir=LR
      node [shape=record,fontname=Helvetica, fontsize=10, width=.1,height=.1];

      e [ label="oyArray2d_s with 8 samples x 10 lines", shape=plaintext];

      y [ label="<0>0|<1>1|<2>2|<3>3|<4>4|<5>5|<6>6|<7>7|<8>8|<9>9", height=2.0 , style=filled ];
      node [width = 1.5];
      0 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      1 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      2 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      3 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      4 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      5 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      6 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      7 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      8 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      9 [ label="{<0>0|1|2|3|4|5|6|<7>7}" ];
      e
      y:0 -> 0:0
      y:1 -> 1:0
      y:2 -> 2:0
      y:3 -> 3:0
      y:4 -> 4:0
      y:5 -> 5:0
      y:6 -> 6:0
      y:7 -> 7:0
      y:8 -> 8:0
      y:9 -> 9:0
      0:7 -> 1:0 [arrowhead="open", style="dashed"];
      1:7 -> 2:0 [arrowhead="open", style="dashed"];
  }
  \enddot
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/08/23 (Oyranos: 0.1.8)
 *  @date    2008/08/23
 */
struct oyArray2d_s {
  oyOBJECT_e           type_;          /*!< @private struct type oyOBJECT_ARRAY2D_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< @private base object */

  oyDATATYPE_e         t;              /**< data type */
  int                  width;          /**< width of actual data view */
  int                  height;         /**< height of actual data view */
  oyRectangle_s        data_area;      /**< size of reserve pixels, x,y <= 0, width,height >= data view width,height */

  unsigned char     ** array2d;        /**< sorted data, pointer is always owned
                                            by the object */
  int                  own_lines;      /**< Are *array2d rows owned by object?
                                            - 0 not owned by the object
                                            - 1 one own monolithic memory block
                                                starting in array2d[0]
                                            - 2 several owned memory blocks */
  oyStructList_s     * refs_;          /**< references of other arrays to this*/
  oyArray2d_s        * refered_;       /**< array this one refers to */
};

OYAPI oyArray2d_s * OYEXPORT
                   oyArray2d_Create  ( oyPointer           data,
                                       int                 width,
                                       int                 height,
                                       oyDATATYPE_e        type,
                                       oyObject_s          object );
OYAPI oyArray2d_s * OYEXPORT
                 oyArray2d_New       ( oyObject_s          object );
OYAPI oyArray2d_s * OYEXPORT
                 oyArray2d_Copy      ( oyArray2d_s       * obj,
                                       oyObject_s          object);
OYAPI int  OYEXPORT
                 oyArray2d_Release   ( oyArray2d_s      ** obj );


OYAPI int  OYEXPORT
                 oyArray2d_DataSet   ( oyArray2d_s       * obj,
                                       oyPointer           data );
OYAPI int  OYEXPORT
                 oyArray2d_RowsSet   ( oyArray2d_s       * obj,
                                       oyPointer         * rows,
                                       int                 do_copy );
OYAPI int  OYEXPORT  oyArray2d_SetFocus (
                                       oyArray2d_s       * array,
                                       oyRectangle_s     * rectangle );

/** @struct  oyImage_s
 *  @brief   a reference struct to gather information for image transformation
 *  @ingroup objects_image
 *  @extends oyStruct_s
 *
 *  as we dont target a complete imaging solution, only raster is supported
 *
 *  Resolution is in pixel per centimeter.
 *
 *  Requirements: \n
 *  - to provide a view on the image data we look at per line arrays
 *  - it should be possible to echange the to be processed data without altering
 *    the context
 *  - oyImage_s should hold image dimensions,
 *  - display rectangle information and
 *  - a reference to the data for conversion
 *
 *  To set a image data module use oyImage_DataSet().
 *  \dot
 digraph oyImage_s {
  bgcolor="transparent";
  nodesep=.1;
  ranksep=1.;
  rankdir=LR;
  graph [fontname=Helvetica, fontsize=12];
  node [shape=record,fontname=Helvetica, fontsize=10, width=.1];

  subgraph cluster_3 {
    label="oyImage_s data modules";
    color=white;
    clusterrank=global;

      i [ label="... | <0>oyStruct_s * pixel | <1> oyImage_GetPoint_f getPoint | <2>oyImage_GetLine_f getLine | <3>oyImage_GetTile_f getTile | ..."];

      node [width = 2.5, style=filled];
      pixel_A [label="oyArray2d_s arrayA"];
      gp_p_A [label="Array2d_GetPointA"];
      gp_l_A [label="Array2d_GetLineA"];
      gp_t_A [label="Array2d_GetTileA"];

      pixel_B [label="mmap arrayB"];
      gp_p_B [label="mmap_GetPointB"];
      gp_l_B [label="mmap_GetLineB"];
      gp_t_B [label="mmap_GetTileB"];

      subgraph cluster_0 {
        rank=max;
        color=red;
        style=dashed;
        node [style="filled"];
        pixel_A; gp_p_A; gp_l_A; gp_t_A;
        //pixel_A -> gp_p_A -> gp_l_A -> gp_t_A [color=white, arrowhead=none, dirtype=none];
        label="module A";
      }

      subgraph cluster_1 {
        color=blue;
        style=dashed;
        node [style="filled"];
        pixel_B; gp_p_B; gp_l_B; gp_t_B;
        label="module B";
      }

      subgraph cluster_2 {
        color=gray;
        node [style="filled"];
        i;
        label="oyImage_s";
        URL="structoyImage__s.html";
      }

      i:0 -> pixel_A [arrowhead="open", color=red];
      i:1 -> gp_p_A [arrowhead="open", color=red];
      i:2 -> gp_l_A [arrowhead="open", color=red];
      i:3 -> gp_t_A [arrowhead="open", color=red];
      i:0 -> pixel_B [arrowhead="open", color=blue];
      i:1 -> gp_p_B [arrowhead="open", color=blue];
      i:2 -> gp_l_B [arrowhead="open", color=blue];
      i:3 -> gp_t_B [arrowhead="open", color=blue];
  }
 }
 \enddot
 *
 *  Should oyImage_s become internal and we provide a user interface?
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 *  @date    2008/08/23
 */
struct oyImage_s {
  oyOBJECT_e           type_;          /*!< @private struct type oyOBJECT_IMAGE_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< @private base object */

  oyRectangle_s      * viewport;       /**< intented viewing area, normalised to the pixel width == 1.0 */
  double               resolution_x;   /**< resolution in horizontal direction*/
  double               resolution_y;   /**< resolution in vertical direction */

  oyPixel_t          * layout_;        /**< @private samples mask;
                                            the oyPixel_t pixel_layout variable
                                            passed during oyImage_Create is
                                            stored in position 0 */
  oyCHANNELTYPE_e    * channel_layout; /**< non profile described channels */
  int                  width;          /*!< data width */
  int                  height;         /*!< data height */
  oyOptions_s        * tags;           /**< display_rectangle, display_name ... */
  oyProfile_s        * profile_;       /*!< @private image profile */

  oyStruct_s         * pixel_data;     /**< struct used by each subsequent call of g/set* pixel acessors */
  oyImage_GetPoint_f   getPoint;       /**< the point interface */
  oyImage_GetLine_f    getLine;        /**< the line interface */
  oyImage_GetTile_f    getTile;        /**< the tile interface */
  oyImage_SetPoint_f   setPoint;       /**< the point interface */
  oyImage_SetLine_f    setLine;        /**< the line interface */
  oyImage_SetTile_f    setTile;        /**< the tile interface */
  int                  tile_width;     /**< needed by the tile interface */
  int                  tile_height;    /**< needed by the tile interface */
  uint16_t             subsampling[2]; /**< 1, 2 or 4 */
  int                  sub_positioning;/**< 0 None, 1 Postscript, 2 CCIR 601-1*/
  oyStruct_s         * user_data;      /**< user provided pointer */
};


oyImage_s *    oyImage_Create        ( int                 width,
                                       int                 height, 
                                       oyPointer           channels,
                                       oyPixel_t           pixel_layout,
                                       oyProfile_s       * profile,
                                       oyObject_s          object);
oyImage_s *    oyImage_CreateForDisplay ( int              width,
                                       int                 height, 
                                       oyPointer           channels,
                                       oyPixel_t           pixel_layout,
                                       const char        * display_name,
                                       int                 window_pos_x,
                                       int                 window_pos_y,
                                       int                 window_width,
                                       int                 window_height,
                                       oyObject_s          object);
oyImage_s *    oyImage_Copy          ( oyImage_s         * image,
                                       oyObject_s          object );
int            oyImage_Release       ( oyImage_s        ** image );


int            oyImage_SetCritical   ( oyImage_s         * image,
                                       oyPixel_t           pixel_layout,
                                       oyProfile_s       * profile,
                                       oyOptions_s       * options );
int            oyImage_DataSet       ( oyImage_s         * image,
                                       oyStruct_s       ** pixel_data,
                                       oyImage_GetPoint_f  getPoint,
                                       oyImage_GetLine_f   getLine,
                                       oyImage_GetTile_f   getTile,
                                       oyImage_SetPoint_f  setPoint,
                                       oyImage_SetLine_f   setLine,
                                       oyImage_SetTile_f   setTile );
int            oyImage_FillArray     ( oyImage_s         * image,
                                       oyRectangle_s     * rectangle,
                                       int                 do_copy,
                                       oyArray2d_s      ** array,
                                       oyRectangle_s     * array_rectangle,
                                       oyObject_s          obj );
int            oyImage_ReadArray     ( oyImage_s         * image,
                                       oyRectangle_s     * rectangle,
                                       oyArray2d_s       * array,
                                       oyRectangle_s     * array_rectangle );
oyPixel_t      oyImage_PixelLayoutGet( oyImage_s         * image );
oyProfile_s *  oyImage_ProfileGet    ( oyImage_s         * image );
oyOptions_s *  oyImage_TagsGet       ( oyImage_s         * image );



typedef struct oyCMMapi4_s oyCMMapi4_s;
typedef struct oyCMMapi6_s oyCMMapi6_s;
typedef struct oyCMMapi7_s oyCMMapi7_s;
typedef struct oyCMMapi8_s oyCMMapi8_s;
typedef struct oyCMMapi9_s oyCMMapi9_s;
typedef struct oyCMMapi10_s oyCMMapi10_s;
typedef struct oyCMMapiFilter_s oyCMMapiFilter_s;
typedef struct oyFilterCore_s oyFilterCore_s;
typedef struct oyFilterCores_s oyFilterCores_s;
typedef struct oyFilterGraph_s oyFilterGraph_s;
typedef struct oyFilterNode_s oyFilterNode_s;
typedef struct oyFilterNodes_s oyFilterNodes_s;
typedef struct oyConnector_s oyConnector_s;
typedef struct oyConnectorImaging_s oyConnectorImaging_s;
typedef struct oyFilterPlug_s oyFilterPlug_s;
typedef struct oyFilterPlugs_s oyFilterPlugs_s;
typedef struct oyFilterSocket_s oyFilterSocket_s;
typedef struct oyPixelAccess_s oyPixelAccess_s;
typedef struct oyConversion_s oyConversion_s;
typedef struct oyNamedColour_s oyNamedColour_s;
typedef struct oyNamedColours_s oyNamedColours_s;
typedef struct oyIcon_s oyIcon_s;
typedef struct oyCMMInfo_s oyCMMInfo_s;
typedef struct oyCMMapiFilters_s oyCMMapiFilters_s;
typedef struct oyCMMapis_s oyCMMapis_s;
typedef struct oyUiHandler_s oyUiHandler_s;

/**
 *  type:
 *  - oyNAME_NICK: compact, e.g. "GPU"
 *  - oyNAME_NAME: a short explanation, e.g. "Rendering with GPU"
 *  - oyNAME_DESCRIPTION: a long explanation, e.g. "Accelerated calculations on a GPU"
 *
 *  @param         select              one from "name", "manufacturer" or "copyright"
 *  @param         type                select flavour
 *  @param         object              the object to ask for a optional context
 *                                     This will typical be the object to which
 *                                     this function belongs, but can be stated
 *                                     otherwise.
 *  @return                            text string or zero
 *
 *  @version Oyranos: 0.3.0
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2011/02/01
 */
typedef
const char *    (* oyCMMGetText_f)   ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );

/** typedef  oyCMMFilterSocket_MatchPlugIn_f
 *  @brief   verify connectors matching each other
 *  @ingroup module_api
 *  @memberof oyCMMapi5_s
 *
 *  A implementation for images is included in the core function
 *  oyFilterSocket_MatchImagePlug().
 *
 *  @param         socket              a filter socket
 *  @param         plug                a filter plug
 *  @return                            1 on success, otherwise 0
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/04/20 (Oyranos: 0.1.10)
 *  @date    2009/04/20
 */
typedef int          (*oyCMMFilterSocket_MatchPlug_f) (
                                       oyFilterSocket_s  * socket,
                                       oyFilterPlug_s    * plug );

/** @enum    oyCONNECTOR_e
 *  @brief   basic connector attributes
 *  @ingroup objects_conversion
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/00/00 (Oyranos: 0.1.8)
 *  @date    2009/04/28
 */
typedef enum {
  /** a data manipulator. e.g. a normal filter - "//imaging/manipulator" */
  oyCONNECTOR_IMAGE_MANIPULATOR,
  /** a data generator, e.g. checkerboard, gradient "//imaging/generator" */
  oyCONNECTOR_IMAGE_GENERATOR,
  /** a pixel data provider, e.g. image data connector "//imaging/data".
   *  This type should be always present to connect processing data.
   *  That data is stored in oyFilterSocket_s::data. */
  oyCONNECTOR_IMAGE,
  /** observer, a endpoint, only input, e.g. text log, thumbnail viewer 
   *  "//imaging/observer" */
  oyCONNECTOR_IMAGE_OBSERVER,
  /** a routing element, without data altering "//imaging/splitter.rectangle" */
  oyCONNECTOR_IMAGE_SPLITTER,
  /** combines or splits image data, e.g. blending "//imaging/blender.rectangle"*/
  oyCONNECTOR_IMAGE_COMPOSITOR,

  /** converts pixel layout to other formats "//imaging/pixel.convertor" */
  oyCONNECTOR_CONVERTOR_PIXELDATA,
  /** converts pixel layout to other formats, with precission loss, e.g. 
   *  float -> uint8_t, only relevant for output connectors 
   *  "//imaging/pixel.convertor.lossy" */
  oyCONNECTOR_CONVERTOR_PIXELDATA_LOSSY,
  /** combines gray channels, e.g. from colour "//imaging/combiner.channels" */
  oyCONNECTOR_COMPOSITOR_CHANNEL,
  /** provides gray scale views of channels "//imaging/splitter.channels" */
  oyCONNECTOR_SPLITTER_CHANNEL,

  /** provides values or text, only output "///analysis" */
  oyCONNECTOR_ANALYSIS
} oyCONNECTOR_e;

/** @enum    oyCONNECTOR_EVENT_e
 *  @brief   connector events types
 *  @ingroup objects_conversion
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/00/00 (Oyranos: 0.1.8)
 *  @date    2008/00/00
 */
typedef enum {
  oyCONNECTOR_EVENT_OK,                /**< kind of ping */
  oyCONNECTOR_EVENT_CONNECTED,         /**< connection established */
  oyCONNECTOR_EVENT_RELEASED,          /**< released the connection */
  oyCONNECTOR_EVENT_DATA_CHANGED,      /**< call to update image views */
  oyCONNECTOR_EVENT_STORAGE_CHANGED,   /**< new data accessors */
  oyCONNECTOR_EVENT_INCOMPATIBLE_DATA, /**< can not process image */
  oyCONNECTOR_EVENT_INCOMPATIBLE_OPTION,/**< can not handle option */
  oyCONNECTOR_EVENT_INCOMPATIBLE_CONTEXT,/**< can not handle profile */
  oyCONNECTOR_EVENT_INCOMPLETE_GRAPH   /**< can not completely process */ 
} oyCONNECTOR_EVENT_e;

/** @struct  oyConnector_s
 *  @brief   a filter connection description structure
 *  @ingroup objects_conversion
 *  @extends oyStruct_s
 *
 *  This structure holds informations about the connection capabilities.
 *  It holds common structure members of oyFilterPlug_s and oyFilterSocket_s.
 *
 *  To signal a value is not initialised or does not apply, set the according
 *  integer value to -1.
 *
 *  @todo generalise the connector properties
 *
 *  @version Oyranos: 0.3.0
 *  @since   2008/07/26 (Oyranos: 0.1.8)
 *  @date    2011/01/31
 */
struct oyConnector_s {
  oyOBJECT_e           type_;          /**< @private struct type oyOBJECT_CONNECTOR_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< @private base object */

  /** Support at least "name" for UIs. */
  oyCMMGetText_f       getText;
  char              ** texts;          /**< zero terminated list for getText */

  char               * connector_type; /**< a @ref registration string */
  /** Check if two oyCMMapi7_s filter connectors of type oyConnector_s can 
   *  match each other inside a given socket and a plug. */
  oyCMMFilterSocket_MatchPlug_f  filterSocket_MatchPlug;

  /** make requests and receive data, by part of oyFilterPlug_s */
  int                  is_plug;
};

/** @struct  oyFilterSocket_s
 *  @brief   a filter connection structure
 *  @ingroup objects_conversion
 *  @extends oySocket_s
 *
 *  The passive output version of a oyConnector_s.
 \dot
digraph G {
  bgcolor="transparent";
  node[ shape=plaintext, fontname=Helvetica, fontsize=10 ];
  edge[ fontname=Helvetica, fontsize=10 ];
  rankdir=LR
  a [label=<
<table border="0" cellborder="1" cellspacing="4">
  <tr> <td>Filter A</td>
      <td bgcolor="red" width="10" port="s"> socket </td>
  </tr>
</table>>
  ]
  b [URL="structoyFilterPlug__s.html", label=< 
<table border="0" cellborder="1" cellspacing="4">
  <tr><td bgcolor="lightblue" width="10" port="p"> plug </td>
      <td>Filter B</td>
  </tr>
</table>>
  ]
  subgraph { rank=min a }

  b:p->a:s [arrowtail=crow, arrowhead=box];
} 
 \enddot
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/29 (Oyranos: 0.1.8)
 *  @date    2008/07/29
 */
struct oyFilterSocket_s {
  oyOBJECT_e           type_;          /**< @private struct type oyOBJECT_FILTER_SOCKET_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< @private base object */

  oyFilterNode_s     * node;           /**< filter node for this connector */
  oyFilterPlugs_s    * requesting_plugs_;/**< @private all remote inputs */
  oyStruct_s         * data;           /**< unprocessed data model */

  oyConnector_s      * pattern;        /**< a pattern the filter node can handle through this connector */
  char               * relatives_;     /**< @private hint about belonging to a filter */

};

OYAPI oyFilterSocket_s * OYEXPORT
                 oyFilterSocket_New  ( oyObject_s          object );
OYAPI oyFilterSocket_s * OYEXPORT
                 oyFilterSocket_Copy ( oyFilterSocket_s  * list,
                                       oyObject_s          object);
OYAPI int  OYEXPORT
                 oyFilterSocket_Release (
                                       oyFilterSocket_s ** list );


OYAPI int  OYEXPORT
                 oyFilterSocket_Callback (
                                       oyFilterPlug_s    * c,
                                       oyCONNECTOR_EVENT_e e );
OYAPI int  OYEXPORT
                 oyFilterSocket_SignalToGraph (
                                       oyFilterSocket_s  * c,
                                       oyCONNECTOR_EVENT_e e );

/** @struct oyFilterPlug_s
 *  @brief  a filter connection structure
 *  @ingroup objects_conversion
 *  @extends oyPlug_s
 *
 *  The active input version of a oyConnector_s.
 *  Each plug can connect to exact one socket.
 \dot
digraph G {
  bgcolor="transparent";
  node[ shape=plaintext, fontname=Helvetica, fontsize=10 ];
  edge[ fontname=Helvetica, fontsize=10 ];
  rankdir=LR
  a [URL="structoyFilterSocket__s.html", label=<
<table border="0" cellborder="1" cellspacing="4">
  <tr> <td>Filter A</td>
      <td bgcolor="red" width="10" port="s"> socket </td>
  </tr>
</table>>
  ]
  b [label=< 
<table border="0" cellborder="1" cellspacing="4">
  <tr><td bgcolor="lightblue" width="10" port="p"> plug </td>
      <td>Filter B</td>
  </tr>
</table>>
  ]
  subgraph { rank=min a }

  b:p->a:s [arrowtail=crow, arrowhead=box];
} 
 \enddot
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/29 (Oyranos: 0.1.8)
 *  @date    2008/07/29
 */
struct oyFilterPlug_s {
  oyOBJECT_e           type_;          /**< @private struct type oyOBJECT_FILTER_PLUG_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< @private base object */

  oyFilterNode_s     * node;           /**< filter node for this connector */
  oyFilterSocket_s   * remote_socket_; /**< @private the remote output */

  oyConnector_s      * pattern;        /**< a pattern the filter node can handle through this connector */
  char               * relatives_;     /**< @private hint about belonging to a filter */
};

/** @struct  oyFilterPlugs_s
 *  @brief   a FilterPlugs list
 *  @ingroup objects_conversion
 *  @extends oyPlugs_s
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/29 (Oyranos: 0.1.8)
 *  @date    2008/07/29
 */
struct oyFilterPlugs_s {
  oyOBJECT_e           type_;          /**< @private struct type oyOBJECT_FILTER_PLUGS_S */ 
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< @private base object */

  oyStructList_s     * list_;          /**< @private the list data */
};

OYAPI oyFilterPlugs_s * OYEXPORT
                 oyFilterPlugs_New   ( oyObject_s          object );
OYAPI oyFilterPlugs_s * OYEXPORT
                 oyFilterPlugs_Copy  ( oyFilterPlugs_s   * list,
                                       oyObject_s          object);
OYAPI int  OYEXPORT
                 oyFilterPlugs_Release (
                                       oyFilterPlugs_s  ** list );


OYAPI oyFilterPlugs_s * OYEXPORT
                 oyFilterPlugs_MoveIn( oyFilterPlugs_s   * list,
                                       oyFilterPlug_s   ** ptr,
                                       int                 pos );
OYAPI int  OYEXPORT
                 oyFilterPlugs_ReleaseAt (
                                       oyFilterPlugs_s   * list,
                                       int                 pos );
OYAPI oyFilterPlug_s * OYEXPORT
                 oyFilterPlugs_Get   ( oyFilterPlugs_s   * list,
                                       int                 pos );
OYAPI int  OYEXPORT
                 oyFilterPlugs_Count ( oyFilterPlugs_s   * list );


/** @struct oyFilterCore_s
 *  @brief  a basic filter to manipulate data
 *  @ingroup objects_conversion
 *  @extends oyStruct_s
 *
 *  This is the Oyranos filter object. Filters are categorised into basic
 *  classes of filters described in the registration_ (//xxx) member.
 *  Filters implement a container for data and options.
 *  Filters can be manipulated by changing their options or data set.
 *
 *  Filters are chained into a oyConversion_s in order to get applied to data.
 *  The relation of filters in a graph is defined through the oyFilterNode_s
 *  struct.
 *
 *  It is possible to chain filters in different ways together. The aimed way
 *  here is to use members and queries to find possible connections. For 
 *  instance a one in one out filter can not be connected to two sources at 
 *  once.
 *
 *  The registration_ describes different basic types of filters (//xxx).
 *  See oyranos::oyCONNECTOR_e.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/08 (Oyranos: 0.1.8)
 *  @date    2009/11/17
 */
struct oyFilterCore_s {
  oyOBJECT_e           type_;          /**< @private struct type oyOBJECT_FILTER_S*/
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< @private base object */

  char               * registration_;  /**< @private a registration name, e.g. "shared/oyranos.org/imaging/scale", see as well @ref registration */

  char               * category_;      /**< @private the ui menue category for this filter, to be specified */

  oyOptions_s        * options_;       /**< @private local options */

  oyCMMapi4_s        * api4_;          /**< @private oyranos library interfaces */
};

#define OY_FILTER_SET_TEST             0x01        /** only test */
#define OY_FILTER_GET_DEFAULT          0x01        /** defaults */
/* decode */
#define oyToFilterSetTest_m(r)         ((r)&1)
#define oyToFilterGetDefaults_m(r)     ((r)&1)
oyOptions_s* oyFilterCore_OptionsSet ( oyFilterCore_s    * filter,
                                       oyOptions_s       * options,
                                       int                 flags );
const char * oyFilterCore_WidgetsSet ( oyFilterCore_s    * filter,
                                       const char        * widgets,
                                       int                 flags );
const char * oyFilterCore_WidgetsGet ( oyFilterCore_s    * filter,
                                       int                 flags );


/** @struct  oyFilterCores_s
 *  @brief   a FilterCore list
 *  @ingroup objects_conversion
 *  @extends oyStruct_s
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/07/08 (Oyranos: 0.1.8)
 *  @date    2009/02/28
 */
struct  oyFilterCores_s {
  oyOBJECT_e           type_;          /**< @private struct type oyOBJECT_FILTER_CORES_S */ 
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< @private base object */

  oyStructList_s     * list_;          /**< @private the list data */
};

OYAPI oyFilterCores_s * OYEXPORT
                 oyFilterCores_New   ( oyObject_s          object );
OYAPI oyFilterCores_s * OYEXPORT
                 oyFilterCores_Copy  ( oyFilterCores_s   * list,
                                       oyObject_s          object);
OYAPI int  OYEXPORT
                 oyFilterCores_Release(oyFilterCores_s  ** list );


OYAPI oyFilterCores_s * OYEXPORT
                 oyFilterCores_MoveIn( oyFilterCores_s   * list,
                                       oyFilterCore_s   ** ptr,
                                       int                 pos );
OYAPI int  OYEXPORT
                 oyFilterCores_ReleaseAt (
                                       oyFilterCores_s   * list,
                                       int                 pos );
OYAPI oyFilterCore_s * OYEXPORT
                 oyFilterCores_Get   ( oyFilterCores_s   * list,
                                       int                 pos );
OYAPI int  OYEXPORT
                 oyFilterCores_Count ( oyFilterCores_s   * list );



/** @struct  oyFilterNode_s
 *  @brief   a FilterNode object
 *  @ingroup objects_conversion
 *  @extends oyNode_s
 *
 *  Filter nodes chain filters into a oyConversion_s graph. The filter nodes
 *  use plugs and sockets for creating connections. Each plug can only connect
 *  to one socket.
 \dot
digraph G {
  bgcolor="transparent";
  node[ shape=plaintext, fontname=Helvetica, fontsize=10 ];
  a [label=<
<table border="0" cellborder="1" cellspacing="4">
  <tr> <td>oyFilterCore_s A</td>
      <td bgcolor="red" width="10" port="s"> socket </td>
  </tr>
</table>>
  ]
  b [label=<
<table border="0" cellborder="1" cellspacing="4">
  <tr><td bgcolor="lightblue" width="10" port="p"> plug </td>
      <td>oyFilterCore_s B</td>
  </tr>
</table>>
  ]

  b:p->a:s [arrowtail=crow, arrowhead=box, constraint=false];

  subgraph cluster_0 {
    color=gray;
    label="FilterNode A";
    a;
  }
  subgraph cluster_1 {
    color=gray;
    label="FilterNode B";
    b;
  }
}
 \enddot
 *
 *  This object provides support for separation of options from chaining.
 *  So it will be possible to implement options changing, which can affect
 *  the same filter instance in different graphs.
 *
 *  A oyFilterNode_s can have various oyFilterPlug_s ' to obtain data from
 *  different sources. The required number is described in the oyCMMapi4_s 
 *  structure, which is part of oyFilterCore_s.
 \dot
digraph G {
  bgcolor="transparent";
  rankdir=LR
  node [shape=record, fontname=Helvetica, fontsize=10, style="rounded"];
  edge [fontname=Helvetica, fontsize=10];

  b [ label="{<plug> | Filter Node 2 |<socket>}"];
  c [ label="{<plug> | Filter Node 3 |<socket>}"];
  d [ label="{<plug> 2| Filter Node 4 |<socket>}"];

  b:socket -> d:plug [arrowtail=normal, arrowhead=none];
  c:socket -> d:plug [arrowtail=normal, arrowhead=none];
}
 \enddot
 *
 *  oyFilterSocket_s is designed to accept arbitrary numbers of connections 
 *  to allow for viewing on a filters data output or observe its state changes.
 \dot
digraph G {
  bgcolor="transparent";
  rankdir=LR
  node [shape=record, fontname=Helvetica, fontsize=10, style="rounded"];
  edge [fontname=Helvetica, fontsize=10];

  a [ label="{<plug> | Filter Node 1 |<socket>}"];
  b [ label="{<plug> 1| Filter Node 2 |<socket>}"];
  c [ label="{<plug> 1| Filter Node 3 |<socket>}"];
  d [ label="{<plug> 1| Filter Node 4 |<socket>}"];
  e [ label="{<plug> 1| Filter Node 5 |<socket>}"];

  a:socket -> b:plug [arrowtail=normal, arrowhead=none];
  a:socket -> c:plug [arrowtail=normal, arrowhead=none];
  a:socket -> d:plug [arrowtail=normal, arrowhead=none];
  a:socket -> e:plug [arrowtail=normal, arrowhead=none];
}
 \enddot
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/07/08 (Oyranos: 0.1.8)
 *  @date    2008/12/16
 */
struct oyFilterNode_s {
  oyOBJECT_e           type_;          /**< @private struct type oyOBJECT_FILTER_NODE_S*/
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< @private base object */

  oyFilterPlug_s    ** plugs;          /**< possible input connectors */
  int                  plugs_n_;       /**< readonly number of inputs */
  oyFilterSocket_s  ** sockets;        /**< possible output connectors */
  int                  sockets_n_;     /**< readonly number of outputs */

  oyFilterCore_s     * core;           /**< the filter core */
  char               * relatives_;     /**< @private hint about belonging to a filter */
  oyOptions_s        * tags;           /**< infos, e.g. group markers */

  /** the filters private data, requested over 
   *  oyCMMapi4_s::oyCMMFilterNode_ContextToMem() and converted to
   *  oyCMMapi4_s::context_type */
  oyPointer_s        * backend_data;
  /** the processing function and node connector descriptions */
  oyCMMapi7_s        * api7_;
};

#define OY_FILTEREDGE_FREE             0x01        /**< list free edges */
#define OY_FILTEREDGE_CONNECTED        0x02        /**< list connected edges */
#define OY_FILTEREDGE_LASTTYPE         0x04        /**< list last type edges */
/* decode */
#define oyToFilterEdge_Free_m(r)       ((r)&1)
#define oyToFilterEdge_Connected_m(r)  (((r) >> 1)&1)
#define oyToFilterEdge_LastType_m(r)   (((r) >> 2)&1)
void oyShowGraph_( oyFilterNode_s * c, const char * selector );


/** @struct  oyFilterNodes_s
 *  @brief   a FilterNodes list
 *  @extends oyStruct_s
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */
struct oyFilterNodes_s {
  oyOBJECT_e           type_;          /**< struct type oyOBJECT_FILTER_NODES_S */ 
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  oyStructList_s     * list_;          /**< the list data */
};

OYAPI oyFilterNodes_s * OYEXPORT
           oyFilterNodes_New         ( oyObject_s          object );
OYAPI oyFilterNodes_s * OYEXPORT
           oyFilterNodes_Copy        ( oyFilterNodes_s   * list,
                                       oyObject_s          object);
OYAPI int  OYEXPORT
           oyFilterNodes_Release     ( oyFilterNodes_s  ** list );


OYAPI int  OYEXPORT
           oyFilterNodes_MoveIn      ( oyFilterNodes_s   * list,
                                       oyFilterNode_s   ** ptr,
                                       int                 pos );
OYAPI int  OYEXPORT
           oyFilterNodes_ReleaseAt   ( oyFilterNodes_s   * list,
                                       int                 pos );
OYAPI oyFilterNode_s * OYEXPORT
           oyFilterNodes_Get         ( oyFilterNodes_s   * list,
                                       int                 pos );
OYAPI int  OYEXPORT
           oyFilterNodes_Count       ( oyFilterNodes_s   * list );



/** @struct  oyFilterGraph_s
 *  @brief   a FilterGraph object
 *  @extends oyStruct_s
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */
struct oyFilterGraph_s {
  oyOBJECT_e           type_;          /**< struct type oyOBJECT_FILTER_GRAPH_S */ 
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  oyFilterNodes_s    * nodes;          /**< the nodes in the graph */
  oyFilterPlugs_s    * edges;          /**< the edges in the graph */
  oyOptions_s        * options;        /**< options, "dirty" ... */
};

OYAPI oyFilterGraph_s * OYEXPORT
           oyFilterGraph_New         ( oyObject_s          object );
OYAPI oyFilterGraph_s * OYEXPORT
           oyFilterGraph_FromNode    ( oyFilterNode_s    * node,
                                       int                 flags );
OYAPI oyFilterGraph_s * OYEXPORT
           oyFilterGraph_Copy        ( oyFilterGraph_s   * obj,
                                       oyObject_s          object);
OYAPI int  OYEXPORT
           oyFilterGraph_Release     ( oyFilterGraph_s  ** obj );

OYAPI int  OYEXPORT
           oyFilterGraph_PrepareContexts (
                                       oyFilterGraph_s   * graph,
                                       int                 flags );
OYAPI oyFilterNode_s * OYEXPORT
           oyFilterGraph_GetNode     ( oyFilterGraph_s   * graph,
                                       int                 pos,
                                       const char        * registration,
                                       const char        * mark );
OYAPI int  OYEXPORT
           oyFilterGraph_SetFromNode ( oyFilterGraph_s   * graph,
                                       oyFilterNode_s    * node,
                                       const char        * mark,
                                       int                 flags );
OYAPI char * OYEXPORT
           oyFilterGraph_ToText      ( oyFilterGraph_s   * graph,
                                       oyFilterNode_s    * input,
                                       oyFilterNode_s    * output,
                                       const char        * head_line,
                                       int                 reserved,
                                       oyAlloc_f           allocateFunc );
oyBlob_s * oyFilterGraph_ToBlob      ( oyFilterGraph_s   * graph,
                                       int                 node_pos,
                                       oyObject_s          object );


/** @struct  oyPixelAccess_s
 *  @brief   control pixel access order
 *  @ingroup objects_conversion
 *  @extends oyStruct_s
 *
 *  A struct to control pixel access. It is a kind of flexible pixel 
 *  iterator. The order or pattern of access is defined by the [array_xy and]
 *  start_[x,y] variables.
 *
 *  oyPixelAccess_s is like a job ticket. Goal is to maintain all intermediate
 *  and processing dependend memory references in this structure.
 *
 * [The index variable specifies the iterator position in the array_xy index
 *  array.]
 *
 * [pixels_n says how many pixels are to be processed for the cache.
 *  pixels_n is used to calculate the buffers located with getBuffer
 *  and freeBuffer.
 *  The amount of pixel specified in pixels_n must be processed by
 *  each filter, because other filters are relying on a properly filled cache.
 *  This variable also determins the size of the next iteration.]
 *
 * [The relation of pixels_n to array_xy and start_[x,y] is that a
 *  minimum of pixels_n must be processed by starting with start_[x,y]
 *  and processing pixels_n through array_xy. array_xy specifies
 *  the offset pixel distance to a next pixel in x and y directions. In case
 *  pixels_n is larger than array_n the array_xy has to be continued
 *  at array_xy[0,1] after reaching its end (array_n). \n
 *  \b Example: \n
 *  Thus a line iterator behaviour can be specified by simply setting 
 *  array_xy = {1,0}, for a advancement in x direction of one, array_n = 1, 
 *  as we need just this type of advancement and pixels_n = image_width, 
 *  for saying how often the pattern descibed in array_xy has to be applied.]
 *
 *  Handling of pixel access is to be supported by a filter in a function of
 *  type oyCMMFilter_GetNext_f() in oyCMMapi4_s::oyCMMConnector_GetNext().
 *
 *  Access to the buffers by concurrenting threads is handled by passing
 *  different oyPixelAccess_s objects per thread.
 *
 *  From the module point of view it is a requirement to obtain the 
 *  intermediate buffers from somewhere. These are the ones to read from and
 *  to write computed results into. \n
 *
 *  Pixel in- and output buffers separation:
 *  - Copy the output area and request to manipulate it by each filter.
 *    There is no overwriting of results.
 *    Reads a bit fixed. How can filters decide upon the input size?
 *    However, if a filter works on more than one dimension, it can
 *    opt to get its area directly from a input mediator.
 *  -[Provide a opaque output and input area and request to copy by each filter.
 *    Filters would overwrite previous manipulations or some mechanism of
 *    swapping the input with the output side is needed.]
 *  - Some filters want different input and output areas. They see the mediator
 *    as the previous, or the input, element in the graph.
 *  - Will the mediators always be visible in order to get all informations
 *    about the image? During setting up the graph this should be handled.
 *
 *  Access to input and output buffers:
 *  - The output oyArray2d_s is to be reserved only.
 *  - The input oyArray2d_s is to be provided for multi dimensional
 *    manipulators directly from the input mediator.
 *
 *  Thread synchronisation:
 *  - The oyArray2d_s is a opaque memory blob. So different filters can act upon
 *    this resource. It would be in the resposiblity of the graph to avoid
 *    conflicts like using the same output for different manipulations. Given
 *    that the output is acting actively, the potential is small anyway.
 *  - The input should be neutral and not directly manipulated. What can happen
 *    is that different threads request the same input area and the according
 *    data is to be rendered first. So this easily could end in rendering two
 *    times for the same result. Some scheduling in the mediators may help
 *    solving this and improove on performance.
 *
 *  Area dimensions:
 *  - One point is very simple to provide. It may easily require additional
 *    preparations for area manipulations like blur.
 *  - Line is the next hard. The advantage it is still simple and speed
 *    efficient. Programming is a bit more demanding.
 *  - Areas of pixel are easy to provide through oyArray2d_s. It can include the
 *    above two cases.
 *  - Pattern accessors are very flexible for manipulators. It's not clear how
 *    the resulting complexity of translating the pattern to a array with known
 *    pixel positions can be hidden from other filters, which need to know about
 *    positions. One strategy would be to use mediators. They can request the
 *    according pixels from previous filters. A function to convert the pattern
 *    to a list of positions should be provided. Very elegant, but probably
 *    better to do later after oyArray2d_s.
 *
 *  Possible strategies are (old text):
 *  - Use mediators to convert between different pixel layouts and areas.
 *    These could cache the record of a successful query. Mediators are Nodes in
 *    the graph. As the graph and thus mediators can be accessed over
 *    concurrenting entries a cache tends to be expensive.
 *  - The oyPixelAccess_s could hold caches instead of mediators. It is the
 *    structure, which is owned by a given thread anyway.
 *    oyPixelAccess_s needs two buffers one for input and one for output.
 *    As the graph is asked to provide the next pixel via a oyPixelAccess_s
 *    struct, this struct must be associated with source and destination 
 *    buffers. The mediator on output has to search through the chain for the 
 *    previous
 *    mediator and ask there for the input buffer. The ouput buffer is provided
 *    by this mediator itself. These two buffers are set as the actual ones for
 *    processing by the normal filters. It must be clear, what is a mediator,
 *    for this scheme to work. As a mediator is reached in the processing graph,
 *    its task is not only to convert between buffers but as well to update the
 *    oyPixelAccess_s struct with the next mediators and its own buffer. Thus
 *    the next inbetween filters can process on their part.
 *    One advantage is that the mediators can pass their buffers to 
 *    oyPixelAccess_s, which are independent to threads and can be shared.
 *  - Each filter obtains a buffer being asked to fill it with the pixels 
 *    positions described in oyPixelAccess_s. A filter is free to create a new
 *    oyPixelAccess_s description and obtain for instance the surounding of the
 *    requested pixels. There is no caching to be expected other than in 
 *    the oyPixelAccess_s own output buffer.
 *
 *  @todo clear about pixel buffer copying, how to reach the buffers, thread
 *        synchronisation, simple or complex pixel areas (point, line, area,
 *        pattern )
 *
 *  @verbatim
    Relation of positional parameters:

                start_xy         output_image_roi
                   |                /
             +-----|---------------/--------------+
    original |     |              /               |
    image ---+     |             /                |
             |  ---+------------/----------+      |
             |     |           /           +---------- output_image
             |     |   +------+--------+   |      |
             |     |   |               |   |      |
             |     |   |               |   |      |
             |     |   +---------------+   |      |
             |     |                       |      |
             |     +-----------------------+      |
             |                                    |
             +------------------------------------+
    @endverbatim
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/07/04 (Oyranos: 0.1.8)
 *  @date    2009/05/05
 */
struct oyPixelAccess_s {
  oyOBJECT_e           type;           /**< internal struct type oyOBJECT_PIXEL_ACCESS_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< @private base object */

  double           start_xy[2];        /**< the start point of output_image */
  double           start_xy_old[2];    /**< @deprecated the previous start point */
  int32_t        * array_xy;           /**< @deprecated array of shifts, e.g. 1,0,2,0,1,0 */
  int              array_n;            /**< @deprecated the number of points in array_xy */

  int              index;              /**< @deprecated to be advanced by the last caller */
  size_t           pixels_n;           /**< @deprecated pixels to process/cache at once; should be set to 0 or 1 */

  int32_t          workspace_id;       /**< a ID to assign distinct resources to */
  oyStruct_s     * user_data;          /**< user data, e.g. for error messages*/
  oyArray2d_s    * array;              /**< processing data. The position is in
                                            start_xy relative to the previous
                                            mediator in the graph. */
  oyRectangle_s  * output_image_roi;   /**< rectangle of interesst; The
                                            rectangle is to be seen in relation
                                            to the
                                            output_image (of the last filter).*/
  oyImage_s      * output_image;       /**< the image which issued the request*/
  oyFilterGraph_s * graph;             /**< the graph to process */
  oyOptions_s    * request_queue;      /**< messaging; requests to resolve */
};

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

oyPixelAccess_s *  oyPixelAccess_Create (
                                       int32_t             start_x,
                                       int32_t             start_y,
                                       oyFilterPlug_s    * plug,
                                       oyPIXEL_ACCESS_TYPE_e type,
                                       oyObject_s          object );
oyPixelAccess_s *  oyPixelAccess_Copy( oyPixelAccess_s   * obj,
                                       oyObject_s          object );
int                oyPixelAccess_Release(
                                       oyPixelAccess_s  ** obj );
int                oyPixelAccess_ChangeRectangle ( 
                                       oyPixelAccess_s   * pixel_access,
                                       double              start_x,
                                       double              start_y,
                                       oyRectangle_s     * output_rectangle );

/** @struct oyConversion_s
 *  @brief  a filter chain or graph to manipulate a image
 *  @ingroup objects_conversion
 *  @extends oyStruct_s
 *
 *  Order of filters matters.
 *  The processing direction is a bit like raytracing as nodes request their
 *  parent.
 *
 *  The graph is allowed to be a directed graph without cycles.
 \dot
digraph G {
  bgcolor="transparent";
  rankdir=LR
  graph [fontname=Helvetica, fontsize=12];
  node [shape=record, fontname=Helvetica, fontsize=10, style="filled,rounded"];
  edge [fontname=Helvetica, fontsize=10];

  a [ label="{<plug> 0| Filter Node 1 == Input |<socket>}"];
  b [ label="{<plug> 1| Filter Node 2 |<socket>}"];
  c [ label="{<plug> 1| Filter Node 3 |<socket>}"];
  d [ label="{<plug> 2| Filter Node 4 == Output |<socket>}"];

  subgraph cluster_0 {
    label="Oyranos Filter Graph";
    color=gray;

    a:socket -> b:plug [arrowtail=normal, arrowhead=none, label=request];
    b:socket -> d:plug [arrowtail=normal, arrowhead=none, label=request];
    a:socket -> c:plug [arrowtail=normal, arrowhead=none, label=request];
    c:socket -> d:plug [arrowtail=normal, arrowhead=none, label=request];
  }
}
 \enddot
 *  oyConversion_s shall provide access to the graph and help in processing
 *  and managing nodes.\n
 \dot
digraph G {
  bgcolor="transparent";
  rankdir=LR
  graph [fontname=Helvetica, fontsize=12];
  node [shape=record, fontname=Helvetica, fontsize=10, style="rounded"];
  edge [fontname=Helvetica, fontsize=10];

  conversion [shape=plaintext, label=<
<table border="0" cellborder="1" cellspacing="0" bgcolor="lightgray">
  <tr><td>oyConversion_s</td></tr>
  <tr><td>
     <table border="0" cellborder="0" align="left">
       <tr><td align="left">...</td></tr>
       <tr><td align="left" port="in">+input</td></tr>
       <tr><td align="left" port="out">+out_</td></tr>
       <tr><td align="left">...</td></tr>
     </table>
     </td></tr>
  <tr><td> </td></tr>
</table>>,
                    style=""];

  a [ label="{<plug> 0| Filter Node 1 == Input |<socket>}"];
  b [ label="{<plug> 1| Filter Node 2 |<socket>}"];
  c [ label="{<plug> 1| Filter Node 3 |<socket>}"];
  d [ label="{<plug> 2| Filter Node 4 == Output |<socket>}"];

  subgraph cluster_0 {
    label="oyConversion_s with attached Filter Graph";
    color=gray;

    a:socket -> b:plug [arrowtail=normal];
    b:socket -> d:plug [arrowtail=normal];
    a:socket -> c:plug [arrowtail=normal];
    c:socket -> d:plug [arrowtail=normal];

    conversion:in -> a;
    conversion:out -> d;
  }

  conversion
}
 \enddot
 *  \b Creating \b Graphs: \n
 *  Most simple is to use the oyConversion_CreateBasicPixles() function to 
 *  create a profile to profile and possible image buffer to image buffer linear
 *  graph.\n
 *  The other possibility is to create a non linear graph. The input member can
 *  be accessed for this directly.
 *
 *  While it would be possible to have several open ends in a graph, there
 *  are two endpoints considered as special. The input member prepresents the
 *  top most required node to be provided in a oyConversion_s graph. The
 *  input node is accessible for user manipulation. The other one is the out_
 *  member. It is the closing node in the graph. It will be set by Oyranos
 *  during closing the graph, e.g. in oyConversion_LinOutputAdd().
 *
 *  \b Using \b Graphs: \n
 *  To obtain the data the oyConversion_GetNextPixel() and
 *  oyConversion_GetOnePixel() functions are available.
 \dot
digraph G {
  bgcolor="transparent";
  rankdir=LR
  graph [fontname=Helvetica, fontsize=12];
  node [shape=record, fontname=Helvetica, fontsize=10, style="rounded"];
  edge [fontname=Helvetica, fontsize=12];

  a [ label="{<plug> 0| Filter Node 1 == Input |<socket>}"];
  b [ label="{<plug> 1| Filter Node 2 |<socket>}"];
  c [ label="{<plug> 1| Filter Node 3 |<socket>}"];
  d [ label="{<plug> 2| Filter Node 4 == Output |<socket>}"];
  app [ label="application", style=filled ]

  subgraph cluster_0 {
    label="Data Flow";
    color=gray;
    a:socket -> b:plug [label=data];
    b:socket -> d:plug [label=data];
    a:socket -> c:plug [label=data];
    c:socket -> d:plug [label=data];
    d:socket -> app [label=<<table  border="0" cellborder="0"><tr><td>return of<br/>oyConversion_GetNextPixel()</td></tr></table>>];
  }
}
 \enddot
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/08 (Oyranos: 0.1.8)
 *  @date    2008/06/08
 */
struct oyConversion_s {
  oyOBJECT_e           type_;          /**< @private struct type oyOBJECT_CONVERSION_S*/
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< @private base object */

  oyFilterNode_s     * input;          /**< the input image filter; Most users will start logically with this pice and chain their filters to get the final result. */
  oyFilterNode_s     * out_;           /**< @private the Oyranos output image. Oyranos will stream the filters starting from the end. This element will be asked on its first plug. */
};

oyConversion_s *   oyConversion_New  ( oyObject_s          object );
oyConversion_s  *  oyConversion_CreateBasicPixels (
                                       oyImage_s         * input,
                                       oyImage_s         * output,
                                       oyOptions_s       * options,
                                       oyObject_s          object );
oyConversion_s *   oyConversion_CreateBasicPixelsFromBuffers (
                                       oyProfile_s       * p_in,
                                       oyPointer           buf_in,
                                       oyDATATYPE_e        buf_type_in,
                                       oyProfile_s       * p_out,
                                       oyPointer           buf_out,
                                       oyDATATYPE_e        buf_type_out,
                                       oyOptions_s       * options,
                                       int                 count );
oyConversion_s  *  oyConversion_Copy ( oyConversion_s    * conversion,
                                       oyObject_s          object );
int                oyConversion_Release (
                                       oyConversion_s   ** conversion );


int                oyConversion_Set  ( oyConversion_s    * conversion,
                                       oyFilterNode_s    * input,
                                       oyFilterNode_s    * output );
int                oyConversion_RunPixels (
                                       oyConversion_s    * conversion,
                                       oyPixelAccess_s   * pixel_access );
int                oyConversion_GetOnePixel (
                                       oyConversion_s    * conversion,
                                       double              x,
                                       double              y,
                                       oyPixelAccess_s   * pixel_access );
#define OY_OUTPUT                      0x01
#define OY_INPUT                       0x02
/* decode */
#define oyToOutput_m(r)                ((r)&1)
#define oyToInput_m(r)                 (((r) >> 1)&1)
oyImage_s        * oyConversion_GetImage (
                                       oyConversion_s    * conversion,
                                       uint32_t            flags );
oyFilterNode_s   * oyConversion_GetNode (
                                       oyConversion_s    * conversion,
                                       uint32_t            flags );
oyFilterGraph_s  * oyConversion_GetGraph (
                                       oyConversion_s    * conversion );
char             * oyConversion_ToText (
                                       oyConversion_s    * conversion,
                                       const char        * head_line,
                                       int                 reserved,
                                       oyAlloc_f           allocateFunc );
int                oyConversion_Correct (
                                       oyConversion_s    * conversion,
                                       const char        * registration,
                                       uint32_t            flags,
                                       oyOptions_s       * options );


/** @struct oyNamedColour_s
 *  @brief colour patch with meta informations
 *  @ingroup objects_single_colour
 *  @extends oyStruct_s
 *
 *  Data management on library side.
 *  User can control memory management at creation time.
 *
 *  It has the complexity of a object, and should not be accessed directly.
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/10/00 (Oyranos: 0.1.8)
 *  @date    2007/10/00
 *
 *  TODO: needs to be Xatom compatible
 */
struct oyNamedColour_s {
  oyOBJECT_e           type_;          /**< @private struct type oyOBJECT_NAMED_COLOUR_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< @private base object */
  double             * channels_;      /**< @private eigther parsed or calculated otherwise */
  double               XYZ_[3];        /**< @private CIE*XYZ representation */
  char               * blob_;          /**< @private advanced : CGATS / ICC ? */
  size_t               blob_len_;      /**< @private advanced : CGATS / ICC ? */
  oyProfile_s        * profile_;       /**< @private ICC */
};

oyNamedColour_s*  oyNamedColour_Create( const double      * chan,
                                        const char        * blob,
                                        int                 blob_len,
                                        oyProfile_s       * profile_ref,
                                        oyObject_s          object );
oyNamedColour_s*  oyNamedColour_CreateWithName (
                                       const char        * nick,
                                       const char        * name,
                                       const char        * description,
                                       const double      * chan,
                                       const double      * XYZ,
                                       const char        * blob,
                                       int                 blob_len,
                                       oyProfile_s       * profile_ref,
                                       oyObject_s          object );
oyNamedColour_s*  oyNamedColour_Copy ( oyNamedColour_s   * colour,
                                       oyObject_s          object );
int               oyNamedColour_Release ( oyNamedColour_s  ** colour );

oyProfile_s *     oyNamedColour_GetSpaceRef ( oyNamedColour_s  * colour );
void              oyNamedColour_SetChannels ( oyNamedColour_s * colour,
                                       const double      * channels,
                                       uint32_t            flags );
int               oyNamedColour_SetColourStd ( oyNamedColour_s * colour,
                                       oyPROFILE_e         colour_space,
                                       oyPointer           channels,
                                       oyDATATYPE_e        channels_type,
                                       uint32_t            flags,
                                       oyOptions_s       * options );
const double *    oyNamedColour_GetChannelsConst ( oyNamedColour_s * colour,
                                       uint32_t          * flags );
const double *    oyNamedColour_GetXYZConst      ( oyNamedColour_s * colour);
int               oyNamedColour_GetColourStd     ( oyNamedColour_s * colour,
                                       oyPROFILE_e         colour_space,
                                       oyPointer           buf,
                                       oyDATATYPE_e        buf_type,
                                       uint32_t          * flags,
                                       oyOptions_s       * options );
int               oyNamedColour_GetColour ( oyNamedColour_s * colour,
                                       oyProfile_s       * profile,
                                       oyPointer           buf,
                                       oyDATATYPE_e        buf_type,
                                       uint32_t            flags,
                                       oyOptions_s       * options );
const char   *    oyNamedColour_GetName( oyNamedColour_s * s,
                                       oyNAME_e            type,
                                       uint32_t            flags );

/** @brief list of colour patches
 *  @ingroup objects_single_colour
 *  @extends oyStruct_s
 *
 *  Data management on library side.
 *  User can control memory management at creation time.
 *
 *  It has the complexity of a object, and should not be accessed directly.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 *
 *  TODO: make the object non visible
 */
struct oyNamedColours_s {
  oyOBJECT_e           type_;          /*!< @private struct type oyOBJECT_NAMED_COLOURS_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /*!< @private base object */
  oyStructList_s     * list_;          /**< @private colour list */
};

oyNamedColours_s* oyNamedColours_New ( oyObject_s       object );
oyNamedColours_s* oyNamedColours_Copy( oyNamedColours_s  * colours,
                                       oyObject_s          object );
int               oyNamedColours_Release ( oyNamedColours_s** colours );

int               oyNamedColours_Count(oyNamedColours_s  * swatch );
oyNamedColour_s*  oyNamedColours_Get ( oyNamedColours_s  * swatch,
                                       int                 position);
oyNamedColours_s* oyNamedColours_MoveIn ( oyNamedColours_s  * list,
                                       oyNamedColour_s  ** obj,
                                       int                 pos );
int               oyNamedColours_ReleaseAt ( oyNamedColours_s * swatch,
                                       int                 position );




void              oyCopyColour       ( const double      * from,
                                       double            * to,
                                       int                 n,
                                       oyProfile_s       * profile_ref,
                                       int                 channels_n );




/* --- CMM API --- */

/** @brief   icon data
 *  @ingroup cmm_handling
 *  @extends oyStruct_s
 *
 *  Since: 0.1.8
 */
struct oyIcon_s {
  oyOBJECT_e       type;               /*!< struct type oyOBJECT_ICON_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyPointer        dummy;              /**< keep to zero */
  int              width;              /**< */
  int              height;             /**< */
  float          * data;               /*!< should be sRGB matched */
  char           * file_list;          /*!< colon ':' delimited list of icon file names, SVG, PNG */
};

typedef struct oyCMMapi_s oyCMMapi_s;


/** @brief   the CMM API resources struct to implement and set by a CMM
 *  @ingroup cmm_handling
 *  @extends oyStruct_s
 *
 *  Given an example CMM with name "little cms", which wants to use the 
 *  four-char ID 'lcms', the CMM can register itself to Oyranos as follows:
 *  The CMM module file must be named
 *  something_lcms_cmm_module_something.something .
 *  On Linux this could be "liboyranos_lcms_cmm_module.so.0.1.8".
 *  The four-chars 'lcms' must be prepended with OY_MODULE_NAME alias
 *  "_cmm_module".
 *
 *  Oyranos will scan the $(libdir)/color/cmms/ path, opens the available 
 *  CMM's from this directory and extracts the four-chars before OY_MODULE_NAME
 *  from the library file names. Module paths can be added through the
 *  OY_MODULE_PATHS environment variable.
 *  Oyranos looks for a symbol to a oyCMMInfo_s struct of the four-byte ID plus
 *  OY_MODULE_NAME which results in our example in the name "lcms_cmm_module".
 *  On Posix system this should be loadable by dlsym.
 *  The lcms_cmm_module should be of type oyCMMInfo_s with the type field
 *  and all other fields set appropriately.
 *
 *  The api field is a placeholder to get a real api struct assigned. If the CMM
 *  wants to provide more than one API, they can be chained.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/05 (Oyranos: 0.1.8)
 *  @date    2008/12/23
 */
struct oyCMMInfo_s {
  oyOBJECT_e       type;               /*!< struct type oyOBJECT_CMM_INFO_S */
  oyStruct_Copy_f      copy_;          /**< copy function; zero for static data */
  oyStruct_Release_f   release_;       /**< release function; zero for static data */
  oyObject_s       oy_;                /**< @private zero for static data */
  char             cmm[8];             /*!< ICC signature, eg 'lcms' */
  char           * backend_version;    /*!< non translatable, eg "v1.17" */
  /** translated, e.g. "name": "lcms" "little cms" "A CMM with 100k ..."
   *  supported should be "name", "copyright" and "manufacturer".
   *  Optional is "help".
   */
  oyCMMGetText_f   getText;
  char          ** texts;              /**< zero terminated list for getText */

  int              oy_compatibility;   /*!< last supported Oyranos CMM API : OYRANOS_VERSION */

  oyCMMapi_s     * api;                /**< must be casted to a according API, zero terminated list */

  oyIcon_s         icon;               /*!< zero terminated list of a icon pyramid */
};

OYAPI oyCMMInfo_s * OYEXPORT
                 oyCMMInfo_New       ( oyObject_s          object );
OYAPI oyCMMInfo_s * OYEXPORT
                 oyCMMInfo_Copy      ( oyCMMInfo_s       * obj,
                                       oyObject_s          object);
OYAPI int  OYEXPORT
                 oyCMMInfo_Release   ( oyCMMInfo_s      ** obj );



/** @struct  oyCMMapiFilters_s
 *  @brief   a CMMapiFilters list
 *  @extends oyStruct_s
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/30 (Oyranos: 0.1.10)
 *  @date    2009/01/30
 */
struct oyCMMapiFilters_s {
  oyOBJECT_e           type_;          /**< struct type oyOBJECT_CMM_API_FILTERS_S */ 
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  oyStructList_s     * list_;          /**< the list data */
};

OYAPI oyCMMapiFilters_s * OYEXPORT
                 oyCMMapiFilters_New ( oyObject_s          object );
OYAPI oyCMMapiFilters_s * OYEXPORT
                 oyCMMapiFilters_Copy( oyCMMapiFilters_s * list,
                                       oyObject_s          object);
OYAPI int  OYEXPORT
                 oyCMMapiFilters_Release (
                                       oyCMMapiFilters_s** list );


OYAPI int  OYEXPORT
                 oyCMMapiFilters_MoveIn (
                                       oyCMMapiFilters_s * list,
                                       oyCMMapiFilter_s ** ptr,
                                       int                 pos );
OYAPI int  OYEXPORT
                 oyCMMapiFilters_ReleaseAt (
                                       oyCMMapiFilters_s * list,
                                       int                 pos );
OYAPI oyCMMapiFilter_s * OYEXPORT
                 oyCMMapiFilters_Get ( oyCMMapiFilters_s * list,
                                       int                 pos );
OYAPI int  OYEXPORT
                 oyCMMapiFilters_Count(oyCMMapiFilters_s * list );

/** @struct  oyCMMapis_s
 *  @brief   a CMMapis list
 *  @extends oyStruct_s
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/06/25 (Oyranos: 0.1.10)
 *  @date    2010/06/25
 */
struct oyCMMapis_s {
  oyOBJECT_e           type_;          /**< struct type oyOBJECT_CMM_APIS_S */ 
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  oyStructList_s     * list_;          /**< the list data */
};

OYAPI oyCMMapis_s * OYEXPORT
           oyCMMapis_New             ( oyObject_s          object );
OYAPI oyCMMapis_s * OYEXPORT
           oyCMMapis_Copy            ( oyCMMapis_s       * list,
                                       oyObject_s          object);
OYAPI int  OYEXPORT
           oyCMMapis_Release         ( oyCMMapis_s      ** list );


OYAPI int  OYEXPORT
           oyCMMapis_MoveIn          ( oyCMMapis_s       * list,
                                       oyCMMapi_s       ** ptr,
                                       int                 pos );
OYAPI int  OYEXPORT
           oyCMMapis_ReleaseAt       ( oyCMMapis_s       * list,
                                       int                 pos );
OYAPI oyCMMapi_s * OYEXPORT
           oyCMMapis_Get             ( oyCMMapis_s       * list,
                                       int                 pos );
OYAPI int  OYEXPORT
           oyCMMapis_Count           ( oyCMMapis_s       * list );



/** obtain 4 char CMM identifiers and count of CMM's */
char **        oyModulsGetNames      ( int               * count,
                                       oyAlloc_f           allocateFunc );
/** Query for available options for a cmm

    @param[in]     cmm                 the 4 char CMM ID or zero for the current CMM
    @param[in]     object              the optional base
    @return                            available options
 */
const char *   oyModulGetOptions     ( const char        * cmm,
                                       oyObject_s          object);
const char *   oyModuleGetActual     ( const char        * type );


/* --- Image Colour Profile API --- */
/* needs extra libraries liboyranos_png liboyranos_tiff ... */
#ifdef OY_HAVE_PNG_
#include <png.h>
#define OY_PNG_s    png_infop
oyProfile_s *  oyImagePNGgetICC      ( OY_PNG_s            info,
                                       int                 flags );
#endif
#ifdef OY_HAVE_TIFF_
#include <tiffio.h>
#define OY_TIFF_s   TIFF*
oyProfile_s *  oyImageTIFFgetICC     ( OY_TIFF_s           dir,
                                       int                 flags );
int            oyImageTIFFsetICC     ( OY_TIFF_s           dir,
                                      oyProfile_s        * profile,
                                      int                  flags);
#endif
#ifdef OY_HAVE_EXR_
#include <OpenEXR/OpenEXR.h>
#define OY_EXR_s    ImfHeader
oyProfile_s *  oyImageEXRgetICC      ( OY_EXR_s            header,
                                       int                 flags );
#ifdef __cplusplus
#define OY_EXRpp_s  Imf::Header*
oyProfile_s *  oyImageEXRgetICC      ( OY_EXRpp_s          header,
                                       int                 flags );
#endif
#endif
/*
with flags something like:
oyIMAGE_EMBED_ICC_MINIMAL  ...
oyIMAGE_EMBED_ICC_FULL (while for OpenEXR this would not make sense) */


char   *       oyDumpColourToCGATS   ( const double      * channels,
                                       size_t              n,
                                       oyProfile_s       * prof,
                                       oyAlloc_f           allocateFunc,
                                       const char        * DESCRIPTOR );


/**
 *  @brief   handle parser output and build the UI
 *
 *  @param[in]     cur                 libxml2 node
 *  @param[in]     collected_elements  from libxml2
 *  @param[in]     handler_context     the toolkit context
 *  @return                            ns + ':' + name
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/08/30 (Oyranos: 0.1.10)
 *  @date    2009/08/30
 */
typedef int  (*oyUiHandler_f)        ( oyPointer           cur,
                                       oyOptions_s       * collected_elements,
                                       oyPointer           handler_context );

/** @struct  oyUiHandler_s
 *  @brief   provide a list of handlers to build the UI
 *
 *  A parser will read out the XFORMS elements and collect those a UI handler
 *  claims interesst in. The handler is then called to process the collected 
 *  elements and to build the UI.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/08/30 (Oyranos: 0.1.10)
 *  @date    2009/11/10
 */
struct oyUiHandler_s {
  oyOBJECT_e           type;           /**< oyOBJECT_UI_HANDLER_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< @private features name and hash */

  char               * dialect;        /**< currently only "oyFORMS",
                                            a subset of W3C XFORMS */
  char               * parser_type;    /**< currently only "libxml2" */
  oyUiHandler_f        handler;        /**< The handler which obtains the parsed
                                            results and a context to construct
                                            the UI. */
  char               * handler_type;   /**< informational handler context type*/
  /** The elements to collect by the parser. Levels are separated by slash '/'.
   *  Alternatives are separated by a point '.' . The list is zero terminated.
   *  e.g. "xf:select1/xf:choices/xf:item/xf:label.xf:value" means, get all 
   *  "xf:select1/xf:choices/xf:item/" paths and
   *  pick there the xf:label and xf:value elements.
   */
  char              ** element_searches;
};

char *       oyXFORMsFromModelAndUi  ( const char        * data,
                                       const char        * ui_text,
                                       const char       ** namespaces,
                                       const char        * head_injection,
                                       oyAlloc_f           allocate_func );
int          oyXFORMsRenderUi        ( const char        * xforms,
                                       oyUiHandler_s    ** ui_handlers,
                                       oyPointer           user_data );


/** in close relation to 
 *  http://www.freedesktop.org/wiki/Specifications/icc_profiles_in_x_spec
 *  OY_ICC_V0_3_TARGET_PROFILE_IN_X_BASE
 */
#define OY_ICC_COLOUR_SERVER_TARGET_PROFILE_IN_X_BASE "_ICC_DEVICE_PROFILE"


#endif /* OY_IN_PLANING */


/* --- deprecated definitions --- */

/** @deprecated */
int            oyModulRegisterXML    ( oyGROUP_e           group,
                                       const char        * xml );
int      oyGetMonitorInfo            ( const char        * display,
                                       char             ** manufacturer,
                                       char             ** model,
                                       char             ** serial,
                                       char             ** display_geometry,
                                       char             ** system_port,
                                       oyBlob_s         ** edit,
                                       oyAlloc_f           allocate_func );

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_ALPHA_H */
