/** @file oyOptions_s.h

   [Template file inheritance graph]
   +-> oyOptions_s.template.h
   |
   +-> BaseList_s.h
   |
   +-> Base_s.h
   |
   +-- oyStruct_s.template.h

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */



#ifndef OY_OPTIONS_S_H
#define OY_OPTIONS_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyOptions_s oyOptions_s;




#include "oyStruct_s.h"

#include "oyOption_s.h"

#include "oyranos.h"
#include "oyObserver_s.h"


/* Include "Options.public.h" { */
/** \addtogroup objects_value
 *  @{ *//* objects_value */
#define OY_CREATE_NEW                  0x02        /**< create */
#define OY_ADD_ALWAYS                  0x04        /**< no check for double occurencies; do not use for configurations */
#define OY_MATCH_KEY                   0x08        /**< check for double occurencies of key name, ignoring the path */
/* decode */
#define oyToCreateNew_m(r)             (((r) >> 1)&1)
#define oyToAddAlways_m(r)             (((r) >> 2)&1)
/** @} *//* objects_value */

/* } Include "Options.public.h" */


/* Include "Options.dox" { */
/** @struct  oyOptions_s
 *  @ingroup objects_value
 *  @extends oyStruct_s
 *  @brief   generic Options
 *  
 *  Options can be any flag or rendering intent and other informations needed to
 *  configure a process. The object contains a list of oyOption_s objects.
 *  @note This class holds a list of objects [list]
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */

/* } Include "Options.dox" */

struct oyOptions_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};



OYAPI oyOptions_s* OYEXPORT
  oyOptions_New( oyObject_s object );
OYAPI oyOptions_s* OYEXPORT
  oyOptions_Copy( oyOptions_s *options, oyObject_s obj );
OYAPI int OYEXPORT
  oyOptions_Release( oyOptions_s **options );

OYAPI int  OYEXPORT
           oyOptions_MoveIn          ( oyOptions_s       * list,
                                       oyOption_s       ** ptr,
                                       int                 pos );
OYAPI int  OYEXPORT
           oyOptions_ReleaseAt       ( oyOptions_s       * list,
                                       int                 pos );
OYAPI oyOption_s * OYEXPORT
           oyOptions_Get             ( oyOptions_s       * list,
                                       int                 pos );
OYAPI int  OYEXPORT
           oyOptions_Count           ( oyOptions_s       * list );
OYAPI int  OYEXPORT
           oyOptions_Clear           ( oyOptions_s       * list );
OYAPI int  OYEXPORT
           oyOptions_Sort            ( oyOptions_s       * list,
                                       int32_t           * rank_list );



/* Include "Options.public_methods_declarations.h" { */
OYAPI oyOptions_s *  OYEXPORT
                oyOptions_FromBoolean( oyOptions_s       * pattern,
                                       oyOptions_s       * options,
                                       oyBOOLEAN_e         type,
                                       oyObject_s          object );
#ifdef OY_FILTER_CORE_S_H
#endif /* OY_FILTER_CORE_S_H */
OYAPI oyOptions_s *  OYEXPORT
                oyOptions_FromText   ( const char        * text,
                                       uint32_t            flags,
                                       oyObject_s          object );
OYAPI int  OYEXPORT
                oyOptions_FromJSON   ( const char        * json_text,
                                       oyOptions_s       * options,
                                       oyOptions_s      ** result,
                                       const char        * xformat,
                                       ... );

OYAPI int  OYEXPORT
                 oyOptions_Add       ( oyOptions_s       * options,
                                       oyOption_s        * option,
                                       int                 pos,
                                       oyObject_s          object );
OYAPI int  OYEXPORT
                 oyOptions_Set       ( oyOptions_s       * options,
                                       oyOption_s        * option,
                                       int                 pos,
                                       oyObject_s          object );
OYAPI int  OYEXPORT
                 oyOptions_SetOpts   ( oyOptions_s       * list,
                                       oyOptions_s       * add );
OYAPI int  OYEXPORT
                 oyOptions_AppendOpts( oyOptions_s       * list,
                                       oyOptions_s       * append );
OYAPI int  OYEXPORT
                 oyOptions_CopyFrom  ( oyOptions_s      ** list,
                                       oyOptions_s       * from,
                                       oyBOOLEAN_e         type,
                                       oyFILTER_REG_e      fields,
                                       oyObject_s          object );
OYAPI int  OYEXPORT
                 oyOptions_DoFilter  ( oyOptions_s       * s,
                                       uint32_t            flags,
                                       const char        * filter_type );
