#include <QFile>
#include <QString>
#include <QStringList>
#include <QRegExp>

#include <QtDebug>

int main(int argc, char** argv)
{
  QString haystack;
  if (argc == 1) { //No arguments -> Print usage
    qWarning() << "Usage:" << argv[0] << "\"a QRegExp string\"" << "[Text File]";
    return 0;
  } else if (argc == 2) { //1 argument -> read from stdin
    QTextStream stream(stdin);
    haystack = stream.readAll();
  } else { //2 arguments -> open file as argv[2]
    QFile file( argv[2] );
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      haystack = file.readAll();
    } else {
      qWarning() << "Could not open file" << argv[2] << "for reading.";
      return 2;
    }
  }

  QString needle( argv[1] );
  QRegExp findNeedle(needle);
  if (findNeedle.indexIn( haystack ) != -1) {
    qDebug() << findNeedle.capturedTexts();
  } else {
    qWarning() << "No match found";
    return 1;
  }

  return 0;
}
