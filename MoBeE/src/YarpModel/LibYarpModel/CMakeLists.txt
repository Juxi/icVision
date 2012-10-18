# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

SET ( src_files		yarpModel.cpp
			yarprobot.cpp
			worldRpcInterface.cpp
			yarpStreamPort.cpp
			exception.cpp
			simSyncer.cpp
			robotSyncer.cpp
)

SET ( hdr_files		constants.h
			exception.h
			yarpModel.h
			yarprobot.h
			worldRpcInterface.h
			yarpStreamPort.h
			simSyncer.h
			robotSyncer.h
		
)

ADD_LIBRARY ( YarpModel STATIC ${hdr_files} ${src_files} )

ADD_DEPENDENCIES( YarpModel KinematicModel )

TARGET_LINK_LIBRARIES ( YarpModel KinematicModel
				  ${SOLID_LIBRARY}
				  ${QT_LIBRARIES}
				  ${OPENGL_LIBRARIES}
				    ${YARP_LIBRARIES}
)

INSTALL ( TARGETS YarpModel DESTINATION ${CMAKE_SOURCE_DIR}/lib )

ADD_SUBDIRECTORY ( KinematicModel )

