/** qcmsevents.c
 *
 *  A small X11 colour management event observer.
 *
 *  License: newBSD
 *  Copyright: (c)2009-2010 - Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 *  c++: ar cru liboyranosedid.a ../../modules/devices/oyranos_edid_parse.o ../../modules/devices/oyranos_monitor.o
 *  c++: prefix=/opt/local; g++ -Wall -g -o qcmsevents qcmsevents.cpp `PKG_CONFIG_PATH=$prefix/lib/pkgconfig pkg-config --cflags --libs x11 xmu xfixes xinerama xrandr xxf86vm oyranos QtGui` -DHAVE_X11 -DTARGET=\"xcmsevents\" -I$prefix/include -I../.. -L$prefix/lib -lXcolor -L./ -loyranosedid
 */

#include <QtGui>
#include <QDesktopWidget>
#include <QWidget>
#include <QX11Info>

#include <stdlib.h>

#include "xcmsevents_common.h"
#include "xcmsevents_common.c"

class Qcmse : public QApplication
{
  xcmseContext_s * c;
  public:
    Qcmse(int argc, char ** argv) : QApplication(argc,argv)
    {
      const char * display_name = getenv("DISPLAY");
      c = xcmseContext_New( );
      /*QDesktopWidget * d = this->desktop();
      QX11Info i = d->x11Info();
      c->display = i.display();*/
      xcmseContext_Setup( c, display_name );
    };
    ~Qcmse()
    {
      xcmseContext_Stop( c );
    };
    bool x11EventFilter( XEvent * event )
    {
      /* set the actual X11 Display, Qt seems to change the old pointer. */
      c->display = event->xany.display;
      /* process the X event */
      return (bool)xcmseContext_InLoop( c, event );
    };
};

int main(int argc, char *argv[])
{
  Qcmse app(argc,argv);

  return app.exec();
}

