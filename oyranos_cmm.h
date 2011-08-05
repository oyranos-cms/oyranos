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
#include "oyPointer_s.h"

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

#define oyCMM_PROFILE "oyPR"
#define oyCMM_COLOUR_CONVERSION "oyCC"
#define oyCOLOUR_ICC_DEVICE_LINK "oyDL"


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
typedef int      (*oyCMMobjectOpen_f)( oyStruct_s        * data,
                                       oyPointer_s       * oy );

typedef void     (*oyCMMProgress_f)  ( int                 ID,
                                       double              progress );


typedef icSignature (*oyCMMProfile_GetSignature_f) (
                                       oyPointer_s       * cmm_ptr,
                                       int                 pcs);

int              oyCMMlibMatchesCMM  ( const char        * lib_name,
                                       const char        * cmm );

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

/* -------------------------------------------------------------------------*/
typedef struct oyCMMapi5_s oyCMMapi5_s;

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

  oyCMMGetText_f   getText;            /**< translated UI texts */
  /** zero terminated categories for getText. Support at least "name",
   *  e.g. {"name",0}
   *
   *  The "name" texts shall include information about the connector.
   *  The oyNAME_NICK is a very small shorthand for the cnnnector. e.g. "Img"
   *  oyNAME_NAME should contain the connectors name, e.g. "Image" 
   *  oyNAME_DESCRIPTION can contain some help text, e.g. "Image[PNG] File Input".
   */
  const char    ** texts;

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
 *  @param[in]     flags               for inbuild defaults |
 *                                     oyOPTIONSOURCE_FILTER;
 *                                     for options marked as advanced |
 *                                     oyOPTIONATTRIBUTE_ADVANCED |
 *                                     OY_SELECT_FILTER |
 *                                     OY_SELECT_COMMON
 *  @param   options                   options to the policy module
 *  @return                            0 - indifferent, >= 1 - error
 *                                     + a message should be sent
 *
 *  @version Oyranos: 0.1.13
 *  @since   2009/07/23 (Oyranos: 0.1.10)
 *  @date    2010/11/27
 */
typedef int  (*oyConversion_Correct_f) (
                                       oyConversion_s    * conversion,
                                       uint32_t            flags,
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

const char * oyCMMgetText            ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );
extern const char * oyCMM_texts[4];
const char * oyCMMgetImageConnectorPlugText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );
const char * oyCMMgetImageConnectorSocketText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );
extern const char * oy_image_connector_texts[2];

/* miscellaneous helpers */
int oyCMMWarnFunc( int code, const oyStruct_s * context, const char * format, ... );


/** @struct  oyObjectInfo_s
 *  @brief   a static object
 *  @extends oyStruct_s
 *
 *  The data field is to be placed alternatively to a oyObject_s at
 *  oyStruct_s::oy_ for a static object. It provides basic object data.
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/02 (Oyranos: 0.3.0)
 *  @date    2011/02/02
 */
typedef struct oyObjectInfoStatic_s {
  oyOBJECT_e       type;               /**< oyOBJECT_INFO_STATIC_S */
  oyPointer        dummy1;             /**< keep zero */
  oyPointer        dummy2;             /**< keep zero */
  oyPointer        dummy3;             /**< keep zero */
  /** inherited structures
   *  start with the position of the last member, followed by the first
   *  object type until the last one. Seven levels of inheritance are possible
   *  at maximum.
   */
  oyOBJECT_e       inheritance[8];
} oyObjectInfoStatic_s;

extern oyObjectInfoStatic_s oy_connector_imaging_static_object;

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_CMM_H */
