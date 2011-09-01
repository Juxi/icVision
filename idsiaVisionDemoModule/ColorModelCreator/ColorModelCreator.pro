TEMPLATE = app
TARGET = ColorModelCreator 
DESTDIR = bin
CONFIG += link_pkgconfig
PKGCONFIG += opencv
INCLUDEPATH += /usr/local/include
LIBS += -L/usr/local/lib 

QT        += core 

HEADERS   +=
SOURCES   += main.cpp
FORMS	  +=
RESOURCES +=
