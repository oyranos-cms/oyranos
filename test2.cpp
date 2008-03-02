#include "oyranos_monitor.h"

#include "oyranos_monitor.c"

int main(void)
{
  char       *manufacturer=0,
             *model=0,
             *serial=0;
  const char *x=0;

  oy_debug = 1;
  char* profil_name =
  oyGetMonitorProfile            (x, manufacturer, model, serial );


  return 0;
}
