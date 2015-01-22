#include "oyranos_devices.h"

int main( int argc, char ** argv)
{
  uint32_t count = 0,
         * rank_list = 0;
  int error = 0;
  char ** texts = 0;

#ifdef USE_GETTEXT
  setlocale(LC_ALL,"");
#endif

  /* get all configuration filters */
  error = oyConfigDomainList( "//"OY_TYPE_STD"/device/config.icc_profile",
                      &texts, &count, &rank_list ,0 );
  

  if(!count && !error)
    oyMessageFunc_p(oyMSG_WARN,0,"No registration found by oyConfigDomainList");
  else
    oyMessageFunc_p(oyMSG_WARN,0,"found %d registrations", count);

  return 0;
}

