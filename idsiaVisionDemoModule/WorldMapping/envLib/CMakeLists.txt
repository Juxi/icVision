CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
if(COMMAND cmake_policy)
     cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)


INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

set(CMAKE_C_FLAGS "${GNU_FLAGS} -std=gnu99 ")

# The extension is already found. Any number of sources could be listed here.
add_library (libEnv env_alloc.c env_channel.c env_c_math_ops.c env_image.c
env_image_ops.c envision_lib.c env_job_server.c env_log.c
env_math.c env_motion_channel.c env_mt_channel.c env_mt_visual_cortex.c
env_params.c env_pthread_interface.c env_pyr.c env_stdio_interface.c
env_visual_cortex.c)