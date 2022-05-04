/** @file app_data.h
 *
 *  OpenICC JSON QML is a graphical renderer of UI files.
 *
 *  @par Copyright:
 *            2018-2022 (C) Kai-Uwe Behrmann
 *            All Rights reserved.
 *
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2018/02/26
 *
 *  data load and C++ <-> QML wrappers
 */

#ifndef APP_DATA_H
#define APP_DATA_H

#include <QObject>
#include <QApplication>
#include <QIcon>
#include <QByteArray>
#include "oyjl.h"

#include <QApplication>
#include <QFile>
#include <QDir>
#include <QTimer>
#include <QUrl>

#include "utils.h"
extern QApplication * a;
extern int app_debug;

class AppData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString quitMsg
               READ quitMsg
               WRITE setQuitMsg
               NOTIFY quitMessage
               REVISION 1)
    Q_PROPERTY(QString log
               READ log
               WRITE setLog
               NOTIFY logMessage
               REVISION 1)
private slots:
public:
    AppData(QObject * parent = nullptr)
      : QObject(parent)
      , m_model(nullptr)
    {
#if !defined(Q_OS_ANDROID)
    // Linux polles the sysfs interface
    // Android receives intent updates
    connect(&battery_timer, SIGNAL(timeout()), this, SLOT(readBattery()));
    battery_timer.setInterval(1000);
    battery_timer.start();
#endif
    }

    QString quitMsg() { return m_msg; }
    void setQuitMsg( QString m ) {
        m_msg = m;
        LOG( m_msg );
        emit quitMessage();
        LOG( QString("quitMessage() signal emitted") );
    }

    QString log() { return m_log_msg; }
    void setLog( QString m ) {
        if(app_debug) BT();
        m_log_msg = m;
        LOG( m_log_msg );
        emit logMessage();
    }
    Q_INVOKABLE void setWindowIcon(QString name) { QUrl url = name; QIcon icon(url.toLocalFile());
                                                   if(icon.isNull()) { QIcon i(name); icon = i; }
                                                   if(icon.isNull()) { LOG( "Icon does not exist ??? " + name );
                                                   } else { LOG( "Set Icon: " + name ); a->setWindowIcon(icon); }}

    Q_INVOKABLE QString getJSON(QString url);
    Q_INVOKABLE QString plainJSON(QString url);
    Q_INVOKABLE void  writeJSON(QString url);
    Q_INVOKABLE void  BT(void) { LOG( oyjlBT(0) );};
    Q_INVOKABLE QString getLibDescription(int);
    Q_INVOKABLE void    setOption(QString key, QString value);
    Q_INVOKABLE QString getOption(QString key);
    Q_INVOKABLE QString dumpOptions();
    Q_INVOKABLE void    clearOptions() { oyjlTreeFree(m_model); m_model = nullptr; }
    Q_INVOKABLE QString findLogo(QString pattern);
    Q_INVOKABLE QString readFile(QString url);
    Q_INVOKABLE QString requestPermission( QString name );
    Q_INVOKABLE int hasPermission( QString name );

public slots:
    void readBattery();

signals:
    void quitMessage();
    void logMessage();
    void batteryDischarging(QVariant s);

private:
    QString m_msg;
    QString m_log_msg;
    QTimer           battery_timer;
    oyjl_val m_model;
};

#endif // APP_DATA_H
