#ifndef WORKTHREAD_H
#define WORKTHREAD_H


#include <QDebug>
#include <QTextStream>
#include <QFile>
#include <QXmlStreamReader>
#include <QProcess>
#include <QStringList>
#include <QString>
#include <QDir>
#include <QEventLoop>
#include <QTimer>
#include <QTime>
#include <QObject>
#include <QCryptographicHash>
#include <QDataStream>
#include <QLockFile>
#include <QHostInfo>


#include "datapack.h"



class workThread : public QObject
{   
    Q_OBJECT
    bool m_running = false;
    bool m_working = false;
    DataPack data;
    int blockSize;
    unsigned long long sizeReceiveData = 0;
    unsigned long long sizeSendedData = 0;
public:
    explicit workThread(QObject *parent = 0);
    QString process(QString exePath, QStringList args);
    QString copy(QString pathTarget, QString pathSource);
    QString del (QString pathTarget);
    bool running() const;
    int outType;
    QString outmessage;


signals:
    void finished();
    void sendDataPack(DataPack data);
    void sendBlock(QByteArray outBlock);
    void unlock(QString fileName);
    void newLocker(QString fileName);
    void sendMsgToPrint(QString msg);

public slots:
    void run();
    void setRunning(bool running);
    void setWorking(bool working);
    void receiveData(DataPack in_data);
};


#endif // WORKTHREAD_H
