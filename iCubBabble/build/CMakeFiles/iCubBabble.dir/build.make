# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
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
CMAKE_COMMAND = /opt/local/bin/cmake

# The command to remove a file.
RM = /opt/local/bin/cmake -E remove -f

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /opt/local/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/kail/imClever/dev/iCubBabble

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/kail/imClever/dev/iCubBabble/build

# Include any dependencies generated for this target.
include CMakeFiles/iCubBabble.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/iCubBabble.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/iCubBabble.dir/flags.make

CMakeFiles/iCubBabble.dir/main.cpp.o: CMakeFiles/iCubBabble.dir/flags.make
CMakeFiles/iCubBabble.dir/main.cpp.o: ../main.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /Users/kail/imClever/dev/iCubBabble/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/iCubBabble.dir/main.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/iCubBabble.dir/main.cpp.o -c /Users/kail/imClever/dev/iCubBabble/main.cpp

CMakeFiles/iCubBabble.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/iCubBabble.dir/main.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /Users/kail/imClever/dev/iCubBabble/main.cpp > CMakeFiles/iCubBabble.dir/main.cpp.i

CMakeFiles/iCubBabble.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/iCubBabble.dir/main.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /Users/kail/imClever/dev/iCubBabble/main.cpp -o CMakeFiles/iCubBabble.dir/main.cpp.s

CMakeFiles/iCubBabble.dir/main.cpp.o.requires:
.PHONY : CMakeFiles/iCubBabble.dir/main.cpp.o.requires

CMakeFiles/iCubBabble.dir/main.cpp.o.provides: CMakeFiles/iCubBabble.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/iCubBabble.dir/build.make CMakeFiles/iCubBabble.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/iCubBabble.dir/main.cpp.o.provides

CMakeFiles/iCubBabble.dir/main.cpp.o.provides.build: CMakeFiles/iCubBabble.dir/main.cpp.o
.PHONY : CMakeFiles/iCubBabble.dir/main.cpp.o.provides.build

CMakeFiles/iCubBabble.dir/partController.cpp.o: CMakeFiles/iCubBabble.dir/flags.make
CMakeFiles/iCubBabble.dir/partController.cpp.o: ../partController.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /Users/kail/imClever/dev/iCubBabble/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/iCubBabble.dir/partController.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/iCubBabble.dir/partController.cpp.o -c /Users/kail/imClever/dev/iCubBabble/partController.cpp

CMakeFiles/iCubBabble.dir/partController.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/iCubBabble.dir/partController.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /Users/kail/imClever/dev/iCubBabble/partController.cpp > CMakeFiles/iCubBabble.dir/partController.cpp.i

CMakeFiles/iCubBabble.dir/partController.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/iCubBabble.dir/partController.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /Users/kail/imClever/dev/iCubBabble/partController.cpp -o CMakeFiles/iCubBabble.dir/partController.cpp.s

CMakeFiles/iCubBabble.dir/partController.cpp.o.requires:
.PHONY : CMakeFiles/iCubBabble.dir/partController.cpp.o.requires

CMakeFiles/iCubBabble.dir/partController.cpp.o.provides: CMakeFiles/iCubBabble.dir/partController.cpp.o.requires
	$(MAKE) -f CMakeFiles/iCubBabble.dir/build.make CMakeFiles/iCubBabble.dir/partController.cpp.o.provides.build
.PHONY : CMakeFiles/iCubBabble.dir/partController.cpp.o.provides

CMakeFiles/iCubBabble.dir/partController.cpp.o.provides.build: CMakeFiles/iCubBabble.dir/partController.cpp.o
.PHONY : CMakeFiles/iCubBabble.dir/partController.cpp.o.provides.build

CMakeFiles/iCubBabble.dir/partBabbler.cpp.o: CMakeFiles/iCubBabble.dir/flags.make
CMakeFiles/iCubBabble.dir/partBabbler.cpp.o: ../partBabbler.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /Users/kail/imClever/dev/iCubBabble/build/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/iCubBabble.dir/partBabbler.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/iCubBabble.dir/partBabbler.cpp.o -c /Users/kail/imClever/dev/iCubBabble/partBabbler.cpp

CMakeFiles/iCubBabble.dir/partBabbler.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/iCubBabble.dir/partBabbler.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /Users/kail/imClever/dev/iCubBabble/partBabbler.cpp > CMakeFiles/iCubBabble.dir/partBabbler.cpp.i

CMakeFiles/iCubBabble.dir/partBabbler.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/iCubBabble.dir/partBabbler.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /Users/kail/imClever/dev/iCubBabble/partBabbler.cpp -o CMakeFiles/iCubBabble.dir/partBabbler.cpp.s

CMakeFiles/iCubBabble.dir/partBabbler.cpp.o.requires:
.PHONY : CMakeFiles/iCubBabble.dir/partBabbler.cpp.o.requires

