#include "oyConversion_s.h"

int main( int argc, char ** argv)
{
  oyConversion_s * cc = oyConversion_New(0);

  if(!cc)
    oyMessageFunc_p(oyMSG_WARN,0,"No oyConversion_s generated");

  oyConversion_Release(&cc);

  return 0;
}

