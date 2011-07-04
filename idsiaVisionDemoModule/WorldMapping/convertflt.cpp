#include "convertflt.h"


ConvertFlt::ConvertFlt() {
	mp_cvoutputimg = NULL;
}

ConvertFlt::~ConvertFlt() {
	if(mp_cvoutputimg) cvReleaseImage(&mp_cvoutputimg);
}

IplImage* ConvertFlt::convert(IplImage* img) {

	if(!img) { std::cerr << "ConvertFlt::convert()::ERROR::img is NULL!\n"; return NULL; }

	if(img->depth != IPL_DEPTH_8U) { std::cerr << "ConvertFlt::convert()::ERROR::img depth incorrect!\n"; return NULL; }

	if(mp_cvoutputimg) {
		if(img->width != mp_cvoutputimg->width || img->height != mp_cvoutputimg->height) {
			cvReleaseImage(&mp_cvoutputimg);
			mp_cvoutputimg = NULL;
		}
	}

	if(!mp_cvoutputimg) {
		mp_cvoutputimg = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_32F, img->nChannels);
	}
	cvConvert(img, mp_cvoutputimg);

	#if DEBUGCONVERT
				std::cout<<"\nConvert::convert:: mp_cvoutputimg->depth = "<<mp_cvoutputimg->depth<<"\n";
	#endif
	
	return mp_cvoutputimg;
}


