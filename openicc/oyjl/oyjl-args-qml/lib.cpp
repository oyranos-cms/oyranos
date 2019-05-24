/** @file lib.cpp
 *
 *  Oyjl JSON QML is a graphical renderer of UI files.
 *
 *  @par Copyright:
 *            2018-2019 (C) Kai-Uwe Behrmann
 *            All Rights reserved.
 *
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2019/04/18
 *
 *  basic QML handling and environment setup
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

int oyjlArgsQmlStart_                ( int                 argc,
                                       char             ** argv,
                                       const char        * json,
                                       int                 debug,
                                       oyjlUi_s          * ui,
                                       int               (*callback)(int argc, const char ** argv))
{
    Q_INIT_RESOURCE(app);

    QApplication app(argc, argv);
    // a = &app; see comment above

    QTranslator translator;
    QString lname( ":/translations/app_" + QLocale::system().name() );
    if(!translator.load( lname ))
        LOG( QString("failed loading locale: ") + lname );
    else
        app.installTranslator(&translator);

    foreach (QScreen * screen, QGuiApplication::screens())
        screen->setOrientationUpdateMask(Qt::LandscapeOrientation | Qt::PortraitOrientation |
                                         Qt::InvertedLandscapeOrientation | Qt::InvertedPortraitOrientation);

    app.setApplicationName(QString("oyjl-args-qml"));
    app.setApplicationDisplayName(QString("Oyjl"));
    app.setApplicationVersion("0.5");
    app.setOrganizationName(QString("oyranos.org"));
    app.setWindowIcon(QIcon(":/images/logo-sw.svg"));

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

      app_init = 1;
      if(app_debug)
          LOG( QString("qml root objects: ") + QString::number( qmlObjects.count() ) );
    }

    processCallback_p = callback;
    app_init = 1;


    mgr.setUri( QString(json) );
    mgr.setCommands("{\"command_set\": \"dummy-callback-placeholder_needed-to-trigger-calling-Process-in-main.qml\"}");

    QQmlContext *ctxt = engine.rootContext();
    ctxt->setContextProperty("ApplicationVersion", QVariant::fromValue( app.applicationVersion() ));
    ctxt->setContextProperty("SysProductInfo", QVariant::fromValue( QSysInfo::prettyProductName() ));
    ctxt->setContextProperty("QtRuntimeVersion", QVariant::fromValue( QString(qVersion()) ));
    ctxt->setContextProperty("QtCompileVersion", QVariant::fromValue( QString(QT_VERSION_STR) ));

    int r = app.exec();
    return r;
}

extern "C" { // "C" API wrapper 
int oyjlArgsQmlStart                 ( int                 argc,
                                       char             ** argv,
                                       const char        * json,
                                       int                 debug,
                                       oyjlUi_s          * ui,
                                       int               (*callback)(int argc, const char ** argv))
{
  int result;
  result = oyjlArgsQmlStart_(argc, argv, json, debug, ui, callback );
  fflush(stdout);
  fflush(stderr);
  return result;
}
} /* extern "C" */
