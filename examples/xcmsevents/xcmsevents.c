/** xcmsevents.c
 *
 *  A small X11 colour management event observer.
 *
 *  License: newBSD
 *  Copyright: (c)2009-2010 - Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 *  c++: ar cru liboyranosedid.a ../../modules/devices/oyranos_edid_parse.o ../../modules/devices/oyranos_monitor.o
 *  c++: prefix=/opt/local; g++ -Wall -g -o qcmsevents xcmsevents.c `PKG_CONFIG_PATH=$prefix/lib/pkgconfig pkg-config --cflags --libs x11 xmu xfixes xinerama xrandr xxf86vm oyranos` -DHAVE_X11 -DTARGET=\"xcmsevents\" -I$prefix/include -I../.. -L$prefix/lib -lXcolor -L./ -loyranosedid
 */

#include <stdlib.h>
#include "xcmsevents_common.h"
#include "xcmsevents_common.c"

int main(int argc, char *argv[])
{
  const char * display_name = getenv("DISPLAY");

  xcmseContext_s * c = xcmseContext_Create( display_name );

  for(;;)
  {
    XEvent event;
    XNextEvent( c->display, &event);
    xcmseContext_InLoop( c, &event );
  }

  xcmseContext_Stop( c );

  return 0;
}

