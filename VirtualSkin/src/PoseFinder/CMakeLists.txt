CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )
FIND_PACKAGE ( YARP REQUIRED )
FIND_PACKAGE ( Qt4 REQUIRED )

INCLUDE ( ${QT_USE_FILE} )

INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} )
#INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/../VirtualSkinLibrary/KinematicModel/LibKinematicModel" )

	
SET ( src_files posefinder.cpp 
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
		connect_delauny.cpp
		matrix.cpp 
		mones.cpp
)

IF ( WIN32 )
	SET (Q_WS_WIN TRUE)
	ADD_DEFINITIONS( /D "NOMINMAX" )
ENDIF ( WIN32 )


SET ( hdr_files	posefinder.h xnes.h constraints.h posefitnessfunction.h simulator.h demothreads.h roadmap.h path_extractor.h sliderwindow.h valueslider.h exception.h pathplanner.h iCubController.h partController.h mones.h matrix.h sprintf.h)

SET ( QT_MOC_FILES sliderwindow.h valueslider.h demothreads.h yarpposecontroller.h iCubController.h )
QT4_WRAP_CPP( src_files ${QT_MOC_FILES} )

find_package(CGAL QUIET COMPONENTS Core )
if ( CGAL_FOUND )
  include( ${CGAL_USE_FILE} )
  include( CGAL_CreateSingleSourceCGALProgram )
else()
  message(STATUS "This program requires the CGAL library, and will not be compiled.")
endif()

ADD_EXECUTABLE ( PoseFind ${hdr_files} ${src_files} main.cpp)
ADD_EXECUTABLE ( PoseControl ${hdr_files} ${src_files} yarpposecontroller.h controller_main.cpp)
ADD_EXECUTABLE ( TEST ${hdr_files} ${src_files} test.cpp)

ADD_DEPENDENCIES( PoseFind WorldModel )
ADD_DEPENDENCIES( PoseFind KinematicModel )
ADD_DEPENDENCIES( PoseControl WorldModel )
ADD_DEPENDENCIES( PoseControl KinematicModel )
ADD_DEPENDENCIES( PoseControl VirtualSkin )
ADD_DEPENDENCIES( TEST KinematicModel )


TARGET_LINK_LIBRARIES ( PoseFind KinematicModel 
                    VirtualSkin
					${SOLID_LIBRARY}
					${QT_LIBRARIES}
					${OPENGL_LIBRARIES}
					${YARP_LIBRARIES}
					${CGAL_LIBRARIES}
)

TARGET_LINK_LIBRARIES ( PoseControl 	KinematicModel
                    VirtualSkin
					${SOLID_LIBRARY}
					${QT_LIBRARIES}
					${OPENGL_LIBRARIES}
					${YARP_LIBRARIES}
					${CGAL_LIBRARIES}
)

TARGET_LINK_LIBRARIES ( TEST 	KinematicModel
                    VirtualSkin
					${SOLID_LIBRARY}
					${QT_LIBRARIES}
					${OPENGL_LIBRARIES}
					${YARP_LIBRARIES}
					${CGAL_LIBRARIES}
)

INSTALL ( TARGETS PoseFind DESTINATION ${CMAKE_SOURCE_DIR}/bin )
INSTALL ( TARGETS PoseControl DESTINATION ${CMAKE_SOURCE_DIR}/bin )
