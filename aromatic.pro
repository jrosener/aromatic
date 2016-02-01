TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
QT += concurrent
QT -= gui core

SOURCES += main.cpp \
    temperature.cpp \
    wind.cpp \
    meteo_forecast.cpp \
    arome_grib_parser.cpp \
    utils.cpp \
    arome_grib_downloader.cpp \
    meteo_forecast_processor.cpp \
    meteo_forecast_printer.cpp

HEADERS += \
    temperature.h \
    wind.h \
    meteo_forecast.h \
    arome_grib_parser.h \
    utils.h \
    arome_grib_downloader.h \
    meteo_forecast_processor.h \
    meteo_forecast_printer.h
