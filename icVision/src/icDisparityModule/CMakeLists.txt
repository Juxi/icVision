# Copyright: (C) 2011-2012 Juxi Leitner
# Author: Juxi Leitner <juxi.leitner@gmail.com>
# CopyPolicy: Released under the terms of the GNU GPL v2.0

### Find External Libraries ###
FIND_PACKAGE ( YARP REQUIRED )
FIND_PACKAGE ( OpenCV REQUIRED )

INCLUDE_DIRECTORIES (${YARP_INCLUDE_DIRS})
INCLUDE_DIRECTORIES (${ICUB_INCLUDE_DIRS})

### Find External Libraries ###
FIND_PACKAGE ( OpenCV REQUIRED )
FIND_PACKAGE ( YARP REQUIRED )
INCLUDE_DIRECTORIES (${YARP_INCLUDE_DIRS})
INCLUDE_DIRECTORIES (${ICUB_INCLUDE_DIRS})
INCLUDE_DIRECTORIES (${OpenCV_INCLUDE_DIRS})

### Add sources for library
SET ( src_files disparityModule.cpp
## icFilterModule.cpp
##		icEvolvedFilterModule.cpp
)

SET ( hdr_files disparityModule.h
## icFilterModule.h
##		icEvolvedFilterModule.h
)

### Also add sources for a test module using the library
ADD_EXECUTABLE ( DisparityModule main.cpp
		${hdr_files}
		${src_files}
)

TARGET_LINK_LIBRARIES ( DisparityModule
			icVisionModule
			${YARP_LIBRARIES}
			${OpenCV_LIBS}
)
