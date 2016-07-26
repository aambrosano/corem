#-------------------------------------------------
#
# Project created by QtCreator 2014-11-28T10:52:33
#
#-------------------------------------------------

include(../retina.pri)
QT       -= core


######################################### Config
QMAKE_CXXFLAGS+= -fopenmp -fPIC
QMAKE_LFLAGS +=  -fopenmp -shared
LIBS +=  -lX11 -lpthread -lboost_python -lboost_filesystem -lboost_system
DESTDIR = ../lib
TARGET = libretina.so

CONFIG -= app_bundle

TEMPLATE = app

SOURCES = GaussFilter.cpp \
    multimeter.cpp \
    LinearFilter.cpp \
    SingleCompartment.cpp \
    module.cpp \
    Retina.cpp \
    GratingGenerator.cpp \
    InterfaceNEST.cpp \
    StaticNonLinearity.cpp \
    DisplayManager.cpp \
    whiteNoise.cpp \
    impulse.cpp \
    ShortTermPlasticity.cpp \
    fixationalMovGrating.cpp \
    constants.cpp

HEADERS = \
    GaussFilter.h \
    multimeter.h \
    LinearFilter.h \
    SingleCompartment.h \
    module.h \
    Retina.h \
    GratingGenerator.h \
    InterfaceNEST.h \
    StaticNonLinearity.h \
    DisplayManager.h \
    whiteNoise.h \
    impulse.h \
    ShortTermPlasticity.h \
    fixationalMovGrating.h \
    constants.h
