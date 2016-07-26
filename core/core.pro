#-------------------------------------------------
#
# Project created by QtCreator 2014-11-28T10:52:33
#
#-------------------------------------------------

include(../retina.pri)

TEMPLATE = lib

######################################### Config

DESTDIR = ../lib

unix {
    TARGET = retina

    QMAKE_LFLAGS += -shared
    LIBS += -lX11 -lpthread -fopenmp
    LIBS += -lboost_python -lboost_filesystem -lboost_system
}
win32 {
    TARGET = retina

    CONFIG += dll

    DEFINES += EXPORTING_RETINA

    QMAKE_LFLAGS += /LIBPATH:"C:\Python27-x64\libs"
    LIBS += user32.lib gdi32.lib shell32.lib

    CONFIG(debug, debug|release) {
        LIBS += boost_python-vc120-mt-gd-1_61.lib
    } else {
        LIBS += boost_python-vc120-mt-1_61.lib
    }
}

SOURCES = GaussFilter.cpp \
    multimeter.cpp \
    LinearFilter.cpp \
    SingleCompartment.cpp \
    module.cpp \
    Retina.cpp \
    GratingGenerator.cpp \
    StaticNonLinearity.cpp \
    DisplayManager.cpp \
    whiteNoise.cpp \
    impulse.cpp \
    ShortTermPlasticity.cpp \
    fixationalMovGrating.cpp \
    constants.cpp \
    DisplayWithBar.cpp

HEADERS = \
    GaussFilter.h \
    multimeter.h \
    LinearFilter.h \
    SingleCompartment.h \
    module.h \
    Retina.h \
    GratingGenerator.h \
    StaticNonLinearity.h \
    DisplayManager.h \
    whiteNoise.h \
    impulse.h \
    ShortTermPlasticity.h \
    fixationalMovGrating.h \
    constants.h \
    DisplayWithBar.h
