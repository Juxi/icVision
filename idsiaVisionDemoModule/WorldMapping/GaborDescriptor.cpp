/*
 * GaborDescriptor.cpp
 *
 *  Created on: Jul 1, 2011
 *      Author: icub
 */

#include "GaborDescriptor.h"

using namespace cv;
using namespace std;

GaborDescriptor::GaborDescriptor() {
	// TODO Auto-generated constructor stub

	mp_GaborFilter = new GaborFilter();

}

GaborDescriptor::~GaborDescriptor() {
	// TODO Auto-generated destructor stub
}


void GaborDescriptor::setParam(unsigned int L, unsigned int D, float f, float kmax, float sigma, float N, bool norm)
{

	#if DEBUGGABOR
		std::cout<<"Gabor class::setParam";
	#endif



/*	COMPUTE GABORDESCRIPTOR	*/
	m_L = L;
	m_D = D;
	m_f = f;
	m_kmax = kmax;
	m_sigma = sigma;
	m_N = N;
	m_norm = norm;

	mvec_filtersize.clear();
	mvec_filtersize.push_back(55);
	mvec_filtersize.push_back(43);
	mvec_filtersize.push_back(35);
	mvec_filtersize.push_back(25);
	mvec_filtersize.push_back(19);


	cout<<mvec_filtersize.size()<<endl;

	m_gap = -1000;

	for(std::vector<int>::iterator iter = mvec_filtersize.begin(); iter != mvec_filtersize.end(); ++iter)
	{
		int temp = *iter;
		if( temp > m_gap)
			m_gap = temp;
	}

	mp_GaborFilter->setParam(L, D, f, kmax, sigma, N);
	mp_rescaledimg = cvCreateImage(cvSize(m_gap, m_gap), IPL_DEPTH_32F, 1);

	m_offsetx = -1; //THIS IS FIXED!
	m_offsety = -1;

	m_init = true;

}

bool GaborDescriptor::detect(vector<KeyPoint> &keypoint, IplImage* iplgrimagein, Mat& gaborDescr)
{
	IplImage* iplresimagein = NULL;
	CvMat* m_descriptor = NULL;


	if(m_init == false) {
		cerr << "Gabor Descriptor::execute::ERROR::Call setParam first!"<<endl;
		return false;
	}

	if(iplgrimagein->nChannels != 1)
	{
		//TODO convert to graylevel image
		std::cerr <<"Gabor class::execute()::ERROR::grayimg must be single-channel 8-bit image!";
		return false;
	}

	if(keypoint.empty()) {
		std::cerr <<"Gabor class::execute()::ERROR::cornerCount <= 0!";
		return false;
	}

	if(m_offsetx == -1) //Where is m_offsety??
	{
		m_imagesize.width = iplgrimagein->width;
		m_imagesize.height = iplgrimagein->height;
		m_biggerimagesize.width = m_imagesize.width + (m_gap-1);
		m_biggerimagesize.height = m_imagesize.height + (m_gap-1);
		m_offsetx = cvRound((m_gap-1)/2);
		m_offsety = cvRound((m_gap-1)/2);

	}

	m_descriptor = cvCreateMat (keypoint.size(), m_L*m_D, CV_32FC1);
	float* descriptorsdata = m_descriptor->data.fl;

	//resize image to add noise close to the borders
	if(!iplresimagein)
		iplresimagein = cvCreateImage(m_biggerimagesize, iplgrimagein->depth, iplgrimagein->nChannels); //IplImage* (before CVImage*)
	resize(iplgrimagein, iplresimagein,  &m_biggerimagesize, true);


	if(iplresimagein->depth == IPL_DEPTH_8U) {
		mp_cvgrayimgflt = m_ConvertFlt.convert(iplresimagein);
	}
	else{
		mp_cvgrayimgflt = iplresimagein;
	}

	cout<<keypoint.size()<<endl;
	//Compute Gabor Filter
	compute(keypoint, m_descriptor);

	#if DEBUGGABORDESCRIPTOR
		float* elementde;
		elementde = m_descriptor->data.fl;
		int   stepde  = m_descriptor->step/sizeof(float);
		for (unsigned int rows = 0; rows < m_descriptor->rows; rows++)
		{
			for (unsigned int cols = 0; cols < m_descriptor->cols; cols ++)
			{

				std::cout<<"\n"<<nameclass<<"::execute::m_descriptor["<<rows<<","<<cols<<"] (before normalization)= "<<(elementde+rows*stepde)[cols]<<"\n";
				//cvWaitKey(0);
			}
		}
	#endif

	if(m_norm)
		normalize(m_descriptor);

	gaborDescr = cvarrToMat(m_descriptor).clone();
	cvReleaseMat(&m_descriptor);
	return true;

}

