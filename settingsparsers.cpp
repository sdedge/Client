#include "settingsparsers.h"

#include <QFile>
#include <QDir>
#include <QHash>
#include <QList>
#include <QMap>

#include <QDebug>

#include "globalfunctions.h"

extern bool boolFromString(QString str);


SettingsParsers::SettingsParsers()
{

}

SettingsParsers::~SettingsParsers()
{

}


//1. Путь к отслеживаемым каталогам и соответствующим настройкам №2 (настройка № 1)
void SettingsParsers::parseSettings1(QString settings_1_path)
{
    QString errorStr;
    int errorLine;
    int errorColumn;

    QFile parse_file_obj(settings_1_path);
    if(parse_file_obj.exists())
    {
        QDomDocument settings_1_document;
        if(!settings_1_document.setContent(&parse_file_obj, true, &errorStr, &errorLine, &errorColumn))
        {
            QString error_text = "Ошибка при чтении файла настроек № 1: \n Строка "
                                 + QString::number(errorLine) + ", символ " + QString::number(errorColumn) + "\n" +
                                 errorStr;
            qDebug() << error_text;
        }
        if(settings_1_document.documentElement().tagName().toUpper() == "SETTING_1")
        {
            QDomElement root = settings_1_document.documentElement();
            QDomNode filters_dir = root.firstChild();
            if(filters_dir.toElement().tagName().toUpper() == "DIR")
            {
                while (!filters_dir.isNull())
                {

                    QString filter_dir_path = filters_dir.toElement().attribute("path");

                    Settings::settings()->replaceVariablesIn(filter_dir_path);

                    createDirIfDoesNotExist(filter_dir_path);

                    QString settings_2_path = filters_dir.toElement().attribute("setting_2");

                    Settings::settings()->replaceVariablesIn(settings_2_path);

                    parseSettings2(settings_2_path, filter_dir_path);


                    filters_dir = filters_dir.nextSibling();
                }
            }
            else
            {
                QString error_text = "Ошибка при чтении файла настроек №1: неверное имя тега отслеживаемых директорий. \n Строка "
                                     + QString::number(filters_dir.lineNumber()) + ", символ " + QString::number(filters_dir.columnNumber()) + ".";
                qDebug() << error_text;
            }
        }
        else
        {
            QString error_text = "Ошибка при чтении файла настроек №1: неверное имя главного тега.";
            qDebug() << error_text;
        }
    }
    else
    {
        QString error_text = "Ошибка при чтении файла настроек №1: файл настроек отсутствует.";
        qDebug() << error_text  << settings_1_path;
    }

}

