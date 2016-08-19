#-------------------------------------------------
#
# Project created by QtCreator 2016-08-09T12:31:39
#
#-------------------------------------------------

QT       += core serialport network

QT       -= gui

TARGET = SomeData
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    network.cpp \
    device.cpp

HEADERS += \
    network.h \
    device.h \
    device.h
