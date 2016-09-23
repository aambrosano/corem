CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QT -= gui
QT -= core

if (nodisplay) {
    DEFINES += cimg_display=0
}

unix {
    # Find python-devel with pkg-config
    CONFIG += link_pkgconfig
    PKGCONFIG += python2
    QMAKE_CXXFLAGS += -std=c++11 -fPIC -fopenmp
}

win32 {
    INCLUDEPATH += C:\Python27\include
    QMAKE_CXXFLAGS += /openmp
    # For non-wide char
    DEFINES -= UNICODE
    # For M_PI
    DEFINES += _USE_MATH_DEFINES
}

# Find Boost library.

######################################### Boost include path
unix {
    _DEFAULT_BOOST_INCLUDEDIR = /usr/include/boost
}
win32 {
    _DEFAULT_BOOST_INCLUDEDIR = C:\Boost\include
}

# Try to use qmake variable's value.
_INSTALL_PREFIX = $$INSTALL_PREFIX

isEmpty(_INSTALL_PREFIX) {
    message(\"INSTALL_PREFIX\" qmake value not detected)

    # Try to use the system environment value.
    _INSTALL_PREFIX = $$(INSTALL_PREFIX)
}

# Try to use the system environment value.
_BOOST_INCLUDEDIR = $$(BOOST_INCLUDEDIR)

isEmpty(_BOOST_INCLUDEDIR) {
    message(environment variable \"BOOST_INCLUDEDIR\" not detected)
    message(using default boost library path: $$_DEFAULT_BOOST_INCLUDEDIR)
    _BOOST_INCLUDEDIR = $$_DEFAULT_BOOST_INCLUDEDIR
} else {
    message(detected in BOOST_INCLUDEDIR = \"$$_BOOST_INCLUDEDIR\".)
}

INCLUDEPATH += $$_BOOST_INCLUDEDIR

######################################### Boost library path
# by default, boost is installed system wide

# Try to use the system environment value.
_BOOST_LIBDIR = $$(BOOST_LIBDIR)

isEmpty(_BOOST_LIBDIR) {
    message(environment variable \"BOOST_LIBDIR\" not detected)
} else {
    message(detected in BOOST_LIBDIR = \"$$_BOOST_LIBDIR\".)
    unix {
        QMAKE_LFLAGS += -L$$_BOOST_LIBDIR
    }
    win32 {
        QMAKE_LFLAGS += /LIBPATH:"$$_BOOST_LIBDIR"
    }
}
