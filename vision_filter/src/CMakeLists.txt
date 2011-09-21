# Copyright: (C) 2011 Juxi Leitner
# Author: Juxi Leitner <juxi.leitner@gmail.com> @ IDSIA.ch
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

FIND_PACKAGE(YARP REQUIRED)

FIND_PACKAGE(OpenCV REQUIRED)
  
# searches for all .cpp and .h files in the current directory and add them 
# to the current project
FILE(GLOB folder_source *.cpp)
FILE(GLOB folder_header *.h)
SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${YARP_MODULE_PATH})
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${OPENCV_MODULE_PATH})
  
# create the project
ADD_EXECUTABLE(${PROJECTNAME} ${folder_source} ${folder_header})
   
# add include directories
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIRS})
				 					 	
# link opencv libraries 
TARGET_LINK_LIBRARIES ( ${PROJECTNAME}
			${OpenCV_LIBS}
			${YARP_LIBRARIES}
)