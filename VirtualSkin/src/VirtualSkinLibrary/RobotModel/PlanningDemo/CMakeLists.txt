# Copyright: (C) 2011 Tobias Glasmachers
# Author: Tobias Glasmachers
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

#SET ( ROBOT_MODEL_LIB "RobotModel" )

SET ( src_files	fwdkin.cpp
		controlThread.cpp
		rng.cpp
		elitistxnes.cpp
		kernelfunction.cpp
		fitness.cpp
		main.cpp
)

SET ( hdr_files
		fwdkin.h
		controlThread.h
		vector.h
		matrix.h
		rng.h
		elitistxnes.h
		kernelfunction.h
		fitnessfunction.h
		fitness.h
)

SET ( QT_MOC_FILES 	fwdkin.h controlThread.h
)

QT4_WRAP_CPP( src_files ${QT_MOC_FILES} )

ADD_EXECUTABLE ( PlanningDemo ${hdr_files} ${src_files} )

ADD_DEPENDENCIES( PlanningDemo RobotModel )

TARGET_LINK_LIBRARIES ( PlanningDemo 	RobotModel
					${SOLID_LIBRARY}
					${QT_LIBRARIES}			
					${OPENGL_LIBRARIES}
)

INSTALL ( TARGETS PlanningDemo DESTINATION ${CMAKE_SOURCE_DIR}/bin )
