
#include "touchthread.h"
#include <iostream>
#include <iomanip>

using namespace std;
using namespace yarp::os;

bool callGraspController(const std::string msg);

int main(int argc, char *argv[])
{   
	Network yarp;
	if (!yarp.checkNetwork())
		return -1;

	char * portname=0;
	char * connectname=0;
	char defportname[] = "/touchcounter";
	if (argc==1)
	{
		portname=defportname; 
		cout << "Setting portname to default port name '" << portname << "'" << endl;
	}
	else if (argc==2)
	{
		portname=argv[1];
		cout << "Setting portname to command line given port name '" << portname << "'" << endl;
	}
	else if (argc==3)
	{
		connectname=argv[1];
		portname=argv[2];
		cout << "Setting portname to command line given port name '" << portname << "'" << endl;
	}
	else
	{
		cout << "Too many parameters" << endl;
		return -1;
	};	
	
	TouchSenseThread tts(portname);

	if (connectname!=0)
	{
		bool connected = Network::connect(connectname,portname);
		if (connected)
		{ 
			cout << "Connected from " << connectname << " to " << portname << "." << endl;
		}
		else
		{
			cout << "Connecting faild  from " << connectname << " to " << portname << "." << endl;
			return -2; 
		};
	}
	else
	{
		cout << "waiting for input, please call something like"<< endl;
		cout << "   'yarp connect /icubSim/skin/left_hand " << portname << "'" << endl;
		cout << "or" << endl;
		cout << "   'yarp connect /icub/skin/lefthand " << portname << "'" << endl;
		cout << "in another shell." << endl;
		cout << "alternatively you can start this program with parameters, like" << endl;
		cout << "   'touchthread /icubSim/skin/left_hand /myportname'." << endl; 
		cout << "or" << endl;
		cout << "   'touchthread /icub/skin/lefthand /myportname'." << endl; 
	};

	if (!tts.start())
	{ 
		return false;
	};
	callGraspController("pre");
	cout << "Running for 60 seconds ..." << endl;
	cout << "Thumb" << "\t" << 
		"Index" << "\t" <<
		"Middle" << "\t" <<
		"Ring" << "\t" <<
		"Little" << "\t" <<
		"Palm" << endl;
	for (int i = 0 ; i< 60; ++i)
	{
		cout <<  setprecision (2) << fixed <<
			tts.getThumbTouchP() << "\t" << 
			tts.getIndexTouchP() << "\t" <<
			tts.getMiddleTouchP() << "\t" <<
			tts.getRingTouchP() << "\t" <<
			tts.getLittleTouchP() << "\t" <<
			tts.getPalmTouchP() << endl;
		if ( tts.getThumbTouch()+ 
			tts.getIndexTouch()+
			tts.getMiddleTouch()+
			tts.getRingTouch()+
			tts.getLittleTouch()+
			tts.getPalmTouch() >=1 )
		{
			cout << "Closing ...";
			callGraspController("cls");
			cout << "done." << endl;
			Time::delay(3.00);
			cout << "Opening ...";
			callGraspController("opn");
			cout << "done." << endl;
			Time::delay(3.00);
			cout << "Preing ...";
			callGraspController("pre");
			cout << "done." << endl;
		}
		else
		{	
			Time::delay(1.00);
		}
	};
	cout << "Done." << endl;
	tts.stop();
    	return true; 
}

bool callGraspController(const std::string msg) {
	yarp::os::Network yarp;
	yarp::os::RpcClient port;

	std::string inputPortName = "/graspController";
	std::string clientPortName = "graspClient";
	if(! port.open( clientPortName.c_str() )){
		std::cout << std::endl << "ERROR: Could not open port: " << clientPortName.c_str()  << std::endl << std::endl;
		return false;
	}
	printf("Trying to connect to %s\n", inputPortName.c_str());
	if(! yarp.connect(clientPortName.c_str(), inputPortName.c_str()) ) {
		std::cout << std::endl << "ERROR: Could not connect to port: " << clientPortName.c_str() << std::endl << std::endl;		
		return false;
	}
	
	
	// sending command and receiving response
	yarp::os::Bottle cmd, response;
	
	cmd.addString(msg.c_str());		// start closing of hand... cls, pre is for pregrasping ...
	port.write(cmd, response);	
		
	if( response.toString() != "OK" ) {
		std::cout << "ERROR: did not receive ok! Rcvd: " << response.toString() << std::endl;
		return false;
	}
	return true;
}		

