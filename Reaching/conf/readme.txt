
you have to either copy the folder or do 
ln -s $ICUB_ROOT/app/simCartesianControl/vSkinConf simCartesianControl


TODO
context changes
how was it with the robot?


Test Case
---------
start oder:
yarp server
iCub_SIM
Virtual Skin/ReflexDemo
simCartesianControl --context simCartesianControl/vSkinConf --robot icubSimF
iKinCartesianSolver --context simCartesianControl/vSkinConf --part right_arm
Reaching/ReachingDemo
