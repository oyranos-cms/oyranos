{% load gsoc_extensions %}
{% include "source_file_header.txt" %}

#ifndef OYRANOS_OBJECT_H
#define OYRANOS_OBJECT_H

{% include "cpp_begin.h" %}

#include <oyranos_core.h>

/* Function Pointers Definitions Start */
typedef oyPointer (*oyPointer_copy_f )   ( oyPointer, size_t );
typedef int       (*oyPointer_release_f )( oyPointer * );
typedef void      (*oyLockRelease_f )( oyPointer           lock,
                                       const char        * marker,
                                       int                 line );
typedef void      (*oyLock_f )       ( oyPointer           lock,
                                       const char        * marker,
                                       int                 line );
typedef void      (*oyUnLock_f )     ( oyPointer           look,
                                       const char        * marker,
                                       int                 line );
/* Function Pointers Definitions Stop */

/* Declare oyObject_s Globally */
typedef struct oyObject_s_* oyObject_s;

/**
 *  @brief   Oyranos structure type
 *  @ingroup objects_generic
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/11/22 (Oyranos: 0.1.x)
 *  @date    2008/07/31
 */
#if 0
typedef enum {
  oyOBJECT_NONE,
{% for class in classes %}{% ifequal class.group "objects_generic" %}
  oyOBJECT_{{ class.baseName|underscores|upper }}_S,   /**< {{ class.name }} */{% endifequal %}{% endfor %}
{% for class in classes %}{% ifequal class.group "objects_value" %}
  oyOBJECT_{{ class.baseName|underscores|upper }}_S,   /**< {{ class.name }} */{% endifequal %}{% endfor %}
  oyOBJECT_CMM_HANDLE_S      = 50,    /**< oyCMMhandle_s */
  oyOBJECT_CMM_API1_S,                 /**< oyCMMapi_s */
  oyOBJECT_CMM_API2_S,                /**< oyCMMapi2_s */
{% for class in classes %}{% ifequal class.group "module_api" %}
  oyOBJECT_{{ class.baseName|underscores|upper }}_S,   /**< {{ class.name }} */{% endifequal %}{% endfor %}
  oyOBJECT_CMM_API_MAX,               /**< not defined */
{% for class in classes %}{% ifequal class.group "cmm_handling" %}
  {% ifequal class.baseName "Info" %}
  oyOBJECT_{{ class.baseName|underscores:"strict"|upper }}_S,   /**< {{ class.name }} */
  {% else %}
  oyOBJECT_{{ class.baseName|underscores|upper }}_S,   /**< {{ class.name }} */
  {% endifequal %}{% endifequal %}{% endfor %}
{% for class in classes %}{% ifequal class.group "objects_profile" %}
  oyOBJECT_{{ class.baseName|underscores|upper }}_S,   /**< {{ class.name }} */{% endifequal %}{% endfor %}
  oyOBJECT_NAMED_COLORS_S,
  oyOBJECT_MAX
} oyOBJECT_e;
#else
typedef enum {
  oyOBJECT_NONE,
  oyOBJECT_OBJECT_S,                  /**< oyObject_s */
  oyOBJECT_MONITOR_S,                 /**< oyMonitor_s */
  oyOBJECT_NAMED_COLOR_S,            /*!< oyNamedColor_s */
  oyOBJECT_NAMED_COLORS_S,           /*!< oyNamedColors_s */
  oyOBJECT_PROFILE_S,                 /*!< oyProfile_s */
  oyOBJECT_PROFILE_TAG_S,             /*!< oyProfileTag_s */
  oyOBJECT_PROFILES_S,                /*!< oyProfiles_s */
  oyOBJECT_OPTION_S,                  /*!< oyOption_s */
  oyOBJECT_OPTIONS_S,                 /*!< oyOptions_s */
  oyOBJECT_RECTANGLE_S,               /**< oyRectangle_s */
  oyOBJECT_IMAGE_S,                   /*!< oyImage_s */
  oyOBJECT_ARRAY2D_S,                 /**< oyArray2d_s */
  oyOBJECT_CONNECTOR_S       = 20,    /**< oyConnector_s */
  oyOBJECT_CONNECTOR_IMAGING_S,       /**< oyConnectorImaging_s */
  oyOBJECT_CONNECTOR_MAX_S,           /**< not defined */
  oyOBJECT_FILTER_PLUG_S     = 30,    /**< oyFilterPlug_s */
  oyOBJECT_FILTER_PLUGS_S,            /**< oyFilterPlugs_s */
  oyOBJECT_FILTER_SOCKET_S,           /**< oyFilterSocket_s */
  oyOBJECT_FILTER_CORE_S,             /**< oyFilterCore_s */
  oyOBJECT_FILTER_CORES_S,            /**< oyFilterCores_s */
  oyOBJECT_FILTER_NODE_S,             /**< oyFilterNode_s */
  oyOBJECT_FILTER_NODES_S,            /**< oyFilterNodes_s */
  oyOBJECT_FILTER_GRAPH_S,            /**< oyFilterGraph_s */
  oyOBJECT_PIXEL_ACCESS_S,            /**< oyPixelAccess_s */
  oyOBJECT_CONVERSION_S,              /**< oyConversion_s */
  oyOBJECT_CMM_HANDLE_S      = 50,    /**< oyCMMhandle_s */
  oyOBJECT_POINTER_S,                 /*!< oyPointer_s */
  oyOBJECT_CMM_INFO_S,                /*!< oyCMMinfo_s */
  oyOBJECT_CMM_API_S,                 /**< oyCMMapi_s */
  oyOBJECT_CMM_APIS_S,                /**< oyCMMapis_s */
  oyOBJECT_CMM_API1_S,                /**< oyCMMapi1_s */
  oyOBJECT_CMM_API2_S,                /**< oyCMMapi2_s */
  oyOBJECT_CMM_API3_S,                /**< oyCMMapi3_s */
  oyOBJECT_CMM_API4_S,                /**< oyCMMapi4_s */
  oyOBJECT_CMM_API5_S,                /**< oyCMMapi5_s */
  oyOBJECT_CMM_API6_S,                /**< oyCMMapi6_s */
  oyOBJECT_CMM_API7_S,                /**< oyCMMapi7_s */
  oyOBJECT_CMM_DATA_TYPES_S,          /**< oyCMMDataTypes_s */
  oyOBJECT_CMM_API8_S,                /**< oyCMMapi8_s */
  oyOBJECT_CMM_API_FILTER_S,          /**< oyCMMapiFilter_s */
  oyOBJECT_CMM_API_FILTERS_S,         /**< oyCMMapiFilters_s */
  oyOBJECT_CMM_API9_S,                /**< oyCMMapi9_s */
  oyOBJECT_CMM_API10_S,               /**< oyCMMapi10_s */
  oyOBJECT_CMM_UI_S,                  /**< oyCMMui_s */
  oyOBJECT_CMM_OBJECT_TYPE_S,         /**< oyCMMobjectType_s */
  oyOBJECT_CMM_API_MAX,               /**< not defined */
  oyOBJECT_ICON_S      = 80,          /*!< oyIcon_s */
  oyOBJECT_MODULE_S,                  /*!< oyModule_s */
  oyOBJECT_EXTERNFUNC_S,              /*!< oyExternFunc_s */
  oyOBJECT_NAME_S,                    /*!< oyName_s */
  oyOBJECT_COMP_S_,                   /*!< oyComp_s_ */
  oyOBJECT_FILE_LIST_S_,              /*!< oyFileList_s_ */
  oyOBJECT_HASH_S,                    /**< oyHash_s */
  oyOBJECT_STRUCT_LIST_S,             /**< oyStructList_s */
  oyOBJECT_BLOB_S,                    /**< oyBlob_s */
  oyOBJECT_CONFIG_S,                  /**< oyConfig_s */
  oyOBJECT_CONFIGS_S,                 /**< oyConfigs_s */
  oyOBJECT_UI_HANDLER_S,              /**< oyUiHandler_s */
  oyOBJECT_FORMS_ARGS_S,              /**< oyFormsArgs_s */
  oyOBJECT_CALLBACK_S,                /**< oyCallback_s */
  oyOBJECT_OBSERVER_S,                /**< oyObserver_s */
  oyOBJECT_CONF_DOMAIN_S,             /**< oyConfDomain_s */
  oyOBJECT_INFO_STATIC_S,             /**< oyObjectInfoStatic_s */
  oyOBJECT_LIST_S,                   /**< oyList_s */
  oyOBJECT_LIS_S,                   /**< oyLis_s dummy */
  oyOBJECT_MAX
} oyOBJECT_e;
#endif
const char *     oyStructTypeToText  ( oyOBJECT_e          type );


