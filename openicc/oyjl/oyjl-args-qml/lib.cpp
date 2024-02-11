/** @file lib.cpp
 *
 *  Oyjl JSON QML is a graphical renderer of UI files.
 *
 *  @par Copyright:
 *            2018-2023 (C) Kai-Uwe Behrmann
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
oyjlUi_s          * oyjl_ui_qml = NULL;

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
      oyjl_ui_qml = ui;
      return 0;
    }

    Q_INIT_RESOURCE(app);

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, (char**)argv);
    // a = &app; see comment above

    QTranslator translator;
    QString lname( ":/translations/app_" + QLocale::system().name() );
    LOG( QString("Qlocale::system():") +QLocale::system().name() );
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
    app.setApplicationVersion("0.9");
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

    if(oyjl_ui_qml)
      ui = oyjl_ui_qml;

    oyjl_val root = NULL;
    int r = 0;
    if( json && strlen( json ) )
    {
      int state = 0;
      r = oyjlIsFile( json, "r", OYJL_NO_CHECK, NULL, 0 );
      if(!r && oyjlDataFormat(json) == 7)
        root = oyjlTreeParse2( json, 0, __func__, &state );
      else
      {
        int size = 0;
        char * t = oyjlReadFile(json, 0, &size);
        if(t)
          LOG( QString("Found file name: ") + json );
        else
        {
          t = oyjlReadFile(json, OYJL_IO_STREAM, &size);
          if(t)
            LOG( QString("Found stream: ") + json );
        }
        root = oyjlTreeParse2( t, 0, __func__, &state );
        if(t) free(t);
      }
      if(state)
      {
        fprintf(stderr, "ERROR:\t\"%s\"\n", oyjlPARSE_STATE_eToString(state));
        char * error = NULL;
        oyjlStringAdd( &error, 0,0, "{\"error\": \"%s\"}", json );
        json = error;
        r = -1;
      }
    }

    int found = 0;
    if( root && oyjlTreeGetValue(root, 0, "org/freedesktop/oyjl/modules") ) /* json */
    {
      found = 1;
      LOG( QString("Found Json org/freedesktop/oyjl/modules: ") + QString::number(strlen(json)) );
    }
    else
    if( root && oyjlTreeGetValue(root, 0, "org/freedesktop/oyjl/ui") ) /* -X=export */
    {
      oyjlUi_s * u = oyjlUi_ImportFromJson( root,0 );
      char * t = oyjlUi_ToText( u, oyjlARGS_EXPORT_JSON, 0 );
      int state = 0;
      oyjlTreeFree(root);
      root = oyjlTreeParse2( t, 0, __func__, &state );
      if(t) free(t);
      if(state)
      {
        fprintf(stderr, "ERROR:\t\"%s\"\n", oyjlPARSE_STATE_eToString(state));
        char * error = NULL;
        oyjlStringAdd( &error, 0,0, "{\"error\": \"%s\"}", json );
        json = error;
        r = -1;
        if(!ui)
          ui = u;
      } else
        found = 1;
      LOG( QString("Found Export org/freedesktop/oyjl/ui: ") + QString::number(strlen(json)) );
    }

    const char * renderer_value = NULL;
    if(found == 0)
    {
      char * json = oyjlUi_ToJson( ui, 0 ); // generate JSON from ui data struct
      LOG( QString("oyjlUi_ToJson(): ") + QString::number(json?strlen(json):0) );
      int state = 0;
      oyjl_val module = oyjlTreeParse2( json, 0, __func__, &state );
      if(json) { free(json); json = NULL; }
      if(root && module)
      {
        oyjl_val rv = oyjlTreeGetValue(root, 0, "org/freedesktop/oyjl/translations");
        oyjl_val mv = oyjlTreeGetValue(module, OYJL_CREATE_NEW, "org/freedesktop/oyjl/translations");
        if(rv && mv) // merge in translations
        {
          size_t size = sizeof(*rv);
          memcpy( mv, rv, size );
          memset( rv, 0, size );
          LOG( QString("merge UI JSON with translation") );
        } else
          LOG( QString("expected translation is missing") );
      }
      if(root) { oyjlTreeFree( root ); root = NULL; }
      if(module)
        LOG( QString("use generated UI JSON") );
      root = module;
      oyjlOptions_GetResult( ui->opts, "render", &renderer_value, 0, 0 );
      if(!renderer_value) oyjlOptions_GetResult( ui->opts, "R", &renderer_value, 0, 0 );
      fprintf( stderr, OYJL_DBG_FORMAT "render=\"%s\"\n", OYJL_DBG_ARGS, renderer_value );
      if(oyjlStringSplitFind(renderer_value, ":", "help", 0, NULL, 0,0) >= 0)
      {
        fprintf( stderr, "  %s:\n", oyjlTermColor(oyjlUNDERLINE, QCoreApplication::translate("main", "Help").toLocal8Bit().data()) );
        fprintf( stderr, "    %s\n\n", oyjlTermColor(oyjlBOLD, "--render=qml:start=instant") );
        fprintf( stderr, "      %s=\t%s\n", oyjlTermColor(oyjlBOLD, "start"), QCoreApplication::translate("main", "Select start mode; optional, default is interactive").toLocal8Bit().data() );
        fprintf( stderr, "        =%s\t- %s\n", oyjlTermColor( oyjlITALIC, "instant"), QCoreApplication::translate("main", "Execute the tool without previous interaction using the provided arguments and display results.").toLocal8Bit().data() );
        return 0;
      }
    }

    // get our defaults
    oyjl_val val = oyjlTreeGetValue(root, 0, "org/freedesktop/oyjl/modules/[0]/groups/[0]/properties/oyjl_args/gui");
    if(!val)
      LOG( QString("no gui properties") );
    {
      const char * oyjl_args_properties = OYJL_GET_STRING(val);
      oyjl_val defaults = oyjlOptionStringToJson( oyjl_args_properties );
      oyjlUiJsonSetDefaults( root, defaults );
      oyjlTreeFree( defaults ); defaults = NULL;
    }

    char * t = oyjlTreeToText( root, OYJL_JSON );
    if(t)
    {
      mgr.setUri( QString(t) );
      free(t); t = NULL;
    }


    oyjl_val c = NULL;
    if( commands )
    {
      int state = 0;
      c = oyjlTreeParse2( commands, 0, __func__, &state );
    }
    else
    {
      c = oyjlTreeNew("");
      oyjlTreeSetStringF( c, OYJL_CREATE_NEW, ui->nick, "command_set" );
    }
    if(oyjlStringSplitFind(renderer_value, ":", "start=instant", 0, NULL, 0,0) >= 0)
      oyjlTreeSetStringF( c, OYJL_CREATE_NEW, "instant", "start" );
    t = oyjlTreeToText( c, OYJL_JSON );
    if(c) { oyjlTreeFree( c ); c = NULL; }
    fprintf( stderr, OYJL_DBG_FORMAT "setCommands(=\"%s\")\n", OYJL_DBG_ARGS, t );
    mgr.setCommands( t );
    free(t); t = NULL;

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
#include "oyjl_tree_internal.h" /* oyjlStringToLower_() */
static int oyjlArgsRendererSelect   (  oyjlUi_s          * ui )
{
  const char * name = NULL;
  char * arg = NULL;
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
      if(oyjlStringStartsWith(arg,"gui", OYJL_COMPARE_CASE))
        name = "OyjlArgsQml";
      else
      if(oyjlStringStartsWith(arg,"qml", OYJL_COMPARE_CASE))
        name = "OyjlArgsQml";
      else
      if(oyjlStringStartsWith(arg,"cli", OYJL_COMPARE_CASE))
        name = "OyjlArgsCli";
      else
      if(oyjlStringStartsWith(arg,"web", OYJL_COMPARE_CASE))
        name = "OyjlArgsWeb";
      if(!name)
      {
        oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "\"-R|--render\" not supported: %s", OYJL_DBG_ARGS, arg );
        return 1;
      }
      if(strcmp(name,"OyjlArgsQml") == 0)
        error = 0;
      else
      if(strcmp(name,"OyjlArgsCli") == 0)
        error = -2;
