QT += core

TEMPLATE = app
CONFIG += c++14
CONFIG -= app_bundle

TARGET = gunit_MainTest # Название приложения
DESTDIR = $$PWD/../../../Builds/ICPMonitor # Размещение таргета

# Указываем зависимости
PRE_TARGETDEPS += $$PWD/../../../Builds/ICPMonitor/libICPMonitorCore.a
INCLUDEPATH += $$PWD/../../core
LIBS += -L$$PWD/../../../Builds/ICPMonitor -lICPMonitorCore
unix:!macx: LIBS += -lgtest -lgmock

# Если необходимо замерить покрытие кода
contains(CONFIG, code_coverage){
    CONFIG += debug gunit_stable
    CONFIG -= release
    QMAKE_CXXFLAGS += -O0
    QMAKE_CFLAGS += -O0
    LIBS += -lgcov
}

# Исходники
SOURCES += main.cpp

# Если включён какой-то пакет тестирования
contains(CONFIG, gunit_alpha){
    include(alphaTestSuit.pri)
    message(Make Alpha Test Suit)
} else: contains(CONFIG, gunit_beta){
    include(alphaTestSuit.pri)
    include(betaTestSuit.pri)
    message(Make Beta Test Suit)
} else: contains(CONFIG, gunit_stable) {
    include(alphaTestSuit.pri)
    include(betaTestSuit.pri)
    include(stableTestSuit.pri)
    message(Make Stable Test Suit)
}
