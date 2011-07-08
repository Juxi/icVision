
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
 VirtualSkin/ReflexDemo:
  ./ReflexDemo --visualize --robot ../../../xml/icub.xml --world ../../../xml/world.xml
 (maybe without world?!if you get: "Problems with obtaining the number of controlled axes")

 on the pc 104: 
  simCartesianControl --context simCartesianControl/vSkinConfRealRobot --robot icubF

 back on the server:
  iKinCartesianSolver --part left_arm --context simCartesianControl/vSkinConfRealRobot

 ./ReachingDemo --robot icubF --part left_arm
