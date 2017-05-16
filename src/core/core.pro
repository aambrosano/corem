include(../../retina.pri)

TEMPLATE = lib
HEADERS_ROOT = ../../include/

unix {
    TARGET = retina

    QMAKE_LFLAGS += -shared
    LIBS += -lX11 -lpthread -fopenmp
    LIBS += -lboost_python -lboost_filesystem -lboost_system -lboost_chrono -lboost_iostreams

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
message($$DESTDIR)

SOURCES = \
    input/fixationalMovGrating.cpp \
    input/gratingGenerator.cpp \
    input/impulse.cpp \
    input/whiteNoise.cpp \
    input/sequence.cpp \
    input/image.cpp \
    module/gaussFilter.cpp \
    module/linearFilter.cpp \
    module/shortTermPlasticity.cpp \
    module/singleCompartment.cpp \
    module/staticNonLinearity.cpp \
    constants.cpp \
    displayManager.cpp \
    displayWithBar.cpp \
    input.cpp \
    module.cpp \
    multimeter.cpp \
    retina.cpp \
    module/parrot.cpp \
    multimeter/spatialMultimeter.cpp \
    multimeter/temporalMultimeter.cpp \
    multimeter/LNMultimeter.cpp \
    module/customNonLinearity.cpp \
    module/spaceVariantGaussFilter.cpp \
    pyretina.cpp

HEADERS = \
    $$HEADERS_ROOT/COREM/core/multimeter.h \
    $$HEADERS_ROOT/COREM/core/module.h \
    $$HEADERS_ROOT/COREM/core/input/whiteNoise.h \
    $$HEADERS_ROOT/COREM/core/input/impulse.h \
    $$HEADERS_ROOT/COREM/core/input/fixationalMovGrating.h \
    $$HEADERS_ROOT/COREM/core/constants.h \
    $$HEADERS_ROOT/COREM/core/displayManager.h \
    $$HEADERS_ROOT/COREM/core/displayWithBar.h \
    $$HEADERS_ROOT/COREM/core/module/gaussFilter.h \
    $$HEADERS_ROOT/COREM/core/input/gratingGenerator.h \
    $$HEADERS_ROOT/COREM/core/input/sequence.h \
    $$HEADERS_ROOT/COREM/core/input/image.h \
    $$HEADERS_ROOT/COREM/core/module/linearFilter.h \
    $$HEADERS_ROOT/COREM/core/retina.h \
    $$HEADERS_ROOT/COREM/core/module/shortTermPlasticity.h \
    $$HEADERS_ROOT/COREM/core/module/singleCompartment.h \
    $$HEADERS_ROOT/COREM/core/module/staticNonLinearity.h \
    $$HEADERS_ROOT/COREM/core/input.h \
    $$HEADERS_ROOT/COREM/core/module/parrot.h \
    $$HEADERS_ROOT/COREM/core/multimeter/spatialMultimeter.h \
    $$HEADERS_ROOT/COREM/core/multimeter/temporalMultimeter.h \
    $$HEADERS_ROOT/COREM/core/multimeter/LNMultimeter.h \
    ../../include/COREM/core/module/customNonLinearity.h \
    ../../include/COREM/core/module/spaceVariantGaussFilter.h

headers_files.files = $$HEADERS
headers_files.path = $$INSTALL_PREFIX/include/COREM/core
INSTALLS += headers_files
