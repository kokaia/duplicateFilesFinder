#-------------------------------------------------
#
# Project created by QtCreator 2017-01-20T19:11:03
#
#-------------------------------------------------

QT       += core gui widgets sql multimedia multimediawidgets webenginewidgets

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
    components/searchressulttablewidget.cpp \
    preview/previewwidgetimage.cpp \
    preview/previewwidgetaudio.cpp \
    preview/previewwidgetvideo.cpp \
    preview/previewwidgetweb.cpp \
    components/commonlistwidget.cpp \
    components/tfilterwidget.cpp \
    utils/common_util.cpp

HEADERS  += mainwindow.h \
    filehashcalculatorthread.h \
    founditemwidget.h \
    common.h \
    components/searchressulttablewidget.h \
    preview/previewwidgetimage.h \
    preview/previewwidgetaudio.h \
    preview/previewwidgetvideo.h \
    preview/previewwidgetweb.h \
    components/commonlistwidget.h \
    components/tfilterwidget.h \
    utils/common_util.h

FORMS    += mainwindow.ui \
    founditemwidget.ui \
    preview/previewwidgetimage.ui \
    preview/previewwidgetaudio.ui \
    preview/previewwidgetvideo.ui \
    preview/previewwidgetweb.ui \
    components/commonlistwidget.ui \
    components/tfilterwidget.ui

DISTFILES +=
