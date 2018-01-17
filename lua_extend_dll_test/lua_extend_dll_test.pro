#-------------------------------------------------
#
# Project created by QtCreator 2018-01-17T17:13:35
#
#-------------------------------------------------

QT       -= core gui
CONFIG += c++14
TARGET = mylib
TEMPLATE = lib

DEFINES += LUA_EXTEND_TEST_LIBRARY

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

unix {
    target.path = /usr/lib
    INSTALLS += target
}

#加入通用lib支持
LIBS += -L$$PWD/../_bin/CryptoUtils/$$CONFIGURATION/$$PLATFORM -lCryptoUtils
INCLUDEPATH += $$PWD/../CryptoUtils/include
LIBS += -L$$PWD/../_lib/openssl/lib/$$PLATFORM -lcrypto -lssl


INCLUDEPATH += $$PWD/../_lib/protobuf/include
LIBS += -L$$PWD/../_lib/protobuf/lib/$$PLATFORM -lprotobuf

INCLUDEPATH += $$PWD/../_lib/lua/include
LIBS += -llua
LIBS +=-ldl  #显式加载动态库的动态函数库,解决 undefined reference to symbol 'dlclose@@GLIBC_2.2.5'

#使Release版本可调试
QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO


INCLUDEPATH += $$PWD/include/
INCLUDEPATH += $$PWD/protos/
