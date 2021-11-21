/** @file utils.h
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
 *  helpers
 */

#ifndef UTILS_H
#define UTILS_H

#include <QString>

void app_log( QString s );
void app_log( QString file, QString func, QString log_text );

#define LOG( text ) app_log( QString(__FILE__) + ":" + QString::number(__LINE__), QString(__func__) + "()", text )

int appIsBigEndian ();

#define GCC_VERSION (__GNUC__ * 10000 \
                     + __GNUC_MINOR__ * 100 \
                     + __GNUC_PATCHLEVEL__)
/* Test for GCC prior 4.6.0 */
#if GCC_VERSION < 40600
#define nullptr NULL
#endif


#endif // UTILS_H
