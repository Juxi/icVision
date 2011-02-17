# Copyright: (C) 2010 Gregor Kaufmann
# Author: Gregor Kaufmann
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

FIND_PACKAGE ( YARP REQUIRED )
INCLUDE_DIRECTORIES ( "../../modules/katana400" )

ADD_EXECUTABLE ( Katana400Demo katana400_test.cpp )
TARGET_LINK_LIBRARIES ( Katana400Demo KNIBase ${IDSIA_YARP_MODULE_LIB} )

# Install executables and config files
INSTALL ( TARGETS Katana400Demo DESTINATION bin )

INSTALL ( FILES katana6M180.cfg DESTINATION bin )
INSTALL ( FILES katanaRemoteServer.ini DESTINATION bin )
