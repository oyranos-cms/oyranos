/** @file oyranos_color_internal.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2019-2020 (C) Kai-Uwe Behrmann
 *
 *  @internal
 *  @brief    color APIs and more
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2019/03/19
 */

#ifndef OYRANOS_COLOR_INTERNAL_H
#define OYRANOS_COLOR_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* libOyranosConfig */
extern float bb_100K[92][3];
double oyEstimateTemperature         ( double              cie_a_,
                                       double              cie_b_,
                                       double            * dist );
/* libOyranos */
double oyGetTemperature              ( double              default_ );

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_COLOR_INTERNAL_H */

