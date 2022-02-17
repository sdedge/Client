#include "workthread.h"






workThread::workThread(QObject *parent) : QObject(parent)
{

}




//==============================================================================================================================================================
//функция возврата состояния потока (активен/отключен)
//==============================================================================================================================================================
bool workThread::running() const
{
    return m_running;
}




//==============================================================================================================================================================
//процедура основной работы потока
//==============================================================================================================================================================
void workThread::run()
{

    while(m_running)                                                                                                //пока поток активен
    {
        while(m_working)                                                                                            //пока есть "работа"
        {



            if(data.type == 14)                                                                                     //копирование файла
            {
                QString msg = "";
                QString status = copy(data.target, data.source);                                                    //вызов функции копирования
                if(status == "0")
                {
                    msg = "Файл скопирован из " + data.source + " в "+ data.target;
                }
                else if(status == "1")
                {
                    msg = "Невозможно скопировать файл. По указанному пути " + data.source + " файл отсутствует";
                }

                qDebug() << msg;


                QEventLoop loop;
                QTimer timer;
                timer.setInterval(100); // 100 msec
                connect (&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
                timer.start();
                loop.exec();


                DataPack answerDataPack;                                                                            //создание и заполнение пакета-ответа
                answerDataPack.time = QTime::currentTime();
                answerDataPack.type = 10;
                QHostInfo hostInfo;
                answerDataPack.name_PC = hostInfo.localHostName();
                answerDataPack.attributes << status;
                emit sendDataPack(answerDataPack);                                                                  //отправка пакета-ответа
                setWorking(false);                                                                                  //завершение "работы" потока
            }





            else if (data.type == 12)                                                                               //запуск программы
            {
                QString msg = "";
                QString status = process(data.application_path, data.attributes);                                   //вызов функции запуска программы
                if(status == "0")
                {
                    msg = " Приложение " + data.application_path + " запустилось и отработало";
                }
                else if(status == "1")
                {
                    msg = " Приложение  " + data.application_path  + " завершилось с ошибкой";
                }
                qDebug() << msg;


                QEventLoop loop;
                QTimer timer;
                timer.setInterval(100); // 1 sec
                connect (&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
                timer.start();
                loop.exec();


                //QString outmessage = "следующее действие";
                DataPack answerDataPack;                                                                            //создание и заполнение пакета-ответа
                answerDataPack.time = QTime::currentTime();
                answerDataPack.type = 10;
                QHostInfo hostInfo;
                answerDataPack.name_PC = hostInfo.localHostName();
                answerDataPack.attributes << status;
                emit sendDataPack(answerDataPack);                                                                  //отправка пакета-ответа
                setWorking(false);                                                                                  //завершение "работы" потока
            }




            else if(data.type == 16)                                                                                //удаление файла
            {
                QString msg = "";
                QString status = del(data.target);                                                                  //вызов функции удаления файла
                if(status == "0")
                {
                    msg = "Файл " + data.target + " удален";
                }
                else if(status == "1")
                {
                    msg = "Ошибка удаления файла "+data.target + ".";
                }

                qDebug() << msg;


                QEventLoop loop;
                QTimer timer;
                timer.setInterval(100); // 1 sec
                connect (&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
                timer.start();
                loop.exec();


                //QString outmessage = "следующее действие";
                DataPack answerDataPack;                                                                            //создание и заполнение пакета-ответа
                answerDataPack.time = QTime::currentTime();
                answerDataPack.type = 10;
                QHostInfo hostInfo;
                answerDataPack.name_PC = hostInfo.localHostName();
                answerDataPack.attributes << status;
                emit sendDataPack(answerDataPack);                                                                  //отправка пакета-ответа
                setWorking(false);                                                                                  //завершение "работы" потока
            }

            else                                                                                                    //пришел необрабатывемый тип
            {
                qDebug() << "Получен пакет команды с некорректным типом" << data.type;
                setWorking(false);
            }

        }
        QEventLoop loop;
        QTimer timer;
        timer.setInterval(100); // 1 sec
        connect (&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        timer.start();
        loop.exec();

    }
    qDebug() << "Завершение обработки.";
}




//==============================================================================================================================================================
//функция копирования
//==============================================================================================================================================================
QString workThread::copy(QString target, QString source)
{
    QString statusString = "1";                                                                                     //возврат ошибки по-умолчанию
    if(QFile::exists(source))                                                                                       //если исходный файл существует
    {
        if(QFile::exists(target))                                                                                   //если конечный файл уже существует
        {
            if(QFile::remove(target))                                                                               //если предварительное удаление прошло успешно
            {
                if(QFile::copy(source, target))                                                                     //если копирование удалось
                    statusString = "0";                                                                             //копирование прошло успешно
            }
        }
        else                                                                                                        //если конечный файл НЕ существует
        {
            if(QFile::copy(source, target))                                                                         //если копирование удалось
                statusString = "0";                                                                                 //копирование прошло успешно
        }
    }
    return statusString;
}





//==============================================================================================================================================================
//функция удаления
//==============================================================================================================================================================
QString workThread::del(QString target)
{
    QString statusString = "1";                                                                                     //возврат ошибки по-умолчанию
    if(QFile::remove(target) || (!QFile::exists(target)))                                                           //если удалось удалить файл или файл не существует
    {
        statusString = "0";                                                                                         //удаление прошло успешно
    }
    return statusString;
}




//==============================================================================================================================================================
//процедура запуска/остановки потока
//==============================================================================================================================================================
void workThread::setRunning(bool running)
{
    m_running = running;
}



//==============================================================================================================================================================
//процедура запуска/остановки работы потока
//==============================================================================================================================================================
void workThread::setWorking(bool working)
{
    m_working = working;
}




//==============================================================================================================================================================
//слот получения пакета от сервера для обработки в потоке
//==============================================================================================================================================================
void workThread::receiveData(DataPack in_data)
{
    data = in_data;
}






//==============================================================================================================================================================
//функция запуска приложения
//==============================================================================================================================================================
QString workThread::process(QString exePath, QStringList args)
{
    QString statusString = "1";                                                                                     //возврат ошибки по-умолчанию
    QFileInfo fileInfo(exePath);
    //emit sendMsgToPrint(exePath, m_number);
    if(fileInfo.exists())                                                                                           //если файл по указанному пути существует
    {
        if(fileInfo.permission(QFile::ExeUser))                                                                     //если файл может быть запущен текущим пользователем
        {
            QProcess process;
            process.start(exePath, args);
            process.waitForFinished(-1);
            statusString = QString::number(process.exitCode());                                                     //возврат кода завершения

        }
    }
    return statusString;
}

