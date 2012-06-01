#ifndef GaborDescriptor_H
#define GaborDescriptor_H

#include "cv.h"
#include "highgui.h"
#include "cxcore.h"

#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>

#include "iCub/camera/displayiplimage.h"
#include "iCub/featureProcessing/gaborfilter.h"
#include "iCub/featureProcessing/resize.h"
#include "iCub/featureProcessing/convertflt.h"

//YARP
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>

#define DEBUGGABOR 0
#define DEBUGGABORDESCRIPTOR 0

using namespace std;
using namespace cv;
using namespace yarp;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

class GaborDescriptor
{
	public:

		GaborDescriptor(std::string name = "GaborDescriptor",std::string side="side",std::string input="input");
		~GaborDescriptor();

		//TODO create this function m_descriptor is an instance of CSCDescriptors
		//void setDescriptorType(const std::string& type) { m_descriptor.setDescriptorType(type); }

		void setParam(unsigned int L=5, unsigned int D=8, float f=1.0/sqrt(2.0), float kmax=1.7, float sigma=2*3.1416, float N=1.0, bool norm = true, std::vector<int>* filtersize = NULL);

		void execute();
	
		void normalize();

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

/*		READ HARRIS	*/
		string nameHarrisInPort;
		string nameSenderHarrisPort;
		BufferedPort<Bottle> ReceivingHarrisPort;
		bool receiveharris;
		Bottle *HarrisIn;
		unsigned int m_cornerCount; //maximum number of corners returned
		CvPoint2D32f* mp_corners;
		
/*		DISPLAY GRAY IMAGE	*/
		#if DEBUGGABOR
			string nameGrImageOutPort;
			DisplayIplImage *dispGrImage;
			BufferedPort<ImageOf<PixelRgb> > SendingGrImagePort;
			ImageOf<PixelRgb> yarpgrimageout;
		#endif
		
/*		COMPUTE GABORDESCRIPTOR	*/
		int m_offsetx, m_offsety, m_gap;
		GaborFilter* mp_GaborFilter;
		bool m_norm;	
		Resize* mpc_ResizeBiggerImage;	
		IplImage* mp_rescaledimg;
		//TODO Create this class
		//CSCDescriptors m_descriptor;
		//TODO POTREBBE ESSERE SENZA PUNTATORE
		CvMat* m_descriptor;
		unsigned int m_L, m_D;
		float m_f, m_kmax, m_sigma, m_N;
		std::vector<int> mvec_filtersize;
		bool m_init;
		CvSize m_imagesize;
		CvSize m_biggerimagesize;
		ConvertFlt m_ConvertFlt;	
		IplImage* mp_cvgrayimgflt;	
		void compute(vector<cv::KeyPoint> KeyPoints);
		//vector<cv::KeyPoint> *HarrisKeys;


/*		READ A RESIZED IMAGE	*/
		//port names	
		string nameResImageInPort;
		string nameSenderResImagePort;
		BufferedPort<ImageOf<PixelMono> > ReceivingResImagePort;
		IplImage* iplresimagein;
		ImageOf<PixelMono> *yarpresimagein;
		bool receiveresimage;
		
/*		SEND GABOR	*/
		string nameGaborOutPort;
		BufferedPort<Bottle> SendingGaborPort;
				
};


#endif
