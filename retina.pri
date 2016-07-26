QMAKE_CXXFLAGS+= -std=c++11 -O3
QT       -= gui

# Find python-devel with pkg-config
CONFIG += link_pkgconfig
PKGCONFIG += python2

# Find Boost library.

######################################### Boost include path
_DEFAULT_BOOST_INCLUDEDIR = /usr/include/boost

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
    LIBS += -L$$_BOOST_LIBDIR
}