void GaborDescriptor::compute(vector<cv::KeyPoint> KeyPoints, CvMat*  m_descriptor) {

	#if DEBUGGABOR
		std::cout<<"Gabor class::compute";
	#endif

	vector<cv::KeyPoint> HarrisKeys = KeyPoints;
	//TODO SKIPPED
	//float* regiondata = mp_regions->getData();
	//TODO HARDCODED
	unsigned int regionwidth = 2;	//mp_regions->getWidth();
	//TODO MODIFIED //float* descdata = m_descriptor.getData();
	float* descdata = m_descriptor->data.fl;
	#if DEBUGGABOR
		std::cout<<"Gabor class::compute::descdata = "<<*descdata<<"\n";
		//cvWaitKey(0);
	#endif
	//TODO HARDCODED
	unsigned int descwidth = 40;	//m_descriptor.getWidth();

	unsigned int startindex = 0;
	unsigned int endindex = HarrisKeys.size()-1;//mp_regions->getCurrentSize() - 1;

	IplImage* img = mp_cvgrayimgflt;
	IplImage* rescaledimg = mp_rescaledimg;

	#if DEBUGGABOR
		std::cout<<"Gabor class::execute::img size = [" << img->width << ", " << img->height <<"]\n";
		std::cout<<"Gabor class::execute::rescaledimg size = [" << rescaledimg->width << ", " << rescaledimg->height <<"]\n";
	#endif

	//TODO CHECK img = mp_cvgrayimgflt non mi convince affatto è troppo bianca
	/*#if DEBUGGABOR
		cvNamedWindow("GABORDESC0",1);
		cvShowImage("GABORDESC0",img);
		cvNamedWindow("GABORDESC1",1);
		cvShowImage("GABORDESC1",rescaledimg);
		cvWaitKey(0);
	#endif	*/

	unsigned int temp = m_gap;

	#if DEBUGGABOR
		std::cout<<"Gabor class::execute::temp= [" << temp <<"]\n";
		//cvWaitKey(0);
	#endif

	GaborFilter* gabor = mp_GaborFilter;

		for(unsigned int i = startindex; i<=endindex; i++) {

		// Rescale image (hopefully cvCopy and cvResize functions are thread-safe)
		unsigned int posx = cvRound(HarrisKeys[i].pt.x) + m_offsetx;
		unsigned int posy = cvRound(HarrisKeys[i].pt.y) + m_offsety;
		#if DEBUGGABOR
			if(i == 0 || i == endindex/2 || i == endindex)
			{
				std::cout<<"Gabor class::compute::Harrisx["<<i<<"] = " << posx-m_offsetx;
				std::cout<<"Gabor class::execute::Harrisy["<<i<<"] = " << posy-m_offsety;
				std::cout<<"Gabor class::compute::posx["<<i<<"] = " << posx;
				std::cout<<"Gabor class::execute::posy["<<i<<"] = " << posy<<"\n";
			}
		#endif
		unsigned int width = cvRound(55/2);
		unsigned int height = cvRound(55/2);
		unsigned int startx = posx - cvRound((width-1)/2);
		unsigned int starty = posy - cvRound((height-1)/2);
		CvRect inputrect;
		inputrect.x = startx;
		inputrect.y = starty;
		inputrect.width = width;
		inputrect.height = height;
		#if DEBUGGABOR
			std::cout<<"Gabor class::execute::inputrect.width = "<<inputrect.width;
			std::cout<<"Gabor class::execute::inputrect.height = "<<inputrect.height;
			std::cout<<"Gabor class::execute::img->nChannels = "<<img->nChannels<<"\n";
			//cvWaitKey(0);
		#endif

		cvSetImageROI(img, inputrect);
		if(width == temp && height == temp) {
			// Simple copy operation
			cvCopy(img, rescaledimg, NULL);
			#if DEBUGGABOR
				cvNamedWindow("filterimgor",1);
				cvShowImage("filterimgor",rescaledimg);
				//cvWaitKey(0);
			#endif
		}
		else{
			// Crop-resize the image to match the filter size
			cvResize(img, rescaledimg, CV_INTER_LINEAR);
			#if DEBUGGABOR
				cvNamedWindow("filterimgor",1);
				cvShowImage("filterimgor",rescaledimg);
				//cvWaitKey(0);
			#endif
		}
		cvResetImageROI(img);


		// Convolution
		CvPoint pos;
		pos.x = (width-1)/2;
		pos.y = (height-1)/2;
		float* output = &(descdata[i*descwidth]);
		#if DEBUGGABOR
			std::cout<<"Gabor class::execute::pos.x = "<<pos.x<<"\n";
			std::cout<<"Gabor class::execute::pos.y = "<<pos.y<<"\n";
			std::cout<<"Gabor class::execute::i*descwidth = "<<i*descwidth<<"\n";
			std::cout<<"Gabor class::execute::output = "<<*output<<"\n";
			//cvWaitKey(0);
		#endif
		gabor->compute(rescaledimg, &pos, output);
	}



	//cvReleaseImage (&img);
	//cvReleaseImage (&rescaledimg);

}

