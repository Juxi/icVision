TEMPLATE = app
TARGET = IDSIAObjectLocalization
DESTDIR = bin
CONFIG += link_pkgconfig
PKGCONFIG += opencv
INCLUDEPATH += /usr/local/include
LIBS += -L/usr/local/lib \
    -L \
    /opt/IMCLEVER/iCub/lib \
    -lYARP_OS \
    -lYARP_init \
    -lYARP_sig \
    -lYARP_dev \
    -liKin \
    -lctrlLib
QT += core \
    gui \
    opengl
HEADERS += CVUtils.h \
    ColoredRect.h \
    WorldObject.h \
    VisionModule.h
SOURCES += CVUtils.cpp \
    ColoredRect.cpp \
    WorldObject.cpp \
    VisionModule.cpp \
    main.cpp
FORMS += 
RESOURCES += 
