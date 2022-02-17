#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QPlainTextEdit>
#include "settingsparsers.h"
#include "settings.h"

MainWindow* MainWindow::m_ptr = nullptr;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_ptr = this;



    work_thread = nullptr;
    send_file = nullptr;
    receive_file = nullptr;
    ui->plainTextEdit->setReadOnly(true);
}




MainWindow::~MainWindow()
{
    delete ui;
}


MainWindow* MainWindow::getPtr()
{
    return m_ptr;
}

void MainWindow::initSystem()
{
    Settings::settings();
    connect(Settings::settings(), &Settings::newText, this, &MainWindow::printOnPlainTextEdit);

    tcp_socket = new QTcpSocket;

    QString main_settings_path = QApplication::applicationDirPath() + "/settings/settings_6.xml";

    SettingsParsers::parseSettings6(main_settings_path);

    QString other_settings_path = "";

    if(Settings::settings()->searchVar("settings_dir_path") == nullptr)
        return;
    else
        other_settings_path = Settings::settings()->searchVar("settings_dir_path")->value;

    SettingsParsers::parseSettings1(other_settings_path + "settings_1.xml");


    connect(tcp_socket,  SIGNAL(readyRead()),                                this,   SLOT(onSokReadyRead()));
    connect(tcp_socket,  SIGNAL(connected()),                                this,   SLOT(slotConnected()));
    connect(this,        SIGNAL(sendIP(QHostAddress,int,QString, QString)),  this,   SLOT(slotConnectToServer(QHostAddress,int, QString, QString)));
    connect(tcp_socket,  SIGNAL(error(QAbstractSocket::SocketError)),        this,   SLOT(slotError(QAbstractSocket::SocketError)));

    startMsgBoxConfig();
}


//==============================================================================================================================================================
//окно предложения подключении с предыдущими настройками
//==============================================================================================================================================================
void MainWindow::startMsgBoxConfig()
{

    QFile file(QApplication::applicationDirPath() + "/config.txt");
    if(file.exists())
    {
        if(file.open(QIODevice::ReadOnly))
        {
            QTextStream in(&file);
            QHostAddress ip(in.readLine());
            int nPort = in.readLine().toInt();
            QString group = in.readLine();
            QString role = in.readLine();
            if((group != "") && (role != ""))
            {
                QMessageBox msgBox;
                msgBox.setWindowTitle("Сообщение");
                msgBox.setText(tr("Подключиться к управляющему диспетчеру "
                                  "с предыдущими настройками?"));
                QAbstractButton* pButtonYes = msgBox.addButton(tr("Да"), QMessageBox::YesRole);
                QAbstractButton* pButtonNo = msgBox.addButton(tr("Нет"), QMessageBox::NoRole);
                msgBox.exec();

                if(msgBox.clickedButton() == pButtonYes)
                {
                    emit sendIP(ip, nPort, group, role);
                }
                if(msgBox.clickedButton() == pButtonNo)
                {
                    msgBox.close();
                }
            }
        }
    }
}




//==============================================================================================================================================================
//слот нажатия на кнопку "подключиться к серверу"
//==============================================================================================================================================================
void MainWindow::on_connect_to_server_button_clicked()
{
    IPConf *ipconf = new IPConf();
    ipconf->show();
    connect(ipconf,SIGNAL(sendIP(QHostAddress,int,QString, QString)),this,SLOT(slotConnectToServer(QHostAddress,int, QString, QString)));
}





//==============================================================================================================================================================
//слот подключения к серверу
//==============================================================================================================================================================
void MainWindow::slotConnectToServer(QHostAddress adress,int port, QString newGroup, QString NewRole)
{
    tcp_socket->connectToHost(adress,port);
    group = newGroup;
    role  = NewRole;
}




