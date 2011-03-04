# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

SET ( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cMakeScripts )

### Find Libraries ###
FIND_PACKAGE ( YARP REQUIRED )
FIND_PACKAGE ( FreeSOLID REQUIRED )
FIND_PACKAGE ( OpenGL REQUIRED )
FIND_PACKAGE ( Qt4 REQUIRED )
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
	SET ( QT_USE_QTOPENGL TRUE )
	SET ( QT_USE_QTXML TRUE )
	INCLUDE ( ${QT_USE_FILE} )
	
SET ( src_files	skinWindow.cpp
		glwidget.cpp
		collisionDetector.cpp
		StateObserver.cpp
		ResponseObserver.cpp
		RobotFilter.cpp
		CallObserver.cpp
		circularBuffer.cpp
		yarpRpcPort.cpp
		worldRpcInterface.cpp
		yarpStreamPort.cpp
)

SET ( hdr_files	skinWindow.h
		glwidget.h
		collisionDetector.h
		StateObserver.h
		ResponseObserver.h
		RobotFilter.h
		CallObserver.h
		circularBuffer.h
		yarpRpcPort.h
		worldRpcInterface.h
		yarpStreamPort.h
)

SET ( QT_MOC_FILES 	skinWindow.h
			glwidget.h
)

QT4_WRAP_CPP( src_files ${QT_MOC_FILES} )

ADD_LIBRARY ( VirtualSkin STATIC ${hdr_files} ${src_files} )

ADD_DEPENDENCIES( VirtualSkin RobotModel YarpFilter )

TARGET_LINK_LIBRARIES ( VirtualSkin 	${ROBOT_MODEL_LIB}
					${YARP_FILTER_LIB}
					#${FREE_SOLID_LIBRARY}
					#${QT_LIBRARIES}
					#${OPENGL_LIBRARIES}
					#${YARP_LIBRARIES}
)

INSTALL ( TARGETS VirtualSkin DESTINATION ${CMAKE_SOURCE_DIR}/lib )