/** @enum    oyNAME_e
 *  @brief   describe the base types of a oyObject_s name
 *  @ingroup objects_generic
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/10/00 (Oyranos: 0.1.8)
 *  @date    2007/10/00
 */
typedef enum {
  oyNAME_NAME,                         /**< compatible to oyName_s/oyObject_s */
  oyNAME_NICK,                         /**< compatible to oyName_s/oyObject_s */
  oyNAME_DESCRIPTION                   /**< compatible to oyName_s/oyObject_s */
} oyNAME_e;

#define oyNAME_XML_VALUE (oyNAME_DESCRIPTION + 1)

/** @brief   select from filter
 *  @ingroup objects_value
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/27 (Oyranos: 0.1.10)
 *  @date    2009/07/27
 */
#define OY_SELECT_FILTER         2048
/** @brief   select from policy
 *  @ingroup objects_value
 *
 *  Select typical from a associatable oyCMMapi9_s type of filter for providing
 *  common options.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/27 (Oyranos: 0.1.10)
 *  @date    2009/07/27
 */
#define OY_SELECT_COMMON         4096
/** @enum    oyOPTIONATTRIBUTE_e
 *  @brief   usage type
 *  @ingroup objects_value
 *
 *  The types of Oyranos default settings to include into the options set.
 *  So proofing makes no sense for toolkit widgets, but for advanced
 *  graphics displaying. All other Oyranos behaviour settings should be taken
 *  over untouched.
 *
 *  The ".invisible" attribute says a option shall not be displayed in a GUI.
 *  The ".readonly" attribute alllows for unchangeable, static options. They
 *  might be used for passing additional informations.
 *  The ".font" and ".advanced" attributes are described for the according
 *  enum values.
 *
 *  See as well @ref registration
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/10/08 (Oyranos: 0.1.8)
 *  @date    2008/11/27
 */