//==============================================================================================================================================================
//слот отображения ошибок подключения к серверу (из клиента диспетчера состояния)
//==============================================================================================================================================================
void MainWindow::slotError(QAbstractSocket::SocketError err)
{
    QString strError = tcp_socket->peerAddress().toString() + "  " +
    "  Ошибка: " +
               (err == QAbstractSocket::HostNotFoundError ?
                    "Сервер не найден." :
                err == QAbstractSocket::RemoteHostClosedError ?
                    "Отключен от сервера." :
                err == QAbstractSocket::ConnectionRefusedError ?
                    "Сервер отклонил запрос подключения." :
                err == QAbstractSocket::SocketAccessError ?
                    "Недостаточно прав для создания сокета." :
                err == QAbstractSocket::SocketResourceError ?
                    "Не удалось создать ещё один сокет (слишком много активных сокетов)." :
                err == QAbstractSocket::SocketTimeoutError ?
                    "Время операции истекло." :
                err == QAbstractSocket::DatagramTooLargeError ?
                    "Слишком большой обьем входящих/исходящих данных." :
                err == QAbstractSocket::NetworkError ?
                    "Неполадки в сети." :
                err == QAbstractSocket::AddressInUseError ?
                    "Данный адрес уже используется." :
                err == QAbstractSocket::SocketAddressNotAvailableError ?
                    "Данный адрес не принадлежит хосту." :
                err == QAbstractSocket::UnsupportedSocketOperationError ?
                    "Требуемый сокет не поддерживается операционной системой." :
                err == QAbstractSocket::UnfinishedSocketOperationError ?
                    "Сервер не найден."   :
                err > 11 ?
                    "Неизвестная ошибка":
    QString(tcp_socket->errorString())
    );
    qDebug() << strError;
}




//==============================================================================================================================================================
//слот идентификации клиента при подключении к серверу
//==============================================================================================================================================================
void MainWindow::slotConnected()
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock,QIODevice::WriteOnly);
    DataPack data;                                                                                                      //создание пакета с информацией,
    data.type = 19;                                                                                                     //идентифицирующей клиента
    data.time = QTime::currentTime();
    QHostInfo hostInfo;
    data.name_PC = hostInfo.localHostName();
    data.message = group + "/" + role;                                                                                  //передача рабочей группы и роли клиента
    out<<quint32(0) << data;
    out.device()->seek(0);
    out<<quint32(arrBlock.size()-sizeof(quint32));
    tcp_socket->write(arrBlock);
    tcp_socket->flush();
    QString str = "Соединение с сервером установлено";
    qDebug() << str;                                                                                                    //отображение сообщения
    str.clear();
}




//==============================================================================================================================================================
//передача пакетов серверу
//==============================================================================================================================================================
void MainWindow::sendDataPackToServer(DataPack data)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock,QIODevice::WriteOnly);
    out<<quint32(0) << data;
    out.device()->seek(0);
    out<<quint32(arrBlock.size()-sizeof(quint32));
    tcp_socket->write(arrBlock);
    tcp_socket->flush();
}






