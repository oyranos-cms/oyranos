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
        if(a.length() && a[0] == 137 && a[1] == 80 && a[2] == 78 && a[3] == 71 && a[4] == 13 && a[5] == 10 && a[6] == 26 && a[7] == 10)
        {
            QString s = QString("data:image/png;base64,") + a.toBase64();
            a = s.toUtf8();
        }

        if(a.length() == 0)
            a = QProcess::readAllStandardError();
        return a;
    }
};
