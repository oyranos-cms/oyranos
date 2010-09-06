/** @file oyranos_cmm.h
 *
 *  Oyranos is an open source Colour Management System 
 * 
 *  @par Copyright:
 *            2007-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief external CMM module API
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2007/11/12
 */


#ifndef OYRANOS_CMM_H
#define OYRANOS_CMM_H

#include "oyranos.h"
#include "oyranos_alpha.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @brief   CMM capabilities query enum
 *  @ingroup module_api
 *
 *  @since: 0.1.8
 */
typedef enum {
  oyQUERY_OYRANOS_COMPATIBILITY,       /*!< provides the Oyranos version and expects the CMM compiled or compatibility Oyranos version back */
  oyQUERY_PROFILE_FORMAT = 20,         /*!< value 1 == ICC */
  oyQUERY_PROFILE_TAG_TYPE_READ,       /**< value a icTagTypeSignature (ICC) */
  oyQUERY_PROFILE_TAG_TYPE_WRITE,      /**< value a icTagTypeSignature (ICC) */
  oyQUERY_MAX
} oyCMMQUERY_e;

/**
 *  typedef oyCMMCanHandle_f
 *  @brief   CMM feature declaration function
 *  @ingroup module_api
 *  @memberof oyCMMapi_s
 */
typedef int      (*oyCMMCanHandle_f) ( oyCMMQUERY_e        type,
                                       uint32_t            value );

/**
 *  typedef oyCMMInit_f
 *  @brief   optional CMM init function
 *  @ingroup module_api
 *  @memberof oyCMMapi_s
 */
typedef int      (*oyCMMInit_f)      ( oyStruct_s        * filter );

/**
 *  typedef oyCMMMessageFuncSet_f
 *  @brief   optionaly sets a CMM message function
 *  @ingroup module_api
 *  @memberof oyCMMapi_s
 */
typedef int      (*oyCMMMessageFuncSet_f)( oyMessage_f     message_func );

#define oyCMM_PROFILE "oyPR"
#define oyCMM_COLOUR_CONVERSION "oyCC"
#define oyCOLOUR_ICC_DEVICE_LINK "oyDL"

/** @brief   CMM pointer
 *  @ingroup module_api
 *  @extends oyStruct_s
 *
 *  The oyCMMptr_s is used internally and for CMM's.
 *  Memory management is done by Oyranos' oyAllocateFunc_ and oyDeallocateFunc_.
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 *  @date    2008/07/02
 */
struct oyCMMptr_s {
  oyOBJECT_e           type;           /*!< internal struct type oyOBJECT_CMM_POINTER_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyPointer        dummy;              /**< keep to zero */
  char               * lib_name;       /*!< the CMM */
  char                 func_name[32];  /*!< optional the CMM's function name */
  oyPointer            ptr;            /*!< a CMM's data pointer */
  int                  size;           /**< -1 && 0 - unknown, > 1 size of ptr*/
  char                 resource[5];    /**< the resource type, e.g. oyCMM_PROFILE, oyCMM_COLOUR_CONVERSION */
  oyPointer_release_f  ptrRelease;     /*!< CMM's deallocation function */
  int                  ref;            /**< Oyranos reference counter */
};

oyCMMptr_s * oyCMMptr_New            ( oyAlloc_f           allocateFunc );
oyCMMptr_s * oyCMMptr_Copy           ( oyCMMptr_s        * cmm_ptr,
                                       oyAlloc_f           allocateFunc );
int          oyCMMptr_Release        ( oyCMMptr_s       ** obj );

oyCMMptr_s * oyCMMptrLookUpFromText  ( const char        * text,
                                       const char        * data_type );
oyCMMptr_s * oyCMMptrLookUpFromObject( oyStruct_s        * data,
                                       const char        * cmm );
int          oyCMMptr_Set            ( oyCMMptr_s        * cmm_ptr,
                                       const char        * lib_name,
                                       const char        * resource,
                                       oyPointer           ptr,
                                       const char        * func_name,
                                       oyPointer_release_f ptrRelease );


/** @brief   CMM data to Oyranos cache
 *
 *  @param[in]     data                the data struct know to the module
 *  @param[in,out] oy                  the Oyranos cache struct to fill by the module
 *  @return                            error
 *
 *  @version Oyranos: 0.1.9
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 *  @date    2008/11/06
 */
typedef int      (*oyCMMobjectOpen_f)  ( oyStruct_s        * data,
                                       oyCMMptr_s        * oy );

typedef void     (*oyCMMProgress_f)  ( int                 ID,
                                       double              progress );


typedef icSignature (*oyCMMProfile_GetSignature_f) (
                                       oyCMMptr_s        * cmm_ptr,
                                       int                 pcs);


/*oyPointer          oyCMMallocateFunc ( size_t              size );
void               oyCMMdeallocateFunc(oyPointer           mem );*/


/**  @ingroup module_api */
typedef enum {
  oyWIDGET_OK,
  oyWIDGET_CORRUPTED,
  oyWIDGET_REDRAW,
  oyWIDGET_HIDE,
  oyWIDGET_SHOW,
  oyWIDGET_ACTIVATE,
  oyWIDGET_DEACTIVATE,
  oyWIDGET_UNDEFINED
} oyWIDGET_EVENT_e;

/** typedef  oyCMMFilter_ValidateOptions_f
 *  @brief    a function to check and validate options
 *  @ingroup  module_api
 *  @memberof oyCMMapi4_s
 *  @memberof oyCMMapi5_s
 *
 *  @param[in]     filter              the filter
 *  @param[in]     validate            to validate
 *  @param[in]     statical            convert to a statical version
 *  @param[out]    ret                 0 if nothing changed otherwise >=1
 *  @return                            corrected options or zero
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/01/02 (Oyranos: 0.1.8)
 *  @date    2008/11/02
 */
typedef oyOptions_s * (*oyCMMFilter_ValidateOptions_f)
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate,
                                       int                 statical,
                                       uint32_t          * result );
typedef const char* (*oyWidgetGet_f) ( uint32_t          * result );
/**
 *  @typedef  oyWidgetEvent_f
 *  @ingroup  module_api
 */
typedef oyWIDGET_EVENT_e   (*oyWidgetEvent_f)
                                     ( oyOptions_s       * options,
                                       oyWIDGET_EVENT_e    type,
                                       oyStruct_s        * event );
/** typedef   oyCMMOptions_Check_f
 *  @brief    a function to check options
 *  @ingroup  module_api
 *  @memberof oyCMMapi_s
 *
 *  @param[in]     options             the options
 *  @return                            0 - good, 1 - bad
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/15 (Oyranos: 0.1.10)
 *  @date    2009/07/15
 */