CMakeFiles/iCubBabble.dir/partBabbler.cpp.o.provides: CMakeFiles/iCubBabble.dir/partBabbler.cpp.o.requires
	$(MAKE) -f CMakeFiles/iCubBabble.dir/build.make CMakeFiles/iCubBabble.dir/partBabbler.cpp.o.provides.build
.PHONY : CMakeFiles/iCubBabble.dir/partBabbler.cpp.o.provides

CMakeFiles/iCubBabble.dir/partBabbler.cpp.o.provides.build: CMakeFiles/iCubBabble.dir/partBabbler.cpp.o
.PHONY : CMakeFiles/iCubBabble.dir/partBabbler.cpp.o.provides.build

CMakeFiles/iCubBabble.dir/iCubBabbler.cpp.o: CMakeFiles/iCubBabble.dir/flags.make
CMakeFiles/iCubBabble.dir/iCubBabbler.cpp.o: ../iCubBabbler.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /Users/kail/imClever/dev/iCubBabble/build/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/iCubBabble.dir/iCubBabbler.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/iCubBabble.dir/iCubBabbler.cpp.o -c /Users/kail/imClever/dev/iCubBabble/iCubBabbler.cpp

CMakeFiles/iCubBabble.dir/iCubBabbler.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/iCubBabble.dir/iCubBabbler.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /Users/kail/imClever/dev/iCubBabble/iCubBabbler.cpp > CMakeFiles/iCubBabble.dir/iCubBabbler.cpp.i

CMakeFiles/iCubBabble.dir/iCubBabbler.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/iCubBabble.dir/iCubBabbler.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /Users/kail/imClever/dev/iCubBabble/iCubBabbler.cpp -o CMakeFiles/iCubBabble.dir/iCubBabbler.cpp.s

CMakeFiles/iCubBabble.dir/iCubBabbler.cpp.o.requires:
.PHONY : CMakeFiles/iCubBabble.dir/iCubBabbler.cpp.o.requires

CMakeFiles/iCubBabble.dir/iCubBabbler.cpp.o.provides: CMakeFiles/iCubBabble.dir/iCubBabbler.cpp.o.requires
	$(MAKE) -f CMakeFiles/iCubBabble.dir/build.make CMakeFiles/iCubBabble.dir/iCubBabbler.cpp.o.provides.build
.PHONY : CMakeFiles/iCubBabble.dir/iCubBabbler.cpp.o.provides

CMakeFiles/iCubBabble.dir/iCubBabbler.cpp.o.provides.build: CMakeFiles/iCubBabble.dir/iCubBabbler.cpp.o
.PHONY : CMakeFiles/iCubBabble.dir/iCubBabbler.cpp.o.provides.build

CMakeFiles/iCubBabble.dir/moc_iCubBabbler.cxx.o: CMakeFiles/iCubBabble.dir/flags.make
CMakeFiles/iCubBabble.dir/moc_iCubBabbler.cxx.o: moc_iCubBabbler.cxx
	$(CMAKE_COMMAND) -E cmake_progress_report /Users/kail/imClever/dev/iCubBabble/build/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/iCubBabble.dir/moc_iCubBabbler.cxx.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/iCubBabble.dir/moc_iCubBabbler.cxx.o -c /Users/kail/imClever/dev/iCubBabble/build/moc_iCubBabbler.cxx

CMakeFiles/iCubBabble.dir/moc_iCubBabbler.cxx.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/iCubBabble.dir/moc_iCubBabbler.cxx.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /Users/kail/imClever/dev/iCubBabble/build/moc_iCubBabbler.cxx > CMakeFiles/iCubBabble.dir/moc_iCubBabbler.cxx.i

CMakeFiles/iCubBabble.dir/moc_iCubBabbler.cxx.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/iCubBabble.dir/moc_iCubBabbler.cxx.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /Users/kail/imClever/dev/iCubBabble/build/moc_iCubBabbler.cxx -o CMakeFiles/iCubBabble.dir/moc_iCubBabbler.cxx.s

CMakeFiles/iCubBabble.dir/moc_iCubBabbler.cxx.o.requires:
.PHONY : CMakeFiles/iCubBabble.dir/moc_iCubBabbler.cxx.o.requires

CMakeFiles/iCubBabble.dir/moc_iCubBabbler.cxx.o.provides: CMakeFiles/iCubBabble.dir/moc_iCubBabbler.cxx.o.requires
	$(MAKE) -f CMakeFiles/iCubBabble.dir/build.make CMakeFiles/iCubBabble.dir/moc_iCubBabbler.cxx.o.provides.build
.PHONY : CMakeFiles/iCubBabble.dir/moc_iCubBabbler.cxx.o.provides

CMakeFiles/iCubBabble.dir/moc_iCubBabbler.cxx.o.provides.build: CMakeFiles/iCubBabble.dir/moc_iCubBabbler.cxx.o
.PHONY : CMakeFiles/iCubBabble.dir/moc_iCubBabbler.cxx.o.provides.build

