# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.
	
SET ( src_files	main.cpp
		objectSoup.cpp )

SET ( hdr_files	objectSoup.h )

#SET ( QT_MOC_FILES objectSoup.h )	
#QT4_WRAP_CPP( src_files ${QT_MOC_FILES} )

ADD_EXECUTABLE ( WorldModelDemo ${hdr_files} ${src_files} )

ADD_DEPENDENCIES( WorldModelDemo WorldModel )

TARGET_LINK_LIBRARIES ( WorldModelDemo 	KinematicModel
					${SOLID_LIBRARY}
					${QT_LIBRARIES}
					${OPENGL_LIBRARIES}			
)

INSTALL ( TARGETS WorldModelDemo DESTINATION ${CMAKE_SOURCE_DIR}/bin )