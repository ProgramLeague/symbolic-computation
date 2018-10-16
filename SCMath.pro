TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    ast.cpp \
    main.cpp \
    nodetype.cpp \
    funinterface.cpp

HEADERS += \
    ast.h \
    nodetype.h \
    scope.h \
    runtime.h \
    funinterface.h

QMAKE_CXXFLAGS += -std=c++14
