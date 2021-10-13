/** @file lib.cpp
 *
 *  Oyjl JSON QML is a graphical renderer of UI files.
 *
 *  @par Copyright:
 *            2018-2021 (C) Kai-Uwe Behrmann
 *            All Rights reserved.
 *
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2019/04/18
 *
 *  basic QML handling and environment setup for the -R=qml option
 */

#include <QApplication>
#include <QIcon>
#include <QMessageBox>
#include <QQmlApplicationEngine>
#include <QtQml> // qmlRegisterType<>()

#include "include/app_data.h"
#include "include/app_manager.h"
#include "include/process.h"

#include <QTranslator>
#include <QUrl>
#include <QObject>
#include <QSysInfo>
#include <QtGui/QScreen>
#ifdef QT_DBUS_LIB
#include <QtDBus>
#endif

#include <stdlib.h> // free()
#include <oyjl.h>

#if defined(Q_OS_ANDROID)
#include <android/log.h>
#endif

// startup stuff
AppManager mgr;
AppManager * m = &mgr;
int app_init = 0;
int app_debug = 0;
extern int (*processCallback_p)(int argc, const char ** argv);

void printObjectClassNames( QObject * o )
{
  const QObjectList list = o->children();
  for(int i= 0; i < list.count(); ++i)
  {
    LOG(QString("QML object[") + QString::number(i) + QString("] ") + list[i]->objectName()
        + " " + list[i]->metaObject()->className() );
    printObjectClassNames( list[i] );
  }
}

oyjlUi_s          * qml_lib_ui = NULL;
int oyjlArgsQmlStart__               ( int                 argc,
                                       const char       ** argv,
                                       const char        * json,
                                       const char        * commands,
                                       const char        * output,
                                       int                 debug,
                                       oyjlUi_s          * ui,
                                       int               (*callback)(int argc, const char ** argv))
{
    if(app_init)
    {
      qml_lib_ui = ui;
      return 0;
    }

    setenv("FORCE_COLORTERM", "1", 0); /* show rich text format on non GNU color extension environment */

    Q_INIT_RESOURCE(app);

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, (char**)argv);
    // a = &app; see comment above

    QTranslator translator;
    QString lname( ":/translations/app_" + QLocale::system().name() );
    if(!translator.load( lname ))
        LOG( QString("failed loading locale: ") + lname );
    else
        app.installTranslator(&translator);

#ifdef __ANDROID__
    QLocale loc;
    QString lang = loc.name();
    lang = QLocale::system().name();
    oyjlLang( lang.toLocal8Bit().data() );
