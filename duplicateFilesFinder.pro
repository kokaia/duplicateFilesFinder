#-------------------------------------------------
#
# Project created by QtCreator 2017-01-20T19:11:03
#
#-------------------------------------------------

QT       += core gui sql webkitwidgets multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

cross_compile {
  posix|qnx|linux: DEFINES += QTWEBENGINE_RECIPE_BROWSER_EMBEDDED
}

CONFIG+=c++11

TARGET = duplicateFilesFinder
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    filehashcalculatorthread.cpp \
    founditemwidget.cpp \
    common.cpp \
    searchressulttablewidget.cpp \
    previewwidgetimage.cpp \
    previewwidgetaudio.cpp \
    previewwidgetvideo.cpp \
    previewwidgetweb.cpp

HEADERS  += mainwindow.h \
    filehashcalculatorthread.h \
    founditemwidget.h \
    common.h \
    searchressulttablewidget.h \
    previewwidgetimage.h \
    previewwidgetaudio.h \
    previewwidgetvideo.h \
    previewwidgetweb.h

FORMS    += mainwindow.ui \
    founditemwidget.ui \
    previewwidgetimage.ui \
    previewwidgetaudio.ui \
    previewwidgetvideo.ui \
    previewwidgetweb.ui

DISTFILES +=
