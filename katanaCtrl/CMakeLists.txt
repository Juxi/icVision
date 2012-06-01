# Copyright: (C) 2011 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

PROJECT ( katanaCtrl )

FIND_PACKAGE ( YARP REQUIRED )
INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} )

SET ( src_files	main.cpp
		jointController.cpp
)

SET ( hdr_files	jointController.h )



ADD_EXECUTABLE ( katanaCtrl ${hdr_files} ${src_files} )

TARGET_LINK_LIBRARIES ( katanaCtrl 	${YARP_LIBRARIES} )

INSTALL ( TARGETS katanaCtrl DESTINATION ${CMAKE_SOURCE_DIR}/bin )

