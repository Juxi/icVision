# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

FIND_PACKAGE ( YARP REQUIRED )
INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} )
	
SET ( src_files posefinder.cpp 
		xnes.cpp 
		roadmap.cpp 
		sliderwindow.cpp
		valueslider.cpp
		pathplanner.cpp
		iCubController.cpp
		partController.cpp
		simulator.cpp
		yarpposecontroller.cpp
		demothreads.cpp
		constraints.cpp
)

SET ( hdr_files	posefinder.h xnes.h constraints.h posefitnessfunction.h simulator.h demothreads.h roadmap.h path_extractor.h sliderwindow.h valueslider.h exception.h pathplanner.h iCubController.h partController.h)

SET ( QT_MOC_FILES sliderwindow.h valueslider.h demothreads.h yarpposecontroller.h iCubController.h )
QT4_WRAP_CPP( src_files ${QT_MOC_FILES} )

ADD_EXECUTABLE ( PoseFind ${hdr_files} ${src_files} main.cpp)
ADD_EXECUTABLE ( PoseControl ${hdr_files} ${src_files} yarpposecontroller.h controller_main.cpp)
ADD_DEPENDENCIES( PoseFind WorldModel )
ADD_DEPENDENCIES( PoseControl WorldModel )

TARGET_LINK_LIBRARIES ( PoseFind 	KinematicModel
					${SOLID_LIBRARY}
					${QT_LIBRARIES}
					${OPENGL_LIBRARIES}
					${YARP_LIBRARIES}
					${CGAL_LIBRARIES}
					gmp
)

TARGET_LINK_LIBRARIES ( PoseControl 	KinematicModel
					${SOLID_LIBRARY}
					${QT_LIBRARIES}
					${OPENGL_LIBRARIES}
					${YARP_LIBRARIES}
					${CGAL_LIBRARIES}
					gmp
)

INSTALL ( TARGETS PoseFind DESTINATION ${CMAKE_SOURCE_DIR}/bin )
INSTALL ( TARGETS PoseControl DESTINATION ${CMAKE_SOURCE_DIR}/bin )
