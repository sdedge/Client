#include "eventactionexecutor_replacefile.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>

#include "settings.h"


EventActionExecutor_ReplaceFile::EventActionExecutor_ReplaceFile()
{

}

EventActionExecutor_ReplaceFile::~EventActionExecutor_ReplaceFile()
{

}

bool EventActionExecutor_ReplaceFile::executeEvent(EventExecutionParameters *action_parameters)
{

    QString variables_buffer = action_parameters->task_exec_parameters;

    Settings::replaceVariablesIn(variables_buffer, m_variables);


    QStringList arguments = variables_buffer.split(arguments_spacer);

    QString file_path = arguments.at(0);

    QFileInfo file_info(file_path);


    QString trash_path = arguments.at(1) + file_info.fileName() + "_1";

    bool success = false;

    if(QFile::exists(file_path))                                                                                //если исходный файл существует
    {
        QString postfix;
        int i = 2;
        while(QFile::exists(trash_path))                                                                        //если конечный файл уже существует
        {
            trash_path.chop(trash_path.size() - trash_path.lastIndexOf("_"));
            postfix = "_" + QString::number(i);
            trash_path += postfix;
            ++i;
        }
        if(QFile::copy(file_path, trash_path))                                                                  //если копирование удалось
        {
            if(QFile::remove(file_path))                                                                        //если удалось удалить файл или файл не существует
            {
                Settings::setValueToVar(Settings::settings()->getVarNameForMessageFileName(), file_info.fileName(), m_variables);
                Settings::setValueToVar(Settings::settings()->getVarNameForMessageFilePath(), trash_path, m_variables);
                success = true;                                                                                 //удаление прошло успешно
            }                                                                                                   //копирование прошло успешно
            else
            {
                qDebug() << "Не удалось удалить из " + file_path + ".";
            }
        }
        else
        {
            qDebug() << "Не удалось скопировать из " + file_path + " в " + trash_path;
        }
    }
    else
    {
        qDebug() << "Исходный файл для перемещения отсутствует";
    }

    return success | action_parameters->skip;
}