typedef int  (*oyCMMOptions_Check_f) ( oyOptions_s       * validate );


/** @struct  oyCMMapi_s
 *  @brief   the basic API interface
 *  @ingroup module_api
 *  @extends oyStruct_s
 *
 *  The registration should provide keywords for selection.
 *  The api5_ member is missed for oyCMMapi5_s.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/16 (Oyranos: 0.1.10)
 *  @date    2009/01/16
 */
struct oyCMMapi_s {
  oyOBJECT_e       type;               /**< struct type oyOBJECT_CMM_API7_S */
  oyPointer        dummya;             /**< keep to zero */
  oyPointer        dummyb;             /**< keep to zero */
  oyPointer        dummyc;             /**< keep to zero */
  oyCMMapi_s     * next;               /**< the next CMM api */

  oyCMMInit_f      oyCMMInit;          /**< */
  oyCMMMessageFuncSet_f oyCMMMessageFuncSet; /**< */

  /** e.g. "sw/oyranos.org/colour.tonemap.imaging/hydra.shiva.CPU.GPU" or "sw/oyranos.org/colour/icc.lcms.CPU",
      see as well @ref registration */
  const char     * registration;

  /** 0: major - should be stable for the live time of a filter, \n
      1: minor - mark new features, \n
      2: patch version - correct errors */
  int32_t          version[3];

  /** 0: last major Oyranos version during development time, e.g. 0
   *  1: last minor Oyranos version during development time, e.g. 0
   *  2: last Oyranos patch version during development time, e.g. 10
   */
  int32_t          module_api[3];

  char           * id_;                /**< @private Oyranos id; keep to zero */
};



/**
 *  typedef oyCMMProfileTag_GetValues_f
 *  @memberof oyCMMapi3_s
 */
typedef oyStructList_s *    (*oyCMMProfileTag_GetValues_f) (
                                       oyProfileTag_s    * tag );
/**
 *  typedef oyCMMProfileTag_Create_f
 *  @memberof oyCMMapi3_s
 */
typedef int                 (*oyCMMProfileTag_Create_f) ( 
                                       oyProfileTag_s    * tag,
                                       oyStructList_s    * list,
                                       icTagTypeSignature  tag_type,
                                       uint32_t            version );

/** @struct  oyCMMapi3_s
 *  @brief   the API 3 to implement and set to provide low level ICC profile
 *           support
 *  @ingroup module_api
 *  @extends oyCMMapi_s
 *
 *  This API provides weak interface compile time checking.
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/01/02 (Oyranos: 0.1.8)
 *  @date    2008/01/02
 */
typedef struct {
  oyOBJECT_e       type;               /**< struct type oyOBJECT_CMM_API3_S */
  oyPointer        dummya;             /**< keep to zero */
  oyPointer        dummyb;             /**< keep to zero */
  oyPointer        dummyc;             /**< keep to zero */
  oyCMMapi_s     * next;

  oyCMMInit_f      oyCMMInit;          /**< */
  oyCMMMessageFuncSet_f oyCMMMessageFuncSet; /**< */

  /** e.g. "sw/oyranos.org/colour.tonemap.imaging/hydra.shiva.CPU.GPU" or "sw/oyranos.org/colour/icc.lcms.CPU",
      see as well @ref registration */
  const char     * registration;

  /** 0: major - should be stable for the live time of a filter, \n
      1: minor - mark new features, \n
      2: patch version - correct errors */
  int32_t          version[3];

  /** 0: last major Oyranos version during development time, e.g. 0
   *  1: last minor Oyranos version during development time, e.g. 0
   *  2: last Oyranos patch version during development time, e.g. 10
   */
  int32_t          module_api[3];

  char           * id_;                /**< @private Oyranos id; keep to zero */

  oyCMMCanHandle_f oyCMMCanHandle;     /**< */

  oyCMMProfileTag_GetValues_f oyCMMProfileTag_GetValues; /**< @memberof oyCMMapi3_s */
  oyCMMProfileTag_Create_f oyCMMProfileTag_Create; /**< @memberof oyCMMapi3_s */
} oyCMMapi3_s;


/* -------------------------------------------------------------------------*/
typedef struct oyCMMapi5_s oyCMMapi5_s;



/** @struct  oyCMMapiFilter_s
 *  @brief   the module API 4,6,7 interface base
 *  @ingroup module_api
 *  @extends oyCMMapi_s
 *
 *  The registration should provide keywords for selection.
 *  The api5_ member is missed for oyCMMapi5_s.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2008/12/28
 */
struct oyCMMapiFilter_s {
  oyOBJECT_e       type;               /**< struct type oyOBJECT_CMM_API[4,6,7,8]_S */
  oyStruct_Copy_f  copy;               /**< copy function */
  oyStruct_Release_f release;          /**< release function */
  oyObject_s       oy_;                /**< @private base object */
  oyCMMapi_s     * next;               /**< the next CMM api */

  oyCMMInit_f      oyCMMInit;          /**< */
  oyCMMMessageFuncSet_f oyCMMMessageFuncSet; /**< */

  /** e.g. "sw/oyranos.org/colour.tonemap.imaging/hydra.shiva.CPU.GPU" or "sw/oyranos.org/colour/icc.lcms.CPU",
      see as well @ref registration */
  const char     * registration;

  /** 0: major - should be stable for the live time of a filter, \n
      1: minor - mark new features, \n
      2: patch version - correct errors */
  int32_t          version[3];

  /** 0: last major Oyranos version during development time, e.g. 0
   *  1: last minor Oyranos version during development time, e.g. 0
   *  2: last Oyranos patch version during development time, e.g. 10
   */
  int32_t          module_api[3];

  char           * id_;                /**< @private Oyranos id; keep to zero */
  oyCMMapi5_s    * api5_;            /**< @private meta module; keep to zero */
};


/** typedef  oyCMMuiGet_f
 *  @brief   obtain a XFORMS ui description
 *  @ingroup module_api
 *
 *  The structures can provide a XFORMS ui based on the modules own
 *  set of options. The options are in the property of the caller.
 *
 *  @param[in]     options             the options to display
 *  @param[out]    ui_text             the XFORMS string
 *  @param[in]     allocateFunc        user allocator
 *  @return                            0 on success; error >= 1; -1 not understood; unknown < -1
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/18 (Oyranos: 0.1.10)
 *  @date    2009/01/18
 */
typedef int  (*oyCMMuiGet_f)         ( oyOptions_s        * options,
                                       char              ** ui_text,
                                       oyAlloc_f            allocateFunc );


/** typedef oyCMMobjectLoadFromMem_f
 *  @brief   load a filter object from a in memory data blob
 *  @ingroup module_api
 *  @memberof oyCMMobjectType_s
 *
 *  @param[in]     buf_size            data size
 *  @param[in]     buf                 data blob
 *  @param[in]     flags               for future use
 *  @param[in]     object              the optional base
 *  @return                            the object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/11/22 (Oyranos: 0.1.9)
 *  @date    2009/09/14
 */
