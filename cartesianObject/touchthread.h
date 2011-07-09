// 1 index(12) 2 middle(12) 3 ring(12) 4 little(12) 5 thumb(12) 6 empty(12) 7 empty(12) 8 empty(12) 9 palm(12) 10 palm(12) 11 palm(12) 12 palm(12) 13 empty(12) 14 empty(12) 15 empty(12) 16 empty(12)
#ifndef TOUCSENSETHREAD_ISDEF
#define TOUCSENSETHREAD_ISDEF

#include <yarp/os/all.h>
#include <iostream>
#include <float.h>
#include <string.h>
#include <string>

using namespace std;
using namespace yarp::os;

class TouchSenseThread: public Thread
{
protected:
	BufferedPort<Bottle> port;
	double mins[12*16];
	double maxs[12*16];
	double means[12*16];
	bool thumbTouch;
	bool indexTouch;
	bool middleTouch;
	bool ringTouch;
	bool littleTouch;
	bool palmTouch;
	double thumbTouchP;
	double indexTouchP;
	double middleTouchP;
	double ringTouchP;
	double littleTouchP;
	double palmTouchP;
	string portname;
	
	
public:
    	TouchSenseThread(const string& portname): portname(portname) 
	{
		Network yarp;
		port.open(portname.c_str());
		cout << "Port " << portname << " created." << endl;
	}
	virtual ~TouchSenseThread()
	{
		port.close();
	}
	bool getThumbTouch() const {return thumbTouch;}
	bool getIndexTouch() const {return indexTouch;}
	bool getMiddleTouch() const {return middleTouch;}
	bool getRingTouch() const {return ringTouch;}
	bool getLittleTouch() const {return littleTouch;}
	bool getPalmTouch() const {return palmTouch;}
	double getThumbTouchP() const {return thumbTouchP;}
	double getIndexTouchP() const {return indexTouchP;}
	double getMiddleTouchP() const {return middleTouchP;}
	double getRingTouchP() const {return ringTouchP;}
	double getLittleTouchP() const {return littleTouchP;}
	double getPalmTouchP() const {return palmTouchP;}

	virtual bool threadInit();

	virtual void afterStart(bool s);

	virtual void run();
	virtual void threadRelease();
	static char * part_name(int i);
};

#endif
