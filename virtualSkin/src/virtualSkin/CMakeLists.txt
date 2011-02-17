# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

### openGL ###
FIND_PACKAGE ( OpenGL )

### QT ###
FIND_PACKAGE ( Qt4 REQUIRED )
SET ( QT_USE_QTOPENGL TRUE )
SET ( QT_USE_QTXML TRUE )
INCLUDE ( ${QT_USE_FILE} )

### YARP ###
FIND_PACKAGE ( YARP REQUIRED )

### HEADERS ###
INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/../robotModel/" )
INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/../robotModel/freeSolid/" )
INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/../yarpFilter/" )

### STATIC LIBRARIES 
SET ( ROBOT_MODEL_LIB "${CMAKE_CURRENT_BINARY_DIR}/../robotModel/librobotModel.a" )
SET ( IDSIA_YARP_FILTER_LIB "${CMAKE_CURRENT_BINARY_DIR}/../yarpFilter/libyarpFilter.a" )

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
TARGET_LINK_LIBRARIES ( virtualSkin ${ROBOT_MODEL_LIB} ${IDSIA_YARP_FILTER_LIB} FreeSOLID ${QT_LIBRARIES} ${OPENGL_LIBRARIES} ${YARP_LIBRARIES} )

# Tell XCode not to use /Debug and /Release directories for the binaries it builds
SET_TARGET_PROPERTIES ( virtualSkin PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})

INSTALL ( TARGETS virtualSkin DESTINATION ${CMAKE_SOURCE_DIR}/bin )
