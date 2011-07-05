CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

## SET(PROJECTNAME tutorial_cartesian_interface)
SET(PROJECTNAME FollowSphere)

PROJECT(${PROJECTNAME})


# Create everything needed to build our executable.
ADD_EXECUTABLE(${PROJECTNAME} tutorial_cartesian_interface.cpp)

TARGET_LINK_LIBRARIES(${PROJECTNAME} ${YARP_LIBRARIES} icubmod)



