/*
 * SaliencyMap.cpp
 *
 *  Created on: Jul 4, 2011
 *      Author: Davide Migliore
 * 	GPL goes here...
 */

#include "SaliencyMap.h"

using namespace std;
using namespace cv;
using namespace yarp;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

SaliencyMap::SaliencyMap()
{
  // TODO Auto-generated constructor stub
  namePointLeftOutPort = "/SaliencyMap/left/point:o";
  namePointRightOutPort = "/SaliencyMap/right/point:o";

  //Input image port
  if(! PointLeftOutPort.open( namePointLeftOutPort.c_str() )){
      cerr<<"Unable to open port "+namePointLeftOutPort<<endl;
  }
  cout<<"Opened port "+namePointLeftOutPort<<endl;

  //Output image port
  if(! PointRightOutPort.open( namePointRightOutPort.c_str() )){
      cerr<<"Unable to open port "+namePointRightOutPort<<endl;
  }
  cout<<"Opened port "+namePointRightOutPort<<endl;

}

SaliencyMap::~SaliencyMap()
{
  // TODO Auto-generated destructor stub
  close();
}

int SaliencyMap::detectSaliencyPoint(Mat &imLeft, Mat &imRight, vector<KeyPoint> &keysLeft, vector<KeyPoint> &keysRight, vector<DMatch> &matches){
  move = false;

  outputLeft = Mat::zeros(imLeft.rows, imLeft.cols, imLeft.type());
  outputRight = Mat::zeros(imRight.rows, imRight.cols, imRight.type());

  IplImage tmpImLeft = imLeft;
  IplImage tmpImRight = imRight;
  IplImage tmpImoutLeft = outputLeft;
  IplImage tmpImoutRight = outputRight;

  /*envision_nextframe((IplImage *)&imLeft, (IplImage *) &outputLeft, 0);
  envision_nextframe((IplImage *)&imRight, (IplImage *) &outputRight, 0);*/

  envision_nextframe(&tmpImLeft, &tmpImoutLeft, 0);
  envision_nextframe(&tmpImRight, &tmpImoutRight, 1);
  int *pointLeft, *pointRight;

  if(matches.size()>0){
      pointLeft = (int*)malloc(2*sizeof(int)*matches.size());
      pointRight = (int*)malloc(2*sizeof(int)*matches.size());

      int j = 0;
      for(unsigned int i = 0; i<matches.size(); i++)
        {
          pointLeft[j] = cvRound(keysLeft[matches[i].queryIdx].pt.x);
          pointRight[j] = cvRound(keysRight[matches[i].trainIdx].pt.x);
          j++;
          pointLeft[j] = cvRound(keysLeft[matches[i].queryIdx].pt.y);
          pointRight[j] = cvRound(keysRight[matches[i].trainIdx].pt.y);
          j++;
        }
  }

  //initialize the values
  int matchindex = -1;
  int *results = NULL;
  int posxLe = 0;
  int posxRi = 0;
  int posyLe = 0;
  int posyRi = 0;

  if(matches.size()>0){
      matchindex = SC_subset_winner(pointLeft, 2*matches.size(), pointRight, 2*matches.size());
      results=SC_naive_competition(&tmpImoutLeft, &tmpImoutRight);
  }


  //TO BE SURE THAT THE WINNER POINT IS NOT TOO CLOSE TO THE BORDER OF THE IMAGE (PROBLEM WHEN WE COMPUTE TEMPLATE FOR TRACKING)
  if(results != NULL){
      if(results[0]==0 || results[0]==1)
        {
          move = true;
          if(results[0]==0)
            {
              std::cout<<"Winner found in the left eye"<<endl;
              posxLe = cvRound(keysLeft[matches[matchindex].queryIdx].pt.x);
              posyLe = cvRound(keysLeft[matches[matchindex].queryIdx].pt.y);
              posxRi = cvRound(keysRight[matches[matchindex].trainIdx].pt.x);
              posyRi = cvRound(keysRight[matches[matchindex].trainIdx].pt.y);

            }
          else if(results[0]==1)
            {
              std::cout<<"Winner found in the right eye"<<endl;
              posxLe = cvRound(keysLeft[matches[matchindex].queryIdx].pt.x);
              posyLe = cvRound(keysLeft[matches[matchindex].queryIdx].pt.y);
              posxRi = cvRound(keysRight[matches[matchindex].trainIdx].pt.x);
              posyRi = cvRound(keysRight[matches[matchindex].trainIdx].pt.y);
            }

        }
  }

  //DEBUG STUFF
  std::cout<<"matchindex = "<<matchindex<<endl;
  std::cout<<"Winner Location Left = ["<<posxLe<<","<<posyLe<<"]"<<endl;
  std::cout<<"Winner Location Right = ["<<posxRi<<","<<posyRi<<"]"<<endl;

  //SENDING POINTS COORDINATES
  Bottle& PntLOut = PointLeftOutPort.prepare();
  Bottle& PntROut = PointRightOutPort.prepare();
  PntLOut.clear();
  PntROut.clear();
  PntLOut.addInt(posxLe);
  PntLOut.addInt(posyLe);
  PntROut.addInt(posxRi);
  PntROut.addInt(posyRi);
  PointLeftOutPort.write();
  PointRightOutPort.write();

  if(matches.size()>0){
      free(pointLeft);
      free(pointRight);
  }
  return matchindex;
}

bool SaliencyMap::interrupt(){
  PointLeftOutPort.interrupt();
  PointRightOutPort.interrupt();
}

bool SaliencyMap::close(){
  interrupt();
  PointLeftOutPort.close();
  PointRightOutPort.close();
}
