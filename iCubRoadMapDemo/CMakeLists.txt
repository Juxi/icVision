# Copyright: (C) 2011 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

PROJECT ( iCubRoadMapDemo )

SET ( iCubRoadMapDemo_VERSION_MAJOR 1 )
SET ( iCubRoadMapDemo_VERSION_MINOR 0 )

FIND_PACKAGE ( YARP REQUIRED )
INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} )

#FIND_PACKAGE ( BOOST REQUIRED )
#INCLUDE_DIRECTORIES ( ${BOOST_INCLUDE_DIR} )

# 
# FIND AND CONFIGURE CGAL
#
SET ( CGAL_DIR "/opt/local/lib" )
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
		roadmap.cpp
)

SET ( hdr_files	roadmap.h
		partController.h
)

ADD_EXECUTABLE ( RoadmapDemo ${hdr_files} ${src_files} )


TARGET_LINK_LIBRARIES ( RoadmapDemo ${CGAL_LIBRARIES}
				    #${BOOST_LIBRARIES}
				    ${YARP_LIBRARIES}
)

INSTALL ( TARGETS RoadmapDemo DESTINATION ${CMAKE_SOURCE_DIR}/bin )

