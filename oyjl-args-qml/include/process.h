#include <QProcess>
#include <QVariant>

class Process : public QProcess {
    Q_OBJECT

public:
    Process(QObject *parent = 0) : QProcess(parent) { }

    Q_INVOKABLE void start(const QString &program, const QVariantList &arguments)
    {
        QStringList args;

        for( int i = 0; i < arguments.length(); i++ )
            args << arguments[i].toString();

        QProcess::start(program, args);
    }

    Q_INVOKABLE void waitForFinished()
    {
        QProcess::waitForFinished();
    }

    Q_INVOKABLE QByteArray readAll()
    {
        QByteArray a = QProcess::readAllStandardOutput();
        // test for PNG and convert to base64 for QML::Image::source Url
        if(a.length() > 16 && (int)a[0] == -119 && (int)a[1] == 80 && (int)a[2] == 78 && (int)a[3] == 71 && (int)a[4] == 13 && (int)a[5] == 10 && (int)a[6] == 26 && (int)a[7] == 10)
        {
            QString s = QString("data:image/png;base64,") + a.toBase64();
            a = s.toUtf8();
        }

        if(a.length() == 0)
            a = QProcess::readAllStandardError();
        return a;
    }
};
