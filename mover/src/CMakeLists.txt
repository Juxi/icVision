# Copyright: (C) 2012 Leo Pape
# Author: Leo Pape
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

PROJECT ( iCubMover )

SET ( iCubMover_VERSION_MAJOR 1 )
SET ( iCubMover_VERSION_MINOR 0 )

FIND_PACKAGE( ICUB )
FIND_PACKAGE ( YARP REQUIRED )
INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} ${ICUB_INCLUDE_DIRS} )

SET ( src_files	main.cpp
		mover.cpp
		moverPosition.cpp
		moverMinJerkLinear.cpp
		moverMinJerkForward.cpp
		moverVelocityForward.cpp
)

SET ( hdr_files	mover.h
		moverPosition.h
		moverMinJerkLinear.h
		moverMinJerkForward.h
		moverVelocityForward.h
		vectormath.h
)

ADD_EXECUTABLE ( iCubMover ${hdr_files} ${src_files} )

TARGET_LINK_LIBRARIES ( iCubMover ${YARP_LIBRARIES} iKin )

INSTALL ( TARGETS iCubMover DESTINATION ${CMAKE_SOURCE_DIR}/bin )
