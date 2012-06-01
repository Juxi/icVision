#ifndef Camera_H
#define Camera_H

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

#define DEBUGCAMERA 0

using namespace std;
using namespace cv;
using namespace yarp;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

class Camera
{
	public:

		Camera(std::string name = "Camera",std::string side="side",std::string input="input");
		~Camera();
		void setParam(std::string path = "CameraPath");
		void execute();

	private:

		//class names
		string nameclass;
		string namesideclass;
		string	nameinput;
	
/*		SEND AN IMAGE	*/	
		//port names	
		string nameImageOutPort;
		//image path
		string nameimagepath;

		//create a port to send images
		BufferedPort<ImageOf<PixelRgb> > SendingImagePort;		
		IplImage *iplimageout;
		//yarp image to send trow the port
		ImageOf<PixelRgb> yarpimageout;
		
};


#endif
