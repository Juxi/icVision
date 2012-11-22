# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

PROJECT ( RobotFilter )

SET ( RobotFilter_VERSION_MAJOR 1 )
SET ( RobotFilter_VERSION_MINOR 0 )

MESSAGE(STATUS "*** Robot Filter Library ***")

SET ( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../CMakeScripts )

### Include Paths ###
INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES ( ${ACE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/../LibYarpModel" )
INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/../LibKinematicModel" )
INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/../LibYarpCBFilter" )

### Find Libraries ###
FIND_PACKAGE ( YARP REQUIRED )
	SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${YARP_MODULE_PATH})
	#MESSAGE(${CMAKE_MODULE_PATH})
FIND_PACKAGE ( ACE REQUIRED )
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

###############################################################

SET ( src_files callObserver.cpp
		 circularBuffer.cpp
		 filterRpcInterface.cpp
		 responseObserver.cpp
		 robotFilter.cpp
		 stateObserver.cpp
)

SET ( hdr_files	 callObserver.h
		 circularBuffer.h
		 filterRpcInterface.h
		 responseObserver.h
		 robotFilter.h
		 stateObserver.h
		
)

SET ( QT_MOC_FILES robotFilter.h
		    responseObserver.h
		    stateObserver.h
)

QT4_WRAP_CPP( src_files ${QT_MOC_FILES} )

ADD_LIBRARY ( RobotFilter STATIC ${hdr_files} ${src_files} )

ADD_DEPENDENCIES( RobotFilter KinematicModel CBFilter )

TARGET_LINK_LIBRARIES ( RobotFilter KinematicModel
				    CBFilter
				    ${FREE_SOLID_LIBRARY}
				    ${QT_LIBRARIES}
				    ${OPENGL_LIBRARIES}
				    ${YARP_LIBRARIES}
)

INSTALL ( TARGETS RobotFilter DESTINATION ${CMAKE_SOURCE_DIR}/lib )

ADD_SUBDIRECTORY( ../LibYarpCBFilter "${CMAKE_CURRENT_BINARY_DIR}/LibYarpCBFilter")

