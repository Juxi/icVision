#ifndef GaborFilter_H
#define GaborFilter_H

#include "cv.h"
#include "highgui.h"
#include "cxcore.h"

//TODO Check if they're useful
//#include <sstream>
//#include <fstream>
#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>

#include "convertflt.h"

//YARP
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>

#define DEBUGGABORFILTER 0

using namespace std;
using namespace cv;
using namespace yarp;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;



class GaborFilter {
	
	public:
		
		typedef enum {ABSOLUTE = 0, COMPLEX = 1, AMPLITUDEPHASE = 2} computemodes;
		
		GaborFilter(std::string name = "GaborFilter",std::string side="side",std::string input="input");
		~GaborFilter();

		void setParam(unsigned int L=5, unsigned int D=8, float f=1.0/sqrt(2.0), float kmax=1.7, float sigma=2*3.1416, float N=1.0, std::vector<int>* filtersize = NULL);
		
		void generatefilters();

		float* compute(IplImage* img, CvPoint* pos, float* dst);
		
		float conv2DPointReal(IplImage* img, CvPoint* pos, unsigned int index);
		float conv2DPointImag(IplImage* img, CvPoint* pos, unsigned int index);

		private:
		
		//class names
		string nameclass;
		string namesideclass;
		string	nameinput;
		
/*		COMPUTE GABORFILTER	*/
		IplImage** mdp_filtersreal;	//(before CVImage**)
		IplImage** mdp_filtersimag;	//(before CVImage**)
		bool m_windowcreated;
		IplImage* mp_displayreal;
		IplImage* mp_displayimag;
		IplImage** mdp_responsesreal;
		IplImage** mdp_responsesimag;
		float* mp_result;
		unsigned int m_computemode;
		unsigned int m_L, m_D, m_LD;
		float m_f, m_kmax, m_sigma, m_N;
		std::string m_windownamereal;
		std::string m_windownameimag;
		std::vector<int> mvec_filtersize;
		ConvertFlt m_ConvertFlt;
					

	};


#endif


