#include <QRegExp>
#include <QFile>

#include <QtDebug>

#include "FuncInfo.h"

const QString FuncInfo::public_regexp_tmpl =
  "OYAPI\\s+(.*(?:\\b|\\*))\\s+OYEXPORT\\s+oy%1_(\\w+)\\s+\\(\\s+oy%1_s\\s+\\*\\s+\\w+";

QList<QObject*> FuncInfo::getPublicFunctions( const ClassInfo* classInfo )
{
  QList<QObject*> functions;

  QFile c_header( classInfo->srcDir() + "/" + classInfo->public_methods_declarations_h() );
  QString c_header_contents = c_header.readAll();
  QStringList declarations = c_header_contents.split(";");
  for (int i=0; i<declarations.size(); i++)
    functions << static_cast<QObject*>(new FuncInfo( classInfo->baseName(), declarations.at(i)) );

  return functions;
}

void FuncInfo::parsePublicPrototype( const QString& prototype )
{
  int pos = 0;

  // Catch function name and return type
  QRegExp findName( public_regexp_tmpl.arg( m_classBaseName ) );
  if ((pos = findName.indexIn(prototype, pos)) != -1) {
    m_name = findName.cap(2);
    m_returnType = findName.cap(1);

    pos += findName.matchedLength();
  } else {
    qWarning() << "Non compatible function signature:";
    qWarning() << prototype;
    return;
  }

  // Catch arguments
  QRegExp findArgs(",\\s+([^,^)]+\\b)");
  QRegExp splitArg("(.*)(\\b[A-Za-z0-9_]+)$");
  while ((pos = findArgs.indexIn(prototype, pos)) != -1) {
    QString arg = findArgs.cap(1).simplified();
    m_arguments << arg;

    if (splitArg.indexIn(arg) != -1) {
      m_argumentTypes << splitArg.cap(1).trimmed();
      m_argumentNames << splitArg.cap(2).trimmed();
    } else {
      qWarning() << "Unable to split argument:" << arg;
    }

    pos += findArgs.matchedLength();
  }
  //QString cpp_arguments = functionArguments.join(",");
  //QString normSignature = QString("%1 %2(%3)").arg(returnType, functionName, cpp_arguments);
  //qDebug() << normSignature;
  //qDebug() << prototype;
}
