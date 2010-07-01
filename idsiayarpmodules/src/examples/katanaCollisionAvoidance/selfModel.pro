QT += opengl \
    xml
TARGET = selfModel
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
LIBS += -lFreeSOLID \
    -L/usr/local/lib
HEADERS += SOLID/solid.h \
    collisiondetector.h \
    displmatrix.h \
    window.h \
    zphandler.h \
    robot.h \
    interval.h \
    glwidget.h \
    motor.h \
    branch.h \
    link.h \
    primitiveobject.h \
    cylinder.h \
    joint.h \
    joint_prismatic.h \
    joint_revolute.h \
    box.h \
    world.h \
    kintreenode.h \
    object.h \
    sphere.h \
    displaylist.h
SOURCES += window.cpp \
    main.cpp \
    zphandler.cpp \
    robot.cpp \
    interval.cpp \
    glwidget.cpp \
    motor.cpp \
    branch.cpp \
    link.cpp \
    primitiveobject.cpp \
    cylinder.cpp \
    joint.cpp \
    joint_prismatic.cpp \
    joint_revolute.cpp \
    box.cpp \
    world.cpp \
    kintreenode.cpp \
    object.cpp \
    sphere.cpp \
    displaylist.cpp
