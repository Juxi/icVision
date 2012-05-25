
cmake_minimum_required(VERSION 2.6)
project (FIAS_code)

FIND_PACKAGE( OpenCV REQUIRED )
find_package(YARP REQUIRED)
find_package(ICUB REQUIRED)

set(CMAKE_MODULE_PATH ${YARP_MODULE_PATH} ${ICUB_MODULE_PATH} ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ICUB_DIR}/conf ${YARP_DIR}/conf) 

add_subdirectory(src)

