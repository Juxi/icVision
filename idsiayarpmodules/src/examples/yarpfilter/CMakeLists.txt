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

SET ( demo_src_files SimpleQtObserver.cpp SimpleQtSimulator.cpp filterDemo.cpp )
SET ( driver_src_files tutorial_arm.cpp )
SET ( robotFilter_src_files robotFilterDemo.cpp robotinterface.cpp
		RobotFilter.cpp StateObserver.cpp CallObserver.cpp
		ResponseObserver.cpp utils.cpp)
		
# tell cmake to create .moc files for all files in the variable qtproject_SRCS
# that require such a file.
# note: this assumes that you use #include "header.moc" in your files
QT4_AUTOMOC ( ${demo_src_files} ${robotFilter_src_files} )

# The generated MOC files are in the binary tree
INCLUDE_DIRECTORIES ( ${CMAKE_CURRENT_BINARY_DIR} )

# If glut is set up, we can turn this on
ADD_DEFINITIONS( -DUSE_GLUT )

# The control board filter demo main application
ADD_EXECUTABLE ( FilterDemo ${demo_src_files} )
TARGET_LINK_LIBRARIES ( FilterDemo ${IDSIA_YARP_FILTER_LIB}
	${YARP_LIBRARIES} ${QT_LIBRARIES} )

# A driver program for the demo
ADD_EXECUTABLE ( FilterDriver ${driver_src_files} )
TARGET_LINK_LIBRARIES ( FilterDriver ${YARP_LIBRARIES} )

# A demo program for the robot filter
ADD_EXECUTABLE ( RobotFilterDemo ${robotFilter_src_files} )
TARGET_LINK_LIBRARIES ( RobotFilterDemo ${IDSIA_YARP_FILTER_LIB}
	 ${YARP_LIBRARIES} ${QT_LIBRARIES} )

INSTALL_TARGETS ( /bin FilterDemo )
INSTALL_TARGETS ( /bin FilterDriver )
INSTALL_TARGETS ( /bin RobotFilterDemo )
