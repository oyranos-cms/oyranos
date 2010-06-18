#include <QDir>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QByteArray>

#include "ClassTemplates.h"

QStringList ClassTemplates::sourceFiles = QStringList() << "members.h"
                                                        << "private_methods_declarations.h"
                                                        << "private_methods_definitions.c"
                                                        << "public_methods_declarations.h"
                                                        << "public_methods_definitions.c";

void ClassTemplates::createTemplates()
{
  //Create the empty sources/* files for all present classes,
  //but only if these files do not exist (we do not overwrite)
  QDir sourceDir( sources );
  sourceDir.setFilter( QDir::Files | QDir::Readable );
  for (int i=0; i<allClasses.size(); i++) {
    sourceDir.setNameFilters( QStringList() << allClasses.at(i) + ".*" );
    QStringList classSourceFiles = sourceDir.entryList();
    for (int s=0; s<sourceFiles.size(); s++) {
      QString classSourceFile = allClasses.at(i) + "." + sourceFiles.at(s);
      QFile f( classSourceFile );
      if (!f.exists())
        f.open( QIODevice::WriteOnly );
    }
  }

  //Create the template files in templates/ for all present classes,
  //but only if these files do not exist (we do not overwrite),
  //unless updateTemplates is set to true
  QDir templateDir( templates );
  templateDir.setFilter( QDir::Files | QDir::Readable );
  for (int i=0; i<allClasses.size(); i++) {
    templateDir.setNameFilters( QStringList() << allClasses.at(i) + "_s*.template.*" );
    QStringList classTemplateFiles = templateDir.entryList();

    templateDir.setNameFilters( QStringList() << "Class_s*.?" );
    QStringList genericTemplateFiles = templateDir.entryList();
    for (int g=0; g<genericTemplateFiles.size(); g++) {
      QString newTemplateFile = QString( genericTemplateFiles.at( g ) ).
                                replace( '.', ".template." ).
                                replace( "Class", allClasses.at( i ) );
      QFile newFile( newTemplateFile );
      QFile oldFile( genericTemplateFiles.at( g ) );
      if (updateTemplates || !newFile.exists()) {
        newFile.open( QIODevice::WriteOnly|QIODevice::Text );
        oldFile.open( QIODevice::ReadOnly|QIODevice::Text );
        QByteArray fileData = oldFile.readAll();
        fileData.replace( QString("Class"), allClasses.at(i).toAscii() );
        newFile.write( fileData );
      }
    }
  }
}

void ClassTemplates::findClasses()
{
  QDir sourceDir( sources );
  sourceDir.setNameFilters( QStringList() << "*.dox" );
  sourceDir.setFilter( QDir::Files | QDir::Readable );
  QStringList doxClasses = sourceDir.entryList();
  doxClasses.removeOne( "Class.dox" );
  for (int c = 0; c<doxClasses.size(); c++) {
    QString ClassName = doxClasses.at( c );
    ClassName.chop(4); //Remove .dox extension
    allClasses << ClassName;
    sourceDir.setNameFilters( QStringList() << ClassName + ".*" );
    if (sourceDir.entryList().size() == 1)
      doxOnlyClasses << ClassName;
  }
}
