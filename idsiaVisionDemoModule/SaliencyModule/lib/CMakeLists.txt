# Copyright: (C) 2011 Juxi Leitner
# Author: Juxi Leitner <juxi.leitner@gmail.com>
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

SET ( CMAKE_C_FLAGS "${GNU_FLAGS} -std=gnu99" )

ADD_LIBRARY ( envisionLib
	env_alloc.c
	env_channel.c
	env_c_math_ops.c
	env_image.c
	env_image_ops.c
	#lib/envision.c
	envision_lib.c
	env_job_server.c
	env_log.c
	env_math.c
	env_motion_channel.c
	env_mt_channel.c
	env_mt_visual_cortex.c
	env_params.c
	env_pthread_interface.c
	env_pyr.c
	env_stdio_interface.c
	env_visual_cortex.c
)


