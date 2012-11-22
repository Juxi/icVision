# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

PROJECT ( FictitiousForceControl )

SET ( YarpModel_VERSION_MAJOR 1 )
SET ( YarpModel_VERSION_MINOR 0 )

SET ( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../CMakeScripts )

### Include Paths ###
INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/../../LibYarpModel" )
INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/../../LibKinematicModel" )

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



SET ( src_files		main.cpp
			partController.cpp
			controller.cpp
)

SET ( hdr_files		partController.h
                    controller.h
)

QT4_WRAP_CPP( src_files controller.h )

ADD_EXECUTABLE ( mobee	${hdr_files}
				${src_files} )

ADD_DEPENDENCIES( mobee	YarpModel 
				KinematicModel
)

TARGET_LINK_LIBRARIES ( mobee	YarpModel 
				 	KinematicModel
					${SOLID_LIBRARY}
					${QT_LIBRARIES}
					${OPENGL_LIBRARIES}
					${YARP_LIBRARIES}
)

INSTALL ( TARGETS mobee DESTINATION ${CMAKE_SOURCE_DIR}/bin )

ADD_SUBDIRECTORY ( ../../LibYarpModel ${CMAKE_CURRENT_BINARY_DIR}/LibYarpModel )
ADD_SUBDIRECTORY( Demo )

