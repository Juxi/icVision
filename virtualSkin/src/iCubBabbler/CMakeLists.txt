# Copyright: (C) 2011 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

### OpenGL ###
FIND_PACKAGE ( OpenGL )

### QT ###
FIND_PACKAGE ( Qt4 REQUIRED )
SET ( QT_USE_QTOPENGL TRUE )
SET ( QT_USE_QTXML TRUE )
INCLUDE ( ${QT_USE_FILE} )

### YARP ###
FIND_PACKAGE ( YARP REQUIRED )

INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/../robotModel/" )

SET ( ROBOT_MODEL_LIB "${CMAKE_CURRENT_BINARY_DIR}/../robotModel/librobotModel.a" )

SET ( src_files	main.cpp
		partBabbler.cpp
		iCubBabbler.cpp
)

SET ( hdr_files	partBabbler.h
		iCubBabbler.h
)

ADD_EXECUTABLE ( iCubBabble ${hdr_files} ${src_files} )
ADD_DEPENDENCIES( iCubBabble robotModel )
TARGET_LINK_LIBRARIES ( iCubBabble ${ROBOT_MODEL_LIB} FreeSOLID ${QT_LIBRARIES} ${OPENGL_LIBRARIES} ${YARP_LIBRARIES} )

# Tell XCode not to use /Debug and /Release directories for the binaries it builds
SET_TARGET_PROPERTIES ( iCubBabble PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})

INSTALL ( TARGETS iCubBabble DESTINATION ${CMAKE_SOURCE_DIR}/bin )
