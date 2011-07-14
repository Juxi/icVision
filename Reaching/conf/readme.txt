
you have to either copy the folder or do 
ln -s $ICUB_ROOT/app/simCartesianControl/vSkinConf simCartesianControl


TODO
context changes
how was it with the robot?


Test Case
---------
start order:
 yarp server
 iCub_SIM
 Virtual Skin/ReflexDemo
 simCartesianControl --context simCartesianControl/vSkinConf --robot icubSimF
 iKinCartesianSolver --context simCartesianControl/vSkinConf --part right_arm
 Reaching/ReachingDemo




for the robot:
-----------------
start order:
 yarp server (should be running)
 iCubInterface 
	1) ssh pc104 
	2) iCubInterface

 VirtualSkin/ReflexDemo:
  IM-CleVER/VirtualSkin/build/src/ReflexDemo/ReflexDemo --visualize --robot ../../../xml/icub.xml --world ../../../xml/world.xml
 (maybe without world?!if you get: "Problems with obtaining the number of controlled axes")


	here you can then run Davide's code to do some stuff ... (object localization)

 for the reaching you still need the iCartesianCtrl ..==>
 on the pc 104: 
  simCartesianControl --context simCartesianControl/vSkinConfRealRobot --robot icubF

 back on the server:
  iKinCartesianSolver --part left_arm --context simCartesianControl/vSkinConfRealRobot

<<<<<<< HEAD
and finally:
 ./ReachingDemo --robot icubF --part left_arm



ReachingDemo commands (on the terminal) are then:
set policy above
	- grab from above

set policy left
	- grab from the side (default)

set offset 0.2 
	- define the pre-reach offset to 20cm (default)

set tgt name
	- to define the target object in the world:
	  (default is the last object in the world, the last that appears when typing ls)

start
	- to start a reach:
stop
	- to stop the reach:
quit
	- to quit the module:






for the robot:
-----------------
start order:
 yarp server (should be running)
 iCubInterface 
 VirtualSkin/ReflexDemo:
  ./ReflexDemo --visualize --robot ../../../xml/icub.xml --world ../../../xml/world.xml
 (maybe without world?!if you get: "Problems with obtaining the number of controlled axes")

 on the pc 104: 
  simCartesianControl --context simCartesianControl/vSkinConfRealRobot --robot icubF

 back on the server:
  iKinCartesianSolver --part left_arm --context simCartesianControl/vSkinConfRealRobot

 ./ReachingDemo --robot icubF --part left_arm


















not using the Virtual Skin
---------
	icubInterface needs to then start iCartesianInterface, enable it within:
		pc104: /usr/local/src/robot/iCub/app/iCubLugano01/conf/iCubInterface.ini
		in this file there should be 2 cartXXXArm ini calls like this:
			 cartLeftArm cartesianLeftArm.ini
			 cartRightArm cartesianRightArm.ini

		on the server bring up the iKin (ipopt solver):
			cd $ICUB_ROOT/app/cartesianSolver/scripts/
			manager.py cartesianSolver.xml
		or with		iKinCartesianSolver --part left_arm




=======
 IM-CleVER/Reaching/build/src/ReachingDemo/ReachingDemo --robot icubF --part left_arm
>>>>>>> caa32e50f2737dc39e73c26175690a9a1f5f5dea
