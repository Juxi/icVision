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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/release

# Include any dependencies generated for this target.
include CMakeFiles/IDSIAObjectLocalization.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/IDSIAObjectLocalization.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/IDSIAObjectLocalization.dir/flags.make

CMakeFiles/IDSIAObjectLocalization.dir/CVUtils.cpp.o: CMakeFiles/IDSIAObjectLocalization.dir/flags.make
CMakeFiles/IDSIAObjectLocalization.dir/CVUtils.cpp.o: ../CVUtils.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/release/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/IDSIAObjectLocalization.dir/CVUtils.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/IDSIAObjectLocalization.dir/CVUtils.cpp.o -c /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/CVUtils.cpp

CMakeFiles/IDSIAObjectLocalization.dir/CVUtils.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/IDSIAObjectLocalization.dir/CVUtils.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/CVUtils.cpp > CMakeFiles/IDSIAObjectLocalization.dir/CVUtils.cpp.i

CMakeFiles/IDSIAObjectLocalization.dir/CVUtils.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/IDSIAObjectLocalization.dir/CVUtils.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/CVUtils.cpp -o CMakeFiles/IDSIAObjectLocalization.dir/CVUtils.cpp.s

CMakeFiles/IDSIAObjectLocalization.dir/CVUtils.cpp.o.requires:
.PHONY : CMakeFiles/IDSIAObjectLocalization.dir/CVUtils.cpp.o.requires

CMakeFiles/IDSIAObjectLocalization.dir/CVUtils.cpp.o.provides: CMakeFiles/IDSIAObjectLocalization.dir/CVUtils.cpp.o.requires
	$(MAKE) -f CMakeFiles/IDSIAObjectLocalization.dir/build.make CMakeFiles/IDSIAObjectLocalization.dir/CVUtils.cpp.o.provides.build
.PHONY : CMakeFiles/IDSIAObjectLocalization.dir/CVUtils.cpp.o.provides

CMakeFiles/IDSIAObjectLocalization.dir/CVUtils.cpp.o.provides.build: CMakeFiles/IDSIAObjectLocalization.dir/CVUtils.cpp.o
.PHONY : CMakeFiles/IDSIAObjectLocalization.dir/CVUtils.cpp.o.provides.build

CMakeFiles/IDSIAObjectLocalization.dir/WorldObject.cpp.o: CMakeFiles/IDSIAObjectLocalization.dir/flags.make
CMakeFiles/IDSIAObjectLocalization.dir/WorldObject.cpp.o: ../WorldObject.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/release/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/IDSIAObjectLocalization.dir/WorldObject.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/IDSIAObjectLocalization.dir/WorldObject.cpp.o -c /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/WorldObject.cpp

CMakeFiles/IDSIAObjectLocalization.dir/WorldObject.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/IDSIAObjectLocalization.dir/WorldObject.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/WorldObject.cpp > CMakeFiles/IDSIAObjectLocalization.dir/WorldObject.cpp.i

CMakeFiles/IDSIAObjectLocalization.dir/WorldObject.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/IDSIAObjectLocalization.dir/WorldObject.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/WorldObject.cpp -o CMakeFiles/IDSIAObjectLocalization.dir/WorldObject.cpp.s

CMakeFiles/IDSIAObjectLocalization.dir/WorldObject.cpp.o.requires:
.PHONY : CMakeFiles/IDSIAObjectLocalization.dir/WorldObject.cpp.o.requires

CMakeFiles/IDSIAObjectLocalization.dir/WorldObject.cpp.o.provides: CMakeFiles/IDSIAObjectLocalization.dir/WorldObject.cpp.o.requires
	$(MAKE) -f CMakeFiles/IDSIAObjectLocalization.dir/build.make CMakeFiles/IDSIAObjectLocalization.dir/WorldObject.cpp.o.provides.build
.PHONY : CMakeFiles/IDSIAObjectLocalization.dir/WorldObject.cpp.o.provides

