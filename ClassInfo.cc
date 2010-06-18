#include <QFile>
#include <QRegExp>
#include <QStringList>

#include <QtDebug>

#include "ClassInfo.h"

void ClassInfo::parseDoxyfile()
{
  QFile doxyfile( directory + "/" + base + ".dox");
  if (doxyfile.exists()) {
    doxyfile.open( QIODevice::ReadOnly|QIODevice::Text );
    QString text = doxyfile.readAll();
    
    QRegExp doxy_struct("@struct\\s+(\\w+)");
    QRegExp doxy_ingroup("@ingroup\\s+(\\w+)");
    QRegExp doxy_extends("@extends\\s+(\\w+)");
    QRegExp doxy_internal("@internal\\b");

    if (doxy_struct.indexIn( text ) != -1) {
      if (doxy_struct.cap(1) != name())
        qWarning << "Struct name '" << doxy_struct.cap(1)
                 << "' does not match with file name: " << doxyfile.fileName();
    } else {
        qWarning << "No Struct name given in Doxygen file: " << doxyfile.fileName();
    }

    if (doxy_ingroup.indexIn( text ) != -1)
      groupName = doxy_ingroup.cap(1);

    if (doxy_extends.indexIn( text ) != -1) {
      parentBase = doxy_extends.cap(1);
      parentBase.remove(0,2); //Remove oy prefix
      parentBase.chop(2); //Remove _s suffix
    }

    if (doxy_internal.indexIn( text ) != -1)
      isInternal = true;
  }
}
