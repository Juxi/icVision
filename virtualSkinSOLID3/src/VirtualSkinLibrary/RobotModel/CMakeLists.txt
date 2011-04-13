# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

PROJECT ( RobotModel )

SET ( RobotModel_VERSION_MAJOR 0 )
SET ( RobotModel_VERSION_MINOR 1 )

MESSAGE(STATUS "*** RobotModel Sub-Project ***")

SET ( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../cMakeScripts )
INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/../" )

### Find Libraries ###
FIND_PACKAGE ( FreeSOLID REQUIRED )
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
ENDIF ( WIN32 )

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


