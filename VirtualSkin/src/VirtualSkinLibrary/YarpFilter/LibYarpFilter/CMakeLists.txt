# Copyright: (C) 2011 Gregor Kaufmann
# Author: Gregor Kaufmann
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

SET ( headers 	yarp/os/impl/RpcFilterImpl.h
		yarp/os/RpcFilter.h
		yarp/os/impl/StreamFilterImpl.h
		yarp/os/StreamFilter.h
		yarp/os/ControlBoardFilter.h
		yarp/os/IObserver.h
		yarp/os/IReplier.h
		yarp/os/RpcFilter.h
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