void GaborDescriptor::normalize(CvMat*  m_descriptor) {

	unsigned int index = 0;
	for(unsigned int i = 0;i<m_descriptor->rows;i++) {
		float sum = 0.0;
		float* tempfl = &(m_descriptor->data.fl[index]);
		for(unsigned int j = 0;j<m_descriptor->cols;j++) {
			float temp = tempfl[j];
			sum += temp * temp;
			index++;
		}

		if(sum > 0.0) {
			sum = sqrt(sum);
			for(unsigned int j = 0;j<m_descriptor->cols;j++) {
				tempfl[j] /= sum;
			}
		}

		index += ((m_descriptor->step - 4*m_descriptor->cols)/4);
	}
}

bool GaborDescriptor::resize(IplImage* iplgrimagein, IplImage* mp_cvimage, CvSize* mp_fixedOutputSize, bool m_addnoise) {
	/*	READ A GRAY IMAGE	*/

	if(iplgrimagein->nChannels != 1)
	{
		std::cerr <<"::execute()::ERROR::grayimg must be single-channel 8-bit image!";
		return false;
	}

	//add noise
	if(m_addnoise == false) {
		cvSetZero(mp_cvimage);
	}
	else{
		CvRNG m_rand = cvRNG(-1);
		cvRandArr( &m_rand, mp_cvimage, CV_RAND_UNI, cvScalar(0,0,0,0), cvScalar(255,255,255,0) );
	}

	m_outputrect.x = cvRound((mp_cvimage->width - iplgrimagein->width)/2.0);
	m_outputrect.y = cvRound((mp_cvimage->height - iplgrimagein->height)/2.0);
	m_outputrect.width = iplgrimagein->width;
	m_outputrect.height = iplgrimagein->height;
	cvSetImageROI(mp_cvimage, m_outputrect);
	//cout<<m_outputrect.width<<" "<<m_outputrect.height<<endl;

	//cout<<iplgrimagein->width<<" "<<iplgrimagein->height<<" "<<iplgrimagein->depth<<" "<<iplgrimagein->nChannels<<endl;
	//cout<<mp_cvimage->width<<" "<<mp_cvimage->height<<" "<<mp_cvimage->depth<<" "<<mp_cvimage->nChannels<<endl;
	cvCopy(iplgrimagein, mp_cvimage);
	cvResetImageROI(iplgrimagein);
	cvResetImageROI(mp_cvimage);

}
