# Copyright: (C) 2010 Gregor Kaufmann
# Author: Gregor Kaufmann
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

FIND_PACKAGE ( YARP REQUIRED )
INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} )

# does the device need to be included in YARP
IF ( COMPILE_DEVICE_LIBRARY )
  PREPARE_DEVICE (firewirecamera2 TYPE FirewireCamera2 INCLUDE FirewireCamera2.h ) 
ENDIF ( COMPILE_DEVICE_LIBRARY )

IF ( NOT SKIP_firewirecamera2 )
  ADD_LIBRARY ( firewirecamera2 FirewireCamera2.cpp FirewireCamera2Set.cpp )
  IF ( APPLE )
    FIND_LIBRARY( CORE_SERVICES_LIBRARY CoreServices )
    MARK_AS_ADVANCED ( CORE_SERVICES_LIBRARY )
    TARGET_LINK_LIBRARIES( firewirecamera2 dc1394 ${CORE_SERVICES_LIBRARY} ${YARP_LIBRARIES} )
  ELSE ( APPLE )
    TARGET_LINK_LIBRARIES( firewirecamera2 dc1394 ${YARP_LIBRARIES} )
  ENDIF ( APPLE )
ENDIF ( NOT SKIP_firewirecamera2 )