typedef enum {
  /** basic settings, as typical for toolkits and office/web applications,
   *  e.g. disable proofing or disable mark out of gamut colors */
  oyOPTIONATTRIBUTE_BASIC = 0,
  /** user modified, e.g. after oyOption_SetFromText() */
  oyOPTIONATTRIBUTE_EDIT = 32,
  /** automatically modified, e.g. by a CMM */
  oyOPTIONATTRIBUTE_AUTOMATIC = 64,
  /** advanced settings, as typical for editing, e.g. include proofing
   *  (options appended with ".advanced") */
  oyOPTIONATTRIBUTE_ADVANCED = 128,
  /** front end options, handled by the framework or policy module,
   *  e.g. ".front" */
  oyOPTIONATTRIBUTE_FRONT = 256,
  /** tell this options is included twice*/
  oyOPTIONATTRIBUTE_DOUBLE = 512
} oyOPTIONATTRIBUTE_e;


/** @enum    oyBOOLEAN_e
 *  @brief   boolean operations
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/28 (Oyranos: 0.1.8)
 *  @date    2008/06/28
 */
typedef enum {
  oyBOOLEAN_INTERSECTION,              /**< and, the part covered by A and B */
  oyBOOLEAN_SUBSTRACTION,              /**< minus, the part covered by A but not by B */
  oyBOOLEAN_DIFFERENZ,                 /**< xor, exclusive parts of A and B */
  oyBOOLEAN_UNION                      /**< or, the parts covered by A or B or both */
} oyBOOLEAN_e;


