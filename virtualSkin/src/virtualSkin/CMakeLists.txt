# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

### MSVC WinSocket vs. WinSocket2 issue ###
IF ( WIN32 )
	ADD_DEFINITIONS( /D "WIN32_LEAN_AND_MEAN" )
ENDIF( WIN32 )

### openGL ###
FIND_PACKAGE ( OpenGL )

### QT ###
FIND_PACKAGE ( Qt4 REQUIRED )
SET ( QT_USE_QTOPENGL TRUE )
SET ( QT_USE_QTXML TRUE )
INCLUDE ( ${QT_USE_FILE} )

### FreeSOLID ###
FIND_PACKAGE ( FreeSOLID REQUIRED )
INCLUDE_DIRECTORIES ( "${FREE_SOLID_INCLUDE_DIR}" )

### ACE ###
FIND_PACKAGE ( ACE REQUIRED )

### YARP ###
FIND_PACKAGE ( YARP REQUIRED )

### HEADERS ###
INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/../robotModel/" )
INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/../yarpFilter/" )

### STATIC LIBRARIES 
SET ( ROBOT_MODEL_LIB "robotModel" )
SET ( IDSIA_YARP_FILTER_LIB "yarpFilter" )

SET ( src_files	window.cpp
		glwidget.cpp
		collisionDetector.cpp
		main.cpp
		StateObserver.cpp
		ResponseObserver.cpp
		RobotFilter.cpp
		CallObserver.cpp
		circularBuffer.cpp
		yarpStreamPort.cpp )

SET ( hdr_files	window.h
		glwidget.h
		collisionDetector.h
		StateObserver.h
		ResponseObserver.h
		RobotFilter.h
		CallObserver.h
		circularBuffer.h
		yarpStreamPort.h )

SET ( QT_MOC_FILES 	window.h
			glwidget.h
			RobotFilter.h
			collisionDetector.h
			yarpStreamPort.h )	
QT4_WRAP_CPP( src_files ${QT_MOC_FILES} )

ADD_EXECUTABLE ( virtualSkin ${hdr_files} ${src_files} )
ADD_DEPENDENCIES( virtualSkin robotModel yarpFilter )
TARGET_LINK_LIBRARIES ( virtualSkin ${ROBOT_MODEL_LIB} ${IDSIA_YARP_FILTER_LIB} ${FREE_SOLID_LIB} ${QT_LIBRARIES} ${OPENGL_LIBRARIES} ${YARP_LIBRARIES} ${ACE_LIBRARIES} )

# Tell XCode not to use /Debug and /Release directories for the binaries it builds
SET_TARGET_PROPERTIES ( virtualSkin PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})

INSTALL ( TARGETS virtualSkin DESTINATION ${CMAKE_SOURCE_DIR}/bin )
