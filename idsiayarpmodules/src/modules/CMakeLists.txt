#
# Derived from a file from the YARP distribution
#
# Copyright: (C) 2010 Gregor Kaufmann
# Author: Gregor Kaufmann
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

PROJECT( IdsiaYarpModules )


###
### DEVICE-DRIVER modules
###

INCLUDE_DIRECTORIES ( ${YARP_OS_INC} ${YARP_sig_INC} ${YARP_dev_INC} )
IF ( WIN32 AND NOT CYGWIN )
  LINK_LIBRARIES ( optimized YARP_dev debug YARP_devd )
  LINK_LIBRARIES ( optimized YARP_sig debug YARP_sigd )
  LINK_LIBRARIES ( optimized YARP_OS debug YARP_OSd )
ELSE ( WIN32 AND NOT CYGWIN )
  LINK_LIBRARIES ( YARP_dev YARP_sig YARP_OS )
ENDIF ( WIN32 AND NOT CYGWIN )

# This directory may be compiled independently of the rest of YARP,
# so we need to pick up YARP explicitly.
IF ( NOT COMPILING_ALL_YARP )
  FIND_PACKAGE ( YARP REQUIRED )
ELSE ( NOT COMPILING_ALL_YARP )
  INCLUDE ( YarpDevice )
  SET ( YARP_DIR ${CMAKE_SOURCE_DIR} )
ENDIF ( NOT COMPILING_ALL_YARP )

IF ( MERGE_DEVICE_LIBRARY_MODULES )
  # add in the content of YARP_dev
  GET_TARGET_PROPERTY ( MERGE_SRCS YARP_dev merge_srcs )
ENDIF ( MERGE_DEVICE_LIBRARY_MODULES )

# Prepare a device library called "idsiayarpmod".
# A test executable called "idsiayarpmoddev" will also be produced.
BEGIN_DEVICE_LIBRARY ( idsiayarpmod )
  ADD_SUBDIRECTORY ( firewirecamera2 )
  ADD_SUBDIRECTORY ( katana400 )
END_DEVICE_LIBRARY ( idsiayarpmod )

IF ( MSVC )
    SET_TARGET_PROPERTIES ( idsiayarpmod PROPERTIES COMPILE_FLAGS /wd4996 )
ENDIF ( MSVC )

# SET ( IDSIA_YARP_MODULE_LIB ${CMAKE_CURRENT_BINARY_DIR} )
# MESSAGE ( IDSIA_YARP_MODULE_LIB ${IDSIA_YARP_MODULE_LIB} )

INSTALL_TARGETS ( /lib idsiayarpmod )
INSTALL_TARGETS ( /bin idsiayarpmoddev )
# FILE(GLOB header ${YARP_DIR}/*.h)
# INSTALL_FILES(/include FILES ${header})

###
### NON-DEVICE-DRIVER modules
###
ADD_SUBDIRECTORY ( yarpview2 )
ADD_SUBDIRECTORY ( yarpfilter )
