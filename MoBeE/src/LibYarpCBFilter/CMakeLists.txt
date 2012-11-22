# Copyright: (C) 2010 Gregor Kaufmann
# Author: Gregor Kaufmann
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

PROJECT ( CBFilter )

SET ( CBFilter_VERSION_MAJOR 1 )
SET ( CBFilter_VERSION_MINOR 0 )

MESSAGE(STATUS "*** Yarp Control Board Filter Library ***")

### Find libraries ###
FIND_PACKAGE ( YARP REQUIRED )
	SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH};${YARP_MODULE_PATH})
FIND_PACKAGE ( ACE REQUIRED )

INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES ( ${ACE_INCLUDE_DIR} )

SET ( headers 	yarp/os/impl/RpcFilterImpl.h
		yarp/os/RpcFilter.h
		yarp/os/impl/StreamFilterImpl.h
		yarp/os/StreamFilter.h
		yarp/os/ControlBoardFilter.h
		yarp/os/IObserver.h
		yarp/os/IReplier.h
		yarp/os/RpcFilter.h
)

ADD_LIBRARY ( CBFilter STATIC	RpcFilterImpl.cpp
				RpcFilter.cpp
				StreamFilterImpl.cpp
				StreamFilter.cpp
				ControlBoardFilter.cpp 
				${headers}
)

TARGET_LINK_LIBRARIES ( CBFilter 	${YARP_LIBRARIES}
					${ACE_LIBRARY}
)

INSTALL ( TARGETS CBFilter DESTINATION ${CMAKE_SOURCE_DIR}/lib )
INSTALL ( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/yarp DESTINATION ${CMAKE_SOURCE_DIR}/include )

