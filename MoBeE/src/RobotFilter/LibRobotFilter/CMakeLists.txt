# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

SET ( src_files callObserver.cpp
		 circularBuffer.cpp
		 filterRpcInterface.cpp
		 responseObserver.cpp
		 robotFilter.cpp
		 stateObserver.cpp
)

SET ( hdr_files	 callObserver.h
		 circularBuffer.h
		 filterRpcInterface.h
		 responseObserver.h
		 robotFilter.h
		 stateObserver.h
		
)

SET ( QT_MOC_FILES robotFilter.h
		   responseObserver.h
		   stateObserver.h
)

QT4_WRAP_CPP( src_files ${QT_MOC_FILES} )

ADD_LIBRARY ( RobotFilter STATIC ${hdr_files} ${src_files} )

ADD_DEPENDENCIES( RobotFilter KinematicModel YarpFilter )

TARGET_LINK_LIBRARIES ( RobotFilter KinematicModel
				    YarpFilter
				    ${FREE_SOLID_LIBRARY}
				    ${QT_LIBRARIES}
				    ${OPENGL_LIBRARIES}
				    ${YARP_LIBRARIES}
)

INSTALL ( TARGETS RobotFilter DESTINATION ${CMAKE_SOURCE_DIR}/lib )

ADD_SUBDIRECTORY ( YarpFilter )

