// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include "saliency.h"

SaliencyWorker::SaliencyWorker() {
	output_image = NULL;
}

SaliencyWorker::~SaliencyWorker() {
	if(output_image != NULL) {
		cvReleaseImage(&output_image[0]);
		cvReleaseImage(&output_image[1]);
	}
	
	// TODO
	// envisiion_cleanup();
}

IplImage** SaliencyWorker::getSaliencyMap(const IplImage* input_imageL, const IplImage* input_imageR){
	if(input_imageL == NULL || input_imageR == NULL) return NULL;
	int img_w = input_imageL->width;
	int img_h = input_imageL->height;
	std::cout << "Size: " << img_w << " " << img_h << std::endl;
	// assume L size == R size
	
	output_image = new IplImage*[2];
	output_image[0] = cvCloneImage(input_imageL);
	output_image[1] = cvCloneImage(input_imageR);
	
	// TODO if not yet initialised then do this (and if the size is different!)
  //REV: note this will also init the long-term saliency map, which you guys don't seem to want.
  //envision_init(img_w, img_h);

	envision_nextframe(input_imageL, output_image[0], 0);
	envision_nextframe(input_imageR, output_image[1], 1);
	int *results = SC_naive_competition(output_image[0], output_image[1]);
   	
   	std::cout << "RESULT im, X, Y: (" << results[1] << "/" << results[2] << ")@" << results[0] << std::endl;
  
	// output_filename = "result.ppm";
	// cvSaveImage(output_filename.c_str(), ipl_output);
	//  pointO = cvPoint(results[0]*2,results[1]*2);
	// cvShowImage((saliencystr).c_str(), ipl_output);
	
	return output_image;
}

