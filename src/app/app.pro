include(../../retina.pri)

TEMPLATE = app

DESTDIR = ../../bin

unix {
    QMAKE_LFLAGS += -Wl,-rpath,\'\$$ORIGIN/../lib\'
    LIBS += -lX11 -L../../lib/ -lretina -lboost_python -lboost_filesystem -lboost_system -lboost_chrono -lpthread -fopenmp

    CONFIG(debug, debug|release) {
        target.path += $$INSTALL_PREFIX/lib/debug/usr/bin
    }
    CONFIG(release, release|debug) {
        target.path += $$INSTALL_PREFIX/bin
    }
}
win32 {
    QMAKE_LFLAGS += /LIBPATH:"C:\Python27\libs" /LIBPATH:"../../lib"
    LIBS += python27.lib retina.lib
    LIBS += user32.lib gdi32.lib shell32.lib

    target.path = $$INSTALL_PREFIX/bin
}

TARGET = COREM

INSTALLS += target

SOURCES = main.cpp
