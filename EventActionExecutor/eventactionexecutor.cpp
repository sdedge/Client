#include "eventactionexecutor.h"
#include <QDebug>

#include "settings.h"
#include "eventexecutor.h"


EventActionExecutor::EventActionExecutor()
{
    arguments_spacer.setPattern(("\\s*!\\s*"));
}

EventActionExecutor::~EventActionExecutor()
{
    
}

void EventActionExecutor::setContext(QList<Variable *> context_variables)
{
    m_variables = context_variables;
}

void EventActionExecutor::setEventExecutorPtr(EventExecutor *initializer)
{
    m_initializer = initializer;
}
