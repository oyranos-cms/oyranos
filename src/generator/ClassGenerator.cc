#include <QDir>
#include <QDirIterator>
#include <QDateTime>
#include <QVariant>
#include <QStringList>

#include <QtDebug>

#include "config.h"
#include "ClassGenerator.h"

const QStringList ClassGenerator::templateSuffixes(
    QStringList() <<
    "*.template.h" <<
    "*.template.hh" <<
    "*.template.c" <<
    "*.template.cc" <<
    "*.template.py" <<
    "*.template.txt"
);

ClassGenerator::ClassGenerator( const QHash<QString,QString>& dirs, const QString& dst ) :
  destinationPath(dst), tpl( dirs )
{
  // Setup grantlee
  QStringList alldirs;
  QHash<QString,QString>::const_iterator i;
  for (i = dirs.constBegin(); i != dirs.constEnd(); i++)
    alldirs << i.key() << i.value();

  engine = getEngine( alldirs );

  classes = tpl.getAllClasses();

  // Set tmplPath for easy retrieval of 'template path' from 'template name'
  for (i = dirs.constBegin(); i != dirs.constEnd(); i++) {
    QDirIterator templateFile( i.key(), templateSuffixes,
                               QDir::Files|QDir::Readable,
                               QDirIterator::Subdirectories );
    while (templateFile.hasNext()) {
      templateFile.next();
      if (tmplPath.contains(templateFile.fileName()))
        qCritical() << "Duplicate template file:"
                    << tmplPath[templateFile.fileName()]
                    << templateFile.filePath();
      tmplPath[templateFile.fileName()] = templateFile.filePath();
    }
  }

  // Set basePath also, for Base_* template files
  QDirIterator baseTemplateFile( TEMPLATES_STD_DIR ,
                                 QStringList() << "Base*.[ch]"
                                               << "Base.cc"
                                               << "Base.hh" );
  while (baseTemplateFile.hasNext()) {
    baseTemplateFile.next();
    basePath[baseTemplateFile.fileName()] = baseTemplateFile.filePath();
  }
}

ClassGenerator::~ClassGenerator()
{
  delete engine;
}

void ClassGenerator::initTemplates()
{
  //Check for newly added classes and...
  //create missing templates
  qDebug() << "#### Create missing template files ##############################";
  tpl.createTemplates();

  //create missing source files
  qDebug() << "#### Create missing source files ################################";
  tpl.createSources();

  //create missing constructor/destructor source files
  qDebug() << "#### Create missing constructor/destructor source files #########";

  const QList<ClassInfo*>& stdClasses = tpl.getStdClasses();
  QString classRendered;
  QString genericTemplate = TEMPLATES_CLASS_PATH "/Class_s_private_custom_definitions.c";
  for (int i=0; i<stdClasses.size(); i++) { //TODO Render in RAM
    QString classTemplate = stdClasses.at(i)->srcDir() +
                            "/" +
                            stdClasses.at(i)->name() +
                            ".template.c";
    QString classSource = stdClasses.at(i)->srcDir() +
                          "/" +
                          stdClasses.at(i)->baseName() +
                          ".private_custom_definitions.c";

    if (QFile::exists(classSource) && QFileInfo(classSource).size() > 0) {
      qDebug() << "Skipping file" << classSource;
      continue;
    }

    if (QFile::copy( genericTemplate, classTemplate )) {
      classRendered = renderFile( classTemplate, stdClasses.at(i)->srcDir() );
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

QString ClassGenerator::renderFile( const QFileInfo& templateFileInfo )
{
  return renderFile( templateFileInfo, destinationPath );
}

QString ClassGenerator::renderFile( const QFileInfo& templateFileInfo, const QString& dstDir )
{
  Grantlee::Template t = engine->loadByName( templateFileInfo.fileName() );
  Grantlee::Context c;

  QVariant classinfo;
  QString class_name = templateFileInfo.baseName();
  if (class_name == "oyStruct_s") {
    classinfo = tpl.getStructClass();
  } else {
    int i;
    for (i=0; i<classes.size(); i++) {
      if (class_name == classes.at( i ).value<QObject*>()->property("name").toString() ||
          class_name == classes.at( i ).value<QObject*>()->property("privName").toString() ||
          class_name == classes.at( i ).value<QObject*>()->property("cppName").toString()) {
        classinfo = classes.at( i );
        break;
      }
    }
    if (i == classes.size()) {
      qDebug() << templateFileInfo.fileName() << "is not a base class file.";
      classinfo = QVariant::fromValue( static_cast<QObject*>(new ClassInfo( "Null", "", "" )) );
    }
  }

  QString sourceName = templateFileInfo.baseName() + "." + templateFileInfo.suffix();
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
  status = sourceFile.exists() ? "Updating" : "Creating";
  sourceFile.open(QIODevice::WriteOnly | QIODevice::Text);
  sourceFile.write( newFileContents.toUtf8() );
  qDebug() << status << sourceFile.fileName() << '\n'
           << "`-->[" << templateFileInfo.absoluteFilePath() << "]";

  return sourceFile.fileName();
}

void ClassGenerator::render()
{
    // Render each template file to destinationPath
    QHash<QString,QString>::const_iterator t;
    for (t = tmplPath.constBegin(); t != tmplPath.constEnd(); t++)
      renderFile( QFileInfo(t.value()) , destinationPath );
}

void ClassGenerator::getTemplateParents( const QString& path, QVariantList& parentList )
{
  QFile tmpl( path );
  tmpl.open( QIODevice::ReadOnly|QIODevice::Text );
  QString text = tmpl.readAll();

  QRegExp extends("\\{%\\s+extends\\s+\"((?:\\w+\\.?)+)\"\\s+%\\}");

  if (extends.indexIn( text ) != -1) {
    QString tmplParentName = extends.cap(1);
    parentList << tmplParentName;

    if (tmplPath.contains(tmplParentName))
      getTemplateParents( tmplPath[tmplParentName], parentList );
    else if (basePath.contains(tmplParentName))
      getTemplateParents( basePath[tmplParentName], parentList );
    else
    {
      qWarning() << "Could not find template" << tmplParentName;
      QHash<QString,QString>::const_iterator t;
      for (t = tmplPath.constBegin(); t != tmplPath.constEnd(); t++)
        qWarning() << "Searched in: " << t.value();
    }
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
  qDebug() << "";

  Grantlee::Engine *engine = new Grantlee::Engine();
  auto loader = QSharedPointer<GFSLoader>::create();
  loader->setTemplateDirs( allTmplDirs );

  engine->addTemplateLoader( loader );
  engine->addPluginPath( "." );
  return engine;
}
