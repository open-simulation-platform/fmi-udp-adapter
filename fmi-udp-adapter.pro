TEMPLATE = subdirs

SUBDIRS = app lib

app.subdir = src/app
lib.subdir = src/lib

app.depends = lib

# Copy files and create FMU folder structure.
win32 {

    INSTALL_PATH = C:/Temp/fmu

    debug:binaries.files = \
        $$OUT_PWD/src/lib/debug/FmiUdpAdapter.dll

    release:binaries.files = \
        $$OUT_PWD/src/lib/release/FmiUdpAdapter.dll

    binaries.files += \
        $$(QTDIR)/bin/Qt5Core.dll \
        $$(QTDIR)/bin/Qt5Network.dll \
        $$(QTDIR)/bin/libgcc_s_seh-1.dll \
        $$(QTDIR)/bin/libstdc++-6.dll \
        $$(QTDIR)/bin/libwinpthread-1.dll

    binaries.path = $$INSTALL_PATH/binaries/win64

    modelDescription.files = fmu/modelDescription.xml
    modelDescription.path = $$INSTALL_PATH

    resources.files = fmu/resources
    resources.path = $$INSTALL_PATH

} else {
    INSTALL_PATH = /tmp/fmi-udp-adapter

    # The Qt version in the installer uses bundled ICU libraries, so add them as well, or eventually build from source to
    # use system ICU libraries. Copying this way dereferences the symbolic links, but that's fine, because it seems like
    # having symbolic links in the FMU doesn't work. In case you at some point want to copy without dereferencing, extra
    # and cp with -P option works fine.
    binaries.files = \
        $$shell_path($$OUT_PWD)/src/lib/libFmiUdpAdapter.so \
        $$(QTDIR)/lib/libQt5Core.so.5 \
        $$(QTDIR)/lib/libQt5Network.so.5 \
        $$(QTDIR)/lib/libicudata.so.56 \
        $$(QTDIR)/lib/libicui18n.so.56 \
        $$(QTDIR)/lib/libicuuc.so.56
    binaries.path = $$INSTALL_PATH/binaries/linux64

    modelDescription.files = fmu/modelDescription.xml
    modelDescription.path = $$INSTALL_PATH

    resources.files = fmu/resources
    resources.path = $$INSTALL_PATH

    compress.extra = \
        cd $$INSTALL_PATH; \
        zip -r FmiUdpAdapter.fmu .
}


compress.path = $$INSTALL_PATH
# Make sure the files are there before compressing. Note the 'install_' prefix.
compress.depends = install_modelDescription install_resources install_binaries

# This one is triggered by make install. To run from Qt Creator add additional make build step with 'install' as argument.
INSTALLS += modelDescription resources binaries compress
