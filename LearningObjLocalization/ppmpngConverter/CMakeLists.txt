# Copyright: (C) 2010-2011 Juxi Leitner
# Author: Juxi Leitner <juxi.leitner@gmail.com> @ IDSIA.ch
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

### Headers within this project ###
INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}" )

### Find External Libraries ###
FIND_PACKAGE ( OpenCV REQUIRED )

FIND_PACKAGE ( YARP REQUIRED )
INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} )

IF ( LINK_ACE )
        FIND_PACKAGE ( ACE REQUIRED )
        INCLUDE_DIRECTORIES ( ${ACE_INCLUDE_DIR} )
ENDIF ( LINK_ACE )

SET ( src_files main.cpp)

ADD_EXECUTABLE ( converter ${src_files} )


TARGET_LINK_LIBRARIES ( converter ${OpenCV_LIBS}
				 ${YARP_LIBRARIES}
)
