/** @file oyOption_s_.h

   [Template file inheritance graph]
   +-> oyOption_s_.template.h
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


#ifndef OY_OPTION_S__H
#define OY_OPTION_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyOptionPriv_m( var ) ((oyOption_s_*) (var))

typedef struct oyOption_s_ oyOption_s_;

  
#include <oyranos_object.h>
  

#include "oyOption_s.h"

/* Include "Option.private.h" { */

/* } Include "Option.private.h" */

/** @internal
 *  @struct   oyOption_s_
 *  @brief      Option object
 *  @ingroup  objects_value
 *  @extends  oyStruct_s
 */
struct oyOption_s_ {

/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */



/* Include "Option.members.h" { */
  uint32_t             id;             /**< id to map to events and widgets */
  char               * registration;   /**< full key path name to store configuration, e.g. "shared/oyranos.org/imaging/scale/x", see as well @ref registration @see oyOPTIONATTRIBUTE_e */
  int                  version[3];     /**< as for oyCMMapi4_s::version */
  oyVALUETYPE_e        value_type;     /**< the type in value */
  oyValue_u          * value;          /**< the actual value */
  oyOPTIONSOURCE_e     source;         /**< the source of this value */
  uint32_t             flags;          /**< | oyOPTIONATTRIBUTE_e */

/* } Include "Option.members.h" */

};


oyOption_s_*
  oyOption_New_( oyObject_s object );
oyOption_s_*
  oyOption_Copy_( oyOption_s_ *option, oyObject_s object);
oyOption_s_*
  oyOption_Copy__( oyOption_s_ *option, oyObject_s object);
int
  oyOption_Release_( oyOption_s_ **option );



/* Include "Option.private_methods_declarations.h" { */
int            oyOption_Match_       ( oyOption_s_       * option_a,
                                       oyOption_s_       * option_b );
void         oyOption_UpdateFlags_   ( oyOption_s_       * o );
int            oyOption_GetId_       ( oyOption_s_       * obj );
int            oyOption_SetFromString_(oyOption_s_       * obj,
                                       const char        * text,
                                       uint32_t            flags );
char *         oyOption_GetValueText_( oyOption_s_       * obj,
                                       oyAlloc_f           allocateFunc );
int            oyOption_SetFromInt_  ( oyOption_s_       * obj,
                                       int32_t             integer,
                                       int                 pos,
                                       uint32_t            flags );
int            oyOption_MoveInStruct_( oyOption_s_       * option,
                                       oyStruct_s       ** s );

/* } Include "Option.private_methods_declarations.h" */



void oyOption_Release__Members( oyOption_s_ * option );
int oyOption_Init__Members( oyOption_s_ * option );
int oyOption_Copy__Members( oyOption_s_ * dst, oyOption_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_OPTION_S__H */
