/** @file oyOption_s.h

   [Template file inheritance graph]
   +-> oyOption_s.template.h
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



#ifndef OY_OPTION_S_H
#define OY_OPTION_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyOption_s oyOption_s;


#include "oyStruct_s.h"


/* Include "Option.public.h" { */
#define OY_STRING_LIST                 0x01 /**< create a oyVAL_STRING_LIST */
/* decode */
#define oyToStringList_m(r)            ((r)&1)

/** @enum    oyOPTIONSOURCE_e
 *  @brief   a option source type
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/13 (Oyranos: 0.1.9)
 *  @date    2008/11/27
 */
typedef enum {
  oyOPTIONSOURCE_NONE = 0,             /**< not clear */
  oyOPTIONSOURCE_FILTER = 2,           /**< internal defaults, e.g. module */
  oyOPTIONSOURCE_DATA = 4,             /**< external defaults, e.g. policy, DB*/
  oyOPTIONSOURCE_USER = 8              /**< user settings, e.g. GUI */
} oyOPTIONSOURCE_e;

/* Value Definitions { */
/** @enum    oyVALUETYPE_e
 *  @brief   a value type
 *
 *  @see     oyValue_u
 *  @see     oyValueTypeText()
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/05/04
 *  @since   2008/02/16 (Oyranos: 0.1.x)
 */
typedef enum {
  oyVAL_NONE,      /* for programming */
  oyVAL_INT,       /**< integer number */
  oyVAL_INT_LIST,  /**< integer numbers */
  oyVAL_DOUBLE,    /**< IEEE double precission floating point number */
  oyVAL_DOUBLE_LIST,  /**< double numbers */
  oyVAL_STRING,    /**< char array */
  oyVAL_STRING_LIST,  /**< array of char arrays */
  oyVAL_STRUCT,    /**< for pure data blobs use oyBlob_s herein */
  oyVAL_MAX        /* for programming */
} oyVALUETYPE_e;

/** @union   oyValue_u
 *  @brief   a value
 *  @ingroup objects_value
 *
 *  @see     oyVALUETYPE_e
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/02/16 (Oyranos: 0.1.x)
 *  @date    2008/02/16
 */
typedef union {
  int32_t          int32;
  int32_t        * int32_list;         /**< first is element number of int32 list */
  double           dbl;
  double         * dbl_list;           /**< first is element number of dbl list */
  char           * string;             /**< null terminated */
  char          ** string_list;        /**< null terminated */

  oyStruct_s     * oy_struct;          /**< a struct, e.g. a profile, or oyBlob_s for a data pointer */
} oyValue_u;

void           oyValueCopy           ( oyValue_u         * to,
                                       oyValue_u         * from,
                                       oyVALUETYPE_e       type,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocateFunc );
void           oyValueRelease        ( oyValue_u        ** value,
                                       oyVALUETYPE_e       type,
                                       oyDeAlloc_f         deallocateFunc );
void           oyValueClear          ( oyValue_u         * v,
                                       oyVALUETYPE_e       type,
                                       oyDeAlloc_f         deallocateFunc );
int            oyValueEqual          ( oyValue_u         * a,
                                       oyValue_u         * b,
                                       oyVALUETYPE_e       type,
                                       int                 pos );
/* } Value Definitions */

/* } Include "Option.public.h" */


/* Include "Option.dox" { */
/** @struct  oyOption_s
 *  @ingroup objects_value
 *  @extends oyStruct_s
 *  @brief   Option object
 *  
    @todo include the oyOptions_t_ type for gui elements
    should be used in a list oyColorTransformOptions_s to form a options set
    oyOptions_t_ covers as well the UI part which should be separated as of the
    SimpleToolkitAbstraction project:
    http://www.oyranos.org/wiki/index.php?title=XML_Plug-in_options
    As of this architecture change The UI part must be decided to obtain a
    place. Probably in oyOptions_s?
    Thus the oyOption_s::name member should be removed.

 *  The id field maps to a oyWidget_s object.
 *  Options and widgets are to be queried by the according function / CMM
 *  combination.
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/00/00 (Oyranos: 0.1.x)
 *  @date    2008/04/14
 */

