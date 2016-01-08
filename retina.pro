TEMPLATE = subdirs

CONFIG += ordered
SUBDIRS = core \
          core/python_bindings \
          app

# Try to use qmake variable's value.
_INSTALL_PREFIX = $$INSTALL_PREFIX
isEmpty(_INSTALL_PREFIX) {
    message(\"INSTALL_PREFIX\" qmake value not detected)

    # Try to use the system environment value.
    _INSTALL_PREFIX = $$(INSTALL_PREFIX)
}

isEmpty(_INSTALL_PREFIX) {
    message(\"INSTALL_PREFIX\" environment variable not detected)
    _INSTALL_PREFIX = /opt/retina
}


retina.path = $$_INSTALL_PREFIX/lib
pyretina.path = $$_INSTALL_PREFIX/lib
corem.path = $$_INSTALL_PREFIX/bin

message(The retina will be installed in $$_INSTALL_PREFIX)

retina.files = lib/libretina.so
pyretina.files = lib/pyretina.so
corem.files = bin/COREM


INSTALLS += retina pyretina corem