OYAPI int  OYEXPORT
                 oyOptions_Filter    ( oyOptions_s      ** add_list,
                                       int32_t           * count,
                                       uint32_t            flags,
                                       oyBOOLEAN_e         type,
                                       const char        * registration,
                                       oyOptions_s       * src_list );
OYAPI const char *  OYEXPORT
                oyOptions_GetText    ( oyOptions_s       * options,
                                       oyNAME_e            type );
OYAPI int  OYEXPORT
                 oyOptions_CountType ( oyOptions_s       * options,
                                       const char        * registration,
                                       oyOBJECT_e          type );
OYAPI oyOption_s *  OYEXPORT
                oyOptions_Find       ( oyOptions_s       * options,
                                       const char        * key,
                                       uint32_t            type );
OYAPI const char *  OYEXPORT
                oyOptions_FindString ( oyOptions_s       * options,
                                       const char        * key,
                                       const char        * value );
OYAPI int  OYEXPORT
                 oyOptions_SetFromString (
                                       oyOptions_s      ** obj,
                                       const char        * key,
                                       const char        * value,
                                       uint32_t            flags );
OYAPI int  OYEXPORT
                 oyOptions_FindInt   ( oyOptions_s       * options,
                                       const char        * key,
                                       int                 pos,
                                       int32_t           * result );
OYAPI int  OYEXPORT
                 oyOptions_SetFromInt( oyOptions_s      ** obj,
                                       const char        * key,
                                       int32_t             integer,
                                       int                 pos,
                                       uint32_t            flags );
OYAPI int  OYEXPORT
                 oyOptions_FindDouble( oyOptions_s       * options,
                                       const char        * key,
                                       int                 pos,
                                       double            * result );
OYAPI int  OYEXPORT
                 oyOptions_SetFromDouble (
                                       oyOptions_s      ** obj,
                                       const char        * key,
                                       double              value,
                                       int                 pos,
                                       uint32_t            flags );
OYAPI oyStruct_s *  OYEXPORT
                 oyOptions_GetType   ( oyOptions_s       * options,
                                       int                 pos,
                                       const char        * registration,
                                       oyOBJECT_e          type );
OYAPI int  OYEXPORT
                 oyOptions_GetType2  ( oyOptions_s       * options,
                                       int                 pos,
                                       const char        * pattern,
                                       uint32_t            pattern_type,
                                       oyOBJECT_e          object_type,
                                       oyStruct_s       ** result,
                                       oyOption_s       ** option );
OYAPI int  OYEXPORT
                 oyOptions_MoveInStruct(
                                       oyOptions_s      ** obj,
                                       const char        * registration,
                                       oyStruct_s       ** oy_struct,
                                       uint32_t            flags );
OYAPI int  OYEXPORT
                 oyOptions_SetFromData(oyOptions_s      ** options,
                                       const char        * registration,
                                       oyPointer           ptr,
                                       size_t              size,
                                       uint32_t            flags );
OYAPI int  OYEXPORT
                 oyOptions_FindData  ( oyOptions_s       * options,
                                       const char        * registration,
                                       oyPointer         * result,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc );
OYAPI int  OYEXPORT
                 oyOptions_SetSource ( oyOptions_s       * options,
                                       oyOPTIONSOURCE_e    source );
OYAPI int  OYEXPORT
               oyOptions_ObserverAdd ( oyOptions_s       * object,
                                       oyStruct_s        * observer,
                                       oyStruct_s        * user_data,
                                       oyObserver_Signal_f signalFunc );
OYAPI int  OYEXPORT
               oyOptions_ObserversDisable (
                                       oyOptions_s       * object );
OYAPI int  OYEXPORT
               oyOptions_ObserversEnable (
                                       oyOptions_s       * object );
/* needs modules from oyranos_modules */
OYAPI int  OYEXPORT
                 oyOptions_Handle    ( const char        * registration,
                                       oyOptions_s       * options,
                                       const char        * command,
                                       oyOptions_s      ** result );
/* needs modules from oyranos_modules */
OYAPI oyOptions_s *  OYEXPORT
                oyOptions_ForFilter  ( const char        * registration,
                                       uint32_t            flags,
                                       oyObject_s          object );
OYAPI int  OYEXPORT
           oyOptions_SetDriverContext( oyOptions_s      ** options,
                                       oyPointer           driver_context,
                                       const char        * driver_context_type,
                                       size_t              driver_context_size,
                                       oyObject_s          object );
/* needs DB from oyranos_config */
OYAPI int  OYEXPORT oyOptions_SaveToDB(oyOptions_s       * options,
                                       oySCOPE_e           scope,
                                       const char        * registration,
                                       char             ** new_reg,
                                       oyAlloc_f           allocateFunc );

/* } Include "Options.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_OPTIONS_S_H */
