# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

SET ( src_files		main.cpp
                    partController.cpp
                    controller.cpp
)

SET ( hdr_files		partController.h
                    controller.h
)

QT4_WRAP_CPP( src_files controller.h )

ADD_EXECUTABLE ( FieldDemo	${hdr_files}
				${src_files} )

ADD_EXECUTABLE ( SimpleMotion	simpleMotion.cpp )
ADD_EXECUTABLE ( OpSpaceMotion	opSpaceMotion.cpp )

ADD_DEPENDENCIES( FieldDemo	YarpModel 
				KinematicModel
)

TARGET_LINK_LIBRARIES ( FieldDemo	YarpModel 
				 	KinematicModel
					${SOLID_LIBRARY}
					${QT_LIBRARIES}
					${OPENGL_LIBRARIES}
					${YARP_LIBRARIES}
)

TARGET_LINK_LIBRARIES ( SimpleMotion	${YARP_LIBRARIES})
TARGET_LINK_LIBRARIES ( OpSpaceMotion	${YARP_LIBRARIES})

INSTALL ( TARGETS FieldDemo DESTINATION ${CMAKE_SOURCE_DIR}/bin )
INSTALL ( TARGETS SimpleMotion DESTINATION ${CMAKE_SOURCE_DIR}/bin )

