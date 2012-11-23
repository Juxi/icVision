# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

SET( PARSED ${PARSED_LIB_KINEMATIC} )
IF( NOT PARSED )


CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

PROJECT ( KinematicModelLibrary )

SET ( KinematicModel_VERSION_MAJOR 1 )
SET ( KinematicModel_VERSION_MINOR 0 )

MESSAGE(STATUS "*** Kinematic Model Library ***")

SET ( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../CMakeScripts )

### Find Libraries ###
FIND_PACKAGE ( SOLID REQUIRED )
FIND_PACKAGE ( OpenGL )
FIND_PACKAGE ( Qt4 REQUIRED )
	SET ( QT_USE_QTOPENGL TRUE )
	SET ( QT_USE_QTXML TRUE )
	INCLUDE ( ${QT_USE_FILE} )

### Set some platform-specific variables ###
SET (Q_WS_MAC FALSE)
SET (Q_WS_X11 FALSE)
SET (Q_WS_WIN FALSE)
IF ( UNIX )
	IF ( APPLE )
		SET (Q_WS_MAC TRUE)
		SET (QT_MAC_USE_COCOA TRUE)
	ELSE ( APPLE )
		SET (Q_WS_X11 TRUE)
	ENDIF ( APPLE )
ENDIF ( UNIX )
IF ( WIN32 )
	SET (Q_WS_WIN TRUE)
	ADD_DEFINITIONS( /D "WIN32_LEAN_AND_MEAN" )
ENDIF ( WIN32 )

SET ( src_files		model.cpp			# the center of it all
			modelexception.cpp		# when things go wrong
			modelwindow.cpp			# the window
			glwidget.cpp			# the OpenGL frustum

			transformable.cpp
			compositeobject.cpp
			primitiveobject.cpp
			sphere.cpp
			cylinder.cpp 
			box.cpp  
	
			robot.cpp
			bodypart.cpp
			linearconstraint.cpp
			kintreenode.cpp 
			motor.cpp
			interval.cpp
				
			link.cpp
			joint.cpp
			joint_revolute.cpp
				#joint_prismatic.cpp 
				
			zphandler.cpp
			worldhandler.cpp
			objectmover.cpp
				
)

SET ( hdr_files		modelconstants.h

			#robotobservation.h
			model.h			# the center of it all
			modelexception.h	# when things go wrong
			modelwindow.h		# the window
			glwidget.h		# the OpenGL frustum

			marker.h
			transformable.h
			compositeobject.h
			primitiveobject.h
			sphere.h
			cylinder.h 
			box.h  

			robot.h
			bodypart.h
		      linearconstraint.h
			kintreenode.h 
			motor.h
			interval.h
				
			link.h
			joint.h
			joint_revolute.h
				#joint_prismatic.h 
				
			zphandler.h
			worldhandler.h
			objectmover.h				
)

SET ( QT_MOC_FILES 	model.h
			modelwindow.h
			glwidget.h
			robot.h
            		bodypart.h
)

QT4_WRAP_CPP( src_files ${QT_MOC_FILES} )

ADD_LIBRARY ( KinematicModel STATIC ${hdr_files} ${src_files} )

TARGET_LINK_LIBRARIES ( KinematicModel	${FREE_SOLID_LIBRARY}
						${QT_LIBRARIES}
						${OPENGL_LIBRARIES}
)

INSTALL ( TARGETS   KinematicModel 	DESTINATION ${CMAKE_SOURCE_DIR}/lib )
INSTALL ( FILES     ${hdr_files}	DESTINATION ${CMAKE_SOURCE_DIR}/include/KinematicModel)



SET( PARSED_LIB_KINEMATIC true PARENT_SCOPE)
ENDIF( NOT PARSED )
