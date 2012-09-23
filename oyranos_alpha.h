/** @file oyranos_alpha.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2004-2012 (C) Kai-Uwe Behrmann
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
#include "oyranos_conversion.h"
#include "oyranos_icc.h"
#include "oyranos_image.h"
#include "oyranos_module.h"
#include "oyranos_object.h"
#include "oyBlob_s.h"
#include "oyConfigs_s.h"
#include "oyConnector_s.h"
#include "oyConversion_s.h"
#include "oyFilterNode_s.h"
#include "oyFilterPlug_s.h"
#include "oyFilterPlugs_s.h"
#include "oyFilterGraph_s.h"
#include "oyHash_s.h"
#include "oyName_s.h"
#include "oyImage_s.h"
#include "oyObject_s.h"
#include "oyObserver_s.h"
#include "oyOption_s.h"
#include "oyOptions_s.h"
#include "oyPointer_s.h"
#include "oyProfile_s.h"
#include "oyProfiles_s.h"
#include "oyStruct_s.h"
#include "oyStructList_s.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define OY_IN_PLANING
#ifdef OY_IN_PLANING



#define oyNAME_ID oyNAME_NICK



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



char   oyFilterRegistrationModify    ( const char        * registration,
                                       oyFILTER_REG_MODE_e mode,
                                       char             ** result,
                                       oyAlloc_f           allocateFunc );
int          oyRegistrationEraseFromDB(const char        * registration );



OYAPI int  OYEXPORT
               oyConfig_Set          ( oyConfig_s        * config,
                                       const char        * registration_domain,
                                       oyOptions_s       * options,
                                       oyBlob_s          * data );





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

int      oySizeofDatatype            ( oyDATATYPE_e        t );






typedef struct oyNamedColour_s oyNamedColour_s;
typedef struct oyNamedColours_s oyNamedColours_s;
typedef struct oyUiHandler_s oyUiHandler_s;




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



#define OY_FILTEREDGE_FREE             0x01        /**< list free edges */
#define OY_FILTEREDGE_CONNECTED        0x02        /**< list connected edges */
#define OY_FILTEREDGE_LASTTYPE         0x04        /**< list last type edges */
/* decode */
#define oyToFilterEdge_Free_m(r)       ((r)&1)
#define oyToFilterEdge_Connected_m(r)  (((r) >> 1)&1)
#define oyToFilterEdge_LastType_m(r)   (((r) >> 2)&1)
void oyShowGraph_( oyFilterNode_s * c, const char * selector );




void               oyShowConversion_ ( oyConversion_s    * conversion,
                                       uint32_t            flags );

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
#if 1
#define oyImage_PixelLayoutGet         oyImage_GetPixelLayout
#define oyImage_ProfileGet             oyImage_GetProfile
#define oyImage_TagsGet                oyImage_GetTags
#define oyFilterNode_OptionsGet        oyFilterNode_GetOptions
#define oyFilterNode_UiGet             oyFilterNode_GetUi
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_ALPHA_H */
