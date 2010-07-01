CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

PROJECT ( idsiayarpmodules )

# Use g++ / gcc compilers for optimal Eclipse support
SET ( CMAKE_CXX_COMPILER /usr/bin/g++ CACHE FILEPATH "please use g++ for #optimal Eclipse support" FORCE )
SET ( CMAKE_C_COMPILER /usr/bin/gcc CACHE FILEPATH "please use gcc for #optimal Eclipse support" FORCE )
# SET ( CMAKE_BUILD_TYPE DEBUG CACHE STRING "please use DEBUG to allow for debugging in Eclipse" FORCE )

IF ( CMAKE_COMPILER_IS_GNUCC )
  SET ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0" )
ENDIF ( CMAKE_COMPILER_IS_GNUCC )
IF ( CMAKE_COMPILER_IS_GNUCXX )
  SET ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0" )
ENDIF ( CMAKE_COMPILER_IS_GNUCXX )

ADD_SUBDIRECTORY ( src )
