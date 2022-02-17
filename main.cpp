#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QScopedPointer>
#include <QDateTime>
#include <QPlainTextEdit>

#include "settings.h"

QScopedPointer<QFile> log_file;


//перенаправление отладочного вывода на интерфейс и в лог-файл
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{

    QTextStream out(log_file.data());

    QString message_text = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + " INFO " + msg + '\n';

    out << message_text;

    Settings::settings()->newText(message_text);

    out.flush();
}


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    QString log_file_path = QApplication::applicationDirPath() + "/log_file.txt";

    log_file.reset(new QFile(log_file_path));

    log_file.data()->open(QFile::Append | QFile::Text);

    qInstallMessageHandler(messageHandler);

    MainWindow w;
    w.show();
    w.initSystem();

    return a.exec();
}
