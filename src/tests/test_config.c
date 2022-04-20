#include "oyranos_devices.h"

int main( int argc OY_UNUSED, char ** argv OY_UNUSED)
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

  oyStringListRelease( &texts, count, free );
  if(rank_list) free(rank_list);
  oyLibConfigRelease(0);

  return 0;
}

