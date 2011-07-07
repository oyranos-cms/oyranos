#include <QDir>
#include <QDirIterator>
#include <QDateTime>
#include <QVariant>
#include <QStringList>

#include <QtDebug>

#include "grantlee_paths.h"
#include "ClassGenerator.h"

const QStringList ClassGenerator::templateSuffixes(
    QStringList() <<
    "*.template.h" <<
    "*.template.c" <<
    "*.template.cc" <<
    "*.template.txt"
);

ClassGenerator::ClassGenerator( const QString& tmpl, const QString& src, const QString& dst ) :
  templatesPath(tmpl), sourcesPath(src), destinationPath(dst),
  tpl( sourcesPath, templatesPath )
{
  //Setup grantlee
  engine = getEngine( QStringList() << templatesPath << sourcesPath );

  classes = tpl.getAllClasses();
}

ClassGenerator::~ClassGenerator()
{
  delete engine;
}

void ClassGenerator::initTemplates()
{
  //Check for newly added classes and...
  //create missing templates
  tpl.createTemplates();
  //create missing source files
  tpl.createSources();

  //create missing constructor/destructor source files
  QList<ClassInfo*> newClassesInfo = tpl.getNewClasses();
  QString classRendered;
  QString genericTemplate = templatesPath + "/Class_s_private_custom_definitions.c";
  for (int i=0; i<newClassesInfo.size(); i++) {
    // If this is a list class, ignore it
    if (not newClassesInfo.at(i)->listOf().isEmpty())
      continue;
    QString classTemplate = sourcesPath +
                            "/" +
                            newClassesInfo.at(i)->baseName() +
                            "_s.template.c";
    QString classSource = sourcesPath +
                          "/" +
                          newClassesInfo.at(i)->baseName() +
                          ".private_custom_definitions.c";
    if (QFile::copy( genericTemplate, classTemplate )) {
      classRendered = render( classTemplate, sourcesPath );
    } else {
      qWarning() << "Could not create file" << classTemplate;
      continue;
    }
    if (not QFile::remove( classTemplate ))
      qWarning() << "Could not remove file" << classTemplate;
    if (not QFile::rename( classRendered, classSource ))
      qWarning() << "Could not rename file" << classRendered;
  }
}

QString ClassGenerator::render( const QString& templateFile, const QString& dstDir )
{
  return render( QFileInfo( templateFile ), dstDir );
}

QString ClassGenerator::render( const QString& templateFile )
{
  return render( templateFile, destinationPath );
}

QString ClassGenerator::render( const QFileInfo& templateFileInfo, const QString& dstDir )
{
  Grantlee::Template t = engine->loadByName( templateFileInfo.fileName() );
  Grantlee::Context c;

  QString class_base_name = templateFileInfo.baseName();
  int idx = class_base_name.indexOf("_");
  class_base_name.truncate(idx);

  QVariant classinfo;
  QString sourceName, oy;
  if (class_base_name == "Struct") {
    classinfo = tpl.getStructClass();
    oy = "oy";
  } else {
    int i;
    for (i=0; i<classes.size(); i++) {
      if (class_base_name == classes.at( i ).value<QObject*>()->property("baseName").toString()) {
        classinfo = classes.at( i );
        oy = "oy";
        break;
      }
    }
    if (i == classes.size())
      qDebug() << templateFileInfo.fileName() << "is not a base class file.";
  }
  sourceName = oy + templateFileInfo.baseName() + "." + templateFileInfo.suffix();
  QFile sourceFile( dstDir + "/" + sourceName );
  QFileInfo sourceFileInfo(sourceFile);

  // Get the template file parent list (do not confuse with class parent list)
  QVariantList parents;
  parents << templateFileInfo.fileName();
  getTemplateParents( templateFileInfo.filePath(), parents );

  c.insert( "file_name", sourceName );
  c.insert( "classes", classes );
  c.insert( "class", classinfo );
  c.insert( "struct", tpl.getStructClass() );
  c.insert( "parents", parents);

  QString newFileContents = t->render( &c );

  QString status;
  sourceFile.exists() ? status = "Updating" : "Creating";
  sourceFile.open(QIODevice::WriteOnly | QIODevice::Text);
  sourceFile.write( newFileContents.toUtf8() );
  qDebug() << status << sourceFile.fileName() << '\n'
           << "`-->[" << templateFileInfo.absoluteFilePath() << "]";

  return sourceFile.fileName();
}

void ClassGenerator::render()
{
  QDirIterator templateFiles( templatesPath,
               templateSuffixes,
               QDir::Files|QDir::Readable,
               QDirIterator::Subdirectories );

  while (templateFiles.hasNext()) {
    templateFiles.next();
    QFileInfo templateFileInfo = templateFiles.fileInfo();

    render( templateFileInfo, destinationPath );
  }
}

void ClassGenerator::getTemplateParents( const QString& tmplPath, QVariantList& parentList )
{
  QFile tmpl( tmplPath );
  tmpl.open( QIODevice::ReadOnly|QIODevice::Text );
  QString text = tmpl.readAll();

  QRegExp extends("\\{%\\s+extends\\s+\"((?:\\w+\\.?)+)\"\\s+%\\}");

  if (extends.indexIn( text ) != -1) {
    QString tmplParentName = extends.cap(1);
    parentList << tmplParentName;

    QString tmplParentPath;
    QDirIterator templateFile( templatesPath, QDirIterator::Subdirectories );
    while (templateFile.hasNext()) {
      templateFile.next();
      if (templateFile.fileName() == tmplParentName) {
        tmplParentPath = templateFile.filePath();
        break;
      }
    }
    if (!tmplParentPath.isEmpty())
      getTemplateParents( tmplParentPath, parentList );
    else
      qWarning() << "Could not find template" << tmplParentName;
  }
}

typedef Grantlee::FileSystemTemplateLoader GFSLoader;

Grantlee::Engine* ClassGenerator::getEngine( const QStringList& tmplDirs )
{
  QStringList allTmplDirs( tmplDirs );

  //Get all template subdirectories
  foreach (QString dir, tmplDirs) {
    QDirIterator subdir( dir,
                         QDir::AllDirs|QDir::NoDotAndDotDot|QDir::Readable|QDir::Executable,
                         QDirIterator::Subdirectories );
    while (subdir.hasNext()) {
      subdir.next();
      allTmplDirs << dir + "/" + subdir.fileName();
    }
  }
  qDebug() << "Loading templates from:";
  qDebug() << allTmplDirs;

  Grantlee::Engine *engine = new Grantlee::Engine();
  GFSLoader::Ptr loader = GFSLoader::Ptr( new GFSLoader() );
  loader->setTemplateDirs( allTmplDirs );

  engine->addTemplateLoader( loader );
  engine->setPluginPaths( QStringList() << "." << GRANTLEE_PLUGIN_PATH );
  return engine;
}
