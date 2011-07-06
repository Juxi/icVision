/*
 * GaborDescriptor.h
 *
 *  Created on: Jul 1, 2011
 *      Author: icub
 */

#ifndef GABORDESCRIPTOR_H_
#define GABORDESCRIPTOR_H_

#define DEBUGGABOR false

#include <opencv2/opencv.hpp>
#include "GaborFilter.h"
#include "convertflt.h"

class GaborDescriptor {
private:
  /*		COMPUTE GABORDESCRIPTOR	*/
  int m_offsetx, m_offsety, m_gap;
  GaborFilter* mp_GaborFilter;
  bool m_norm;
  IplImage* mp_rescaledimg;
  //TODO Create this class
  //CSCDescriptors m_descriptor;
  //TODO POTREBBE ESSERE SENZA PUNTATORE
  unsigned int m_L, m_D;
  float m_f, m_kmax, m_sigma, m_N;
  std::vector<int> mvec_filtersize;
  bool m_init;
  CvSize m_imagesize;
  CvSize m_biggerimagesize;
  IplImage* mp_cvgrayimgflt;
  ConvertFlt m_ConvertFlt;


  CvRect m_outputrect;

  void compute(std::vector<cv::KeyPoint> KeyPoints, CvMat*  m_descriptor);
  bool resize(IplImage* iplgrimagein, IplImage* mp_cvimage, CvSize* mp_fixedOutputSize, bool m_addnoise);

public:
  GaborDescriptor();
  virtual ~GaborDescriptor();
  bool detect(vector<KeyPoint> &keypoint, IplImage* iplgrimagein, Mat& gaborDescr);
  void setParam(unsigned int L=5, unsigned int D=8, float f=1.0/sqrt(2.0), float kmax=1.7, float sigma=2*3.1416, float N=1.0, bool norm = true);
  void normalize(CvMat*  m_descriptor);

};

#endif /* GABORDESCRIPTOR_H_ */
