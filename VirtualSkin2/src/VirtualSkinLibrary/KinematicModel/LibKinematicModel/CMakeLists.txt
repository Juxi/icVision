# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

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
			bodyPart.cpp
			kintreenode.cpp 
			motor.cpp
			interval.cpp
				
			link.cpp
			joint.cpp
			joint_revolute.cpp
				#joint_prismatic.cpp 
				
			zpHandler.cpp
			worldhandler.cpp
				
)

SET ( QT_MOC_FILES 	model.h
			modelwindow.h
			glwidget.h
			robot.h
)

SET ( hdr_files		modelconstants.h

			model.h			# the center of it all
			modelexception.h	# when things go wrong
			modelwindow.h		# the window
			glwidget.h		# the OpenGL frustum

			transformable.h
			compositeobject.h
			primitiveobject.h
			sphere.h
			cylinder.h 
			box.h  

			robot.h
			bodyPart.h
			kintreenode.h 
			motor.h
			interval.h
				
			link.h
			joint.h
			joint_revolute.h
				#joint_prismatic.h 
				
			zpHandler.h
			worldhandler.h

				
)

	

QT4_WRAP_CPP( src_files ${QT_MOC_FILES} )

ADD_LIBRARY ( KinematicModel STATIC ${hdr_files} ${src_files} )

TARGET_LINK_LIBRARIES ( KinematicModel 	${FREE_SOLID_LIBRARY}
					${QT_LIBRARIES}
					${OPENGL_LIBRARIES}
)

INSTALL ( TARGETS   KinematicModel 	DESTINATION ${CMAKE_SOURCE_DIR}/lib )
INSTALL ( FILES     ${hdr_files}	DESTINATION ${CMAKE_SOURCE_DIR}/include/KinematicModel)


