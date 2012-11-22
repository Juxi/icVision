# Copyright: (C) 2012 Mikhail Frank
# Author: Mikhail Frank
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

SET ( PARSED_LIB_KINEMATIC false )
SET ( PARSED_LIB_YARP_MODEL false )

ADD_SUBDIRECTORY ( LibKinematicModel )
ADD_SUBDIRECTORY ( LibYarpModel )


ADD_SUBDIRECTORY ( Apps )
