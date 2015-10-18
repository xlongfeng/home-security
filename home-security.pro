#-------------------------------------------------
#
# Project created by QtCreator 2015-10-17T12:28:47
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = home-security
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    watertower.cpp \
    watertowerwidget.cpp \
    avatarwidget.cpp \
    multipointcom.cpp

HEADERS  += mainwindow.h \
    watertower.h \
    watertowerwidget.h \
    avatarwidget.h \
    multipointcom.h

FORMS    += mainwindow.ui \
    watertowerwidget.ui
