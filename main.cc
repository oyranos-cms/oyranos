#include <iostream>

#include <QDir>
#include <QStringList>
#include <QFileInfoList>
#include <QTextStream>

#include <grantlee_core.h>
#include "grantlee_paths.h"

using namespace std;

Grantlee::Engine* getEngine()
{
   Grantlee::Engine *engine = new Grantlee::Engine();

   Grantlee::FileSystemTemplateLoader::Ptr loader = Grantlee::FileSystemTemplateLoader::Ptr( new Grantlee::FileSystemTemplateLoader() );
   loader->setTemplateDirs( QStringList() << "templates" );

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

   Grantlee::Engine *engine = getEngine();

   QDir templateDir( argv[1] );
   templateDir.setNameFilters( QStringList() << "*.template.h" << "*.template.c" );
   templateDir.setFilter( QDir::Files | QDir::Readable );
   QFileInfoList templateFiles = templateDir.entryInfoList();
   for (int t=0; t<templateFiles.size(); ++t) {
      QFileInfo file = templateFiles.at(t);

      Grantlee::Template t = engine->loadByName( file.fileName() );
      Grantlee::Context c;
      c.insert( "class_name", file.baseName() );

      QString sourceName = "oy" + file.baseName() + "." + file.suffix();
      QString sourceFilePath = outputDir.filePath( sourceName );
      QFile sourceFile( sourceFilePath );
      sourceFile.open(QIODevice::WriteOnly | QIODevice::Text);
      sourceFile.write( t->render( &c ).toUtf8() );
      clog << "Creating " << sourceFilePath.toUtf8().data() << endl;
   }
   delete engine;

   return 0;
}
