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
      :updateTemplates(false), sources(src), templates(tpl), structClassInfo(new ClassInfo("Struct", src))
    { allClassesInfo = ClassInfo::getAllClasses( sources ); }
    ~ClassTemplates();

    void createTemplates();
    /// Return a QVariantList for use with grantlee
    QList<QVariant> getAllClasses();
    /// Return a QVariant for use with grantlee
    QVariant getStructClass() const;


    bool updateTemplates;           ///< Not only create, but update all .*template*. files, too

  private:
    QString sources,                  ///< Directory that holds the sources
            templates;                ///< Directory that holds the templates
    ClassInfo* structClassInfo;       ///< Info for the oyStruct_s class is kept here
    QList<ClassInfo*> allClassesInfo; ///< Info list of all classes in sources directory
    static QStringList sourceFiles;   ///< List of source file types for each class
};

#endif //CLASSTEMPLATES_H
