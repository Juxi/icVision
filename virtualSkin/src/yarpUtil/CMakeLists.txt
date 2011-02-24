# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

SET ( src_files	partController.cpp
		yarpRpcPort.cpp
		worldRpcInterface.cpp
		yarpStreamPort.cpp
)

SET ( hdr_files	partController.h
		yarpRpcPort.h
		worldRpcInterface.h
		yarpStreamPort.h
)

SET ( QT_MOC_FILES 	yarpRpcPort.h
			yarpStreamPort.h
)	
QT4_WRAP_CPP( src_files ${QT_MOC_FILES} )

ADD_LIBRARY ( yarpUtil STATIC ${hdr_files} ${src_files} )

TARGET_LINK_LIBRARIES ( yarpUtil ${YARP_LIBRARIES} ${QT_LIBRARIES} ${OPENGL_LIBRARIES} ${ACE_LIBRARY} )

INSTALL ( TARGETS   yarpUtil 	 DESTINATION ${CMAKE_SOURCE_DIR}/lib )
INSTALL ( FILES     ${hdr_files} DESTINATION ${CMAKE_SOURCE_DIR}/include/yarpUtil)

ADD_SUBDIRECTORY ( gregorFilter )

