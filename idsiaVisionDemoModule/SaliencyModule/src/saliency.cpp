// Copyright: (C) 2011 Juxi Leitner
// Author: Juxi Leitner <juxi.leitner@gmail.com>
// CopyPolicy: Released under the terms of the GNU GPL v2.0.

#include "saliency.h"

SaliencyWorker::SaliencyWorker() {
	output_image = NULL;
}

SaliencyWorker::~SaliencyWorker() {
	if(output_image != NULL) cvReleaseImage(&output_image);
	
	// TODO
	// envisiion_cleanup();
}

IplImage* SaliencyWorker::getSaliencyMap(const IplImage* input_image){
	if(input_image == NULL) return NULL;
	int img_w = input_image->width;
	int img_h = input_image->height;
	std::cout << "Size: " << img_w << " " << img_h << std::endl;
	output_image = cvCloneImage(input_image);
	
	// TODO if not yet initialised then do this (and if the size is different!)
  //REV: note this will also init the long-term saliency map, which you guys don't seem to want.
  //envision_init(img_w, img_h);


	envision_nextframe(input_image, output_image, 0);
	int *results = SC_naive_competition(output_image, output_image );
   	
   	std::cout << "RESULT im, X, Y: (" << results[1] << "/" << results[2] << ")@" << results[0] << std::endl;
  
	// output_filename = "result.ppm";
	// cvSaveImage(output_filename.c_str(), ipl_output);
	//  pointO = cvPoint(results[0]*2,results[1]*2);
	// cvShowImage((saliencystr).c_str(), ipl_output);
	
	return output_image;
}

