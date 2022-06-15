# Reference: https://doc.qt.io/qt-6/cmake-manual.html
cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT_MIN_VERSION 6.2.2)
ADD_DEFINITIONS(-DQT_DISABLE_DEPRECATED_BEFORE=0x060000 -DUSE_WIDGETS)

# Add folder where are supportive functions
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Include Qt basic functions
include(QtCommon)

project(find-duplicates VERSION 0.1 LANGUAGES CXX)

# Set PROJECT_VERSION_PATCH and PROJECT_VERSION_TWEAK to 0 if not present, needed by add_project_meta
fix_project_version()

# Set additional project information
set(COMPANY "Softgen")
set(COPYRIGHT "Copyright (c) 2022 Softgen Inc. All rights reserved.")
set(IDENTIFIER "ge.softgen.voix-client")

set(INSTALLDIR "installdir")

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets LinguistTools Network Sql REQUIRED)

set(TS_FILES resources/voix-audio-recorder_ka_GE.ts)
set(RESOURCE_FILES resources/resources.qrc)
# @formatter:off
set(SOURCE_FILES
        src/main.cpp
        src/common.cpp                              src/common.h
        src/utils/common_util.cpp                   src/utils/common_util.h
        src/filehashcalculatorthread.cpp            src/filehashcalculatorthread.h
        src/components/searchressulttablewidget.cpp src/components/searchressulttablewidget.h
        src/mainwindow.cpp                          src/mainwindow.h                     src/mainwindow.ui
        src/founditemwidget.cpp                     src/founditemwidget.h                src/founditemwidget.ui
        src/components/commonlistwidget.cpp         src/components/commonlistwidget.h    src/components/commonlistwidget.ui
        src/components/tfilterwidget.cpp            src/components/tfilterwidget.h       src/components/tfilterwidget.ui
        src/preview/previewwidgetaudio.cpp          src/preview/previewwidgetaudio.h     src/preview/previewwidgetaudio.ui
        src/preview/previewwidgetimage.cpp          src/preview/previewwidgetimage.h     src/preview/previewwidgetimage.ui
        src/preview/previewwidgetvideo.cpp          src/preview/previewwidgetvideo.h     src/preview/previewwidgetvideo.ui
#       src/preview/previewwidgetweb.cpp            src/preview/previewwidgetweb.h       src/preview/previewwidgetweb.ui


        ${TS_FILES}
        )
# @formatter:on

add_project_meta(META_FILES_TO_INCLUDE)

qt_add_executable(${PROJECT_NAME} MANUAL_FINALIZATION ${OS_BUNDLE} # Expands to WIN32 or MACOS_BUNDLE depending on OS
        ${SOURCE_FILES} ${META_FILES_TO_INCLUDE} ${RESOURCE_FILES}
        )

set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
        MACOSX_BUNDLE TRUE
        )

target_precompile_headers(${PROJECT_NAME} INTERFACE QtWidgets.h QtMultimedia.h)
target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Multimedia
        Qt6::Network
        Qt6::Sql
        Qt6::MultimediaWidgets
        )

qt6_add_translations(${PROJECT_NAME}
        TS_FILES ${TS_FILES}
        QM_FILES_OUTPUT_VARIABLE qm_files)

install(FILES ${qm_files} DESTINATION "${INSTALLDIR}/resources/")

set(i18n_resource_files
        ${qm_files}
        )

qt6_add_resources(${PROJECT_NAME} "i18n"
        PREFIX
        "/"
        FILES
        ${i18n_resource_files}
        )


install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION "${INSTALLDIR}"
        BUNDLE DESTINATION "${INSTALLDIR}"
        LIBRARY DESTINATION "${INSTALLDIR}"
        )

qt_finalize_executable(${PROJECT_NAME})
