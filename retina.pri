CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QT -= gui
QT -= core

INCLUDEPATH += $$PWD/include
DEFINES += DEBUG

if (nodisplay) {
    DEFINES += cimg_display=0
}

unix {
    # Find python-devel with pkg-config
    CONFIG += link_pkgconfig
    PKGCONFIG += python2
    #QMAKE_CXX = g++-4.8
    QMAKE_CXXFLAGS += -fPIC -fopenmp -std=c++03 -m64
    #QMAKE_CXXFLAGS += -isystem $$PWD/CImg-1.6.0_rolling141127
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE += -O3
    QMAKE_LFLAGS_DEBUG += # -fsanitize=thread -lasan
    QMAKE_CXXFLAGS_DEBUG -= -O2
    QMAKE_CXXFLAGS_DEBUG += -O -g # -fsanitize=thread -fno-omit-frame-pointer -fPIC -lasan
    message($$QMAKE_CXXFLAGS)
}

win32 {
    INCLUDEPATH += C:\Python27\include
    INCLUDEPATH += $$PWD/CImg-1.6.0_rolling141127
    QMAKE_CXXFLAGS += /openmp
    # For non-wide char
    DEFINES -= UNICODE
    # For M_PI
    DEFINES += _USE_MATH_DEFINES
}


isEmpty(INSTALL_PREFIX) {
    INSTALL_PREFIX = $$(INSTALL_PREFIX)
}
isEmpty(INSTALL_PREFIX) {
    message("\"INSTALL_PREFIX\" not detected, defaulting to /usr/local")
    INSTALL_PREFIX = /usr/local
}

# Find Boost library.

######################################### Boost include path

# Try to use the system environment value.
BOOST_INCLUDEDIR = $$(BOOST_INCLUDEDIR)

isEmpty(BOOST_INCLUDEDIR) {
    message(environment variable \"BOOST_INCLUDEDIR\" not detected)
    unix {
        BOOST_INCLUDEDIR = /usr/include/boost
    }
    win32 {
        BOOST_INCLUDEDIR = C:\Boost\include
    }
    message(using default boost library path: $$BOOST_INCLUDEDIR)
} else {
    message(detected in BOOST_INCLUDEDIR = \"$$BOOST_INCLUDEDIR\".)
}

INCLUDEPATH += $$BOOST_INCLUDEDIR

######################################### Boost library path
# by default, boost is installed system wide

BOOST_LIBDIR = $$(BOOST_LIBDIR)

isEmpty(BOOST_LIBDIR) {
    message(Using default boost lib installation)
    unix {
        BOOST_LIBDIR = /usr/lib
    }
    win32 {
        BOOST_LIBDIR = C:\Boost\lib
    }
} else {
    message(Detected custom boost lib installation in \"$$(BOOST_LIBDIR)\".)
    unix {
        QMAKE_LFLAGS += -L$$BOOST_LIBDIR
    }
    win32 {
        QMAKE_LFLAGS += /LIBPATH:"$$BOOST_LIBDIR"
    }
}
