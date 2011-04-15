# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

SET ( src_files	skinWindow.cpp
				glwidget.cpp
				renderList.cpp
				robotmodelexception.cpp
				zphandler.cpp
				robot.cpp
				interval.cpp
				motor.cpp
				bodyPart.cpp
				link.cpp
				primitiveobject.cpp 
				cylinder.cpp 
				joint.cpp 
				joint_prismatic.cpp 
				joint_revolute.cpp 
				box.cpp 
				world.cpp 
				kintreenode.cpp 
				compositeObject.cpp 
				sphere.cpp
				displmatrix.cpp
				displaylist.cpp
				model.cpp
)

SET ( hdr_files	skinWindow.h
				glwidget.h
				renderList.h
				robotmodelexception.h
				zphandler.h
				robot.h
				interval.h
				motor.h
				bodyPart.h
				link.h
				primitiveobject.h 
				cylinder.h 
				joint.h 
				joint_prismatic.h 
				joint_revolute.h 
				box.h 
				world.h 
				kintreenode.h 
				compositeObject.h 
				sphere.h
				displmatrix.h
				displaylist.h
				model.h
				marker.h
				robotobservation.h
)

SET ( QT_MOC_FILES 	skinWindow.h
			glwidget.h
			renderList.h
			model.h
			robot.h
)	

QT4_WRAP_CPP( src_files ${QT_MOC_FILES} )

ADD_LIBRARY ( RobotModel STATIC ${hdr_files} ${src_files} )

TARGET_LINK_LIBRARIES ( RobotModel 	${FREE_SOLID_LIBRARY}
					${QT_LIBRARIES}
					${OPENGL_LIBRARIES}
)

INSTALL ( TARGETS   RobotModel 	 DESTINATION ${CMAKE_SOURCE_DIR}/lib )
INSTALL ( FILES     ${hdr_files} DESTINATION ${CMAKE_SOURCE_DIR}/include/RobotModel)


