#include "touchthread.h"

using namespace std;
using namespace yarp::os;

// #define DEBUG

bool TouchSenseThread::threadInit()
	{
		
		thumbTouch = false;
		indexTouch = false;
		middleTouch = false;
		ringTouch = false;
		littleTouch = false;
		palmTouch = false;
		thumbTouchP = 0.0;
		indexTouchP = 0.0;
		middleTouchP = 0.0;
		ringTouchP = 0.0;
		littleTouchP = 0.0;
		palmTouchP = 0.0;

		unsigned long nums = 0;
		for (int i=0; i<12*16; i++) 
		{
			mins[i] = DBL_MAX;
			maxs[i] = DBL_MIN;
			means[i] = 0.0;
		};
	while (nums<100) 
	{    
		Bottle *input = port.read();
		if (input!=NULL) 
		{
			// cout << "got " << input->toString().c_str() << endl;
			double total = 0;
			double thumb = 0;
			double index = 0;
			double middle = 0;
			double ring = 0;
			double little = 0; 
			double palm = 0;
			double empty = 0;
			if (12*16!=input->size())
			{
				cerr << "Warning: not enougth values! 12*16 values required for the touch sensors of each hand" << endl;
				continue;
			};
			nums = nums+1;
			for (int i=0; i<input->size(); i++) 
			{
				double value = input->get(i).asDouble();
				if (value<mins[i]) mins[i] = value;
				if (value>maxs[i]) maxs[i] = value;
				means[i] = ((nums-1.0)/nums)*means[i]+(1.0/nums)*value;
				total += value;
				if (i<1*12)
					index += value;
				else if (i<2*12)
					middle += value;
				else if (i<3*12)
					ring += value;
				else if (i<4*12)
					little += value;
				else if (i<5*12)
					thumb += value;
				else if (i<6*12)
					empty += value;
				else if (i<7*12)
					empty += value;
				else if (i<8*12)
					empty += value;
				else if (i<9*12)
					palm += value;
				else if (i<10*12)
					palm += value;
				else if (i<11*12)
					palm += value;
				else if (i<12*12)
					palm += value;
				else if (i<13*12)
					empty += value;
				else if (i<14*12)
					empty += value;
				else if (i<15*12)
					empty += value;
				else if (i<16*12)
					empty += value;
		    	}
#ifdef DEBUG
			cout << "thumb contacts mean: " << thumb/12 << endl;
			cout << "index contacts mean: " << index/12 << endl;
			cout << "middle contacts mean: " << middle/12 << endl;
			cout << "ring contacts mean: " << ring/12 << endl;
			cout << "little contacts mean: " << little/12 << endl;
			cout << "palm contacts mean: " << palm/(4*12) << endl;
			cout << "should be empty sum: " << empty << endl;
			cout << "sum of contacts mean w/o empty: " << (thumb +index +middle +ring +little +palm)/(9*12) << endl;
			cout << "sum of contacts: " << total << endl;
#endif
		}
		else
		{
			cerr << "Input is empty!" << endl; 
		};
	    }
#ifdef DEBUG
	    cout << "i" << ": " << "min" << "\t" << "mean" << "\t" << "max" << "\t" << "(part)" << endl;
	    for (int i=0; i<12*16; i++) 
	    {
		cout << i << ": " << mins[i] << "\t" << means[i] << "\t" << maxs[i] << "\t" << "(" <<  part_name(i) << ")" << endl;
	    };
#endif
    	    return true;
	}

	void TouchSenseThread::afterStart(bool s)
	{
#ifdef DEBUG
		if (s)
			cout << "Thread started successfully" << endl;
		else
			cout << "Thread did not start" << endl;
#endif
	}

	void TouchSenseThread::run()
	{
		while (!isStopping()) 
		{
		        Bottle *input = port.read();
			if (input!=NULL) 
			{
				// cout << "got " << input->toString().c_str() << endl;
				double thumb = 0;
				double index = 0;
				double middle = 0;
				double ring = 0;
				double little = 0; 
				double palm = 0;
				if (12*16!=input->size())
				{
					cerr << "Warning: not enougth values! 12*16 values required for the touch sensors of each hand" << endl;
					return;
				};
				//nums = nums+1;
				for (int i=0; i<input->size(); i++) 
				{
					double value = input->get(i).asDouble();
					double mvalue = value-means[i];
					// using a moving average for sensor drift....
					means[i] = 0.95*means[i]+0.05*value; // 0.05 is for a event arround 1 second 
					if (i<1*12)
						index += mvalue;
					else if (i<2*12)
						middle += mvalue;
					else if (i<3*12)
						ring += mvalue;
					else if (i<4*12)
						little += mvalue;
					else if (i<5*12)
						thumb += mvalue;
					else if (i<8*12)
						/* nop */;
					else if (i<12*12)
						palm += mvalue; // 4 times more values, but also 10 times less sensitive...
				}
				thumbTouchP = (-thumb/50);
				indexTouchP = (-index/50);
				middleTouchP = (-middle/50);
				ringTouchP = (-ring/50);
				littleTouchP = (-little/50);
				palmTouchP = (-palm/20);
				thumbTouch = (thumb<-50);
				indexTouch = (index<-50);
				middleTouch = (middle<-50);
				ringTouch = (ring<-50);
				littleTouch = (little<-50);
				palmTouch = (palm<-20);
#ifdef DEBUG
				cout.precision(0);
				cout << fixed ;
				cout << "thumb contacts diff to mean: " << thumb; 
				if (thumb<-50) cout << " touchée!";
				cout << endl;
				cout << "index contacts diff to mean: " << index;
				if (index<-50) cout << " touchée!";
				cout << endl;
				cout << "middle contacts diff to mean: " << middle;
				if (middle<-50) cout << " touchée!";
				cout << endl;
				cout << "ring contacts diff to mean: " << ring;
				if (ring<-50) cout << " touchée!";
				cout << endl;
				cout << "little contacts diff to mean: " << little;
				if (little<-50) cout << " touchée!";
				cout << endl;
				cout << "palm contacts diff to mean: " << palm;
				if (palm<-20) cout << " touchée!";
				cout << endl;
#endif
			}
			else
			{
				cerr << "Input is empty!" << endl; 
			};
		};
	}

	void TouchSenseThread::threadRelease()
	{    
#ifdef DEBUG
		cout << "Thread released. Anything to do?" << endl;
#endif
		
		thumbTouch = false;
		indexTouch = false;
		middleTouch = false;
		ringTouch = false;
		littleTouch = false;
		palmTouch = false;
	}

	char * TouchSenseThread::part_name(int i)
	{
		static char name[256];
		if (i<1*12)
			strcpy(name,"0 index");
		else if (i<2*12)
			strcpy(name,"1 middle");
		else if (i<3*12)
			strcpy(name,"2 ring");
		else if (i<4*12)
			strcpy(name,"3 little");
		else if (i<5*12)
			strcpy(name,"4 thumb");
		else if (i<6*12)
			strcpy(name,"5 empty00");
		else if (i<7*12)
			strcpy(name,"6 empty01");
		else if (i<8*12)
			strcpy(name,"7 empty02");
		else if (i<9*12)
			strcpy(name,"8 palm0");
		else if (i<10*12)
			strcpy(name,"9 palm1");
		else if (i<11*12)
			strcpy(name,"10 palm2");
		else if (i<12*12)
			strcpy(name,"11 palm3");
		else if (i<13*12)
			strcpy(name,"12 empty10");
		else if (i<14*12)
			strcpy(name,"13 empty11");
		else if (i<15*12)
			strcpy(name,"14 empty12");
		else if (i<16*12)
			strcpy(name,"15 empty13");
		else
			strcpy(name,"outofrange");
		return name;
	};





