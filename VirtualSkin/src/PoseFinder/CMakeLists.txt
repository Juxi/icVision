CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )
FIND_PACKAGE ( YARP REQUIRED )
FIND_PACKAGE ( Qt4 REQUIRED )

INCLUDE ( ${QT_USE_FILE} )

INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} )
#INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/../VirtualSkinLibrary/KinematicModel/LibKinematicModel" )

	
SET ( src_control controller_main.cpp
		roadmap.cpp
		pathplanner.cpp
		yarpposecontroller.cpp
		connect_delauny.cpp
)

SET ( hdr_control 
        astar.h
		pathplanner.h
		roadmap.h
		yarpposecontroller.h
		spline.hpp
)


SET ( src_finder_lib posefinder.cpp
		simulator.cpp
		demothreads.cpp
		constraints.cpp
		matrix.cpp 
		mones.cpp
		posefitnessfunction.cpp
		sliderwindow.cpp
		valueslider.cpp
)

SET ( hdr_finder_lib	constraints.h
		demothreads.h
		exception.h
		function.h
		mones.h
		matrix.h
		posefitnessfunction.h
		poses_reader.h
		posefinder.h
		simulator.h
		sprintf.h
		sliderwindow.h
		valueslider.h
		xnes.h
        yarp_misc.h
        )

IF (MSVC)
	ADD_DEFINITIONS("/Gm /Zi")
ENDIF (MSVC)

IF ( WIN32 )
	SET (Q_WS_WIN TRUE)
	ADD_DEFINITIONS( /D "NOMINMAX" )
ENDIF ( WIN32 )

SET ( QT_MOC_CONTROL_HEADERS yarpposecontroller.h )
QT4_WRAP_CPP( src_control ${QT_MOC_CONTROL_HEADERS} )

SET ( QT_MOC_HEADERS demothreads.h sliderwindow.h valueslider.h )
QT4_WRAP_CPP( src_finder_lib ${QT_MOC_HEADERS} )

find_package(CGAL QUIET COMPONENTS Core )
if ( CGAL_FOUND )
  include( ${CGAL_USE_FILE} )
  include( CGAL_CreateSingleSourceCGALProgram )
else()
  message(STATUS "This program requires the CGAL library, and will not be compiled.")
endif()

ADD_LIBRARY ( PoseFindLib STATIC ${hdr_finder_lib} ${src_finder_lib} )
ADD_EXECUTABLE ( PoseFind main.cpp)
ADD_EXECUTABLE ( PoseControl ${hdr_control} ${src_control})
ADD_EXECUTABLE ( TEST test.cpp)

ADD_DEPENDENCIES( PoseFindLib WorldModel )
ADD_DEPENDENCIES( PoseFindLib KineMaticModel )
ADD_DEPENDENCIES( PoseFind PoseFindLib )
ADD_DEPENDENCIES( PoseFind WorldModel )
ADD_DEPENDENCIES( PoseFind KinematicModel )
ADD_DEPENDENCIES( PoseControl KinematicModel )
ADD_DEPENDENCIES( PoseControl PoseFindLib )
ADD_DEPENDENCIES( PoseControl VirtualSkin )
ADD_DEPENDENCIES( TEST KinematicModel )


TARGET_LINK_LIBRARIES ( PoseFind KinematicModel 
                    VirtualSkin
					PoseFindLib
					${SOLID_LIBRARY}
					${QT_LIBRARIES}
					${OPENGL_LIBRARIES}
					${YARP_LIBRARIES}
					${CGAL_LIBRARIES}
)

TARGET_LINK_LIBRARIES ( PoseControl
				 	KinematicModel
                    VirtualSkin
					PoseFindLib
					${SOLID_LIBRARY}
					${QT_LIBRARIES}
					${OPENGL_LIBRARIES}
					${YARP_LIBRARIES}
					${CGAL_LIBRARIES}
)

TARGET_LINK_LIBRARIES ( TEST 	KinematicModel
                    VirtualSkin
					PoseFindLib
					${SOLID_LIBRARY}
					${QT_LIBRARIES}
					${OPENGL_LIBRARIES}
					${YARP_LIBRARIES}
					${CGAL_LIBRARIES}
)

INSTALL ( TARGETS PoseFindLib DESTINATION ${CMAKE_SOURCE_DIR}/bin )
INSTALL ( TARGETS PoseFind DESTINATION ${CMAKE_SOURCE_DIR}/bin )
INSTALL ( TARGETS PoseControl DESTINATION ${CMAKE_SOURCE_DIR}/bin )
