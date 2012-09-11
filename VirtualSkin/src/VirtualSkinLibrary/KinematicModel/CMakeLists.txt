# Copyright: (C) 2011 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

PROJECT ( KinematicModel )

SET ( KinematicModel_VERSION_MAJOR 1 )
SET ( KinematicModel_VERSION_MINOR 0 )

MESSAGE(STATUS "*** KinematicModel Project ***")

SET ( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeScripts )
INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/LibKinematicModel" )

### Find Libraries ###
FIND_PACKAGE ( SOLID REQUIRED )
FIND_PACKAGE ( OpenGL )
FIND_PACKAGE ( Qt4 REQUIRED )
	SET ( QT_USE_QTOPENGL TRUE )
	SET ( QT_USE_QTXML TRUE )
	INCLUDE ( ${QT_USE_FILE} )

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


ADD_SUBDIRECTORY ( LibKinematicModel )
ADD_SUBDIRECTORY ( WorldModelDemo )
