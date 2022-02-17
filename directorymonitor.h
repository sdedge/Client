#ifndef DIRECTORYMONITOR_H
#define DIRECTORYMONITOR_H

#include <QObject>
#include <QDir>
#include <QTimer>
#include <QEventLoop>
#include <eventexecutor.h>

class DirectoryMonitor : public QObject
{
    Q_OBJECT
public:
    explicit    DirectoryMonitor(QObject *parent = nullptr);

    virtual     ~DirectoryMonitor();

    void        setDirectory    (QString monitored_dir);

    void        run             ();

    void        lockFile        (QString file_path);

    void        unlockFile      (QString file_path);

    void        stopEventLoop   ();

    void        setExecutor     (EventExecutor *executor);


signals:

    void        getSomething    (QString fileName);
    void        finished        ();

private:
    QString m_monitored_dir;
    QStringList locked_files;
    const unsigned int ms_interval = 50;

    EventExecutor *m_executor;
};

#endif // DIRECTORYMONITOR_H
