#-------------------------------------------------
#
# Project created by QtCreator 2022-10-07T15:11:14
#
#-------------------------------------------------

QT       += core gui xml charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

TARGET = WeatherForecast
TEMPLATE = app
RCICONS = ./StyleTheme/Base/tqyb.png



    MOC_DIR = tmp/moc

    RCC_DIR = tmp/rcc

    UI_DIR = tmp/ui

    OBJECTS_DIR = tmp/obj

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    CForecastBtn.cpp \
        CForecastWidget.cpp \
        main.cpp \
        CWeatherForecast.cpp

HEADERS += \
        CForecastBtn.h \
        CForecastWidget.h \
        CWeatherForecast.h \
        StWeatherForecast.h

FORMS += \
        CForecastBtn.ui \
        CForecastWidget.ui \
        CWeatherForecast.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
