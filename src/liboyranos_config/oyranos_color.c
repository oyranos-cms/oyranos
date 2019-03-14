/** @file oyranos_color.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2008-2019 (C) Kai-Uwe Behrmann
 *
 *  @internal
 *  @brief    misc color APIs
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2008/01/20
 */


#include <math.h>
#include <stdio.h>
#include <string.h>

#include "oyranos_color.h"

/** \addtogroup color Color
 *  @brief Color and Color Management APIs

 *  @{ *//* color */

/** \addtogroup color_low Color calculations
 *  @brief Fast hardcoded color conversions

 *  @{ *//* color_low */


/**
 *  @brief CIE Lab to ICC XYZ
 */
void     oyLab2XYZ                   ( const double      * CIELab,
                                       double            * XYZ )
{
  const double * l = CIELab;
  /* double e = 216./24389.;             // 0.0088565 */
  /* double k = 24389./27.;              // 903.30 */
  double d = 6./29.;                  /* 24.0/116.0 0.20690 */

  double Xn = 0.964294;
  double Yn = 1.000000;
  double Zn = 0.825104;

  double fy = (l[0] + 16) / 116.;
  double fx = fy + l[1] / 500.;
  double fz = fy - l[2] / 200.;


  if(fy > d)
    XYZ[1] = Yn * pow( fy, 3 );
  else
    XYZ[1] = (fy - 16./116.) * 108.0/841.0 * Yn;
  if(fx > d)
    XYZ[0] = Xn * pow( fx, 3 );
  else
    XYZ[0] = (fx - 16./116.) * 108.0/841.0 * Xn;
  if(fz > d)
    XYZ[2] = Zn * pow( fz, 3 );
  else
    XYZ[2] = (fz - 16./116.) * 108.0/841.0 * Zn;
}

/**
 *  @brief ICC XYZ to CIE * Lab
 */
void     oyXYZ2Lab                   ( const double      * XYZ,
                                       double            * lab )
{
    /* white point D50 [0.964294 , 1.000000 , 0.825104]
     * XYZ->Lab is defined as (found with the help of Marti Maria):
     *
     * L* = 116*f(Y/Yn) - 16                     0 <= L* <= 100
     * a* = 500*[f(X/Xn) - f(Y/Yn)]
     * b* = 200*[f(Y/Yn) - f(Z/Zn)]
     *
     * and
     *
     *        f(t) = t^(1/3)                     1 >= t >  0.008856
     *         7.787*t + (16/116)          0 <= t <= 0.008856
     */

      double gamma = 1.0/3.0; /* standard is 1.0/3.0 */
      double XYZ_[3];
      double K = 24389./27.;
      double k = K/116.;      /* 7.787 */
      double e = 216./24389.; /* 0.008856 */
      int i;

      /* CIE XYZ -> CIE*Lab (D50) */
      XYZ_[0] = XYZ[0] / 0.964294;
      XYZ_[1] = XYZ[1] / 1.000000;
      XYZ_[2] = XYZ[2] / 0.825104;

      for(i = 0; i < 3; ++i)
      {
        if ( XYZ_[i] > e)
           XYZ_[i] = pow (XYZ_[i], gamma);
        else
           XYZ_[i] = k * XYZ_[i] + (16.0/116.0);
      }

      lab[0] = (116.0*  XYZ_[1] -  16.0);
      lab[1] = (500.0*( XYZ_[0] -  XYZ_[1]));
      lab[2] = (200.0*( XYZ_[1] -  XYZ_[2]));
}

/** Function oyCIEabsXYZ2ICCrelXYZ
 *  @brief CIE absolute colorimetric to ICC relative colorimetric
 *
 *  @param[in]     CIEXYZ              CIE absolute colorimetric input
 *  @param[out]    ICCXYZ              ICC relative colorimetric output
 *  @param[in]     XYZmin              CIE absolute black point
 *  @param[in]     XYZmax              CIE absolute media white
 *  @param[in]     XYZwhite            illuminant white
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/20 (API 0.1.8)
 */
void         oyCIEabsXYZ2ICCrelXYZ   ( const double      * CIEXYZ,
                                       double            * ICCXYZ,
                                       const double      * XYZmin,
                                       const double      * XYZmax,
                                       const double      * XYZwhite )
{
  int i = 0;
  for( ; i < 3; ++i )
    ICCXYZ[i] = (CIEXYZ[i] - XYZmin[i]) / (XYZmax[i] - XYZmin[i]) * XYZwhite[i];
}

/** Function oyICCXYZrel2CIEabsXYZ
 *  @brief ICC relative colorimetric to CIE absolute colorimetric
 *
 *  @param[in]     ICCXYZ              ICC relative colorimetric input
 *  @param[out]    CIEXYZ              CIE absolute colorimetric output
 *  @param[in]     XYZmin              CIE absolute black point
 *  @param[in]     XYZmax              CIE absolute media white
 *  @param[in]     XYZwhite            illuminant white
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/20 (API 0.1.8)
 */