typedef oyStruct_s * (*oyCMMobjectLoadFromMem_f) (
                                       size_t              buf_size,
                                       const oyPointer     buf,
                                       uint32_t            flags,
                                       oyObject_s          object);

/** typedef oyCMMobjectGetText_f
 *  @brief   build a text string from a given object
 *  @ingroup module_api
 *  @memberof oyCMMobjectType_s
 *
 *  Serialise a object into:
 *  - oyNAME_NICK: XML ID
 *  - oyNAME_NAME: XML
 *  - oyNAME_DESCRIPTION: ??
 *
 *  For type information the object argument is omitted. Then the function shall
 *  programatically tell in oyNAME_NICK about the object type,
 *  e.g. "oyProfile_s",
 *  in oyNAME_NAME translated about its intented usage,
 *  e.g. i18n("ICC profile") and give with oyNAME_DESCRIPTION some further long 
 *  informations.
 *
 *  Note: Dynamically generated informations can be stored in the 
 *  oyStruct_s::oy::name_  member and then returned by the function. Oyranos 
 *  will typical look first at that oyObject_s member and then ask this 
 *  function to get the information. @see oyObject_SetName()
 *
 *  @param[in]     object              the object, omit to get a general text
 *  @param[out]    type                the string type
 *  @param[in]     flags               for future use
 *  @return                            0 on success; error >= 1; unknown < 0
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/24 (Oyranos: 0.1.10)
 *  @date    2009/09/14
 */
typedef const char *   (*oyCMMobjectGetText_f) (
                                       oyStruct_s        * object,
                                       oyNAME_e            type,
                                       int                 flags );

/** typedef oyCMMobjectScan_f
 *  @brief   load a filter object from a in memory data blob
 *  @ingroup module_api
 *  @memberof oyCMMobjectType_s
 *
 *  @param[in]     data                data blob
 *  @param[in]     size                data size
 *  @param[out]    registration        filter registration string
 *  @param[out]    name                filter name
 *  @param[in]     allocateFunc        e.g. malloc
 *  @return                            0 on success; error >= 1; unknown < 0
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/11/22 (Oyranos: 0.1.9)
 *  @date    2009/09/14
 */
typedef int          (*oyCMMobjectScan_f) (
                                       oyPointer           data,
                                       size_t              size,
                                       char             ** registration,
                                       char             ** name,
                                       oyAlloc_f           allocateFunc );

/** @struct  oyCMMobjectType_s
 *  @brief   custom object handler
 *  @ingroup module_api
 *  @extends oyStruct_s
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/11/23 (Oyranos: 0.1.9)
 *  @date    2009/09/14
 */
typedef struct {
  oyOBJECT_e       type;               /**< struct type oyOBJECT_CMM_DATA_TYPES_S */
  oyPointer        dummya;             /**< keep to zero */
  oyPointer        dummyb;             /**< keep to zero */
  oyPointer        dummyc;             /**< keep to zero */

  /** object type id,
   *  set to a object type known to Oyranos, or
   *  set as a unique four byte signature, like 'myID' just more unique
   *  to avoid collisions. The id shall match a the oyStruct_s::type_ member
   *  generated through oyCMMobjectLoadFromMem_f. */
  oyOBJECT_e       id;
  /** a colon separated list of sub paths to expect the data in,
      e.g. "color/icc" */
  const char     * paths;
  const char     * (*pathsGet)();      /**< e.g. non XDG colon separated paths*/
  const char     * exts;               /**< file extensions, e.g. "icc:icm" */
  const char     * element_name;       /**< XML element name, e.g. "profile" */
  oyCMMobjectGetText_f             oyCMMobjectGetText; /**< */
  oyCMMobjectLoadFromMem_f         oyCMMobjectLoadFromMem; /**< */
  oyCMMobjectScan_f                oyCMMobjectScan; /**< */
} oyCMMobjectType_s;


/** typedef oyCMMFilterLoad_f
 *  @brief   load a filter from a in memory data blob
 *  @ingroup module_api
 *  @memberof oyCMMapi5_s
 *
 *  @param[in]     data                data blob
 *  @param[in]     size                data size
 *  @return                            filter
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/11/22 (Oyranos: 0.1.9)
 *  @date    2008/12/28
 */
typedef oyCMMapiFilter_s * (*oyCMMFilterLoad_f) (
                                       oyPointer           data,
                                       size_t              size,
                                       const char        * file_name,
                                       oyOBJECT_e          type,
                                       int                 num );

/** typedef oyCMMFilterScan_f
 *  @brief   load a filter from a in memory data blob
 *  @ingroup module_api
 *  @memberof oyCMMapi5_s
 *
 *  @param[in]     data                filter data blob
 *  @param[in]     size                data size
 *  @param[in]     file_name           the filter file for information or zero
 *  @param[in]     type                filter type
 *  @param[in]     num                 number of filter
 *  @param[out]    registration        filter registration string
 *  @param[out]    name                filter name
 *  @param[in]     allocateFunc        e.g. malloc
 *  @param[out]    info                oyCMMInfo_s pointer to set
 *  @param[in]     object              e.g. Oyranos object
 *  @return                            0 on success; error >= 1; -1 not found; unknown < -1
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/22 (Oyranos: 0.1.9)
 *  @date    2008/12/17
 */
typedef int          (*oyCMMFilterScan_f) (
                                       oyPointer           data,
                                       size_t              size,
                                       const char        * file_name,
                                       oyOBJECT_e          type,
                                       int                 num,
                                       char             ** registration,
                                       char             ** name,
                                       oyAlloc_f           allocateFunc,
                                       oyCMMInfo_s      ** info,
                                       oyObject_s          object );


/** @struct  oyConnectorImaging_s
 *  @brief   node connection descriptor
 *  @ingroup objects_conversion
 *  @extends oyConnector_s
 *
 *  This structure holds informations about the connection capabilities.
 *  It holds common structure members of oyFilterPlug_s and oyFilterSocket_s.
 *
 *  To signal a value is not initialised or does not apply, set the according
 *  integer value to -1.
 *
 *  @todo generalise the connector properties
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/07/26 (Oyranos: 0.1.8)
 *  @date    2010/06/25
 */
struct oyConnectorImaging_s {
  oyOBJECT_e           type_;          /**< @private struct type oyOBJECT_CONNECTOR_IMAGING_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< @private base object */

  /** unique strings, nick and name will be used as the connector's type ID,
   *  e.g."Img", "Image", "Image Socket"*/
  oyName_s             name;

