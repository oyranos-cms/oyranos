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
        if(a.length() == 0)
            a = QProcess::readAllStandardError();
        return a;
    }
};
