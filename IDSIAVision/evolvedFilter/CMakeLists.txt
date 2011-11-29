# Copyright: (C) 2011 Juxi Leitner
# Author: Juxi Leitner <juxi.leitner@gmail.com>
# CopyPolicy: Released under the terms of the GNU GPL v2.0

### Find External Libraries ###
FIND_PACKAGE ( YARP REQUIRED )
FIND_PACKAGE ( OpenCV REQUIRED )

INCLUDE_DIRECTORIES (${YARP_INCLUDE_DIRS})
INCLUDE_DIRECTORIES (${ICUB_INCLUDE_DIRS})

SET(PROJECTNAME evolvedFilter )
PROJECT ( ${PROJECTNAME} )

### Create some options for use with ccmake ###
option (LINK_ACE ON)
IF ( LINK_ACE )
        FIND_PACKAGE ( ACE REQUIRED )
        INCLUDE_DIRECTORIES ( ${ACE_INCLUDE_DIR} )
ENDIF ( LINK_ACE )

### Find External Libraries ###
FIND_PACKAGE ( OpenCV REQUIRED )
FIND_PACKAGE ( YARP REQUIRED )
INCLUDE_DIRECTORIES (${YARP_INCLUDE_DIRS})
INCLUDE_DIRECTORIES (${ICUB_INCLUDE_DIRS})
INCLUDE_DIRECTORIES (${OpenCV_INCLUDE_DIRS})

### Add sources
SET ( src_files main.cpp
		GpImage.cpp
		GaborImage.cpp
		
		evolved_filter_module.cpp
		evolved_filters/blue_detector.cpp
		evolved_filters/redbox_detector.cpp
		evolved_filters/teabox.cpp		
)


SET ( hdr_files evolved_filter_module.h boolList.h GpImage.h GaborImage.h
		evolved_filters/blue_detector.h
		evolved_filters/redbox_detector.h
		evolved_filters/teabox.h
)

ADD_EXECUTABLE ( ${PROJECTNAME} ${hdr_files} ${src_files} )

TARGET_LINK_LIBRARIES (
	${PROJECTNAME}	${YARP_LIBRARIES}
			${OpenCV_LIBS}
)
