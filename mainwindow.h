#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "workthread.h"
#include "ipconf.h"

#include <QMainWindow>
#include <QTcpSocket>
#include <QTime>
#include <QThread>
#include <QDataStream>
#include <QFileSystemWatcher>
#include <QFile>
#include <QHostInfo>
#include <QMessageBox>
#include <QXmlStreamReader>
#include <QMap>



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QTcpSocket* tcp_socket;
    QString     group;
    QString     role;
    quint32     block_size = 0;
    static MainWindow* getPtr();
    void initSystem();

public slots:
    void slotConnected();
    void onSokReadyRead();
    void slotError(QAbstractSocket::SocketError);
    void sendDataPackToServer(DataPack data);
    void slotConnectToServer(QHostAddress, int, QString newGroup, QString);
    void startMsgBoxConfig();
    void printOnPlainTextEdit(QString msg);
signals:
    void sendFileName(QString srt);
    void readyForNextPart(DataPack data);
    void sendIP(QHostAddress adress, int port, QString group, QString role);

private slots:
    void on_connect_to_server_button_clicked();

    void on_action_IP_triggered();
    void addLocker(QString fileName);
    void delLocker(QString fileName);
    void sendPartOfFile(DataPack data);

private:
    Ui::MainWindow *ui;
    static MainWindow *m_ptr;
    QString tempPath;
    workThread* work_thread;
    QFile* send_file;
    QFile* receive_file;
    int current_received_file_size;
    int full_received_file_size;
    QStringList global_file_lokers;


};

#endif // MAINWINDOW_H
