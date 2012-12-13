# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

PROJECT ( Part_Controller_Demo )

SET ( Simple_Control_Demo_VERSION_MAJOR 1 )
SET ( Simple_Control_Demo_VERSION_MINOR 0 )

### Find Libraries ###
FIND_PACKAGE ( YARP REQUIRED )
	SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${YARP_MODULE_PATH})
	#MESSAGE(${CMAKE_MODULE_PATH})
FIND_PACKAGE ( ACE REQUIRED )

### Include Paths ###
INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} )

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

SET ( src_files main.cpp partController.cpp )
SET ( hdr_files	 partController.h )
ADD_EXECUTABLE ( simpleCtrlDemo ${hdr_files} ${src_files} )
ADD_DEPENDENCIES( simpleCtrlDemo YarpModel KinematicModel )

TARGET_LINK_LIBRARIES ( simpleCtrlDemo   ${YARP_LIBRARIES} )

INSTALL ( TARGETS simpleCtrlDemo DESTINATION ${CMAKE_SOURCE_DIR}/bin )

