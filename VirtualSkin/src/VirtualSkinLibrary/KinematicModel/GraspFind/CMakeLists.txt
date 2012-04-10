# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

FIND_PACKAGE ( YARP REQUIRED )
INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} )
	
SET ( src_files graspfinder.cpp 
		xnes.cpp 
		roadmap.cpp 
		sliderwindow.cpp
		valueslider.cpp
		pathplanner.cpp
		iCubController.cpp
		partController.cpp
		simulator.cpp
		yarpgraspcontroller.cpp
)

SET ( hdr_files	graspfinder.h xnes.h constraints.h posefitnessfunction.h simulator.h demothreads.h roadmap.h path_extractor.h sliderwindow.h valueslider.h exception.h pathplanner.h iCubController.h partController.h)

SET ( QT_MOC_FILES sliderwindow.h valueslider.h demothreads.h yarpgraspcontroller.h iCubController.h )
QT4_WRAP_CPP( src_files ${QT_MOC_FILES} )

ADD_EXECUTABLE ( GraspFind ${hdr_files} ${src_files} main.cpp)
ADD_EXECUTABLE ( GraspControl ${hdr_files} ${src_files} yarpgraspcontroller.h controller_main.cpp)
ADD_DEPENDENCIES( GraspFind WorldModel )
ADD_DEPENDENCIES( GraspControl WorldModel )

TARGET_LINK_LIBRARIES ( GraspFind 	KinematicModel
					${SOLID_LIBRARY}
					${QT_LIBRARIES}
					${OPENGL_LIBRARIES}
					${YARP_LIBRARIES}
					${CGAL_LIBRARIES}
					gmp
)

TARGET_LINK_LIBRARIES ( GraspControl 	KinematicModel
					${SOLID_LIBRARY}
					${QT_LIBRARIES}
					${OPENGL_LIBRARIES}
					${YARP_LIBRARIES}
					${CGAL_LIBRARIES}
					gmp
)

INSTALL ( TARGETS GraspFind DESTINATION ${CMAKE_SOURCE_DIR}/bin )
INSTALL ( TARGETS GraspControl DESTINATION ${CMAKE_SOURCE_DIR}/bin )
