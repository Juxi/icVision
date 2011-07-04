# Copyright: (C) 2011 Juxi Leitner
# Author: Juxi Leitner <juxi.leitner@gmail.com>
# CopyPolicy: Released under the terms of the GNU GPL v2.0

### Find External Libraries ###
FIND_PACKAGE ( YARP REQUIRED )
FIND_PACKAGE ( ICUB )
# FIND_PACKAGE ( OpenCV REQUIRED )

INCLUDE_DIRECTORIES (${YARP_INCLUDE_DIRS})
INCLUDE_DIRECTORIES (${ICUB_INCLUDE_DIRS})

### Parse the directories where build targets are ###
ADD_SUBDIRECTORY ( ReachingDemo )

