/** @file utils.cpp
 *
 *  OpenICC JSON QML is a graphical renderer of UI files.
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

#include <QDateTime>
#include <QObject>
#include <QTextStream>
#if defined(Q_OS_ANDROID)
#include <android/log.h>
#endif

void app_log( QString log_text )
{
    QTextStream s(stderr);
    s << "openicc-json-qml("<< QDateTime::currentDateTime().toString("hh:mm:ss.zzz") <<"): "
      << log_text << endl;
#if defined(Q_OS_ANDROID)
    __android_log_print(ANDROID_LOG_INFO, "openicc-json-qml", "%s", log_text.toLocal8Bit().data() );
#endif
}

void app_log( QString file, QString func, QString log_text )
{
    QString t(file);
    int len = t.lastIndexOf('/');
    if(len > 0)
      t.remove( 0, len+1 );
    t += " " + func;

    QTextStream s(stderr);
    s << t << " " << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") << " "
      << log_text << endl;

#if defined(Q_OS_ANDROID)
    __android_log_print(ANDROID_LOG_INFO, t.toLocal8Bit().data(), "%s", log_text.toLocal8Bit().data() );
#endif
}

int appIsBigEndian ()
{ union { unsigned short u16; unsigned char c; } test = { .u16 = 1 }; return !test.c; }
