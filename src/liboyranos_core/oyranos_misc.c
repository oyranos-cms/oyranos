/** @file oyranos_misc.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2006-2013 (C) Kai-Uwe Behrmann
 *
 *  @brief    misc
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2006/07/29
 */

#include <math.h>
#include "oyranos_types.h"

float        oyLinInterpolateRampU16 ( uint16_t          * ramp,
                                       int                 ramp_size,
                                       float               pos )
{
  uint16_t val1, val2;
  float start, dist, result;

  if(!ramp)
    return 0.0;

  if(pos < 0)
    return ramp[0];

  if(pos > ramp_size-1)
    return ramp[ramp_size-1];

  dist = modff( pos, &start );
  val1 = ramp[(int)start];
  val2 = ramp[(int)start+1];

  result = val2 - val1;
  result *= dist;
  result += val1;

  return result;
}
