/** qcmsevents.c
 *
 *  Qt based X11 color management event observer
 *
 *  License: newBSD
 *  Copyright: (c)2009-2016 - Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 */

#include <cstdio>

#define TARGET "X Color Management Events"
#include "qcmsevents.h"
#include "oyranos.h"
#include "oyranos_debug.h"
#include "oyranos_i18n.h"
#include "oyranos_io.h"
#include "oyProfile_s.h"
#include "oyObject_s.h"

#include <X11/Xcm/Xcm.h>

#include <QAction>
#include <QActionGroup>
#include <QVBoxLayout>
#include <QMenu>

QcmseDialog * dialog = NULL;

QcmseDialog::QcmseDialog()
{
  quitA = new QAction( tr("&Quit"), this );
  connect( quitA, SIGNAL(triggered()), this, SLOT(quit()) );
  showA = new QAction( tr("&Show Window"), this );
  connect( showA, SIGNAL(triggered()), this, SLOT(showNormal()) );
  showC = new QAction( tr("&Configure"), this );
  connect( showC, SIGNAL(triggered()), this, SLOT(showConfig()) );

  icon = NULL;
  systrayIconMenu = NULL;
  index_ = 0;
  init = 1;
  icons = new QComboBox;
  icons->addItem(QIcon(":/plugin-compicc_gray.png"), tr("none"));
  icons->addItem(QIcon(":/qcmsevents.png"), tr("active"));
  connect( icons, SIGNAL(currentIndexChanged(int)), this, SLOT(setIcon(int)));
 

  log_list = new QListWidget(this);

  QVBoxLayout * vertical_layout = new QVBoxLayout;
  vertical_layout->addWidget( log_list );
  setLayout( vertical_layout );

  setWindowTitle( tr(TARGET) );
  createIcon();

  resize(400, 250);
}

void QcmseDialog::createIcon()
{
  systrayIconMenu = new QMenu(this);

  QActionGroup * actions = new QActionGroup(this);
  const char ** names = NULL;
  int count = 0, i, current = -1;
  oyOptionChoicesGet( oyWIDGET_POLICY, &count, &names, &current );
  for(i = 0; i < count; ++i)
  {
    QAction * a = new QAction(QString(names[i]),this);
    systrayIconMenu->addAction( a );
    actions->addAction(a);
  }
  connect(actions,SIGNAL(triggered(QAction*)),this,SLOT(onAction(QAction*)));
  oyOptionChoicesFree( oyWIDGET_POLICY,&names, count );

  systrayIconMenu->addSeparator();

  systrayIconMenu->addAction( showA );
  systrayIconMenu->addAction( showC );
  systrayIconMenu->addAction( quitA );

  /* set a first icon */
  QIcon ic(":/plugin-compicc_gray.png");
  icon = new QSystemTrayIcon( this );
  icon->setIcon(ic);
  icon->setContextMenu( systrayIconMenu );
  icon->show();
  icons->setCurrentIndex(0);
}

void SendNativeUpdate(const char * func)
{
    // e.g. send native (e.g. X11) reload event
    oyOptions_s * opts = oyOptions_New(NULL), * results = 0;
    int error = oyOptions_Handle( "//" OY_TYPE_STD "/send_native_update_event",
                      opts,"send_native_update_event",
                      &results );
    oyOptions_Release( &opts );

    if(oy_debug)
      fprintf(stderr, "send_native_update_event %s()\n", func);
    if(error)
      fprintf(stderr, "send_native_update_event failed\n");
}

void QcmseDialog::onAction(QAction*a)
{
  oyPolicySet(a->text().toLocal8Bit().data(),NULL);
  setIcon(index_);
  SendNativeUpdate(__func__);
}

QString actualPolicy()
{
  QString policy;
  // clear the Oyranos settings cache
  oyGetPersistentStrings( NULL );

  const char ** names = NULL;
  int count = 0, current = -1;
  oyOptionChoicesGet( oyWIDGET_POLICY, &count, &names, &current );
  if(current >= 0)
    policy = QString(names[current]);
  oyOptionChoicesFree( oyWIDGET_POLICY, &names, count );
  return policy;
}

void QcmseDialog::setIcon(int index)
{
  index_ = index;
  QIcon ic = icons->itemIcon(index_);
  QPixmap pm = ic.pixmap(ic.availableSizes().last());
  QPainter p;
  QString t = actualPolicy();
  p.begin(&pm);
  QFont font = p.font();
  font.setPixelSize(pm.width()/4);
  p.setFont(font);

  const QRect rectangle = QRect(0, 0, pm.width(), pm.height());
  QRect boundingRect;
  p.drawText(rectangle, Qt::AlignCenter, t, &boundingRect);
  p.fillRect(boundingRect,QColor(255,255,255,185));
  if(boundingRect.width() >= pm.width())
    p.drawText(rectangle, Qt::AlignLeft|Qt::AlignVCenter, t);
  else
    p.drawText(rectangle, Qt::AlignCenter, t);

  p.end();

  QIcon ict = pm;
  icon->setIcon(ict);

  setWindowIcon(ic);

  icon->setToolTip( t );
}

void QcmseDialog::showConfig()
{
  const char * oyranos_settings_gui_app = getenv("OYRANOS_SETTINGS_GUI");
  char * app = NULL;
  const char * synnefo_bins[] = {"oyranos-config-synnefo",
                                 "oyranos-config-synnefo-qt4",
                                 "synnefo",
                                 "Synnefo",
                                 "oyranos-config-fltk",
                                 NULL};
  int i = 0;
#if 0
  const char * xdg_desktop = getenv("XDG_CURRENT_DESKTOP");
  if(xdg_desktop && strcmp(xdg_desktop,"KDE"))
    app = strdup("systemsettings5 settings-kolor-management");
#endif
  if(oyranos_settings_gui_app)
    app = strdup(oyranos_settings_gui_app);
  while(!app && synnefo_bins[i])
    app = oyFindApplication( synnefo_bins[i++] );

  if(app)
  { char * command = (char*) malloc(strlen(app) + 128);
    sprintf(command, "%s&", app);
    system(command);
    free(command);
    free(app); app = 0;
  } 
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
      setIcon( 0 );
    } else
    /*  base color server should support opt-out (ICR)
     *  through _ICC_COLOR_MANAGEMENT - ICM in _ICC_COLOR_DESKTOP
     */
    if(pid > 0 && strstr(text, "|ICM|") != 0)
    {
      color.setHsvF( 0.41, 0.5, 0.9 );
      setIcon( 1 );
    } else
    if(strstr(text, "_ICC_COLOR_DISPLAY_ADVANCED"))
      setIcon( index() );
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
      setIcon( 0 );
      setWindowIcon( icons->itemIcon(0) );
    } else
    /*  base color server should support opt-out (ICR)
     *  through _ICC_COLOR_MANAGEMENT - ICM in _ICC_COLOR_DESKTOP
     */
    if(pid > 0 && strstr(text, "|ICM|") != 0)
    {
      color.setHsvF( 0.41, 0.5, 0.9 );
      setIcon( 1 );
      setWindowIcon( icons->itemIcon(1) );
    }
  }
  item->setText( text );

  if(dialog->log_list->count() > 500)
    // expecting 11 head lines
    delete (dialog->log_list->takeItem(11));

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
  oyProfile_s * p = oyProfile_FromMD5( (uint32_t*)md5_hash, 0, 0 );
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

