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

INCLUDE_DIRECTORIES ( "${CMAKE_SOURCE_DIR}/src/modules/yarpfilter/" )

SET ( driver_src_files KatanaDriver.cpp )
SET ( observer_src_files katanaObserver.cpp robotinterface.cpp
		RobotFilter.cpp CommandObserver.cpp StateObserver.cpp CallObserver.cpp
		ResponseObserver.cpp DetectionParams.cpp Blob.cpp utils.cpp)
		
# tell cmake to create .moc files for all files in the variable qtproject_SRCS
# that require such a file.
# note: this assumes that you use #include "header.moc" in your files
QT4_AUTOMOC ( ${observer_src_files} )

# The generated MOC files are in the binary tree
INCLUDE_DIRECTORIES ( ${CMAKE_CURRENT_BINARY_DIR} )

# If glut is set up, we can turn this on
ADD_DEFINITIONS( -DUSE_GLUT )

# A driver program for the Katana
ADD_EXECUTABLE ( KatanaDriver ${driver_src_files} )
TARGET_LINK_LIBRARIES ( KatanaDriver ${YARP_LIBRARIES} )

# An observer program for the Katana
ADD_EXECUTABLE ( KatanaObserver ${observer_src_files} )
TARGET_LINK_LIBRARIES ( KatanaObserver ${IDSIA_YARP_FILTER_LIB}
	 ${YARP_LIBRARIES} ${QT_LIBRARIES} )

# Install executables and config files
INSTALL ( TARGETS KatanaDriver DESTINATION bin )
INSTALL ( TARGETS KatanaObserver DESTINATION bin )

INSTALL ( FILES observerconfig.ini DESTINATION bin )
