#ifndef EVENTACTIONEXECUTOR_H
#define EVENTACTIONEXECUTOR_H

#include <QObject>

class EventExecutor;

struct Variable;


//хранит параметры для обработки по настройке 2
//поступления файла в папку-фильтр
struct EventExecutionParameters
{
    int task_type;
    QString message_for_user;
    bool skip;
    QString task_exec_parameters;

    EventExecutionParameters()
    {
        task_type = -1;
    }
};

struct ReceivingFileActions
{
    QRegExp file_name_reg_exp;
    QList<EventExecutionParameters*> actions;
};


class EventActionExecutor : public QObject
{
    Q_OBJECT
public:
    explicit EventActionExecutor();
    virtual  ~EventActionExecutor();
    
    virtual bool executeEvent(EventExecutionParameters *action_parameters) = 0;

    void setContext(QList<Variable*> context_variables);

    void setEventExecutorPtr(EventExecutor *initializer);
    
protected:

    bool replaceVariablesIn(QString &attribute);

    Variable* searchVar(QString var_name);

    EventExecutionParameters *m_action_parameters;

    QRegExp arguments_spacer;

    QList<Variable*> m_variables;

    EventExecutor *m_initializer;

signals:
    

};

#endif // EVENTACTIONEXECUTOR_H
