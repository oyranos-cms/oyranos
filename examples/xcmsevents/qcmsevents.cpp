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

#include <cstdio>

#define TARGET "X Color Management Events"
#include "qcmsevents.h"

#include "xcmsevents_common.c"

QcmseDialog * dialog = 0;

QcmseDialog::QcmseDialog()
{
  quitA = new QAction( tr("&Quit"), this );
  connect( quitA, SIGNAL(triggered()), this, SLOT(quit()) );
  showA = new QAction( tr("&Show Window"), this );
  connect( showA, SIGNAL(triggered()), this, SLOT(showNormal()) );

  createIcon();

  log_list = new QListWidget(this);

  QVBoxLayout * vertical_layout = new QVBoxLayout;
  vertical_layout->addWidget( log_list );
  setLayout( vertical_layout );

  icon->show();
  setWindowTitle( tr(TARGET) );

  resize(400, 250);
}

void QcmseDialog::createIcon()
{
  iconMenu = new QMenu(this);
  iconMenu->addAction( showA );
  iconMenu->addAction( quitA );

  icon = new QSystemTrayIcon( this );
  icon->setContextMenu( iconMenu );
}

void QcmseDialog::log( const char * text, int code )
{
  int pid = -1;

  dialog->icon->setToolTip( text );

  QListWidgetItem * item = new QListWidgetItem;
  QColor colour;
  if(code == oyMSG_DISPLAY_EVENT)
  {
    static int zebra = 0;
    if(zebra)
    {
      zebra = 0;
      colour.setHsvF( 0.17, 0.05, 0.9 );
    }
    else
    {
      zebra = 1;
      colour.setHsvF( 0.17, 0.05, 0.75 );
    }

    if(strstr(text, "PropertyNotify : "))
      text = strstr(text, "PropertyNotify : ") + strlen("PropertyNotify : ");
    if(strstr(text, "_NET_COLOR_DESKTOP "))
      sscanf( text, "_NET_COLOR_DESKTOP %d", &pid );
    if(pid == 0)
    {
      colour.setHsvF( 0.6, 0.4, 0.9 );
      QIcon ic(":/plugin-colour_desktop_gray.svg");
      icon->setIcon( ic );
    } else
    if(pid > 0)
    {
      colour.setHsvF( 0.41, 0.5, 0.9 );
      QIcon ic(":/colour_desktop/plugin-colour_desktop.svg");
      icon->setIcon( ic );
    }
    item->setBackground( QBrush( colour ) );
  }
  else if (oyMSG_DISPLAY_STATUS)
  {
    int i;
    if(strstr(text, "atom: \"_NET_COLOR_DESKTOP\":"))
      sscanf( text, "atom: \"_NET_COLOR_DESKTOP\": %d %d", &i, &pid );
    if(pid == 0)
    {
      colour.setHsvF( 0.6, 0.4, 0.9 );
      QIcon ic(":/plugin-colour_desktop_gray.svg");
      icon->setIcon( ic );
    } else
    if(pid > 0)
    {
      colour.setHsvF( 0.41, 0.5, 0.9 );
      QIcon ic(":/colour_desktop/plugin-colour_desktop.svg");
      icon->setIcon( ic );
    }
  }
  item->setText( text );

  dialog->log_list->insertItem( dialog->log_list->count(), item );
  dialog->log_list->scrollToItem( item );
}

int QcmseMessageFunc( int code, const oyStruct_s * context, const char * format, ... )
{
  char* text = 0, *pos = 0;
  va_list list;
  const char * type_name = "";
  int id = -1;

  if(code == oyMSG_DBG && !oy_debug)
    return 0;


  if(context && oyOBJECT_NONE < context->type_)
  {
    type_name = oyStructTypeToText( context->type_ );
    id = oyObject_GetId( context->oy_ );
  }

  text = (char*)calloc(sizeof(char), 4096);
  text[0] = 0;


  switch(code)
  {
    case oyMSG_WARN:
         sprintf( &text[strlen(text)], _("WARNING"));
         break;
    case oyMSG_ERROR:
         sprintf( &text[strlen(text)], _("!!! ERROR"));
         break;
    case oyMSG_DBG:
         break;
  }

  /*snprintf( &text[strlen(text)], 4096 - strlen(text), " %03f %s[%d] ", 
                                                     DBG_UHR_, type_name,id );*/

  va_start( list, format);
  vsnprintf( &text[strlen(text)], 4096 - strlen(text), format, list);
  va_end  ( list );


  pos = &text[strlen(text)];
  *pos = 0;

  dialog->log( text, code );
  /* for debugging it is better to see messages on the console rather than
     getting lost during a crash */
#ifdef DEBUG
  printf( "%d %s\n", code, text );
#endif

  if(text) free( text );

  return 0;
}

int main(int argc, char *argv[])
{
  Qcmse app(argc,argv);

  if( QSystemTrayIcon::isSystemTrayAvailable() )
    QApplication::setQuitOnLastWindowClosed( false );

  dialog = new QcmseDialog();
  //dialog->show();

  oyMessageFuncSet( QcmseMessageFunc );

  app.setup();
  return app.exec();
}

