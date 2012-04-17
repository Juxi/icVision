// Copyright: (C) 2011-2012 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// find more information at http://Juxi.net/projects/icVision/
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _ICVISION_CORE_FILTERPORT_H_
#define _ICVISION_CORE_FILTERPORT_H_

#include <string>
using namespace std;

#include <yarp/os/all.h>
#include <yarp/sig/all.h>

using namespace yarp::os;
using namespace yarp::sig;

#include "../icFilterModule/icImage.h"

class icFilterPort : public BufferedPort< ImageOf<PixelBgr> >
{
protected:
	int moduleID;
	// probably not really needed! (should be predefined)
	char port_name[100];

public:
	icFilterPort(int ID_in);
	virtual ~icFilterPort();

	bool openAndConnectPort(const char *in_port_name);	
	
	icImage* getImage() {
		ImageOf<PixelBgr> *img = this->read();  // read an image
		if (img == NULL) { 
			std::cout << "ERROR: Could not read from port '" << this->getName() << "'!" << std::endl;
			return false;
		}
		IplImage *read = (IplImage*) img->getIplImage();
		icImage* output = new icImage(read);
		return output;
	}
	
};


#endif
