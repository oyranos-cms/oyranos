#include "oyCMMapi6_s.h"

int main( int argc, char ** argv)
{
  oyCMMapi6_s * api = oyCMMapi6_New(0);
  const char * in,*out;

  if(!api)
    oyMessageFunc_p(oyMSG_WARN,0,"No oyCMMapi_s generated");

  in = oyCMMapi6_GetDataType(api,0);
  out = oyCMMapi6_GetDataType(api,1);

  oyCMMapi6_Release(&api);

  return 0;
}

