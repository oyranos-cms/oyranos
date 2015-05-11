/** qcmsevents.h
 *
 *  A small X11 color management event observer.
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
#include <QApplication>
#include <QSystemTrayIcon>
#include <QDialog>
#include <QDesktopWidget>
#include <QWidget>
#include <QListWidget>
#include <QComboBox>
#include <QX11Info>

#include <cstdlib>
#include <cstring>

#include <X11/Xcm/XcmEvents.h>
#include <X11/Xcm/Xcm.h>

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
    void createIcon();
  private:
    QMenu * iconMenu;
    QAction * quitA;
    QAction * showA;

    QListWidget * log_list;
    QComboBox * icons;
  public:
    void log( const char * text, int code );
    int init;
};

extern QcmseDialog * dialog;

class Qcmse : public QApplication
{
  XcmeContext_s * c;
  public:
    Qcmse(int & argc, char ** argv) : QApplication(argc,argv)
    {
      c = XcmeContext_New( );
    };
    ~Qcmse()
    {
      XcmeContext_Release( &c );
    };
    void setup()
    {
      const char * display_name = getenv("DISPLAY");
#if QT_VERSION < 0x050000
      QDesktopWidget * d = this->desktop();
      QX11Info i = d->x11Info();
      XcmeContext_DisplaySet( c, i.display() );
#else
      if(QX11Info::isPlatformX11())
        XcmeContext_DisplaySet( c, QX11Info::display() );
#endif
      XcmeContext_WindowSet( c, dialog->winId() );
      XcmeContext_Setup( c, display_name );
    };
    bool x11EventFilter( XEvent * event )
    {
      /* set the actual X11 Display, Qt seems to change the old pointer. */
      XcmeContext_DisplaySet( c, event->xany.display );
      /* process the X event */
      XcmeContext_InLoop( c, event );
      return false; 
    };
};


#endif /* QCMSEVENTS_H */

