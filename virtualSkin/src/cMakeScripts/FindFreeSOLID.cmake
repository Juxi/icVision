# Copyright: (C) 2011 Kail Frank
# Authors: Kail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

########################################################################
##  general find

FIND_PATH( FREE_SOLID_INCLUDE_DIR 	NAMES 	SOLID/solid.h SOLID/broad.h SOLID/types.h
					PATHS 	$ENV{FREE_SOLID_ROOT}
						$ENV{FREE_SOLID_ROOT}/include
						/usr/include
						/usr/local/include
						~/Library/FreeSOLID
						~/Library/FreeSOLID/include
)

FIND_LIBRARY( FREE_SOLID_LIBRARY 	NAMES 	FreeSOLID solid SOLID Solid FreeSolid
					PATHS 	$ENV{FREE_SOLID_ROOT}
						$ENV{FREE_SOLID_ROOT}/build
						/usr/lib
						/usr/local/lib
)

########################################################################

IF ( FREE_SOLID_INCLUDE_DIR )
	MESSAGE(STATUS "Found FreeSOLID include dir: ${FREE_SOLID_INCLUDE_DIR}")
	INCLUDE_DIRECTORIES ( "${FREE_SOLID_INCLUDE_DIR}" )
ELSE ( FREE_SOLID_INCLUDE_DIR )
	MESSAGE(FATAL_ERROR "FreeSOLID include dir: not found")
ENDIF ( FREE_SOLID_INCLUDE_DIR )

IF ( FREE_SOLID_LIBRARY )
	MESSAGE(STATUS "Found FreeSOLID library: ${FREE_SOLID_LIBRARY}")
ELSE ( FREE_SOLID_LIBRARY )
	MESSAGE(FATAL_ERROR "FreeSOLID library: not found")
ENDIF ( FREE_SOLID_LIBRARY )