CMakeFiles/IDSIAObjectLocalization.dir/WorldObject.cpp.o.provides.build: CMakeFiles/IDSIAObjectLocalization.dir/WorldObject.cpp.o
.PHONY : CMakeFiles/IDSIAObjectLocalization.dir/WorldObject.cpp.o.provides.build

CMakeFiles/IDSIAObjectLocalization.dir/main.cpp.o: CMakeFiles/IDSIAObjectLocalization.dir/flags.make
CMakeFiles/IDSIAObjectLocalization.dir/main.cpp.o: ../main.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/release/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/IDSIAObjectLocalization.dir/main.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/IDSIAObjectLocalization.dir/main.cpp.o -c /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/main.cpp

CMakeFiles/IDSIAObjectLocalization.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/IDSIAObjectLocalization.dir/main.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/main.cpp > CMakeFiles/IDSIAObjectLocalization.dir/main.cpp.i

CMakeFiles/IDSIAObjectLocalization.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/IDSIAObjectLocalization.dir/main.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/main.cpp -o CMakeFiles/IDSIAObjectLocalization.dir/main.cpp.s

CMakeFiles/IDSIAObjectLocalization.dir/main.cpp.o.requires:
.PHONY : CMakeFiles/IDSIAObjectLocalization.dir/main.cpp.o.requires

CMakeFiles/IDSIAObjectLocalization.dir/main.cpp.o.provides: CMakeFiles/IDSIAObjectLocalization.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/IDSIAObjectLocalization.dir/build.make CMakeFiles/IDSIAObjectLocalization.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/IDSIAObjectLocalization.dir/main.cpp.o.provides

CMakeFiles/IDSIAObjectLocalization.dir/main.cpp.o.provides.build: CMakeFiles/IDSIAObjectLocalization.dir/main.cpp.o
.PHONY : CMakeFiles/IDSIAObjectLocalization.dir/main.cpp.o.provides.build

CMakeFiles/IDSIAObjectLocalization.dir/ColoredObjectDetector.cpp.o: CMakeFiles/IDSIAObjectLocalization.dir/flags.make
CMakeFiles/IDSIAObjectLocalization.dir/ColoredObjectDetector.cpp.o: ../ColoredObjectDetector.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/release/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/IDSIAObjectLocalization.dir/ColoredObjectDetector.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/IDSIAObjectLocalization.dir/ColoredObjectDetector.cpp.o -c /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/ColoredObjectDetector.cpp

CMakeFiles/IDSIAObjectLocalization.dir/ColoredObjectDetector.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/IDSIAObjectLocalization.dir/ColoredObjectDetector.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/ColoredObjectDetector.cpp > CMakeFiles/IDSIAObjectLocalization.dir/ColoredObjectDetector.cpp.i

CMakeFiles/IDSIAObjectLocalization.dir/ColoredObjectDetector.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/IDSIAObjectLocalization.dir/ColoredObjectDetector.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/ColoredObjectDetector.cpp -o CMakeFiles/IDSIAObjectLocalization.dir/ColoredObjectDetector.cpp.s

CMakeFiles/IDSIAObjectLocalization.dir/ColoredObjectDetector.cpp.o.requires:
.PHONY : CMakeFiles/IDSIAObjectLocalization.dir/ColoredObjectDetector.cpp.o.requires

CMakeFiles/IDSIAObjectLocalization.dir/ColoredObjectDetector.cpp.o.provides: CMakeFiles/IDSIAObjectLocalization.dir/ColoredObjectDetector.cpp.o.requires
	$(MAKE) -f CMakeFiles/IDSIAObjectLocalization.dir/build.make CMakeFiles/IDSIAObjectLocalization.dir/ColoredObjectDetector.cpp.o.provides.build
.PHONY : CMakeFiles/IDSIAObjectLocalization.dir/ColoredObjectDetector.cpp.o.provides

