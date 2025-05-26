TARGET = qgeoview-samples-drag-and-drop
TEMPLATE = app
CONFIG-= console

QT += gui widgets network

include(../lib.pri)
include(../shared.pri)

SOURCES += \
    MovableIcon.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    MovableIcon.h \
    mainwindow.h

RESOURCES += \
    resources.qrc
