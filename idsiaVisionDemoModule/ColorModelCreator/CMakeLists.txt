#### assume FindOpenCV.cmake is in one of the CMAKE_MODULE_PATH folders.
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

FIND_PACKAGE(Qt4 REQUIRED)
  
FIND_PACKAGE(YARP REQUIRED)
#SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${YARP_MODULE_PATH})
FIND_PACKAGE(ICUB)

FIND_PACKAGE(OpenCV REQUIRED)
  
# searches for all .cpp and .h files in the current directory and add them 
# to the current project
FILE(GLOB folder_source *.cpp)
FILE(GLOB folder_header *.h)
SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${YARP_MODULE_PATH})
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ICUB_MODULE_PATH})

  
# create the project
ADD_EXECUTABLE(test ${folder_source} ${folder_header})
   
# add include directories
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(/usr/local/include)
INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})
#ADD_DEFINITIONS(${YARP_DEFINES}) # optional   
INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

INCLUDE_DIRECTORIES(${ICUB_INCLUDE_DIRS}
                    ${YARP_INCLUDE_DIRS})


#LINK_DIRECTORIES(${OPENCV_LINK_DIRECTORIES})
#LINK_LIBRARIES(${OPENCV_LIBS})
#LINK_DIRECTORIES(${YARP_LINK_DIRECTORIES})
#LINK_LIBRARIES(${YARP_LIBS})
#LINK_LIBRARIES(${QT_LIBS})

#ADD_LIBRARY(test STATIC /usr/local/libiKin.a)

# link opencv libraries 
TARGET_LINK_LIBRARIES(test ${OPENCV_LIBS})
MESSAGE(STATUS "OPENCV Library: ${OPENCV_LIBS}")
TARGET_LINK_LIBRARIES(test ${YARP_LIBS})
TARGET_LINK_LIBRARIES(test ${QT_LIBS})
TARGET_LINK_LIBRARIES(test ${OPENCV_LINKER_LIBS} opencv_core
            opencv_flann opencv_imgproc opencv_highgui opencv_ml opencv_video opencv_objdetect
            opencv_features2d opencv_calib3d opencv_legacy opencv_contrib)
TARGET_LINK_LIBRARIES(test iKin)
