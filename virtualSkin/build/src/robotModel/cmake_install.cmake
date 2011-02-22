# Install script for directory: /Users/kail/imClever/dev/virtualSkin/src/robotModel

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "/Users/kail/imClever/dev/virtualSkin/lib" TYPE STATIC_LIBRARY FILES "/Users/kail/imClever/dev/virtualSkin/build/src/robotModel/librobotModel.a")
  IF(EXISTS "$ENV{DESTDIR}/Users/kail/imClever/dev/virtualSkin/lib/librobotModel.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/Users/kail/imClever/dev/virtualSkin/lib/librobotModel.a")
    EXECUTE_PROCESS(COMMAND "/usr/bin/ranlib" "$ENV{DESTDIR}/Users/kail/imClever/dev/virtualSkin/lib/librobotModel.a")
  ENDIF()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "/Users/kail/imClever/dev/virtualSkin/include/robotModel" TYPE FILE FILES
    "/Users/kail/imClever/dev/virtualSkin/src/robotModel/renderList.h"
    "/Users/kail/imClever/dev/virtualSkin/src/robotModel/robotmodelexception.h"
    "/Users/kail/imClever/dev/virtualSkin/src/robotModel/zphandler.h"
    "/Users/kail/imClever/dev/virtualSkin/src/robotModel/robot.h"
    "/Users/kail/imClever/dev/virtualSkin/src/robotModel/interval.h"
    "/Users/kail/imClever/dev/virtualSkin/src/robotModel/motor.h"
    "/Users/kail/imClever/dev/virtualSkin/src/robotModel/bodyPart.h"
    "/Users/kail/imClever/dev/virtualSkin/src/robotModel/link.h"
    "/Users/kail/imClever/dev/virtualSkin/src/robotModel/primitiveobject.h"
    "/Users/kail/imClever/dev/virtualSkin/src/robotModel/cylinder.h"
    "/Users/kail/imClever/dev/virtualSkin/src/robotModel/joint.h"
    "/Users/kail/imClever/dev/virtualSkin/src/robotModel/joint_prismatic.h"
    "/Users/kail/imClever/dev/virtualSkin/src/robotModel/joint_revolute.h"
    "/Users/kail/imClever/dev/virtualSkin/src/robotModel/box.h"
    "/Users/kail/imClever/dev/virtualSkin/src/robotModel/world.h"
    "/Users/kail/imClever/dev/virtualSkin/src/robotModel/kintreenode.h"
    "/Users/kail/imClever/dev/virtualSkin/src/robotModel/object.h"
    "/Users/kail/imClever/dev/virtualSkin/src/robotModel/sphere.h"
    "/Users/kail/imClever/dev/virtualSkin/src/robotModel/displmatrix.h"
    "/Users/kail/imClever/dev/virtualSkin/src/robotModel/displaylist.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

