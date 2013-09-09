/** @internal
 *  @file oyranos_alpha_internal.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  Copyright (C) 2004-2008  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    misc alpha internal APIs
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *  new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2004/11/25
 */

#ifndef OYRANOS_ALPHA_INTERNAL_H
#define OYRANOS_ALPHA_INTERNAL_H

#include "oyranos_image.h"
#include "oyProfile_s.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** \addtogroup alpha Alpha API's

 *  @{
 */




int  oyColorConvert_ ( oyProfile_s       * p_in,
                        oyProfile_s       * p_out,
                        oyPointer           buf_in,
                        oyPointer           buf_out,
                        oyDATATYPE_e        buf_type_in,
                        oyDATATYPE_e        buf_type_out,
                        oyOptions_s       * options,
                        int                 count );







void     oyAlphaFinish_              ( int                 unused );


/** \addtogroup misc Miscellaneous

 *  @{
 */



/** @} *//*alpha
 */

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_ALPHA_INTERNAL_H */
