# Copyright: (C) 2011 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

PROJECT ( iCubRoadMapDemo )

SET ( iCubRoadMapDemo_VERSION_MAJOR 1 )
SET ( iCubRoadMapDemo_VERSION_MINOR 0 )

FIND_PACKAGE ( YARP REQUIRED )
INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} )

FIND_PACKAGE ( Qt4 REQUIRED )
	SET ( QT_USE_QTOPENGL TRUE )
	SET ( QT_USE_QTXML TRUE )
INCLUDE ( ${QT_USE_FILE} )

### Set some platform-specific variables ###
SET (Q_WS_MAC FALSE)
SET (Q_WS_X11 FALSE)
SET (Q_WS_WIN FALSE)
IF ( UNIX )
	IF ( APPLE )
		SET (Q_WS_MAC TRUE)
		SET (QT_MAC_USE_COCOA TRUE)
	ELSE ( APPLE )
		SET (Q_WS_X11 TRUE)
	ENDIF ( APPLE )
ENDIF ( UNIX )
IF ( WIN32 )
	SET (Q_WS_WIN TRUE)
	ADD_DEFINITIONS( /D "WIN32_LEAN_AND_MEAN" )
ENDIF ( WIN32 )

#FIND_PACKAGE ( BOOST REQUIRED )
#INCLUDE_DIRECTORIES ( ${BOOST_INCLUDE_DIR} )

# 
# FIND AND CONFIGURE CGAL
#
#SET ( CGAL_DIR "/opt/local/lib" )
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)
if ( COMMAND cmake_policy )
  cmake_policy( SET CMP0003 NEW )  
endif()
find_package(CGAL QUIET COMPONENTS Core )
if ( CGAL_FOUND )
  include( ${CGAL_USE_FILE} )
  include( CGAL_CreateSingleSourceCGALProgram )
else()
  message(STATUS "This program requires the CGAL library, and will not be compiled.")
endif()


SET ( src_files	main.cpp
		partController.cpp
		iCubController.cpp
		roadmap.cpp
		graphwidget.cpp
		widgetNode.cpp
		widgetEdge.cpp
		newMapDialog.cpp
		mainwindow.cpp
		controlThread.cpp
)

SET ( hdr_files	roadmap.h
		partController.h
		iCubController.h
		graphwidget.h
		widgetNode.h
		widgetEdge.h
		newMapDialog.h
		mainwindow.h
		controlThread.h
)

SET ( QT_MOC_FILES 	iCubController.h
			graphwidget.h
			roadmap.h
			newMapDialog.h
			mainwindow.h
			controlThread.h
)

QT4_WRAP_CPP( src_files ${QT_MOC_FILES} )

ADD_EXECUTABLE ( RoadmapDemo ${hdr_files} ${src_files} )


TARGET_LINK_LIBRARIES ( RoadmapDemo ${CGAL_LIBRARIES}
				    #${BOOST_LIBRARIES}
				    ${QT_LIBRARIES}
				    ${YARP_LIBRARIES}
)

INSTALL ( TARGETS RoadmapDemo DESTINATION ${CMAKE_SOURCE_DIR}/bin )

