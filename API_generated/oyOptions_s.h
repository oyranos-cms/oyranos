/** @file oyOptions_s.h

   [Template file inheritance graph]
   +-> Options_s.template.h
   |
   +-> BaseList_s.h
   |
   +-> Base_s.h
   |
   +-- Struct_s.template.h

 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2004-2011 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @since    2011/01/26
 */



#ifndef OY_OPTIONS_S_H
#define OY_OPTIONS_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>




#include "oyStruct_s.h"

#include "oyOption_s.h"

#include "oyObserver_s.h"


typedef struct oyOptions_s oyOptions_s;
/* Include "Options.public.h" { */
#define OY_CREATE_NEW                  0x02        /** create */
#define OY_ADD_ALWAYS                  0x04        /** do not use for configurations */
/* decode */
#define oyToCreateNew_m(r)             (((r) >> 1)&1)
#define oyToAddAlways_m(r)             (((r) >> 2)&1)

/* } Include "Options.public.h" */


/* Include "Options.dox" { */
/** @struct  oyOptions_s
 *  @ingroup objects_value
 *  @extends oyStruct_s
 *  @brief   generic Options
 *  
 *  Options can be any flag or rendering intent and other informations needed to
 *  configure a process. The object contains a list of oyOption_s objects.
 *  @note New templates will not be created automaticly [notemplates]
 *  @note This class holds a list of objects [list]
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */

/* } Include "Options.dox" */

struct oyOptions_s {
/* Include "Struct.members.h" { */
oyOBJECT_e           type_;          /**< @private struct type */
oyStruct_Copy_f      copy;           /**< copy function */
oyStruct_Release_f   release;        /**< release function */
oyObject_s           oy_;            /**< @private features name and hash */

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



/* Include "Options.public_methods_declarations.h" { */
oyOptions_s *  oyOptions_FromBoolean ( oyOptions_s       * pattern,
                                       oyOptions_s       * options,
                                       oyBOOLEAN_e         type,
                                       oyObject_s          object );
#ifdef OY_FILTER_CORE_S_H
oyOptions_s *  oyOptions_ForFilter   ( const char        * registration,
                                       const char        * cmm,
                                       uint32_t            flags,
                                       oyObject_s          object );
#endif /* OY_FILTER_CORE_S_H */
oyOptions_s *  oyOptions_FromText    ( const char        * text,
                                       uint32_t            flags,
                                       oyObject_s          object );

int            oyOptions_Add         ( oyOptions_s       * options,
                                       oyOption_s        * option,
                                       int                 pos,
                                       oyObject_s          object );
int            oyOptions_Set         ( oyOptions_s       * options,
                                       oyOption_s        * option,
                                       int                 pos,
                                       oyObject_s          object );
int            oyOptions_SetOpts     ( oyOptions_s       * list,
                                       oyOptions_s       * add );
int            oyOptions_AppendOpts  ( oyOptions_s       * list,
                                       oyOptions_s       * append );
int            oyOptions_CopyFrom    ( oyOptions_s      ** list,
                                       oyOptions_s       * from,
                                       oyBOOLEAN_e         type,
                                       oyFILTER_REG_e      fields,
                                       oyObject_s          object );
int            oyOptions_DoFilter    ( oyOptions_s       * s,
                                       uint32_t            flags,
                                       const char        * filter_type );
int            oyOptions_Filter      ( oyOptions_s      ** add_list,
                                       int32_t           * count,
                                       uint32_t            flags,
                                       oyBOOLEAN_e         type,
                                       const char        * registration,
                                       oyOptions_s       * src_list );
const char *   oyOptions_GetText     ( oyOptions_s       * options,
                                       oyNAME_e            type );
int            oyOptions_CountType   ( oyOptions_s       * options,
                                       const char        * registration,
                                       oyOBJECT_e          type );
oyOption_s *   oyOptions_Find        ( oyOptions_s       * options,
                                       const char        * key );
const char *   oyOptions_FindString  ( oyOptions_s       * options,
                                       const char        * key,
                                       const char        * value );
int            oyOptions_SetFromText ( oyOptions_s      ** obj,
                                       const char        * key,
                                       const char        * value,
                                       uint32_t            flags );
int            oyOptions_FindInt     ( oyOptions_s       * options,
                                       const char        * key,
                                       int                 pos,
                                       int32_t           * result );
int            oyOptions_SetFromInt  ( oyOptions_s      ** obj,
                                       const char        * key,
                                       int32_t             integer,
                                       int                 pos,
                                       uint32_t            flags );
int            oyOptions_FindDouble  ( oyOptions_s       * options,
                                       const char        * key,
                                       int                 pos,
                                       double            * result );
int            oyOptions_SetFromDouble(oyOptions_s      ** obj,
                                       const char        * key,
                                       double              value,
                                       int                 pos,
                                       uint32_t            flags );
oyStruct_s *   oyOptions_GetType     ( oyOptions_s       * options,
                                       int                 pos,
                                       const char        * registration,
                                       oyOBJECT_e          type );
int            oyOptions_MoveInStruct( oyOptions_s      ** obj,
                                       const char        * registration,
                                       oyStruct_s       ** oy_struct,
                                       uint32_t            flags );
int            oyOptions_SetFromData ( oyOptions_s      ** options,
                                       const char        * registration,
                                       oyPointer           ptr,
                                       size_t              size,
                                       uint32_t            flags );
int            oyOptions_FindData    ( oyOptions_s       * options,
                                       const char        * registration,
                                       oyPointer         * result,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc );
int            oyOptions_SetSource   ( oyOptions_s       * options,
                                       oyOPTIONSOURCE_e    source );
OYAPI int  OYEXPORT
               oyOptions_SaveToDB    ( oyOptions_s       * options,
                                       const char        * key_base_name );
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
int             oyOptions_Handle     ( const char        * registration,
                                       oyOptions_s       * options,
                                       const char        * command,
                                       oyOptions_s      ** result );

/* } Include "Options.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_OPTIONS_S_H */
