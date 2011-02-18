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

ADD_EXECUTABLE ( iCubBabble ${hdr_files} ${src_files} )

ADD_DEPENDENCIES( iCubBabble yarpUtil )

TARGET_LINK_LIBRARIES ( iCubBabble 	${YARP_UTILITIES_LIB}
					${QT_LIBRARIES}
					${YARP_LIBRARIES}
					${ACE_LIBRARY}
)

INSTALL ( TARGETS iCubBabble DESTINATION ${CMAKE_SOURCE_DIR}/bin )

