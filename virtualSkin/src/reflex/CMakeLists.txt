# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

SET ( src_files	skinWindow.cpp
		glwidget.cpp
		collisionDetector.cpp
		main.cpp
		StateObserver.cpp
		ResponseObserver.cpp
		RobotFilter.cpp
		CallObserver.cpp
		circularBuffer.cpp
)

SET ( hdr_files	skinWindow.h
		glwidget.h
		collisionDetector.h
		StateObserver.h
		ResponseObserver.h
		RobotFilter.h
		CallObserver.h
		circularBuffer.h
)

SET ( QT_MOC_FILES 	skinWindow.h
			glwidget.h
			RobotFilter.h
			collisionDetector.h
)

QT4_WRAP_CPP( src_files ${QT_MOC_FILES} )

ADD_EXECUTABLE ( reflex ${hdr_files} ${src_files} )

ADD_DEPENDENCIES( reflex robotModel yarpUtil yarpFilter )

TARGET_LINK_LIBRARIES ( reflex 	${ROBOT_MODEL_LIB}
				${YARP_UTILITIES_LIB}
				${IDSIA_YARP_FILTER_LIB}
				${FREE_SOLID_LIBRARY}
				${QT_LIBRARIES}
				${OPENGL_LIBRARIES}
				${YARP_LIBRARIES}
				${ACE_LIBRARY}
)

INSTALL ( TARGETS reflex DESTINATION ${CMAKE_SOURCE_DIR}/bin )

