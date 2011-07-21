# Copyright: (C) 2011 Kail Frank
# Authors: Kail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

########################################################################

SET( SOLID_DIR $ENV{SOLID_DIR} )

IF( SOLID_DIR )
	FIND_PATH( SOLID_INCLUDE_DIR	NAMES 	/solid.h
						/broad.h
						/types.h
					PATHS	${SOLID_DIR}/include
					NO_DEFAULT_PATH
	)
	FIND_LIBRARY( SOLID_LIBRARY 	NAMES 	solid
					PATHS	${SOLID_DIR}/libsolid/.libs
						${SOLID_DIR}/src/.libs
					NO_DEFAULT_PATH
	)
ELSE( SOLID_DIR )
	FIND_PATH( SOLID_INCLUDE_DIR 	NAMES		/SOLID.h
							/SOLID_broad.h
							/SOLID_types.h		
					PATHS 		/opt/local/include
							/opt/include
							/usr/local/include
							/usr/include
							/include
					PATH_SUFFIXES	SOLID
	)
	FIND_LIBRARY( SOLID_LIBRARY 	NAMES 		solid
					PATHS 		/opt/local/lib
							/opt/lib
							/usr/local/lib
							/usr/lib
							/lib
	)
ENDIF( SOLID_DIR )

########################################################################

IF ( SOLID_INCLUDE_DIR )
	MESSAGE(STATUS "Found SOLID include dir: ${SOLID_INCLUDE_DIR}")
	INCLUDE_DIRECTORIES ( ${SOLID_INCLUDE_DIR} )
ELSE ( SOLID_INCLUDE_DIR )
	MESSAGE(FATAL_ERROR "SOLID include dir: not found")
ENDIF ( SOLID_INCLUDE_DIR )

IF ( SOLID_LIBRARY )
	MESSAGE(STATUS "Found SOLID library: ${SOLID_LIBRARY}")
ELSE ( SOLID_LIBRARY )
	MESSAGE(FATAL_ERROR "SOLID library: not found")
ENDIF ( SOLID_LIBRARY )