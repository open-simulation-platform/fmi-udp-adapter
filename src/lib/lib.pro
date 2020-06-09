TEMPLATE = lib

QT -= gui
QT += network
#CONFIG += qt c++14 file_copies
CONFIG += qt c++14

win32:CONFIG += dll

# Most FMUs don't use the lib prefix when compiling linux libraries
unix:CONFIG += no_plugin_name_prefix

TARGET = FmiUdpAdapter


# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Use Qt and ICU libraries from the same directory as this library at run time.
unix:QMAKE_RPATHDIR += .

INCLUDEPATH += \
    $$PWD/../../vendor/fmi-standard.org

SOURCES += \
    Component.cpp \
    FmiFunctions.cpp \
    Enums.cpp

HEADERS += \
    Component.h \
    Enums.h
