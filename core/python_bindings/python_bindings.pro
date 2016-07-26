include(../../retina.pri)
TARGET = pyretina.so
QT       -= core
DESTDIR = ../../lib

QMAKE_CXXFLAGS += -fPIC
QMAKE_LFLAGS += -shared '-Wl,-rpath,\'\$$ORIGIN\''

CONFIG -= app_bundle
TEMPLATE = app

INCLUDEPATH += ../core/

SOURCES = pyretina.cpp InterfaceNESTWrapper.cpp PythonRetina.cpp
HEADERS = InterfaceNESTWrapper.h PythonRetina.h

pathToLibretina = ../../lib

LIBS += -L$$pathToLibretina -lretina