  /**
   *  The following types should have a "data" attribute in the application
   *  level as these connectors pass the imaging data.
   *  - a data manipulator. e.g. a normal filter - "//imaging/manipulator.data"
   *  - a data generator, e.g. checkerboard, gradient "//imaging/generator.data"
   *  - a pixel data provider, e.g. oyFILTER_TYPE_IMAGE "//imaging/image.data"
   *  - observer, a endpoint, only input, e.g. text log, thumbnail viewer 
   *    "//imaging/observer.data"
   *  - a routing element, without data altering "//imaging/splitter.rectangle.data"
   *  - combines or splits image data, e.g. blending "//imaging/blender.rectangle.data"
   *  - converts pixel layout to other formats "//imaging/pixel.convertor.data"
   *  - converts pixel layout to other formats, with precission loss, e.g. 
   *    float -> uint8_t, only relevant for output connectors 
   *    "//imaging/pixel.convertor.lossy.data"
   *  - combines gray channels, e.g. from colour "//imaging/combiner.channels.data"
   *  - provides gray scale views of channels "//imaging/splitter.channels.data"
   *
   *  The following types should have a "text" attribute in the application
   *  level as this connector type passes text.
   *  - provides values or text, only output "///analysis.text"
   */
  char               * connector_type;
  /** Check if two oyCMMapi7_s filter connectors of type oyConnector_s can 
   *  match each other inside a given socket and a plug.
   *  For a imaging plugin just add here oyFilterSocket_MatchImagingPlug */
  oyCMMFilterSocket_MatchPlug_f  filterSocket_MatchPlug;

  /** 1 - make requests and receive data, by part of oyFilterPlug_s;
    * 0 - receive requests and provide data, oyFilterSocket_s
    */
  int                  is_plug;
  oyDATATYPE_e       * data_types;     /**< supported float's and int's */
  int                  data_types_n;   /**< # elements in data_types array */
  int                  max_colour_offset;
  int                  min_channels_count;
  int                  max_channels_count;
  int                  min_colour_count;
  int                  max_colour_count;
  int                  can_planar;     /**< can read separated channels */
  int                  can_interwoven; /**< can read continuous channels */
  int                  can_swap;       /**< can swap colour channels (BGR)*/
  int                  can_swap_bytes; /**< non host byte order */
  int                  can_revert;     /**< revert 1 -> 0 and 0 -> 1 */
  int                  can_premultiplied_alpha;
  int                  can_nonpremultiplied_alpha;
  int                  can_subpixel;   /**< understand subpixel order */
  /** describe which channel types the connector requires */
  oyCHANNELTYPE_e    * channel_types;
  int                  channel_types_n;/**< count in channel_types */
  int                  id;             /**< relative to oyFilterCore_s, e.g. 1*/
  /**< connector is mandatory or optional, important for modules */
  int                  is_mandatory;
};


OYAPI oyConnectorImaging_s * OYEXPORT
                 oyConnectorImaging_New( oyObject_s          object );
OYAPI oyConnectorImaging_s * OYEXPORT
                 oyConnectorImaging_Copy(oyConnectorImaging_s* list,
                                       oyObject_s          object);
OYAPI int  OYEXPORT
                 oyConnectorImaging_Release ( 
                                       oyConnectorImaging_s**list );

int          oyFilterSocket_MatchImagingPlug (
                                       oyFilterSocket_s  * socket,
                                       oyFilterPlug_s    * plug );


/** @struct  oyCMMapi5_s
 *  @brief   module or script loader
 *  @ingroup module_api
 *  @extends oyCMMapi_s
 *
 *  Filters can be provided in non library form, e.g. as text files. This API 
 *  allowes for registring of paths and file types to be recognised as filters.
 *  The API must provide the means to search, list, verify and open these 
 *  script filters through Oyranos. The filters are opened in Oyranos and passed
 *  as blobs to the API function for obtaining light wight informations, e.g.
 *  list the scanned filters in a user selection widget. Further the API is
 *  responsible to open the filter and create a oyFilter_s object.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/11/22 (Oyranos: 0.1.9)
 *  @date    2010/06/25
 */
struct oyCMMapi5_s {
  oyOBJECT_e       type;               /**< struct type oyOBJECT_CMM_API5_S */
  oyPointer        dummya;             /**< keep to zero */
  oyPointer        dummyb;             /**< keep to zero */
  oyPointer        dummyc;             /**< keep to zero */
  oyCMMapi_s     * next;

  oyCMMInit_f      oyCMMInit;          /**< */
  oyCMMMessageFuncSet_f oyCMMMessageFuncSet;  /**< */

  /** e.g. "sw/oyranos.org/colour.tonemap.imaging/hydra.shiva" or "sw/oyranos.org/colour/icc",
      see as well @ref registration */
  const char     * registration;

  /** 0: major - should be stable for the live time of a filter, \n
      1: minor - mark new features, \n
      2: patch version - correct errors */
  int32_t          version[3];

  /** 0: last major Oyranos version during development time, e.g. 0
   *  1: last minor Oyranos version during development time, e.g. 0
   *  2: last Oyranos patch version during development time, e.g. 10
   */
  int32_t          module_api[3];

  char           * id_;                /**< @private Oyranos id; keep to zero */

  /** a colon separated list of sub paths to expect the scripts in,
      e.g. "color/shiva:color/octl" */
  const char     * sub_paths;
  /** optional filename extensions, e.g. "shi:ctl" */
  const char     * ext;
  /** 0: libs - libraries, Oyranos searches in the XDG_LIBRARY_PATH and sub_paths, The library will be provided as file_name\n  1: scripts - platform independent filters, Oyranos will search in the XDG_DATA_* paths, Script are provided as i memory blobs */
  int32_t          data_type;          /**< */

  oyCMMFilterLoad_f                oyCMMFilterLoad; /**< */
  oyCMMFilterScan_f                oyCMMFilterScan; /**< */
};

/** typedef oyCMMFilterPlug_Run_f
 *  @brief   get a pixel or channel from the previous filter
 *  @ingroup module_api
 *  @memberof oyCMMapi7_s
 *
 *  You have to call oyCMMFilter_CreateContext_t or oyCMMFilter_ContextFromMem_t first.
 *  The API provides flexible pixel access and cache configuration by the
 *  passed oyPixelAccess_s object. The filters internal precalculated data
 *  are passed by the filter object.
 *
 *  @verbatim
    while (
    error = oyCMMFilterSocket_GetNext( filter_plug, pixel_access ) == 0
    ) {}; @endverbatim
 *
 *  @param[in]     plug                including the CMM's private data, connector is the requesting plug to obtain a handle for calling back
 *  @param[in]     pixel_access        processing order instructions
 *  @return                            -1 end; 0 on success; error > 1
 *
 *  A requested context will be stored in oyFilterNode_s::backend_data.
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/03 (Oyranos: 0.1.8)
 *  @date    2008/07/28
 */
