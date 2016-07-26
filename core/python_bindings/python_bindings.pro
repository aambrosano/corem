include(../../retina.pri)

TEMPLATE = lib

# For retina core lib headers
INCLUDEPATH += ../../core/

unix {
    TARGET = pyretina

    # Avoids the creation of libpyretina.so instead of pyretina.so
    CONFIG += no_plugin_name_prefix

    QMAKE_CXXFLAGS += -fPIC
    QMAKE_LFLAGS += -shared -Wl,-rpath,\'\$$ORIGIN\'

    LIBS += -L../../lib -lretina
}

win32 {
    TARGET = pyretina

    CONFIG += dll

    QMAKE_LFLAGS += /LIBPATH:../../lib
    QMAKE_LFLAGS += /LIBPATH:"$$_BOOST_LIBDIR" /LIBPATH:"C:\Python27-x64\libs" /FORCE:MULTIPLE

    # Python extensions need pyd extensions from Python2.5 on
    QMAKE_EXTENSION_SHLIB = pyd

    LIBS += python27.lib retina.lib
    CONFIG(debug, debug|release) {
        LIBS += boost_python-vc120-mt-gd-1_61.lib libboost_filesystem-vc120-mt-gd-1_61.lib
    } else {
        LIBS += boost_python-vc120-mt-1_61.lib libboost_filesystem-vc120-mt-1_61.lib
    }
    LIBS += user32.lib gdi32.lib shell32.lib

}

DESTDIR = ../../lib

SOURCES = pyretina.cpp \
    RetinaLoader.cpp \
    RetinaWrapper.cpp

HEADERS = \
    RetinaLoader.h \
    RetinaWrapper.h
