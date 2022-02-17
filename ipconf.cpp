#include "ipconf.h"
#include "ui_ipconf.h"

IPConf::IPConf(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::IPConf)
{
    ui->setupUi(this);

    QFile file(QApplication::applicationDirPath() + "/config.txt");
    if(file.exists())
    {
        if(file.open(QIODevice::ReadOnly))
        {

            QTextStream in(&file);
            ui->lineEdit->setText(in.readLine());
            ui->lineEdit_2->setText(in.readLine());
            ui->lineEdit_3->setText(in.readLine());
            ui->lineEdit_4->setText(in.readLine());
        }
    }
}

IPConf::~IPConf()
{
    delete ui;
}

void IPConf::on_pushButton_2_clicked()
{
    this->close();
}

void IPConf::on_pushButton_clicked()
{

    QHostAddress ip;
    int nPort;
    QString role;
    QString group;
    ip     = ui->lineEdit->text();
    nPort  = ui->lineEdit_2->text().toInt();
    group  = ui->lineEdit_3->text();
    role   = ui->lineEdit_4->text();
    QMessageBox msgBox;
    msgBox.setWindowTitle("Сообщение");
    msgBox.setText(tr("Сохранить настройки подключения в файл?"));
    QAbstractButton* pButtonYes = msgBox.addButton(tr("Да"), QMessageBox::YesRole);
    QAbstractButton* pButtonNo = msgBox.addButton(tr("Нет"), QMessageBox::NoRole);

    msgBox.exec();

    if(msgBox.clickedButton() == pButtonYes)
    {
        QFile file(QApplication::applicationDirPath() + "/config.txt");
        file.open(QIODevice::WriteOnly);
        QTextStream stream(&file);
        stream << ui->lineEdit->text()<< "\r\n";
        stream << ui->lineEdit_2->text()<< "\r\n";
        stream << ui->lineEdit_3->text()<< "\r\n";
        stream << ui->lineEdit_4->text()<< "\r\n";
        stream.flush();
        file.close();
        emit sendIP(ip, nPort, group, role);
    }
    if(msgBox.clickedButton() == pButtonNo)
    {
        emit sendIP(ip, nPort, group, role);
    }
    this->close();

}