CMakeFiles/IDSIAObjectLocalization.dir/ColoredObjectDetector.cpp.o.provides.build: CMakeFiles/IDSIAObjectLocalization.dir/ColoredObjectDetector.cpp.o
.PHONY : CMakeFiles/IDSIAObjectLocalization.dir/ColoredObjectDetector.cpp.o.provides.build

CMakeFiles/IDSIAObjectLocalization.dir/GeoUtils.cpp.o: CMakeFiles/IDSIAObjectLocalization.dir/flags.make
CMakeFiles/IDSIAObjectLocalization.dir/GeoUtils.cpp.o: ../GeoUtils.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/release/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/IDSIAObjectLocalization.dir/GeoUtils.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/IDSIAObjectLocalization.dir/GeoUtils.cpp.o -c /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/GeoUtils.cpp

CMakeFiles/IDSIAObjectLocalization.dir/GeoUtils.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/IDSIAObjectLocalization.dir/GeoUtils.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/GeoUtils.cpp > CMakeFiles/IDSIAObjectLocalization.dir/GeoUtils.cpp.i

CMakeFiles/IDSIAObjectLocalization.dir/GeoUtils.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/IDSIAObjectLocalization.dir/GeoUtils.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/GeoUtils.cpp -o CMakeFiles/IDSIAObjectLocalization.dir/GeoUtils.cpp.s

CMakeFiles/IDSIAObjectLocalization.dir/GeoUtils.cpp.o.requires:
.PHONY : CMakeFiles/IDSIAObjectLocalization.dir/GeoUtils.cpp.o.requires

CMakeFiles/IDSIAObjectLocalization.dir/GeoUtils.cpp.o.provides: CMakeFiles/IDSIAObjectLocalization.dir/GeoUtils.cpp.o.requires
	$(MAKE) -f CMakeFiles/IDSIAObjectLocalization.dir/build.make CMakeFiles/IDSIAObjectLocalization.dir/GeoUtils.cpp.o.provides.build
.PHONY : CMakeFiles/IDSIAObjectLocalization.dir/GeoUtils.cpp.o.provides

CMakeFiles/IDSIAObjectLocalization.dir/GeoUtils.cpp.o.provides.build: CMakeFiles/IDSIAObjectLocalization.dir/GeoUtils.cpp.o
.PHONY : CMakeFiles/IDSIAObjectLocalization.dir/GeoUtils.cpp.o.provides.build

CMakeFiles/IDSIAObjectLocalization.dir/ObjectDetector.cpp.o: CMakeFiles/IDSIAObjectLocalization.dir/flags.make
CMakeFiles/IDSIAObjectLocalization.dir/ObjectDetector.cpp.o: ../ObjectDetector.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/release/CMakeFiles $(CMAKE_PROGRESS_6)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/IDSIAObjectLocalization.dir/ObjectDetector.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/IDSIAObjectLocalization.dir/ObjectDetector.cpp.o -c /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/ObjectDetector.cpp

CMakeFiles/IDSIAObjectLocalization.dir/ObjectDetector.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/IDSIAObjectLocalization.dir/ObjectDetector.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/ObjectDetector.cpp > CMakeFiles/IDSIAObjectLocalization.dir/ObjectDetector.cpp.i

CMakeFiles/IDSIAObjectLocalization.dir/ObjectDetector.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/IDSIAObjectLocalization.dir/ObjectDetector.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/ObjectDetector.cpp -o CMakeFiles/IDSIAObjectLocalization.dir/ObjectDetector.cpp.s

CMakeFiles/IDSIAObjectLocalization.dir/ObjectDetector.cpp.o.requires:
.PHONY : CMakeFiles/IDSIAObjectLocalization.dir/ObjectDetector.cpp.o.requires

CMakeFiles/IDSIAObjectLocalization.dir/ObjectDetector.cpp.o.provides: CMakeFiles/IDSIAObjectLocalization.dir/ObjectDetector.cpp.o.requires
	$(MAKE) -f CMakeFiles/IDSIAObjectLocalization.dir/build.make CMakeFiles/IDSIAObjectLocalization.dir/ObjectDetector.cpp.o.provides.build
