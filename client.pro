#-------------------------------------------------
#
# Project created by QtCreator 2018-08-27T14:53:23
#
#-------------------------------------------------

QT       += core gui network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = client
TEMPLATE = app


SOURCES += main.cpp\
    EventActionExecutor/eventactionexecutor.cpp \
    EventActionExecutor/eventactionexecutor_replacefile.cpp \
    EventActionExecutor/eventactionexecutor_reporttoserver.cpp \
    directorymanager.cpp \
    directorymonitor.cpp \
    eventexecutor.cpp \
        mainwindow.cpp \
    ipconf.cpp \
    settings.cpp \
    settingsparsers.cpp \
    workthread.cpp

HEADERS  += \
    EventActionExecutor/eventactionexecutor.h \
    EventActionExecutor/eventactionexecutor_replacefile.h \
    EventActionExecutor/eventactionexecutor_reporttoserver.h \
    datapack.h \
    directorymanager.h \
    directorymonitor.h \
    eventexecutor.h \
    globalfunctions.h \
    ipconf.h \
    mainwindow.h \
    settings.h \
    settingsparsers.h \
    workthread.h

FORMS    += mainwindow.ui \
    ipconf.ui
