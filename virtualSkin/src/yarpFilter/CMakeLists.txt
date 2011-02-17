# Copyright: (C) 2010 Gregor Kaufmann
# Author: Gregor Kaufmann
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

PROJECT ( yarpFilter )
SET ( yarpFilter_VERSION_MAJOR 0 )
SET ( yarpFilter_VERSION_MINOR 1 )

FIND_PACKAGE ( ACE REQUIRED )
INCLUDE_DIRECTORIES ( ${ACE_INCLUDE_DIR} )

FIND_PACKAGE ( YARP REQUIRED )
INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} )

INCLUDE_DIRECTORIES ( "${CMAKE_SOURCE_DIR}/modules/yarpfilter/" )

SET ( headers 	yarp/os/impl/RpcFilterImpl.h
		yarp/os/RpcFilter.h
		yarp/os/impl/StreamFilterImpl.h
		yarp/os/StreamFilter.h
		yarp/os/ControlBoardFilter.h )

ADD_LIBRARY ( yarpFilter 	RpcFilterImpl.cpp
				RpcFilter.cpp
				StreamFilterImpl.cpp
				StreamFilter.cpp
				ControlBoardFilter.cpp 
				${headers} )

TARGET_LINK_LIBRARIES ( yarpFilter ${YARP_LIBRARIES} )

# Tell XCode not to put built targets in /Debug or /Release directories 
SET_TARGET_PROPERTIES ( yarpFilter PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})

INSTALL ( TARGETS yarpFilter DESTINATION ${CMAKE_SOURCE_DIR}/lib )
INSTALL ( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/yarp DESTINATION ${CMAKE_SOURCE_DIR}/include )
