# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/local/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/stollenga/workspace/IM-CLeVeR/mover/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/stollenga/workspace/IM-CLeVeR/mover/src

# Include any dependencies generated for this target.
include CMakeFiles/iCubMover.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/iCubMover.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/iCubMover.dir/flags.make

CMakeFiles/iCubMover.dir/main.cpp.o: CMakeFiles/iCubMover.dir/flags.make
CMakeFiles/iCubMover.dir/main.cpp.o: main.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/stollenga/workspace/IM-CLeVeR/mover/src/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/iCubMover.dir/main.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/iCubMover.dir/main.cpp.o -c /home/stollenga/workspace/IM-CLeVeR/mover/src/main.cpp

CMakeFiles/iCubMover.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/iCubMover.dir/main.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/stollenga/workspace/IM-CLeVeR/mover/src/main.cpp > CMakeFiles/iCubMover.dir/main.cpp.i

CMakeFiles/iCubMover.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/iCubMover.dir/main.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/stollenga/workspace/IM-CLeVeR/mover/src/main.cpp -o CMakeFiles/iCubMover.dir/main.cpp.s

CMakeFiles/iCubMover.dir/main.cpp.o.requires:
.PHONY : CMakeFiles/iCubMover.dir/main.cpp.o.requires

CMakeFiles/iCubMover.dir/main.cpp.o.provides: CMakeFiles/iCubMover.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/iCubMover.dir/build.make CMakeFiles/iCubMover.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/iCubMover.dir/main.cpp.o.provides

CMakeFiles/iCubMover.dir/main.cpp.o.provides.build: CMakeFiles/iCubMover.dir/main.cpp.o

CMakeFiles/iCubMover.dir/moverPosition.cpp.o: CMakeFiles/iCubMover.dir/flags.make
CMakeFiles/iCubMover.dir/moverPosition.cpp.o: moverPosition.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/stollenga/workspace/IM-CLeVeR/mover/src/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/iCubMover.dir/moverPosition.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/iCubMover.dir/moverPosition.cpp.o -c /home/stollenga/workspace/IM-CLeVeR/mover/src/moverPosition.cpp

CMakeFiles/iCubMover.dir/moverPosition.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/iCubMover.dir/moverPosition.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/stollenga/workspace/IM-CLeVeR/mover/src/moverPosition.cpp > CMakeFiles/iCubMover.dir/moverPosition.cpp.i

CMakeFiles/iCubMover.dir/moverPosition.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/iCubMover.dir/moverPosition.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/stollenga/workspace/IM-CLeVeR/mover/src/moverPosition.cpp -o CMakeFiles/iCubMover.dir/moverPosition.cpp.s

CMakeFiles/iCubMover.dir/moverPosition.cpp.o.requires:
.PHONY : CMakeFiles/iCubMover.dir/moverPosition.cpp.o.requires

CMakeFiles/iCubMover.dir/moverPosition.cpp.o.provides: CMakeFiles/iCubMover.dir/moverPosition.cpp.o.requires
	$(MAKE) -f CMakeFiles/iCubMover.dir/build.make CMakeFiles/iCubMover.dir/moverPosition.cpp.o.provides.build
.PHONY : CMakeFiles/iCubMover.dir/moverPosition.cpp.o.provides

CMakeFiles/iCubMover.dir/moverPosition.cpp.o.provides.build: CMakeFiles/iCubMover.dir/moverPosition.cpp.o

# Object files for target iCubMover
iCubMover_OBJECTS = \
"CMakeFiles/iCubMover.dir/main.cpp.o" \
"CMakeFiles/iCubMover.dir/moverPosition.cpp.o"

# External object files for target iCubMover
iCubMover_EXTERNAL_OBJECTS =

iCubMover: CMakeFiles/iCubMover.dir/main.cpp.o
iCubMover: CMakeFiles/iCubMover.dir/moverPosition.cpp.o
iCubMover: CMakeFiles/iCubMover.dir/build.make
iCubMover: /usr/local/yarp2/build/lib/libYARP_OS.a
iCubMover: /usr/local/yarp2/build/lib/libYARP_sig.a
iCubMover: /usr/local/yarp2/build/lib/libYARP_math.a
iCubMover: /usr/local/yarp2/build/lib/libYARP_dev.a
iCubMover: /usr/local/yarp2/build/lib/libYARP_name.a
iCubMover: /usr/local/yarp2/build/lib/libYARP_init.a
iCubMover: /usr/local/icub/main/build/lib/libicubmod.a
iCubMover: /usr/local/yarp2/build/lib/libYARP_init.a
iCubMover: /usr/local/yarp2/build/lib/libYARP_math.a
iCubMover: /usr/lib/libgsl.so
iCubMover: /usr/lib/libgslcblas.so
iCubMover: /usr/local/yarp2/build/lib/libYARP_dev.a
iCubMover: /usr/local/yarp2/build/lib/libYARP_sig.a
iCubMover: /usr/local/yarp2/build/lib/libYARP_name.a
iCubMover: /usr/local/yarp2/build/lib/libYARP_OS.a
iCubMover: CMakeFiles/iCubMover.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable iCubMover"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/iCubMover.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/iCubMover.dir/build: iCubMover
.PHONY : CMakeFiles/iCubMover.dir/build

CMakeFiles/iCubMover.dir/requires: CMakeFiles/iCubMover.dir/main.cpp.o.requires
CMakeFiles/iCubMover.dir/requires: CMakeFiles/iCubMover.dir/moverPosition.cpp.o.requires
.PHONY : CMakeFiles/iCubMover.dir/requires

CMakeFiles/iCubMover.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/iCubMover.dir/cmake_clean.cmake
.PHONY : CMakeFiles/iCubMover.dir/clean

CMakeFiles/iCubMover.dir/depend:
	cd /home/stollenga/workspace/IM-CLeVeR/mover/src && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/stollenga/workspace/IM-CLeVeR/mover/src /home/stollenga/workspace/IM-CLeVeR/mover/src /home/stollenga/workspace/IM-CLeVeR/mover/src /home/stollenga/workspace/IM-CLeVeR/mover/src /home/stollenga/workspace/IM-CLeVeR/mover/src/CMakeFiles/iCubMover.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/iCubMover.dir/depend

