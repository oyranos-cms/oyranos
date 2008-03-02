/**
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2007  Kai-Uwe Behrmann
 *
 * @autor: Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * -----------------------------------------------------------------------------
 *
 * ICC definitions
 * 
 */

/** @date      03. 07. 2007 */


#ifndef OYRANOS_ICC_H
#define OYRANOS_ICC_H

#include <stdlib.h> /* for linux size_t */
#include "oyranos_definitions.h"
#include "oyranos_version.h"
#include "icc34.h"

/** \namespace oyranos
    @brief The Oyranos namespace.
 */
#ifdef __cplusplus
extern "C" {
namespace oyranos
{
#endif /* __cplusplus */

icUInt16Number          oyValueUInt16   (icUInt16Number val);
icUInt32Number          oyValueUInt32   (icUInt32Number val);
unsigned long           oyValueUInt64   (icUInt64Number val);
icColorSpaceSignature   oyValueCSpaceSig(icColorSpaceSignature val);
icPlatformSignature     oyValuePlatSig  (icPlatformSignature val);
icProfileClassSignature oyValueProfCSig (icProfileClassSignature val);
icTagSignature          oyValueTagSig   (icTagSignature val);

#ifdef __cplusplus
} /* extern "C" */
} /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OYRANOS_H */
