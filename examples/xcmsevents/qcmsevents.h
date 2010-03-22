/** qcmsevents.h
 *
 *  A small X11 colour management event observer.
 *
 *  License: newBSD
 *  Copyright: (c)2009-2010 - Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 *  c++: ar cru liboyranosedid.a ../../modules/devices/oyranos_edid_parse.o ../../modules/devices/oyranos_monitor.o
 *  c++: prefix=/opt/local; g++ -Wall -g -o qcmsevents qcmsevents.cpp `PKG_CONFIG_PATH=$prefix/lib/pkgconfig pkg-config --cflags --libs x11 xmu xfixes xinerama xrandr xxf86vm oyranos QtGui` -DHAVE_X11 -DTARGET=\"xcmsevents\" -I$prefix/include -I../.. -L$prefix/lib -lXcolor -L./ -loyranosedid
 */

#ifndef QCMSEVENTS_H
#define QCMSEVENTS_H

#include <QtGui>
#include <QSystemTrayIcon>
#include <QDialog>
#include <QDesktopWidget>
#include <QWidget>
#include <QX11Info>

#include <cstdlib>
#include <cstring>

#include "xcmsevents_common.h"

class QcmseDialog : public QDialog
{
  Q_OBJECT

  public:
    QcmseDialog();

  protected:
    void close( QCloseEvent * e );

  public slots:
    void quit() { exit(0); }

  public:
    QSystemTrayIcon * icon;
  private:
    void createIcon();
    QMenu * iconMenu;
    QAction * quitA;
    QAction * showA;

    QListWidget * log_list;
  public:
    void log( const char * text, int code );
};

extern QcmseDialog * dialog;

class Qcmse : public QApplication
{
  xcmseContext_s * c;
  public:
    Qcmse(int argc, char ** argv) : QApplication(argc,argv)
    {
      c = xcmseContext_New( );
    };
    ~Qcmse()
    {
      xcmseContext_Stop( c );
    };
    void setup()
    {
      const char * display_name = getenv("DISPLAY");
      QDesktopWidget * d = this->desktop();
      QX11Info i = d->x11Info();
      c->display = i.display();
      c->w = dialog->winId();
      xcmseContext_Setup( c, display_name );
    };
    bool x11EventFilter( XEvent * event )
    {
      /* set the actual X11 Display, Qt seems to change the old pointer. */
      c->display = event->xany.display;
      /* process the X event */
      xcmseContext_InLoop( c, event );
      return false; 
    };
};


#endif /* QCMSEVENTS_H */

