
/** @file oyranos_object_internal.h

   [Template file inheritance graph]
   +-- oyranos_object_internal.template.h

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


#ifndef OYRANOS_OBJECT_INTERNAL_H
#define OYRANOS_OBJECT_INTERNAL_H

#include <string.h>
#include <math.h>

#include <oyranos_object.h>
#include <oyranos_helper.h>
#include <oyranos_debug.h>
#include <oyranos_i18n.h>
#include <oyranos_string.h>

#include "oyranos_object.h"
/*#include "oyCMMapiFilters_s.h"*/

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define hashTextAdd_m( text_ ) \
  oyjlStringAdd( &hash_text, s->oy_->allocateFunc_, \
                             s->oy_->deallocateFunc_, text_ )

int oyCheckType_( oyOBJECT_e type1, oyOBJECT_e type2 );

#define oyCheckType_m( typ, action ) \
  if( !s || oyCheckType_(s->type_, typ)) \
  { char * text = oyBT(0); \
    const char * info = oyStruct_GetInfo(s, oyNAME_NAME, 0x01); \
    WARNc5_S( "%s %s %s(%s)\n%s", _("Unexpected object type:"), oyNoEmptyString_m_(info), \
              oyStructTypeToText( s ? s->type : oyOBJECT_NONE ), \
              oyStructTypeToText( typ ), text?text:"" ) \
    oyFree_m_(text); \
    action; \
  }
#define oyCheckType__m( type, action ) \
  if( !s || oyCheckType_( s->type_, type )) \
  { char * text = oyBT(0); \
    const char * info = oyStruct_GetInfo(s, oyNAME_NAME, 0x01); \
    WARNc5_S( "%s %s %s(%s)\n%s", _("Unexpected object type:"), oyNoEmptyString_m_(info), \
              oyStructTypeToText( s ? s->type_ : oyOBJECT_NONE ), \
              oyStructTypeToText( type ), text?text:"") \
    oyFree_m_(text); \
    action; \
  }
#define oyCheckTypeRange_m( type1, type2, action ) \
  if( !s || ( s->type_ < type1 || s->type_ > type2 )) \
  { char * text = oyBT(0); \
    const char * info = oyStruct_GetInfo(s, oyNAME_NAME, 0x01); \
    WARNc6_S( "%s %s %s(%s-%s)\n%s", _("Unexpected object type:"), oyNoEmptyString_m_(info), \
              oyStructTypeToText( s ? s->type_ : oyOBJECT_NONE ), \
              oyStructTypeToText( type1 ), oyStructTypeToText( type2 ), text?text:"") \
    oyFree_m_(text); \
    action; \
  }

#define OY_ERR if(l_error != 0) error = l_error;

int    oyTextIccDictMatch            ( const char        * text,
                                       const char        * pattern,
                                       double              delta,
                                       char                path_separator,
                                       char                key_separator );
int    oyPointerRelease              ( oyPointer         * ptr );

#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */


#endif /* OYRANOS_OBJECT_INTERNAL_H */
