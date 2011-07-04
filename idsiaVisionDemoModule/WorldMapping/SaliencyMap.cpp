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

SaliencyMap::SaliencyMap()
{
  // TODO Auto-generated constructor stub

  //Setting channels and other stuffs

}

SaliencyMap::~SaliencyMap()
{
  // TODO Auto-generated destructor stub
}

int SaliencyMap::detectSaliencyPoint(Mat &imLeft, Mat &imRight, vector<KeyPoint> &keysLeft, vector<KeyPoint> &keysRight, vector<DMatch> &matches){

  move = false;

  cout<<imLeft.rows<<" "<<imLeft.cols<<endl;
  outputLeft = Mat::zeros(imLeft.rows, imLeft.cols, imLeft.type());
  outputRight = Mat::zeros(imRight.rows, imRight.cols, imRight.type());

  IplImage tmpImLeft = imLeft;
  IplImage tmpImRight = imRight;
  IplImage tmpImoutLeft = outputLeft;
  IplImage tmpImoutRight = outputRight;

  /*envision_nextframe((IplImage *)&imLeft, (IplImage *) &outputLeft, 0);
  envision_nextframe((IplImage *)&imRight, (IplImage *) &outputRight, 0);*/

  envision_nextframe(&tmpImLeft, &tmpImoutLeft, 0);
  envision_nextframe(&tmpImRight, &tmpImoutRight, 0);

  int * pointLeft = (int*)malloc(2*sizeof(int)*keysLeft.size());
  int * pointRight = (int*)malloc(2*sizeof(int)*keysRight.size());

/*
  int ii = 0;
  for(unsigned int i = 0; i<2*keysLeft.size(); i=i+2)
  {
      pointLeft[i] = cvRound(keysLeft[ii].pt.x);
      pointLeft[i+1] = cvRound(keysLeft[ii].pt.y);
      ii++;
  }

  ii=0;
  for(unsigned int i = 0; i<2*keysRight.size(); i=i+2)
  {
      pointRight[i] = cvRound(keysRight[ii].pt.x);
      pointRight[i+1] = cvRound(keysRight[ii].pt.y);
      ii++;
  }
*/

  int j = 0;
  for(unsigned int i = 0; i<matches.size(); i++)
  {
      pointLeft[j] = cvRound(keysLeft[matches[i].queryIdx].pt.x);
      pointRight[j] = cvRound(keysLeft[matches[i].trainIdx].pt.x);
      j++;
      pointLeft[j] = cvRound(keysLeft[matches[i].queryIdx].pt.y);
      pointRight[j] = cvRound(keysLeft[matches[i].trainIdx].pt.y);
      j++;
  }

  int matchindex = SC_subset_winner(pointLeft, keysLeft.size(), pointRight, keysRight.size());
  int *results=SC_naive_competition((IplImage *) &outputLeft, (IplImage *) &outputRight);


  //TO BE SURE THAT THE WINNER POINT IS NOT TOO CLOSE TO THE BORDER OF THE IMAGE (PROBLEM WHEN WE COMPUTE TEMPLATE FOR TRACKING)
  if(results[0]==0 || results[0]==1)
  {

            //SEND TO CUE INTEGRATION
            /*
            trackL.winnerPos = sal.pointOL;
            trackL.winnerSize.width = 50;
            trackL.winnerSize.height = 50;
            trackL.winnerRect.x = (sal.pointOL.x - 50);
            trackL.winnerRect.y = (sal.pointOL.y - 50);
            trackL.winnerRect.width = 50;
            trackL.winnerRect.height = 50;
            trackL.reliability = 1;

            trackR.winnerPos = sal.pointOR;
            trackR.winnerSize.width = 50;
            trackR.winnerSize.height = 50;
            trackR.winnerRect.x = (sal.pointOR.x - 50);
            trackR.winnerRect.y = (sal.pointOR.y - 50);
            trackR.winnerRect.width = 50;
            trackR.winnerRect.height = 50;
            trackR.reliability = 1;
            */

            move = true;
    }
  else
    matchindex = -1;

  delete pointLeft;
  delete pointRight;

  return matchindex;
}

