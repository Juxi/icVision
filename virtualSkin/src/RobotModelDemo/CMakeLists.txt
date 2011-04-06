# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
	
SET ( src_files	controlThread.cpp
		main.cpp
)

SET ( hdr_files	controlThread.h
)

SET ( QT_MOC_FILES 	controlThread.h
)

QT4_WRAP_CPP( src_files ${QT_MOC_FILES} )

ADD_EXECUTABLE ( RobotModelDemo ${hdr_files} ${src_files} )

ADD_DEPENDENCIES( RobotModelDemo RobotModel )

TARGET_LINK_LIBRARIES ( RobotModelDemo 	${ROBOT_MODEL_LIB}
)

INSTALL ( TARGETS RobotModelDemo DESTINATION ${CMAKE_SOURCE_DIR}/bin )

