#ifndef SETTINGSPARSERS_H
#define SETTINGSPARSERS_H

#include <QObject>
#include "settings.h"
#include <QDomDocument>

class Settings;

struct EventExecutionParameters;

class SettingsParsers : public QObject
{
    Q_OBJECT
public:
    SettingsParsers();
    ~SettingsParsers();

    //1. Путь к отслеживаемым каталогам и соответствующим настройкам №2 (настройка № 1)
    static void parseSettings1(QString settings_1_path);

    //2. Действия при поступлении файла в один из отслеживаемых каталогов на серевере (настройка № 2)
    static void parseSettings2(QString settings_2_path, QString filter_dir_path);

    static QString formParametersString(int action_type, QDomNode *parameters_node);

    //6. Переменные пользователя, глобальные пути программы (настройка 6)
    static void parseSettings6(QString settings_6_path);

    static bool createDirIfDoesNotExist(QString dir_path);

private:




signals:

};

#endif // SETTINGSPARSERS_H
