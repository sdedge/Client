#include "eventexecutor.h"
#include <QDebug>
#include "settings.h"

EventExecutor::EventExecutor()
{
    m_current_action_executor = nullptr;
}

EventExecutor::~EventExecutor()
{
    delete m_current_action_executor;
}

void EventExecutor::setExecutionSettings(QList<ReceivingFileActions*> settings)
{
    m_settings = settings;
}

void EventExecutor::setJunkDir(QString junk_path)
{
    m_junk_path = junk_path;
}

void EventExecutor::getCurrentVariables()
{
    variables_copy = Settings::settings()->getCurrentVariables();
}

void EventExecutor::insertNewTaskAfterCurrentTask(EventExecutionParameters *new_task)
{
    current_execution_parameters.insert(current_execution_parameters.indexOf(current_task) + 1, new_task);
}

QList<EventExecutionParameters*> EventExecutor::searchParametersForFileName(QString file_name)
{
    for(ReceivingFileActions *file_actions : m_settings)
    {
        if(file_actions->file_name_reg_exp.exactMatch(file_name))
        {
            return file_actions->actions;
        }
    }
    return QList<EventExecutionParameters*>();//пустой лист
}

bool EventExecutor::executeFileReceiving(QString receiving_file_name)
{

    current_execution_parameters = searchParametersForFileName(receiving_file_name);



    if(current_execution_parameters.size() == 0)
    {
        qDebug() << "Отсутствуют настойки для файла " + receiving_file_name;
        return false;
    }
    else
    {
        for(EventExecutionParameters* task : current_execution_parameters)
        {
            //Выполнение всех действий
            if(selectActionExecutor(task->task_type) == false)
            {
                qDebug() << "Не удалось выбрать исполнителя для команды " << task->message_for_user;
                return false;
            }
            m_current_action_executor->setContext(variables_copy);
            m_current_action_executor->setEventExecutorPtr(this);
            qDebug() << task->message_for_user;
            if(m_current_action_executor->executeEvent(task) == false)
            {
                qDebug() << "Ошибка при выполнении " << task->message_for_user;
                return false;
            }
            qDebug() << "Выполнено " << task->message_for_user;
        }
    }
    variables_copy.clear();
    return true;
}

bool EventExecutor::executeActions(QList<EventExecutionParameters *> execution_parameters)
{
    current_execution_parameters = execution_parameters;
    if(current_execution_parameters.size() == 0)
    {
        qDebug() << "Отсутствуют настойки для файла задан пустой список действий";
        return false;
    }
    else
    {
        for(EventExecutionParameters* task : current_execution_parameters)
        {
            //Выполнение всех действий
            current_task = task;
            if(selectActionExecutor(task->task_type) == false)
            {
                qDebug() << "Не удалось выбрать исполнителя для команды " << task->task_type;
                return false;
            }
            m_current_action_executor->setContext(variables_copy);
            m_current_action_executor->setEventExecutorPtr(this);
            qDebug() << task->message_for_user;
            if(m_current_action_executor->executeEvent(task) == false)
            {
                qDebug() << "Ошибка при выполнении " << task->message_for_user;
                return false;
            }
            qDebug() << "Выполнено " << task->message_for_user;
        }
    }
    variables_copy.clear();
    return true;
}

bool EventExecutor::selectActionExecutor(int action_type)
{
    if(m_current_action_executor != nullptr)
    {
        delete m_current_action_executor;
        m_current_action_executor = nullptr;
    }

    switch (action_type)
    {
        case 10:
        {
            m_current_action_executor = new EventActionExecutor_ReplaceFile();
            return true;
        }
        case 12:
        {
            m_current_action_executor = new EventActionExecutor_ReportToServer();
            return true;
        }
    default:
        return false;

    }
}

