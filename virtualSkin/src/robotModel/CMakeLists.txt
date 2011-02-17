# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

PROJECT ( robotModel )

SET ( robotModel_VERSION_MAJOR 0 )
SET ( robotModel_VERSION_MINOR 1 )

### OpenGL ###
FIND_PACKAGE ( OpenGL )

### QT ###
FIND_PACKAGE ( Qt4 REQUIRED )
SET ( QT_USE_QTOPENGL TRUE )
SET ( QT_USE_QTXML TRUE )
INCLUDE ( ${QT_USE_FILE} )

### FreeSOLID ###
FIND_PACKAGE ( FreeSOLID REQUIRED )
INCLUDE_DIRECTORIES ( "${FREE_SOLID_INCLUDE_DIR}" )

### YARP ###
FIND_PACKAGE ( YARP REQUIRED )

SET ( src_files	partController.cpp
		renderList.cpp
		robotmodelexception.cpp
		worldRpcInterface.cpp
		yarpRpcPort.cpp
		zphandler.cpp
		robot.cpp
		interval.cpp
		motor.cpp
 		bodyPart.cpp
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
		displmatrix.cpp
		displaylist.cpp )

SET ( hdr_files	partController.h
		constants.h
		renderList.h
		robotmodelexception.h
		worldRpcInterface.h
		yarpRpcPort.h
		zphandler.h
		robot.h
		interval.h
		motor.h
 		bodyPart.h
 		link.h
    		primitiveobject.h 
    		cylinder.h 
    		joint.h 
    		joint_prismatic.h 
    		joint_revolute.h 
    		box.h 
    		world.h 
    		kintreenode.h 
    		object.h 
    		sphere.h
		displmatrix.h
		displaylist.h )

SET ( QT_MOC_FILES 	renderList.h
			yarpRpcPort.h
			worldRpcInterface.h )	
QT4_WRAP_CPP( src_files ${QT_MOC_FILES} )

ADD_LIBRARY ( robotModel STATIC ${hdr_files} ${src_files} )

# Tell XCode not to use /Debug and /Release directories for the binaries it builds
SET_TARGET_PROPERTIES ( robotModel PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})

TARGET_LINK_LIBRARIES ( robotModel ${FREE_SOLID_LIBRARIES} ${YARP_LIBRARIES} ${QT_LIBRARIES} ${OPENGL_LIBRARIES} )

INSTALL ( TARGETS   robotModel 	 DESTINATION ${CMAKE_SOURCE_DIR}/lib )
INSTALL ( FILES     ${hdr_files} DESTINATION ${CMAKE_SOURCE_DIR}/include/robotModel)
INSTALL ( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/freeSolid DESTINATION ${CMAKE_SOURCE_DIR}/include )

