#include <QDir>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QVariant>

#include <QtDebug>

#include "ClassTemplates.h"

const QStringList ClassTemplates::sourceFiles = QStringList()
  << "members.h"
  << "private_methods_declarations.h"
  << "private_methods_definitions.c"
  << "public.h"
  << "public_methods_declarations.h"
  << "public_methods_definitions.c";

ClassTemplates::~ClassTemplates()
{
  delete structClassInfo;
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
  //if these files do not already exist.
  //(We do not overwrite, unless updateTemplates is set to true.)
  //Additionally we ignore special classes using the [notemplates]
  //tag (like oyStruct_s and oyObject_s) because their templates are hand-written
  QDir templateDir( templates );
  templateDir.setFilter( QDir::Files | QDir::Readable );
  for (int i=0; i<allClassesInfo.size(); i++) {
    if (!allClassesInfo.at(i)->createTemplates()) {
      qDebug() << "Skipping template files for" << allClassesInfo.at(i)->baseName();
      continue;
    }

    templateDir.setNameFilters( QStringList() << allClassesInfo.at(i)->baseName() + "_s*.template.*" );
    QStringList classTemplateFiles = templateDir.entryList();

    //Create the templates/<group_name>/ directory, if not present
    QString group( allClassesInfo.at(i)->group() );
    if (!templateDir.exists( group ))
      if (!templateDir.mkdir( group )) {
        qWarning() << "Could not create directory" << group;
        continue;
      }

    templateDir.setNameFilters( QStringList() << "Class_s.?" << "Class_s_.?" );
    QStringList genericTemplateFiles = templateDir.entryList();

    for (int g=0; g<genericTemplateFiles.size(); g++) {
      QString oldTemplateFile = genericTemplateFiles.at( g );
      QString newTemplateFile = QString( oldTemplateFile ).
                                replace( '.', ".template." ).
                                replace( "Class", allClassesInfo.at(i)->baseName() );
      QFile newFile( templates + "/" + group + "/" + newTemplateFile );
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
  // Create pairs of (class name,pointer to itself)
  // for easy lookup on the next 'for loop'
  QHash<QString,ClassInfo*> parents;
  parents["oyStruct_s"] = structClassInfo;
  for (int c=0; c<allClassesInfo.size(); c++)
    parents[allClassesInfo.at( c )->name()] = allClassesInfo.at( c );

  // Set the parent of each class
  for (int c=0; c<allClassesInfo.size(); c++)
    allClassesInfo.at( c )->setParent( parents[allClassesInfo.at( c )->parentName()] );

  QVariantList classes;
  for (int c=0; c<allClassesInfo.size(); c++)
    classes << QVariant( QVariant::fromValue( static_cast<QObject*>(allClassesInfo.at( c )) ) );

  return classes;
}

QVariant ClassTemplates::getStructClass() const
{
  return QVariant( QVariant::fromValue( static_cast<QObject*>(structClassInfo) ) );
}
