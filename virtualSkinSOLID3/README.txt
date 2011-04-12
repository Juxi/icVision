
VirtualSkin is a collection of tools intended to facilitate your research on complex robotic platforms such as humanoids. It is particularly well suited to reaching, grasping and manipulation, however some parts of the VirtualSkin distribution could certainly be applied to other research topics as well. The idea behind VirtualSkin is that having a cartesian space representation of the geometry of the robot's pose, which is light-weight and fast (provided by a forward kinematic model and attached geometries), is a useful thing to have. Such a representation, driven by streams of encoder positions from the real hardware (run online), can be used to protect the robot from damage resulting from collision with itself or with immovable objects in the environment. Moreover, it can be used offline to facilitate motion planning by searching for feasible motions much faster than real-time. Whether run online or offline, VirtualSkin reports collision information in bodypart-centric cartesian coordinates, which provides a pose-invariant representation of the location of collision events on the body.


What's In The Box

(librobotModel.a) - a static library for building and moving a kinematic robot model
(libyarpFilter.a) - a static library containing a port filter for YARP communication channels (contributed by Gregor Kaufmann - also available as part of "idsiaYarpModules")
(reflex) - an executable that integrates these two libraries. It tracks a YARP robot (currently iCub or iCubSim but you can model any robot quickly and easily in xml), reports collision events (as described above). In case of collision, it takes control away from the user, executes a reflexive response to a previous collision free pose, then returns control to the user.
(iCubBabble) - a simple control program for the iCub robot (only) that issues random position or velocity commands.


Coming Soon

A new YARP interface to move the robot model directly using position and velocity commands (a planner that functions independently from an external robot or simulated robot)
Collision events will be reported in a 2D parameterized "Skin Space" that preserves neighborhood information for adjacent body parts


Installation

1. Make sure you have all dependencies installed and working.
	CMake >= 2.6
	YARP >= 2.6
	Qt >= 4
	OpenGL
	FreeSOLID

3. From PATH_TO_YOUR_LOCAL_GIT_REPOSITORY/virtualSkin do the following:

	mkdir build
	cd build
	cmake ..			(if this yields errors that it failed to find something, run the next command is no longer optional!)
	(optional) ccmake ..		(to tweak build options i.e. to set build type to DEBUG... this may be necessary to set paths to dependencies if 'cmake ..' fails to find them)
	make
	(optional) make install		(this will install build products to	PATH_TO_YOUR_LOCAL_GIT_REPOSITORY/virtualSkin/bin
										PATH_TO_YOUR_LOCAL_GIT_REPOSITORY/virtualSkin/lib
										PATH_TO_YOUR_LOCAL_GIT_REPOSITORY/virtualSkin/include


This project was developed during the fist year of my PhD at IDSIA's robotics lab.
2011-02-15, Kail Frank - kail@idsia.ch
