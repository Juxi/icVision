# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

SET ( src_files	yarpModel.cpp
		stateObserver.cpp
		responseObserver.cpp
		robotFilter.cpp
		callObserver.cpp
		circularBuffer.cpp
		yarpRpcPort.cpp
		worldRpcInterface.cpp
		yarpStreamPort.cpp
		virtualskinexception.cpp
)

SET ( hdr_files	yarpModel.h
		stateObserver.h
		responseObserver.h
		robotFilter.h
		callObserver.h
		circularBuffer.h
		yarpRpcPort.h
		worldRpcInterface.h
		yarpStreamPort.h
		constants.h
		virtualskinexception.h
)

SET ( QT_MOC_FILES 	robotFilter.h
			responseObserver.h
			stateObserver.h
)

#QT4_WRAP_CPP( src_files ${QT_MOC_FILES} )

#ADD_LIBRARY ( VirtualSkin STATIC ${hdr_files} ${src_files} )

#ADD_DEPENDENCIES( VirtualSkin RobotModel YarpFilter )

#TARGET_LINK_LIBRARIES ( VirtualSkin 	RobotModel
#					YarpFilter
#					${FREE_SOLID_LIBRARY}
#					${QT_LIBRARIES}
#					${OPENGL_LIBRARIES}
#					${YARP_LIBRARIES}
#)

#INSTALL ( TARGETS VirtualSkin DESTINATION ${CMAKE_SOURCE_DIR}/lib )

ADD_SUBDIRECTORY ( RobotModel )
#ADD_SUBDIRECTORY ( YarpFilter )

