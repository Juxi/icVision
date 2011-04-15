# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

SET ( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/VirtualSkinLibrary/RobotModel/CMakeScripts )

### Find Libraries ###
FIND_PACKAGE ( YARP REQUIRED )
FIND_PACKAGE ( SOLID REQUIRED )
FIND_PACKAGE ( OpenGL )
FIND_PACKAGE ( Qt4 REQUIRED )
	SET ( QT_USE_QTOPENGL TRUE )
	SET ( QT_USE_QTXML TRUE )

### Include Paths ###
INCLUDE ( ${QT_USE_FILE} )
INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/VirtualSkinLibrary" )
INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/VirtualSkinLibrary/RobotModel/LibRobotModel" )
INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/VirtualSkinLibrary/YarpFilter/LibYarpFilter" )

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
ENDIF ( WIN32 )

### Parse the directories where build targets are ###
ADD_SUBDIRECTORY ( VirtualSkinLibrary )
ADD_SUBDIRECTORY ( ReflexDemo )
ADD_SUBDIRECTORY ( iCubBabble )