//2. Действия при поступлении файла в один из отслеживаемых каталогов на серевере (настройка № 2)
void SettingsParsers::parseSettings2(QString settings_2_path, QString filter_dir_path)
{
    QString errorStr;
    int errorLine;
    int errorColumn;

    QFile parse_file_obj(settings_2_path);
    if(parse_file_obj.exists())
    {
        QDomDocument settings_2_document;
        if(!settings_2_document.setContent(&parse_file_obj, true, &errorStr, &errorLine, &errorColumn))
        {
            QString error_text = "Ошибка при чтении файла настроек № 2: \n Строка "
                                 + QString::number(errorLine) + ", символ " + QString::number(errorColumn) + "\n" +
                                 errorStr;
            qDebug() << error_text;
        }
        if(settings_2_document.documentElement().tagName().toUpper() == "SETTING_2")
        {
            QDomElement root = settings_2_document.documentElement();
            QDomNode file_node = root.firstChild();

            QList<ReceivingFileActions*> files_execut_params;
            QString junk_path;

            while(!file_node.isNull())
            {
                if(file_node.toElement().tagName().toUpper() == "FILE")
                {
                    ReceivingFileActions* file_actions = new ReceivingFileActions();

                    QString buffer_file_name = file_node.toElement().attribute("name");
                    Settings::settings()->replaceVariablesIn(buffer_file_name);
                    QRegExp file_name_reg_exp(buffer_file_name);
                    QDomNode action_node = file_node.firstChild();

                    QList<EventExecutionParameters*> actions_list;

                    while(!action_node.isNull())
                    {

                        if(action_node.toElement().tagName().toUpper() == "ACTION")
                        {

                            EventExecutionParameters* action_parameters = new EventExecutionParameters();


                            int action_type = action_node.toElement().attribute("type").toInt();
                            action_parameters->task_type = action_type;

                            action_parameters->message_for_user = action_node.toElement().attribute("message");


                            action_parameters->skip = boolFromString(action_node.toElement().attribute("skip", "0"));


                            QDomNode parameters_node = action_node.firstChild();




                            if(parameters_node.toElement().tagName().toUpper() == "PARAMETERS")
                            {
                                QString parameters_string = formParametersString(action_type, &parameters_node);
                                if(parameters_string == "")
                                {
                                    QString error_text = "Ошибка при чтении файла настроек №2: ошибка при чтении параметров команды для поступившего файла " + file_name_reg_exp.pattern() + ".";
                                    qDebug() << error_text;
                                }
                                else
                                {
                                    action_parameters->task_exec_parameters = parameters_string;
                                    actions_list.append(action_parameters);
                                }
                            }
                            else
                            {
                                QString error_text = "Ошибка при чтении файла настроек №2: неверное имя тега параметров действия.";
                                qDebug() << error_text;
                            }

                        }
                        else
                        {
                            QString error_text = "Ошибка при чтении файла настроек №2: неверное имя тега выполняемого действия.";
                            qDebug() << error_text;
                        }

                        action_node = action_node.nextSibling();
                    }

                    file_actions->file_name_reg_exp = file_name_reg_exp;
                    file_actions->actions = actions_list;

                    files_execut_params.append(file_actions);

                }
                else if(file_node.toElement().tagName().toUpper() == "JUNK")
                {
                    junk_path = file_node.toElement().attribute("path");

                    Settings::settings()->replaceVariablesIn(junk_path);
                    createDirIfDoesNotExist(junk_path);
                }
                else
                {
                    QString error_text = "Ошибка при чтении файла настроек №2: неверное имя тега обнаруживаемого файла. \n Строка "
                                         + QString::number(file_node.lineNumber()) + ", символ " + QString::number(file_node.columnNumber()) + ".";
                    qDebug() << error_text;
                }
                file_node = file_node.nextSibling();

            }

            Settings::settings()->directoryManager()->lookForDir(filter_dir_path, files_execut_params, junk_path);

        }
        else
        {
            QString error_text = "Ошибка при чтении файла настроек №2: неверное имя главного тега.";
            qDebug() << error_text;
        }
    }
    else
    {
        QString error_text = "Ошибка при чтении файла настроек №2: файл настроек отсутствует.";
        qDebug() << error_text  << settings_2_path;
    }

}

