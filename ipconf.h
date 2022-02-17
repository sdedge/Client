#ifndef IPCONF_H
#define IPCONF_H

#include <QMainWindow>
#include <QHostAddress>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>


namespace Ui {
class IPConf;
}

class IPConf : public QMainWindow
{
    Q_OBJECT

public:
    explicit IPConf(QWidget *parent = 0);

    ~IPConf();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();
signals:
    void sendIP(QHostAddress adress, int port, QString group, QString role);
private:
    Ui::IPConf *ui;

};

#endif // IPCONF_H
