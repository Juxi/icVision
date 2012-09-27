#include <iostream>

#include "RobotFilter.h"

using namespace std;
using namespace yarp::os;

int main(int argc, char **argv) {
	cout << "enter main" << endl;
	
	Network yarp;

	// create the RobotFilter
	RobotInterface ri;
	RobotFilter rf;

	// open the RobotFilter
	if (true == rf.open(ri)) {
		while (true) {
			// run for ever
		}
		rf.close();
	} else {
		cout << "##  COULD NOT OPEN THE FILTER!  Program is stopping." << endl;
		cout.flush();
	}

	return 0;
}