typedef int (*oyCMMFilterPlug_Run_f) ( oyFilterPlug_s    * plug,
                                       oyPixelAccess_s   * pixel_access );

/** @struct  oyCMMapi7_s
 *  @brief   data processing node
 *  @ingroup module_api
 *  @extends oyCMMapiFilter_s
 *
 *  The structure forms a node element in a directed acyclic graph. It's 
 *  connectors allow to communicate cababilities and semantics. This node type
 *  acts mostly on data processing. 
 *
 *  It is possible to delegate parameter or context creation to other
 *  specialised filter types.
 *  The filter context can be stored in oyFilterNode_s::backend_data if the
 *  oyCMMapi7_s::context_type is filled with a understood format hint.
 *  The registration should provide keywords to select the processing function.
 *
 *  (in CM jargon - interpolator)
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/15 (Oyranos: 0.1.9)
 *  @date    2008/12/27
 */
struct oyCMMapi7_s {
  oyOBJECT_e       type;               /**< struct type oyOBJECT_CMM_API7_S */
  oyPointer        dummya;             /**< keep to zero */
  oyPointer        dummyb;             /**< keep to zero */
  oyPointer        dummyc;             /**< keep to zero */
  oyCMMapi_s     * next;


  oyCMMInit_f      oyCMMInit;          /**< @memberof oyCMMapi7_s */
  oyCMMMessageFuncSet_f oyCMMMessageFuncSet;  /**< @memberof oyCMMapi7_s */

  /** e.g. "sw/oyranos.org/colour.tonemap.imaging/hydra.shiva.CPU.GPU" or "sw/oyranos.org/colour/icc.lcms.CPU",
      see as well @ref registration */
  const char     * registration;

  /** 0: major - should be stable for the live time of a filter, \n
      1: minor - mark new features, \n
      2: patch version - correct errors */
  int32_t          version[3];

  /** 0: last major Oyranos version during development time, e.g. 0
   *  1: last minor Oyranos version during development time, e.g. 0
   *  2: last Oyranos patch version during development time, e.g. 10
   */
  int32_t          module_api[3];

  char           * id_;                /**< @private Oyranos id; keep to zero */
  oyCMMapi5_s    * api5_;            /**< @private meta module; keep to zero */

  /** mandatory for all filters; Special care has to taken for the
      oyPixelAccess_s argument to this function. */
  oyCMMFilterPlug_Run_f oyCMMFilterPlug_Run;
  /** e.g. oyCMMFilterPlug_Run specific context data "lcCC" */
  char             context_type[8];

  /** We have to tell about valid input and output connectors, by 
      passively providing enough informations. */
  oyConnector_s ** plugs;
  uint32_t         plugs_n;            /**< number of different plugs */ 
  /** additional allowed number for last input connector, e.g. typical 0 */
  uint32_t         plugs_last_add;
  oyConnector_s ** sockets;            /**< the socket description */
  uint32_t         sockets_n;          /**< number of sockets */
  /** additional allowed number for last output connector, e.g. typical 0 */
  uint32_t         sockets_last_add;
  /** A zero terminated list of strings. The semantics are plugin defined.
   *  e.g.: {"key1=value1","key2=value2",0} */
  char **          properties;
};

/**
 *  typedef oyCMMdata_Convert_f
 *  @brief   convert between data formats
 *  @ingroup module_api
 *  @memberof oyCMMapi6_s
 *
 *  The function might be used to provide a module specific context.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/27 (Oyranos: 0.1.10)
 *  @date    2008/12/27
 */
typedef int(*oyCMMdata_Convert_f)    ( oyCMMptr_s        * data_in,
                                       oyCMMptr_s        * data_out,
                                       oyFilterNode_s    * node );

/** @struct  oyCMMapi6_s
 *  @brief   context convertor
 *  @ingroup module_api
 *  @extends oyCMMapiFilter_s
 *
 *  The context provided by a filter can be exotic. The API provides the means
 *  to get him into a known format. With this format connector it is possible to
 *  interface otherwise divergine formats.
 *
 \dot
digraph G {
  rankdir=LR
  node [shape=record, fontname=Helvetica, fontsize=10];
  edge [fontname=Helvetica, fontsize=10];

  a [ label="ICC device link - oyDL" ];
  b [ label="littleCMS specific device link - lcCC" ];

  a -> b [arrowtail=none, arrowhead=normal];
}
 \enddot
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/15 (Oyranos: 0.1.9)
 *  @date    2008/12/27
 */
struct oyCMMapi6_s {
  oyOBJECT_e       type;               /**< struct type oyOBJECT_CMM_API6_S */
  oyPointer        dummya;             /**< keep to zero */
  oyPointer        dummyb;             /**< keep to zero */
  oyPointer        dummyc;             /**< keep to zero */
  oyCMMapi_s     * next;

  oyCMMInit_f      oyCMMInit;
  oyCMMMessageFuncSet_f oyCMMMessageFuncSet;

  /** place data_type_in + underscore '_' + data_type_out, 
   *  e.g. "sw/oyranos.org/colour/icc.lcms.oyDL_lcCC",
      see as well @ref registration */
  const char     * registration;

  /** 0: major - should be stable for the live time of a filter, \n
      1: minor - mark new features, \n
      2: patch version - correct errors */
  int32_t          version[3];

  /** 0: last major Oyranos version during development time, e.g. 0
   *  1: last minor Oyranos version during development time, e.g. 0
   *  2: last Oyranos patch version during development time, e.g. 10
   */
  int32_t          module_api[3];

  char           * id_;                /**< @private Oyranos id; keep to zero */
  oyCMMapi5_s    * api5_;              /**< @private meta module; keep to zero*/

  /** oyCMMapi4_s::context_type typic data; e.g. "oyDL" */
  char           * data_type_in;
  /** oyCMMapi7_s::context_type specific data; e.g. "lcCC" */
  char           * data_type_out;
  oyCMMdata_Convert_f oyCMMdata_Convert;
};


/** @struct  oyCMMui_s
 *  @brief   the CMM API UI part
 *  @ingroup module_api
 *  @extends oyStruct_s
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/06 (Oyranos: 0.1.10)
 *  @date    2009/12/17
 */
