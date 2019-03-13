/** @file oyranos_color.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2008-2019 (C) Kai-Uwe Behrmann
 *
 *  @brief    color definitions
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2008/01/20
 */


#ifndef OYRANOS_COLOR_H
#define OYRANOS_COLOR_H

#include <stdlib.h> /* for linux size_t */
#include "icc34.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void                    oyLab2XYZ       (const double * CIElab, double * XYZ);
void                    oyXYZ2Lab       (const double * XYZ, double * CIElab);
void         oyCIEabsXYZ2ICCrelXYZ   ( const double      * CIEXYZ,
                                       double            * ICCXYZ,
                                       const double      * XYZmin,
                                       const double      * XYZmax,
                                       const double      * XYZwhite );
void         oyICCXYZrel2CIEabsXYZ   ( const double      * ICCXYZ,
                                       double            * CIEXYZ,
                                       const double      * XYZmin,
                                       const double      * XYZmax,
                                       const double      * XYZwhite );

/** \addtogroup color_low
 *  @{ */

/* The oyVEC3, oyMAT3, oyMAT3inverse, oyVEC3init and oyMAT3per definitions 
 * origin from lcms2' cmsmtrx.c written by Marti Maria www.littlecms.com 
 * and is MIT licensed there
 */
/** @brief Vector of 3 doubles
 */
typedef struct {
  double n[3];                         /**< triple of doubles */ 
} oyVEC3;
 
/** @brief 3x3 Matrix
 */
typedef struct {
  oyVEC3 v[3];                         /**< 3x3 matrix */
} oyMAT3;

/** @brief vector of 3 doubles
 */
typedef struct {
        double X;
        double Y;
        double Z;

} oyCIEXYZ;

/** @} */ /* color_low */

int          oyMAT3inverse           ( const oyMAT3      * a,
                                       oyMAT3            * b);
void         oyVEC3init              ( oyVEC3            * r,
                                       double              x,
                                       double              y,
                                       double              z );
void         oyMAT3per               ( oyMAT3            * r,
                                       const oyMAT3      * a,
                                       const oyMAT3      * b );
void         oyMAT3eval              ( oyVEC3            * r,
                                       const oyMAT3      * a,
                                       const oyVEC3      * v );
int          oyAdaptationMatrix      ( oyMAT3            * r,
                                       const oyMAT3      * ConeMatrix,
                                       const oyCIEXYZ    * FromIll,
                                       const oyCIEXYZ    * ToIll );

/* End of lcms code */

/** \addtogroup color_low
 *  @{ */

/** @brief CIE*xy
 */
typedef struct {
  double xy[2];                        /**< CIE*xy */
} oyCIExyY;
/** @brief CIE*xy triple
 */
typedef struct {
  oyCIExyY v[3];                     /**< CIE*xy triple */
} oyCIExyYTriple;

/** @} */ /* color_low */

int          oyMAT3toCIExyYTriple  ( const oyMAT3    * a,
                                       oyCIExyYTriple  * triple );
const char * oyMAT3show ( const oyMAT3* a );
const char * oyMat34show ( const float a[3][4] );
const char * oyMat4show ( const float a[4] );
const char * oyMat43show ( const float a[4][3] );
const char * oyCIExyYTriple_Show( oyCIExyYTriple * triple );

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_COLOR_H */
