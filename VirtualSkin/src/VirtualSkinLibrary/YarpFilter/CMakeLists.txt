# Copyright: (C) 2010 Gregor Kaufmann
# Author: Gregor Kaufmann
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

PROJECT ( YarpFilter )

SET ( YarpFilter_VERSION_MAJOR 1 )
SET ( YarpFilter_VERSION_MINOR 0 )

MESSAGE(STATUS "*** Sub-Project YarpFilter ***")

### Find libraries ###
FIND_PACKAGE ( YARP REQUIRED )
	SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH};${YARP_MODULE_PATH})
FIND_PACKAGE ( ACE REQUIRED )

INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES ( ${ACE_INCLUDE_DIR} )

ADD_SUBDIRECTORY ( LibYarpFilter )
ADD_SUBDIRECTORY ( Demo )
