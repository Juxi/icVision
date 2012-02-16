# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
	
SET ( src_files	main.cpp
		graspfinder.cpp 
		xnes.cpp )

SET ( hdr_files	graspfinder.h xnes.h constraints.h graspthread.h posefitnessfunction.h map_creator.h simulator.h )

#SET ( QT_MOC_FILES graspfinder.h )	
#QT4_WRAP_CPP( src_files ${QT_MOC_FILES} )

ADD_EXECUTABLE ( GraspFind ${hdr_files} ${src_files} )

ADD_DEPENDENCIES( GraspFind WorldModel )

TARGET_LINK_LIBRARIES ( GraspFind 	KinematicModel
					${SOLID_LIBRARY}
					${QT_LIBRARIES}
					${OPENGL_LIBRARIES}			
)

INSTALL ( TARGETS GraspFind DESTINATION ${CMAKE_SOURCE_DIR}/bin )
