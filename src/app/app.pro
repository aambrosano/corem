include(../../retina.pri)

TEMPLATE = app

DESTDIR = ../../bin

unix {
    QMAKE_LFLAGS += -Wl,-rpath,\'\$$ORIGIN/../lib\'
    LIBS += -lX11 -L../../lib/ -lretina -lboost_python -lboost_filesystem -lboost_system -lpthread -fopenmp
}
win32 {
    QMAKE_LFLAGS += /LIBPATH:"C:\Python27\libs" /LIBPATH:"../../lib"
    LIBS += python27.lib retina.lib
    LIBS += user32.lib gdi32.lib shell32.lib
}

TARGET = COREM

target.path = $$INSTALL_PREFIX/bin
INSTALLS += target

SOURCES = main.cpp
