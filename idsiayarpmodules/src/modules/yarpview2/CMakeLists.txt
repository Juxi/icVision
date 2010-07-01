# Copyright: (C) 2010 Gregor Kaufmann
# Author: Gregor Kaufmann
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

## The following section sets up YARP support 
FIND_PACKAGE( YARP )

## The following section sets up QT support
FIND_PACKAGE ( Qt4 REQUIRED )
# SET ( QT_USE_QTOPENGL TRUE )
# SET ( QT_USE_QTXML TRUE )
INCLUDE ( ${QT_USE_FILE} )

## Define the source files
SET ( SOURCE_FILES
	blob.cpp
#	dot.cpp
	dotparam.cpp
	grabber.cpp
	grabberfeature.cpp
	grabberfeaturewindow.cpp
	grabberprop.cpp
	grabberthread.cpp
	imageutils.cpp
	main.cpp
	mainwindow.cpp
	mathutils.cpp
	mirovision.cpp
#	utils.cpp
#	visionball.cpp
#	visionrobot.cpp
)
# camthread.cpp

# Resource files for which rcc has to be run need to be treated special	
SET ( QT_QRC_FILES
	camview.qrc
)	
QT4_ADD_RESOURCES(SOURCE_FILES ${QT_QRC_FILES})

# Source files for which moc has to be run need to be treated special	
SET ( QT_MOC_FILES
	grabberfeaturewindow.h
	grabberprop.h
	grabberthread.h
	mainwindow.h
)	

QT4_WRAP_CPP( SOURCE_FILES ${QT_MOC_FILES} )

# Compile and link the application
ADD_EXECUTABLE ( yarpview2 ${SOURCE_FILES} )
TARGET_LINK_LIBRARIES ( yarpview2 ${QT_LIBRARIES} dc1394 )

# Copy the binaries to the bin directory
INSTALL_TARGETS ( /bin yarpview2 )  
