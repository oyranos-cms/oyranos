// !cc -Wall -Wextra -g -fopenmp -I /usr/include testIlluminant.c -o testIlluminant -llcms2 -lm
#include <lcms2.h>
#include "lcm2_profiler.h"
#include "lcm2_profiler.c"
int main(int argc, char**argv)
{
  cmsCIEXYZ Result;
  const cmsCIExyY D65 = { 0.31271, 0.32902, 1.0 }; /* D65 */;
  cmsCIEXYZ SourceWhitePt;
  cmsxyY2XYZ(&SourceWhitePt, &D65);
  const cmsCIEXYZ * Illuminant = cmsD50_XYZ();
  cmsCIEXYZ Value = {0,1,0};
  cmsBool result = cmsAdaptToIlluminant(&Result,
                                        &SourceWhitePt,
                                        Illuminant,
                                        &Value );
  lcm2MAT3 D65_D50, r;
  printf(" D65 %f, %f, %f\n", SourceWhitePt.X, SourceWhitePt.Y, SourceWhitePt.Z);
  printf(" D50 %f, %f, %f\n", Illuminant->X, Illuminant->Y, Illuminant->Z);
  result = lcm2AdaptationMatrix( &D65_D50,
                                 NULL,
                                 &SourceWhitePt, Illuminant );
  lcm2MAT3 grayer = {{
    {{ 0.0000000,  0.9642957,  0.0000000}},
    {{ 0.0000000,  1.0000000,  0.0000000}},
    {{ 0.0000000,  0.8251046,  0.0000000}}
  }};
  lcm2MAT3per(&r, &D65_D50, &grayer);
  puts(" D65 -> D50 matrix:");
  puts(lcm2MAT3show(&r));

  lcm2MAT3 a = {{ {{0.6,0,0}}, {{0,1,0}}, {{0,0,0.6}} }},
           b = {{ {{1,1,1}}, {{1,1,1}}, {{1,1,1}} }};
  lcm2MAT3per(&r, &a, &b);
  puts(" scale XZ * 0.6:");
  puts(lcm2MAT3show(&r));

  return 0;
}

