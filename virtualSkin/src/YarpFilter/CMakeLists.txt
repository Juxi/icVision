# Copyright: (C) 2010 Gregor Kaufmann
# Author: Gregor Kaufmann
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

MESSAGE(STATUS "*** Sub-Project YarpFilter ***")

PROJECT ( YarpFilter )

SET ( YarpFilter_VERSION_MAJOR 0 )
SET ( YarpFilter_VERSION_MINOR 1 )

### Create some options for use with ccmake ###
option (LINK_ACE OFF)

### Location of script to find Ace ###
SET ( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../cMakeScripts )

### Find libraries ###
FIND_PACKAGE ( YARP REQUIRED )
INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} )

IF ( LINK_ACE )
	FIND_PACKAGE ( ACE REQUIRED )
	INCLUDE_DIRECTORIES ( ${ACE_INCLUDE_DIR} )
ENDIF ( LINK_ACE )

SET ( headers 	yarp/os/impl/RpcFilterImpl.h
		yarp/os/RpcFilter.h
		yarp/os/impl/StreamFilterImpl.h
		yarp/os/StreamFilter.h
		yarp/os/ControlBoardFilter.h
)

ADD_LIBRARY ( YarpFilter STATIC	RpcFilterImpl.cpp
				RpcFilter.cpp
				StreamFilterImpl.cpp
				StreamFilter.cpp
				ControlBoardFilter.cpp 
				${headers}
)

TARGET_LINK_LIBRARIES ( YarpFilter 	${YARP_LIBRARIES}
					${ACE_LIBRARY}
)

INSTALL ( TARGETS YarpFilter DESTINATION ${CMAKE_SOURCE_DIR}/lib )
INSTALL ( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/yarp DESTINATION ${CMAKE_SOURCE_DIR}/include )

