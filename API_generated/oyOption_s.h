/** @file oyOption_s.h

   [Template file inheritance graph]
   +-> Option_s.template.h
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



#ifndef OY_OPTION_S_H
#define OY_OPTION_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>


#include "oyStruct_s.h"


typedef struct oyOption_s oyOption_s;
/* Include "Option.public.h" { */
#define OY_STRING_LIST                 0x01 /** create a oyVAL_STRING_LIST */
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
  oyOPTIONSOURCE_USER = 8              /**< user settings, e.g. elektra */
} oyOPTIONSOURCE_e;

/* Value Definitions { */
/** @enum    oyVALUETYPE_e
 *  @brief   a value type
 *
    @see     oyValue_u
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/02/16 (Oyranos: 0.1.x)
 *  @date    2008/02/16
 */
typedef enum {
  oyVAL_INT,
  oyVAL_INT_LIST,
  oyVAL_DOUBLE,    /*!< IEEE double precission floating point number */
  oyVAL_DOUBLE_LIST,
  oyVAL_STRING,
  oyVAL_STRING_LIST,
  oyVAL_STRUCT     /**< for pure data blobs use oyBlob_s herein */
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
    should be used in a list oyColourTransformOptions_s to form a options set
    oyOptions_t_ covers as well the UI part which should be separated as of the
    SimpleToolkitAbstraction project:
    http://www.oyranos.org/wiki/index.php?title=XML_Plug-in_options
    As of this architecture change The UI part must be decided to obtain a
    place. Probably in oyOptions_s?
    Thus the oyOption_s::name member should be removed.

 *  The id field maps to a oyWidget_s object.
 *  Options and widgets are to be queried by the according function / CMM
 *  combination.
 *  @note New templates will not be created automaticly [notemplates]
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/00/00 (Oyranos: 0.1.x)
 *  @date    2008/04/14
 */

/* } Include "Option.dox" */

struct oyOption_s {
/* Include "Struct.members.h" { */
oyOBJECT_e           type_;          /**< @private struct type */
oyStruct_Copy_f      copy;           /**< copy function */
oyStruct_Release_f   release;        /**< release function */
oyObject_s           oy_;            /**< @private features name and hash */

/* } Include "Struct.members.h" */
};


OYAPI oyOption_s* OYEXPORT
  oyOption_New( oyObject_s object );
OYAPI oyOption_s* OYEXPORT
  oyOption_Copy( oyOption_s *option, oyObject_s obj );
OYAPI int OYEXPORT
  oyOption_Release( oyOption_s **option );



/* Include "Option.public_methods_declarations.h" { */
oyOption_s *   oyOption_FromRegistration( const char        * registration,
                                          oyObject_s          object );
#if HAVE_ELEKTRA
oyOption_s *   oyOption_FromDB       ( const char        * registration,
                                       oyObject_s          object );
#endif
int            oyOption_GetId        ( oyOption_s        * option );
const char *   oyOption_GetText      ( oyOption_s        * obj,
                                       oyNAME_e            type );
int            oyOption_SetFromText  ( oyOption_s        * obj,
                                       const char        * text,
                                       uint32_t            flags );
char *         oyOption_GetValueText ( oyOption_s        * obj,
                                       oyAlloc_f           allocateFunc );
int            oyOption_SetFromInt   ( oyOption_s        * obj,
                                       int32_t             integer,
                                       int                 pos,
                                       uint32_t            flags );
int32_t        oyOption_GetValueInt  ( oyOption_s        * obj,
                                       int                 pos );
int            oyOption_SetFromDouble( oyOption_s        * obj,
                                       double              floating_point,
                                       int                 pos,
                                       uint32_t            flags );
double         oyOption_GetValueDouble(oyOption_s        * obj,
                                       int                 pos );
int            oyOption_Clear        ( oyOption_s        * s );
int            oyOption_SetFromData  ( oyOption_s        * option,
                                       oyPointer           ptr,
                                       size_t              size );
oyPointer      oyOption_GetData      ( oyOption_s        * option,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc );
int            oyOption_SetRegistration (
                                       oyOption_s        * option,
                                       const char        * registration );
const char *   oyOption_GetRegistration (
                                       oyOption_s        * option );
#if HAVE_ELEKTRA
int            oyOption_SetValueFromDB(oyOption_s        * option );
#endif
int            oyOption_StructMoveIn ( oyOption_s        * option,
                                       oyStruct_s       ** s );
oyStruct_s *   oyOption_StructGet    ( oyOption_s        * option,
                                       oyOBJECT_e          type );
void           oyOption_SetSource    ( oyOption_s        * option,
                                       oyOPTIONSOURCE_e    source );
oyOPTIONSOURCE_e oyOption_GetSource  ( oyOption_s        * option );


/* } Include "Option.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_OPTION_S_H */
