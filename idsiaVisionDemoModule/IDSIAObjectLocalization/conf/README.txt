How to use the software

Launch the two nodes
	
	yarp run --server /iCubIDSIA

and

	yarp run --server /IDSIAOutput
	
when the software will run on the server or on the laptop the iCubIDSIA and use directly the icubsrv

Once they are running you can launch the software using manager.py and the configuration file that you want to use




Only for Davide

yarpserver &
yarp conf 195.176.191.90 10000
sudo route add -net 10.0.0.0 gw 195.176.191.90 netmask 255.255.0.0


yarp run --server /iCubIDSIA &
yarp run --server /IDSIAOutput &
iCub_SIM 


