#ifndef DisplayMatching_H
#define DisplayMatching_H

#include "cv.h"
#include "highgui.h"
#include "cxcore.h"

#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>

//YARP
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>

#define DEBUGDISPLAYMATCHING 0

#define SAVEMATCHIMAGE 0
#define DISPLAYMATCHIMAGE 1

using namespace std;
using namespace cv;
using namespace yarp;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;


class DisplayMatching
{
	public:
		DisplayMatching(std::string name = "DisplayMatching", std::string input ="input", int x=0, int y=0);
		~DisplayMatching(); 

		void execute(); 

		void putImagesSideBySide(IplImage* result, const IplImage* img1, const IplImage* img2);

		void showKeypoints();

		void drawAPlusandLine(const int xL, const int yL, const int xR, const int yR);
		
		void createWindow();
		void destroyWindow();		
		void readfromports();
	  

	private:


		//class names
		string nameclass;
		string	nameinput;		
		
/*		READ IMAGE LEFT		*/
		string nameImageLeftInPort;
		string nameSenderImageLeftPort;
		BufferedPort<ImageOf<PixelRgb> > ReceivingImageLeftPort;
		IplImage* iplimageleftin;
		ImageOf<PixelRgb> *yarpimageleftin;
		bool receiveimageleft;

/*		READ IMAGE RIGHT	*/
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
		
/*		DISPLAY MATCHING	*/
		IplImage* sideBySideImage;		
		int posx;
		int posy;
		bool createwindow;		

                //Display in yarpview
                BufferedPort<ImageOf<PixelRgb> > DisplayLeftMatchYarpView;
                BufferedPort<ImageOf<PixelRgb> > DisplayRightMatchYarpView;
                BufferedPort<ImageOf<PixelRgb> > DisplayLeftRightMatchYarpView;
                ImageOf<PixelRgb> DisplayLeftMatchYarpImageOut;
                ImageOf<PixelRgb> DisplayRightMatchYarpImageOut;
                ImageOf<PixelRgb> DisplayLeftRightMatchYarpImageOut;
                string nameDisplayLeftMatchYarpView;
                string nameDisplayRightMatchYarpView;
                string nameDisplayLeftRightMatchYarpView;
                IplImage* iplimageinLeftyarpview;
                IplImage* iplimageinRightyarpview;
                IplImage* iplimageinLeftRightyarpview;
		
		#if SAVEMATCHIMAGE
			int number;
			char* svName1;
			char* svName2;
			char* svName3;			
			char saveName1[250];
			char saveName2[250];
			char saveName3[250];
                        char FolderImagePath[200];
                        char command[250];
		#endif		

		string nameleftmatchwindow;
		string namerightmatchwindow;
		string nameleftrightmatchwindow;			

		
		
		
};


#endif
