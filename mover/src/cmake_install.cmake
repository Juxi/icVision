# Install script for directory: /home/stollenga/workspace/IM-CLeVeR/mover/src

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

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  IF(EXISTS "$ENV{DESTDIR}/home/stollenga/workspace/IM-CLeVeR/mover/src/bin/iCubMover" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/stollenga/workspace/IM-CLeVeR/mover/src/bin/iCubMover")
    FILE(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/stollenga/workspace/IM-CLeVeR/mover/src/bin/iCubMover"
         RPATH "")
  ENDIF()
  list(APPEND CPACK_ABSOLUTE_DESTINATION_FILES
   "/home/stollenga/workspace/IM-CLeVeR/mover/src/bin/iCubMover")
FILE(INSTALL DESTINATION "/home/stollenga/workspace/IM-CLeVeR/mover/src/bin" TYPE EXECUTABLE FILES "/home/stollenga/workspace/IM-CLeVeR/mover/src/iCubMover")
  IF(EXISTS "$ENV{DESTDIR}/home/stollenga/workspace/IM-CLeVeR/mover/src/bin/iCubMover" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/stollenga/workspace/IM-CLeVeR/mover/src/bin/iCubMover")
    IF(CMAKE_INSTALL_DO_STRIP)
      EXECUTE_PROCESS(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/stollenga/workspace/IM-CLeVeR/mover/src/bin/iCubMover")
    ENDIF(CMAKE_INSTALL_DO_STRIP)
  ENDIF()
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
ELSE(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
ENDIF(CMAKE_INSTALL_COMPONENT)

FILE(WRITE "/home/stollenga/workspace/IM-CLeVeR/mover/src/${CMAKE_INSTALL_MANIFEST}" "")
FOREACH(file ${CMAKE_INSTALL_MANIFEST_FILES})
  FILE(APPEND "/home/stollenga/workspace/IM-CLeVeR/mover/src/${CMAKE_INSTALL_MANIFEST}" "${file}\n")
ENDFOREACH(file)
