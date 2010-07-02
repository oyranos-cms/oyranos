#include <QFile>
#include <QDir>
#include <QRegExp>
#include <QList>
#include <QStringList>
#include <QVariant>

#include <QtDebug>

#include "ClassInfo.h"

QList<ClassInfo*> ClassInfo::getAllClasses( const QString& directory )
{
  QDir sourceDir( directory );
  sourceDir.setNameFilters( QStringList() << "*.dox" );
  sourceDir.setFilter( QDir::Files | QDir::Readable );
  QStringList doxClasses = sourceDir.entryList();
  doxClasses.removeOne( "Class.dox" );
  doxClasses.removeOne( "Struct.dox" );

  QList<ClassInfo*> allClassesInfo;
  for (int c = 0; c<doxClasses.size(); c++) {
    QString ClassName = doxClasses.at( c );
    ClassName.chop(4); //Remove .dox extension

    sourceDir.setNameFilters( QStringList() << ClassName + ".*" );
    if (sourceDir.entryList().size() == 1)
      allClassesInfo << new ClassInfo( ClassName, directory, true );
    else
      allClassesInfo << new ClassInfo( ClassName, directory, false );
  }

  qDebug() << "Found the following classes:";
  for (int i=0; i<allClassesInfo.size(); i++)
    qDebug() << allClassesInfo.at( i )->baseName();

  return allClassesInfo;
}

void ClassInfo::parseDoxyfile()
{
  QFile doxyfile( directory + "/" + base + ".dox");
  if (doxyfile.exists()) {
    doxyfile.open( QIODevice::ReadOnly|QIODevice::Text );
    QString text = doxyfile.readAll();
    
    QRegExp doxy_struct("@struct\\s+(\\w+)");
    QRegExp doxy_ingroup("@ingroup\\s+(\\w+)");
    QRegExp doxy_brief("@brief\\s+([^\\n\\r\\f]+)\\n");
    QRegExp doxy_extends("@extends\\s+(\\w+)");
    QRegExp doxy_internal("@internal\\b");

    if (doxy_struct.indexIn( text ) != -1) {
      if (doxy_struct.cap(1) != name())
        qWarning() << "Struct name '" << doxy_struct.cap(1)
                 << "' does not match with file name: " << doxyfile.fileName();
    } else {
        qWarning() << "No Struct name given in Doxygen file: " << doxyfile.fileName();
    }

    if (doxy_ingroup.indexIn( text ) != -1)
      groupName = doxy_ingroup.cap(1);

    if (doxy_brief.indexIn( text ) != -1)
      doxyBrief = doxy_brief.cap(1);
    else
      qDebug() << "There is no brief description in class" << base;

    if (doxy_extends.indexIn( text ) != -1) {
      parentBase = doxy_extends.cap(1);
      parentBase.remove(0,2); //Remove oy prefix
      parentBase.chop(2); //Remove _s suffix
    }

    if (doxy_internal.indexIn( text ) != -1)
      isInternal = true;

    // Template tags:
    if (text.contains( "[notemplates]" ))
      autotemplates = false;
    if (text.contains( "[opaquepointer]" ))
      hiddenstruct = false;
  }
}

void ClassInfo::parseSourceFiles()
{
  // Get all source files, except from <base>.dox
  QDir sourceDir( directory );
  sourceDir.setNameFilters( QStringList() << base + ".*.h" <<  base + ".*.c" );
  sourceDir.setFilter( QDir::Files | QDir::Readable );
  QStringList sourceFiles = sourceDir.entryList();

  // Attach the names of the source files to the QObject as
  // dynamic properties.
  foreach (QString file, sourceFiles) {
    // Remove the base name + '.'
    int chars = base.size() + 1;
    QString name( QString( file ).remove( 0, chars ).replace( '.', '_' ) );

    setProperty( qPrintable(name), QVariant(file) );
    qDebug() << "Set property" << qPrintable(name) << "->" << property(qPrintable(name)).toString();
  }
}
