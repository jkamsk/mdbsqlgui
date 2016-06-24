#-------------------------------------------------
#
# Project created by QtCreator 2015-09-15T05:46:07
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mdbsqlgui
TEMPLATE = app


SOURCES += main.cpp\
    mdbsqlgui.cpp \
    dbconnection.cpp \
    guiconnection.cpp \
    dbentity.cpp \
    guicreatedatabase.cpp \
    sqlwidget.cpp \
    syntax.cpp \
    usermng.cpp \
    guitable.cpp \
    dbuser.cpp \
    guiuser.cpp \
    settings.cpp

HEADERS  += mdbsqlgui.h \
    dbconnection.h \
    config.h \
    guiconnection.h \
    dbentity.h \
    guicreatedatabase.h \
    sqlwidget.h \
    syntax.h \
    usermng.h \
    guitable.h \
    dbuser.h \
    guiuser.h \
    settings.h

FORMS    += mdbsqlgui.ui \
    guiconnection.ui \
    guicreatedatabase.ui \
    sqlwidget.ui \
    usermng.ui \
    guitable.ui \
    guiuser.ui

RESOURCES += \
    resources.qrc

DISTFILES += \
    readme.txt \
    change.log
