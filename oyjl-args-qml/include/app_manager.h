/** @file app_manager.h
 *
 *  OpenICC JSON QML is a graphical renderer of UI files.
 *
 *  @par Copyright:
 *            2018-2021 (C) Kai-Uwe Behrmann
 *            All Rights reserved.
 *
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2018/02/26
 *
 *  independent file load instance
 */

#ifndef APP_MANAGER_H
#define APP_MANAGER_H
#include <include/app_data.h>
#include <QUrl>
#ifdef QT_DBUS_LIB
#include <QtDBus>
#endif
#ifdef OPENICC_LIB
#include <openicc_db.h>
#endif

class AppManager : public QObject
{
    Q_OBJECT
    QString battery_device_;
    QString uri_;

public:
    AppManager(QObject * parent = 0) : QObject(parent) { }
    ~AppManager() { }

    // keep uri around (before main() call) or send uri to QML data
    void setUri( QString uri )
    {
        uri_ = uri;
        LOG( QString("emit uriChanged(%1)").arg(uri[0] != '{'?uri:"---") );
        emit uriChanged(uri_);
    }
    QString getUri() { return uri_; }
    void setOutput( QString output ) { emit outputChanged(output); }
    void setCommands( QString commands ) { emit commandsChanged(commands); }
    void setDebug( qint64 debug ) { emit debugChanged(debug); }

public slots:
    void setFullLog(QString text) { emit fullLogMessage( QVariant::fromValue( text ) ); }
    void setBatteryInfo(QString text) {  battery_device_ = text; emit batteryInfo( QVariant::fromValue( battery_device_ ) ); }
    QString getBatteryInfo() { return battery_device_; }
#if defined(QT_DBUS_LIB) && defined(OPENICC_LIB)
    void setNightInfo(QDBusMessage msg) {
        QList<QVariant> args = msg.arguments();
        if( strstr( args[0].toString().toLocal8Bit().data(), "user/org/freedesktop/openicc/display/night" ) ) {
            LOG("detected night change");
            emit nightInfo( QVariant::fromValue( isNight() ) );
        }
    }
    bool isNight() {
        bool is_night = false;
        const char * night = nullptr, * night_effect = nullptr;
        openiccDB_s * db = openiccDB_NewFrom("org/freedesktop/openicc", openiccSCOPE_USER_SYS );
        openiccDB_GetString( db, "org/freedesktop/openicc/display/night_effect", &night_effect );
        openiccDB_GetString( db, "org/freedesktop/openicc/display/night", &night );
        if(night && strcmp(night,"1") == 0 && !night_effect)
            is_night = true;
        openiccDB_Release( &db );
        return is_night;
    }
#endif

Q_SIGNALS:
    void fullLogMessage(QVariant);
    void batteryInfo(QVariant);
    void nightInfo(QVariant);
    void uriChanged(QVariant);
    void outputChanged(QVariant);
    void commandsChanged(QVariant);
    void debugChanged(QVariant);
};

#endif // APP_MANAGER_H