moc_iCubBabbler.cxx: ../iCubBabbler.h
	$(CMAKE_COMMAND) -E cmake_progress_report /Users/kail/imClever/dev/iCubBabble/build/CMakeFiles $(CMAKE_PROGRESS_6)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold "Generating moc_iCubBabbler.cxx"
	/Developer/Tools/Qt/moc -I/usr/include -F/Library/Frameworks -I/Library/Frameworks/QtGui.framework/Headers -I/Library/Frameworks/QtCore.framework/Headers -I/Users/kail/Library/yarp-2.3.4/generated_include -I/Users/kail/Library/yarp-2.3.4/src/libYARP_OS/include -I/Users/kail/Library/yarp-2.3.4/src/libYARP_sig/include -I/Users/kail/Library/yarp-2.3.4/src/libYARP_math/include -I/Users/kail/Library/yarp-2.3.4/src/libYARP_dev/include -DQT_GUI_LIB -DQT_CORE_LIB -o /Users/kail/imClever/dev/iCubBabble/build/moc_iCubBabbler.cxx /Users/kail/imClever/dev/iCubBabble/iCubBabbler.h

# Object files for target iCubBabble
iCubBabble_OBJECTS = \
"CMakeFiles/iCubBabble.dir/main.cpp.o" \
"CMakeFiles/iCubBabble.dir/partController.cpp.o" \
"CMakeFiles/iCubBabble.dir/partBabbler.cpp.o" \
"CMakeFiles/iCubBabble.dir/iCubBabbler.cpp.o" \
"CMakeFiles/iCubBabble.dir/moc_iCubBabbler.cxx.o"

# External object files for target iCubBabble
iCubBabble_EXTERNAL_OBJECTS =

iCubBabble: CMakeFiles/iCubBabble.dir/main.cpp.o
iCubBabble: CMakeFiles/iCubBabble.dir/partController.cpp.o
iCubBabble: CMakeFiles/iCubBabble.dir/partBabbler.cpp.o
iCubBabble: CMakeFiles/iCubBabble.dir/iCubBabbler.cpp.o
iCubBabble: CMakeFiles/iCubBabble.dir/moc_iCubBabbler.cxx.o
iCubBabble: /Users/kail/Library/yarp-2.3.4/lib/libYARP_OS.a
iCubBabble: /Users/kail/Library/yarp-2.3.4/lib/libYARP_sig.a
iCubBabble: /Users/kail/Library/yarp-2.3.4/lib/libYARP_math.a
iCubBabble: /Users/kail/Library/yarp-2.3.4/lib/libYARP_dev.a
iCubBabble: /Users/kail/Library/yarp-2.3.4/lib/libYARP_init.a
iCubBabble: /Users/kail/Library/yarp-2.3.4/lib/libYARP_math.a
iCubBabble: /opt/local/lib/libgsl.dylib
iCubBabble: /opt/local/lib/libgslcblas.dylib
iCubBabble: /Users/kail/Library/yarp-2.3.4/lib/libYARP_dev.a
iCubBabble: /Users/kail/Library/yarp-2.3.4/lib/libYARP_sig.a
iCubBabble: /Users/kail/Library/yarp-2.3.4/lib/libYARP_OS.a
iCubBabble: /usr/local/lib/libACE.dylib
iCubBabble: CMakeFiles/iCubBabble.dir/build.make
iCubBabble: CMakeFiles/iCubBabble.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable iCubBabble"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/iCubBabble.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/iCubBabble.dir/build: iCubBabble
.PHONY : CMakeFiles/iCubBabble.dir/build

CMakeFiles/iCubBabble.dir/requires: CMakeFiles/iCubBabble.dir/main.cpp.o.requires
CMakeFiles/iCubBabble.dir/requires: CMakeFiles/iCubBabble.dir/partController.cpp.o.requires
CMakeFiles/iCubBabble.dir/requires: CMakeFiles/iCubBabble.dir/partBabbler.cpp.o.requires
CMakeFiles/iCubBabble.dir/requires: CMakeFiles/iCubBabble.dir/iCubBabbler.cpp.o.requires
CMakeFiles/iCubBabble.dir/requires: CMakeFiles/iCubBabble.dir/moc_iCubBabbler.cxx.o.requires
.PHONY : CMakeFiles/iCubBabble.dir/requires

CMakeFiles/iCubBabble.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/iCubBabble.dir/cmake_clean.cmake
.PHONY : CMakeFiles/iCubBabble.dir/clean

CMakeFiles/iCubBabble.dir/depend: moc_iCubBabbler.cxx
	cd /Users/kail/imClever/dev/iCubBabble/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/kail/imClever/dev/iCubBabble /Users/kail/imClever/dev/iCubBabble /Users/kail/imClever/dev/iCubBabble/build /Users/kail/imClever/dev/iCubBabble/build /Users/kail/imClever/dev/iCubBabble/build/CMakeFiles/iCubBabble.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/iCubBabble.dir/depend

