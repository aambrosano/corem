include(../../retina.pri)

TEMPLATE = lib

unix {
    TARGET = pyretina

    # Avoids the creation of libpyretina.so instead of pyretina.so
    CONFIG += plugin no_plugin_name_prefix

    QMAKE_CXXFLAGS += -fPIC
    QMAKE_LFLAGS += -shared -Wl,-rpath,\'\$$ORIGIN\'

    LIBS += -L../../lib -lretina

    CONFIG(debug, debug|release) {
        target.path += $$INSTALL_PREFIX/lib/debug/lib
    }
    CONFIG(release, release|debug) {
        target.path += $$INSTALL_PREFIX/lib
    }

    INSTALLS += target
}

win32 {
    TARGET = pyretina

    CONFIG += dll

    QMAKE_LFLAGS += /LIBPATH:../../lib
    QMAKE_LFLAGS += /LIBPATH:"$$BOOST_LIBDIR" /LIBPATH:"C:\Python27\libs" /FORCE:MULTIPLE

    # Python extensions need pyd extensions from Python2.5 on
    QMAKE_EXTENSION_SHLIB = pyd

    LIBS += python27.lib retina.lib
    CONFIG(debug, debug|release) {
        LIBS += libboost_python-vc140-mt-gd-1_61.lib libboost_filesystem-vc140-mt-gd-1_61.lib
    } else {
        LIBS += libboost_python-vc140-mt-1_61.lib libboost_filesystem-vc140-mt-1_61.lib
    }
    LIBS += user32.lib gdi32.lib shell32.lib

    dlltarget.path = $$INSTALL_PREFIX/lib
    INSTALLS += dlltarget
}

HEADERS_ROOT = ../../include

DESTDIR = ../../lib

SOURCES = pyretina.cpp
