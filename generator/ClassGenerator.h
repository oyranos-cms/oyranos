#ifndef CLASS_GENERATOR_H
#define CLASS_GENERATOR_H

#include <QFileInfo>
#include <QVariantList>
#include <grantlee_core.h>

#include "ClassTemplates.h"

class ClassGenerator {
  public:
    ClassGenerator( const QHash<QString,QString>& dirs, const QString& dst );
    ~ClassGenerator();

    void initTemplates();
    void render();
    QString render( const QString& templateFile );
    QString render( const QString& templateFile, const QString& dstDir );

  private:
    void getTemplateParents( const QString& tmplPath, QVariantList& parentList );
    Grantlee::Engine* getEngine( const QStringList& tmplDirs );
    QString render( const QFileInfo& templateFileInfo, const QString& dstDir );

    QHash<QString,QString> tmplPath; //< Map of 'template name' -> 'template path'
    QHash<QString,QString> basePath; //< Map of 'base template name' -> 'base template path'
    QString destinationPath;
    Grantlee::Engine* engine;
    ClassTemplates tpl;
    QVariantList classes;

    static const QStringList templateSuffixes;
};

#endif //CLASS_GENERATOR_H
