#-------------------------------------------------
#
# Project created by QtCreator 2017-12-15T10:31:47
#
#-------------------------------------------------

QT       -= core gui

TARGET = CommonUtils
TEMPLATE = lib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#define platform variable for folder name
win32 {contains(QMAKE_TARGET.arch, x86_64) {PLATFORM = x64} else {PLATFORM = Win32}}
macx {PLATFORM = macx}
unix:!macx:!android {PLATFORM = linux}

#define configuration variable for folder name
CONFIG(debug, debug|release) {CONFIGURATION = Debug} else {CONFIGURATION = Release}

DESTDIR = ../_bin/$$TARGET/$$CONFIGURATION/$$PLATFORM
OBJECTS_DIR = ../_intermediate/$$TARGET/$$CONFIGURATION/$$PLATFORM
MOC_DIR = ../_intermediate/$$TARGET/$$CONFIGURATION/$$PLATFORM
RCC_DIR = ../_intermediate/$$TARGET/$$CONFIGURATION/$$PLATFORM
UI_DIR = ../_intermediate/$$TARGET/$$CONFIGURATION/$$PLATFORM

HEADERS     += $$files(*.h,true)
SOURCES     += $$files(*.cpp,true)
FORMS       += $$files(*.ui,true)
RESOURCES   += $$files(*.qrc,true)
OTHER_FILES += $$files(*,true)

# exclude plugin templates
#HEADERS     -= media/plugin_development/_template/NAME.h
#SOURCES     -= media/plugin_development/_template/NAME.cpp
#SOURCES     -= media/plugin_development/_template/plugin.cpp

win32: {
    CONFIG += staticlib
    #CONFIG(release, debug|release): LIBS += -L$$PWD/../_bin/$$CONFIGURATION/$$PLATFORM -lIPL
    #else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../_bin/$$CONFIGURATION/$$PLATFORM -lIPL

    LIBS += -L$$PWD/../_lib/libxml2 -llibxml2

    LIBS += -L$$PWD/../_lib/jsoncpp/lib/$$CONFIGURATION/$$PLATFORM -ljsoncpp

    LIBS += -L$$PWD/../_lib/iconv -liconv

    CONFIG(release, debug|release): LIBS += -L$$PWD/../_lib/curl/lib/windows/$$CONFIGURATION/$$PLATFORM -llibcurl
    else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../_lib/curl/lib/windows/$$CONFIGURATION/$$PLATFORM -llibcurl_debug

}

linux: {
    INCLUDEPATH += $$PWD/../_lib/boost_1_66_0/include
    LIBS += -L$$PWD/../_lib/boost_1_66_0/lib -lboost_system -lboost_filesystem
}

msvc {
    #QMAKE_CXXFLAGS_RELEASE -= -O1
    #QMAKE_CXXFLAGS_RELEASE -= -O2
    #QMAKE_CXXFLAGS_RELEASE *= -O3
}

clang {
    CONFIG +=c++14
}

gcc:!clang {
    CONFIG +=c++14
    LIBS += -lgomp
}

#使Release版本可调试
QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO


INCLUDEPATH += $$PWD/include/
INCLUDEPATH += $$PWD/include/commonutils/

INCLUDEPATH += $$PWD/../_lib/libxml2/include
INCLUDEPATH += $$PWD/../_lib/iconv/include

INCLUDEPATH += $$PWD/../_lib/jsoncpp/include

INCLUDEPATH += $$PWD/../_lib/curl/include



#INCLUDEPATH += $$PWD/../_lib/google_breakpad/include

message("Defines:")
message($$DEFINES)

# Visual Leak Detector
#win32: LIBS += -L"C:/Program Files (x86)/Visual Leak Detector/lib/Win32/" -lvld

#INCLUDEPATH += "C:/Program Files (x86)/Visual Leak Detector/include/"
#DEPENDPATH += "C:/Program Files (x86)/Visual Leak Detector/include/"


#INCLUDEPATH += $$PWD/../_bin/Release/macx
#DEPENDPATH += $$PWD/../_bin/Release/macx
