#include "eventactionexecutor_reporttoserver.h"

#include <QDebug>
#include "mainwindow.h"
#include "settings.h"

EventActionExecutor_ReportToServer::EventActionExecutor_ReportToServer()
{

}

EventActionExecutor_ReportToServer::~EventActionExecutor_ReportToServer()
{

}

bool EventActionExecutor_ReportToServer::executeEvent(EventExecutionParameters *action_parameters)
{

    QString variables_buffer = action_parameters->task_exec_parameters;

    Settings::replaceVariablesIn(variables_buffer, m_variables);

    QStringList arguments = variables_buffer.split(arguments_spacer);

    QString temp_file_path = arguments.at(0);

    QString file_name = arguments.at(1);

    DataPack report_data_pack;
    report_data_pack.time = QTime::currentTime();
    report_data_pack.type = 17;
    report_data_pack.source = file_name;
    report_data_pack.attributes.append(temp_file_path);

    qRegisterMetaType <DataPack>("DataPack");
    connect(this, &EventActionExecutor_ReportToServer::sendReportDataPack, MainWindow::getPtr(), &MainWindow::sendDataPackToServer);

    emit sendReportDataPack(report_data_pack);

    return true;
}
