cmake_minimum_required(VERSION 2.6)

PROJECT(cableLengthGuard)
SET(YARP_DIR "$ENV{YARP_ROOT}" CACHE LOCATION "where is yarp?")

FIND_PACKAGE(YARP)
# FIND_PACKAGE(ICUB)

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${YARP_LINK_FLAGS}")

# SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ICUB_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/conf)
# FIND_LIBRARY(CQP_LIB cqp $ENV{CQP_DIR}/lib)
# FIND_PATH(CQP_INCLUDE gsl_cqp.h $ENV{CQP_DIR}/include)

INCLUDE_DIRECTORIES(${YARP_INCLUDE_DIRS})

FILE(GLOB folder_source *.cpp)
FILE(GLOB folder_header *.h)
SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})

ADD_EXECUTABLE(cableLengthGuard ${folder_source} ${folder_header})
INSTALL(TARGETS cableLengthGuard DESTINATION ${ICUB_DIR}/bin)
TARGET_LINK_LIBRARIES(cableLengthGuard ${YARP_LIBRARIES})


#Link cqp for solving LQ minimization
#TARGET_LINK_LIBRARIES(cableLengthGuard /home/iron/Code/cqp/lib/libcqp.a)
#INCLUDE_DIRECTORIES(/home/iron/Code/cqp/include)

#TARGET_LINK_LIBRARIES(cableLengthGuard /media/WDBACKUP/Code/cqp/lib/libcqp.a)
#INCLUDE_DIRECTORIES(/media/WDBACKUP/Code/cqp/include)

# TARGET_LINK_LIBRARIES(cableLengthGuard ${CQP_LIB})
# INCLUDE_DIRECTORIES(${CQP_INCLUDE})
