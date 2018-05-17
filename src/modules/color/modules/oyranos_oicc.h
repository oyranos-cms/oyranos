/** @file oyranos_oicc.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2013 (C) Kai-Uwe Behrmann
 *
 *  @brief    registration definitions for Oyranos backends
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2013/10/10
 */

#ifndef OYRANOS_CMM_OICC_H
#define OYRANOS_CMM_OICC_H

#include "oyStruct_s.h"

/* --- internal definitions --- */

#define CMM_NICK "oicc"
#define CMM_VERSION {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C}

int  oiccInit                        ( oyStruct_s        * module_info );

#endif /* OYRANOS_CMM_OICC_H */