#endif

    foreach (QScreen * screen, QGuiApplication::screens())
        screen->setOrientationUpdateMask(Qt::LandscapeOrientation | Qt::PortraitOrientation |
                                         Qt::InvertedLandscapeOrientation | Qt::InvertedPortraitOrientation);

    app.setApplicationName(QString("oyjl-args-qml"));
    app.setApplicationDisplayName(QString("Oyjl"));
    app.setApplicationVersion("0.7");
    app.setOrganizationName(QString("oyranos.org"));
    app.setWindowIcon(QIcon(":/images/logo"));

    app_debug = debug;
    if(app_debug)
    {
        LOG( QString("app args[") + QString::number(argc) + "]:" );
        for(int i = 0; i < argc; ++i)
            LOG( QString(argv[i]) );
    }


    qmlRegisterType<AppData>("AppData", 1, 0, "AppData");
    qmlRegisterType<Process>("Process", 1, 0, "Process");

    QQmlApplicationEngine engine;
    QQmlContext *ctxt = engine.rootContext();
    ctxt->setContextProperty("ApplicationVersion", QVariant::fromValue( app.applicationVersion() ));
    ctxt->setContextProperty("SysProductInfo", QVariant::fromValue( QSysInfo::prettyProductName() ));
    ctxt->setContextProperty("QtRuntimeVersion", QVariant::fromValue( QString(qVersion()) ));
    ctxt->setContextProperty("QtCompileVersion", QVariant::fromValue( QString(QT_VERSION_STR) ));

    engine.load(QUrl(QStringLiteral("qrc:qml/main.qml")));

    // extract the app data from QML
      // get all objects from QML
    QList<QObject*> qmlObjects = engine.rootObjects();
    if(!qmlObjects.count())
      LOG( QString("no QML objects") );
    else
    {
      QObject * o = qmlObjects[0];
      QObject::connect( &mgr, SIGNAL(fullLogMessage(QVariant)), o, SIGNAL(fullLogMessage(QVariant)) );

      QObject::connect( &mgr, SIGNAL(batteryInfo(QVariant)), o, SIGNAL(batteryInfo(QVariant)) );
      if( mgr.getBatteryInfo().length() )
          mgr.setBatteryInfo( mgr.getBatteryInfo() ); // update QML

      QObject::connect( &mgr, SIGNAL(uriChanged(QVariant)), o, SIGNAL(fileChanged(QVariant)) );
      QObject::connect( &mgr, SIGNAL(outputChanged(QVariant)), o, SIGNAL(outputChanged(QVariant)) );
      QObject::connect( &mgr, SIGNAL(commandsChanged(QVariant)), o, SIGNAL(commandsChanged(QVariant)) );
      QObject::connect( &mgr, SIGNAL(debugChanged(QVariant)), o, SIGNAL(debugChanged(QVariant)) );

#if defined(QT_DBUS_LIB) && defined(OPENICC_LIB)
      if( QDBusConnection::sessionBus().connect( QString(), "/org/libelektra/configuration", "org.libelektra", QString(),
                                                 m, SLOT( setNightInfo( QDBusMessage ) )) )
      {
          QObject::connect( &mgr, SIGNAL(nightInfo(QVariant)), o, SIGNAL(nightInfo(QVariant)) );
          LOG( QString("connect to /org/libelektra/configuration") );
      }
      emit mgr.nightInfo( mgr.isNight() ); // update QML
#endif

      app_init = 1;
      if(app_debug)
      {
          LOG( QString("qml root objects: ") + QString::number( qmlObjects.count() ) );
          mgr.setDebug( app_debug );
          LOG( QString("setDebug: ") + QString::number( app_debug ) );
      }
    }

    processCallback_p = callback;
    app_init = 1;

#ifdef __ANDROID__
    if(oyjlLang("") != NULL && strlen(oyjlLang("")) > 0)
      callback(argc, argv);
#endif

    if(qml_lib_ui)
      ui = qml_lib_ui;

    oyjl_val root = NULL;
    char error_buffer[256] = {0};
    int r = 0;
    if( json && strlen( json ) )
    {
      r = oyjlIsFile( json, "r", NULL, 0 );
      if(!r && oyjlDataFormat(json) == 7)
      {
        root = oyjlTreeParse( json, error_buffer, 256 );
        if(error_buffer[0] != '\000')
        {
          fprintf(stderr, "ERROR:\t\"%s\"\n", error_buffer);
          char * error = NULL;
          oyjlStringAdd( &error, 0,0, "{\"error\": \"%s\"}", json );
          json = error;
          r = -1;
        }
      }
    }

    if( (root && oyjlTreeGetValue(root, 0, "org/freedesktop/oyjl/modules")) || // use UI JSON
        (!root && json && strlen(json)) ) // assume JSON filename
    {
      if(!root && json && strlen(json))
      {
        if(r)
          LOG( QString("Found file name: ") + json );
        else
          LOG( QString("Assume file name or stream: ") + json );
      } else
        LOG( QString("Found Json org/freedesktop/oyjl/modules: ") + QString::number(strlen(json)) );
      mgr.setUri( QString(json) );
    }
    else
    {
      json = oyjlUi_ToJson( ui, 0 ); // generate JSON from ui data struct
      char * merged = NULL;
      LOG( QString("oyjlUi_ToJson(): ") + QString::number(json?strlen(json):0) );
      if(root && json)
      {
        oyjl_val module = oyjlTreeParse( json, error_buffer, 256 );
        oyjl_val rv = oyjlTreeGetValue(root, 0, "org/freedesktop/oyjl/translations");
        oyjl_val mv = oyjlTreeGetValue(module, OYJL_CREATE_NEW, "org/freedesktop/oyjl/translations");
        if(rv && mv) // merge in translations
        {
          size_t size = sizeof(*rv);
          memcpy( mv, rv, size );
          memset( rv, 0, size );
          int level = 0;
          oyjlTreeToJson( module, &level, &merged );
          LOG( QString("merge UI JSON with translation") );
        } else
          LOG( QString("expected translation is missing") );
        oyjlTreeFree( module );
      }
      if(!merged && json)
      {
        merged = oyjlStringCopy( json, NULL );
        LOG( QString("use generated UI JSON") );
      }
      oyjlTreeFree( root );
      mgr.setUri( QString(merged) );
      if(merged)
        free(merged);
    }

    if( commands )
      mgr.setCommands( commands );
    else
    {
      char * t = NULL; oyjlStringAdd( &t, 0,0, "{\"command_set\": \"%s\"}", ui->nick );
      mgr.setCommands(t);
      free(t);
    }

    if( output )
      mgr.setOutput( output );

    int result = app.exec();
    return result;
}