.PHONY : CMakeFiles/IDSIAObjectLocalization.dir/ObjectDetector.cpp.o.provides

CMakeFiles/IDSIAObjectLocalization.dir/ObjectDetector.cpp.o.provides.build: CMakeFiles/IDSIAObjectLocalization.dir/ObjectDetector.cpp.o
.PHONY : CMakeFiles/IDSIAObjectLocalization.dir/ObjectDetector.cpp.o.provides.build

CMakeFiles/IDSIAObjectLocalization.dir/ColoredRect.cpp.o: CMakeFiles/IDSIAObjectLocalization.dir/flags.make
CMakeFiles/IDSIAObjectLocalization.dir/ColoredRect.cpp.o: ../ColoredRect.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/release/CMakeFiles $(CMAKE_PROGRESS_7)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/IDSIAObjectLocalization.dir/ColoredRect.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/IDSIAObjectLocalization.dir/ColoredRect.cpp.o -c /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/ColoredRect.cpp

CMakeFiles/IDSIAObjectLocalization.dir/ColoredRect.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/IDSIAObjectLocalization.dir/ColoredRect.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/ColoredRect.cpp > CMakeFiles/IDSIAObjectLocalization.dir/ColoredRect.cpp.i

CMakeFiles/IDSIAObjectLocalization.dir/ColoredRect.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/IDSIAObjectLocalization.dir/ColoredRect.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/ColoredRect.cpp -o CMakeFiles/IDSIAObjectLocalization.dir/ColoredRect.cpp.s

CMakeFiles/IDSIAObjectLocalization.dir/ColoredRect.cpp.o.requires:
.PHONY : CMakeFiles/IDSIAObjectLocalization.dir/ColoredRect.cpp.o.requires

CMakeFiles/IDSIAObjectLocalization.dir/ColoredRect.cpp.o.provides: CMakeFiles/IDSIAObjectLocalization.dir/ColoredRect.cpp.o.requires
	$(MAKE) -f CMakeFiles/IDSIAObjectLocalization.dir/build.make CMakeFiles/IDSIAObjectLocalization.dir/ColoredRect.cpp.o.provides.build
.PHONY : CMakeFiles/IDSIAObjectLocalization.dir/ColoredRect.cpp.o.provides

CMakeFiles/IDSIAObjectLocalization.dir/ColoredRect.cpp.o.provides.build: CMakeFiles/IDSIAObjectLocalization.dir/ColoredRect.cpp.o
.PHONY : CMakeFiles/IDSIAObjectLocalization.dir/ColoredRect.cpp.o.provides.build

CMakeFiles/IDSIAObjectLocalization.dir/VisionModule.cpp.o: CMakeFiles/IDSIAObjectLocalization.dir/flags.make
CMakeFiles/IDSIAObjectLocalization.dir/VisionModule.cpp.o: ../VisionModule.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/release/CMakeFiles $(CMAKE_PROGRESS_8)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/IDSIAObjectLocalization.dir/VisionModule.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/IDSIAObjectLocalization.dir/VisionModule.cpp.o -c /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/VisionModule.cpp

CMakeFiles/IDSIAObjectLocalization.dir/VisionModule.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/IDSIAObjectLocalization.dir/VisionModule.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/VisionModule.cpp > CMakeFiles/IDSIAObjectLocalization.dir/VisionModule.cpp.i

CMakeFiles/IDSIAObjectLocalization.dir/VisionModule.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/IDSIAObjectLocalization.dir/VisionModule.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/VisionModule.cpp -o CMakeFiles/IDSIAObjectLocalization.dir/VisionModule.cpp.s

CMakeFiles/IDSIAObjectLocalization.dir/VisionModule.cpp.o.requires:
.PHONY : CMakeFiles/IDSIAObjectLocalization.dir/VisionModule.cpp.o.requires

