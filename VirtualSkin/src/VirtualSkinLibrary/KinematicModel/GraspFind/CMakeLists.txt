# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
	
SET ( src_files graspfinder.cpp 
		xnes.cpp 
		roadmap.cpp 
		sliderwindow.cpp
		valueslider.cpp
		yarpgrasp.cpp
)

SET ( hdr_files	graspfinder.h xnes.h constraints.h posefitnessfunction.h simulator.h demothreads.h roadmap.h path_extractor.h sliderwindow.h valueslider.h isnan.h yarpgrasp.h )

SET ( QT_MOC_FILES sliderwindow.h valueslider.h demothreads.h yarpgrasp.h )	
QT4_WRAP_CPP( src_files ${QT_MOC_FILES} )

ADD_EXECUTABLE ( GraspFind ${hdr_files} ${src_files} main.cpp)
ADD_EXECUTABLE ( GraspControl ${hdr_files} ${src_files} yarpgraspcontroller.cpp)
ADD_DEPENDENCIES( GraspFind WorldModel )
ADD_DEPENDENCIES( GraspControl WorldModel )

TARGET_LINK_LIBRARIES ( GraspFind 	KinematicModel
					${SOLID_LIBRARY}
					${QT_LIBRARIES}
					${OPENGL_LIBRARIES}
					gmp
					CGAL
)

TARGET_LINK_LIBRARIES ( GraspControl 	KinematicModel
					${SOLID_LIBRARY}
					${QT_LIBRARIES}
					${OPENGL_LIBRARIES}
					gmp
					CGAL
)

INSTALL ( TARGETS GraspFind DESTINATION ${CMAKE_SOURCE_DIR}/bin )
INSTALL ( TARGETS GraspControl DESTINATION ${CMAKE_SOURCE_DIR}/bin )
