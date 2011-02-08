#
# Derived from a file from the YARP distribution
#
# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

### openGL ###
FIND_PACKAGE ( OpenGL )

### QT ###
FIND_PACKAGE ( Qt4 REQUIRED )
SET ( QT_USE_QTOPENGL TRUE )
SET ( QT_USE_QTXML TRUE )
INCLUDE ( ${QT_USE_FILE} )

#INCLUDE_DIRECTORIES ( 	"${CMAKE_SOURCE_DIR}/src/robotModel/model"
#			"${CMAKE_SOURCE_DIR}/src/robotModel/model/freeSolid")

ADD_SUBDIRECTORY ( yarpFilter )
ADD_SUBDIRECTORY ( robotModel )
ADD_SUBDIRECTORY ( virtualSkin )
ADD_SUBDIRECTORY ( babble )