//==============================================================================================================================================================
//слот получения пакетов от сервера
//==============================================================================================================================================================
void MainWindow::onSokReadyRead()
{
    QDataStream in(tcp_socket);
    DataPack data;
    QByteArray block = 0;
    for(;;)                                                                                                             //цикл ожидания пакета
    {
        if(!block_size)
        {
            if(tcp_socket->bytesAvailable() < sizeof(quint32))
            {
                break;
            }
            in >> block_size;
        }

        if(tcp_socket->bytesAvailable() < block_size)
        {
            break;
        }

        in >> data;                                                                                                     //запись полного пакета


        if(data.type == 10)                                                                                             //Проверка готовности потока на клиенте
        {
            if(work_thread == nullptr)                                                                                  //..если поток ещё не создан
            {
                work_thread = new workThread();                                                                         //создание объекта класса обработки в потоке
                QThread *thread = new QThread();                                                                        //создание потока
                qRegisterMetaType <DataPack>("DataPack");
                connect(thread,     &QThread::started,              work_thread,  &workThread::run);
                connect(work_thread,  &workThread::finished,          thread,     &QThread::terminate);
                connect(work_thread,  &workThread::sendDataPack,      this,       &MainWindow::sendDataPackToServer);
                work_thread->moveToThread(thread);                                                                      //помещение объекта в поток
                work_thread->setRunning(true);                                                                          //установка флага продолжения работы потока
                thread->start();                                                                                        //запуск потока в режиме ожидания работы
                receive_file = nullptr;
            }

                QByteArray arrBlock;                                                                                    //сюда пишется структура-ответ серверу через QDataStream
                QDataStream out(&arrBlock,QIODevice::WriteOnly);                                                        //тот самый QDataStream
                DataPack answer_pack;                                                                                   //объект структуры-ответа
                answer_pack.time = QTime::currentTime();
                QHostInfo hostInfo;
                answer_pack.type = 10;
                answer_pack.name_PC = hostInfo.localHostName();
                answer_pack.attributes << "0";
                out<<quint32(0) << answer_pack;
                out.device()->seek(0);
                out<<quint32(arrBlock.size()-sizeof(quint32));
                tcp_socket->write(arrBlock);                                                                            //передача серверу сообщения о готовнисти к работе
                tcp_socket->flush();
        }
        else if(data.type == 8)                                                                                         //завершение работы
        {
            work_thread->setRunning(false);

            if(receive_file != 0)
            {
                if(receive_file->exists())
                {
                    if(global_file_lokers.contains(receive_file->fileName()))
                    {
                        receive_file->remove();
                        delLocker(receive_file->fileName());
                    }
                }
                receive_file = nullptr;
            }

            send_file = nullptr;
            current_received_file_size = 0;
            full_received_file_size = 0;
        }

        else if(data.type == 11)                                                                                        //получение файла от сервера
        {
            if(receive_file == nullptr)                                                                                 //если файл не в процессе получения
            {

                QString path = data.target.left(data.target.lastIndexOf("/"));
                QDir check(path);
                if(check.mkpath(path))                                                                                  //проверка существования указанного пути и
                {                                                                                                       //попытка его создания при необходимости
                    receive_file = new QFile(data.target);
                    if(!receive_file->open(QIODevice::WriteOnly))                                                       //если файл не удалось открыть
                    {
                        qDebug() << "Не удалось получить файл.";
                        receive_file = 0;                                                                               //отмена приема файла
                    }
                    else                                                                                                //если файл удалось открыть
                    {
                        emit addLocker(data.target);                                                                    //"блокировка" получаемого файла
                        current_received_file_size = 0;                                                                 //обнуление количества полученных байт файла
                        full_received_file_size = data.file_size;                                                       //запись полного размера получаемого файла

                    }
                }
            }
            DataPack answer_data_pack;                                                                                  //создание пакета-ответа
            answer_data_pack.time = QTime::currentTime();
            QHostInfo hostInfo;
            answer_data_pack.name_PC = hostInfo.localHostName();
            if(receive_file != 0)                                                                                       //если файл находится в процессе получения
            {
                block.clear();                                                                                          //очистка буфера
                in >> block;                                                                                            //потоковое чтение получаемой части файла
                block.remove(block_size - sizeof(data), block.size() - (block_size - sizeof(data)));                    //удаление из буфера "мусора", считанного из потока
                current_received_file_size += block.size();                                                             //уточнение полученных байт файла
                receive_file->write(block);                                                                             //запись содержимого буфера в файл


                if(current_received_file_size == full_received_file_size)                                               //если файл получен полностью
                {
                    receive_file->close();                                                                              //закрыть файл
                    emit delLocker(data.target);                                                                        //разблокировать файл
                    QString message = "Файл " + data.target + " получен.";
                    qDebug() << message;
                    answer_data_pack.type = 10;                                                                         //пакет-ответ оповещает о том,
                    answer_data_pack.attributes << "0";                                                                 //что файл получен полностью
                    current_received_file_size = 0;
                    delete receive_file;
                    receive_file = nullptr;
                }
                else                                                                                                    //если файл получен НЕ полностью
                {
                    qDebug() << "send 9";
                    answer_data_pack.type = 9;                                                                          //пакет-ответ, оповещает сервер о том, что часть файла
                    answer_data_pack.target = data.target;                                                              //получена, и клиент готов к приему следующей части
                }

            }
            else
            {
                QString message = "Файл " + data.target + " не найден или не может быть открыт. Отмена приема файла.";
                qDebug() << message;
                answer_data_pack.type = 10;                                                                             //пакет-ответ оповещает о том,
                answer_data_pack.attributes << "1";                                                                     //что файл НЕ получен
            }
            sendDataPackToServer(answer_data_pack);                                                                     //отправка пакета-ответа
        }



        else if(data.type == 13)                                                                                        //инициализация отправки файла на сервер
        {
            if(send_file == nullptr)                                                                                    //если файл НЕ в процессе отправки
            {                                                                                                           //инициализация отправки файла с проверками
                QString path = data.source;
                send_file = new QFile(path);
                if(send_file->exists())                                                                                 //проверка на существование файла
                {
                    if(!send_file->open(QIODevice::ReadOnly))                                                           //проверка возможности открытия файла
                    {
                        send_file = nullptr;
                        QString message = "Файл " + data.source + " не может быть открыт. Отмена отправки файла.";
                        qDebug() << message;
                    }
                }
                else
                {
                    send_file = nullptr;
                    QString message = "Файл " + data.source + " не найден. Отмена отправки файла.";
                    qDebug() << message;
                }
            }
            if(send_file != nullptr)                                                                                    //если файл в процессе отправки
            {                                                                                                           //или инициализация прошла успешно
                if (tcp_socket->isOpen())
                {
                    tcp_socket->waitForBytesWritten();//возможно не нужно
                    sendPartOfFile(data);                                                                               //отправить часть файла
                }
                else                                                                                                    //никогда не должен сюда попасть,
                {                                                                                                       //пусть останется, на всякий случай
                    QString message = "Соединение потеряно, сокет закрыт. Остановка процесса отправки файла";
                    qDebug() << message;
                }
            }
            else
            {
                QString message = "Передаваемый файл " + data.source + " не найден или не может быть открыт. Отмена передачи файла.";
                qDebug() << message;
                DataPack answer_data_pack;                                                                              //создание пакета-ответа
                answer_data_pack.time = QTime::currentTime();
                QHostInfo hostInfo;
                answer_data_pack.name_PC = hostInfo.localHostName();
                answer_data_pack.type = 10;                                                                             //пакет-ответ оповещает о том,
                answer_data_pack.attributes << "1";                                                                     //что файл НЕ получен
                sendDataPackToServer(answer_data_pack);                                                                 //отправка пакета-ответа
            }
        }






        else if(data.type == 9)                                                                                         //отправка части файла на сервер
        {
            sendPartOfFile(data);
        }



        else                                                                                                            //передача пакетов оставшихся типов в поток
        {                                                                                                               //для дальнейшей обработки
            work_thread->receiveData(data);
            work_thread->setWorking(true);                                                                              //возобновление работы потока
        }
        block_size = 0;
    }
}








