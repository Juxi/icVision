# Copyright: (C) 2010 Gregor Kaufmann
# Author: Gregor Kaufmann
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

## The following section sets up YARP support 
FIND_PACKAGE( YARP )

# openGL
FIND_PACKAGE ( OpenGL )

## The following section sets up QT support
FIND_PACKAGE ( Qt4 REQUIRED )

SET ( QT_USE_QTOPENGL TRUE )
SET ( QT_USE_QTXML TRUE )

# SET ( QT_DONT_USE_QTCORE )
# SET ( QT_DONT_USE_QTGUI )
# SET ( QT_USE_QT3SUPPORT )
# SET ( QT_USE_QTASSISTANT )
# SET ( QT_USE_QAXCONTAINER )
# SET ( QT_USE_QAXSERVER )
# SET ( QT_USE_QTDESIGNER )
# SET ( QT_USE_QTMOTIF )
# SET ( QT_USE_QTMAIN )
# SET ( QT_USE_QTNETWORK )
# SET ( QT_USE_QTNSPLUGIN )
# SET ( QT_USE_QTOPENGL )
# SET ( QT_USE_QTSQL )
# SET ( QT_USE_QTXML )
# SET ( QT_USE_QTSVG )
# SET ( QT_USE_QTTEST )
# SET ( QT_USE_QTUITOOLS )
# SET ( QT_USE_QTDBUS )
# SET ( QT_USE_QTSCRIPT )
# SET ( QT_USE_QTASSISTANTCLIENT )
# SET ( QT_USE_QTHELP )
# SET ( QT_USE_QTWEBKIT )
# SET ( QT_USE_QTXMLPATTERNS )
# SET ( QT_USE_PHONON )

INCLUDE ( ${QT_USE_FILE} )
# MESSAGE ( " QT_USE_FILE: ${QT_USE_FILE}" )
# MESSAGE ( "QT_LIBRARIES: ${QT_LIBRARIES}" )

INCLUDE_DIRECTORIES ( "${CMAKE_SOURCE_DIR}/src/modules/yarpfilter/" )

# SET ( demo_src_files SimpleQtObserver.cpp SimpleQtSimulator.cpp filterDemo.cpp )
# SET ( driver_src_files tutorial_arm.cpp )
# SET ( robotFilter_src_files robotFilterDemo.cpp robotinterface.cpp
# 		RobotFilter.cpp StateObserver.cpp CallObserver.cpp
#		ResponseObserver.cpp utils.cpp)
SET ( kails_src_files
	window.cpp
    main.cpp
    zphandler.cpp
    robot.cpp
    interval.cpp
    glwidget.cpp
    motor.cpp
    branch.cpp
#    robotinterface.cpp
    link.cpp
    primitiveobject.cpp 
    cylinder.cpp 
    joint.cpp 
    joint_prismatic.cpp 
    joint_revolute.cpp 
    box.cpp 
    world.cpp 
    kintreenode.cpp 
    object.cpp 
    sphere.cpp 
    displaylist.cpp
    RobotFilter.cpp
	StateObserver.cpp
	CallObserver.cpp
	ResponseObserver.cpp
)

# Source files for which moc has to be run need to be treated special	
SET ( QT_MOC_FILES
	glwidget.h
	window.h
	world.h
	robot.h
	collisiondetector.h
	RobotFilter.h
	)	
QT4_WRAP_CPP( kails_src_files ${QT_MOC_FILES} )

# If glut is set up, we can turn this on
ADD_DEFINITIONS( -DUSE_GLUT )

# Kail's original application
ADD_EXECUTABLE ( KailsDemo ${kails_src_files} )
TARGET_LINK_LIBRARIES ( KailsDemo FreeSOLID ${IDSIA_YARP_FILTER_LIB}
	${YARP_LIBRARIES} ${QT_LIBRARIES} ${OPENGL_LIBRARIES} )

# The control board filter demo main application
# ADD_EXECUTABLE ( CollisionAvoidanceDemo ${demo_src_files} )
# TARGET_LINK_LIBRARIES ( FilterDemo ${IDSIA_YARP_FILTER_LIB}
#	${YARP_LIBRARIES} ${QT_LIBRARIES} )

# A driver program for the demo
# ADD_EXECUTABLE ( FilterDriver ${driver_src_files} )
# TARGET_LINK_LIBRARIES ( FilterDriver ${YARP_LIBRARIES} )

# A demo program for the robot filter
# ADD_EXECUTABLE ( RobotFilterDemo ${robotFilter_src_files} )
# TARGET_LINK_LIBRARIES ( RobotFilterDemo ${IDSIA_YARP_FILTER_LIB}
#	 ${YARP_LIBRARIES} ${QT_LIBRARIES} )

# INSTALL_TARGETS ( /bin FilterDemo )
# INSTALL_TARGETS ( /bin FilterDriver )
# INSTALL_TARGETS ( /bin RobotFilterDemo )
INSTALL_TARGETS ( /bin KailsDemo )
