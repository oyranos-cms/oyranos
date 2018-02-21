/** @file oyranos_misc.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2006-2013 (C) Kai-Uwe Behrmann
 *
 *  @brief    misc
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2006/07/29
 */

#include <math.h>
#include "oyranos_types.h"

float        oyLinInterpolateRampU16c( uint16_t          * ramp,
                                       int                 ramp_size,
                                       int                 ramp_channel,
                                       int                 ramp_channels,
                                       float               pos )
{
  uint16_t val1, val2;
  float start, dist, result;

  if(!ramp)
    return 0.0;

  if(pos < 0)
    return ramp[0];

  if(pos >= 1.0)
    return ramp[ramp_size-1];

  dist = modff( pos*(ramp_size-1), &start );
  val1 = ramp[(int)start*ramp_channels+ramp_channel];
  val2 = ramp[(int)start*ramp_channels+ramp_channel+ramp_channels];

  result = val2 - val1;
  result *= dist;
  result += val1;

  return result;
}
float        oyLinInterpolateRampU16 ( uint16_t          * ramp,
                                       int                 ramp_size,
                                       float               pos )
{
  return oyLinInterpolateRampU16c( ramp, ramp_size, 0, 1, pos );
}

float        oyLinInterpolateRampF32 ( float             * ramp,
                                       int                 ramp_size,
                                       float               pos )
{
  float val1, val2;
  float start, dist, result;

  if(!ramp)
    return 0.0;

  if(pos < 0)
    return ramp[0];

  if(pos >= 1.0)
    return ramp[ramp_size-1];

  dist = modff( pos*(ramp_size-1), &start );
  val1 = ramp[(int)start];
  val2 = ramp[(int)start+1];

  result = val2 - val1;
  result *= dist;
  result += val1;

  return result;
}

double       oyLinInterpolateRampF64 ( double            * ramp,
                                       int                 ramp_size,
                                       double              pos )
{
  double val1, val2;
  double start, dist, result;

  if(!ramp)
    return 0.0;

  if(pos < 0)
    return ramp[0];

  if(pos >= 1.0)
    return ramp[ramp_size-1];

  dist = modf( pos*(ramp_size-1), &start );
  val1 = ramp[(int)start];
  val2 = ramp[(int)start+1];

  result = val2 - val1;
  result *= dist;
  result += val1;

  return result;
}

#ifdef HAVE_POSIX
#define MANIPULATION_FUNCTION( type, operator, name ) \
inline type name( type val1, type val2 ) \
{ return val1 operator val2; }
#else
#define MANIPULATION_FUNCTION( type, operator, name ) \
type name( type val1, type val2 ) \
{ return val1 operator val2; }
#endif
MANIPULATION_FUNCTION( uint16_t, +, oyAddU16 )
MANIPULATION_FUNCTION( uint16_t, -, oySubstU16 )
MANIPULATION_FUNCTION( uint16_t, *, oyMultU16 )
MANIPULATION_FUNCTION( uint16_t, /, oyDivU16 )

int            oyRampManipulateU16   ( uint16_t          * ramp1,
                                       uint16_t          * ramp2,
                                       uint16_t          * ramp_result,
                                       int                 ramp_size,
                                       uint16_t          (*manip_f)(uint16_t,uint16_t) )
{
  int error = !ramp1 || !ramp2 || !ramp_result || !manip_f,
      i;

  if(!error)
    for(i = 0; i < ramp_size; ++i)
      ramp_result[i] = manip_f( ramp1[i], ramp2[i] );

  return error;
}

MANIPULATION_FUNCTION( float, +, oyAddF32 )
MANIPULATION_FUNCTION( float, -, oySubstF32 )
MANIPULATION_FUNCTION( float, *, oyMultF32 )
MANIPULATION_FUNCTION( float, /, oyDivF32 )

int            oyRampManipulateF32   ( float             * ramp1,
                                       float             * ramp2,
                                       float             * ramp_result,
                                       int                 ramp_size,
                                       float             (*manip_f)(float,float) )
{
  int error = !ramp1 || !ramp2 || !ramp_result || !manip_f,
      i;

  if(!error)
    for(i = 0; i < ramp_size; ++i)
      ramp_result[i] = manip_f( ramp1[i], ramp2[i] );

  return error;
}

MANIPULATION_FUNCTION( double, +, oyAddF64 )
MANIPULATION_FUNCTION( double, -, oySubstF64 )
MANIPULATION_FUNCTION( double, *, oyMultF64 )
MANIPULATION_FUNCTION( double, /, oyDivF64 )

int            oyRampManipulateF64   ( double            * ramp1,
                                       double            * ramp2,
                                       double            * ramp_result,
                                       int                 ramp_size,
                                       double            (*manip_f)(double,double) )
{
  int error = !ramp1 || !ramp2 || !ramp_result || !manip_f,
      i;

  if(!error)
    for(i = 0; i < ramp_size; ++i)
      ramp_result[i] = manip_f( ramp1[i], ramp2[i] );

  return error;
}

