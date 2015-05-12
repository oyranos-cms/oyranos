/** qcmsevents.h
 *
 *  A small X11 color management event observer.
 *
 *  License: newBSD
 *  Copyright: (c)2009-2015 - Kai-Uwe Behrmann <ku.b@gmx.de>
 *
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



#if QT_VERSION >= 0x050000
class Qcmse : public QApplication, QAbstractNativeEventFilter
{
  XcmeContext_s * c;
  public:
    Qcmse(int & argc, char ** argv) : QApplication(argc,argv)
    {
      c = NULL;
      QCoreApplication::eventDispatcher()->installNativeEventFilter( this );
    };
    ~Qcmse()
    {
      XcmeContext_Release( &c );
    };
    void setup()
    {
      const char * display_name = getenv("DISPLAY");
      if(QX11Info::isPlatformX11())
        c = XcmeContext_Create( display_name );
    };
    // ask Qt for new arriving events
    virtual bool nativeEventFilter(const QByteArray &, void *, long *) Q_DECL_OVERRIDE
    {
      // use Xlib for Xcm's stand alone X11 context 
      while(XPending(XcmeContext_DisplayGet( c )))
      {
        XEvent event;
        XNextEvent( XcmeContext_DisplayGet( c ), &event);
        XcmeContext_InLoop( c, &event );
      }

      return false;
    };
};
#else /* QT_VERSION >= 0x050000 */
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
      QDesktopWidget * d = this->desktop();
      QX11Info i = d->x11Info();
      XcmeContext_DisplaySet( c, i.display() );
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
#endif /* QT_VERSION >= 0x050000 */

#endif /* QCMSEVENTS_H */