#ifdef OYJL_HAVE_MHD
      else
      if(strcmp(name,"OyjlArgsWeb") == 0)
        error = -3;
#endif
      else
        oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "\"-R|--render\" not supported: %s", OYJL_DBG_ARGS, arg );
    }
    else /* report all available renderers */
    {
      oyjlMessage_p( oyjlMSG_INFO, 0, OYJL_DBG_FORMAT "OyjlArgsQml available - option -R=\"gui\"", OYJL_DBG_ARGS );
    }
    free(arg); arg = NULL;
  }

  return error;
}
// public API from liboyjl-args-cli-static.a
int oyjlArgsCli_                     ( int                 argc,
                                       const char       ** argv,
                                       const char        * json,
                                       const char        * commands,
                                       const char        * output,
                                       int                 debug,
                                       oyjlUi_s          * ui,
                                       int               (*callback)(int argc, const char ** argv));
int oyjlArgsWeb_                     ( int                 argc,
                                       const char       ** argv,
                                       const char        * json,
                                       const char        * commands,
                                       const char        * output,
                                       int                 debug,
                                       oyjlUi_s          * ui,
                                       int               (*callback)(int argc, const char ** argv));
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
  int use = oyjlArgsRendererSelect(ui);
  if(use == 0)
    result = oyjlArgsQml_(argc, argv, json, commands, output, debug, ui, callback );
  else if(use == -2)
    result = oyjlArgsCli_(argc, argv, json, commands, output, debug, ui, callback );
#ifdef OYJL_HAVE_MHD
  else if(use == -3)
    result = oyjlArgsWeb_(argc, argv, json, commands, output, debug, ui, callback );
#endif
  fflush(stdout);
  fflush(stderr);
  return result;
}

#endif /* COMPILE_STATIC */
} /* extern "C" */

