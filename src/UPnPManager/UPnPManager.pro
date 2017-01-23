QT += core
QT -= gui
QT += network

CONFIG += c++11

TARGET = UPnPManager
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    ../Shared/UPnP/upnpdiscovery.cpp \
    ../Shared/UPnP/upnpengine.cpp \
    ../Shared/UPnP/addportmappingresponse.cpp \
    ../Shared/AndamaHeaders/socket_functions.cpp \
    ../Shared/AndamaHeaders/byte_functions.cpp \
    ../Shared/AndamaHeaders/exception_helper.cpp \
    ../Shared/UPnP/deviceresponse.cpp \
    ../Shared/UPnP/upnpaddportmapping.cpp \
    ../Shared/General/bench.cpp

HEADERS += \
    ../Shared/UPnP/upnpdiscovery.h \
    ../Shared/UPnP/upnpengine.h \
    ../Shared/General/finally.h \
    ../Shared/UPnP/addportmappingresponse.h \
    ../Shared/AndamaHeaders/socket_functions.h \
    ../Shared/AndamaHeaders/byte_functions.h \
    ../Shared/AndamaHeaders/exception_helper.h \
    ../Shared/UPnP/deviceresponse.h \
    ../Shared/UPnP/upnpaddportmapping.h \
    ../Shared/General/bench.h
