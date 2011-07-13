#ifndef CLASSTEMPLATES_H
#define CLASSTEMPLATES_H

#include "ClassInfo.h"

class QString;
class QStringList;
class QVariant;

class ClassTemplates
{
  public:
    ClassTemplates( const QHash<QString,QString>& dirs );
    ~ClassTemplates();

    //Create the empty sources/* files for all present classes.
    //But only if these files do not exist (we do not overwrite)
    void createSources() const;

    /// Create the template files in templates/ for all present classes,
    /// if these files do not already exist.
    /// We do not overwrite already existing template files.
    /// Additionally we ignore special classes using the [notemplates]
    /// tag (like oyStruct_s and oyObject_s) because their templates are hand-written
    void createTemplates() const;

    /// Return a QVariantList for use with grantlee
    QList<QVariant> getAllClasses() const;

    /// Return a QVariant for use with grantlee
    QVariant getStructClass() const;

    /// Return a list of all 'object system' Classes information
    QList<ClassInfo*> getStdClasses() const;

    /// Return all classes that only have a .dox file
    QList<ClassInfo*> getNewClasses() const;

    /// Return a constant reference of the templates directory map
    const QHash<QString,QString>& getDirMap() const { return dirMap; }

  private:
    QHash<QString,QString> dirMap;          ///< Maps the 'template' <-> 'sources' directories
    ClassInfo* structClassInfo;             ///< Info for the oyStruct_s class is kept here
    ClassInfo* nullClassInfo;               ///< Info for the null class is kept here
    QList<ClassInfo*> allClassesInfo;       ///< Info list of all classes in sources directory
    static const QStringList sourceFiles;   ///< List of source file types for each class
};

#endif //CLASSTEMPLATES_H
