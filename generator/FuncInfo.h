#ifndef FUNC_INFO_H
#define FUNC_INFO_H

#include <QObject>
#include <QStringList>
#include <QVariant>

#include "ClassInfo.h"

class QString;
class ClassInfo;

class FuncInfo: public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString name READ name)
  Q_PROPERTY(QString returnType READ returnType)
  Q_PROPERTY(QString args READ args)
  Q_PROPERTY(QString argNames READ argNames)
  Q_PROPERTY(QString argTypes READ argTypes)
  Q_PROPERTY(QStringList argList READ argList)

  public:
    FuncInfo( const QString& className, const QString& prototype ) :
      m_classBaseName(className)
    {
      parsePublicPrototype( prototype );
    }

    /// Get the function name
    QString name() const { return m_name; }
    /// Get the function return type
    QString returnType() const { return m_returnType; }
    /// Get the function arguments (comma separated)
    QString args() const { return m_arguments.join(", "); }
    /// Get the function argument names (comma separated)
    QString argNames() const { return m_argumentNames.join(", "); }
    /// Get the function argument types (comma separated)
    QString argTypes() const { return m_argumentTypes.join(", "); }
    /// Get the function arguments as a list
    QStringList argList() const { return m_arguments; }

    static QVariantList getPublicFunctions( const ClassInfo* classInfo );
    static const QString public_regexp_tmpl;

  private:
    QString m_name;              ///< The name of the function, without the oyClass_ prefix
    QString m_returnType;        ///< The function return type
    QString m_classBaseName;     ///< The name of the function, without the oyClass_ prefix
    QStringList m_arguments;     ///< A list of all function arguments
    QStringList m_argumentNames; ///< A list of all function argument names
    QStringList m_argumentTypes; ///< A list of all function argument types

    void parsePublicPrototype( const QString& prototype );
};

#endif // FUNC_INFO_H