typedef struct {
  oyOBJECT_e       type;               /**< struct type oyOBJECT_CMM_DATA_TYPES_S */
  oyPointer        dummya;             /**< keep to zero */
  oyPointer        dummyb;             /**< keep to zero */
  oyPointer        dummyc;             /**< keep to zero */

  /** 0: major - should be stable for the live time of a filters UI, \n
      1: minor - mark new features, \n
      2: patch version - correct errors */
  int32_t          version[3];

  /** 0: last major Oyranos version during development time, e.g. 0
   *  1: last minor Oyranos version during development time, e.g. 0
   *  2: last Oyranos patch version during development time, e.g. 10
   */
  int32_t          module_api[3];

  /** check options for validy and correct */
  oyCMMFilter_ValidateOptions_f    oyCMMFilter_ValidateOptions;
  oyWidgetEvent_f                  oyWidget_Event; /**< handle widget events */

  const char     * category;           /**< menu structure */
  const char     * options;            /**< default options as Elektra/XFORMS 
                                        *   compatible XML */
  oyCMMuiGet_f     oyCMMuiGet;         /**< xml ui elements for filter options*/

  oyCMMGetText_f   getText;            /**< translated UI texts */
  /** zero terminated categories for getText,
   *  e.g. {"name","category","help",0}
   *
   *  The "name" texts shall include information about the module.
   *  The "category" gives in oyNAME_NAME a translated version of
   *  oyCMMui_s::category.
   *  The "help" texts should provide general infromations about the module.
   *
   *  The oyNAME_NICK for the several oyCMMui_s::getText() texts is typical not
   *  translated. For "name" the oyNAME_NICK should be the module name. For 
   *  other texts like "help" and "category" oyNAME_NICK makes no sense.
   */
  const char    ** texts;
} oyCMMui_s;


/** typedef oyCMMFilterNode_ContextToMem_f
 *  @brief   store a CMM filter context into a memory blob
 *  @ingroup module_api
 *  @memberof oyCMMapi4_s
 *
 *  The goal is to have a data blob for later reusing. It is as well used for
 *  exchange and analysis. A oyCMMapi4_s filter with context_type member set to
 *  something should implement this function and fill the data
 *  blob with the according context data for easy forwarding and
 *  on disk caching.
 *
 *  @param[in,out] node                access to the complete filter struct,
 *                                     most important to handle is the options
 *                                     and image members
 *  @param[out]    size                size in return 
 *  @param         allocateFunc        memory allocator for the returned data
 *  @return                            the CMM memory blob, preferedly ICC
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/02 (Oyranos: 0.1.8)
 *  @date    2008/07/02
 */
typedef oyPointer(*oyCMMFilterNode_ContextToMem_f) (
                                       oyFilterNode_s    * node,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc );

/** typedef oyCMMFilterNode_GetText_f
 *  @brief   describe a CMM filter context
 *  @ingroup module_api
 *  @memberof oyCMMapi4_s
 *
 *  For a oyNAME_NICK and oyNAME_NAME type argument, the function shall
 *  describe only those elements, which are relevant to the result of the
 *  context creation. The resulting string is CMM specific by intention.
 *
 *  Serialise into:
 *  - oyNAME_NICK: XML ID
 *  - oyNAME_NAME: XML
 *  - oyNAME_DESCRIPTION: ??
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/27 (Oyranos: 0.1.10)
 *  @date    2008/12/27
 */
typedef char *(*oyCMMFilterNode_GetText_f) (
                                       oyFilterNode_s    * node,
                                       oyNAME_e            type,
                                       oyAlloc_f           allocateFunc );

/** @struct  oyCMMapi4_s
 *  @brief   context creator
 *  @ingroup module_api
 *  @extends oyCMMapiFilter_s
 *
 *  The structure contains functions to specify and prepare parameters or
 *  a context dedicated for a filter node class. UI elements can be included for
 *  manual settings. As per definition the result of this filter is serialised,
 *  and can therefore be cached by Oyranos' core.
 *
 *  Different filters have to provide this struct each one per filter.
 *
 *  The ::oyCMMFilterNode_ContextToMem @see oyCMMFilterNode_ContextToMem_f
 *  should be implemented in case the context_type is set to a
 *  context data type.
 *
 *  @todo in commit 5a2b7396 the connection descriptors where moved to 
 *  oyCMMapi7_s. For a context, which must be created from the processing data
 *  this is not relyable, as the interpolator can not be shure that the context
 *  plug-in supports the same or larger set of connectors.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2009/09/06
 */
struct  oyCMMapi4_s {
  oyOBJECT_e       type;               /**< struct type oyOBJECT_CMM_API4_S */
  oyPointer        dummya;             /**< keep to zero */
  oyPointer        dummyb;             /**< keep to zero */
  oyPointer        dummyc;             /**< keep to zero */
  oyCMMapi_s     * next;

  oyCMMInit_f      oyCMMInit;
  oyCMMMessageFuncSet_f oyCMMMessageFuncSet;

  /** e.g. "sw/oyranos.org/imaging/scale" or "sw/oyranos.org/colour/icc.lcms",
      see as well @ref registration */
  const char     * registration;

  /** 0: major - should be stable for the live time of a filter, \n
      1: minor - mark new features, \n
      2: patch version - correct errors */
  int32_t          version[3];

  /** 0: last major Oyranos version during development time, e.g. 0
   *  1: last minor Oyranos version during development time, e.g. 0
   *  2: last Oyranos patch version during development time, e.g. 10
   */
  int32_t          module_api[3];

  char           * id_;                /**< @private Oyranos id; keep to zero */
  oyCMMapi5_s    * api5_;              /**< @private meta module; keep to zero*/

  /** mandatory for "//colour/icc" context producing filters */
  oyCMMFilterNode_ContextToMem_f   oyCMMFilterNode_ContextToMem;
  /** optional for a set oyCMMFilterNode_ContextToMem,
   *  used to override a Oyranos side hash creation */
  oyCMMFilterNode_GetText_f        oyCMMFilterNode_GetText;
  /** the data type of the context returned by oyCMMapi4_s::oyCMMFilterNode_ContextToMem_f, mandatory for a set oyCMMFilterNode_ContextToMem
   *  e.g. oyCOLOUR_ICC_DEVICE_LINK / "oyDL" */
  char             context_type[8];

  /** a UI description
   *  Obligatory is a implemented oyCMMapi4_s::ui->getText( x, y ) call. The x
   *  argument shall cover "name" and "help" */
  oyCMMui_s      * ui;                 /**< a UI description */
};


/** typedef  oyConfigs_FromPattern_f
 *  @brief   return available configurations
 *  @ingroup module_api
 *  @memberof oyCMMapi8_s
 *
 *  @param[in]     registration        a selection string marked with "///config"
 *  @param[in]     options             the complete options from the user
 *  @param[out]    configs             the returned configurations
 *  @return                            0 - good, 1 <= error, -1 >= unknown issue, look for messages
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/18 (Oyranos: 0.1.10)
 *  @date    2009/01/18
 */
