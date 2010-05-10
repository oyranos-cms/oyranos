#include <iostream>

#include <QDir>
#include <QStringList>
#include <QFileInfoList>
#include <QFileInfo>
#include <QTextStream>
#include <QDateTime>

#include <grantlee_core.h>

using namespace std;

Grantlee::Engine* getEngine()
{
   Grantlee::Engine *engine = new Grantlee::Engine();

   Grantlee::FileSystemTemplateLoader::Ptr loader = Grantlee::FileSystemTemplateLoader::Ptr( new Grantlee::FileSystemTemplateLoader() );
   loader->setTemplateDirs( QStringList() << "templates" );

   engine->addTemplateLoader( loader );
   engine->setPluginPaths( QStringList() << "/usr/local/lib/" );
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
      QFileInfo templateFileInfo = templateFiles.at(t);

      Grantlee::Template t = engine->loadByName( templateFileInfo.fileName() );
      Grantlee::Context c;
      c.insert( "class_name", templateFileInfo.baseName() );
      QString newFileContents = t->render( &c );

      QString sourceName = "oy" + templateFileInfo.baseName() + "." + templateFileInfo.suffix();
      QFile sourceFile( outputDir.filePath( sourceName ) );
      QFileInfo sourceFileInfo(sourceFile);

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

   return 0;
}