QString SettingsParsers::formParametersString(int action_type, QDomNode *parameters_node)
{
    QString splitter = "!";
    QString parameters_string = "";
    switch (action_type)
    {
        case 1:
        {
            parameters_string = parameters_node->toElement().attribute("scen_path") + splitter + parameters_node->toElement().attribute("wait_list_id", "0");
            break;
        }
        case 2:
        {
            parameters_string = parameters_node->toElement().attribute("file_path");
            break;
        }
        case 3:
        {
            parameters_string = parameters_node->toElement().attribute("source_path") + splitter + parameters_node->toElement().attribute("target_path") + splitter + parameters_node->toElement().attribute("write_to_db", "0");
            break;
        }
        case 4:
        {
            parameters_string = parameters_node->toElement().attribute("delete_path");
            break;
        }
        case 5:
        {
            parameters_string = parameters_node->toElement().attribute("pack_path") + splitter + parameters_node->toElement().attribute("unpack_path");
            break;
        }
        case 6:
        {
            parameters_string = parameters_node->toElement().attribute("manual_path");
            break;
        }
        case 7:
        {
            parameters_string = parameters_node->toElement().attribute("var_name") + splitter + parameters_node->toElement().attribute("start_marker") + splitter + parameters_node->toElement().attribute("finish_marker");
            break;
        }
        case 8:
        {
            parameters_string = parameters_node->toElement().attribute("var_name") + splitter + parameters_node->toElement().attribute("operator") + splitter + parameters_node->toElement().attribute("number");
            break;
        }
        case 9:
        {
            parameters_string = parameters_node->toElement().attribute("settings_path");
            break;
        }
        case 10:
        {
            parameters_string = parameters_node->toElement().attribute("file_path") + splitter + parameters_node->toElement().attribute("trash_path");
            break;
        }
        case 11:
        {
            parameters_string = parameters_node->toElement().attribute("settings_path");
            break;
        }
        case 12:
        {
        parameters_string = parameters_node->toElement().attribute("temp_file_path") + splitter + parameters_node->toElement().attribute("file_name");
        break;
        }
        default:
        {
            QString error_text = "Ошибка при чтении файла настроек №2: неизвестный тип действия";
            break;
        }
    }

    return parameters_string;
}


