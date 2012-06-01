#ifndef CameraiCub_H
#define CameraiCub_H

#include "cv.h"
#include "highgui.h"
#include "cxcore.h"

#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>

//YARP
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>

#define DEBUGCAMERAICUB 0

using namespace std;
using namespace cv;
using namespace yarp;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

class CameraiCub
{
	public:

		CameraiCub(std::string name = "Camera",std::string side="side",std::string input="input");
		~CameraiCub();
		void setParam();
		void execute();

	private:

		//class names
		string nameclass;
		string namesideclass;
		string	nameinput;
		
/*		READ AN IMAGE	*/
		string nameImageInPort;
		string nameSenderImagePort;
		BufferedPort<ImageOf<PixelRgb> > ReceivingImagePort;
		IplImage* iplimagein;
		ImageOf<PixelRgb> *yarpimagein;
		bool receiveimage;
	
/*		SEND AN IMAGE	*/	
		string nameImageOutPort;
		string nameimagepath;

		//create a port to send images
		BufferedPort<ImageOf<PixelRgb> > SendingImagePort;		
		IplImage *iplimageout;
		//yarp image to send trow the port
		ImageOf<PixelRgb> yarpimageout;
		
};


#endif
