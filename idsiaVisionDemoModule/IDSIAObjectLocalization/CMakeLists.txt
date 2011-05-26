#### assume FindOpenCV.cmake is in one of the CMAKE_MODULE_PATH folders.
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(PROJECTNAME IDSIAObjectLocalization)
PROJECT(${PROJECTNAME})

FIND_PACKAGE(Qt4 REQUIRED)
FIND_PACKAGE(YARP REQUIRED)
#FIND_PACKAGE(ICUB)
FIND_PACKAGE(OpenCV REQUIRED)
  
# searches for all .cpp and .h files in the current directory and add them 
# to the current project
FILE(GLOB folder_source *.cpp)
FILE(GLOB folder_header *.h)
SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${YARP_MODULE_PATH})
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ICUB_MODULE_PATH})
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${OPENCV_MODULE_PATH})
  
# create the project
ADD_EXECUTABLE(${PROJECTNAME} ${folder_source} ${folder_header})
   
# add include directories
INCLUDE_DIRECTORIES(${iKin_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${ICUB_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
#INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIRS})
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})
#INCLUDE_DIRECTORIES(/usr/local/include)
				 					 	
# link opencv libraries 
TARGET_LINK_LIBRARIES(${PROJECTNAME} iKin ctrlLib ${YARP_LIBRARIES})
TARGET_LINK_LIBRARIES(${PROJECTNAME} ${OPENCV_LIBS})
MESSAGE(STATUS "OPENCV Library: ${OPENCV_LIBS}")
TARGET_LINK_LIBRARIES(${PROJECTNAME} ${OPENCV_LINKER_LIBS} opencv_core
            opencv_flann opencv_imgproc opencv_highgui opencv_ml opencv_video opencv_objdetect
            opencv_features2d opencv_calib3d opencv_legacy opencv_contrib)
TARGET_LINK_LIBRARIES(${PROJECTNAME} ${QT_LIBRARIES})
									 