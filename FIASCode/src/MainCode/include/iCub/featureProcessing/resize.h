#ifndef Resize_H
#define Resize_H

#include "cv.h"
#include "highgui.h"
#include "cxcore.h"

#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>

#include "iCub/camera/displayiplimage.h"

//YARP
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>

#define DEBUGRESIZE 0

using namespace std;
using namespace cv;
using namespace yarp;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;


	class Resize	{

		public: 

			typedef enum {CROP = 0, RESCALE = 1} mode;

			Resize(std::string name="Resize",std::string side="side",std::string input="input", unsigned int mode = RESCALE, bool addnoise = false);
			~Resize();

			void execute();

			void setParam(unsigned int mode, bool addnoise = false);
			
			void fixOutputSize(const CvSize& size);

		private:
		
		//class names
		string nameclass;
		string namesideclass;
		string	nameinput;
		
/*		READ A GRAY IMAGE	*/
		//port names	
		string nameGrImageInPort;
		string nameSenderGrImagePort;
		BufferedPort<ImageOf<PixelMono> > ReceivingGrImagePort;
		IplImage* iplgrimagein;
		ImageOf<PixelMono> *yarpgrimagein;
		bool receivegrimage;
		
		/*		DISPLAY GRAY IMAGE	*/
		#if DEBUGRESIZE
			string nameGrImageOutPort;
			DisplayIplImage *dispGrImage;
			BufferedPort<ImageOf<PixelRgb> > SendingGrImagePort;
			ImageOf<PixelRgb> yarpgrimageout;
		#endif
		
/*		SEND A RESIZED IMAGE	*/
		string nameResImageOutPort;
		BufferedPort<ImageOf<PixelMono> > SendingResImagePort;
		//IplImage *iplgrimageout;
		ImageOf<PixelMono> yarpresimageout;		

/*		COMPUTE RESIZE	*/		
		CvSize* mp_fixedOutputSize;
		unsigned int m_mode;
		bool m_addnoise;
		CvRNG m_rand;
		void addNoise(); // add white noise to unfilled areas
		int m_interpolation;  
		//TODO CREATE CVRECT TO SENDBACK AS OUTPUT
		CvRect m_outputrect;
		IplImage* mp_cvimage; //IplImage* (before CVImage*)
		CvRect temprect;


	};


#endif
