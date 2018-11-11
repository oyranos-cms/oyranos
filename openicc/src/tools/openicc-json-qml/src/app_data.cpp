/** @file app_data.cpp
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
 *  - AppData -> JSON conversion
 *  - a few app specific functions
 */

#include "include/app_data.h"
#include <QFileInfo>
#include <QDirIterator>
#include <QLocale>
#include <QProcess>
#include <QUrl>
#include <QTextStream>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <openicc_config.h>
#include <openicc_version.h>
#include <oyjl.h>

/** @brief start a process
 *
 *  @param prog    executeable in file path
 *  @param args    passed to the *prog* [ "--optionA", "optionAarg", "-o", "shortOptOarg", "other" ]
 *  @return        the data from *prog*
 */
QByteArray qpOpen( QString prog, QStringList args )
{
    QProcess qp;
    QByteArray ba;
    qp.start(prog, args);
    if (!qp.waitForStarted())
            return ba;
    if (!qp.waitForFinished())
            return ba;
    ba = qp.readAll();

    return ba;
}

/** @brief open a local file from disc */
QString AppData::readFile(QString url)
{
    QString text;
    QFile f;
    if(url == "-")
    {
        f.open(stdin, QIODevice::ReadOnly);
        LOG(QString("Load: %1").arg(url));
    }
    else
    {
        f.setFileName( url );
        f.open(QIODevice::ReadOnly|QIODevice::Unbuffered);
    }
    QByteArray a;
    if(f.isOpen())
    {
        a = f.readAll();
        text = a;
        f.close();
    }
    return text;
}

/** @brief read document JSON from file
 *
 *  The function opens a JSON file. It then injects localisation, the
 *  locale name from Qt. The result is returned as JSON string.
 */
QString AppData::getJSON(QString url)
{
    QString jui = readFile( url );
    QByteArray a = jui.toLocal8Bit();

    QJsonParseError e;
    QJsonDocument jdoc = QJsonDocument::fromJson(a,&e);
    if(jdoc.isNull())
    {
        LOG(QString("%1: %2\n%3").arg(tr("Json is invalid")).arg(url).arg(jui));
    }
    QJsonObject json;
    json = jdoc.object();

    QLocale loc;
    json["prefix"] = "LOCALE_";
    json["LOCALE_info"] = loc.name();

    LOG(tr("finished loading"));

    return QString(QJsonDocument(json).toJson(QJsonDocument::Indented));
}
/** @brief export JSON string from internal model
 *
 *  @param url    the file name to write to
 */
void AppData::writeJSON( QString url )
{
    QFile f;
    if(url == "-")
        f.open(stdout, QIODevice::WriteOnly);
    else if(url.length())
    {
        f.setFileName( url );
        f.open(QIODevice::WriteOnly);
    }
    if(f.isOpen())
    {
        int level = 0;
        char * json = NULL;
        oyjlTreeToJson( m_model, &level, &json );
        if(json)
        {
            f.write(json);
            free(json);
        }
        f.close();
        LOG(QString("Wrote to: %1").arg(url));
    }
}

/** @brief get the first file name in the logo search path
 */
QString findFile(QString pattern, openiccSCOPE_e scope)
{
    QString fn;

    char * p = openiccGetInstallPath(openiccPATH_LOGO, scope, malloc);
    QString path(p);
    if(p) free(p);
    path.replace(QString("~"), QString(QDir::homePath()));

    QStringList search = QStringList() << pattern + "*";
    QDirIterator it(path, search, QDir::AllEntries | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    QStringList files;
    while (it.hasNext())
    {
       files << it.next();
       break;
    }
    if(files.length())
        fn = "file:/" + files[0];
    return fn;
}
/** @brief search for a logo
 *
 *  Scan first the user scope and then for system installed files.
 */
QString AppData::findLogo(QString pattern)
{
    QString fn;
    if (pattern.isEmpty())
        return fn;

    fn = findFile(pattern, openiccSCOPE_USER);
    if(fn.length() == 0)
        fn = findFile(pattern, openiccSCOPE_SYSTEM);
    if(fn.length() == 0)
        fn = findFile(pattern, (openiccSCOPE_e)openiccSCOPE_OPENICC);
    return fn;
}

/** @brief Version strings for UIs
 *
 *  @param         type                - 0 : compile time version
 *                                     - 1 : lib name
 *                                     - 2 : lib name
 *                                     - 3 : run time version
 *  @return                            information string
 */
QString AppData::getLibDescription(int type)
{
    QString v(OPENICC_VERSION_NAME);
    QString p("libOpenICC");
    if(p[0].isLower())
        p[0] = p[0].toUpper();

    switch(type)
    {
    case 0:
        return QString(v);
    case 1:
        return QString(tr("%1 Version")).arg(p);
    case 2:
        return QString(tr("%1 Version")).arg(p);
    case 3:
        return QString::number(openiccVersion());
    case 4:
    {
        QString qv(tr("NONE"));
        return qv;
    }
    }
    return QString("no description found for type ") + QString::number(type);
}

/** @brief modify the JSON model
 *
 *  @param key    object name levels separated by slash '/';
 *                e.g.: "org/freedesktop/openicc/keyA" ;
 *                The key can contain as well array indices:
 *                e.g.: "org/freedesktop/openicc/[0]/firstKey"
 *  @param value  the actual string for the object
 */
void AppData::setOption(QString key, QString value)
{
    LOG(tr("%1:%2").arg(key).arg(value));
    oyjl_val o;
    char * k = oyjlStringCopy(key.toLocal8Bit().constData(), malloc);
    char * v = oyjlStringCopy(value.toLocal8Bit().constData(), malloc);
    if(!m_model)
        m_model = oyjlTreeNew(k);

    o = oyjlTreeGetValue(m_model, OYJL_CREATE_NEW, k);
    oyjlValueSetString(o, v);

#if 0 // debuging
    char * json = NULL;
    int levels = 0;
    oyjlTreeToJson(m_model, &levels, &json);
    LOG(QString(json) + " \n" + k + ":" + v);
    if(json) free(json);
    if(k) free(k);
    if(v) free(v);
#endif
}

/** @brief obtain status on Linux
 */
void AppData::readBattery()
{
    const char * linuxBatteryStatusFile = "/sys/class/power_supply/BAT0/status";
    QFile f(linuxBatteryStatusFile);
    f.open(QIODevice::ReadOnly|QIODevice::Unbuffered);
    char * buf = (char*)calloc(sizeof(char),24);
    if(!buf) return;
    if(f.isOpen())
        f.read(buf,24);
    int state = 0; // no battery use
    if(strstr(buf,"Discharging") != nullptr)
        state = 1;
    emit batteryDischarging(QVariant::fromValue(state));
    free(buf);
}
