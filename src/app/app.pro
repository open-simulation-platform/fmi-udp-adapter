TEMPLATE = app

CONFIG += console
CONFIG -= app_bundle qt

TARGET = FmiUdpAdapterApp


win32 {

    LIBS += \
        -L$$OUT_PWD/../lib/release -lFmiUdpAdapter

} else {

    LIBS += \
        -L$$OUT_PWD/../lib -lFmiUdpAdapter \
        -L/opt/Qt5.12.1/5.12.1/gcc_64/lib -licudata -licui18n -licuuc

}



INCLUDEPATH += \
    $$PWD/../../vendor/fmi-standard.org

SOURCES += \
    Main.c
