QT += core gui sql svg widgets concurrent
DEFINES += QT_NO_PRINTER

TARGET = ICPMonitor                         # Название приложения
DESTDIR = $$PWD/../../Builds/ICPMonitor     # Размещение таргета

# Указываем зависимости
PRE_TARGETDEPS += $$PWD/../../Builds/ICPMonitor/libICPMonitorCore.a
INCLUDEPATH += $$PWD/../core/
LIBS += -L$$PWD/../../Builds/ICPMonitor -lICPMonitorCore

SOURCES += \
    abstractdialogpage.cpp \
    abstractmenupage.cpp \
    animatedtoolbutton.cpp \
    clock.cpp \
    gui/alarmlevelicpwidget.cpp \
    gui/alarmsetuppage.cpp \
    gui/averageicpwidget.cpp \
    gui/averagepointerwidget.cpp \
    gui/dialogWindows/abstractdialogwindow.cpp \
    gui/dialogWindows/blockdevicesselectiondialog.cpp \
    gui/dialogWindows/messagedialog.cpp \
    gui/dialogWindows/movedatadialog.cpp \
    gui/dialogWindows/selectiondialog.cpp \
    gui/dialogWindows/texteditdialog.cpp \
    gui/downloadData/datadownloadpage.cpp \
    gui/datetimepage.cpp \
    gui/decoratorspinbox.cpp \
    gui/generalsettingspage.cpp \
    gui/gui_funcs.cpp \
    gui/languagepage.cpp \
    gui/mainmenu.cpp \
    gui/numkeyboard.cpp \
    gui/settingsmenu.cpp \
    gui/systeminfopage.cpp \
    gui/technicalaccesspage.cpp \
    gui/zerosensorpage.cpp \
    ipagewidget.cpp \
    main.cpp \
    gui/mainwindow.cpp \
    gui/mainpage.cpp \
    owi_protocol.cpp \
    plots/abstractgraphareawidget.cpp \
    plots/abstractmultiplegraphsareaswidget.cpp \
    plots/boxplot.cpp \
    plots/currentgraphsarea.cpp \
    plots/labelmarkitem.cpp \
    plots/markitem.cpp \
    plots/recordedplot.cpp \
    plots/statisticgraphsarea.cpp \
    plots/abstractcustomplot.cpp \
    plots/waveformplot.cpp \
    plots/qcustomplot.cpp

HEADERS += \
    abstractdialogpage.h \
    abstractmenupage.h \
    animatedtoolbutton.h \
    clock.h \
    global_define.h \
    gui/alarmlevelicpwidget.h \
    gui/alarmsetuppage.h \
    gui/averageicpwidget.h \
    gui/averagepointerwidget.h \
    gui/dialogWindows/abstractdialogwindow.h \
    gui/dialogWindows/blockdevicesselectiondialog.h \
    gui/dialogWindows/messagedialog.h \
    gui/dialogWindows/movedatadialog.h \
    gui/dialogWindows/selectiondialog.h \
    gui/dialogWindows/texteditdialog.h \
    gui/downloadData/datadownloadpage.h \
    gui/datetimepage.h \
    gui/decoratorspinbox.h \
    gui/generalsettingspage.h \
    gui/gui_funcs.h \
    gui/languagepage.h \
    gui/mainmenu.h \
    gui/mainwindow.h \
    gui/mainpage.h \
    gui/numkeyboard.h \
    gui/settingsmenu.h \
    gui/systeminfopage.h \
    gui/technicalaccesspage.h \
    gui/zerosensorpage.h \
    ipagewidget.h \
    owi_protocol.h \
    plots/abstractgraphareawidget.h \
    plots/abstractmultiplegraphsareaswidget.h \
    plots/boxplot.h \
    plots/currentgraphsarea.h \
    plots/labelmarkitem.h \
    plots/markitem.h \
    plots/recordedplot.h \
    plots/statisticgraphsarea.h \
    plots/abstractcustomplot.h \
    plots/waveformplot.h \
    plots/qcustomplot.h

FORMS += \
    abstractdialogpage.ui \
    gui/abstractmenupage.ui \
    gui/alarmlevelicpwidget.ui \
    gui/alarmsetuppage.ui \
    gui/averageicpwidget.ui \
    gui/datetimepage.ui \
    gui/dialogWindows/abstractdialogwindow.ui \
    gui/dialogWindows/messagedialog.ui \
    gui/dialogWindows/movedatadialog.ui \
    gui/dialogWindows/selectiondialog.ui \
    gui/dialogWindows/texteditdialog.ui \
    gui/downloadData/datadownloadpage.ui \
    gui/generalsettingspage.ui \
    gui/languagepage.ui \
    gui/mainwindow.ui \
    gui/mainpage.ui \
    gui/numkeyboard.ui \
    gui/systeminfopage.ui \
    gui/technicalaccesspage.ui \
    gui/zerosensorpage.ui \
    plots/abstractgraphareawidget.ui \
    plots/currentgraphsarea.ui \
    plots/statisticgraphsarea.ui

RESOURCES += \
    res/app_res.qrc

TRANSLATIONS += \
    res/trans/icp_monitor_en.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
