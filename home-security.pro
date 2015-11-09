#-------------------------------------------------
#
# Project created by QtCreator 2015-10-17T12:28:47
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = skynet
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    watertower.cpp \
    watertowerwidget.cpp \
    avatarwidget.cpp \
    alarmcontroller.cpp \
    settings.cpp \
    multipointcom.cpp

HEADERS  += mainwindow.h \
    watertower.h \
    watertowerwidget.h \
    avatarwidget.h \
    multipointcom.h \
    alarmcontroller.h \
    settings.h

FORMS    += mainwindow.ui \
    watertowerwidget.ui

TRANSLATIONS += skynet_zh.ts
