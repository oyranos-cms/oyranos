/** @file oyCMMapi9_s_.h

   [Template file inheritance graph]
   +-> oyCMMapi9_s_.template.h
   |
   +-> oyCMMapiFilter_s_.template.h
   |
   +-> oyCMMapi_s_.template.h
   |
   +-- Base_s_.h

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


#ifndef OY_CMM_API9_S__H
#define OY_CMM_API9_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyCMMapi9Priv_m( var ) ((oyCMMapi9_s_*) (var))

typedef struct oyCMMapi9_s_ oyCMMapi9_s_;

  
#include <oyranos_object.h>




#include "oyranos_module_internal.h"
#include "oyCMMinfo_s_.h"

#include "oyCMMapi5_s_.h"

#include "oyCMMobjectType_s.h"
#include "oyConversion_s.h"
#include "oyCMMui_s_.h"
  

#include "oyCMMapi9_s.h"

/* Include "CMMapi9.private.h" { */
/** typedef  oyConversion_Correct_f
 *  @brief   Check for correctly adhering to policies
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


/** typedef  oyCMMGetFallback_f
 *  @brief   get pattern specific module fallback
 *  @ingroup module_api
 *  @memberof oyCMMapi9_s
 *
 *  @param[in]     node                the node context
 *  @param[in]     flags               unused
 *  @param[in]     select_core         apply to api4 core else to api7
 *  @param[in]     allocate_func       user supplied memory allocator
 *  @return                            the fallback string
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/06/25
 *  @since   2014/06/25 (Oyranos: 0.9.6)
 */
typedef char * (*oyCMMGetFallback_f) ( oyFilterNode_s    * node,
                                       uint32_t            flags,
                                       int                 select_core,
                                       oyAlloc_f           allocate_func );

/** typedef  oyCMMRegistrationToName_f
 *  @brief   get names from a module registration
 *  @ingroup module_api
 *  @memberof oyCMMapi9_s
 *
 *  @param[in]     node                the node context
 *  @param[in]     name_type           oyNAME_e, oyNAME_PATTERN
 *  @param[in]     flags               unused
 *  @param[in]     select_core         apply to api4 core else to api7
 *  @param[in]     allocate_func       user supplied memory allocator
 *  @return                            the fallback string
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/06/29
 *  @since   2014/06/29 (Oyranos: 0.9.6)
 */
typedef char * (*oyCMMRegistrationToName_f) (
                                       const char        * registration,
                                       int                 name_type,
                                       uint32_t            flags,
                                       int                 select_core,
                                       oyAlloc_f           allocate_func );

/** typedef  oyCMMGetDefaultPattern_f
 *  @brief   get module default
 *  @ingroup module_api
 *  @memberof oyCMMapi9_s
 *
 *  @param[in]     pattern             the basic pattern
 *  @param[in]     flags               unused
 *  @param[in]     select_core         apply to api4 core else to api7
 *  @param[in]     allocate_func       user supplied memory allocator
 *  @return                            the default pattern string
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/07/21
 *  @since   2014/07/21 (Oyranos: 0.9.6)
 */
typedef char * (*oyCMMGetDefaultPattern_f) (
                                       const char        * base_pattern,
                                       uint32_t            flags,
                                       int                 select_core,
                                       oyAlloc_f           allocate_func );

/* } Include "CMMapi9.private.h" */

/** @internal
 *  @struct   oyCMMapi9_s_
 *  @brief      Graph policies and various pattern handlers
 *  @ingroup  module_api
 *  @extends  oyCMMapiFilter_s
 */
struct oyCMMapi9_s_ {

/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */





/* Include "CMMapi.members.h" { */
  oyCMMapi_s     * next;               /**< the next CMM api */

  oyCMMInit_f      oyCMMInit;          /**< */
  oyCMMReset_f     oyCMMReset;         /**< */
  oyCMMMessageFuncSet_f oyCMMMessageFuncSet; /**< */

  /** e.g. "sw/oyranos.org/color.tonemap.imaging/hydra.shiva.CPU.GPU" or "sw/oyranos.org/color/icc.lcms.CPU",
      see as well @ref registration */
  char           * registration;

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

/* } Include "CMMapi.members.h" */

/* Include "CMMapiFilter.members.h" { */
  oyCMMapi5_s_   * api5_;            /**< @private meta module; keep to zero */
  oyPointer_s    * runtime_context;  /**< data needed to run the filter */

/* } Include "CMMapiFilter.members.h" */

/* Include "CMMapi9.members.h" { */
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
  const char    ** texts;              /**< zero terminated categories for getText, e.g. {"///GPU","///CPU","//color",0} */

  oyConversion_Correct_f oyConversion_Correct; /**< check a graph */
  /** registration pattern which are supported by oyConversion_Correct \n
   *
   *  e.g. for imaging this could be "//imaging"
   */
  const char     * pattern;

  /** basic key which is used in the Oyranos DB together with "context",
   *  "renderer", "context_fallback" and "renderer_fallback" \n
   *
   *  e.g. for CMMs this could be OY_CMM_STD
   */
  const char     * key_base;

  /** get registration of fallback module for this pattern */
  oyCMMGetFallback_f oyCMMGetFallback;

  /** get name from module registration */
  oyCMMRegistrationToName_f oyCMMRegistrationToName;

  /** get the default pattern for a module group */
  oyCMMGetDefaultPattern_f oyCMMGetDefaultPattern;

/* } Include "CMMapi9.members.h" */

};


oyCMMapi9_s_*
  oyCMMapi9_New_( oyObject_s object );
oyCMMapi9_s_*
  oyCMMapi9_Copy_( oyCMMapi9_s_ *cmmapi9, oyObject_s object);
oyCMMapi9_s_*
  oyCMMapi9_Copy__( oyCMMapi9_s_ *cmmapi9, oyObject_s object);
int
  oyCMMapi9_Release_( oyCMMapi9_s_ **cmmapi9 );



/* Include "CMMapi9.private_methods_declarations.h" { */

/* } Include "CMMapi9.private_methods_declarations.h" */



void oyCMMapi9_Release__Members( oyCMMapi9_s_ * cmmapi9 );
int oyCMMapi9_Init__Members( oyCMMapi9_s_ * cmmapi9 );
int oyCMMapi9_Copy__Members( oyCMMapi9_s_ * dst, oyCMMapi9_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_API9_S__H */