void         oyICCXYZrel2CIEabsXYZ   ( const double      * ICCXYZ,
                                       double            * CIEXYZ,
                                       const double      * XYZmin,
                                       const double      * XYZmax,
                                       const double      * XYZwhite )
{
  int i = 0;
  for( ; i < 3; ++i )
    CIEXYZ[i] = (ICCXYZ[i] * (XYZmax[i] - XYZmin[i]) + XYZmin[i]) / XYZwhite[i];
}

// Compute chromatic adaptation matrix using Chad as cone matrix

static int ComputeChromaticAdaptation(oyMAT3* Conversion,
                                const oyCIEXYZ* SourceWhitePoint,
                                const oyCIEXYZ* DestWhitePoint,
                                const oyMAT3* Chad)

{

    oyMAT3 Chad_Inv;
    oyVEC3 ConeSourceXYZ, ConeSourceRGB;
    oyVEC3 ConeDestXYZ, ConeDestRGB;
    oyMAT3 Cone, Tmp;


    Tmp = *Chad;
    if (!oyMAT3inverse(&Tmp, &Chad_Inv)) return 0;

    oyVEC3init(&ConeSourceXYZ, SourceWhitePoint -> X,
                             SourceWhitePoint -> Y,
                             SourceWhitePoint -> Z);

    oyVEC3init(&ConeDestXYZ,   DestWhitePoint -> X,
                             DestWhitePoint -> Y,
                             DestWhitePoint -> Z);

    oyMAT3eval(&ConeSourceRGB, Chad, &ConeSourceXYZ);
    oyMAT3eval(&ConeDestRGB,   Chad, &ConeDestXYZ);

    // Build matrix
    oyVEC3init(&Cone.v[0], ConeDestRGB.n[0]/ConeSourceRGB.n[0],    0.0,  0.0);
    oyVEC3init(&Cone.v[1], 0.0,   ConeDestRGB.n[1]/ConeSourceRGB.n[1],   0.0);
    oyVEC3init(&Cone.v[2], 0.0,   0.0,   ConeDestRGB.n[2]/ConeSourceRGB.n[2]);


    // Normalize
    oyMAT3per(&Tmp, &Cone, Chad);
    oyMAT3per(Conversion, &Chad_Inv, &Tmp);

    return 1;
}
/** Returns the final chrmatic adaptation from illuminant FromIll to Illuminant ToIll.
 *  The cone matrix can be specified in ConeMatrix. If NULL, Bradford is assumed
 */
int  oyAdaptationMatrix(oyMAT3* r, const oyMAT3* ConeMatrix, const oyCIEXYZ* FromIll, const oyCIEXYZ* ToIll)
{
    oyMAT3 LamRigg   = {{ // Bradford matrix
        {{  0.8951,  0.2664, -0.1614 }},
        {{ -0.7502,  1.7135,  0.0367 }},
        {{  0.0389, -0.0685,  1.0296 }}
    }};

    if (ConeMatrix == NULL)
        ConeMatrix = &LamRigg;

    return ComputeChromaticAdaptation(r, FromIll, ToIll, ConeMatrix);
}

/* The oyVEC3, oyMAT3, oyMAT3inverse, oyVEC3init and oyMAT3per definitions 
 * origin from lcms' cmsmtrx.c written by Marti Maria www.littlecms.com 
 * and is MIT licensed there
 * Vectors
 */
#define MATRIX_DET_TOLERANCE    0.0001
/** @brief Inverse of a matrix b = a^(-1) */
int oyMAT3inverse(const oyMAT3* a, oyMAT3* b)
{
   double det, c0, c1, c2;

   c0 =  a -> v[1].n[1]*a -> v[2].n[2] - a -> v[1].n[2]*a -> v[2].n[1];
   c1 = -a -> v[1].n[0]*a -> v[2].n[2] + a -> v[1].n[2]*a -> v[2].n[0];
   c2 =  a -> v[1].n[0]*a -> v[2].n[1] - a -> v[1].n[1]*a -> v[2].n[0];

   det = a -> v[0].n[0]*c0 + a -> v[0].n[1]*c1 + a -> v[0].n[2]*c2;

   if (fabs(det) < MATRIX_DET_TOLERANCE) return 0;  // singular matrix; can't invert

   b -> v[0].n[0] = c0/det;
   b -> v[0].n[1] = (a -> v[0].n[2]*a -> v[2].n[1] - a -> v[0].n[1]*a -> v[2].n[2])/det;
   b -> v[0].n[2] = (a -> v[0].n[1]*a -> v[1].n[2] - a -> v[0].n[2]*a -> v[1].n[1])/det;
   b -> v[1].n[0] = c1/det;
   b -> v[1].n[1] = (a -> v[0].n[0]*a -> v[2].n[2] - a -> v[0].n[2]*a -> v[2].n[0])/det;
   b -> v[1].n[2] = (a -> v[0].n[2]*a -> v[1].n[0] - a -> v[0].n[0]*a -> v[1].n[2])/det;
   b -> v[2].n[0] = c2/det;
   b -> v[2].n[1] = (a -> v[0].n[1]*a -> v[2].n[0] - a -> v[0].n[0]*a -> v[2].n[1])/det;
   b -> v[2].n[2] = (a -> v[0].n[0]*a -> v[1].n[1] - a -> v[0].n[1]*a -> v[1].n[0])/det;

   return 1;
}
/* Axis of the matrix/array. No specific meaning at all. */
#define VX      0
#define VY      1
#define VZ      2
/** @brief Initiate a vector */
void oyVEC3init(oyVEC3* r, double x, double y, double z)
{
    r -> n[VX] = x;
    r -> n[VY] = y;
    r -> n[VZ] = z;
}
/** @brief Multiply two matrices */
void oyMAT3per(oyMAT3* r, const oyMAT3* a, const oyMAT3* b)
{
#define ROWCOL(i, j) \
    a->v[i].n[0]*b->v[0].n[j] + a->v[i].n[1]*b->v[1].n[j] + a->v[i].n[2]*b->v[2].n[j]

    oyVEC3init(&r-> v[0], ROWCOL(0,0), ROWCOL(0,1), ROWCOL(0,2));
    oyVEC3init(&r-> v[1], ROWCOL(1,0), ROWCOL(1,1), ROWCOL(1,2));
    oyVEC3init(&r-> v[2], ROWCOL(2,0), ROWCOL(2,1), ROWCOL(2,2));

#undef ROWCOL //(i, j)
}

