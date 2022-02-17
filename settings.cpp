#include "settings.h"

#include <QDebug>


Settings* Settings::obj_ptr = nullptr;


Settings::Settings()
{
    directory_manager = new DirectoryManager();
}



Settings* Settings::settings()
{
    if(obj_ptr == nullptr)
    {
        obj_ptr = new Settings();
    }
    return obj_ptr;
}

Settings::~Settings()
{
    if(directory_manager != nullptr)
    {
        delete directory_manager;
        directory_manager = nullptr;
    }
}

bool Settings::replaceVariablesIn(QString &attribute)
{
    bool done_without_errors = true;                                                        //флаг отсутствия ошибок в процессе замены переменных
    int first_variable_occurrence;
    QRegExp variable_reg_exp("@*@");
    if(attribute.contains(variable_reg_exp))
    {
        first_variable_occurrence = attribute.indexOf("@");
        QString variable_name = attribute.mid(first_variable_occurrence + 1, attribute.indexOf("@", first_variable_occurrence + 1) - first_variable_occurrence - 1);
        Variable *var = Settings::settings()->searchVar(variable_name);
        if(var != nullptr)                                                                  //проверка наличия переменной с таким именем
        {
            attribute.replace(first_variable_occurrence, var->name.size() + 2, var->value); //замена имени переменной на её значение
            done_without_errors = replaceVariablesIn(attribute);                            //рекурсивный вызов, до тех пор, пока не будут заменены все переменные в атрибуте
        }
        else
        {
           qDebug() << "Не найдена переменная " << variable_name;
           done_without_errors = false;
        }
    }
    return done_without_errors;
}

bool Settings::replaceVariablesIn(QString &attribute, QList<Variable *> variables)
{
    bool done_without_errors = true;                                                        //флаг отсутствия ошибок в процессе замены переменных
    int first_variable_occurrence;
    QRegExp variable_reg_exp("@*@");
    if(attribute.contains(variable_reg_exp))
    {
        first_variable_occurrence = attribute.indexOf("@");
        QString variable_name = attribute.mid(first_variable_occurrence + 1, attribute.indexOf("@", first_variable_occurrence + 1) - first_variable_occurrence - 1);
        Variable *var = Settings::searchVar(variable_name,variables);
        if(var != nullptr)                                                                  //проверка наличия переменной с таким именем
        {
            attribute.replace(first_variable_occurrence, var->name.size() + 2, var->value); //замена имени переменной на её значение
            done_without_errors = replaceVariablesIn(attribute, variables);                 //рекурсивный вызов, до тех пор, пока не будут заменены все переменные в атрибуте
        }
        else
        {
           qDebug() << "Не найдена переменная " << variable_name;
           done_without_errors = false;
        }
    }
    return done_without_errors;
}



DirectoryManager* Settings::directoryManager() const
{
    return directory_manager;
}


Variable *Settings::searchVar(QString var_name)
{
    for (Variable* var: vars)
    {
        if(var->name == var_name)
            return var;
    }

    return nullptr;
}

Variable *Settings::searchVar(QString var_name, QList<Variable *> variables)
{
    for (Variable* var: variables)
    {
        if(var->name == var_name)
            return var;
    }

    return nullptr;
}

bool Settings::setValueToVar(QString var_name, QString value)
{
    for (Variable* var: vars)
    {
        if(var->name == var_name)
        {
            var->value = value;
            return true;
        }
    }
    return  false;
}

bool Settings::setValueToVar(QString var_name, QString value, QList<Variable *> variables)
{
    for (Variable* var: variables)
    {
        if(var->name == var_name)
        {
            var->value = value;
            return true;
        }
    }
    return  false;
}



bool Settings::createVar(QString var_name, VariableType type, QString value)
{
    for (Variable* var: vars)
    {
        if(var->name == var_name)
        {
            return false;
        }
    }

    Variable *new_var = new Variable();
    new_var->name = var_name;
    new_var->type = type;
    new_var->value = value;

    vars.append(new_var);

    return true;
}

QList<Variable *> Settings::getCurrentVariables() const
{
    return vars;
}


void Settings::setVarNameForWorkProgramCurrentIndex(QString var_name)
{
    var_name_work_program_current_index = var_name;
}

QString Settings::getVarNameForWorkProgramCurrentIndex() const
{
    return var_name_work_program_current_index;
}

void Settings::setVarNameForSeanceCurrentIndex(QString var_name)
{
    var_name_seance_current_index = var_name;
}

QString Settings::getVarNameForSeanceCurrentIndex() const
{
    return var_name_seance_current_index;
}

void Settings::setVarNameForRotateCurrentIndex(QString var_name)
{
    var_name_rotate_index = var_name;
}

QString Settings::getVarNameForRotateCurrentIndex() const
{
    return var_name_rotate_index;
}

void Settings::setVarNameForMessageFilePath(QString var_name)
{
    var_name_message_file_path = var_name;
}

QString Settings::getVarNameForMessageFilePath() const
{
    return var_name_message_file_path;
}

void Settings::setVarNameForMessageFileName(QString var_name)
{
    var_name_message_file_name = var_name;
}

QString Settings::getVarNameForMessageFileName() const
{
    return var_name_message_file_name;
}

void Settings::setVarNameForReturnCode(QString var_name)
{
    var_name_return_code = var_name;
}

QString Settings::getVarNameForReturnCode() const
{
    return var_name_return_code;
}
