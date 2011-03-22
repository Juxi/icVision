# Install script for directory: /Users/kail/imClever/dev/virtualSkin/src/VirtualSkinLibrary/RobotModel

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
  FILE(INSTALL DESTINATION "/Users/kail/imClever/dev/virtualSkin/lib" TYPE STATIC_LIBRARY FILES "/Users/kail/imClever/dev/virtualSkin/xcode/src/VirtualSkinLibrary/RobotModel/libRobotModel.a")
  IF(EXISTS "$ENV{DESTDIR}/Users/kail/imClever/dev/virtualSkin/lib/libRobotModel.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/Users/kail/imClever/dev/virtualSkin/lib/libRobotModel.a")
    EXECUTE_PROCESS(COMMAND "/usr/bin/ranlib" "$ENV{DESTDIR}/Users/kail/imClever/dev/virtualSkin/lib/libRobotModel.a")
  ENDIF()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "/Users/kail/imClever/dev/virtualSkin/include/RobotModel" TYPE FILE FILES
    "/Users/kail/imClever/dev/virtualSkin/src/VirtualSkinLibrary/RobotModel/skinWindow.h"
    "/Users/kail/imClever/dev/virtualSkin/src/VirtualSkinLibrary/RobotModel/glwidget.h"
    "/Users/kail/imClever/dev/virtualSkin/src/VirtualSkinLibrary/RobotModel/renderList.h"
    "/Users/kail/imClever/dev/virtualSkin/src/VirtualSkinLibrary/RobotModel/robotmodelexception.h"
    "/Users/kail/imClever/dev/virtualSkin/src/VirtualSkinLibrary/RobotModel/zphandler.h"
    "/Users/kail/imClever/dev/virtualSkin/src/VirtualSkinLibrary/RobotModel/robot.h"
    "/Users/kail/imClever/dev/virtualSkin/src/VirtualSkinLibrary/RobotModel/interval.h"
    "/Users/kail/imClever/dev/virtualSkin/src/VirtualSkinLibrary/RobotModel/motor.h"
    "/Users/kail/imClever/dev/virtualSkin/src/VirtualSkinLibrary/RobotModel/bodyPart.h"
    "/Users/kail/imClever/dev/virtualSkin/src/VirtualSkinLibrary/RobotModel/link.h"
    "/Users/kail/imClever/dev/virtualSkin/src/VirtualSkinLibrary/RobotModel/primitiveobject.h"
    "/Users/kail/imClever/dev/virtualSkin/src/VirtualSkinLibrary/RobotModel/cylinder.h"
    "/Users/kail/imClever/dev/virtualSkin/src/VirtualSkinLibrary/RobotModel/joint.h"
    "/Users/kail/imClever/dev/virtualSkin/src/VirtualSkinLibrary/RobotModel/joint_prismatic.h"
    "/Users/kail/imClever/dev/virtualSkin/src/VirtualSkinLibrary/RobotModel/joint_revolute.h"
    "/Users/kail/imClever/dev/virtualSkin/src/VirtualSkinLibrary/RobotModel/box.h"
    "/Users/kail/imClever/dev/virtualSkin/src/VirtualSkinLibrary/RobotModel/world.h"
    "/Users/kail/imClever/dev/virtualSkin/src/VirtualSkinLibrary/RobotModel/kintreenode.h"
    "/Users/kail/imClever/dev/virtualSkin/src/VirtualSkinLibrary/RobotModel/compositeObject.h"
    "/Users/kail/imClever/dev/virtualSkin/src/VirtualSkinLibrary/RobotModel/sphere.h"
    "/Users/kail/imClever/dev/virtualSkin/src/VirtualSkinLibrary/RobotModel/displmatrix.h"
    "/Users/kail/imClever/dev/virtualSkin/src/VirtualSkinLibrary/RobotModel/displaylist.h"
    "/Users/kail/imClever/dev/virtualSkin/src/VirtualSkinLibrary/RobotModel/model.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

