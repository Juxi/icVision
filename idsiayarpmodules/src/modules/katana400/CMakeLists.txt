# Copyright: (C) 2010 Gregor Kaufmann
# Author: Gregor Kaufmann
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

FIND_PACKAGE ( YARP REQUIRED )
INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} )

# does the device need to be included in YARP
IF ( COMPILE_DEVICE_LIBRARY )
  PREPARE_DEVICE (katana400 TYPE Katana400 INCLUDE Katana400.h ) 
ENDIF ( COMPILE_DEVICE_LIBRARY )


IF ( NOT SKIP_katana400 )
  ADD_LIBRARY ( katana400 Katana400.cpp )
  TARGET_LINK_LIBRARIES( katana400 KNIBase ${YARP_LIBRARIES} )
ENDIF ( NOT SKIP_katana400 )
