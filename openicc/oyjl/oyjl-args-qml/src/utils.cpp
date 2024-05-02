/** @file utils.cpp
 *
 *  Oyjl JSON QML is a graphical renderer of UI files.
 *
 *  @par Copyright:
 *            2018 (C) Kai-Uwe Behrmann
 *            All Rights reserved.
 *
 *  @par License:
 *            MIT <http://www.opensource.org/licenses/mit-license.php>
 *  @since    2018/02/26
 *
 *  logging and helpers
 */

#include "include/utils.h"
#include "include/app_manager.h"

#include <QDateTime>
#include <QObject>
#include <QTextStream>
#if defined(Q_OS_ANDROID)
#include <android/log.h>
#endif

#define LOG_NAME "oyjl-args-qml"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
#define e Qt::endl
#else
#define e endl
#endif

void app_log( QString log_text )
{
    QString text;
    QTextStream l(&text);
    QTextStream s(stderr);
    l << LOG_NAME "("<< QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<"): "
      << log_text << e;
    s << text;
    m->setFullLog(text);
#if defined(Q_OS_ANDROID)
    __android_log_print(ANDROID_LOG_INFO, LOG_NAME, "%s", log_text.toLocal8Bit().data() );
#endif
}

void app_log( const char * log_text )
{
  app_log( QString(log_text) );
}

static QString * log_history = nullptr;

void app_log( QString file, QString func, QString log_text )
{
    if(log_history == nullptr)
        log_history = new QString("");

    QString t(file);
    int len = t.lastIndexOf('/');
    if(len > 0)
      t.remove( 0, len+1 );
    t += " " + func;

    QString text;
    QTextStream l(&text);
    QTextStream s(stderr);
    l << t << " " << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") << " "
      << log_text << e;
    s << text;

    int ready = 0;
    if(app_init)
    {
      QString * h = log_history;
      if(m)
      {
        QString msg = *h + text;
        m->setFullLog( msg );
        *log_history = "";
        ready = 1;
      }
    }

    if(!ready)
        *log_history += text;

#if defined(Q_OS_ANDROID)
    __android_log_print(ANDROID_LOG_INFO, t.toLocal8Bit().data(), "%s", log_text.toLocal8Bit().data() );
#endif
}

int appIsBigEndian ()
{ union { unsigned short u16; unsigned char c; } test = { .u16 = 1 }; return !test.c; }
