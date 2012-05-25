CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(PROJECTNAME SEGMENTATION)

PROJECT(${PROJECTNAME})

FIND_PACKAGE( OpenCV REQUIRED )
FIND_PACKAGE(  YARP)
FIND_PACKAGE(ICUB)

FILE(GLOB folder_source src/*.cpp src/camera/*.cpp src/featureProcessing/*.cpp src/saliency/*.cpp src/saliency/*.c src/segmentation/*.cpp )
FILE(GLOB folder_header include/iCub/*.h include/iCub/camera/*.h include/iCub/featureProcessing/*.h include/iCub/saliency/*.h include/iCub/segmentation/*.h)

SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})


LINK_DIRECTORIES(${OPENCV_LINK_DIRECTORIES})
LINK_LIBRARIES(${OPENCV_LIBS})
LINK_DIRECTORIES(${YARP_LINK_DIRECTORIES})
LINK_LIBRARIES(${YARP_LIBS})

SET(CMAKE_MODULE_PATH ${YARP_DIR}/conf ${CMAKE_MODULE_PATH})

INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIR} ${YARP_INCLUDE_DIRS} ${ICUB_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include)

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ICUB_LINK_FLAGS}")
set(CMAKE_C_FLAGS "${GNU_FLAGS} -std=gnu99 ")

ADD_EXECUTABLE(${PROJECTNAME} ${folder_header} ${folder_source})


TARGET_LINK_LIBRARIES(${PROJECTNAME} icubmod ${YARP_LIBRARIES} ${OpenCV_LIBS})
INSTALL(TARGETS ${PROJECTNAME} DESTINATION $ENV{CHRIS_ROOT}/bin)
