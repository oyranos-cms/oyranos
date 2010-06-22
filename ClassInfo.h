#include <QObject>

class QString;

class ClassStruct: public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString name READ name)
  Q_PROPERTY(QString privName READ privName)
  Q_PROPERTY(QString baseName READ baseName)
  Q_PROPERTY(QString parentBaseName READ parentBaseName)
  Q_PROPERTY(QString parentName READ parentName)
  Q_PROPERTY(QString group READ group)
  Q_PROPERTY(bool internal READ internal)
  Q_PROPERTY(bool doxOnly READ doxOnly)

  public:
    ClassStruct() {}

    /// Get the class full public name
    QString name() const { return "oyStruct_s"; }
    /// Get the class full private name
    QString privName() const { return ""; }
    /// Get the class name without any prefix/suffix
    QString baseName() const { return "Struct"; }
    /// Get the baseName of the parent class
    QString parentBaseName() const { return ""; }
    /// Get the full name of the parent class
    QString parentName() const { return ""; }
    /// Get the group this class belongs to
    QString group() const { return "objects_generic"; }
    /// True if this is an internal(not public) class
    bool internal() const { return false; }
    /// True if this is a new class (with only a .dox file)
    bool doxOnly() const { return false; }
    //TODO: Return the parent class
};

class ClassInfo: public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString name READ name)
  Q_PROPERTY(QString privName READ privName)
  Q_PROPERTY(QString baseName READ baseName)
  Q_PROPERTY(QString parentBaseName READ parentBaseName)
  Q_PROPERTY(QString parentName READ parentName)
  Q_PROPERTY(QString group READ group)
  Q_PROPERTY(QString brief READ brief)
  Q_PROPERTY(bool internal READ internal)
  Q_PROPERTY(bool doxOnly READ doxOnly)

  public:
    ClassInfo( const QString& name, const QString& dir, bool isnew = false )
      : base(name), directory(dir), isInternal(false), isNew(isnew)
    {
      parseDoxyfile();
    }

    /// Get the class full public name
    QString name() const { return "oy" + base + "_s"; }
    /// Get the class full private name
    QString privName() const { return "oy" + base + "_s_"; }
    /// Get the class name without any prefix/suffix
    QString baseName() const { return base; }
    /// Get the baseName of the parent class
    QString parentBaseName() const { return parentBase; }
    /// Get the full name of the parent class
    QString parentName() const { return "oy" + parentBase + "_s"; }
    /// Get the group this class belongs to
    QString group() const { return groupName; }
    /// Get the class brief description
    QString brief() const { return doxyBrief; }
    /// True if this is an internal(not public) class
    bool internal() const { return isInternal; }
    /// True if this is a new class (with only a .dox file)
    bool doxOnly() const { return isNew; }
    //TODO: Return the parent class

    static QList<ClassInfo*> getAllClasses( const QString& directory );

  private:
    QString base;           ///< The class name without any prefix/suffix
    QString parentBase;     ///< The base name of the parent class
    QString groupName;      ///< The group this class belongs to
    QString doxyBrief;      ///< The doxygen class brief description
    QString directory;      ///< Where the class source files live
    bool isInternal;        ///< True if this is an internal(not public) class
    bool isNew;             ///< True if this is a new class (with only a .dox file)

    void parseDoxyfile();
};
