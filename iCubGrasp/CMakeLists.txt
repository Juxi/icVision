# Copyright: (C) 2011 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

PROJECT ( iCubGrasp )

SET ( iCubGrasp_VERSION_MAJOR 1 )
SET ( iCubGrasp_VERSION_MINOR 0 )

FIND_PACKAGE ( YARP REQUIRED )
FIND_PACKAGE ( Qt4 REQUIRED )
INCLUDE ( ${QT_USE_FILE} )
INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} )

SET ( src_files	main.cpp
		handController.cpp
		touchthread.cpp
)

SET ( hdr_files	handController.h
		touchthread.h
)

SET ( QT_MOC_FILES 	
)	
QT4_WRAP_CPP( src_files ${QT_MOC_FILES} )

ADD_EXECUTABLE ( iCubGrasp ${hdr_files} ${src_files} )

TARGET_LINK_LIBRARIES ( iCubGrasp 	${QT_LIBRARIES}
					${YARP_LIBRARIES}
)

INSTALL ( TARGETS iCubGrasp DESTINATION ${CMAKE_SOURCE_DIR}/bin )

