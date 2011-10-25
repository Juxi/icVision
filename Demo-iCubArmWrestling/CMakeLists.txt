# Copyright: (C) 2011 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

PROJECT ( armWrestling )

SET ( iCubGrasp_VERSION_MAJOR 1 )
SET ( iCubGrasp_VERSION_MINOR 0 )

FIND_PACKAGE ( YARP REQUIRED )
FIND_PACKAGE ( Qt4 REQUIRED )
INCLUDE ( ${QT_USE_FILE} )
INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} )

SET ( src_files	main.cpp
		armController.cpp
		headController.cpp
		torsoController.cpp
)

SET ( hdr_files	armController.h headController.h torsoController.h )

SET ( QT_MOC_FILES 	
)	
QT4_WRAP_CPP( src_files ${QT_MOC_FILES} )

ADD_EXECUTABLE ( armWrestling ${hdr_files} ${src_files} )

TARGET_LINK_LIBRARIES ( armWrestling 	${QT_LIBRARIES}
					${YARP_LIBRARIES}
)

INSTALL ( TARGETS armWrestling DESTINATION ${CMAKE_SOURCE_DIR}/bin )