//==============================================================================================================================================================
//слот отправки части файла на сервер
//==============================================================================================================================================================
void MainWindow::sendPartOfFile(DataPack data)
{
    DataPack header;
    header.time = QTime::currentTime();
    header.type = 13;
    header.target = data.target;
    header.source = data.source;
    header.file_size = send_file->size();
    char block[2000000] = {0};
    QByteArray block1 = 0;
    QByteArray buf;
    QDataStream out(&block1, QIODevice::WriteOnly);
    if(!send_file->atEnd())
    {
        qint64 in = send_file->read(block, sizeof(block));
        buf = buf.fromRawData(block, sizeof(block));
        buf.truncate(in);

        out << quint32(0);
        out << header;
        out << buf;

        out.device()->seek(0);
        out <<quint32(block1.size() - sizeof(quint32));
        if (tcp_socket->isOpen())
        {
            tcp_socket->write(block1);
            if(send_file->atEnd())                                                                                      //если была отправлена последняя часть файла
            {
                send_file->close();
                delete send_file;
                send_file = nullptr;
                DataPack answer_data_pack;                                                                              //создание и заполнение пакета-ответа
                answer_data_pack.time = QTime::currentTime();
                answer_data_pack.type = 10;
                QHostInfo hostInfo;
                answer_data_pack.name_PC = hostInfo.localHostName();
                answer_data_pack.attributes << "0";
                sendDataPackToServer(answer_data_pack);                                                            //отправка пакета-ответа
            }
        }
        else                                                                                                            //никогда не должен сюда попасть,
        {                                                                                                               //пусть останется, на всякий случай
            QString message = "Соединение потеряно, сокет закрыт. Остановка процесса отправки файла";
            qDebug() << message;
        }

    }
    else                                                                                                                //никогда не должен сюда попасть,
    {                                                                                                                   //пусть останется, на всякий случай
        QString message = "Ошибка указателя на отправляемый файл. Остановка процесса отправки файла";
        qDebug() << message;
    }
}


//==============================================================================================================================================================
//слот кнопки, дублирующей действие "подключиться к серверу"
//зачем???
//оставлен без изменений
//==============================================================================================================================================================
void MainWindow::on_action_IP_triggered()
{
    IPConf *ipconf = new IPConf();
    ipconf->show();
    connect(ipconf,SIGNAL(sendIP(QHostAddress,int, QString)),this,SLOT(slotConnectToServer(QHostAddress,int,QString)));
}




//==============================================================================================================================================================
//слот блокировки файла до завершения его получения от сервера
//==============================================================================================================================================================
void MainWindow::addLocker(QString fileName)
{
    global_file_lokers << fileName;
}




//==============================================================================================================================================================
//слот разблокировки файла по завершении его получения от сервера
//==============================================================================================================================================================
void MainWindow::delLocker(QString fileName)
{
    global_file_lokers.removeOne(fileName);
}



//==============================================================================================================================================================
//слот отображения текстовых сообщений
//==============================================================================================================================================================
void MainWindow::printOnPlainTextEdit(QString msg)
{
    ui->plainTextEdit->appendPlainText(msg);
}
