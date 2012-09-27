# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
	
SET ( src_files main.cpp
		 objectsoup.cpp )

SET ( hdr_files objectsoup.h )


ADD_EXECUTABLE ( WorldModelDemo ${hdr_files} ${src_files} )

ADD_DEPENDENCIES( WorldModelDemo KinematicModel )

TARGET_LINK_LIBRARIES ( WorldModelDemo KinematicModel
					 ${SOLID_LIBRARY}
					 ${QT_LIBRARIES}
					 ${OPENGL_LIBRARIES}			
)

INSTALL ( TARGETS WorldModelDemo DESTINATION ${CMAKE_SOURCE_DIR}/bin )
