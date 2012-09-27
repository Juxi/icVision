# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

SET ( src_files	main.cpp
		reflexFilter.cpp
)

SET ( hdr_files	reflexFilter.h
)

ADD_EXECUTABLE ( PositionReflex ${hdr_files} ${src_files} )

ADD_DEPENDENCIES( PositionReflex RobotFilter 
				 YarpFilter 
				 YarpModel 
				 KinematicModel
)

TARGET_LINK_LIBRARIES ( PositionReflex  RobotFilter 
				 	YarpFilter 
				 	YarpModel 
				 	KinematicModel
					 ${SOLID_LIBRARY}
					 ${QT_LIBRARIES}
					 ${OPENGL_LIBRARIES}
					 ${YARP_LIBRARIES}
)

INSTALL ( TARGETS PositionReflex DESTINATION ${CMAKE_SOURCE_DIR}/bin )

