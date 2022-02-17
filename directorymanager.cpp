#include "directorymanager.h"
#include <QDebug>
#include <QThread>

DirectoryManager::DirectoryManager()
{

}


DirectoryManager::~DirectoryManager()
{

}


bool DirectoryManager::lookForDir(QString dir_path,QList<ReceivingFileActions*> settings_2, QString junk_path)
{
    if(monitored_dirs.contains(dir_path))
        return false;
    else
    {
        DirectoryMonitor *new_monitor = new DirectoryMonitor();
        new_monitor->setDirectory(dir_path);

        EventExecutor *new_receiveng_file_executor = new EventExecutor();

        new_receiveng_file_executor->setExecutionSettings(settings_2);

        new_receiveng_file_executor->setJunkDir(junk_path);

        new_monitor->setExecutor(new_receiveng_file_executor);

        QThread *thread = new QThread();

        connect(thread,         &QThread::started,              new_monitor,    &DirectoryMonitor::run);
        connect(new_monitor,    &DirectoryMonitor::finished,    thread,         &QThread::terminate);


        new_monitor->moveToThread(thread);
        thread->start();

        return true;
    }
}
