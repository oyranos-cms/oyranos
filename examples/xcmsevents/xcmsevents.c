/** xcmsevents.c
 *
 *  A small X11 colour management event observer.
 *
 *  License: newBSD
 *  Copyright: 2009 - Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 *  c++: ar cru liboyranosedid.a ../../modules/devices/oyranos_edid_parse.o ../../modules/devices/oyranos_monitor.o
 *  c++: prefix=/opt/local; g++ -Wall -g -o qcmsevents xcmsevents.c `PKG_CONFIG_PATH=$prefix/lib/pkgconfig pkg-config --cflags --libs x11 xmu xfixes xinerama xrandr xxf86vm oyranos` -DHAVE_X11 -I$prefix/include -I../.. -L$prefix/lib -lXcolor -L./ -loyranosedid
 */

#include <stdlib.h>
#include "xcmsevents_common.h"
#include "xcmsevents_common.c"

int main(int argc, char *argv[])
{
  const char * display_name = getenv("DISPLAY");

  xcmseContext_s * c = xcmseContext_New( display_name );
  for(;;)
    xcmseInLoop( c );
  xcmseStop( c );

  return 0;
}

