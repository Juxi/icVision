# Install script for directory: /Users/kail/imClever/dev/virtualSkin/src/VirtualSkinLibrary/YarpFilter

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
    SET(CMAKE_INSTALL_CONFIG_NAME "Release")
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
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    FILE(INSTALL DESTINATION "/Users/kail/imClever/dev/virtualSkin/lib" TYPE STATIC_LIBRARY FILES "/Users/kail/imClever/dev/virtualSkin/xcode/src/VirtualSkinLibrary/YarpFilter/Debug/libYarpFilter.a")
    IF(EXISTS "$ENV{DESTDIR}/Users/kail/imClever/dev/virtualSkin/lib/libYarpFilter.a" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}/Users/kail/imClever/dev/virtualSkin/lib/libYarpFilter.a")
      EXECUTE_PROCESS(COMMAND "/usr/bin/ranlib" "$ENV{DESTDIR}/Users/kail/imClever/dev/virtualSkin/lib/libYarpFilter.a")
    ENDIF()
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    FILE(INSTALL DESTINATION "/Users/kail/imClever/dev/virtualSkin/lib" TYPE STATIC_LIBRARY FILES "/Users/kail/imClever/dev/virtualSkin/xcode/src/VirtualSkinLibrary/YarpFilter/Release/libYarpFilter.a")
    IF(EXISTS "$ENV{DESTDIR}/Users/kail/imClever/dev/virtualSkin/lib/libYarpFilter.a" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}/Users/kail/imClever/dev/virtualSkin/lib/libYarpFilter.a")
      EXECUTE_PROCESS(COMMAND "/usr/bin/ranlib" "$ENV{DESTDIR}/Users/kail/imClever/dev/virtualSkin/lib/libYarpFilter.a")
    ENDIF()
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    FILE(INSTALL DESTINATION "/Users/kail/imClever/dev/virtualSkin/lib" TYPE STATIC_LIBRARY FILES "/Users/kail/imClever/dev/virtualSkin/xcode/src/VirtualSkinLibrary/YarpFilter/MinSizeRel/libYarpFilter.a")
    IF(EXISTS "$ENV{DESTDIR}/Users/kail/imClever/dev/virtualSkin/lib/libYarpFilter.a" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}/Users/kail/imClever/dev/virtualSkin/lib/libYarpFilter.a")
      EXECUTE_PROCESS(COMMAND "/usr/bin/ranlib" "$ENV{DESTDIR}/Users/kail/imClever/dev/virtualSkin/lib/libYarpFilter.a")
    ENDIF()
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    FILE(INSTALL DESTINATION "/Users/kail/imClever/dev/virtualSkin/lib" TYPE STATIC_LIBRARY FILES "/Users/kail/imClever/dev/virtualSkin/xcode/src/VirtualSkinLibrary/YarpFilter/RelWithDebInfo/libYarpFilter.a")
    IF(EXISTS "$ENV{DESTDIR}/Users/kail/imClever/dev/virtualSkin/lib/libYarpFilter.a" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}/Users/kail/imClever/dev/virtualSkin/lib/libYarpFilter.a")
      EXECUTE_PROCESS(COMMAND "/usr/bin/ranlib" "$ENV{DESTDIR}/Users/kail/imClever/dev/virtualSkin/lib/libYarpFilter.a")
    ENDIF()
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "/Users/kail/imClever/dev/virtualSkin/include" TYPE DIRECTORY FILES "/Users/kail/imClever/dev/virtualSkin/src/VirtualSkinLibrary/YarpFilter/yarp")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

