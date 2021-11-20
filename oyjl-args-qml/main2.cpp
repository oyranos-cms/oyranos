/** @file main.cpp
 *
 *  Oyjl JSON QML is a graphical renderer of UI files.
 *
 *  @par Copyright:
 *            2018-2019 (C) Kai-Uwe Behrmann
 *            All Rights reserved.
 *
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2018/02/26
 *
 *  basic QML handling and environment setup
 */

#include <QApplication>
#include <QIcon>
#include <QImageReader>
#include <QMessageBox>
#include <QQmlApplicationEngine>
#include <QtQml> // qmlRegisterType<>()

#include "include/app_data.h"
#include "include/app_manager.h"
#include "include/process.h"

// QApplication * a = NULL; not needed, as only static member functions are called

#include <QTranslator>
#include <QUrl>
#include <QObject>
#include <QSysInfo>
#include <QtGui/QScreen>
#ifdef QT_DBUS_LIB
#include <QtDBus>
#endif

#if defined(Q_OS_ANDROID)
#include <android/log.h>
#include "main2.h"
#endif

// startup stuff
AppManager mgr;
AppManager * m = &mgr;
int app_init = 0;
int app_debug = 0;

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


int main(int argc, char *argv[])
{
    setenv("FORCE_COLORTERM", "1", 0); /* show rich text format on non GNU color extension environment */

    Q_INIT_RESOURCE(app);

    QGuiApplication app(argc, argv);
    // a = &app; see comment above
    printf( "image plugins:\n");
    foreach (QByteArray io, QImageReader::supportedImageFormats())
      printf( "\t%s", qPrintable( QString(io) ));
    printf( "\n");


    QTranslator translator;
    QString lname( ":/translations/app_" + QLocale::system().name() );
    if(!translator.load( lname ))
        LOG( QString("failed loading locale: ") + lname );
    else
        app.installTranslator(&translator);

#if QT_VERSION_MAJOR == 5
    foreach (QScreen * screen, QGuiApplication::screens())
        screen->setOrientationUpdateMask(Qt::LandscapeOrientation | Qt::PortraitOrientation |
                                         Qt::InvertedLandscapeOrientation | Qt::InvertedPortraitOrientation);
#endif

    app.setApplicationName(QString("oyjl-args-qml"));
    app.setApplicationDisplayName(QString("Oyjl"));
    app.setApplicationVersion("0.7");
    app.setOrganizationName(QString("oyranos.org"));
    app.setWindowIcon(QIcon(":/images/logo-sw.svg"));

    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::translate("main", "QML Renderer for JSON Options"));
    QCommandLineOption ho = parser.addHelpOption();

    QCommandLineOption inputOption(QStringList() << "i" << "input",
              QCoreApplication::translate("main", "specify input JSON file; '-' stands for stdin stream"),
              QCoreApplication::translate("main", "path/to/options.json"));
    parser.addOption(inputOption);
    QCommandLineOption outputOption(QStringList() << "o" << "output",
              QCoreApplication::translate("main", "specify output JSON file; '-' stands for stdout stream"),
              QCoreApplication::translate("main", "path/to/options.json"));
    parser.addOption(outputOption);
    QCommandLineOption commandsOption(QStringList() << "c" << "command",
              QCoreApplication::translate("main", "specify commands JSON file; '-' stands for stdin stream; '+' stands for read from -i file"),
              QCoreApplication::translate("main", "path/to/commands.json"));
    parser.addOption(commandsOption);
    QCommandLineOption verboseOption("v",
              QCoreApplication::translate("main", "Verbose mode. Prints out more information."));
    parser.addOption(verboseOption);
    QCommandLineOption aboutOption("verbose",
              QCoreApplication::translate("main", "Prints basic information."));
    parser.addOption(aboutOption);

    parser.parse(QApplication::arguments());
    if(parser.isSet(verboseOption))
        ++app_debug;

    if(app_debug)
    {
        LOG( QString("app args[") + QString::number(argc) + "]:" );
        for(int i = 0; i < argc; ++i)
            LOG( QString(argv[i]) );
    }

    if(parser.isSet(ho))
    {
        printf(qPrintable(parser.helpText()));
        printf( "%s:\n\t%s\n\t%s\n",
        qPrintable( QCoreApplication::translate("main", "Example") ),
        qPrintable( QCoreApplication::translate("main", "Open JSON from stdin and give result JSON to stdout:\n\t\tcat options.json | %1 -i - -o - | xargs echo").arg(app.applicationName()) ),
        qPrintable( QCoreApplication::translate("main", "Open JSON from stdin and specify commands:\n\t\tcat options.json | %1 -i - -c commands.json").arg(app.applicationName()) ));
        return 0;
    }
    if(parser.isSet(aboutOption))
    {
        printf( "%s %s\t%s\n",
        qPrintable( app.applicationName() ),
        qPrintable( app.applicationVersion() ),
        qPrintable( QCoreApplication::translate("main", "is a QML renderer for Oyjl JSON options.") ));
        return 0;
    }

    parser.process(app);

    QString inputJSON = parser.value(inputOption);
    QString outputJSON = parser.value(outputOption);
    QString commandsJSON = parser.value(commandsOption);
#if defined(Q_OS_ANDROID)
    inputJSON = QString(oyjl_json);
#endif


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
          LOG( QString("qml root objects: ") + QString::number( qmlObjects.count() ) );
    }

    app_init = 1;


    if(inputJSON.length())
        mgr.setUri( inputJSON );
    else
        mgr.setUri( "-" );
    if(outputJSON.length())
        mgr.setOutput( outputJSON );
    if(commandsJSON.length())
        mgr.setCommands( commandsJSON );

    QQmlContext *ctxt = engine.rootContext();
    ctxt->setContextProperty("ApplicationVersion", QVariant::fromValue( app.applicationVersion() ));
    ctxt->setContextProperty("SysProductInfo", QVariant::fromValue( QSysInfo::prettyProductName() ));
    ctxt->setContextProperty("QtRuntimeVersion", QVariant::fromValue( QString(qVersion()) ));
    ctxt->setContextProperty("QtCompileVersion", QVariant::fromValue( QString(QT_VERSION_STR) ));

    return app.exec();
}