/** see:http://lists.freedesktop.org/archives/openicc/2008q4/001724.html
 *  @ingroup objects_conversion
 *
 *  @see @ref registration
 */
typedef enum {
  oyFILTER_REG_NONE = 0,
  oyFILTER_REG_TOP = 0x01,             /**< e.g. "shared" for filters */
  oyFILTER_REG_DOMAIN = 0x02,          /**< e.g. "oyranos.org" */
  oyFILTER_REG_TYPE = 0x04,            /**< e.g. "imaging" filter group */
  oyFILTER_REG_APPLICATION = 0x08,     /**< e.g. "scale" filter name */
  oyFILTER_REG_OPTION = 0x10,          /**< e.g. "x" filter option */
  oyFILTER_REG_MAX = 0x20
} oyFILTER_REG_e;

char * oyFilterRegistrationToText    ( const char        * registration,
                                       oyFILTER_REG_e      type,
                                       oyAlloc_f           allocateFunc );
int    oyFilterRegistrationMatch     ( const char        * registration,
                                       const char        * pattern,
                                       oyOBJECT_e          api_number );
int    oyFilterRegistrationMatchKey  ( const char        * registration1,
                                       const char        * registration2,
                                       oyOBJECT_e          api_number );


/** @internal
 *  @struct  oyRankMap
 *  @brief   a means to rank the result of comparing two key's
 *
 *  The structure is part of an other data structure and to be maintained by
 *  that. So it is not a full member of the oyStruct_s object familiy.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/27 (Oyranos: 0.1.10)
 *  @date    2009/01/27
*/
typedef struct {
  char *  key;                         /**< key name to evaluate with this pad*/
  int32_t match_value;                 /**< add to rank sum when matching */
  int32_t none_match_value;            /**< add to rank sum when not matching*/
  int32_t not_found_value;             /**< add to rank sum when entry is not found */
} oyRankMap;

oyRankMap *        oyRankMapCopy     ( const oyRankMap   * rank_map,
                                       oyAlloc_f           allocateFunc );

/** @enum     oySTATUS_e
 *  @ingroup  objects_profile
 *  @brief    File status type
 */
typedef enum {
  oyOK,
  oyCORRUPTED,
  oyUNDEFINED
} oySTATUS_e;

typedef enum {
  oyERROR_USER      = 10,              /**< wrong or missing argument */
  oyERROR_DATA_READ,                   /**< input error */
  oyERROR_DATA_WRITE,                  /**< output error */
  oyERROR_DATA_AMBIGUITY               /**< uniqueness of data is conflicted */
} oyERROR_e;

int          oyTextboolean_          ( const char        * text_a,
                                       const char        * text_b,
                                       oyBOOLEAN_e         type );
char * oyFilterRegistrationToText    ( const char        * registration,
                                       oyFILTER_REG_e      type,
                                       oyAlloc_f           allocateFunc );
int    oyFilterRegistrationMatch     ( const char        * registration,
                                       const char        * pattern,
                                       oyOBJECT_e          api_number );
int    oyFilterRegistrationMatchKey  ( const char        * registration1,
                                       const char        * registration2,
                                       oyOBJECT_e          api_number );

#define OY_OUTPUT                      0x01
#define OY_INPUT                       0x02
/* decode */
#define oyToOutput_m(v)                ((v)&0x01)
#define oyToInput_m(v)                 ((v)&0x02)


{% include "cpp_end.h" %}

#endif /* OYRANOS_OBJECT_H */
