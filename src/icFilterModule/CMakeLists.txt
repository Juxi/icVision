# Copyright: (C) 2011-2012 Juxi Leitner
# Author: Juxi Leitner <juxi.leitner@gmail.com>
# CopyPolicy: Released under the terms of the GNU GPL v2.0

### Find External Libraries ###
FIND_PACKAGE ( YARP REQUIRED )
FIND_PACKAGE ( OpenCV REQUIRED )

INCLUDE_DIRECTORIES (${YARP_INCLUDE_DIRS})
INCLUDE_DIRECTORIES (${ICUB_INCLUDE_DIRS})

### Find External Libraries ###
FIND_PACKAGE ( OpenCV REQUIRED )
FIND_PACKAGE ( YARP REQUIRED )
INCLUDE_DIRECTORIES (${YARP_INCLUDE_DIRS})
INCLUDE_DIRECTORIES (${ICUB_INCLUDE_DIRS})
INCLUDE_DIRECTORIES (${OpenCV_INCLUDE_DIRS})

### Add sources for library
SET ( src_files icFilterModule.cpp
		icEvolvedFilterModule.cpp

		GaborImage.cpp
		icImage.cpp	
		../evolvedFilters/blue_detector.cpp
#		evolved_filters/glass_detector.cpp
		../evolvedFilters/redblock_detector.cpp
		../evolvedFilters/teabox.cpp
		../evolvedFilters/mars.cpp
		face_detector.cpp		
)

SET ( hdr_files icFilterModule.h
		icEvolvedFilterModule.h
		boolList.h
		icImage.h
		GaborImage.h

		face_detector.h
# temporarily
		../evolvedFilters/blue_detector.h
#		evolved_filters/glass_detector.h
		../evolvedFilters/redblock_detector.h
		../evolvedFilters/teabox.h
		../evolvedFilters/mars.h
)

ADD_LIBRARY ( icVisionModule STATIC ${hdr_files} ${src_files} )

### Also add sources for a test module using the library
ADD_EXECUTABLE ( FilterTestModule main.cpp
		testModule.h redFilterModule.h ${hdr_files}
		testModule.cpp redFilterModule.cpp ${src_files}
)

TARGET_LINK_LIBRARIES ( FilterTestModule
			icVisionModule
			${YARP_LIBRARIES}
			${OpenCV_LIBS}
)
