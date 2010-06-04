# Copyright: (C) 2010 Gregor Kaufmann
# Author: Gregor Kaufmann
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

FIND_PACKAGE ( YARP REQUIRED )
INCLUDE_DIRECTORIES ( ${YARP_INCLUDE_DIRS} )
INCLUDE_DIRECTORIES ( "${CMAKE_SOURCE_DIR}/src/modules/yarpfilter/" )

ADD_LIBRARY ( idsiayarpfilter RpcFilterImpl.cpp RpcFilter.cpp StreamFilterImpl.cpp StreamFilter.cpp ControlBoardFilter.cpp )

INSTALL_TARGETS ( /lib idsiayarpfilter )
INSTALL ( DIRECTORY ${CMAKE_SOURCE_DIR}/src/modules/yarpfilter/yarp DESTINATION ${CMAKE_SOURCE_DIR}/include )
