# Copyright: (C) 2010-2011 Juxi Leitner
# Author: Juxi Leitner <juxi.leitner@gmail.com> @ IDSIA.ch
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

### Location of scripts to find Ace and SOLID ###
#SET ( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cMakeScripts )

### Headers within this project ###
INCLUDE_DIRECTORIES ( "${CMAKE_CURRENT_SOURCE_DIR}" )

### Find External Libraries ###
FIND_PACKAGE ( OpenCV REQUIRED )

FIND_PACKAGE ( OpenGL REQUIRED )

FIND_PACKAGE ( Qt4 REQUIRED )
        SET ( QT_USE_QTOPENGL TRUE )
        SET ( QT_USE_QTXML TRUE )
        INCLUDE ( ${QT_USE_FILE} )

FIND_PACKAGE ( YARP REQUIRED )
INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} )

IF ( LINK_ACE )
        FIND_PACKAGE ( ACE REQUIRED )
        INCLUDE_DIRECTORIES ( ${ACE_INCLUDE_DIR} )
ENDIF ( LINK_ACE )

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

### Include the directories where build targets are ###

SET ( src_files main.cpp
                cv_widget.cpp
                dashboard.cpp
                iCub_controller.cpp
		window.cpp
		ColoredRect.cpp
		partController.cpp
)

SET ( hdr_files cv_widget.h
		dashboard.h
		iCub_controller.h
		window.h
		ColoredRect.h
		partController.h
)

SET ( QT_MOC_FILES      dashboard.h
                        iCub_controller.h
                        window.h
)

QT4_WRAP_CPP( src_files ${QT_MOC_FILES} )

ADD_EXECUTABLE ( ${PROJECTNAME} ${hdr_files} ${src_files} )


TARGET_LINK_LIBRARIES ( ${PROJECTNAME} ${OpenCV_LIBS}
				 iKin
				#### iCub
				 ${QT_LIBRARIES}
                                 ${OPENGL_LIBRARIES}
				 ${YARP_LIBRARIES}
)
