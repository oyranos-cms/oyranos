#ifndef CLASSTEMPLATES_H
#define CLASSTEMPLATES_H

#include "ClassInfo.h"

class QString;
class QStringList;
class QVariant;

class ClassTemplates
{
  public:
    ClassTemplates( const QString& src, const QString& tpl )
      :updateTemplates(false), sources(src), templates(tpl)
    { allClassesInfo = ClassInfo::getAllClasses( sources ); }
    ~ClassTemplates();

    void createTemplates();
    /// Return a QVariantList for use with grantlee
    QList<QVariant> getAllClasses();

    bool updateTemplates;           ///< Not only create, but update all .*template*. files, too

  private:
    QString sources,                  ///< Directory that holds the sources
            templates;                ///< Directory that holds the templates
    QList<ClassInfo*> allClassesInfo; ///< Info list of all classes in sources directory
    static QStringList sourceFiles;   ///< List of possible source files in sources directory
};

#endif //CLASSTEMPLATES_H
