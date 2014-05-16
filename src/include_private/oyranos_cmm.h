/** @internal
 *  @file oyranos_cmm.h
 *
 *  Oyranos is an open source Color Management System 
 * 
 *  @par Copyright:
 *            2007-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief external CMM module API
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2007/11/12
 */


#ifndef OYRANOS_CMM_H
#define OYRANOS_CMM_H

#include "oyFilterPlug_s.h"
#include "oyPixelAccess_s.h"
#include "oyPointer_s.h"
#include "oyStruct_s.h"

#include "oyranos.h"
#include "oyranos_icc.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CMM_API_VERSION {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C}

/** @brief   CMM data to Oyranos cache
 *
 *  @param[in]     data                the data struct know to the module
 *  @param[in,out] oy                  the Oyranos cache struct to fill by the module
 *  @return                            error
 *
 *  @version Oyranos: 0.1.9
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 *  @date    2008/11/06
 */
typedef int      (*oyCMMobjectOpen_f)( oyStruct_s        * data,
                                       oyPointer_s       * oy );

typedef void     (*oyCMMProgress_f)  ( int                 ID,
                                       double              progress );


typedef icSignature (*oyCMMProfile_GetSignature_f) (
                                       oyPointer_s       * cmm_ptr,
                                       int                 pcs);

int              oyCMMlibMatchesCMM  ( const char        * lib_name,
                                       const char        * cmm );

/*oyPointer          oyCMMallocateFunc ( size_t              size );
void               oyCMMdeallocateFunc(oyPointer           mem );*/


typedef const char* (*oyWidgetGet_f) ( uint32_t          * result );

/* -------------------------------------------------------------------------*/

/* implemented filter functions */
const char * oyCMMgetText            ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );
extern const char * oyCMM_texts[4];

/* miscellaneous helpers */

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_CMM_H */