typedef int      (*oyConfigs_FromPattern_f) (
                                       const char        * registration,
                                       oyOptions_s       * options,
                                       oyConfigs_s      ** configs );
/** typedef  oyConfigs_Modify_f
 *  @brief   enrich/manipulate existing configurations
 *  @ingroup module_api
 *  @memberof oyCMMapi8_s
 *
 *  @param[in,out] configs             the configurations
 *  @param[in]     options             the complete options from the user
 *  @return                            0 - good, 1 <= error, -1 >= unknown issue, look for messages
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/08/21 (Oyranos: 0.1.10)
 *  @date    2009/08/21
 */
typedef int      (*oyConfigs_Modify_f)( oyConfigs_s       * configs,
                                        oyOptions_s       * options );

/** typedef  oyConfig_Rank_f
 *  @brief   check for correctness
 *  @ingroup module_api
 *  @memberof oyCMMapi8_s
 *
 *  The function is called in Oyranos' core to give a module a hook to check  
 *  and accept a device. If the module does not find its previously set  
 *  handles and can neigther open the device from the device_name then
 *  chances are good that it can't handle and should return zero otherwise    
 *  one. Oyranos will then try an other module with this device.
 *
 *  @param   config                    the to be checked configuration
 *  @return                            - 0 - indifferent,
 *                                     - >= 1 - rank,
 *                                     - <= -1 error + a message should be sent
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/16 (Oyranos: 0.1.10)
 *  @date    2009/01/26
 */
typedef int  (*oyConfig_Rank_f)     ( oyConfig_s         * config );


/** @struct  oyCMMapi8_s
 *  @brief   configuration handler
 *  @ingroup module_api
 *  @extends oyCMMapiFilter_s
 *
 *  Oyranos knows about configurations sets as oyConfig_s structures.
 *  These configurations can be created, modified and compared by this module 
 *  type and stored by Oyranos' core.
 *  They are stored under the base key path decided by each configuration 
 *  module individualy in its oyCMMapi8_s::registration string.
 *
 *  This API provides weak interface compile time checking.
 *
 *  The module knows what to configure. This can be devices, filters or 
 *  something else. A oyCMMapi8_s module counts the understood endities
 *  and veryfies a provided oyConfig_s for correctness. Oyranos requires just
 *  the fourth type string in the registration path starting with "config".
 *  This requirement is useful to group DB keys alphabetically.
 *
 *  In the case of a device, a application can ask Oyranos for all or a subset
 *  of available devices with oyConfigs_FromPattern.
 *
 *  A application can modify a selection of oyConfig_s objects and
 *  modify them.
 *  It is adviced to let Oyranos ask the module in advance through 
 *  oyConfig_Rank_f, if the modified oyConfig_s object is still valid.
 *
 *  @Future directions:
 *  For automatic UI's this module API should provide a XFORMS UI about the
 *  result and return a oyConfig_s for a filled form.
 *
 *  A empty device module is provided with oyranos_cmm_dDev.c in Oyranos'
 *  examples/ directory.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/16 (Oyranos: 0.1.10)
 *  @date    2009/12/15
 */
struct oyCMMapi8_s {
  oyOBJECT_e           type;           /**< struct type oyOBJECT_CMM_API8_S */ 
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< @private base object */

  oyCMMapi_s     * next;

  oyCMMInit_f      oyCMMInit;          /**< */
  oyCMMMessageFuncSet_f oyCMMMessageFuncSet; /**< */

  /** The oyFILTER_REG_APPLICATION of "config" is obligatory.
   *
   *  e.g. "shared/freedesktop.org/imaging/config.device.icc_profile.scanner.sane" or 
   *       "shared/freedesktop.org/imaging/config.device.icc_profile.monitor.xorg"...\n
      see as well @ref registration
   */
  char           * registration;

  /** 0: major - should be stable for the live time of a module, \n
      1: minor - mark new features, \n
      2: patch version - correct errors */
  int32_t          version[3];

  /** 0: last major Oyranos version during development time, e.g. 0
   *  1: last minor Oyranos version during development time, e.g. 0
   *  2: last Oyranos patch version during development time, e.g. 10
   */
  int32_t          module_api[3];

  char           * id_;                /**< @private Oyranos id; keep to zero */
  oyCMMapi5_s    * api5_;            /**< @private meta module; keep to zero */

  /** obtain configs matching to a set of options */
  oyConfigs_FromPattern_f oyConfigs_FromPattern;
  /** manipulate given configs */
  oyConfigs_Modify_f oyConfigs_Modify;
  oyConfig_Rank_f  oyConfig_Rank;      /**< test config */

  oyCMMui_s      * ui;                 /**< a UI description */
  oyIcon_s       * icon;               /**< module associated icon */

  /** zero terminated list of rank attributes;
   *  The data is just informational. In case all properties to rank a given 
   *  device pair are well known, this rank_map can be copied into each 
   *  oyConfig_s device created by oyConfigs_FromPattern():
   *  @verbatim
      oyConfig_s * device = oyConfig_New( CMM_BASE_REG, 0);
      device->rank_map = oyRankMapCopy( dDev_api8->rank_map,
                                        device->oy_->allocateFunc_);
      @endverbatim
   *  For additional dynamic rank pads the allocation can be done individually.
   */
  oyRankPad      * rank_map;
};


/** typedef  oyConversion_Correct_f
 *  @brief   check for correctly adhering to policies
 *  @ingroup module_api
 *  @memberof oyCMMapi9_s
 *
 *  Without any options the module shall
 *  perform graph analysis and correct the graph.
 *
 *  @see oyConversion_Correct()
 *
 *  @par Typical Options:
 *  - "command"-"help" - a string option issuing a help text as message
 *  - "verbose" - reporting changes as message
 *
 *  @param   conversion                the to be checked configuration
 *  @param   options                   options to the policy module
 *  @return                            0 - indifferent, >= 1 - error
 *                                     + a message should be sent
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/23 (Oyranos: 0.1.10)
 *  @date    2009/07/23
 */
typedef int  (*oyConversion_Correct_f) (
                                       oyConversion_s    * conversion,
                                       oyOptions_s       * options );

/** @struct  oyCMMapi9_s
 *  @brief   graph policies
 *  @ingroup module_api
 *  @extends oyCMMapiFilter_s
 *
 *  The user visible Oyranos settings are subject to be applied to graphs.
 *  This module type provides an interface to check and verified a graph
 *  according to arbitrary policies in a automated fashion. 
 *  The usage of graph policy modules is optional and
 *  can be controled by the front end function oyConversion_Correct().
 *
 *  The policy check can cover options and data checks. Graph analysis and 
 *  correction is possible as well.
 *
 *  Differences to native node modules are that, a policy module runs before
 *  the graph starts data processing, it can provide options for a group of 
 *  node modules including a UI and provide additional object types.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/23 (Oyranos: 0.1.10)
 *  @date    2009/09/14
 */
