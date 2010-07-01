#include <QObject>

class QString;

class ClassInfo: public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString name READ name)
  Q_PROPERTY(QString privName READ privName)
  Q_PROPERTY(QString baseName READ baseName)
  Q_PROPERTY(QObject* parent READ parent)
  Q_PROPERTY(QString parentBaseName READ parentBaseName)
  Q_PROPERTY(QString parentName READ parentName)
  Q_PROPERTY(QString group READ group)
  Q_PROPERTY(QString brief READ brief)
  Q_PROPERTY(bool internal READ internal)
  Q_PROPERTY(bool doxOnly READ doxOnly)

  Q_PROPERTY(QString members_h READ members_h)
  Q_PROPERTY(QString private_methods_declarations_h READ private_methods_declarations_h)
  Q_PROPERTY(QString private_methods_definitions_c READ private_methods_definitions_c)
  Q_PROPERTY(QString public_h READ public_h)
  Q_PROPERTY(QString public_methods_declarations_h READ public_methods_declarations_h)
  Q_PROPERTY(QString public_methods_definitions_c READ public_methods_definitions_c)

  public:
    ClassInfo( const QString& name, const QString& dir, bool isnew = false )
      : base(name), directory(dir), isInternal(false), isNew(isnew), m_parent(NULL)
    {
      parseDoxyfile();
      //parseSourceFiles(); FIXME This is not working
    }

    /// Get the class full public name
    QString name() const { return "oy" + base + "_s"; }
    /// Get the class full private name
    QString privName() const { return "oy" + base + "_s_"; }
    /// Get the class name without any prefix/suffix
    QString baseName() const { return base; }
    /// Get the parent class
    QObject* parent() const { return static_cast<QObject*>(m_parent); }
    /// Set the parent class
    void setParent( ClassInfo* ci ) { m_parent = ci; }
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

    /// Get the source file members.h
    QString members_h() const { return base + "." + "members.h"; }
    /// Get the source file private_methods_declarations.h
    QString private_methods_declarations_h() const { return base + "." + "private_methods_declarations.h"; }
    /// Get the source file private_methods_definitions.c
    QString private_methods_definitions_c() const { return base + "." + "private_methods_definitions.c"; }
    /// Get the source file public.h
    QString public_h() const { return base + "." + "public.h"; }
    /// Get the source file public_methods_declarations.h
    QString public_methods_declarations_h() const { return base + "." + "public_methods_declarations.h"; }
    /// Get the source file public_methods_definitions.c
    QString public_methods_definitions_c() const { return base + "." + "public_methods_definitions.c"; }

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
    ClassInfo* m_parent;    ///< A pointer to the parent class info

    void parseDoxyfile();
    void parseSourceFiles();
};
