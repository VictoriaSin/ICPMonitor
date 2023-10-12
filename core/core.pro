QT += core gui widgets svg sql concurrent

TEMPLATE = lib
CONFIG += staticlib c++14

TARGET = ICPMonitorCore
DESTDIR = $$PWD/../../Builds/ICPMonitor # Размещение таргета

# Если необходимо замерить покрытие кода
contains(CONFIG, code_coverage) {
    CONFIG -= release
    CONFIG += debug
    QMAKE_CXXFLAGS += --coverage -O0
    QMAKE_CFLAGS += --coverage -O0
    LIBS += -lgcov
}

HEADERS += \
    blockDevices/blockdevicemanager_enums.h \
    blockDevices/deviceWatcher/qdevicewatcher.h \
    blockDevices/deviceWatcher/qdevicewatcher_p.h \
    blockDevices/blockdevice.h \
    blockDevices/blockdevicemanager.h \
    controller/alarm_enums.h \
    controller/alarmcontroller.h \
    controller/averageicpcontroller.h \
    controller/controller_enums.h \
    controller/databasemanager.h \
    controller/fileProcessing/export_enums.h \
    controller/fileProcessing/filecontroller_events.h \
    controller/fileProcessing/serialization/abstracticpdataserializer.h \
    controller/fileProcessing/serialization/idataserializer.h \
    controller/labels/label.h \
    controller/labels/labelmanager.h \
    controller/labels/labelprovider.h \
    controller/languagesetting.h \
    controller/monitorcontroller.h \
    controller/settings.h \
    crc8table.h \
    global_enums.h \
    global_functions.h \
    global_structs.h \
    sensor/impl/bmp280temperaturespisensor.h \
    sensor/impl/fileimitsensor.h \
    sensor/isensor.h \
    sensor/savespi.h \
    sensor/savespi_1.h \
    sensor/sensor_enums.h \
    sensor/spi.h \
    sensor/zsc.h \
    statistical_functions.h \
    ultrafastconverting/countdecimaldigit.h \
    ultrafastconverting/countlut.h \
    ultrafastconverting/dateconversion.h \
    ultrafastconverting/digitslut.h \
    version.h \
    usb_funcs.h

SOURCES += \
    blockDevices/deviceWatcher/qdevicewatcher.cpp \
    blockDevices/deviceWatcher/qdevicewatcher_linux.cpp \
    blockDevices/blockdevice.cpp \
    blockDevices/blockdevicemanager.cpp \
    controller/alarmcontroller.cpp \
    controller/averageicpcontroller.cpp \
    controller/databasemanager.cpp \
    controller/fileProcessing/export_enums.cpp \
    controller/fileProcessing/serialization/abstracticpdataserializer.cpp \
    controller/labels/label.cpp \
    controller/labels/labelmanager.cpp \
    controller/labels/labelprovider.cpp \
    controller/languagesetting.cpp \
    controller/monitorcontroller.cpp \
    controller/settings.cpp \
    global_functions.cpp \
    global_structs.cpp \
    sensor/impl/bmp280temperaturespisensor.cpp \
    sensor/impl/fileimitsensor.cpp \
    sensor/savespi.cpp \
    sensor/savespi_1.cpp \
    sensor/spi.cpp \
    sensor/zsc.cpp \
    statistical_functions.cpp \
    ultrafastconverting/countlut.cpp \
    ultrafastconverting/dateconversion.cpp \
    usb_funcs.cpp

RESOURCES += \
    res/core_res.qrc

TRANSLATIONS += \
    res/trans/core_en.ts
