# Copyright: (C) 2011 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

PROJECT ( iCubRL_Demos )

SET ( iCubRL_Demos_VERSION_MAJOR 1 )
SET ( iCubRL_Demos_VERSION_MINOR 0 )

MESSAGE(STATUS "*** iCub RL Demos ***")

INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/../../LibRL" )

ADD_EXECUTABLE ( imrl	imrl.cpp )
ADD_DEPENDENCIES( imrl	iCubRL )
TARGET_LINK_LIBRARIES( imrl	iCubRL )

ADD_EXECUTABLE ( least_tried least_tried.cpp )
ADD_DEPENDENCIES( least_tried	iCubRL )
TARGET_LINK_LIBRARIES( least_tried	iCubRL )

ADD_EXECUTABLE ( random random.cpp )
ADD_DEPENDENCIES( random	iCubRL )
TARGET_LINK_LIBRARIES( random		iCubRL )

ADD_EXECUTABLE ( parallel_imrl parallel_imrl.cpp )
ADD_DEPENDENCIES( parallel_imrl	iCubRL )
TARGET_LINK_LIBRARIES( parallel_imrl	iCubRL )

ADD_EXECUTABLE ( bruteForceReach bruteForceReach.cpp )
ADD_DEPENDENCIES( bruteForceReach	iCubRL ) 
TARGET_LINK_LIBRARIES( bruteForceReach	iCubRL )

ADD_EXECUTABLE ( powerPlayReach powerPlayReach.cpp )
ADD_DEPENDENCIES( powerPlayReach	iCubRL ) 
TARGET_LINK_LIBRARIES( powerPlayReach	iCubRL ) 

INSTALL ( TARGETS imrl least_tried random parallel_imrl bruteForceReach powerPlayReach DESTINATION ${CMAKE_SOURCE_DIR}/bin/learning )

