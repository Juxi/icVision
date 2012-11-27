# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

ADD_EXECUTABLE ( poses		randomPose.cpp )
ADD_EXECUTABLE ( circle     opSpaceCircle.cpp )
ADD_EXECUTABLE ( reaches	opSpaceReach.cpp )

TARGET_LINK_LIBRARIES ( poses		${YARP_LIBRARIES})
TARGET_LINK_LIBRARIES ( circle		${YARP_LIBRARIES})
TARGET_LINK_LIBRARIES ( reaches     ${YARP_LIBRARIES})

INSTALL ( TARGETS poses     DESTINATION ${CMAKE_SOURCE_DIR}/bin )
INSTALL ( TARGETS circle	DESTINATION ${CMAKE_SOURCE_DIR}/bin )
INSTALL ( TARGETS reaches	DESTINATION ${CMAKE_SOURCE_DIR}/bin )

