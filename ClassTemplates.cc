#include <QDir>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QVariant>

#include <QtDebug>

#include "ClassTemplates.h"

QStringList ClassTemplates::sourceFiles = QStringList() << "members.h"
                                                        << "private_methods_declarations.h"
                                                        << "private_methods_definitions.c"
                                                        << "public_methods_declarations.h"
                                                        << "public_methods_definitions.c";

ClassTemplates::~ClassTemplates()
{
  for (int i=0; i<allClassesInfo.size(); i++)
    delete allClassesInfo.at( i );
}

void ClassTemplates::createTemplates()
{
  //Create the empty sources/* files for all present classes,
  //but only if these files do not exist (we do not overwrite)
  QDir sourceDir( sources );
  sourceDir.setFilter( QDir::Files | QDir::Readable );
  for (int i=0; i<allClassesInfo.size(); i++) {
    sourceDir.setNameFilters( QStringList() << allClassesInfo.at(i)->baseName() + ".*" );
    QStringList classSourceFiles = sourceDir.entryList();
    for (int s=0; s<sourceFiles.size(); s++) {
      QString classSourceFile = allClassesInfo.at(i)->baseName() + "." + sourceFiles.at(s);
      QFile f( sources + "/" + classSourceFile );
      if (!f.exists()) {
        f.open( QIODevice::WriteOnly );
        qDebug() << "Creating file" << classSourceFile;
      } else {
        qDebug() << "Skipping file" << classSourceFile;
      }
    }
  }

  //Create the template files in templates/ for all present classes,
  //but only if these files do not exist (we do not overwrite),
  //unless updateTemplates is set to true
  QDir templateDir( templates );
  templateDir.setFilter( QDir::Files | QDir::Readable );
  for (int i=0; i<allClassesInfo.size(); i++) {
    templateDir.setNameFilters( QStringList() << allClassesInfo.at(i)->baseName() + "_s*.template.*" );
    QStringList classTemplateFiles = templateDir.entryList();

    templateDir.setNameFilters( QStringList() << "Class_s*.?" );
    QStringList genericTemplateFiles = templateDir.entryList();
    for (int g=0; g<genericTemplateFiles.size(); g++) {
      QString oldTemplateFile = genericTemplateFiles.at( g );
      QString newTemplateFile = QString( oldTemplateFile ).
                                replace( '.', ".template." ).
                                replace( "Class", allClassesInfo.at(i)->baseName() );
      QFile newFile( templates + "/" + newTemplateFile );
      QFile oldFile( templates + "/" + oldTemplateFile );
      if (updateTemplates || !newFile.exists()) {
        newFile.open( QIODevice::WriteOnly|QIODevice::Text );
        oldFile.open( QIODevice::ReadOnly|QIODevice::Text );
        QString fileData = oldFile.readAll();
        fileData.replace( QRegExp("(include\\s+\")Class\\."),
                          "\\1" + allClassesInfo.at(i)->baseName() + "." );

        if (allClassesInfo.at(i)->parentBaseName() != "Struct")
          fileData.replace( QRegExp("Base(_s_?)\\.([ch])"),
                            allClassesInfo.at(i)->parentBaseName() + "\\1" + ".template." + "\\2" );

        newFile.write( fileData.toAscii() );
        qDebug() << "Creating file" << newFile.fileName();
      } else {
        qDebug() << "Skipping file" << newFile.fileName();
      }
    }
  }
}

QList<QVariant> ClassTemplates::getAllClasses()
{
  QVariantList classes;
  for (int c=0; c<allClassesInfo.size(); c++)
    classes << QVariant( QVariant::fromValue( static_cast<QObject*>(allClassesInfo.at( c )) ) );

  return classes;
}
