# Copyright: (C) 2011 Juxi Leitner
# Author: Juxi Leitner <juxi.leitner@gmail.com>
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.8 )

PROJECT ( cvSaliency )

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

### Add internal librarys (envision)
INCLUDE_DIRECTORIES ( "${PROJECT_SOURCE_DIR}/lib" )
ADD_SUBDIRECTORY (lib)

### Add sources
ADD_SUBDIRECTORY (src)
