# Copyright: (C) 2011 Juxi Leitner
# Author: Juxi Leitner <juxi.leitner@gmail.com>
# CopyPolicy: Released under the terms of the GNU GPL v2.0

### Find External Libraries ###
FIND_PACKAGE ( YARP REQUIRED )
FIND_PACKAGE ( OpenCV REQUIRED )

INCLUDE_DIRECTORIES (${YARP_INCLUDE_DIRS})
INCLUDE_DIRECTORIES (${ICUB_INCLUDE_DIRS})

SET(PROJECTNAME visionCore )
PROJECT ( ${PROJECTNAME} )

### Create some options for use with ccmake ###
option (LINK_ACE ON)
IF ( LINK_ACE )
        FIND_PACKAGE ( ACE REQUIRED )
        INCLUDE_DIRECTORIES ( ${ACE_INCLUDE_DIR} )
ENDIF ( LINK_ACE )

### Add sources
SET ( src_files core_module.cpp
		icFilterPort.cpp
		../icFilterModule/icImage.cpp
		../icFilterModule/GaborImage.cpp
)

SET ( hdr_files core_module.h
		icFilterPort.h
		module_info.h
		../icFilterModule/icImage.h
		../icFilterModule/GaborImage.h
)

ADD_EXECUTABLE ( ${PROJECTNAME} ${hdr_files} ${src_files} main.cpp )
TARGET_LINK_LIBRARIES (
	${PROJECTNAME}	${YARP_LIBRARIES} ${OpenCV_LIBS}
)


### Find External Libraries ###
option (CORE_GUI "Create the QT GUI for the icVision core" ON)
if( CORE_GUI )

	FIND_PACKAGE ( Qt4 REQUIRED )
        INCLUDE ( ${QT_USE_FILE} )
	INCLUDE_DIRECTORIES (${YARP_INCLUDE_DIRS})
	INCLUDE_DIRECTORIES (${ICUB_INCLUDE_DIRS})

	### Set some platform-specific variables ###
	SET (Q_WS_MAC FALSE)
	SET (Q_WS_X11 FALSE)
	SET (Q_WS_WIN FALSE)
	IF ( UNIX )
        	IF ( APPLE )
                	SET (Q_WS_MAC TRUE)
	                SET (QT_MAC_USE_COCOA TRUE)
	        ELSE ( APPLE )
        	        SET (Q_WS_X11 TRUE)
	        ENDIF ( APPLE )
	ENDIF ( UNIX )
	IF ( WIN32 )
        	SET (Q_WS_WIN TRUE)
	        ADD_DEFINITIONS( /D "WIN32_LEAN_AND_MEAN" )
	ENDIF ( WIN32 )

	### Add sources for GUI
	SET ( guisrc_files GUI/main.cpp
			GUI/window.cpp
			GUI/module_widget.cpp
	)

	SET ( guihdr_files core_module.h
			GUI/window.h
			GUI/module_thread.h
			GUI/module_widget.h
	)

	SET ( QT_MOC_FILES
			GUI/window.h
	)
	

	QT4_WRAP_CPP( guisrc_files ${QT_MOC_FILES} )


	ADD_EXECUTABLE ( ${PROJECTNAME}GUI ${hdr_files} ${src_files} ${guihdr_files} ${guisrc_files}  )

	TARGET_LINK_LIBRARIES (
		${PROJECTNAME}GUI	${YARP_LIBRARIES}
				 ${OpenCV_LIBS}
				 ${QT_LIBRARIES}
	)

else(CORE_GUI)
    message(STATUS "OpenCV not found/selected, skipping iCubVis")
endif(CORE_GUI)