/** @brief Evaluate a vector across a matrix */
void oyMAT3eval(oyVEC3* r, const oyMAT3* a, const oyVEC3* v)
{
    r->n[VX] = a->v[0].n[VX]*v->n[VX] + a->v[0].n[VY]*v->n[VY] + a->v[0].n[VZ]*v->n[VZ];
    r->n[VY] = a->v[1].n[VX]*v->n[VX] + a->v[1].n[VY]*v->n[VY] + a->v[1].n[VZ]*v->n[VZ];
    r->n[VZ] = a->v[2].n[VX]*v->n[VX] + a->v[2].n[VY]*v->n[VY] + a->v[2].n[VZ]*v->n[VZ];
}

/* end of lcms code */

/** convert a matrix to CIE * xy triple */
int oyMAT3toCIExyYTriple ( const oyMAT3* a, oyCIExyYTriple * triple )
{
  int i,j,
      fail=0;
  double sum;
    for(i = 0; i < 3; ++i)
    {
      for(j = 0; j < 3; ++j)
      {
        if(i < 3 && a->v[i].n[j] == 0)
          fail = 1;
      }
      sum = a->v[i].n[0]+a->v[i].n[1]+a->v[i].n[2];
      if(sum != 0)
      {
        triple->v[i].xy[0] = a->v[i].n[0]/sum;
        triple->v[i].xy[1] = a->v[i].n[1]/sum;
      } else
      {
        triple->v[i].xy[0] = 1;
        triple->v[i].xy[1] = 1;
      }
    }
  return fail;
}

const char * oyMAT3show ( const oyMAT3* a )
{
  static char * t = NULL;
  if(!t) t = (char*) malloc(1024);
  int i,j;
  t[0] = 0;
  for(i = 0; i < 3; ++i)
  {
    for(j = 0; j < 3; ++j)
      sprintf( &t[strlen(t)], " %g", a->v[i].n[j]);
    sprintf( &t[strlen(t)], "\n" );
  }
  return t;
}
const char * oyMat34show ( const float a[3][4] )
{
  static char * t = NULL;
  if(!t) t = (char*) malloc(1024);
  int i,j;
  t[0] = 0;
  for(i = 0; i < 3; ++i)
  {
    for(j = 0; j < 4; ++j)
      sprintf( &t[strlen(t)], " %g", a[i][j]);
    sprintf( &t[strlen(t)], "\n" );
  }
  return t;
}
const char * oyMat4show ( const float a[4] )
{
  static char * t = NULL;
  if(!t) t = (char*) malloc(1024);
  int i;
  t[0] = 0;
  for(i = 0; i < 4; ++i)
    sprintf( &t[strlen(t)], " %g", a[i]);
  sprintf( &t[strlen(t)], "\n" );
  return t;
}
const char * oyMat43show ( const float a[4][3] )
{
  static char * t = NULL;
  if(!t) t = (char*) malloc(1024);
  int i,j;
  t[0] = 0;
  for(i = 0; i < 4; ++i)
  {
    for(j = 0; j < 3; ++j)
      sprintf( &t[strlen(t)], " %g", a[i][j]);
    sprintf( &t[strlen(t)], "\n" );
  }
  return t;
}
const char * oyCIExyYTriple_Show( oyCIExyYTriple * triple )
{
  static char * t = NULL;
  if(!t) t = (char*) malloc(1024);
  int i;
  t[0] = 0;
  for(i = 0; i < 3; ++i)
  {
    sprintf( &t[strlen(t)], " x:%g y:%g", triple->v[i].xy[0],
                                          triple->v[i].xy[1]);
    sprintf( &t[strlen(t)], "\n" );
  }
  return t;
}


/** @} *//* color_low */


/** @} *//* color */



