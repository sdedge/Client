#ifndef RECEIVINGFILEEXECUTOR_H
#define RECEIVINGFILEEXECUTOR_H

#include <QObject>
#include <QFile>
#include <QMap>
#include <QList>


#include "EventActionExecutor/eventactionexecutor_replacefile.h"
#include "EventActionExecutor/eventactionexecutor_reporttoserver.h"


struct Variable;


class EventExecutor : public QObject
{
    Q_OBJECT
public:
    explicit EventExecutor();

    virtual ~EventExecutor();

    void setExecutionSettings(QList<ReceivingFileActions*> settings);

    bool executeFileReceiving(QString receiving_file_name);

    bool executeActions(QList<EventExecutionParameters*> execution_parameters);

    void setJunkDir(QString junk_path);

    void getCurrentVariables();

    void insertNewTaskAfterCurrentTask(EventExecutionParameters* new_task);


private:

    QList<EventExecutionParameters *> searchParametersForFileName(QString file_name);

    bool selectActionExecutor(int action_type);

 /*\\имя файла\\соответствующие имени настройки обработки\\*/
    QList<ReceivingFileActions*> m_settings;

    QList<EventExecutionParameters*> current_execution_parameters;

    EventActionExecutor *m_current_action_executor;

    QString m_junk_path;

    QList<Variable*> variables_copy;

    EventExecutionParameters* current_task;

signals:

};

#endif // RECEIVINGFILEEXECUTOR_H
