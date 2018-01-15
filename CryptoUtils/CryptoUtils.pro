#-------------------------------------------------
#
# Project created by QtCreator 2018-01-15T10:22:38
#
#-------------------------------------------------

QT       -= core gui

TARGET = CryptoUtils
TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++14
#CONFIG -= app_bundle
CONFIG -= qt

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

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

win32: {
    #CONFIG(release, debug|release): LIBS += -L$$PWD/../_bin/$$CONFIGURATION/$$PLATFORM -lIPL
    #else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../_bin/$$CONFIGURATION/$$PLATFORM -lIPL

    INCLUDEPATH += $$PWD/../_lib/wxwidgets/windows/include
    #DEPENDPATH += $$PWD/../_lib/wxwidgets/include
    INCLUDEPATH += $$PWD/../_lib/wxwidgets/windows/lib/vc140_x64_dll/mswu
    INCLUDEPATH += $$PWD/../_lib/wxwidgets/windows/lib/vc140_x64_dll/mswud

    LIBS += -L$$PWD/../_lib/wxwidgets/windows/lib/vc140_x64_dll -lwxmsw31ud_xrc -lwxmsw31ud_html -lwxmsw31ud_adv -lwxmsw31ud_core -lwxbase31ud_xml -lwxbase31ud_net -lwxbase31ud -lwxtiff -lwxjpeg -lwxpng -lwxzlib -lwxregexu -lwxexpat -lkernel32 -luser32 -lgdi32 -lcomdlg32 -lwxregexu -lwinspool -lwinmm -lshell32 -lcomctl32 -lole32 -loleaut32 -luuid -lrpcrt4 -ladvapi32 -lwsock32 -lShlwapi -lversion
    #官方在编译时使用了动态库配置
    DEFINES += WXUSINGDLL
    #DEFINES += __WXMSW__ __WXDEBUG__

    #重要：目的就是让别的头文件别包含了”winsock.h”内容，否则加入libuv会导致编译错误： 'sockaddr': 'struct' type redefinition
    DEFINES += _WINSOCKAPI_
    LIBS += -L$$PWD/../_lib/libuv/lib/windows/$$CONFIGURATION/$$PLATFORM -llibuv
    LIBS += wsock32.lib Ws2_32.lib Advapi32.lib User32.lib Iphlpapi.lib Psapi.lib Userenv.lib

    #LIBS += core.lib


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

    #特别注意:在linux下，-l后面需要忽略"lib"字符，比如libuv.so，就要写成-luv
    LIBS += -L$$PWD/../_lib/openssl/lib/$$PLATFORM -lcrypto -lssl

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

INCLUDEPATH += $$PWD/../_lib/openssl/include


message("Defines:")
message($$DEFINES)


# Visual Leak Detector
#win32: LIBS += -L"C:/Program Files (x86)/Visual Leak Detector/lib/Win32/" -lvld

#INCLUDEPATH += "C:/Program Files (x86)/Visual Leak Detector/include/"
#DEPENDPATH += "C:/Program Files (x86)/Visual Leak Detector/include/"


#INCLUDEPATH += $$PWD/../_bin/Release/macx
#DEPENDPATH += $$PWD/../_bin/Release/macx

