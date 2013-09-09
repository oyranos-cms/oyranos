/** qcmsevents.c
 *
 *  Qt based X11 color management event observer
 *
 *  License: newBSD
 *  Copyright: (c)2009-2010 - Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 */

#include <cstdio>

#define TARGET "X Color Management Events"
#include "qcmsevents.h"
#include "oyranos_alpha.h"
#include "oyranos_i18n.h"

#include <X11/Xcm/Xcm.h>

QcmseDialog * dialog = 0;

QcmseDialog::QcmseDialog()
{
  quitA = new QAction( tr("&Quit"), this );
  connect( quitA, SIGNAL(triggered()), this, SLOT(quit()) );
  showA = new QAction( tr("&Show Window"), this );
  connect( showA, SIGNAL(triggered()), this, SLOT(showNormal()) );

  icon = 0;
  init = 1;
  icons = new QComboBox;
  icons->addItem(QIcon(":/plugin-compicc_gray.svg"), tr("none"));
  icons->addItem(QIcon(":/qcmsevents.svg"), tr("active"));
 

  log_list = new QListWidget(this);

  QVBoxLayout * vertical_layout = new QVBoxLayout;
  vertical_layout->addWidget( log_list );
  setLayout( vertical_layout );

  setWindowTitle( tr(TARGET) );

  resize(400, 250);
}

void QcmseDialog::createIcon()
{
  iconMenu = new QMenu(this);
  iconMenu->addAction( showA );
  iconMenu->addAction( quitA );

  /* set a first icon */
  QIcon ic(":/plugin-compicc_gray.svg");
  icon = new QSystemTrayIcon( ic, this );
  icon->setContextMenu( iconMenu );
  icon->show();
}

void QcmseDialog::log( const char * text, int code )
{
  int pid = -1;

  if(!dialog->icon)
    dialog->createIcon();
  dialog->icon->setToolTip( text );

  QListWidgetItem * item = new QListWidgetItem;
  QColor color;
  if(code == XCME_MSG_DISPLAY_EVENT)
  {
    static int zebra = 0;
    if(zebra)
    {
      zebra = 0;
      color.setHsvF( 0.17, 0.05, 0.9 );
    }
    else
    {
      zebra = 1;
      color.setHsvF( 0.17, 0.05, 0.75 );
    }

    if(strstr(text, "PropertyNotify : "))
      text = strstr(text, "PropertyNotify : ") + strlen("PropertyNotify : ");
    if(strstr(text, XCM_COLOR_DESKTOP" "))
      sscanf( text, XCM_COLOR_DESKTOP" %d", &pid );
    if(pid == 0)
    {
      color.setHsvF( 0.6, 0.4, 0.9 );
      icon->setIcon( icons->itemIcon(0) );
    } else
    /*  base color server should support opt-out (ICR)
     *  through _ICC_COLOR_MANAGEMENT - ICM in _ICC_COLOR_DESKTOP
     */
    if(pid > 0 && strstr(text, "|ICM|") != 0)
    {
      color.setHsvF( 0.41, 0.5, 0.9 );
      icon->setIcon( icons->itemIcon(1) );
    }
    item->setBackground( QBrush( color ) );
  }
  else if(code == XCME_MSG_DISPLAY_ERROR)
  {
    static int zebra = 0;
    if(zebra)
    {
      zebra = 0;
      color.setHsvF( 0.0, 0.7, 0.9 );
    }
    else
    {
      zebra = 1;
      color.setHsvF( 0.0, 0.7, 0.75 );
    }

    if(strstr(text, "PropertyNotify : "))
      text = strstr(text, "PropertyNotify : ") + strlen("PropertyNotify : ");
    item->setBackground( QBrush( color ) );
  }
  else if (XCME_MSG_DISPLAY_STATUS)
  {
    int i;
    if(strstr(text, "atom: \"" XCM_COLOR_DESKTOP "\":"))
      sscanf( text, "atom: \"" XCM_COLOR_DESKTOP "\": %d %d", &i, &pid );
    if(pid == 0)
    {
      color.setHsvF( 0.6, 0.4, 0.9 );
      icon->setIcon( icons->itemIcon(0) );
    } else
    /*  base color server should support opt-out (ICR)
     *  through _ICC_COLOR_MANAGEMENT - ICM in _ICC_COLOR_DESKTOP
     */
    if(pid > 0 && strstr(text, "|ICM|") != 0)
    {
      color.setHsvF( 0.41, 0.5, 0.9 );
      icon->setIcon( icons->itemIcon(1) );
    }
  }
  item->setText( text );

  dialog->log_list->insertItem( dialog->log_list->count(), item );
  if(!dialog->init)
    dialog->log_list->scrollToItem( item );
}

int QcmseMessageFunc( XCME_MSG_e code, const void * context, const char * format, ... )
{
  char* text = 0, *pos = 0;
  va_list list;
  int id = -1;
  const oyStruct_s * s = (oyStruct_s*) context;

  if(code == (XCME_MSG_e)oyMSG_DBG && !oy_debug)
    return 0;


  if(s && oyOBJECT_NONE < s->type_)
  {
    id = oyObject_GetId( s->oy_ );
  }

  text = (char*)calloc(sizeof(char), 4096);
  text[0] = 0;


  switch((int)code)
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

  if(id >= 0)
    sprintf( &text[strlen(text)], " [%d]", id);

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

extern "C" {
void * fromMD5                       ( const void        * md5_hash,
                                       size_t            * size,
                                       void              *(allocate_func)(size_t) )
{
  void * data = 0;
  oyProfile_s * p = oyProfile_FromMD5( (uint32_t*)md5_hash, 0 );
  data = oyProfile_GetMem( p, size, 0, allocate_func );
  oyProfile_Release( &p );
  return data;
}

char * getName                       ( const void        * data,
                                       size_t              size,
                                       void              *(allocate_func)(size_t),
                                       int                 file_name )
{
  char * text = 0;
  const char * t = 0;
  oyProfile_s * p = oyProfile_FromMem( size, (void*)data, 0, 0 );
  if(file_name)
    t = oyProfile_GetFileName( p, -1 );
  else
    t = oyProfile_GetText( p, oyNAME_DESCRIPTION );

  if(t && t[0])
  {
    text = (char*)allocate_func( strlen(t) + 1 );
    strcpy( text, t );
  }

  oyProfile_Release( &p );
  return text;
}
}

int main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(qcmsevents);

  Qcmse app(argc,argv);

  if( QSystemTrayIcon::isSystemTrayAvailable() )
    QApplication::setQuitOnLastWindowClosed( false );

  XcmMessageFuncSet( QcmseMessageFunc );

  XcmICCprofileFromMD5FuncSet( fromMD5 );
  XcmICCprofileGetNameFuncSet( getName );

  dialog = new QcmseDialog();
  if(argc > 1 && strcmp(argv[1],"--show") == 0)
    dialog->show();

  app.setup();

  if(!dialog->icon)
    dialog->createIcon();
  dialog->init = 0;

  return app.exec();
}

