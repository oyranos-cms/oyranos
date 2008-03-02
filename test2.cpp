#define DEBUG 1
#include "oyranos_debug.h"
#include "oyranos.h"
#include "oyranos_monitor.h"
#include <fstream>
#include <iostream>

void* myAllocFunc(size_t size) { return new char [size]; }

int main(int argc, char** argv)
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
    //return 0;
  }

  if(manufacturer)
    std::cout << manufacturer << "|" << strlen(manufacturer) << std::endl;
  if(model)
    std::cout << model << "|" << std::endl;
  if(serial)
    std::cout << serial << "|" << std::endl;
  if(manufacturer) delete [] manufacturer;
  if(model) delete [] model;
  if(serial) delete [] serial;


  // now an more simple approach
  char* profil_name =
    oyranos::oyGetMonitorProfileName (display_name, myAllocFunc);
  if(profil_name)
    std::cout << profil_name << std::endl;
  else
    std::cout << "no profile found for your monitor" << std::endl;
  if(profil_name) delete [] profil_name;

  // standard profiles
  std::cout << "Default Profiles:\n";
  for(int i = (int)oyranos::oyDEFAULT_PROFILE_START + 1;
        i < (int)oyranos::oyDEFAULT_PROFILE_END ; ++i)
  {
    std::cout <<"  "<< oyGetOptionUITitle( (oyranos::oyOPTION)i,0,0,0,0 )
              <<": ";
    char *default_name = oyranos::oyGetDefaultProfileName( 
                           (oyranos::oyDEFAULT_PROFILE)i, myAllocFunc );
    if(default_name)
      std::cout << default_name;
    std::cout << "\n";
  }

  oy_debug = 0;
  char *data = 0;


  if(argc > 1)
  {
    printf("%s\n", argv[1]);
    std::ifstream f( argv[1], std::ios::binary | std::ios::ate);
    if(f.good())
    {
      size_t size = f.tellg();
      f.seekg(0);
      if(size) {
        data = (char*) new char [size+1];
        f.read ((char*)data, size);
        f.close();
        std::cout << "Opened file: " << argv[1] << std::endl;
      }
    }
  }

  char *xml = data;
  if( !xml)
    xml = oyranos::oyPolicyToXML( oyranos::oyGROUP_ALL, 1, myAllocFunc );

  if(xml) {
    oyranos::oyReadXMLPolicy(oyranos::oyGROUP_ALL, xml);
    printf("xml text: \n%s", xml);
    delete [] xml;
  }

  return 0;
}
