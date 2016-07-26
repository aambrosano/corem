TEMPLATE = subdirs

QT -= core
QT -= gui

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
    unix {
        _INSTALL_PREFIX = /opt/retina
    }
    win32 {
        _INSTALL_PREFIX = $$(USERPROFILE)\AppData\Local\retina
    }
}

retina.path = $$_INSTALL_PREFIX/lib
pyretina.path = $$_INSTALL_PREFIX/lib
corem.path = $$_INSTALL_PREFIX/bin

message(The retina will be installed in $$_INSTALL_PREFIX)

unix {
    retina.files = lib/libretina.so
    pyretina.files = lib/pyretina.so
    corem.files = bin/COREM
}

win32 {
    retina.files = lib/retina.lib lib/retina.dll
    pyretina.files = lib/pyretina.lib lib/pyretina.pyd
    corem.files = bin/COREM.exe
}

INSTALLS += retina pyretina corem
