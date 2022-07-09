QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    BasicShader.cpp \
    Camera.cpp \
    Helper.cpp \
    Light.cpp \
    Main.cpp \
    Material.cpp \
    Model.cpp \
    ModelData.cpp \
    Node.cpp \
    RendererManager.cpp \
    Window.cpp

HEADERS += \
    BasicShader.h \
    Camera.h \
    Helper.h \
    Light.h \
    Material.h \
    Model.h \
    ModelData.h \
    Node.h \
    RendererManager.h \
    Window.h

include(Dependencies/qtimgui/qtimgui.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc
