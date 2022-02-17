#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QMap>
#include <QHash>
#include <QDateTime>

#include "settingsparsers.h"
#include "directorymanager.h"

class SettingsParsers;



//Настройки обработки поступлений всех файлов в фильтр
struct FilterSettings
{
    //      путь к "мусорке" фильтра
    QString junk_path;

       /*\\имя файла\\соответствующие имени настройки обработки\\*/
    QMap<QString,   QList<EventExecutionParameters>> files_execution_parameters;
};



enum VariableType
{
    string,
    number,
    undefined
};



//структура хранения пользовательских переменных
struct Variable
{
    QString name;
    QString value;
    VariableType type;
};



class Settings : public QObject
{
    Q_OBJECT
public:
    static Settings* settings();
    ~Settings();

    //рекурсивная замена переменных в строках на их значения
    bool replaceVariablesIn(QString &attribute);
    static bool replaceVariablesIn(QString &attribute, QList<Variable*> variables);


    //-----------------------------------------------------------
    //Работа с переменными
    Variable* searchVar(QString var_name);
    static Variable* searchVar(QString var_name, QList<Variable*> variables);
    bool setValueToVar(QString var_name, QString value);
    static bool setValueToVar(QString var_name, QString value, QList<Variable*> variables);
    bool createVar(QString var_name, VariableType type, QString value = "");
    QList<Variable*> getCurrentVariables() const;


    void setVarNameForWorkProgramCurrentIndex(QString var_name);
    QString getVarNameForWorkProgramCurrentIndex() const;

    void setVarNameForSeanceCurrentIndex(QString var_name);
    QString getVarNameForSeanceCurrentIndex() const;

    void setVarNameForRotateCurrentIndex(QString var_name);
    QString getVarNameForRotateCurrentIndex() const;

    void setVarNameForMessageFilePath(QString var_name);
    QString getVarNameForMessageFilePath() const;

    void setVarNameForMessageFileName(QString var_name);
    QString getVarNameForMessageFileName() const;

    void setVarNameForReturnCode(QString var_name);
    QString getVarNameForReturnCode() const;


    DirectoryManager *directoryManager() const;


public slots:

private:
    explicit Settings();

    static Settings *obj_ptr;


    //управляющий отслеживаемыми каталогами
    DirectoryManager *directory_manager;

    //-----------------------------------------------------------
    // Переменные прорграммы
    QList<Variable*> vars;

    //-----------------------------------------------------------
    // имена переменных, используемых для обновления
    // текущих рабочих параметров
    QString var_name_work_program_current_index;

    QString var_name_seance_current_index;

    QString var_name_rotate_index;

    QString var_name_message_file_path;

    QString var_name_message_file_name;

    QString var_name_return_code;

    //-----------------------------------------------------------


signals:
    void newText(QString error_text);
};


#endif // SETTINGS_H
