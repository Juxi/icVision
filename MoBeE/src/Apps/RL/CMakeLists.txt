# Copyright: (C) 2011 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

PROJECT ( iCubRoadMapDemo )

SET ( rl_VERSION_MAJOR 1 )
SET ( rl_VERSION_MINOR 0 )

FIND_PACKAGE ( YARP REQUIRED )
INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} )

# 
# FIND AND CONFIGURE CGAL
#
SET ( CGAL_DIR "/opt/local/lib/cmake" )
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)
if ( COMMAND cmake_policy )
  cmake_policy( SET CMP0003 NEW )  
endif()
find_package(CGAL QUIET COMPONENTS Core )
if ( CGAL_FOUND )
  include( ${CGAL_USE_FILE} )
  include( CGAL_CreateSingleSourceCGALProgram )
  include_directories (BEFORE "../include")
else()
  message(STATUS "This program requires the CGAL library, and will not be compiled.")
endif()


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

###############################################################


SET ( src_files		main.cpp
			learner.cpp
			#state.cpp
			#action.cpp
)

SET ( hdr_files		learner.h
			#state.h
			#action.h
)


ADD_EXECUTABLE ( rl ${hdr_files} ${src_files}
)


TARGET_LINK_LIBRARIES ( rl ${CGAL_LIBRARIES}
			    ${YARP_LIBRARIES}
)

INSTALL ( TARGETS rl DESTINATION ${CMAKE_SOURCE_DIR}/bin )

