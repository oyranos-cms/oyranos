
/** @file oyranos_image_internal.h

   [Template file inheritance graph]
   +-- oyranos_image_internal.template.h

 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2004-2012 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @date     2012/09/15
 */


#ifndef OYRANOS_IMAGE_INTERNAL_H
#define OYRANOS_IMAGE_INTERNAL_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#include "oyranos_image.h"

int                oySizeofDatatype  ( oyDATATYPE_e        t );
const char *       oyDatatypeToText  ( oyDATATYPE_e        t );

#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */


#endif /* OYRANOS_IMAGE_INTERNAL_H */
