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
  Q_PROPERTY(bool isVoid READ isVoid)
  Q_PROPERTY(bool isStatic READ isStatic)
  Q_PROPERTY(QString args READ args)
  Q_PROPERTY(QString argNames READ argNames)
  Q_PROPERTY(QString argTypes READ argTypes)
  Q_PROPERTY(QStringList argList READ argList)
  Q_PROPERTY(QString argsCPP READ argsCPP)
  Q_PROPERTY(QString argNamesCPP READ argNamesCPP)
  Q_PROPERTY(QString argTypesCPP READ argTypesCPP)

  public:
    FuncInfo( const QString& className, const QString& prototype ) :
      m_static(false), m_valid(true), m_classBaseName(className)
    {
      parsePublicPrototype( prototype );

      if (m_valid)
        cppTransformations();
    }

    /// Get the function name
    QString name() const { return m_name; }
    /// Get the function return type
    QString returnType() const { return m_returnType; }
    /// Check if function is void
    bool isVoid() const { return m_returnType == "void" ? true : false; }
    /// Check if function is a static function
    bool isStatic() const { return m_static; }
    /// Get the function arguments (comma separated)
    QString args() const { return m_arguments.join(", "); }
    /// Get the function argument names (comma separated)
    QString argNames() const { return m_argumentNames.join(", "); }
    /// Get the function argument types (comma separated)
    QString argTypes() const { return m_argumentTypes.join(", "); }
    /// Get the function arguments as a list
    QStringList argList() const { return m_arguments; }
    /// Get the function arguments (comma separated) C++
    QString argsCPP() const { return m_cppArguments.join(", "); }
    /// Get the function argument names (comma separated) C++
    QString argNamesCPP() const { return m_cppArgumentNames.join(", "); }
    /// Get the function argument types (comma separated) C++
    QString argTypesCPP() const { return m_cppArgumentTypes.join(", "); }

    bool isValid() const { return m_valid; }

    static QVariantList getPublicFunctions( const ClassInfo* classInfo );
    static const QString public_regexp_tmpl;
    static const QString public_regexp_static_tmpl;

  private:
    bool m_static;                  ///< Is this a static function?
    bool m_valid;                   ///< Is this function parsed correctly?
    QString m_name;                 ///< The name of the function, without the oyClass_ prefix
    QString m_returnType;           ///< The function return type
    QString m_classBaseName;        ///< The name of the function, without the oyClass_ prefix
    QStringList m_arguments;        ///< A list of all function arguments
    QStringList m_argumentNames;    ///< A list of all function argument names
    QStringList m_argumentTypes;    ///< A list of all function argument types
    QStringList m_cppArguments;     ///< A list of all function arguments for C++
    QStringList m_cppArgumentNames; ///< A list of all function argument names for C++
    QStringList m_cppArgumentTypes; ///< A list of all function argument types for C++

    void parsePublicPrototype( const QString& prototype );
    void cppTransformations();
};

#endif // FUNC_INFO_H
