# Copyright: (C) 2012 Leo Pape
# Author: Leo Pape
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

PROJECT ( safeCartesian )

SET ( safeCartesian_VERSION_MAJOR 1 )
SET ( safeCartesian_VERSION_MINOR 0 )

FIND_PACKAGE ( YARP REQUIRED )

INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} )

SET ( src_files	main.cpp
		cartesian.cpp
		portmonitor.cpp
)

SET ( hdr_files	cartesian.h portmonitor.h
)

ADD_EXECUTABLE ( safeCartesian  ${hdr_files} ${src_files} )

TARGET_LINK_LIBRARIES ( safeCartesian ${YARP_LIBRARIES} )


INSTALL ( TARGETS safeCartesian DESTINATION ${CMAKE_SOURCE_DIR}/bin )