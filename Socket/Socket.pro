#-------------------------------------------------
#
# Project created by QtCreator 2018-01-18T09:43:10
#
#-------------------------------------------------

QT       -= qt core gui

TARGET = Socket
TEMPLATE = lib
#CONFIG += staticlib  #2018.01.18:设为staticlib后，client无法通过编译
CONFIG += c++14

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
SOURCES     += $$files(*.cc,true)
FORMS       += $$files(*.ui,true)
RESOURCES   += $$files(*.qrc,true)
OTHER_FILES += $$files(*,true)

win32: {

}

linux: {

}


msvc {
    #QMAKE_CXXFLAGS_RELEASE -= -O1
    #QMAKE_CXXFLAGS_RELEASE -= -O2
    #QMAKE_CXXFLAGS_RELEASE *= -O3
}

clang {
    #CONFIG +=c++14
}

gcc:!clang {
    #CONFIG +=c++14
    #LIBS += -lgomp
}

#使Release版本可调试
QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO


INCLUDEPATH += $$PWD/include/

#特别注意:在linux下，-l后面需要忽略"lib"字符，比如libuv.so，就要写成-luv
LIBS += -L$$PWD/../_lib/libuv/lib/$$PLATFORM -luv
INCLUDEPATH += $$PWD/../_lib/libuv/include

message("Defines:")
message($$DEFINES)


# Visual Leak Detector
#win32: LIBS += -L"C:/Program Files (x86)/Visual Leak Detector/lib/Win32/" -lvld

#INCLUDEPATH += "C:/Program Files (x86)/Visual Leak Detector/include/"
#DEPENDPATH += "C:/Program Files (x86)/Visual Leak Detector/include/"


#INCLUDEPATH += $$PWD/../_bin/Release/macx
#DEPENDPATH += $$PWD/../_bin/Release/macx
