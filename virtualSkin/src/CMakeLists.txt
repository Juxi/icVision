# Copyright: (C) 2010 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

SET ( CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cMakeScripts )

ADD_SUBDIRECTORY ( yarpFilter )
ADD_SUBDIRECTORY ( robotModel )
ADD_SUBDIRECTORY ( virtualSkin )
ADD_SUBDIRECTORY ( iCubBabbler )
