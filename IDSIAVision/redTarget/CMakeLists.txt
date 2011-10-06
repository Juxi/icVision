# Copyright: (C) 2011 Juxi Leitner
# Author: Juxi Leitner <juxi.leitner@gmail.com>
# CopyPolicy: Released under the terms of the GNU GPL v2.0

### Find External Libraries ###
FIND_PACKAGE ( YARP REQUIRED )
#FIND_PACKAGE ( ICUB )
FIND_PACKAGE ( OpenCV REQUIRED )

INCLUDE_DIRECTORIES (${YARP_INCLUDE_DIRS})
INCLUDE_DIRECTORIES (${ICUB_INCLUDE_DIRS})

SET(PROJECTNAME redTargetLocator )
PROJECT ( ${PROJECTNAME} )

### Create some options for use with ccmake ###
option (LINK_ACE ON)
IF ( LINK_ACE )
        FIND_PACKAGE ( ACE REQUIRED )
        INCLUDE_DIRECTORIES ( ${ACE_INCLUDE_DIR} )
ENDIF ( LINK_ACE )

### Find External Libraries ###
# FIND_PACKAGE ( OpenCV REQUIRED )
FIND_PACKAGE ( YARP REQUIRED )
#FIND_PACKAGE ( ICUB )
INCLUDE_DIRECTORIES (${YARP_INCLUDE_DIRS})
INCLUDE_DIRECTORIES (${ICUB_INCLUDE_DIRS})

### Add sources
SET ( src_files main.cpp
		redlocator_module.cpp
)


SET ( hdr_files redlocator_module.h
)

ADD_EXECUTABLE ( ${PROJECTNAME} ${hdr_files} ${src_files} )

TARGET_LINK_LIBRARIES (
	${PROJECTNAME}	${YARP_LIBRARIES}
			#${OpenCV_LIBS}
)
