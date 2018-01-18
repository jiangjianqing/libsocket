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

win32: {
    #LIBS += core.lib

    CONFIG(release, debug|release):{
        LIBS += -L$$PWD/../_lib/protobuf/lib/$$PLATFORM -llibprotobuf
    }
    else:CONFIG(debug, debug|release): {
        LIBS += -L$$PWD/../_lib/protobuf/lib/$$PLATFORM -llibprotobufd
    }

    #LIBS += -L$$PWD/../_bin/$$CONFIGURATION/$$PLATFORM -lPropertyWidgets
    #LIBS += -L$$PWD/../_bin/$$CONFIGURATION/$$PLATFORM -lImageViewer
    #LIBS += -L$$PWD/../_bin/$$CONFIGURATION/$$PLATFORM -lProcessFactory
    #LIBS += -L$$PWD/../_bin/$$CONFIGURATION/$$PLATFORM -lProcessDiagram

    #copy stuff after compiling
    #resources.path = ../_bin/$$CONFIGURATION/$$PLATFORM
    #resources.files += media/process_icons
    #INSTALLS += resources

    #run windeployqt
    #copy dlls
    #copy media

    #清理目标目录
    QMAKE_POST_LINK +=  rmdir /s/q  .\\$$DESTDIR\\config & \

    #在release时才会执行windeployqt
    CONFIG(release, debug|release): QMAKE_POST_LINK +=  windeployqt.exe --no-angle --no-svg --no-system-d3d-compiler --no-quick-import --no-translations $$DESTDIR\\$$TARGET.exe & \
    #else:CONFIG(debug, debug|release):{
    #LIBS += -L$$PWD/../_bin/$$CONFIGURATION/$$PLATFORM -lIPL
    #QMAKE_POST_LINK +=  windeployqt.exe --no-angle --no-svg --no-system-d3d-compiler --no-quick-import --no-translations ../_bin/$$CONFIGURATION/$$PLATFORM/$$TARGET.exe & \

    QMAKE_POST_LINK +=  $${QMAKE_COPY_DIR} ..\\_lib\\windows $$DESTDIR & \
                        $${QMAKE_COPY_DIR} ..\\_lib\\windows\\$$CONFIGURATION $$DESTDIR & \
                        $${QMAKE_COPY_DIR} media\\config $$DESTDIR\\config\\ & \
#                        $${QMAKE_COPY_DIR} media\\process_icons ..\\_bin\\$$CONFIGURATION\\$$PLATFORM\\process_icons\\ & \
#                        $${QMAKE_COPY_DIR} media\\examples ..\\_bin\\$$CONFIGURATION\\$$PLATFORM\\examples\\ & \
#                        $${QMAKE_COPY_DIR} media\\examples\images ..\\_bin\\$$CONFIGURATION\\$$PLATFORM\\examples\\images\\ & \
#                        $${QMAKE_COPY_DIR} media\\plugin_development ..\\_bin\\$$CONFIGURATION\\$$PLATFORM\\plugin_development & \
#                        $${QMAKE_COPY_DIR} ..\\IPL\\include ..\\_bin\\$$CONFIGURATION\\$$PLATFORM\\plugin_development\\_lib\\include & \
#                        del ..\\_bin\\$$CONFIGURATION\\$$PLATFORM\\IPL.exp & \
#                        del ..\\_bin\\$$CONFIGURATION\\$$PLATFORM\\IPL.lib & \

    USE_FERVOR_UPDATER = true
}

linux: {
    LIBS += -L$$PWD/../_lib/protobuf/lib/$$PLATFORM -lprotobuf

}
#加入通用lib支持
LIBS += -L$$PWD/../_lib/openssl/lib/$$PLATFORM -lcrypto -lssl


LIBS += -L$$PWD/../_bin/CryptoUtils/$$CONFIGURATION/$$PLATFORM -lCryptoUtils
INCLUDEPATH += $$PWD/../CryptoUtils/include

INCLUDEPATH += $$PWD/../_lib/protobuf/include


INCLUDEPATH += $$PWD/../_lib/lua/include
LIBS += -L$$PWD/../_lib/lua/lib/$$PLATFORM -llua
linux: {
    LIBS +=-ldl  #显式加载动态库的动态函数库,解决 undefined reference to symbol 'dlclose@@GLIBC_2.2.5'
}


#使Release版本可调试
QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO


INCLUDEPATH += $$PWD/include/
INCLUDEPATH += $$PWD/protos/
