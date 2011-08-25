#include <stdio.h>

#include <cv.h>
#include <cvaux.h>
#include <highgui.h>

#include <yarp/sig/all.h>

using namespace yarp::sig;
using namespace yarp::sig::draw;
using namespace yarp::sig::file;


int main(int argc, char *argv[]) {
    ImageOf<PixelRgb> yarpImage;


    printf("Reading a YARP image (ppm)\n");
    read(yarpImage, "left_0.ppm");
    // addCircle(yarpImage,PixelRgb(255,0,0),
    //           yarpImage.width()/2,yarpImage.height()/2,
    //           yarpImage.height()/4);
    // addCircle(yarpImage,PixelRgb(255,50,50),
    //           yarpImage.width()/2,yarpImage.height()/2,
    //           yarpImage.height()/5);
    // 

    printf("Copying YARP image to an OpenCV/IPL image\n");
    IplImage *cvImage = cvCreateImage(cvSize(yarpImage.width(),  
                                             yarpImage.height()), 
                                             IPL_DEPTH_8U, 3 );
    cvCvtColor((IplImage*)yarpImage.getIplImage(), cvImage, CV_RGB2BGR);

    printf("Showing OpenCV/IPL image\n");
    // cvNamedWindow("test",1);
    // cvShowImage("test",cvImage);
    if( ! cvSaveImage("test.png", cvImage) )
        printf("Could not save cvFile (png).\n");

    printf("Taking image back into YARP...\n");
    ImageOf<PixelBgr> yarpReturnImage;
    yarpReturnImage.wrapIplImage(cvImage);
    yarp::sig::file::write(yarpReturnImage, "test.ppm");
    printf("Saving YARP image to test.ppm\n");


    // cvWaitKey(3000);

    // cvDestroyWindow("test");

    cvReleaseImage(&cvImage);

    printf("...done\n");
    return 0;
}