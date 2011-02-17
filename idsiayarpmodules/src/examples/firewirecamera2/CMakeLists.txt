# Copyright: (C) 2010 Gregor Kaufmann
# Author: Gregor Kaufmann
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

FIND_PACKAGE ( YARP REQUIRED )
INCLUDE_DIRECTORIES ( "../../modules/firewirecamera2" )

ADD_EXECUTABLE ( FirewireCamera2Demo firewireCamera2_test.cpp )
IF ( APPLE )
  FIND_LIBRARY ( CORE_SERVICES_LIBRARY CoreServices )
  MARK_AS_ADVANCED ( CORE_SERVICES_LIBRARY )
  TARGET_LINK_LIBRARIES ( FirewireCamera2Demo dc1394 ${IDSIA_YARP_MODULE_LIB} ${CORE_SERVICES_LIBRARY} )
ELSE ( APPLE )
  TARGET_LINK_LIBRARIES ( FirewireCamera2Demo dc1394 ${IDSIA_YARP_MODULE_LIB} )
ENDIF ( APPLE )
  
# Install executables and config files
INSTALL ( TARGETS FirewireCamera2Demo DESTINATION bin )

INSTALL ( FILES remoteFirewirecamera2.ini DESTINATION bin )
