/*
 * ColoredObjectDetector.h
 *
 *  Created on: May 25, 2011
 *      Author: Davide Migliore
 * 	GPL goes here...
 */

#ifndef COLOREDOBJECTDETECTOR_H_
#define COLOREDOBJECTDETECTOR_H_

#define COLORSTATFILENAME "colorMatrices.yaml"

#include "ObjectDetector.h"
#include "WorldObject.h"


class ColoredObjectDetector: public ObjectDetector
{
  private:
  /*Colors Statistics*/
    vector<Mat> colorsStatMean, colorsStatICov;
    bool loadColorStats(string filename);

  public:
    ColoredObjectDetector(string );
    virtual ~ColoredObjectDetector();
    int objectDetection(Mat &image, Mat &outputImage, vector<ColoredRect> &objects);
    void colorObjectDetection(Mat &image, Mat& outputMask, vector<ColoredRect> &listofrect, Mat& outputImage);
    void colorSegmentation(Mat &image, Mat &m, Mat &icov, Mat &binaryImage, float threshold = 0.035);
};

#endif /* COLOREDOBJECTDETECTOR_H_ */
