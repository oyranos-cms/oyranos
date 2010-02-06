/** xcmsevents_common.h
 *
 *  A small X11 colour management event observer.
 *
 *  License: newBSD
 *  Copyright: 2009-2010 - Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 *  c++: ar cru liboyranosedid.a ../../modules/devices/oyranos_edid_parse.o ../../modules/devices/oyranos_monitor.o
 *  c++: prefix=/opt/local; g++ -Wall -g -o qcmsevents xcmsevents.c `PKG_CONFIG_PATH=$prefix/lib/pkgconfig pkg-config --cflags --libs x11 xmu xfixes xinerama xrandr xxf86vm oyranos` -DHAVE_X11 -I$prefix/include -I../.. -L$prefix/lib -lXcolor -L./ -loyranosedid
 */

#ifndef XCMSEVENTS_COMMON_H
#define XCMSEVENTS_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#ifdef __cplusplus
}
#endif


char *   printWindowName             ( Display           * display,
                                       Window              w );
void     printWindowRegions          ( Display           * display,
                                       Window              w,
                                       int                 always );

typedef struct {
  Display * display;
  int screen;
  Window root;
  int nWindows;
  Window * Windows;
  Window w;
  pid_t old_pid;
  Atom aProfile, aTarget, aCM, aRegion, aDesktop;
} xcmseContext_s;
xcmseContext_s *
         xcmseContext_New            ( const char        * display_name );
int      xcmseStop                   ( xcmseContext_s * c );
int      xcmseInLoop                 ( xcmseContext_s * c );



#endif /* XCMSEVENTS_COMMON_H */

