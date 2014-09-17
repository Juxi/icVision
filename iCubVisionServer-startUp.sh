#!/bin/bash
#gnome-terminal --profile=hold --tab --working-directory=~ -t "YARPRun server" -e "yarprun --server /icubvision" &
#sleep 1

gnome-terminal --profile=hold --tab --working-directory=/home/icub/im-clever/icVision/build/src/core -t "icVision Core" -e "./visionCoreGUI --robot icub"

# need -x -ic to allow loading the bashrc file!! (ICUB_ROOT)
gnome-terminal --profile=Default --tab --working-directory=/home/icub/im-clever/icVision/ -t "iKinGazeCtrl" -x $SHELL -ic "iKinGazeCtrl --context iCubLugano01/cameraCalibration/conf --config icubEyes_640x480.ini --ping_robot_tmo 40.0 --eyeTiltMin -15 --eyeTiltMax 15.0 --camerasFile icubEyes.ini"

# iKinGazeCtrl --context iCubLugano01/cameraCalibration/conf --config icubEyes_640x480.ini --ping_robot_tmo 40.0 --eyeTiltMin -15 --eyeTiltMax 15.0 --camerasFile icubEyes_640x480.ini"
# or shoudl it be like that?! gnome-terminal --profile=hold --tab --working-directory=/home/icub/im-clever/icVision/ -t "iKinGazeCtrl" -e "iKinGazeCtrl --context iCubLugano01/cameraCalibration/conf --config icubEyes_640x480.ini --ping_robot_tmo 40.0 --eyeTiltMin -15 --eyeTiltMax 15.0 --camerasFile icubEyes.ini"
gnome-terminal --profile=hold \
--tab --working-directory=/home/icub/im-clever/icVision/build/src/icThreeDModule/ -t "icThreeDModule" -e "./icThreeDModule --robot icub" \
--tab --working-directory=/home/icub/im-clever/icVision/build/src/icFilterModule/ -t "icFilter" -e "./BlueCupDetector --robot icub" \
--tab --working-directory=/home/icub/im-clever/icVision/build/src/icGazeModule/ -t "icGazeModule" -e "./icGazeModule --robot icub"

gnome-terminal --profile=hold \
--tab --working-directory=~ -t "YARPView" -e "yarpview /bla" \
--tab -t "iCub App Directory" --working-directory=$ICUB_ROOT/app -e "yarp connect /icVision/BlueDetector/rawoutput:o /bla"


#--tab --working-directory=$ICUB_ROOT/app -t "gYarpManagger" -e "gyarpmanager" --tab -t "iCub App Directory" --working-directory=$ICUB_ROOT/app
# exit
