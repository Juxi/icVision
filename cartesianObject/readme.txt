How to start the reactive grasper?
==================================

In a shell
cd ~/IM-CLeVeR/iCubGrasp/build
./iCubGrasp --robot icub --part left_arm --attenuation 4.0 --acceleration 5.0

in anoter one
cd ~/IM-CLeVeR/cartesianObject
./reactive /icub/skin/lefthand /myportname

put somtheing in the hand and touch a sensor with a finger....

How to test the grasper?
========================
In a shell
cd ~/IM-CLeVeR/iCubGrasp/build
./iCubGrasp --robot icub --part left_arm --attenuation 4.0 --acceleration 5.0

in another shell
yarp rpc /graspController
call the commands 
pre
cls
opn

wait after each command until an OK returns.


