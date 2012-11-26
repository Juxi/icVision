# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

ADD_EXECUTABLE ( cyclePoses	jointSpacePosition.cpp )
ADD_EXECUTABLE ( babble		jointSpaceForce.cpp )
ADD_EXECUTABLE ( circle	rightHandCircle.cpp )
ADD_EXECUTABLE ( reach		randomReach.cpp )

TARGET_LINK_LIBRARIES ( cyclePoses	${YARP_LIBRARIES})
TARGET_LINK_LIBRARIES ( babble		${YARP_LIBRARIES})
TARGET_LINK_LIBRARIES ( circle		${YARP_LIBRARIES})
TARGET_LINK_LIBRARIES ( reach		${YARP_LIBRARIES})

INSTALL ( TARGETS cyclePoses DESTINATION ${CMAKE_SOURCE_DIR}/bin )
INSTALL ( TARGETS babble DESTINATION ${CMAKE_SOURCE_DIR}/bin )
INSTALL ( TARGETS circle DESTINATION ${CMAKE_SOURCE_DIR}/bin )
INSTALL ( TARGETS reach DESTINATION ${CMAKE_SOURCE_DIR}/bin )

