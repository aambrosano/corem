include(../../retina.pri)

TEMPLATE = lib
HEADERS_ROOT = ../../include/

unix {
    TARGET = retina

    QMAKE_LFLAGS += -shared

    LIBS += -lX11 -lpthread -fopenmp
    LIBS += -lboost_python -lboost_filesystem -lboost_system -lboost_chrono -lboost_iostreams

    target.path += $$INSTALL_PREFIX/lib
    CONFIG(debug, debug|release) {
        target.path += $$INSTALL_PREFIX/lib/debug/usr/lib
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

message($$QMAKE_CXXFLAGS)

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
    pyretina.cpp \
    input/localizedImpulse.cpp \
    module/highpass.cpp \
    module/rectification.cpp \
    spikes/noisylif.cpp \
    spikes/lif.cpp \
    spinnakerInjector.cpp

HEADERS = \
    $$HEADERS_ROOT/corem/spikes.h \
    $$HEADERS_ROOT/corem/multimeter.h \
    $$HEADERS_ROOT/corem/module.h \
    $$HEADERS_ROOT/corem/input/whiteNoise.h \
    $$HEADERS_ROOT/corem/input/impulse.h \
    $$HEADERS_ROOT/corem/input/fixationalMovGrating.h \
    $$HEADERS_ROOT/corem/constants.h \
    $$HEADERS_ROOT/corem/displayManager.h \
    $$HEADERS_ROOT/corem/displayWithBar.h \
    $$HEADERS_ROOT/corem/module/gaussFilter.h \
    $$HEADERS_ROOT/corem/input/gratingGenerator.h \
    $$HEADERS_ROOT/corem/input/sequence.h \
    $$HEADERS_ROOT/corem/input/image.h \
    $$HEADERS_ROOT/corem/module/linearFilter.h \
    $$HEADERS_ROOT/corem/retina.h \
    $$HEADERS_ROOT/corem/module/shortTermPlasticity.h \
    $$HEADERS_ROOT/corem/module/singleCompartment.h \
    $$HEADERS_ROOT/corem/module/staticNonLinearity.h \
    $$HEADERS_ROOT/corem/input.h \
    $$HEADERS_ROOT/corem/module/parrot.h \
    $$HEADERS_ROOT/corem/multimeter/spatialMultimeter.h \
    $$HEADERS_ROOT/corem/multimeter/temporalMultimeter.h \
    $$HEADERS_ROOT/corem/multimeter/LNMultimeter.h \
    $$HEADERS_ROOT/corem/module/customNonLinearity.h \
    $$HEADERS_ROOT/corem/module/spaceVariantGaussFilter.h \
    $$HEADERS_ROOT/corem/input/localizedImpulse.h \
    $$HEADERS_ROOT/corem/module/highpass.h \
    $$HEADERS_ROOT/corem/module/rectification.h \
    $$HEADERS_ROOT/corem/spikes/lif.h \
    $$HEADERS_ROOT/corem/spikes/noisylif.h

headers_files.files = $$HEADERS
headers_files.path = $$INSTALL_PREFIX/include
INSTALLS += headers_files