CMakeFiles/IDSIAObjectLocalization.dir/VisionModule.cpp.o.provides: CMakeFiles/IDSIAObjectLocalization.dir/VisionModule.cpp.o.requires
	$(MAKE) -f CMakeFiles/IDSIAObjectLocalization.dir/build.make CMakeFiles/IDSIAObjectLocalization.dir/VisionModule.cpp.o.provides.build
.PHONY : CMakeFiles/IDSIAObjectLocalization.dir/VisionModule.cpp.o.provides

CMakeFiles/IDSIAObjectLocalization.dir/VisionModule.cpp.o.provides.build: CMakeFiles/IDSIAObjectLocalization.dir/VisionModule.cpp.o
.PHONY : CMakeFiles/IDSIAObjectLocalization.dir/VisionModule.cpp.o.provides.build

# Object files for target IDSIAObjectLocalization
IDSIAObjectLocalization_OBJECTS = \
"CMakeFiles/IDSIAObjectLocalization.dir/CVUtils.cpp.o" \
"CMakeFiles/IDSIAObjectLocalization.dir/WorldObject.cpp.o" \
"CMakeFiles/IDSIAObjectLocalization.dir/main.cpp.o" \
"CMakeFiles/IDSIAObjectLocalization.dir/ColoredObjectDetector.cpp.o" \
"CMakeFiles/IDSIAObjectLocalization.dir/GeoUtils.cpp.o" \
"CMakeFiles/IDSIAObjectLocalization.dir/ObjectDetector.cpp.o" \
"CMakeFiles/IDSIAObjectLocalization.dir/ColoredRect.cpp.o" \
"CMakeFiles/IDSIAObjectLocalization.dir/VisionModule.cpp.o"

# External object files for target IDSIAObjectLocalization
IDSIAObjectLocalization_EXTERNAL_OBJECTS =

IDSIAObjectLocalization: CMakeFiles/IDSIAObjectLocalization.dir/CVUtils.cpp.o
IDSIAObjectLocalization: CMakeFiles/IDSIAObjectLocalization.dir/WorldObject.cpp.o
IDSIAObjectLocalization: CMakeFiles/IDSIAObjectLocalization.dir/main.cpp.o
IDSIAObjectLocalization: CMakeFiles/IDSIAObjectLocalization.dir/ColoredObjectDetector.cpp.o
IDSIAObjectLocalization: CMakeFiles/IDSIAObjectLocalization.dir/GeoUtils.cpp.o
IDSIAObjectLocalization: CMakeFiles/IDSIAObjectLocalization.dir/ObjectDetector.cpp.o
IDSIAObjectLocalization: CMakeFiles/IDSIAObjectLocalization.dir/ColoredRect.cpp.o
IDSIAObjectLocalization: CMakeFiles/IDSIAObjectLocalization.dir/VisionModule.cpp.o
IDSIAObjectLocalization: /usr/local/src/robot/yarp2/lib/libYARP_OS.so
IDSIAObjectLocalization: /usr/local/src/robot/yarp2/lib/libYARP_sig.so
IDSIAObjectLocalization: /usr/local/src/robot/yarp2/lib/libYARP_math.so
IDSIAObjectLocalization: /usr/local/src/robot/yarp2/lib/libYARP_dev.so
IDSIAObjectLocalization: /usr/local/src/robot/yarp2/lib/libyarpmod.so
IDSIAObjectLocalization: /usr/local/src/robot/yarp2/lib/libyarpcar.so
IDSIAObjectLocalization: /usr/local/src/robot/yarp2/lib/libYARP_name.so
IDSIAObjectLocalization: /usr/local/src/robot/yarp2/lib/libYARP_init.so
IDSIAObjectLocalization: /usr/lib/libQtGui.so
IDSIAObjectLocalization: /usr/lib/libQtCore.so
IDSIAObjectLocalization: /usr/local/src/robot/yarp2/lib/libYARP_OS.so
IDSIAObjectLocalization: /usr/local/src/robot/yarp2/lib/libYARP_sig.so
IDSIAObjectLocalization: /usr/local/src/robot/yarp2/lib/libYARP_math.so
IDSIAObjectLocalization: /usr/local/src/robot/yarp2/lib/libYARP_dev.so
IDSIAObjectLocalization: /usr/local/src/robot/yarp2/lib/libyarpmod.so
IDSIAObjectLocalization: /usr/local/src/robot/yarp2/lib/libyarpcar.so
IDSIAObjectLocalization: /usr/local/src/robot/yarp2/lib/libYARP_name.so
IDSIAObjectLocalization: /usr/local/src/robot/yarp2/lib/libYARP_init.so
IDSIAObjectLocalization: /usr/local/src/robot/yarp2/lib/libyarpcar.so
IDSIAObjectLocalization: /usr/local/src/robot/yarp2/lib/libyarpmod.so
IDSIAObjectLocalization: /usr/local/src/robot/yarp2/lib/libYARP_math.so
IDSIAObjectLocalization: /usr/lib/libgsl.so
IDSIAObjectLocalization: /usr/lib/libgslcblas.so
IDSIAObjectLocalization: /usr/local/src/robot/yarp2/lib/libYARP_dev.so
IDSIAObjectLocalization: /usr/local/src/robot/yarp2/lib/libYARP_sig.so
IDSIAObjectLocalization: /usr/local/src/robot/yarp2/lib/libYARP_name.so
IDSIAObjectLocalization: /usr/local/src/robot/yarp2/lib/libYARP_OS.so
IDSIAObjectLocalization: CMakeFiles/IDSIAObjectLocalization.dir/build.make
IDSIAObjectLocalization: CMakeFiles/IDSIAObjectLocalization.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable IDSIAObjectLocalization"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/IDSIAObjectLocalization.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/IDSIAObjectLocalization.dir/build: IDSIAObjectLocalization
.PHONY : CMakeFiles/IDSIAObjectLocalization.dir/build

