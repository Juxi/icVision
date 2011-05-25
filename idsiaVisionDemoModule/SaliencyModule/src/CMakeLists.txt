# Copyright: (C) 2011 Juxi Leitner
# Author: Juxi Leitner <juxi.leitner@gmail.com>
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

SET ( src_files main.cpp
		saliency_module.cpp
		saliency.cpp 
)


SET ( hdr_files saliency_module.h
		saliency.h
)

ADD_EXECUTABLE ( cvSaliency ${hdr_files} ${src_files} )

TARGET_LINK_LIBRARIES (
	cvSaliency	envisionLib
			${OpenCV_LIBS}
			${YARP_LIBRARIES}		   
)

