# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.


SET( PARSED ${PARSED_LIB_YARP_MODEL} )
IF( NOT PARSED )


CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

PROJECT ( YarpModel )
SET ( YarpModel_VERSION_MAJOR 1 )
SET ( YarpModel_VERSION_MINOR 0 )

MESSAGE(STATUS "*** Yarpified Kinematic Model Library ***")

SET ( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../CMakeScripts )

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

### Include Paths ###
INCLUDE ( ${QT_USE_FILE} )
INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES ( ${ACE_INCLUDE_DIR} )
INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/../LibKinematicModel" )

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

########################################################

SET ( src_files		yarpModel.cpp
			yarprobot.cpp
			worldRpcInterface.cpp
			yarpStreamPort.cpp
			exception.cpp
			simSyncer.cpp
			robotSyncer.cpp
)

SET ( hdr_files		constants.h
			exception.h
			yarpModel.h
			yarprobot.h
			worldRpcInterface.h
			yarpStreamPort.h
			simSyncer.h
			robotSyncer.h
		
)

ADD_LIBRARY ( YarpModel STATIC ${hdr_files} ${src_files} )

ADD_DEPENDENCIES( YarpModel KinematicModel )

TARGET_LINK_LIBRARIES ( YarpModel KinematicModel
				  ${SOLID_LIBRARY}
				  ${QT_LIBRARIES}
				  ${OPENGL_LIBRARIES}
				    ${YARP_LIBRARIES}
)

INSTALL ( TARGETS YarpModel DESTINATION ${CMAKE_SOURCE_DIR}/lib )

ADD_SUBDIRECTORY ( ../LibKinematicModel ${CMAKE_CURRENT_BINARY_DIR}/LibKinematicModel )



SET( PARSED_LIB_YARP_MODEL true PARENT_SCOPE)
ENDIF( NOT PARSED )

