# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.


SET ( src_files	main.cpp
		reflexFilter.cpp
)

SET ( hdr_files	reflexFilter.h
)

ADD_EXECUTABLE ( VelocityReflex ${hdr_files} ${src_files} )

ADD_DEPENDENCIES( VelocityReflex KinematicModel )

TARGET_LINK_LIBRARIES ( VelocityReflex 	VirtualSkin
					KinematicModel
					YarpFilter
					
					${SOLID_LIBRARY}
					${QT_LIBRARIES}
					${OPENGL_LIBRARIES}
					${YARP_LIBRARIES}
)

INSTALL ( TARGETS VelocityReflex DESTINATION ${CMAKE_SOURCE_DIR}/bin )

