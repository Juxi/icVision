# Copyright: (C) 2010 Gregor Kaufmann
# Author: Gregor Kaufmann
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

FIND_PACKAGE ( YARP REQUIRED )
SET ( IDSIA_YARP_MODULE_LIB "${CMAKE_BINARY_DIR}/src/modules/libidsiayarpmod.a" )
SET ( IDSIA_YARP_FILTER_LIB "${CMAKE_BINARY_DIR}/src/modules/yarpfilter/libidsiayarpfilter.a" )


# YARP filter

ADD_SUBDIRECTORY ( yarpfilter )


# Firewire Camera 2

IF ( ENABLE_idsiayarpmod_firewirecamera2 )
  ADD_SUBDIRECTORY ( firewirecamera2 )
ENDIF ( ENABLE_idsiayarpmod_firewirecamera2 )


# Katana Driver

IF ( ENABLE_idsiayarpmod_katana400 )
  ADD_SUBDIRECTORY ( katana400 )
ENDIF ( ENABLE_idsiayarpmod_katana400 )


# Katana Observer

ADD_SUBDIRECTORY ( katanaobserver )


# Collision Avoidance Filter

ADD_SUBDIRECTORY ( katanaCollisionAvoidance )
