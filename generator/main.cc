#include <iostream>

#include <QDir>

#include <QtDebug>

#include "ClassGenerator.h"

#define TEMPALTE_DIR "templates"
#define SOURCE_DIR "sources"
#define API_DIR "API_generated"

using namespace std;


int main(int argc, char *argv[])
{
  if (QString(argv[1]) == "-h" ||
      QString(argv[1]) == "--help") {
    cout << "Usage: " << argv[0] << " [template dir]" << " [sources dir]" << " [output dir]" << endl;
    return 0;
  }
  QDir templateDir, sourceDir, outputDir;
  templateDir.setPath( argc > 1 ? argv[1] : TEMPALTE_DIR );
  sourceDir.setPath  ( argc > 2 ? argv[2] : SOURCE_DIR   );
  outputDir.setPath  ( argc > 3 ? argv[3] : API_DIR      );

  if (!templateDir.exists()) {
    qCritical() << "Directory" << templateDir.path() << "does not exist";
    return 1;
  }
  if (!sourceDir.exists()) {
    qCritical() << "Directory" << sourceDir.path() << "does not exist";
    return 1;
  }
  if (!outputDir.exists()) {
    qDebug() << "Creating directory" << outputDir.path();
    QDir currentDir;
    if (!currentDir.mkdir(outputDir.path())) {
      qCritical() << "Directory" << outputDir.path() << "cannot be created";
      return 1;
    }
  }

  ClassGenerator cg( templateDir.path(), sourceDir.path(), outputDir.path() );

  cg.initTemplates();
  qDebug() << "";
  cg.render();

  return 0;
}
