#-------------------------------------------------
#
# Project created by QtCreator 2017-01-20T19:11:03
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = duplicateFilesFinder
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    filehashcalculatorthread.cpp \
    founditemwidget.cpp

HEADERS  += mainwindow.h \
    filehashcalculatorthread.h \
    founditemwidget.h

FORMS    += mainwindow.ui \
    founditemwidget.ui
