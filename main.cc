#include <iostream>
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
   if (argc < 3) {
      cout << "Usage: " << argv[0] << " <class base name>" << "[template name]" << endl;
      return 0;
   }
   QString class_base_name(argv[1]);
   QString template_file(argv[2]);

   Grantlee::Engine *engine = getEngine();
   Grantlee::Template t = engine->loadByName( template_file );
   Grantlee::Context c;
   c.insert( "class_name", class_base_name );
   cout << t->render( &c ).toUtf8().data();

   return 0;
}
