# Copyright: (C) 2011 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

### Find External Libraries ###
FIND_PACKAGE ( Qt4 REQUIRED )
FIND_PACKAGE ( YARP REQUIRED )
#INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} )

SET ( src_files	main.cpp
		partController.cpp
		partBabbler.cpp
		iCubBabbler.cpp
)

SET ( hdr_files	partController.h
		partBabbler.h
		iCubBabbler.h
)

SET ( QT_MOC_FILES 	iCubBabbler.h
)	
QT4_WRAP_CPP( src_files ${QT_MOC_FILES} )

ADD_EXECUTABLE ( iCubBabble ${hdr_files} ${src_files} )

TARGET_LINK_LIBRARIES ( iCubBabble 	${QT_LIBRARIES}
					${YARP_LIBRARIES}
)

INSTALL ( TARGETS iCubBabble DESTINATION ${CMAKE_SOURCE_DIR}/bin )

