/** @file utils.h
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
 *  helpers
 */

#ifndef UTILS_H
#define UTILS_H

#include <QString>

void app_log( QString s );
void app_log( QString file, QString func, QString log_text );

#define LOG( text ) app_log( QString(__FILE__) + ":" + QString::number(__LINE__), QString(__func__) + "()", text )

int appIsBigEndian ();

#endif // UTILS_H
