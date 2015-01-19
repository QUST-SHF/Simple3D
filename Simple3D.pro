#-------------------------------------------------
#
# Project created by QtCreator 2015-01-18T13:57:36
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Simple3D
TEMPLATE = app
CONFIG += c++11

INCLUDEPATH += "C:/Program Files/Boost/include"


SOURCES += \
    boxSize.cpp \
    dimEditDialog.cpp \
    main.cpp \
    managedPart.cpp \
    openGLWidget.cpp \
    packer.cpp \
    packing.cpp \
    part.cpp \
    partFactory.cpp \
    partsModel.cpp \
    partStl.cpp \
    recentFilesQMenu.cpp \
    simple3d.cpp

HEADERS  += \
    boxSize.h \
    dimEditDialog.h \
    managedPart.h \
    openGLWidget.h \
    packer.h \
    packing.h \
    part.h \
    partFactory.h \
    partsModel.h \
    partStl.h \
    recentFilesQMenu.h \
    simple3d.h \
    ui_dimEditDialog.h \
    ui_simple3d.h

RESOURCES += \
    resources.qrc

FORMS += \
    dimEditDialog.ui \
    simple3d.ui

DISTFILES += \
    lightingFragmentShader.fsh \
    lightingVertexShader.vsh

win32:RC_ICONS = simple3d.ico
unix:RC_ICONS = simple3d.png
