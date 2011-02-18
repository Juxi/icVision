# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

### Create some options for use with ccmake ###
option (LINK_ACE "If you have problems finding ace, turn this on" OFF)
option (YARP_AUTO_LINK "... and this off" ON)

### Location of scripts to find Ace and FreeSOLID ###
SET ( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cMakeScripts )

### Locations of headers contained within this project ###
INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/robotModel/" )
INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/yarpUtil/" )
INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/yarpUtil/gregorFilter/" )

### Names of static libraries built by this project ###
SET ( ROBOT_MODEL_LIB "robotModel" )
SET ( YARP_UTILITIES_LIB "yarpUtil" )
SET ( IDSIA_YARP_FILTER_LIB "yarpFilter" )

### Find libraries ###
FIND_PACKAGE ( OpenGL )
FIND_PACKAGE ( Qt4 REQUIRED )
	SET ( QT_USE_QTOPENGL TRUE )
	SET ( QT_USE_QTXML TRUE )
	INCLUDE ( ${QT_USE_FILE} )
FIND_PACKAGE ( FreeSOLID REQUIRED )
FIND_PACKAGE ( YARP REQUIRED )
IF ( LINK_ACE )
	FIND_PACKAGE ( ACE REQUIRED )
ENDIF ( LINK_ACE )

### Set some platform-specific variables ###
SET (Q_WS_MAC FALSE)
SET (Q_WS_X11 FALSE)
SET (Q_WS_WIN FALSE)
IF ( UNIX )
	IF ( APPLE )
		SET (Q_WS_MAC TRUE)
		SET (QT_MAC_USE_COCOA TRUE)
	ELSE ( APPLE )
		SET (Q_WS_X11 TRUE)
	ENDIF ( APPLE )
ENDIF ( UNIX )
IF ( WIN32 )
	SET (Q_WS_WIN TRUE)
	ADD_DEFINITIONS( /D "WIN32_LEAN_AND_MEAN" )
ENDIF ( WIN32 )

### Include the directories where build targets are ###
ADD_SUBDIRECTORY ( yarpUtil )
ADD_SUBDIRECTORY ( robotModel )
ADD_SUBDIRECTORY ( reflex )
ADD_SUBDIRECTORY ( iCubBabbler )