extern "C" { // "C" API wrapper
// internal API entry for public API in libOyjlCore
int oyjlArgsQml_                     ( int                 argc,
                                       const char       ** argv,
                                       const char        * json,
                                       const char        * commands,
                                       const char        * output,
                                       int                 debug,
                                       oyjlUi_s          * ui,
                                       int               (*callback)(int argc, const char ** argv))
{
    int r = oyjlArgsQmlStart__(argc, argv, json, commands, output, debug, ui, callback );
    return r;
}

#ifdef COMPILE_STATIC
#include "oyjl_tree_internal.h" /* oyjlStringToLower() */
static int oyjlArgsRendererSelect   (  oyjlUi_s          * ui )
{
  const char * arg = NULL, * name = NULL;
  oyjlOption_s * R;
  int error = -1;

  if( !ui )
  {
    oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "no \"ui\" argument passed in", OYJL_DBG_ARGS );
    return 1;
  }

  R = oyjlOptions_GetOptionL( ui->opts, "R", 0 );
  if(!R)
  {
    oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "no \"-R|--render\" argument found: Can not select", OYJL_DBG_ARGS );
    return 1;
  }

  if(R->variable_type != oyjlSTRING)
  {
    oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "no \"-R|--render\" oyjlSTRING variable declared", OYJL_DBG_ARGS );
    return 1;
  }

  arg = oyjlStringCopy( *R->variable.s, NULL );
  if(!arg)
  {
    oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "no \"-R|--render\" oyjlSTRING variable found", OYJL_DBG_ARGS );
    return 1;
  }
  else
  {
    if(arg[0])
    {
      char * low = oyjlStringToLower( arg );
      if(low)
      {
        if(strlen(low) >= strlen("gui") && memcmp("gui",low,strlen("gui")) == 0)
          name = "OyjlArgsQml";
        else
        if(strlen(low) >= strlen("qml") && memcmp("qml",low,strlen("qml")) == 0)
          name = "OyjlArgsQml";
        else
        if(strlen(low) >= strlen("web") && memcmp("web",low,strlen("web")) == 0)
          name = "OyjlArgsWeb";
        if(!name)
        {
          oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "\"-R|--render\" not supported: %s|%s", OYJL_DBG_ARGS, arg,low );
          free(low);
          return 1;
        }
        if(strcmp(name,"OyjlArgsQml") == 0)
          error = 0;
        else
          oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "\"-R|--render\" not supported: %s|%s", OYJL_DBG_ARGS, arg,low );
        free(low);
      }
    }
    else /* report all available renderers */
    {
      oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "OyjlArgsQml available - option -R=\"gui\"", OYJL_DBG_ARGS );
    }
  }

  return error;
}
// public API for liboyjl-args-qml-static.a
int oyjlArgsRender                   ( int                 argc,
                                       const char       ** argv,
                                       const char        * json,
                                       const char        * commands,
                                       const char        * output,
                                       int                 debug,
                                       oyjlUi_s          * ui,
                                       int               (*callback)(int argc, const char ** argv))
{
  int result = 1;
  if(oyjlArgsRendererSelect(ui) == 0)
    result = oyjlArgsQml_(argc, argv, json, commands, output, debug, ui, callback );
  fflush(stdout);
  fflush(stderr);
  return result;
}

#endif /* COMPILE_STATIC */
} /* extern "C" */

