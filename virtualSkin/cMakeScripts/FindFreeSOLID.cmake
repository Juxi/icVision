# Copyright: (C) 2011 Kail Frank
# Authors: Kail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

########################################################################
##  general find
SET( FreeSolidDIR $ENV{FREE_SOLID_DIR} )
IF( FreeSolidDIR )
	MESSAGE(STATUS "Found FREE_SOLID_DIR as an environment variable")
	FIND_PATH( FREE_SOLID_DIR		NAMES 	/include/SOLID/solid.h
							/include/SOLID/broad.h
							/include/SOLID/types.h
							/libsolid/.libs/libFreeSOLID.a
						PATHS	FreeSolidDIR
						NO_DEFAULT_PATH
	)
ELSE( FreeSolidDIR )
	MESSAGE(STATUS "No FREE_SOLID_DIR environment variable")
	FIND_PATH( FREE_SOLID_DIR		NAMES 		/include/SOLID/solid.h
								/include/SOLID/broad.h
								/include/SOLID/types.h
								/libsolid/.libs/libFreeSOLID.a
						PATHS		${CMAKE_SOURCE_DIR}/..
								${CMAKE_SOURCE_DIR}/../..
								~/
								~/Library
								/opt
								/Library
						PATH_SUFFIXES	FreeSOLID-2.1.1
								FreeSOLID
	)
ENDIF( FreeSolidDIR )
########################################################################
IF( FREE_SOLID_DIR )
	SET( FREE_SOLID_INCLUDE_DIR ${FREE_SOLID_DIR}/include )
	FIND_LIBRARY( FREE_SOLID_LIBRARY 	NAMES 	FreeSOLID
						PATHS	${FREE_SOLID_DIR}/libsolid/.libs
						NO_DEFAULT_PATH
	)
ELSE( FREE_SOLID_DIR )
	FIND_PATH( FREE_SOLID_INCLUDE_DIR 	NAMES 	/SOLID/solid.h
							/SOLID/broad.h
							/SOLID/types.h		
						PATHS 	/usr/local/include
							/usr/include
							/include
	)
	FIND_LIBRARY( FREE_SOLID_LIBRARY 	NAMES 	FreeSOLID
						PATHS 	/usr/local/lib
							/usr/lib
							/lib
	)
ENDIF( FREE_SOLID_DIR )

########################################################################

IF ( FREE_SOLID_INCLUDE_DIR )
	MESSAGE(STATUS "Found FreeSOLID include dir: ${FREE_SOLID_INCLUDE_DIR}")
	INCLUDE_DIRECTORIES ( ${FREE_SOLID_INCLUDE_DIR} )
ELSE ( FREE_SOLID_INCLUDE_DIR )
	MESSAGE(FATAL_ERROR "FreeSOLID include dir: not found")
ENDIF ( FREE_SOLID_INCLUDE_DIR )

IF ( FREE_SOLID_LIBRARY )
	MESSAGE(STATUS "Found FreeSOLID library: ${FREE_SOLID_LIBRARY}")
ELSE ( FREE_SOLID_LIBRARY )
	MESSAGE(FATAL_ERROR "FreeSOLID library: not found")
ENDIF ( FREE_SOLID_LIBRARY )