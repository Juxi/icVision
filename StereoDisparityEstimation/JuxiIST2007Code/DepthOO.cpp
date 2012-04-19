// DepthOO.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <ctime>

int _tmain(int argc, _TCHAR* argv[])
{
	bool show = false;

	if(argc < 3) {
		printf("To few parameters...\nDepthOO <leftImg> <rightImg> [horizontalStart] [horizontalEnd] [horizontalStep] [verticalStart] [verticalEnd] [verticalStep] [lklhdparam]");
		return 1;
	}
	// parameter conversion
	char leftImgFile[256], rightImgFile[256], param[256];
	WideCharToMultiByte( GetACP(), 0, argv[1], -1, leftImgFile, 256, NULL, NULL );
	WideCharToMultiByte( GetACP(), 0, argv[2], -1, rightImgFile, 256, NULL, NULL );

	int horizontal[3], vertical[3];
	horizontal[0] = -30;		vertical[0] = 0;
	horizontal[1] = 30;			vertical[1] = 0;
	horizontal[2] = 1;			vertical[2] = 2;
	double lklhdParam = .67, filterParam = .67;
	// check if more parameters are given
	for(int i = 0;i < 3;i++) {
		if(argc > 3+i) {
			WideCharToMultiByte( GetACP(), 0, argv[3+i], -1, param, 256, NULL, NULL );
			horizontal[i] = atoi(param);
		}
		if(argc > 6+i) {
			WideCharToMultiByte( GetACP(), 0, argv[6+i], -1, param, 256, NULL, NULL );
			vertical[i] = atoi(param);
		}
	}
	if(argc > 9) {
		WideCharToMultiByte( GetACP(), 0, argv[9], -1, param, 256, NULL, NULL );
		lklhdParam = atof(param);
	}
	if(argc > 10) {
		WideCharToMultiByte( GetACP(), 0, argv[10], -1, param, 256, NULL, NULL );
		filterParam = atof(param);
	}

	// start program
	clock_t start = clock();
	DisparityMapper *mapper = new DisparityMapper();
	mapper->loadLeftImage(leftImgFile);
	mapper->loadRightImage(rightImgFile);

	// needs some more parameters I guess :)
	mapper->setShiftingRange(horizontal[0], horizontal[1], horizontal[2],
							 vertical[0] ,  vertical[1] ,  vertical[2]);
							//start,		stop,			step

	mapper->setLikelihoodParameters("gaussian", lklhdParam);
	((LinFilter*)mapper->getFilter())->setParameter(filterParam);

	mapper->generateDisparityMap();

	mapper->normalizeDisparityMap();
	
	clock_t finish = clock();

	printf("\n\tRunTime (ticks): %d\n\tTicks per second: %d\n", finish-start, CLOCKS_PER_SEC);
	
	/*	DepthTestDisplay *dtDisplay = new DepthTestDisplay();
		dtDisplay->setLeftImage(mapper->getLeftImage());
		dtDisplay->setRightImage(mapper->getRightImage());
	//	dtDisplay->setWarpImages(mapper->getWarpImages(), mapper->getImageCount());
	//	dtDisplay->setDiffImages(mapper->getDifferenceImages(), mapper->getImageCount());
	//	dtDisplay->setLikelihoodImages(mapper->getLikelihoodImages(), mapper->getImageCount());
		dtDisplay->setFilteredLikelihoodImages(mapper->getFilteredLikelihoodImages(), mapper->getImageCount());
		dtDisplay->setMaxDisparityImage(mapper->getMaxActivationMap()); 
		dtDisplay->setHorizontalDisparityMap(mapper->getHorizontalDisparityMap());
		dtDisplay->setVerticalDisparityMap(mapper->getVerticalDisparityMap());
		case 'm':
					//dtDisplay->rotateImagesRight();
					break;
				case 'n':
					//dtDisplay->rotateImagesLeft();
					break;
				case 'a':
					//dtDisplay->rotateUp();
					//dtDisplay->updateDisplay();
					break;
				case 'z':
					//dtDisplay->rotateDown();
					//dtDisplay->updateDisplay();
					break;
				case 'q':
					key = 27;
					break;

		dtDisplay->updateDisplay();*/


/*		cvNamedWindow("left");
		IplImage *left = cvCreateImage( cvGetSize(mapper->getLeftImage()), 32, 3);
		cvCvtColor(mapper->getLeftImage(), left, CV_GRAY2BGR);
		cvShowImage("left", left);
*/

		IplImage *map = mapper->getHorizontalDisparityMap();
		IplImage *mainImg = cvCloneImage(map);
		IplImage* img = cvCreateImage( cvGetSize(mainImg), 8, 1 );
		cvConvertScale(mainImg, img, 255, 0);
		cvSaveImage("map.bmp", img);
		//cvSmooth(img, img, CV_BLUR);//, 3, 3);

	if(show) {
		cvNamedWindow("main");
		cvShowImage("main", map);


	// post processing!

/*		
		IplImage* dst = cvCreateImage( cvGetSize(img), 8, 3 );
		CvMemStorage* storage = cvCreateMemStorage(0);
		CvSeq* contour = 0;
		cvThreshold( img, img, 190, 255, CV_THRESH_BINARY );

		cvFindContours( img, storage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
		cvZero( dst );

			for( ; contour != 0; contour = contour->h_next )
			{
				int x = ((CvPoint*)cvGetSeqElem(contour, 0))->x;
				int y = ((CvPoint*)cvGetSeqElem(contour, 0))->y;
				uchar pixel = ((uchar*)(img->imageData + img->widthStep * y))[x * img->nChannels];

				//CvScalar color = CV_RGB( pixel,0 , 0 );
				CvScalar color = CV_RGB( rand()&255, rand()&255 , rand()&255 );
				if(pixel < 50) {
					//printf("contourvalue: %d\n", pixel);
					cvDrawContours( left, contour, color, color, -1, 1); //CV_FILLED );
					cvDrawContours( dst, contour, color, color, -1, CV_FILLED );
					//cvDrawCircle(dst, *((CvPoint*)cvGetSeqElem(contour, 0)), 5, color, 2);
				}
			}

			cvNamedWindow( "Components", 1 );
			cvShowImage( "Components", dst );

		cvShowImage("left", left);

		cvNamedWindow("right");
		cvShowImage("right", mapper->getRightImage());

*/
		int key = 0;
		while(key != 27 && (key = cvWaitKey(0))) {
			if(key == 27) break;
			switch(key){
				case 27: break;
			}
		}

		//free(dtDisplay);
		free(mapper);
		cvDestroyAllWindows();
	}
	return 0;
}
