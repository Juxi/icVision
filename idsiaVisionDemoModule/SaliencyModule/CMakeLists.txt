# Copyright: (C) 2011 Juxi Leitner
# Author: Juxi Leitner <juxi.leitner@gmail.com>
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.8 )

PROJECT ( cvSaliency )

### Create some options for use with ccmake ###
option (LINK_ACE ON)

### Headers within this project ###
INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}" )
INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/lib" )
#INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}/robotModel/" )

### Find External Libraries ###
FIND_PACKAGE ( OpenCV REQUIRED )

#FIND_PACKAGE ( Qt4 REQUIRED )
#        SET ( QT_USE_QTOPENGL TRUE )
#        SET ( QT_USE_QTXML TRUE )
#        INCLUDE ( ${QT_USE_FILE} )

FIND_PACKAGE ( YARP REQUIRED )
INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} )

IF ( LINK_ACE )
        FIND_PACKAGE ( ACE REQUIRED )
        INCLUDE_DIRECTORIES ( ${ACE_INCLUDE_DIR} )
ENDIF ( LINK_ACE )


SET ( src_files main.cpp
		saliency_module.cpp
		saliency.cpp 
)
SET ( lib_files lib/env_alloc.c
		lib/env_channel.c
		lib/env_c_math_ops.c
		lib/env_image.c
		lib/env_image_ops.c
		#lib/envision.c
		lib/envision_lib.c
		lib/env_job_server.c
		lib/env_log.c
		lib/env_math.c
		lib/env_motion_channel.c
		lib/env_mt_channel.c
		lib/env_mt_visual_cortex.c
		lib/env_params.c
		lib/env_pthread_interface.c
		lib/env_pyr.c
		lib/env_stdio_interface.c
		lib/env_visual_cortex.c
)

SET ( hdr_files saliency_module.h
		saliency.h
)

SET ( CMAKE_C_FLAGS "${GNU_FLAGS} -std=gnu99" )
ADD_EXECUTABLE ( cvSaliency ${hdr_files} ${src_files} ${lib_files})

TARGET_LINK_LIBRARIES ( cvSaliency ${OpenCV_LIBS}
				   ${YARP_LIBRARIES}
)

