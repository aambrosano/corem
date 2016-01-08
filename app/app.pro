QT       += core

TEMPLATE = app
SOURCES = main.cpp
include(../retina.pri)

DESTDIR = ../bin
INCLUDEPATH += ../core/

pathToLibretina = ../lib
LIBS += -lX11 -L$$pathToLibretina/ -lretina -Wl,-rpath=$$pathToLibretina -lboost_python -lboost_filesystem -lboost_system

CONFIG += link_pkgconfig
PKGCONFIG += python2

CONFIG += console
CONFIG -= app_bundle

TARGET = COREM
