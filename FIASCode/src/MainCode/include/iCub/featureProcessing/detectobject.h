#ifndef DetectObject_H
#define DetectObject_H

#include "cv.h"
#include "highgui.h"
#include "cxcore.h"

#include "iCub/variables.h"

#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>

//YARP
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>

#define DEBUGDETECT 0
#define SEGMENTATION 1

using namespace std;
using namespace cv;
using namespace yarp;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;


//! class DetectObject  -- Module for detecting presence of an object in a scene based on stereo (depth) information

	class DetectObject 
	{
		public: 
		
			typedef enum {EXHAUSTIVE = 0, BOUNDARY_BOX = 1, BOUNDARY_VERTICAL = 2} opmodes;
		
			// Parameters 
			// 320x240 : 0, 0.96, 5, 3
			// 640x480 : 0, 0.93, 11, 5

			DetectObject(std::string name = "Match");

			~DetectObject();

                        void setParam(float simthres = SIM_THRESHOLD, float hordispthres = HOR_DISP_THRESHOLD, float verdispthres = VER_DISP_THRESHOLD, unsigned int opmode = EXHAUSTIVE);
                        //void setParam(float simthres = 0.95, float hordispthres = 160.0, float verdispthres = 25.0, unsigned int opmode = EXHAUSTIVE);

			void execute();
			
			unsigned int compareleftright();
			
		private:
		
		//class names
		string nameclass;

/*		READ HARRIS	LEFT	*/
		string nameHarrisLeftInPort;
		string nameSenderHarrisLeftPort;
		BufferedPort<Bottle> ReceivingHarrisLeftPort;
		bool receiveharrisleft;
		Bottle *HarrisLeftIn;
		unsigned int m_cornerCountLeft; 
		//maximum number of corners returned
		CvPoint2D32f* mp_cornersleft;
		
/*		READ HARRIS RIGHT	*/
		string nameHarrisRightInPort;
		string nameSenderHarrisRightPort;
		BufferedPort<Bottle> ReceivingHarrisRightPort;
		bool receiveharrisright;
		Bottle *HarrisRightIn;
		unsigned int m_cornerCountRight; //maximum number of corners returned
		CvPoint2D32f* mp_cornersright;
		
/*		READ GABOR	LEFT	*/
		string nameGaborLeftInPort;
		string nameSenderGaborLeftPort;
		BufferedPort<Bottle> ReceivingGaborLeftPort;
		bool receivegaborleft;
		Bottle *GaborLeftIn;
		CvMat* m_descriptorleft;
				
/*		READ GABOR RIGHT	*/
		string nameGaborRightInPort;
		string nameSenderGaborRightPort;
		BufferedPort<Bottle> ReceivingGaborRightPort;
		bool receivegaborright;
		Bottle *GaborRightIn;
		CvMat* m_descriptorright;
		
/*		SEND MATCHING HARRIS LEFT	*/
		string nameHarrisLeftOutPort;
		BufferedPort<Bottle> SendingHarrisLeftPort;
/*		SEND MATCHING HARRIS RIGHT	*/
		string nameHarrisRightOutPort;
		BufferedPort<Bottle> SendingHarrisRightPort;
		
#if SEGMENTATION
/*		SEND MATCHING GABOR LEFT	*/
		string nameGaborLeftOutPort;
		BufferedPort<Bottle> SendingGaborLeftPort;
/*		SEND MATCHING GABOR RIGHT	*/
		string nameGaborRightOutPort;
		BufferedPort<Bottle> SendingGaborRightPort;
#endif		
		
/*		COMPUTE DETECTION	*/
		CvMat* mp_simmat;
		//CvMat* checkmat;
		/*unsigned int m_pointsthres;
		unsigned int m_detectthres, m_detectcounter;
		bool forceMaxSizeL;
		bool forceMaxSizeR;
		int m_maxsize;
		*/
		float m_simthres, m_hordispthres, m_verdispthres;
		unsigned int m_opmode;
		std::vector<unsigned int> mvec_indexesonleft;
		std::vector<unsigned int> mvec_indexesonright;
		std::vector<float> mvec_hordisp;
		std::vector<float> mvec_verdisp;
		//TODO NOT USED IN THE END THE MATCHING HARRIS CORNER POINT ARE CONTAINED IN detectregionleft and detectregionright
		//CvPoint2D32f* m_leftregiondetect;
		//CvPoint2D32f* m_rightregiondetect;
		//Will contain the matching harris corner point (first left matchs with first right etc.)
		//vector<cv::KeyPoint> detectregionleft;
		//vector<cv::KeyPoint> detectregionright;
		double m_minval, m_maxval;
		CvPoint m_minloc, m_maxloc;	

		


	};


#endif
