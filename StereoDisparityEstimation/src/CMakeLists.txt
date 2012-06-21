# Copyright: (C) 2011 Juxi Leitner
# Author: Juxi Leitner <juxi.leitner@gmail.com>
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.8 )

### Create some options for use with ccmake ###
option (LINK_ACE ON)
IF ( LINK_ACE )
        FIND_PACKAGE ( ACE REQUIRED )
        INCLUDE_DIRECTORIES ( ${ACE_INCLUDE_DIR} )
ENDIF ( LINK_ACE )

### Find External Libraries ###
FIND_PACKAGE ( YARP REQUIRED )
FIND_PACKAGE ( OpenCV REQUIRED )

INCLUDE_DIRECTORIES (${YARP_INCLUDE_DIRS})
INCLUDE_DIRECTORIES (${ICUB_INCLUDE_DIRS})

### Add sources
SET ( src_files ##DepthOO.cpp
		DepthTestDisplay.cpp
		DepthYARP.cpp
		DisparityMapper.cpp
		LinFilter.cpp
		stdafx.cpp
)

SET ( hdr_files DepthTestDisplay.h
		DepthYARP.h
		DisparityMapper.h
		Filter.h
		LinFilter.h
		stdafx.h
)

ADD_EXECUTABLE ( ${PROJECTNAME} ${hdr_files} ${src_files} main.cpp )
TARGET_LINK_LIBRARIES (
	${PROJECTNAME}	${YARP_LIBRARIES} ${OpenCV_LIBS}
)

ADD_EXECUTABLE ( ${PROJECTNAME}CLS ${hdr_files} ${src_files} mainCLS.cpp )
TARGET_LINK_LIBRARIES (
	${PROJECTNAME}CLS ${YARP_LIBRARIES} ${OpenCV_LIBS}
)
