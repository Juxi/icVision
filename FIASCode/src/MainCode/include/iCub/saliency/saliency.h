#ifndef Saliency_H_
#define Saliency_H_


#include "cv.h"
#include "highgui.h"
#include "cxcore.h"

#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>

#include <fstream>
#include <sys/stat.h>
#include <cstdlib>

//YARP
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>

#include "iCub/camera/displayiplimage.h"
#include "iCub/saliency/envision_lib.h"

#define DEBUGSALIENCY 0

#define DISPLAYSALIENCYIMAGE 1
#define SAVESALIENCYIMAGE 0

#include "iCub/variables.h"

using namespace std;
using namespace cv;
using namespace yarp;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;


class Saliency
{

	public:

		Saliency(std::string name = "Saliency");

		~Saliency();

                void execute();
		
		void inib(int featnumber);
		
		bool move;
		CvPoint pointOL;
		CvPoint pointOR;
		
		double excite;

	private:
	
		//class names
		string nameclass;
		
/*		READ IMAGE LEFT		*/
		//port names	
		string nameImageLeftInPort;
		string nameSenderImageLeftPort;
		BufferedPort<ImageOf<PixelRgb> > ReceivingImageLeftPort;
		IplImage* iplimageleftin;
		ImageOf<PixelRgb> *yarpimageleftin;
		bool receiveimageleft;

/*		READ IMAGE RIGHT	*/
		//port names	
		string nameImageRightInPort;
		string nameSenderImageRightPort;
		BufferedPort<ImageOf<PixelRgb> > ReceivingImageRightPort;
		IplImage* iplimagerightin;
		ImageOf<PixelRgb> *yarpimagerightin;
		bool receiveimageright;	
		
/*		READ HARRIS	LEFT	*/
		string nameHarrisLeftInPort;
		string nameSenderHarrisLeftPort;
		BufferedPort<Bottle> ReceivingHarrisLeftPort;
		bool receiveharrisleft;
		Bottle *HarrisLeftIn;
		int* keypointsL;
		
/*		READ HARRIS RIGHT	*/
		string nameHarrisRightInPort;
		string nameSenderHarrisRightPort;
		BufferedPort<Bottle> ReceivingHarrisRightPort;
		bool receiveharrisright;
		Bottle *HarrisRightIn;
		int* keypointsR;

                //for parallelization
                string nameInhibitHarrisInPort;
                BufferedPort<Bottle> InhibitHarrisInPort;
                bool receiveinhibitharris;
                Bottle *inhibitHarrisArray;
                vector<bool> inhibitHarris;
		
/*		SALIENCY	*/
		IplImage* ipl_inputL;
		IplImage* ipl_inputR;
		IplImage* ipl_outputL;
		IplImage* ipl_outputR;	
		int index;
		int* results;
		string output_filenameL;
		string output_filenameR;
		CvFont font;
		CvPoint CenterL;
		CvPoint CenterR;
		
/*		SEND WINNER POINT	*/
		/*
		CvPoint winnerPosL;
		CvSize winnerSizeL;
		CvRect winnerRectL;
		int reliabilityL;
		CvSize imageSizeL;
		CvPoint winnerPosR;
		CvSize winnerSizeR;
		CvRect winnerRectR;
		int reliabilityR;
		CvSize imageSizeR;
		string namePointLeftOutPort;
		BufferedPort<Bottle> SendingPointLeftPort;
		string namePointRightOutPort;
		BufferedPort<Bottle> SendingPointRightPort;
		*/

/*		DISPLAY SALIENCY	*/
		#if DISPLAYSALIENCYIMAGE
			string nameImageLeftOutPort;
			DisplayIplImage *dispImageLeft;
			BufferedPort<ImageOf<PixelRgb> > SendingImageLeftPort;
			ImageOf<PixelRgb> yarpimageleftout;
			
			string nameImageRightOutPort;
			DisplayIplImage *dispImageRight;
			BufferedPort<ImageOf<PixelRgb> > SendingImageRightPort;
			ImageOf<PixelRgb> yarpimagerightout;
		#endif


};

#endif 
