# Copyright: (C) 2011 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

SET ( src_files	main.cpp
		partBabbler.cpp
		iCubBabbler.cpp
)

SET ( hdr_files	partBabbler.h
		iCubBabbler.h
)

SET ( QT_MOC_FILES 	iCubBabbler.h
)	
QT4_WRAP_CPP( src_files ${QT_MOC_FILES} )

ADD_EXECUTABLE ( iCubBabble ${hdr_files} ${src_files} )

ADD_DEPENDENCIES( iCubBabble yarpUtil )

TARGET_LINK_LIBRARIES ( iCubBabble 	${YARP_UTILITIES_LIB}
					#${YARP_LIBRARIES}
					${QT_LIBRARIES}
)

INSTALL ( TARGETS iCubBabble DESTINATION ${CMAKE_SOURCE_DIR}/bin )

