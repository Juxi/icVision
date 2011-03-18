 
   |======================|
   |======================|
   | Welcome to PyiCubGUI |
   |======================|
   |======================|


Author: Varun Raj Kompella (PhD Student, IDSIA)

=======================================================================================================

PyiCubGUI is a graphical user interface written using python. You'll find the following files 
in the directory:

iCub_init.py		-> This file has the yarp port intializations and other basic routines.
iCub_GUI.py		-> The main file that needs to be executed. Check for the settings inside
			   the file to switch on/off a few GUI's. 
World_GUI.py		-> This file is a GUI to control the objects in the simulator.
Robot_GUI.py		-> This file is a GUI to control the real/simulated robot. Much like the usual Motor				   GUI.
Program_GUI.py		-> This file is a User Program GUI to control movement of either objects in the world 				   or robot parts. 
iCub_home_vals		-> This is file that contains the "safe" values for the motors of the robot.
init			-> This is an example workspace for the robot. (To be loaded in the Robot_GUI)


=======================================================================================================


Prerequisites
--------------
1) Swig: Intall the package from the internet.

2) Python-dev: python developer package which contains header files

3) Other basic python libs: scipy, numpy etc.

4) Yarp python module : Do a " ccmake . " in the yarp2/example/swig/ directory. Enable the python option and then press 'c' twice followed by a 'g'. Then do a make install. 

5) MDP: Modular Digital Processing toolkit. Download it free from the internet

6) Py-QT4 : Python bindings for QT 4.0 

7) python-opencv: Python bindings for opencv


=========================================================================================================

Starting the code:
------------------

Simulation Mode:

1) Run yarpserver and iCub_SIM in a terminal.

2) Set the robot_name variable in iCub_init.py to your icub's root yarp port. <usually "/icubSim/">

3) Make sure you set the iCub_parts_activation.ini file path in iCub_GUI.py file

4) run the code on command prompt by typing: $python iCub_GUI.py



Real-Robot Mode:

1) Get the iCub robot up and running.

2) Set the robot_name variable in iCub_init.py to your icub's root yarp port. <usually "/icub/">

3) Set the SimMode parameter to False in iCub_GUI.py

4) run the code on command prompt by typing: $python iCub_GUI.py


=========================================================================================================

Any comments, suggestions or problems:

Write to me at varun@idsia.ch


Enjoy working with python!








