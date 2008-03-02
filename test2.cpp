#define DEBUG 1
#include "oyranos_debug.h"
#include "oyranos.h"
#include "oyranos_monitor.h"
#include <fstream>
#include <iostream>

void* myAllocFunc(size_t size) { return new char [size]; }

int main(void)
{
  char       *manufacturer=0,
             *model=0,
             *serial=0;
  const char *display_name=0;

  oy_debug = 0;


  int erg =
    oyranos::oyGetMonitorInfo ( display_name, &manufacturer, &model, &serial,
                                myAllocFunc );

  if(erg) {
    std::cout << "error while retrieving monitor profile!!\n";
    return 0;
  }

  if(manufacturer)
    std::cout << manufacturer << "|" << strlen(manufacturer) << std::endl;
  if(model)
    std::cout << model << "|" << std::endl;
  if(serial)
    std::cout << serial << "|" << std::endl;
  if(manufacturer) free(manufacturer);
  if(model) free(model);
  if(serial) free(serial);


  // now an more simple approach
  char* profil_name =
    oyranos::oyGetMonitorProfileName (display_name, myAllocFunc);
  if(profil_name)
    std::cout << profil_name << std::endl;
  else
    std::cout << "no profile found for your monitor" << std::endl;
  if(profil_name) free(profil_name);

  // standard profiles
  std::cout << "Default Profiles:\n";
  for(int i = 0; i < (int)oyranos::oyDEFAULT_PROFILE_NUMS ; ++i) {
  std::cout <<"  "<< oyGetDefaultProfileUITitle( (oyranos::oyDEFAULT_PROFILE)i )
         << ": ";
    char *default_name = oyranos::oyGetDefaultProfileName( (oyranos::oyDEFAULT_PROFILE)i, myAllocFunc );
    if(default_name)
      std::cout << default_name;
    std::cout << "\n";
  }
  return 0;
}
