# Copyright: (C) 2011 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

PROJECT ( partController )

SET ( partController_VERSION_MAJOR 1 )
SET ( partController_VERSION_MINOR 0 )

FIND_PACKAGE ( YARP REQUIRED )
INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} )

### Set some platform-specific variables ###
#IF ( WIN32 )
#	SET (Q_WS_WIN TRUE)
#	ADD_DEFINITIONS( /D "WIN32_LEAN_AND_MEAN" )
#ENDIF ( WIN32 )

#FIND_PACKAGE ( BOOST REQUIRED )
#INCLUDE_DIRECTORIES ( ${BOOST_INCLUDE_DIR} )


SET ( src_files	main.cpp
				partController.cpp
		#iCubController.cpp
		#roadmap.cpp
		#graphwidget.cpp
		#widgetNode.cpp
		#widgetEdge.cpp
		#newMapDialog.cpp
		#mainwindow.cpp
		#controlThread.cpp
)

SET ( hdr_files	#roadmap.h
				partController.h
		#iCubController.h
		#graphwidget.h
		#widgetNode.h
		#widgetEdge.h
		#newMapDialog.h
		#mainwindow.h
		#controlThread.h
		#poses_reader.h
)

ADD_EXECUTABLE ( partController ${hdr_files} ${src_files} )


TARGET_LINK_LIBRARIES ( partController ${YARP_LIBRARIES} )

INSTALL ( TARGETS partController DESTINATION ${CMAKE_SOURCE_DIR}/bin )

