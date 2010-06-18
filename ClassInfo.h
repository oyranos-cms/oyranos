#include <QObject>

class QString;

class ClassInfo: public QObject
{
  QOBJECT
  Q_PROPERTY(QString name READ name)
  Q_PROPERTY(QString privName READ privName)
  Q_PROPERTY(QString baseName READ baseName)
  Q_PROPERTY(QString parentBaseName READ parentBaseName)
  Q_PROPERTY(QString parentName READ parentName)
  Q_PROPERTY(QString group READ group)
  Q_PROPERTY(bool internal READ internal)

  public:
    ClassInfo( const QString& name, const QString& dir )
      : base(name), directory(dir), isInternal(false)
    {
      parseDoxyfile();
    }

    /// Get the class full public name
    QString name() const { return "oy" + baseName + "_s"; }
    /// Get the class full private name
    QString privName() const { return "oy" + baseName + "_s_"; }
    /// Get the class name without any prefix/suffix
    QString baseName() const { return base; }
    /// Get the baseName of the parent class
    QString parentBaseName() const { return parentBase; }
    /// Get the full name of the parent class
    QString parentName() const { return "oy" + parentBase + "_s"; }
    /// Get the group this class belongs to
    QString group() const { return groupName; }
    /// True if this is an internal(not public) class
    bool internal() const { return isInternal; }
    //TODO: Return the parent class

  private:
    QString base;             ///< The class name without any prefix/suffix
    QString parentBase;       ///< The base name of the parent class
    QString groupName;        ///< The group this class belongs to
    QString directory;        ///< Where the class source files live
    bool isInternal;          ///< True if this is an internal(not public) class

    void parseDoxyfile();
};
