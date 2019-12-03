#ifndef CUSTOM_PLUGIN_LIBRARY_H
#define CUSTOM_PLUGIN_LIBRARY_H

#include <grantlee/taglibraryinterface.h>

#include "string_filters.h"

class GSoCPluginLibrary : public QObject, public Grantlee::TagLibraryInterface
{
  Q_OBJECT
  Q_INTERFACES( Grantlee::TagLibraryInterface )
  Q_PLUGIN_METADATA(IID "org.grantlee.gsoc_extensions")

  public:
    GSoCPluginLibrary( QObject *parent = 0 )
      : QObject( parent ) {}

    QHash<QString, Grantlee::Filter*> filters( const QString &name = QString() ) {
      Q_UNUSED( name );

      QHash<QString, Grantlee::Filter*> filters;

      filters.insert( "underscores", new CamelcaseToUnderscoreFilter() );
      filters.insert( "tr", new trFilter() );

      return filters;
    }
};

#endif // CUSTOM_PLUGIN_LIBRARY_H
