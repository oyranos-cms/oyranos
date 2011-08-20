#include <QVariant>
#include <QtDebug>
#include <grantlee/util.h>

#include "string_filters.h"


QVariant
CamelcaseToUnderscoreFilter::doFilter( const QVariant& input, const QVariant& argument, bool autoescape ) const
{
  Q_UNUSED( autoescape )

  QString inputString = getSafeString( input );
  QString arg = getSafeString( argument );
  QString under;

  bool strict = false;
  if (arg == "strict")
    strict = true;

  QChar pre, cur;
  int idx = 1;
  bool upword = false;
  while (idx < inputString.size()) {
    pre = inputString.at(idx-1);
    cur = inputString.at(idx);
    if (!pre.isLetterOrNumber() || !cur.isLetterOrNumber()) {
      under += pre;
      upword = false;
    } else if (((pre.isLower() || pre.isNumber()) && cur.isLower()) || cur.isNumber()) {
      under += pre;
    } else if (pre.isLower() && cur.isUpper()) {
      under += pre;
      under += "_";
    } else if (pre.isUpper() && cur.isUpper()) {
      under += pre;
      upword = true;
    } else if (pre.isUpper() && cur.isLower()) {
      if (strict) {
        under += upword ? "_" : "";
        pre = pre.toLower();
        under += pre;
      } else {
        under += pre;
        under += upword ? "_" : "";
      }
      upword = false;
    }
    idx++;
  }
  under += cur;

  return markSafe( under );
}

QVariant
trFilter::doFilter( const QVariant& input, const QVariant& argument, bool autoescape ) const
{
  Q_UNUSED( autoescape )

  QString inputString = getSafeString( input );
  QString arg = getSafeString( argument );

  if (arg.size() != 3 || arg.at(1) != ' ') {
    qWarning() << "Argument" << arg << arg.size()<< "to tr filter has wrong format";
    return markSafe( inputString );
  }

  return markSafe( inputString.replace( arg.at(0), arg.at(2) ) );
}