/* } Include "Option.dox" */

struct oyOption_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};


OYAPI oyOption_s* OYEXPORT
  oyOption_New( oyObject_s object );
OYAPI oyOption_s* OYEXPORT
  oyOption_Copy( oyOption_s *option, oyObject_s obj );
OYAPI int OYEXPORT
  oyOption_Release( oyOption_s **option );



/* Include "Option.public_methods_declarations.h" { */
OYAPI oyOption_s  *    OYEXPORT
                oyOption_FromRegistration (
                                       const char        * registration,
                                       oyObject_s          object );
OYAPI int  OYEXPORT
                oyOption_GetId       ( oyOption_s        * option );
OYAPI const  char *    OYEXPORT
                oyOption_GetText     ( oyOption_s        * obj,
                                       oyNAME_e            type );
OYAPI int  OYEXPORT
                oyOption_SetFromString(oyOption_s        * obj,
                                       const char        * text,
                                       uint32_t            flags );
OYAPI char  *  OYEXPORT
                oyOption_GetValueText( oyOption_s        * obj,
                                       oyAlloc_f           allocateFunc );
OYAPI const  char *  OYEXPORT
                oyOption_GetValueString (
                                       oyOption_s        * obj,
                                       int                 pos );
OYAPI int  OYEXPORT
                 oyOption_SetFromInt ( oyOption_s        * obj,
                                       int32_t             integer,
                                       int                 pos,
                                       uint32_t            flags );
OYAPI int32_t  OYEXPORT
                 oyOption_GetValueInt( oyOption_s        * obj,
                                       int                 pos );
OYAPI int  OYEXPORT
                 oyOption_SetFromDouble (
                                       oyOption_s        * obj,
                                       double              floating_point,
                                       int                 pos,
                                       uint32_t            flags );
OYAPI double  OYEXPORT
                 oyOption_GetValueDouble(oyOption_s        * obj,
                                       int                 pos );
OYAPI int  OYEXPORT
                 oyOption_Clear      ( oyOption_s        * s );
OYAPI int  OYEXPORT
                 oyOption_SetFromData( oyOption_s        * option,
                                       oyPointer           ptr,
                                       size_t              size );
OYAPI oyPointer  OYEXPORT
                 oyOption_GetData    ( oyOption_s        * option,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc );
OYAPI int  OYEXPORT
                 oyOption_SetRegistration (
                                       oyOption_s        * option,
                                       const char        * registration );
OYAPI const  char *  OYEXPORT
                oyOption_GetRegistration (
                                       oyOption_s        * option );
OYAPI int  OYEXPORT
                 oyOption_MoveInStruct(oyOption_s        * option,
                                       oyStruct_s       ** s );
OYAPI oyStruct_s *  OYEXPORT
                oyOption_GetStruct   ( oyOption_s        * option,
                                       oyOBJECT_e          type );
OYAPI int  OYEXPORT
                 oyOption_SetValueFromDB (
                                       oyOption_s        * option );
OYAPI void  OYEXPORT
                 oyOption_SetSource  ( oyOption_s        * option,
                                       oyOPTIONSOURCE_e    source );
OYAPI oyOPTIONSOURCE_e  OYEXPORT
                 oyOption_GetSource  ( oyOption_s        * option );
OYAPI int  OYEXPORT
                 oyOption_GetFlags   ( oyOption_s        * object );
OYAPI int  OYEXPORT
                 oyOption_SetFlags   ( oyOption_s        * object,
                                       uint32_t            flags );
OYAPI oyVALUETYPE_e OYEXPORT
                 oyOption_GetValueType(oyOption_s        * object );

const char *     oyValueTypeText     ( oyVALUETYPE_e       type );


/* } Include "Option.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_OPTION_S_H */
