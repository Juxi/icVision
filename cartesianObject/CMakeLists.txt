cmake_minimum_required(VERSION 2.6)

SET(PROJECTNAME ballFollowerDemo)

PROJECT(${PROJECTNAME})

# Find YARP.  Point the YARP_DIR environment variable at your build.
find_package(YARP REQUIRED)
find_package(ICUB REQUIRED)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${YARP_MODULE_PATH})
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ICUB_MODULE_PATH})
INCLUDE(iCubOptions)
INCLUDE(iCubHelpers)

# Inclue any directories needed for YARP
include_directories(${ICUB_INCLUDE_DIRS} ${YARP_INCLUDE_DIRS})
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${ICUB_LINK_FLAGS}")

# Set up our main executable.
add_executable("turnaround" "turnaround.cpp")
add_executable("cartesianfollower" "cartesianfollower.cpp")
add_executable("touched" "touched.cpp")
add_executable("touchthread" "touchthread.cpp" "touchthread.h" "touchthreadtest.cpp")
add_executable("reactive" "touchthread.cpp" "touchthread.h" "reactiveClosing.cpp")

target_link_libraries("turnaround" ${YARP_LIBRARIES})
target_link_libraries("cartesianfollower" ${YARP_LIBRARIES} icubmod)
target_link_libraries("touched" ${YARP_LIBRARIES})
target_link_libraries("touchthread" ${YARP_LIBRARIES})
target_link_libraries("reactive" ${YARP_LIBRARIES})

