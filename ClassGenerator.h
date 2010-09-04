#ifndef CLASS_GENERATOR_H
#define CLASS_GENERATOR_H

#include <QFileInfo>
#include <QVariantList>
#include <grantlee_core.h>

#include "ClassTemplates.h"

class ClassGenerator {
  public:
    ClassGenerator( const QString& tmpl, const QString& src, const QString& dst );
    ~ClassGenerator();

    void setTemplateDir( const QString& dir );
    void setSourceDir( const QString& dir );
    void setDestinationDir( const QString& dir );

    void initTemplates();
    void render();
    void render( const QString& templateFile );
    void render( const QString& templateFile, const QString& dstDir );

  private:
    void getTemplateParents( const QString& tmplPath, QVariantList& parentList );
    Grantlee::Engine* getEngine( const QStringList& tmplDirs );
    void render( const QFileInfo& templateFileInfo, const QString& dstDir );

    QString templatesPath, sourcesPath, destinationPath;
    Grantlee::Engine* engine;
    ClassTemplates tpl;
    QVariantList classes;

    static const QStringList templateSuffixes;
};

#endif //CLASS_GENERATOR_H
