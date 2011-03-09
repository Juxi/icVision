# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

SET ( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cMakeScripts )

### Find Libraries ###
FIND_PACKAGE ( YARP REQUIRED )
FIND_PACKAGE ( FreeSOLID REQUIRED )
FIND_PACKAGE ( OpenGL REQUIRED )
FIND_PACKAGE ( Qt4 REQUIRED )
	SET (Q_WS_MAC FALSE)
	SET (Q_WS_X11 FALSE)
	SET (Q_WS_WIN FALSE)
	SET ( QT_USE_QTOPENGL TRUE )
	SET ( QT_USE_QTXML TRUE )
	INCLUDE ( ${QT_USE_FILE} )

### Platform Specific Config ###
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

INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/VirtualSkinLibrary" )	
INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/VirtualSkinLibrary/RobotModel" )
INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/VirtualSkinLibrary/YarpFilter" )

### Paths to static libraries built by sub-projects ###
SET ( VIRTUAL_SKIN_LIB "VirtualSkin" )
SET ( ROBOT_MODEL_LIB "RobotModel" )
SET ( YARP_FILTER_LIB "YarpFilter" )

### Parse the directories where build targets are ###
ADD_SUBDIRECTORY ( VirtualSkinLibrary )
ADD_SUBDIRECTORY ( ReflexDemo )
ADD_SUBDIRECTORY ( iCubBabble )
