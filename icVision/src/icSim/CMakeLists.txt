# Copyright: (C) 2012-2013 Juxi Leitner
# Author: Juxi Leitner <juxi.leitner@gmail.com>
# CopyPolicy: Released under the terms of the GNU GPL v2.0

### Find External Libraries ###
FIND_PACKAGE ( YARP REQUIRED )
FIND_PACKAGE ( OpenCV REQUIRED )

INCLUDE_DIRECTORIES (${YARP_INCLUDE_DIRS})

### Find External Libraries ###
FIND_PACKAGE ( YARP REQUIRED )
INCLUDE_DIRECTORIES (${YARP_INCLUDE_DIRS})

### Add sources for library
SET ( src_files simulatedRobot.cpp)

SET ( hdr_files simulatedRobot.h )

### Also add sources for a test module using the library
ADD_EXECUTABLE ( icSim main.cpp ${hdr_files} ${src_files} )

TARGET_LINK_LIBRARIES ( icSim ${YARP_LIBRARIES} )
