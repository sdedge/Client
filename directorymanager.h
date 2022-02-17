#ifndef DIRECTORYMANAGER_H
#define DIRECTORYMANAGER_H


#include "directorymonitor.h"
#include <QObject>
#include <QStringList>
#include <QDir>


class DirectoryManager : public QObject
{
    Q_OBJECT
public:
    explicit DirectoryManager();
    ~DirectoryManager();

    bool lookForDir(QString dir_path,  QList<ReceivingFileActions *> settings_2, QString junk_path);


signals:


private:
    QStringList monitored_dirs; 



};

#endif // DIRECTORYMANAGER_H