struct oyCMMapi9_s {
  oyOBJECT_e           type;           /**< struct type oyOBJECT_CMM_API9_S */ 
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< @private base object */

  oyCMMapi_s     * next;

  oyCMMInit_f      oyCMMInit;          /**< */
  oyCMMMessageFuncSet_f oyCMMMessageFuncSet; /**< */

  /** The oyFILTER_REG_APPLICATION of "config" is obligatory.
   *  e.g. "shared/freedesktop.org/imaging/config.scanner.sane" or 
   *       "shared/freedesktop.org/imaging/config.monitor.xorg" ...\n
      see as well @ref registration
   */
  char           * registration;

  /** 0: major - should be stable for the live time of a module, \n
      1: minor - mark new features, \n
      2: patch version - correct errors */
  int32_t          version[3];

  /** 0: last major Oyranos version during development time, e.g. 0
   *  1: last minor Oyranos version during development time, e.g. 0
   *  2: last Oyranos patch version during development time, e.g. 10
   */
  int32_t          module_api[3];

  char           * id_;                /**< @private Oyranos id; keep to zero */
  oyCMMapi5_s    * api5_;            /**< @private meta module; keep to zero */

  /** check options for validy and correct */
  oyCMMFilter_ValidateOptions_f    oyCMMFilter_ValidateOptions;
  oyWidgetEvent_f              oyWidget_Event;     /**< handle widget events */

  const char     * options;            /**< default options */
  oyCMMuiGet_f     oyCMMuiGet;         /**< xml ui elements for filter options*/
  /**< XML namespace to describe the used data_types
   *   e.g. 'oy="http://www.oyranos.org/2009/oyranos"'
   */
  const char     * xml_namespace;

  oyCMMobjectType_s ** object_types;   /**< zero terminated list of types */

  oyCMMGetText_f   getText;            /**< describe selectors in UI */
  const char    ** texts;              /**< zero terminated categories for getText, e.g. {"///GPU","///CPU","//colour",0} */

  oyConversion_Correct_f oyConversion_Correct; /**< check a graph */
  /** registration pattern which are supported by oyConversion_Correct \n
   *
   *  e.g. for imaging this could be "//imaging"
   */
  const char     * pattern;
};


/** typedef  oyMOptions_Handle_f
 *  @brief   generic data exchange
 *  @ingroup module_api
 *  @memberof oyCMMapi10_s
 *
 *  The provided data passed with oyOptions_s shall be processed according to
 *  the rules of a given command. The result is again a generic oyOptions_s
 *  object.
 *
 *  This module type works similiar to the device modules, which are defined in
 *  oyCMMapi8_s. While the module interface is simple it has minimal meaning to
 *  the intented work or data exchange. The rules are to be defined by the
 *  communicating parties, the host and the module. They use the plug-in
 *  mechanism only for their convinience.
 *
 *  For speed up the "can_handle.xxx" command must be implemented. "xxx" means
 *  in the previous command string the intented command to be performed. The
 *  "can_handle" command shall not produce any output in result, but shall
 *  indicate with zero that it could handle the request. This command is
 *  intented to lightwight select a capable module.
 *
 *  @param[in]     options             the input data; It is typical not
 *                                     changed.
 *  @param[in]     command             a string describing the intention of the
 *                                     caller; Allowed are alphanumerical signs.
 *                                     lower bar '_', minus '-', plus '+' and
 *                                     point '.' . other signs including white
 *                                     space must not occure.
 *                                     Strings are separated by point '.' .
 *                                     The first string is the actual command.
 *                                     The following strings are merely
 *                                     modifiers.
 *  @param[out]    result              the created data
 *  @return                            0 - indifferent, >= 1 - error,
 *                                     <= -1 - issue,
 *                                     + a message should be sent
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/11 (Oyranos: 0.1.10)
 *  @date    2009/12/11
 */
typedef int  (*oyMOptions_Handle_f)  ( oyOptions_s       * options,
                                       const char        * command,
                                       oyOptions_s      ** result );

/** @struct  oyCMMapi10_s
 *  @brief   generic data exchange
 *  @ingroup module_api
 *  @extends oyCMMapiFilter_s
 *
 *  This API provides a very generic interface to exchange data between Oyranos
 *  core and a module.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/11 (Oyranos: 0.1.10)
 *  @date    2009/12/11
 */
struct oyCMMapi10_s {
  oyOBJECT_e       type;               /**< struct type oyOBJECT_CMM_API10_S */
  oyStruct_Copy_f  copy;               /**< copy function */
  oyStruct_Release_f release;          /**< release function */
  oyObject_s       oy_;                /**< @private base object */
  oyCMMapi_s     * next;

  oyCMMInit_f      oyCMMInit;          /**< */
  oyCMMMessageFuncSet_f oyCMMMessageFuncSet; /**< */

  /** e.g. "sw/oyranos.org/colour.tonemap.imaging/hydra.shiva.CPU.GPU" or "sw/oyranos.org/colour/icc.lcms.CPU",
      see as well @ref registration */
  const char     * registration;

  /** 0: major - should be stable for the live time of a filter, \n
      1: minor - mark new features, \n
      2: patch version - correct errors */
  int32_t          version[3];

  /** 0: last major Oyranos version during development time, e.g. 0
   *  1: last minor Oyranos version during development time, e.g. 0
   *  2: last Oyranos patch version during development time, e.g. 10
   */
  int32_t          module_api[3];

  char           * id_;                /**< @private Oyranos id; keep to zero */
  oyCMMapi5_s    * api5_;              /**< @private meta module; keep to zero*/

  /** translated, e.g. "help": "createICC" "create a ICC profile" "With ..."
   */
  oyCMMGetText_f   getText;
  /** zero terminated list for ::getText()
   *  include "help", "can_handle" and the supported commands
   */
  char          ** texts;              /**< zero terminated list for getText */

  oyMOptions_Handle_f oyMOptions_Handle; /**< @memberof oyCMMapi10_s */
};



/* implemented filter functions */
int      oyFilterPlug_ImageRootRun   ( oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket );
int      oyFilterMessageFunc         ( int                 code,
                                       const oyStruct_s  * context,
                                       const char        * format,
                                       ... );
int          oyFilterSocket_MatchImagingPlug (
                                       oyFilterSocket_s  * socket,
                                       oyFilterPlug_s    * plug );

const char * oyCMMgetText            ( const char        * select,
                                       oyNAME_e            type );
extern const char * oyCMM_texts[4];

/* miscellaneous helpers */
int oyCMMWarnFunc( int code, const oyStruct_s * context, const char * format, ... );

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_CMM_H */
