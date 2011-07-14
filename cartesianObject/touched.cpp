// 1 index(12) 2 middle(12) 3 ring(12) 4 little(12) 5 thumb(12) 6 empty(12) 7 empty(12) 8 empty(12) 9 palm(12) 10 palm(12) 11 palm(12) 12 palm(12) 13 empty(12) 14 empty(12) 15 empty(12) 16 empty(12)

#include <yarp/os/all.h>
#include <iostream>
#include <float.h>
#include <string.h>

using namespace std;
using namespace yarp::os;

char * part_name(int i)
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

int main(int argc, char *argv[]) 
{
    char *portname;		
    char defportname[] = "/touchcounter";
    if (argc<2)
    {
    	portname=defportname; 
        cout << "Setting portname to default port name '" << portname << "'" << endl;
    }
    else
    {
    	portname=argv[1];
        cout << "Setting portname to command line given port name '" << portname << "'" << endl;
    };	
    Network yarp;
    BufferedPort<Bottle> port;
    port.open(portname);
    cout << "waiting for input, please call something like"<< endl;
    cout << "   'yarp connect /icubSim/skin/left_hand " << portname << "'" << endl;
    cout << "or" << endl;
    cout << "   'yarp connect /icub/skin/lefthand " << portname << "'" << endl;
    cout << "in another shell." << endl;
    double mins[12*16];
    double maxs[12*16];
    double means[12*16];
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
            for (int i=0; i<input->size(); i++) {
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
            cout << "thumb contacts mean: " << thumb/12 << endl;
            cout << "index contacts mean: " << index/12 << endl;
            cout << "middle contacts mean: " << middle/12 << endl;
            cout << "ring contacts mean: " << ring/12 << endl;
            cout << "little contacts mean: " << little/12 << endl;
            cout << "palm contacts mean: " << palm/(4*12) << endl;
            cout << "should be empty sum: " << empty << endl;
            cout << "sum of contacts mean w/o empty: " << (thumb +index +middle +ring +little +palm)/(9*12) << endl;
            cout << "sum of contacts: " << total << endl;
        }
	else
	{
		cerr << "Input is empty!" << endl; 
	};
    }
    cout << "i" << ": " << "min" << "\t" << "mean" << "\t" << "max" << "\t" << "(part)" << endl;
    for (int i=0; i<12*16; i++) 
    {
        cout << i << ": " << mins[i] << "\t" << means[i] << "\t" << maxs[i] << "\t" << "(" <<  part_name(i) << ")" << endl;
    };
    while (true) {    
        Bottle *input = port.read();
        if (input!=NULL) {
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
	        continue;
	    };
	    nums = nums+1;
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
        }
	else
	{
		cerr << "Input is empty!" << endl; 
	};
    }
    return 0;
}
