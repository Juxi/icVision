# Copyright: (C) 2010 Gregor Kaufmann
# Author: Gregor Kaufmann
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

PROJECT ( yarpFilter )
SET ( yarpFilter_VERSION_MAJOR 1 )
SET ( yarpFilter_VERSION_MINOR 0 )

FIND_PACKAGE ( YARP REQUIRED )
INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES ( "${CMAKE_SOURCE_DIR}/src/modules/yarpfilter/" )

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

# Tell XCode not to put built targets in /Debug or /Release directories 
SET_TARGET_PROPERTIES ( yarpFilter PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})

INSTALL ( TARGETS yarpFilter DESTINATION ${CMAKE_SOURCE_DIR}/lib )
INSTALL ( DIRECTORY ${CMAKE_SOURCE_DIR}/src/yarpFilter/yarp DESTINATION ${CMAKE_SOURCE_DIR}/include )
