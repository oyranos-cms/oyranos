/** @file oyOptions_s_.h

   [Template file inheritance graph]
   +-> Options_s_.template.h
   |
   +-> BaseList_s_.h
   |
   +-- Base_s_.h

 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2004-2011 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @since    2011/02/09
 */


#ifndef OY_OPTIONS_S__H
#define OY_OPTIONS_S__H

#define oyOptionsPriv_m( var ) ((oyOptions_s_*) (var))



#include <libxml/parser.h>
  
#include <oyranos_object.h>

#include "oyStructList_s.h"
  

#include "oyOptions_s.h"

typedef struct oyOptions_s_ oyOptions_s_;
/* Include "Options.private.h" { */

/* } Include "Options.private.h" */

/** @internal
 *  @struct   oyOptions_s_
 *  @brief      generic Options
 *  @ingroup  objects_value
 *  @extends  oyStruct_s
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
struct oyOptions_s_ {

/* Include "Struct.members.h" { */
oyOBJECT_e           type_;          /**< @private struct type */
oyStruct_Copy_f      copy;           /**< copy function */
oyStruct_Release_f   release;        /**< release function */
oyObject_s           oy_;            /**< @private features name and hash */

/* } Include "Struct.members.h" */


  oyStructList_s     * list_;          /**< the list data */

};



oyOptions_s_*
  oyOptions_New_( oyObject_s object );
oyOptions_s_*
  oyOptions_Copy_( oyOptions_s_ *options, oyObject_s object);
oyOptions_s_*
  oyOptions_Copy__( oyOptions_s_ *options, oyObject_s object);
int
  oyOptions_Release_( oyOptions_s_ **options );

#if 0
/* FIXME Are these needed at all?? */
int
           oyOptions_MoveIn_         ( oyOptions_s_      * list,
                                       oyOption_s       ** ptr,
                                       int                 pos );
int
           oyOptions_ReleaseAt_      ( oyOptions_s_      * list,
                                       int                 pos );
oyOption_s *
           oyOptions_Get_            ( oyOptions_s_      * list,
                                       int                 pos );
int
           oyOptions_Count_          ( oyOptions_s_      * list );
#endif



/* Include "Options.private_methods_declarations.h" { */
int            oyOptions_Init__Members(oyOptions_s_      * s );
int            oyOptions_Copy__Members(oyOptions_s_      * dest,
                                       oyOptions_s_      * src );
int            oyOptions_Release__Members (
                                       oyOptions_s_      * s );

void           oyOptions_ParseXML_   ( oyOptions_s_      * s,
                                       char            *** texts,
                                       int               * texts_n,
                                       xmlDocPtr           doc,
                                       xmlNodePtr          cur );

/* } Include "Options.private_methods_declarations.h" */




#endif /* OY_OPTIONS_S__H */
