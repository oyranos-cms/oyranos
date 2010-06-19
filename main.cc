#include <iostream>

#include <QDir>
#include <QStringList>
#include <QFileInfoList>
#include <QFileInfo>
#include <QTextStream>
#include <QDateTime>
#include <QVariant>

#include <QtDebug>

#include <grantlee_core.h>
#include "grantlee_paths.h"

#include "ClassTemplates.h"

#define TEMPALTE_DIR "templates"
#define SOURCE_DIR "sources"

using namespace std;

Grantlee::Engine* getEngine()
{
   Grantlee::Engine *engine = new Grantlee::Engine();

   Grantlee::FileSystemTemplateLoader::Ptr loader = Grantlee::FileSystemTemplateLoader::Ptr( new Grantlee::FileSystemTemplateLoader() );
   loader->setTemplateDirs( QStringList() << TEMPALTE_DIR << SOURCE_DIR );

   engine->addTemplateLoader( loader );
   engine->setPluginPaths( QStringList() << GRANTLEE_PLUGIN_PATH );
   return engine;
}

int main(int argc, char *argv[])
{
   if (argc < 2) {
      cout << "Usage: " << argv[0] << " <template dir>" << " [output dir]" << endl;
      return 0;
   }
   QDir outputDir( argc > 2 ? argv[2] : QDir::currentPath() );

   //Check for newly added classes and create missing templates
   ClassTemplates tpl( SOURCE_DIR, TEMPALTE_DIR );
   tpl.updateTemplates = true;
   tpl.createTemplates();
   QVariantList classes = tpl.getAllClasses();

   //Setup grantlee
   Grantlee::Engine *engine = getEngine();

   QDir templateDir( argv[1] );
   templateDir.setNameFilters( QStringList() << "*.template.h" << "*.template.c" );
   templateDir.setFilter( QDir::Files | QDir::Readable );
   QFileInfoList templateFiles = templateDir.entryInfoList();
   for (int t=0; t<templateFiles.size(); ++t) {
      QFileInfo templateFileInfo = templateFiles.at(t);

      Grantlee::Template t = engine->loadByName( templateFileInfo.fileName() );
      Grantlee::Context c;

      QString sourceName = "oy" + templateFileInfo.baseName() + "." + templateFileInfo.suffix();
      QFile sourceFile( outputDir.filePath( sourceName ) );
      QFileInfo sourceFileInfo(sourceFile);

      QString class_base_name = templateFileInfo.baseName();
      int idx = class_base_name.indexOf("_");
      class_base_name.truncate(idx);

      c.insert( "class_name", "oy" + templateFileInfo.baseName() );
      c.insert( "class_base_name", class_base_name );
      c.insert( "file_name", sourceName );
      c.insert( "classes", classes );
      for (int i=0; i<classes.size(); i++) {
        if (class_base_name == classes.at( i ).value<QObject*>()->property("baseName").toString()) {
          c.insert( "class", classes.at( i ) );
          break;
        }
      }
      QString newFileContents = t->render( &c );

      //1. There is no source file yet
      if (not sourceFile.exists()) {
        sourceFile.open(QIODevice::WriteOnly | QIODevice::Text);
        sourceFile.write( newFileContents.toUtf8() );
        clog << "Creating " << sourceFile.fileName().toUtf8().data() << endl;
      } else
      //2. The template file is more recent
      if (templateFileInfo.lastModified() > sourceFileInfo.lastModified()) {
        sourceFile.open(QIODevice::WriteOnly | QIODevice::Text);
        sourceFile.write( newFileContents.toUtf8() );
        clog << "Updating " << sourceFile.fileName().toUtf8().data() << endl;
      } else {
        sourceFile.open(QIODevice::ReadOnly | QIODevice::Text);
        QString oldFileContents = sourceFile.readAll();
        //4. The souce file has changed //TODO
        if (oldFileContents != newFileContents)
          cerr << "Warning: " << sourceFile.fileName().toUtf8().data() << " has changed!" << endl;
        else
        //3. There is no difference in file contents -> do nothing
          clog << "Skipping " << sourceFile.fileName().toUtf8().data() << endl;
      }

      //5. Both have changed //TODO
   }
   delete engine;

   return 0;
}
