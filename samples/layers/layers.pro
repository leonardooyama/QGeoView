TARGET = qgeoview-samples-layers
TEMPLATE = app
CONFIG-= console

QT += gui widgets network

include(../../lib/lib.pri)
include(../shared/shared.pri)

LIBS += -L$$OUT_PWD/../../lib -lqgeoview
LIBS += -L$$OUT_PWD/../shared -lqgeoview-samples-shared

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h
