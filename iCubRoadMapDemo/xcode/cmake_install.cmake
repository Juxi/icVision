# Install script for directory: /Users/kail/imClever/dev/iCubRoadMapDemo

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
    FILE(INSTALL DESTINATION "/Users/kail/imClever/dev/iCubRoadMapDemo/bin" TYPE EXECUTABLE FILES "/Users/kail/imClever/dev/iCubRoadMapDemo/xcode/Debug/RoadmapDemo")
    IF(EXISTS "$ENV{DESTDIR}/Users/kail/imClever/dev/iCubRoadMapDemo/bin/RoadmapDemo" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}/Users/kail/imClever/dev/iCubRoadMapDemo/bin/RoadmapDemo")
      IF(CMAKE_INSTALL_DO_STRIP)
        EXECUTE_PROCESS(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/Users/kail/imClever/dev/iCubRoadMapDemo/bin/RoadmapDemo")
      ENDIF(CMAKE_INSTALL_DO_STRIP)
    ENDIF()
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    FILE(INSTALL DESTINATION "/Users/kail/imClever/dev/iCubRoadMapDemo/bin" TYPE EXECUTABLE FILES "/Users/kail/imClever/dev/iCubRoadMapDemo/xcode/Release/RoadmapDemo")
    IF(EXISTS "$ENV{DESTDIR}/Users/kail/imClever/dev/iCubRoadMapDemo/bin/RoadmapDemo" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}/Users/kail/imClever/dev/iCubRoadMapDemo/bin/RoadmapDemo")
      IF(CMAKE_INSTALL_DO_STRIP)
        EXECUTE_PROCESS(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/Users/kail/imClever/dev/iCubRoadMapDemo/bin/RoadmapDemo")
      ENDIF(CMAKE_INSTALL_DO_STRIP)
    ENDIF()
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    FILE(INSTALL DESTINATION "/Users/kail/imClever/dev/iCubRoadMapDemo/bin" TYPE EXECUTABLE FILES "/Users/kail/imClever/dev/iCubRoadMapDemo/xcode/MinSizeRel/RoadmapDemo")
    IF(EXISTS "$ENV{DESTDIR}/Users/kail/imClever/dev/iCubRoadMapDemo/bin/RoadmapDemo" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}/Users/kail/imClever/dev/iCubRoadMapDemo/bin/RoadmapDemo")
      IF(CMAKE_INSTALL_DO_STRIP)
        EXECUTE_PROCESS(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/Users/kail/imClever/dev/iCubRoadMapDemo/bin/RoadmapDemo")
      ENDIF(CMAKE_INSTALL_DO_STRIP)
    ENDIF()
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  IF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    FILE(INSTALL DESTINATION "/Users/kail/imClever/dev/iCubRoadMapDemo/bin" TYPE EXECUTABLE FILES "/Users/kail/imClever/dev/iCubRoadMapDemo/xcode/RelWithDebInfo/RoadmapDemo")
    IF(EXISTS "$ENV{DESTDIR}/Users/kail/imClever/dev/iCubRoadMapDemo/bin/RoadmapDemo" AND
       NOT IS_SYMLINK "$ENV{DESTDIR}/Users/kail/imClever/dev/iCubRoadMapDemo/bin/RoadmapDemo")
      IF(CMAKE_INSTALL_DO_STRIP)
        EXECUTE_PROCESS(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/Users/kail/imClever/dev/iCubRoadMapDemo/bin/RoadmapDemo")
      ENDIF(CMAKE_INSTALL_DO_STRIP)
    ENDIF()
  ENDIF("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
ELSE(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
ENDIF(CMAKE_INSTALL_COMPONENT)

FILE(WRITE "/Users/kail/imClever/dev/iCubRoadMapDemo/xcode/${CMAKE_INSTALL_MANIFEST}" "")
FOREACH(file ${CMAKE_INSTALL_MANIFEST_FILES})
  FILE(APPEND "/Users/kail/imClever/dev/iCubRoadMapDemo/xcode/${CMAKE_INSTALL_MANIFEST}" "${file}\n")
ENDFOREACH(file)
