
/** @file oyName_s.h

   [Template file inheritance graph]
   +-- Name_s.template.h

 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2004-2011 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @since    2011/01/30
 */


#ifndef OY_NAME_S_H
#define OY_NAME_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#include <oyranos_object.h>

#include "oyStruct_s.h"

typedef struct oyName_s oyName_s;


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
oyName_s *   oyName_new              ( oyObject_s          object );

oyName_s *   oyName_copy             ( oyName_s          * obj,
                                       oyObject_s          object );
int          oyName_release          ( oyName_s         ** obj );
/* } oyName_s common object functions */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_NAME_S_H */
