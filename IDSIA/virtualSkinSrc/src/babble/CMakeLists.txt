#
# Derived from a file from the YARP distribution
#
# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

FIND_PACKAGE ( YARP REQUIRED )

INCLUDE_DIRECTORIES ( "${CMAKE_SOURCE_DIR}/src/robotModel/" )

SET ( ROBOT_MODEL_LIB "${CMAKE_BINARY_DIR}/src/robotModel/librobotModel.a" )

SET ( src_files	main.cpp
		partBabbler.cpp
		iCubBabbler.cpp
)

SET ( hdr_files	partBabbler.h
		iCubBabbler.h
)

#SET ( QT_MOC_FILES )	
#QT4_WRAP_CPP( src_files ${QT_MOC_FILES} )

ADD_EXECUTABLE ( babble ${hdr_files} ${src_files} )
TARGET_LINK_LIBRARIES ( babble ${YARP_LIBRARIES} ${QT_LIBRARIES} ${ROBOT_MODEL_LIB} )

# Tell XCode not to use /Debug and /Release directories for the binaries it builds
SET_TARGET_PROPERTIES ( babble PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})

INSTALL ( TARGETS babble DESTINATION ${CMAKE_SOURCE_DIR}/bin )