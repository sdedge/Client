#ifndef EVENTACTIONEXECUTOR_REPORTTOSERVER_H
#define EVENTACTIONEXECUTOR_REPORTTOSERVER_H

#include <QObject>
#include "eventactionexecutor.h"

#include "datapack.h"


class EventActionExecutor_ReportToServer : public EventActionExecutor
{
    Q_OBJECT
public:
    EventActionExecutor_ReportToServer();
    virtual ~EventActionExecutor_ReportToServer();

    virtual bool executeEvent(EventExecutionParameters *action_parameters) override;
signals:
    void sendReportDataPack(DataPack);
};

#endif // EVENTACTIONEXECUTOR_REPORTTOSERVER_H
