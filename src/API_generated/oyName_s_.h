
/** @file oyName_s_.h

   [Template file inheritance graph]
   +-- oyName_s_.template.h

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


#ifndef OY_NAME_S__H
#define OY_NAME_S__H

#include <oyranos_object.h>

#include "oyName_s.h"

#include "oyStruct_s.h"



/* Include "Name.dox" { */
/** @struct oyName_s
 *  @ingroup objects_generic
 *  @brief   Oyranos Name structure
 *
 *  This class is a public members only class and does not
 *  belong to the oyranos object model, like all "Generic Objects".
 *
 *  @note New templates will not be created automaticly [notemplates]
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */

/* } Include "Name.dox" */

struct oyName_s {
  oyOBJECT_e           type;          /*!< internal struct type oyOBJECT_NAME_S */
  oyStruct_Copy_f      copy;          /**< copy function */
  oyStruct_Release_f   release;       /**< release function */
  oyPointer        dummy;             /**< keep to zero */
  char               * nick;          /*!< few letters for mass representation, eg. "A1" */
  char               * name;          /*!< normal visible name, eg. "A1-MySys"*/
  char               * description;   /*!< full user description, eg. "A1-MySys from Oyranos" */
  char                 lang[8];       /**< i18n language, eg. "en_GB" */
};


/* oyName_s common object functions { */
oyName_s *   oyName_newWith          ( oyAlloc_f           allocateFunc );
oyName_s *   oyName_new              ( oyObject_s          object );

oyName_s *   oyName_copy             ( oyName_s          * obj,
                                       oyObject_s          object );
int          oyName_release          ( oyName_s         ** obj );
/* } oyName_s common object functions */


int          oyName_release_         ( oyName_s         ** name,
                                       oyDeAlloc_f         deallocateFunc );

int          oyName_copy_            ( oyName_s          * dest,
                                       oyName_s          * src,
                                       oyObject_s          object );
oyName_s *   oyName_set_             ( oyName_s          * obj,
                                       const char        * text,
                                       oyNAME_e            type,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deallocateFunc );
const char * oyName_get_             ( const oyName_s    * obj,
                                       oyNAME_e            type );
int          oyName_releaseMembers   ( oyName_s          * obj,
                                       oyDeAlloc_f         deallocateFunc );

#endif /* OY_NAME_S__H */
