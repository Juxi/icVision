/*
 * ObjectDetector.h
 *
 *  Created on: May 25, 2011
 *      Author: Davide Migliore
 * 	GPL goes here...
 */

#ifndef OBJECTDETECTOR_H_
#define OBJECTDETECTOR_H_

#include <opencv2/opencv.hpp>
#include "ColoredRect.h"
#include <string>

using namespace std;
using namespace cv;

class ObjectDetector
{
  protected:
    string cameraFlag;

  public:
    ObjectDetector();
    ObjectDetector(string camera);

    virtual ~ObjectDetector();
    void objectDetection();

};

#endif /* OBJECTDETECTOR_H_ */
