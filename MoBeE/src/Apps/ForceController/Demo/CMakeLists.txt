# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

ADD_EXECUTABLE ( SimpleMotion	simpleMotion.cpp )
ADD_EXECUTABLE ( OpSpaceMotion	opSpaceMotion.cpp )

TARGET_LINK_LIBRARIES ( SimpleMotion	${YARP_LIBRARIES})
TARGET_LINK_LIBRARIES ( OpSpaceMotion	${YARP_LIBRARIES})

INSTALL ( TARGETS OpSpaceMotion DESTINATION ${CMAKE_SOURCE_DIR}/bin )
INSTALL ( TARGETS SimpleMotion DESTINATION ${CMAKE_SOURCE_DIR}/bin )

