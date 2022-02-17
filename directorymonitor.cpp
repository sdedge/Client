#include "directorymonitor.h"

#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QStringList>


extern QFile *log;
extern QTextStream stream;


DirectoryMonitor::DirectoryMonitor(QObject *parent) : QObject(parent)
{

}

DirectoryMonitor::~DirectoryMonitor()
{
    delete m_executor;
}


void DirectoryMonitor::lockFile(QString file_path)
{
    if(!locked_files.contains(file_path))
        locked_files.append(file_path);
}



void DirectoryMonitor::unlockFile(QString file_path)
{
    if(locked_files.contains(file_path))
        locked_files.removeOne(file_path);
}


void DirectoryMonitor::setExecutor(EventExecutor *executor)
{
    m_executor = executor;
}



void DirectoryMonitor::setDirectory(QString monitored_dir)
{
    m_monitored_dir = monitored_dir;
}



void DirectoryMonitor::run()
{
    if(m_executor == nullptr)
    {
        qDebug() << "Настройки обработки не заданы. Отслеживание директории " + m_monitored_dir + " невозможно";
        return;
    }

    QDir inDir(m_monitored_dir);


    while(1)
    {

        inDir.refresh();                                                                //обновление содержимого директории

        QStringList files = inDir.entryList(QDir::Filter::NoDotAndDotDot|QDir::Filter::AllEntries, QDir::SortFlag::NoSort);
        foreach(QString file_name, files)
        {
            QFile *new_file = new QFile(m_monitored_dir + file_name);
            if(new_file->exists())
            {
                m_executor->getCurrentVariables();
                if(m_executor->executeFileReceiving(file_name) != true)
                {
                    qDebug() << "Ошибка при обработке файла " + file_name;
                }
            }
            else
            {
                qDebug() << "Ошибка. Обнаруженный файл отсутствует";
            }
        }
        QEventLoop loop;                                                                //
        QTimer timer;                                                                   //
        timer.setInterval(ms_interval);                                                 //   небольшая задержка, чтобы не грузить ЦП
        connect (&timer, &QTimer::timeout, &loop, &QEventLoop::quit);                   //
        timer.start();                                                                  //
        loop.exec();                                                                    //
    }
}
