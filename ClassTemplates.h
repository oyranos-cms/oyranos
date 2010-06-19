#ifndef CLASSTEMPLATES_H
#define CLASSTEMPLATES_H

#include "ClassInfo.h"

class QString;
class QStringList;

class ClassTemplates
{
  public:
    ClassTemplates( const QString& src, const QString& tpl )
      :updateTemplates(false), sources(src), templates(tpl)
    { findClasses(); }
    ~ClassTemplates();

    void createTemplates();

    bool updateTemplates;           ///< Not only create, but update all .*template*. files, too

  private:
    QString sources,                  ///< Directory that holds the sources
            templates;                ///< Directory that holds the templates
    QStringList doxOnlyClasses,       ///< List of classes having only a .dox file (currently unused)
                allClasses;           ///< List of all classes in sources directory
    QList<ClassInfo*> allClassesInfo; ///< Info list of all classes in sources directory
    static QStringList sourceFiles;   ///< List of possible source files in sources directory

    void findClasses();
};

#endif //CLASSTEMPLATES_H
