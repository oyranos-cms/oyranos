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
  << "private.h"
  << "private_methods_declarations.h"
  << "private_methods_definitions.c"
  << "public.h"
  << "public_methods_declarations.h"
  << "public_methods_definitions.c";

ClassTemplates::ClassTemplates( const QString& src, const QString& tpl )
  : sources(src), templates(tpl),
    structClassInfo(new ClassInfo("Struct", src)), nullClassInfo(new ClassInfo("Null", src))
{
  structClassInfo->setParent( nullClassInfo );
  nullClassInfo->setParent( nullClassInfo );

  // 1. Get all classes that have a *.dox file
  // (oyStruct_s is ommited)
  allClassesInfo = ClassInfo::getAllClasses( sources );

  // 2. Initialise the pointers with the parent class
  // and the content class if it is a "list" class
  // a) Create pairs of (class name,pointer to itself)
  // for easy lookup on the next 'for loop'
  QHash<QString,ClassInfo*> parents;
  parents["oyStruct_s"] = structClassInfo;
  for (int c=0; c<allClassesInfo.size(); c++)
    parents[allClassesInfo.at( c )->name()] = allClassesInfo.at( c );

  // b) Set the parent pointer of each class
  for (int c=0; c<allClassesInfo.size(); c++) {
    allClassesInfo.at( c )->setParent( parents[allClassesInfo.at( c )->parentName()] );
    allClassesInfo.at( c )->setContent( parents[allClassesInfo.at( c )->listOf()] );
  }
}

ClassTemplates::~ClassTemplates()
{
  delete structClassInfo;
  delete nullClassInfo;
  for (int i=0; i<allClassesInfo.size(); i++)
    delete allClassesInfo.at( i );
}

void ClassTemplates::createSources() const
{
  for (int i=0; i<allClassesInfo.size(); i++) {
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
}

void ClassTemplates::createTemplates() const
{
  QDir templateDir( templates );
  templateDir.setFilter( QDir::Files | QDir::Readable );
  for (int i=0; i<allClassesInfo.size(); i++) {
    if (!allClassesInfo.at(i)->createTemplates()) {
      qDebug() << "Skipping template files for" << allClassesInfo.at(i)->baseName();
      continue;
    }

    //Create the templates/<group_name>/ directory, if not present
    QString group( allClassesInfo.at(i)->group() );
    if (!templateDir.exists( group ))
      if (!templateDir.mkdir( group )) {
        qWarning() << "Could not create directory" << group;
        continue;
      }

    QString catchBase;
    QStringList genericTemplateFiles;
    if (allClassesInfo.at(i)->hiddenStruct()) { //Use the "hidden struct" API
      if (allClassesInfo.at(i)->listOf() == "") { //Create a simple object
        qDebug() << "Creating template files for simple object" << allClassesInfo.at(i)->baseName();
        genericTemplateFiles << "Class_s.h" << "Class_s.c"
                             << "Class_s_.h" << "Class_s_.c";
        catchBase = "Base(_s_?)\\.([ch])";
      } else { //Create a list of objects
        qDebug() << "Creating template files for list of" << allClassesInfo.at(i)->listOf() << "objects";
        genericTemplateFiles << "Class_s.list.h" << "Class_s.list.c"
                             << "Class_s_.list.h" << "Class_s_.list.c";
        catchBase = "BaseList(_s_?)\\.([ch])";
      }
    } else { //Use the "opaque pointer" API
      genericTemplateFiles << "Class_s.opaque.h" << "Class_s.opaque.c"
                           << "Class_s_impl.opaque.h" << "Class_s_impl.opaque.c";
      catchBase = "Base(_s(?:_impl){0,1})\\.opaque\\.([ch])";
    }

    for (int g=0; g<genericTemplateFiles.size(); g++) {
      QString oldTemplateFile = genericTemplateFiles.at( g );
      QString newTemplateFile = QString( oldTemplateFile ).
                                remove( ".opaque" ).
                                remove( ".list" ).
                                replace( '.', ".template." ).
                                replace( "Class", allClassesInfo.at(i)->baseName() );
      QFile newFile( templates + "/" + group + "/" + newTemplateFile );
      QFile oldFile( templates + "/" + oldTemplateFile );
      if (!newFile.exists()) {
        if (!newFile.open( QIODevice::WriteOnly|QIODevice::Text )) {
          qWarning() << "Could not open file" << newFile.fileName() << "for writting";
          continue;
        }
        if (!oldFile.open( QIODevice::ReadOnly|QIODevice::Text )) {
          qWarning() << "Could not open file" << oldFile.fileName() << "for reading";
          continue;
        }
        QString fileData = oldFile.readAll();
        fileData.replace( QRegExp("([Ii]nclude\\s+\")Class\\."),
                          "\\1" + allClassesInfo.at(i)->baseName() + "." );

        // If our parent is not oyStruct_s, replace Base* with parent template file
        if (allClassesInfo.at(i)->parentBaseName() != "Struct")
          fileData.replace( QRegExp(catchBase),
                            allClassesInfo.at(i)->parentBaseName() + "\\1" + ".template." + "\\2" );

        newFile.write( fileData.toAscii() );

        qDebug() << "  Creating file" << newFile.fileName() << "from" << oldFile.fileName();
      } else {
        qDebug() << "  Skipping file" << newFile.fileName();
      }
    }
  }
}

QList<QVariant> ClassTemplates::getAllClasses() const
{
  QVariantList classes;
  for (int c=0; c<allClassesInfo.size(); c++)
    classes << QVariant( QVariant::fromValue( static_cast<QObject*>(allClassesInfo.at( c )) ) );

  return classes;
}

QVariant ClassTemplates::getStructClass() const
{
  return QVariant( QVariant::fromValue( static_cast<QObject*>(structClassInfo) ) );
}

QList<ClassInfo*> ClassTemplates::getNewClasses() const
{
  QList<ClassInfo*> newClassesInfo;
  for (int i=0; i<allClassesInfo.size(); i++) {
    if (allClassesInfo.at(i)->doxOnly())
      newClassesInfo << allClassesInfo.at(i);
  }

  return newClassesInfo;
}
