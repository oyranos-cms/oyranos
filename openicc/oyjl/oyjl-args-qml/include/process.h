#include <QProcess>
#include <QThread>
#include <QVariant>
#include <QFile>
#include <QTemporaryFile>
#include <QIODevice>
#include <oyjl.h>
#include <oyjl_macros.h>
#include <unistd.h>
#include <errno.h>

extern int (*processCallback_p)(int argc, const char ** argv);

class Thread : public QThread
{
    Q_OBJECT

    int argc_;
    const char ** argv_;

public:
    Thread( ) { }
    Thread( int argc, const char ** argv ) {argc_=argc;argv_=argv;};
    ~Thread( ) { }

    void setData( int argc, const char ** argv ) {argc_=argc;argv_=argv;};
signals:
    void finishedSignal();

protected:
    void run() { processCallback_p(argc_,argv_); emit finishedSignal(); };
};


class Process : public QProcess {
    Q_OBJECT

    QString tempName_;
    QByteArray a_cb;
    QByteArray e_cb;
    FILE * fm_cb;
    FILE * fme_cb;
    int saved_stdout = -1;
    int saved_stderr = -1;
    Thread thread_cb;

public:
    Process(QObject *parent = 0) : QProcess(parent) {
        fm_cb = fme_cb = NULL;
        connect( &thread_cb, SIGNAL(finishedSignal()), this, SLOT( setData()));
    }

    Q_INVOKABLE void start(const QString &program, const QVariantList &arguments , const QString &mark)
    {
        QStringList args;

        fprintf(stderr, "%s ", program.toLocal8Bit().data() );
        for( int i = 0; i < arguments.length(); i++ )
        {
            fprintf(stderr, "%s ", arguments[i].toString().toLocal8Bit().data() );
            args << arguments[i].toString();
        }
        fprintf(stderr, "\n" );

        if(processCallback_p != NULL)
        {
            //fprintf(stderr, "starting callback %s\n", program.toUtf8().data());
            QString tempName;
            {
                QTemporaryFile temp;
                if (temp.open()) {
                    tempName_ = temp.fileName() + "2" + mark;
                }
            }
            char * tfn = oyjlStringCopy( tempName_.toUtf8().data(), malloc );
            fm_cb = fopen( tfn, "w+" );
            int mf_fd_cb = fileno(fm_cb);
            int stdout_fd = fileno(stdout);
            saved_stdout = dup(STDOUT_FILENO);
            if(dup2( mf_fd_cb, stdout_fd ) == -1)
            {
              fprintf(stderr, "mf_fd_cb: %d stdout_fd: %d %s\n", mf_fd_cb, stdout_fd, strerror(errno));
            }

            char * tefn = oyjlStringCopy( (tempName_ + "-err").toUtf8().data(), malloc );
            fme_cb = fopen( tefn, "w+" );
            int mfe_fd_cb = fileno(fme_cb);
            int stderr_fd = fileno(stderr);
            saved_stderr = dup(STDERR_FILENO);
            if(dup2( mfe_fd_cb, stderr_fd ) == -1)
            {
              fprintf(stderr, "mfe_fd_cb: %d stderr_fd: %d %s\n", mfe_fd_cb, stderr_fd, strerror(errno));
            }

            int count = arguments.length();
            char ** argv = (char**) calloc( count+2, sizeof(char*) );
            argv[0] = oyjlStringCopy( program.toUtf8().data(), malloc );
            for( int i = 0; i < count; ++i )
              argv[1+i] = oyjlStringCopy( args[i].toUtf8(), malloc );

            thread_cb.setData( 1+count, (const char**)argv );
            thread_cb.start();
            free(tfn);

        } else
          QProcess::start(program, args);
    }

    Q_INVOKABLE void waitForFinished()
    {
        QProcess::waitForFinished();
    }

    Q_INVOKABLE QByteArray readAll()
    {
        QByteArray a;
        e_cb = QProcess::readAllStandardError();
        if(processCallback_p != NULL)
        {
            //fprintf(stderr, "read callback %d\n", (int)a_cb.size());
            a = a_cb;
        } else
            a = QProcess::readAllStandardOutput();
        // test for PNG and convert to base64 for QML::Image::source Url
        if(a.length() > 16 && (unsigned char)a[0] == 137 && (char)a[1] == 80 && (char)a[2] == 78 && (char)a[3] == 71 && (char)a[4] == 13 && (char)a[5] == 10 && (char)a[6] == 26 && (char)a[7] == 10)
        {
            QString s = QString("data:image/png;base64,") + a.toBase64();
            a = s.toUtf8();
        }
        // test for SVG
        if(a.length() > 16)
        {
            QString t = a;
            if((t.startsWith( "<?xml version=\"1.0\"") && t.contains( "<svg" )) ||
                t.startsWith( "<svg" ))
            {
                QString s = QString("data:image/svg+xml;utf8,") + a;
                a = s.toUtf8();
            }
        }

        if(a.length() == 0)
        {
          if(processCallback_p != NULL)
          {
            a = a_cb; // not implemented; would need stderr
          } else
            a = e_cb;
        }
        return a;
    }
    Q_INVOKABLE QByteArray readErr()
    {
        return e_cb;
    }
public Q_SLOTS:
    void setData()
    {
      char * cfn = oyjlStringCopy( tempName_.toUtf8().data(), malloc );
      if(fm_cb == NULL)
      {
          fprintf(stderr, "no result in %s\n", cfn);
          return;
      }
      fflush(fm_cb);
      fflush(stdout); // stdout is bufferd
      fclose(fm_cb); fm_cb = NULL;
      fflush(fme_cb);
      fflush(stderr); // stderr is bufferd
      fclose(fme_cb); fme_cb = NULL;
      QFile f( tempName_ );
      f.open(QIODevice::ReadOnly|QIODevice::Unbuffered);
      qint64 size = f.size();
      a_cb = f.read(size);
      f.close();
      FILE * fp = fopen(cfn,"rb");
      if(!fp)
      {
          fprintf(stderr, OYJL_DBG_FORMAT "filed to open: %s\n", OYJL_DBG_ARGS, cfn );
          return;
      }
      fseek(fp, 0, SEEK_END);
      size = ftell(fp);
      f.remove();
      //fprintf(stderr, OYJL_DBG_FORMAT "read: %s %d %d\n", OYJL_DBG_ARGS, cfn, (int)size, (int)a_cb.size() );

      QFile fe( tempName_ + "-err" );
      fe.open(QIODevice::ReadOnly|QIODevice::Unbuffered);
      qint64 esize = fe.size();
      e_cb = fe.read(esize);
      fe.close();
      if(size == 0 && a_cb.size() == 0)
          a_cb = e_cb;
      fe.remove();

      // restore stdout
      if(saved_stdout >= 0)
        dup2(saved_stdout, STDOUT_FILENO);
      if(saved_stderr >= 0)
        dup2(saved_stderr, STDERR_FILENO);

      emit readChannelFinished();
      free(cfn);
    }
};
