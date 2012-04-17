# Copyright: (C) 2011 Juxi Leitner
# Author: Juxi Leitner <juxi.leitner@gmail.com>
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

CMAKE_MINIMUM_REQUIRED ( VERSION 2.8 )

SET(PROJECTNAME icVision)
PROJECT ( ${PROJECTNAME} )

### Create some options for use with ccmake ###
option (LINK_ACE ON)
IF ( LINK_ACE )
        FIND_PACKAGE ( ACE REQUIRED )
        INCLUDE_DIRECTORIES ( ${ACE_INCLUDE_DIR} )
ENDIF ( LINK_ACE )

### Add sources

# core and coreGUI
ADD_SUBDIRECTORY (core)

# core functionality: filter (base functionality), disparity
ADD_SUBDIRECTORY (icFilterModule)
ADD_SUBDIRECTORY (icDisparityModule)

# # TBA localisation and saliency, http ...

# FilterModules and evolved Filters
# TBA evolved parts
# ADD_SUBDIRECTORY (httpModule)
#ADD_SUBDIRECTORY (evolvedFilters)
# ?! ADD_SUBDIRECTORY (redTarget)

