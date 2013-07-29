# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

PROJECT ( PoseFinder )

SET ( PoseFinder_VERSION_MAJOR 1 )
SET ( PoseFinder_VERSION_MINOR 0 )

MESSAGE(STATUS "*** PoseFinder ***")

SET ( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../CMakeScripts )

INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/../../LibKinematicModel" )
INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/../../LibRL" )

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
	
SET ( src_files main.cpp
		 pose_finder.cpp
		sample.cpp )

SET ( hdr_files pose_finder.h
		sample.h )


ADD_EXECUTABLE ( PoseFinder ${hdr_files} ${src_files} )

ADD_DEPENDENCIES( PoseFinder KinematicModel )

TARGET_LINK_LIBRARIES ( PoseFinder 	KinematicModel
				      iCubRL
					 ${SOLID_LIBRARY}
					 ${QT_LIBRARIES}
					 ${OPENGL_LIBRARIES}			
)

INSTALL ( TARGETS PoseFinder DESTINATION ${CMAKE_SOURCE_DIR}/bin )

ADD_SUBDIRECTORY ( ../../LibKinematicModel ${CMAKE_CURRENT_BINARY_DIR}/LibKinematicModel )