CMakeFiles/IDSIAObjectLocalization.dir/requires: CMakeFiles/IDSIAObjectLocalization.dir/CVUtils.cpp.o.requires
CMakeFiles/IDSIAObjectLocalization.dir/requires: CMakeFiles/IDSIAObjectLocalization.dir/WorldObject.cpp.o.requires
CMakeFiles/IDSIAObjectLocalization.dir/requires: CMakeFiles/IDSIAObjectLocalization.dir/main.cpp.o.requires
CMakeFiles/IDSIAObjectLocalization.dir/requires: CMakeFiles/IDSIAObjectLocalization.dir/ColoredObjectDetector.cpp.o.requires
CMakeFiles/IDSIAObjectLocalization.dir/requires: CMakeFiles/IDSIAObjectLocalization.dir/GeoUtils.cpp.o.requires
CMakeFiles/IDSIAObjectLocalization.dir/requires: CMakeFiles/IDSIAObjectLocalization.dir/ObjectDetector.cpp.o.requires
CMakeFiles/IDSIAObjectLocalization.dir/requires: CMakeFiles/IDSIAObjectLocalization.dir/ColoredRect.cpp.o.requires
CMakeFiles/IDSIAObjectLocalization.dir/requires: CMakeFiles/IDSIAObjectLocalization.dir/VisionModule.cpp.o.requires
.PHONY : CMakeFiles/IDSIAObjectLocalization.dir/requires

CMakeFiles/IDSIAObjectLocalization.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/IDSIAObjectLocalization.dir/cmake_clean.cmake
.PHONY : CMakeFiles/IDSIAObjectLocalization.dir/clean

CMakeFiles/IDSIAObjectLocalization.dir/depend:
	cd /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/release && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/release /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/release /home/icub/IM-CLeVeR/idsiaVisionDemoModule/IDSIAObjectLocalization/release/CMakeFiles/IDSIAObjectLocalization.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/IDSIAObjectLocalization.dir/depend

