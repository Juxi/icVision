# Copyright: (C) 2011 Juxi Leitner
# Author: Juxi Leitner <juxi.leitner@gmail.com>
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

SET ( PROJECTNAME ReachingDemo )
SET ( src_files main.cpp
		reaching_module.cpp
		reaching.cpp 
)


SET ( hdr_files reaching_module.h
		reaching.h
)

ADD_EXECUTABLE ( ${PROJECTNAME} ${hdr_files} ${src_files} )

TARGET_LINK_LIBRARIES (
	${PROJECTNAME}	${YARP_LIBRARIES}
			icubmod
			#${OpenCV_LIBS}
)

