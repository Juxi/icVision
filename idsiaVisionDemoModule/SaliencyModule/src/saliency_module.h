// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef _SALIENCY_MODULE_H_
#define _SALIENCY_MODULE_H_

#include <string>
using namespace std;

#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/sig/Matrix.h>

#include <opencv2/opencv.hpp>

#include "saliency.h"

//# using namespace yarp::os;
//# using namespace yarp::sig;
//# using namespace std;
//# using namespace cv;

class SaliencyModule : public yarp::os::Module
{
	private:
		// module parameters
		string moduleName;
		string handlerPortName;

		//a port to handle messages
		yarp::os::Port handlerPort; 

		//output Module
		yarp::os::Port moduleOutput;
    
    
    /*port name strings*/
    string inputPortNameLeft;
    string inputPortNameRight;
    string outputPortNameLeft;
    string outputPortNameRight;
    string outputPortName;
    string inputHeadPortName;
    string inputTorsoPortName;

		/* thread parameters: they are pointers so that they refer to the original variables */
		yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelBgr> >  imageInLeft;
		yarp::os::BufferedPort<yarp::sig::ImageOf<yarp::sig::PixelBgr> >  imageOutLeft;
   



		/*Image support*/
		yarp::sig::ImageOf<yarp::sig::PixelBgr> *iCubLeft;
		cv::Mat imageLeft; //, imageRight;
		cv::Mat outputImageLeft; //, outputImageRight;
		
		
		// Saliency interface to the envision lib
		SaliencyWorker *saliency;


	public:
		SaliencyModule();
		virtual ~SaliencyModule();
		
		double getPeriod();
		bool updateModule();
		
		bool open			(yarp::os::Searchable& config);
		bool respond		(const yarp::os::Bottle& command, yarp::os::Bottle& reply);
		bool configure		(yarp::os::Searchable& config);
		bool interruptModule();
		bool close			();
	};


#endif
