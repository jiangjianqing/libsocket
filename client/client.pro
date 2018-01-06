TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt




VERSION = "1.0.0.snapshot"
QMAKE_TARGET_COMPANY = "FS"
QMAKE_TARGET_PRODUCT = "Algorithm Integration Helper"
QMAKE_TARGET_DESCRIPTION = ""
QMAKE_TARGET_COPYRIGHT = ""

equals(TEMPLATE, app) {
    #message("Template value equals app")
    DEFINES += \
    APP_VERSION=\"\\\"$$VERSION\\\"\" \
    APP_COMPANY=\"\\\"$$QMAKE_TARGET_COMPANY\\\"\" \
    APP_PRODUCT=\"\\\"$$QMAKE_TARGET_PRODUCT\\\"\" \
    APP_DESCRIPTION=\"\\\"$$QMAKE_TARGET_DESCRIPTION\\\"\" \
    APP_COPYRIGHT=\"\\\"$$QMAKE_TARGET_COPYRIGHT\\\"\" \
    APP_NAME=\\\"$$TARGET\\\" \

} else {
    #message("Template value equals vcapp")
    DEFINES += \
    APP_VERSION=\\\"$$VERSION\\\" \
    APP_COMPANY=\\\"$$QMAKE_TARGET_COMPANY\\\" \
    APP_PRODUCT=\\\"$$QMAKE_TARGET_PRODUCT\\\" \
    APP_DESCRIPTION=\\\"$$QMAKE_TARGET_DESCRIPTION\\\" \
    APP_COPYRIGHT=\\\"$$QMAKE_TARGET_COPYRIGHT\\\" \
    APP_NAME=\\\"$$TARGET\\\" \
}

# enable or disable update checker
USE_FERVOR_UPDATER = false
DEFINES += IMAGEPLAY_URL=\\\"http://imageplay.io\\\"
DEFINES += IMAGEPLAY_APPCAST_URL=\\\"http://imageplay.io/Appcast.xml\\\"

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


RC_FILE = res/icon.rc

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32: {
    #CONFIG(release, debug|release): LIBS += -L$$PWD/../_bin/$$CONFIGURATION/$$PLATFORM -lIPL
    #else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../_bin/$$CONFIGURATION/$$PLATFORM -lIPL

    LIBS += -L$$PWD/../_lib/libxml2 -llibxml2

    LIBS += -L$$PWD/../_lib/jsoncpp/lib/$$CONFIGURATION/$$PLATFORM -ljsoncpp

    LIBS += -L$$PWD/../_lib/iconv -liconv

    CONFIG(release, debug|release): LIBS += -L$$PWD/../_lib/curl/lib/windows/$$CONFIGURATION/$$PLATFORM -llibcurl
    else:CONFIG(debug, debug|release): LIBS += -L$$PWD/../_lib/curl/lib/windows/$$CONFIGURATION/$$PLATFORM -llibcurl_debug


    #dump解析的话，我利用的是在release中加入调试信息，然后生成pdb配合传上来的dump进行代码的定位解析。
    #debug {#调试模式下才使用DumpHelper
        LIBS += -L$$PWD/../_lib/google_breakpad/lib/windows/$$CONFIGURATION/$$PLATFORM -lexception_handler
        LIBS += -L$$PWD/../_lib/google_breakpad/lib/windows/$$CONFIGURATION/$$PLATFORM -lcrash_generation_client
        LIBS += -L$$PWD/../_lib/google_breakpad/lib/windows/$$CONFIGURATION/$$PLATFORM -lcommon

        LIBS += -L$$PWD/../DumpHelper/_lib/$$CONFIGURATION/$$PLATFORM -lDumpHelper
    #}

    LIBS += -L$$PWD/../CommonUtils/_lib/$$CONFIGURATION/$$PLATFORM -lCommonUtils
    LIBS += -L$$PWD/../Lighting/_lib/$$CONFIGURATION/$$PLATFORM -lLighting

    LIBS += -L$$PWD/../AlgRepoLib/_lib/$$CONFIGURATION/$$PLATFORM -lAlgRepoLib

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
    LIBS += -L$$PWD/../_lib/libuv/lib/linux -luv

    #wxCXXFLAGS = $$system(wx-config --prefix=D:\wxWidgets-3.0.1-rel-static --wxcfg=gcc_lib\mswud --unicode=yes --debug=yes --static=yes)
    #wxLinkOptions = $$system(wx-config --prefix=D:\wxWidgets-3.0.1-rel-static --wxcfg=gcc_lib\mswud --unicode=yes --debug=yes --static=yes)
    #wxCXXFLAGS = $$system(wx-config --prefix=$$WXWIN --wxcfg=$$WXCFG --unicode=yes --static=yes)
    #注意点2：在wxwidget目录下，执行 ./wx-config --cxxflags  得到
    wxCXXFLAGS = -I/home/cz_jjq/Downloads/wxWidgets-3.1.0/lib/wx/include/gtk2-unicode-3.1 -I/home/cz_jjq/Downloads/wxWidgets-3.1.0/include -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXGTK__ -pthread
    #wxLinkOptions = $$system(wx-config --prefix=$$WXWIN --wxcfg=$$WXCFG --unicode=yes --static=yes)
    #注意点3#：在wxwidget目录下，执行 ./wx-config --cxxflags  得到
    wxLinkOptions = -L/home/cz_jjq/Downloads/wxWidgets-3.1.0/lib -pthread   -Wl,-rpath,/home/cz_jjq/Downloads/wxWidgets-3.1.0/lib -lwx_gtk2u_xrc-3.1 -lwx_gtk2u_html-3.1 -lwx_gtk2u_qa-3.1 -lwx_gtk2u_adv-3.1 -lwx_gtk2u_core-3.1 -lwx_baseu_xml-3.1 -lwx_baseu_net-3.1 -lwx_baseu-3.1


    #QMAKE_POST_LINK +=  $${QMAKE_COPY_DIR} media/process_icons/ $$DESTDIR && \
                        #$${QMAKE_COPY_DIR} media/examples/ ../_bin/$$CONFIGURATION/$$PLATFORM/ &&\
                        #$${QMAKE_MKDIR} ../_bin/$$CONFIGURATION/$$PLATFORM/plugin_development && \
                        #rm -rf ../_bin/$$CONFIGURATION/$$PLATFORM/plugin_development/_lib ../_bin/$$CONFIGURATION/$$PLATFORM/plugin_development/_template && \
                        #$${QMAKE_COPY_DIR} media/plugin_development/_template ../_bin/$$CONFIGURATION/$$PLATFORM/plugin_development && \
                        #$${QMAKE_MKDIR} ../_bin/$$CONFIGURATION/$$PLATFORM/plugin_development/_lib && \
                        #$${QMAKE_COPY_DIR} ../IPL/include ../_bin/$$CONFIGURATION/$$PLATFORM/plugin_development/_lib/include \

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

INCLUDEPATH += $$PWD/../_lib/libuv/include

INCLUDEPATH += $$PWD/../_lib/wxwidgets/include
#DEPENDPATH += $$PWD/../_lib/wxwidgets/include
INCLUDEPATH += $$PWD/../_lib/wxwidgets/lib/linux/wx/include/gtk2-unicode-3.1



#注意点1：使用wxwidget必须在这里配置下Defines
DEFINES +=  __WXGTK__
LIBS += $$wxLinkOptions
QMAKE_CXXFLAGS_RELEASE += $$wxCXXFLAGS


message("Defines:")
message($$DEFINES)


# Visual Leak Detector
#win32: LIBS += -L"C:/Program Files (x86)/Visual Leak Detector/lib/Win32/" -lvld

#INCLUDEPATH += "C:/Program Files (x86)/Visual Leak Detector/include/"
#DEPENDPATH += "C:/Program Files (x86)/Visual Leak Detector/include/"


#INCLUDEPATH += $$PWD/../_bin/Release/macx
#DEPENDPATH += $$PWD/../_bin/Release/macx

