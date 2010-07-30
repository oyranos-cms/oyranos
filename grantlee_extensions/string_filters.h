#ifndef STRING_FILTERS_H
#define STRING_FILTERS_H

#include <grantlee/filter.h>

using namespace Grantlee;

class CamelcaseToUnderscoreFilter : public Filter
{
  public:
    QVariant
      doFilter( const QVariant &input, const QVariant &argument = QVariant(), bool autoescape = false ) const;

  bool isSafe() const {
    return true;
  }
};

#endif // STRING_FILTERS_H
