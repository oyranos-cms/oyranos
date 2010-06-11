/** @file oyStruct_s.h
 *
 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2004-2009 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @since    r
 */


#ifndef OY_STRUCT_S_H
#define OY_STRUCT_S_H

#include "oyStruct.h"

struct oyStruct_s {
  
  /* Struct base class start */
  oyOBJECT_e           type_;          /**< @private struct type */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< @private features name and hash */
  /* Struct base class stop */
  
  
};

#endif /* OY_STRUCT_S_H */
