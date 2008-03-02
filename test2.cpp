#define DEBUG 1
#include "oyranos_debug.h"
#include "oyranos_monitor.h"
#include <fstream>
#include <iostream>

int main(void)
{
  char       *manufacturer=0,
             *model=0,
             *serial=0;
  const char *display_name=0;

  oy_debug = 1;

  int erg =
    oyranos::oyGetMonitorInfo ( display_name, &manufacturer, &model, &serial );

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
    oyranos::oyGetMonitorProfileName (display_name);
  if(profil_name)
    std::cout << profil_name << std::endl;
  else
    std::cout << "no profile found for Your monitor" << std::endl;
  if(profil_name) free(profil_name);

  return 0;
}
