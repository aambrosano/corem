include(../../retina.pri)

TEMPLATE = lib
HEADERS_ROOT = ../../include/

unix {
    TARGET = retina

    QMAKE_LFLAGS += -shared
    LIBS += -lX11 -lpthread -fopenmp
    LIBS += -lboost_python -lboost_filesystem -lboost_system -lboost_chrono

    CONFIG(debug, debug|release) {
        target.path += $$INSTALL_PREFIX/lib/debug/usr/lib
    }
    CONFIG(release, release|debug) {
        target.path += $$INSTALL_PREFIX/lib
    }

    INSTALLS += target
}
win32 {
    TARGET = retina

    CONFIG += dll

    DEFINES += EXPORTING_RETINA

    QMAKE_LFLAGS += /LIBPATH:"C:\Python27\libs"
    LIBS += user32.lib gdi32.lib shell32.lib

    QMAKE_LFLAGS += /LIBPATH:"$$BOOST_LIBDIR" /FORCE:MULTIPLE
    message($$QMAKE_LFLAGS)
    CONFIG(debug, debug|release) {
        LIBS += libboost_python-vc140-mt-gd-1_61.lib libboost_filesystem-vc140-mt-gd-1_61.lib libboost_system-vc140-mt-gd-1_61.lib
    } else {
        LIBS += libboost_python-vc140-mt-1_61.lib libboost_filesystem-vc140-mt-1_61.lib libboost_system-vc140-mt-1_61.lib
    }

    dlltarget.path += $$INSTALL_PREFIX/lib
    INSTALLS += dlltarget
}

DESTDIR = ../../lib

SOURCES = \
    multimeter.cpp \
    module.cpp \
    whiteNoise.cpp \
    impulse.cpp \
    fixationalMovGrating.cpp \
    constants.cpp \
    displayManager.cpp \
    displayWithBar.cpp \
    gaussFilter.cpp \
    gratingGenerator.cpp \
    linearFilter.cpp \
    retina.cpp \
    shortTermPlasticity.cpp \
    staticNonLinearity.cpp \
    singleCompartment.cpp

HEADERS = \
    $$HEADERS_ROOT/COREM/core/multimeter.h \
    $$HEADERS_ROOT/COREM/core/module.h \
    $$HEADERS_ROOT/COREM/core/whiteNoise.h \
    $$HEADERS_ROOT/COREM/core/impulse.h \
    $$HEADERS_ROOT/COREM/core/fixationalMovGrating.h \
    $$HEADERS_ROOT/COREM/core/constants.h \
    $$HEADERS_ROOT/COREM/core/displayManager.h \
    $$HEADERS_ROOT/COREM/core/displayWithBar.h \
    $$HEADERS_ROOT/COREM/core/gaussFilter.h \
    $$HEADERS_ROOT/COREM/core/gratingGenerator.h \
    $$HEADERS_ROOT/COREM/core/linearFilter.h \
    $$HEADERS_ROOT/COREM/core/retina.h \
    $$HEADERS_ROOT/COREM/core/shortTermPlasticity.h \
    $$HEADERS_ROOT/COREM/core/singleCompartment.h \
    $$HEADERS_ROOT/COREM/core/staticNonLinearity.h

headers_files.files = $$HEADERS
headers_files.path = $$INSTALL_PREFIX/include/COREM/core
INSTALLS += headers_files
