
/** @file oyObject_s_.h

   [Template file inheritance graph]
   +-- Object_s_.template.h

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


#ifndef OY_OBJECT_S__H
#define OY_OBJECT_S__H

#include <oyranos_object.h>

#include "oyObject_s.h"


/* Include "Object.private_methods_declarations.h" { */
oyObject_s   oyObject_SetAllocators_  ( oyObject_s        object,
                                        oyAlloc_f         allocateFunc,
                                        oyDeAlloc_f       deallocateFunc );
int          oyObject_Ref            ( oyObject_s          obj );
int32_t      oyObject_Hashed_        ( oyObject_s          s );
int          oyObject_HashSet          ( oyObject_s        s,
                                         const unsigned char * hash );
int          oyObject_HashEqual        ( oyObject_s        s1,
                                         oyObject_s        s2 );

/* } Include "Object.private_methods_declarations.h" */


#endif /* OY_OBJECT_S__H */
