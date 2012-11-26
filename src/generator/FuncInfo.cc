#include <QRegExp>
#include <QFile>

#include <QtDebug>

#include "FuncInfo.h"

const QString FuncInfo::public_regexp_tmpl =
  "OYAPI\\s+(.*(?:\\b|\\*))\\s+OYEXPORT\\s+oy%1_(\\w+)\\s*\\(\\s*oy%1_s\\s*\\*\\s*\\w+.*\\)";

const QString FuncInfo::public_special_regexp_tmpl =
  "OYAPI\\s+(.*(?:\\b|\\*))\\s+OYEXPORT\\s+oy%1_(\\w+)\\s*\\(.*\\)";

QVariantList FuncInfo::getPublicFunctions( const ClassInfo* classInfo )
{
  QVariantList functions;

  QFile c_header( classInfo->srcDir() + "/" + classInfo->public_methods_declarations_h() );
  if (!c_header.exists()) {
    qWarning() << "File not found:" << c_header.fileName();
    return functions;
  }
  if (!c_header.open( QIODevice::ReadOnly|QIODevice::Text )) {
    qWarning() << "Could not open file" << c_header.fileName() << "for reading";
  }

  QString c_header_contents = c_header.readAll().trimmed();
  QStringList declarations = c_header_contents.split(";", QString::SkipEmptyParts);
  for (int i=0; i<declarations.size(); i++) {
    FuncInfo* fp = new FuncInfo( classInfo->baseName(), declarations.at(i));
    if (!fp->isValid()) {
      delete fp; fp = NULL;
      continue;
    }
    functions << QVariant( QVariant::fromValue( static_cast<QObject*>(fp) ) );
  }

  return functions;
}

void FuncInfo::parsePublicPrototype( const QString& prototype )
{
  // Catch function name and return type
  QRegExp findName( public_regexp_tmpl.arg( m_classBaseName ) );
  QRegExp findNameSpecial( public_special_regexp_tmpl.arg( m_classBaseName ) );
  if (findName.indexIn(prototype) != -1) {
    m_name = findName.cap(2);
    m_returnType = findName.cap(1);
  } else if (findNameSpecial.indexIn(prototype) != -1) {
    m_name = findNameSpecial.cap(2);
    m_returnType = findNameSpecial.cap(1);

    if (m_name.contains( QRegExp("From|Create") ) &&
        m_returnType.contains( QRegExp("^\\s*oy"+m_classBaseName+"_s\\s*\\*\\s*$") ))
      m_constructor = true;
    else
      m_static = true;
  } else {
    m_valid = false;
    qWarning() << "Non compatible function signature:";
    qWarning() << prototype;
    return;
  }

  // Catch arguments
  int open_idx = prototype.indexOf( "(" );
  int close_idx = prototype.indexOf( ")" );
  QString args = prototype.mid( open_idx+1, close_idx-open_idx-1 ).simplified();
  QStringList arglist = args.split( ",", QString::SkipEmptyParts );
  if (!(m_static || m_constructor) && !arglist.isEmpty())
    arglist.removeFirst();

  for (int i=0; i<arglist.size(); i++) {
    QString arg = arglist.at(i).trimmed();
    m_arguments << arg;

    QRegExp splitArg("(.*)(\\b[A-Za-z0-9_]+\\b)(?:\\[.*\\])*$");
    if (splitArg.indexIn(arg) != -1) {
      m_argumentTypes << splitArg.cap(1).trimmed();
      m_argumentNames << splitArg.cap(2).trimmed();
    } else {
      m_valid = false;
      qWarning() << "Unable to split argument:" << arg;
    }
  }
}

// Later: Add an argInfo class that keeps C/C++/python/... types/transforms, etc
void FuncInfo::cppTransformations()
{
  QRegExp const_char_p( "^const +char *\\*$" );
  QRegExp oyclass_p( "^oy([A-Za-z0-9]+)_s *\\*$" );

  for (int i=0; i<m_argumentTypes.size(); i++) {
    if (m_argumentTypes.at(i).contains(const_char_p)) {
      m_cppArgumentTypes << "const std::string&";
      m_cppArgumentNames << m_argumentNames.at(i) + ".c_str()";
    } else if (m_argumentTypes.at(i).contains(oyclass_p)) {
      m_cppArgumentTypes << oyclass_p.cap(1);
      m_cppArgumentNames << m_argumentNames.at(i) + ".c_struct()";
    } else {
      m_cppArgumentTypes << m_argumentTypes.at(i);
      m_cppArgumentNames << m_argumentNames.at(i);
    }
  }

  for (int i=0; i<m_argumentNames.size(); i++)
    m_cppArguments << m_cppArgumentTypes.at(i) + " " + m_argumentNames.at(i);
}
