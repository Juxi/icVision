
#include "touchthread.h"

using namespace std;
using namespace yarp::os;

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
	cout << "Running for 60 seconds ..." << endl;
	cout << "Thumb" << "\t" << 
		"Index" << "\t" <<
		"Middle" << "\t" <<
		"Ring" << "\t" <<
		"Little" << "\t" <<
		"Palm" << endl;
	for (int i = 0 ; i< 60; ++i)
	{
		cout << tts.getThumbTouch() << "\t" << 
			tts.getIndexTouch() << "\t" <<
			tts.getMiddleTouch() << "\t" <<
			tts.getRingTouch() << "\t" <<
			tts.getLittleTouch() << "\t" <<
			tts.getPalmTouch() << endl;

		Time::delay(1.00);
	};
	cout << "Done." << endl;
	tts.stop();
    	return true; 
}