//6. Переменные пользователя, глобальные пути программы (настройка 6)
void SettingsParsers::parseSettings6(QString settings_6_path)
{
    QString errorStr;
    int errorLine;
    int errorColumn;


    QFile parse_file_obj(settings_6_path);
    if(parse_file_obj.exists())
    {
        QDomDocument doc;
        if(!doc.setContent(&parse_file_obj, true, &errorStr, &errorLine, &errorColumn))
        {
            QString error_text = "Ошибка при чтении файла настроек №6: \n Строка "
                                 + QString::number(errorLine) + ", символ " + QString::number(errorColumn) + "\n" +
                                 errorStr;
            qDebug() << error_text;
        }
        if(doc.documentElement().tagName().toUpper() == "SETTING_6")
        {
            QDomElement root = doc.documentElement();
            QDomNode node = root.firstChild();
            while (!node.isNull())
            {
                if(node.toElement().tagName().toUpper() == "DIR_CONSTANTS")
                {
                    QDomNode constant = node.firstChild();
                    while (!constant.isNull())
                    {
                        if(constant.toElement().tagName().toUpper() == "CONST")
                        {
                            QString const_name = constant.toElement().attribute("name");
                            QString const_value = constant.toElement().attribute("value");
                            QString type = constant.toElement().attribute("type");
                            VariableType const_type;

                            if(type == "str")
                            {
                                const_type = string;
                                Settings::settings()->createVar(const_name, const_type, const_value);
                                if(createDirIfDoesNotExist(const_value) == false)
                                {
                                    qDebug() << "Ошибка! Не удалось создать каталог. " << const_value;
                                }
                            }
                            else if(type == "int")
                            {
                                qDebug() << "Ошибка! Задан неверный тип для константы пути к директории. " << const_name;
                            }
                            else
                            {
                                qDebug() << "Некорректный тип константы " << const_name;
                            }
                        }
                        else
                        {
                            QString error_text = "Ошибка при чтении файла настроек №6: неверное имя тега константы пути. \n Строка "
                                                 + QString::number(constant.lineNumber()) + ", символ " + QString::number(constant.columnNumber()) + ".";
                            qDebug () << error_text;
                        }
                        constant = constant.nextSibling();
                    }
                }
                else if(node.toElement().tagName().toUpper() == "VARIABLES")
                {
                    QDomNode variable = node.firstChild();
                    while (!variable.isNull())
                    {
                        if(variable.toElement().tagName().toUpper() == "VAR_WORK_PROGRAM_NUMBER")
                        {
                            QString var_name = variable.toElement().attribute("name");

                            Settings::settings()->setVarNameForWorkProgramCurrentIndex(var_name);

                            QString type = variable.toElement().attribute("type");
                            VariableType var_type;

                            if(type == "str")
                            {
                                var_type = string;
                                Settings::settings()->createVar(var_name, var_type);
                            }
                            else if(type == "int")
                            {
                                var_type = number;
                                Settings::settings()->createVar(var_name, var_type);
                            }
                            else
                            {
                                var_type = undefined;
                                qDebug() << "Некорректный тип переменной " << var_name;
                            }
                        }
                        else if(variable.toElement().tagName().toUpper() == "VAR_CURR_ROTATE_NUMBER")
                        {
                            QString var_name = variable.toElement().attribute("name");

                            Settings::settings()->setVarNameForRotateCurrentIndex(var_name);

                            QString type = variable.toElement().attribute("type");
                            VariableType var_type;

                            if(type == "str")
                            {
                                var_type = string;
                                Settings::settings()->createVar(var_name, var_type);
                            }
                            else if(type == "int")
                            {
                                var_type = number;
                                Settings::settings()->createVar(var_name, var_type);
                            }
                            else
                            {
                                qDebug() << "Некорректный тип переменной " << var_name;
                            }
                        }
                        else if(variable.toElement().tagName().toUpper() == "VAR_SEANCE_NUMBER")
                        {
                            QString var_name = variable.toElement().attribute("name");

                            Settings::settings()->setVarNameForSeanceCurrentIndex(var_name);

                            QString type = variable.toElement().attribute("type");
                            VariableType var_type;

                            if(type == "str")
                            {
                                var_type = string;
                                Settings::settings()->createVar(var_name, var_type);
                            }
                            else if(type == "int")
                            {
                                var_type = number;
                                Settings::settings()->createVar(var_name, var_type);
                            }
                            else
                            {
                                qDebug() << "Некорректный тип переменной " << var_name;
                            }


                        }
                        else if(variable.toElement().tagName().toUpper() == "VAR_MESSAGE_FILE_PATH")
                        {
                            QString var_name = variable.toElement().attribute("name");

                            Settings::settings()->setVarNameForMessageFilePath(var_name);

                            QString type = variable.toElement().attribute("type");
                            VariableType var_type;

                            if(type == "str")
                            {
                                var_type = string;
                                Settings::settings()->createVar(var_name, var_type);
                            }
                            else if(type == "int")
                            {
                                var_type = number;
                                Settings::settings()->createVar(var_name, var_type);
                            }
                            else
                            {
                                qDebug() << "Некорректный тип переменной " << var_name;
                            }
                        }
                        else if(variable.toElement().tagName().toUpper() == "VAR_MESSAGE_FILE_NAME")
                        {
                            QString var_name = variable.toElement().attribute("name");

                            Settings::settings()->setVarNameForMessageFileName(var_name);

                            QString type = variable.toElement().attribute("type");
                            VariableType var_type;

                            if(type == "str")
                            {
                                var_type = string;
                                Settings::settings()->createVar(var_name, var_type);
                            }
                            else if(type == "int")
                            {
                                var_type = number;
                                Settings::settings()->createVar(var_name, var_type);
                            }
                            else
                            {
                                qDebug() << "Некорректный тип переменной " << var_name;
                            }
                        }
                        else
                        {
                            QString error_text = "Ошибка при чтении файла настроек №6: неверное имя тега переменной. \n Строка "
                                                 + QString::number(variable.lineNumber()) + ", символ " + QString::number(variable.columnNumber()) + ".";
                            qDebug() << error_text;
                        }
                        variable = variable.nextSibling();
                    }
                }
                node = node.nextSibling();
            }
        }
        else
        {
            QString error_text = "Ошибка при чтении файла настроек №6: неверное имя главного тега.";
            qDebug() << error_text;
        }
    }
    else
    {
        QString error_text = "Ошибка при чтении файла настроек №6: файл настроек отсутствует.";
        qDebug() << error_text;
    }

}

bool SettingsParsers::createDirIfDoesNotExist(QString dir_path)
{
    QDir dir(dir_path);
    if(!dir.exists())
    {
        return dir.mkpath(dir_path);
    }
    return true;
}
