#include "GaborFilter.h"


GaborFilter::GaborFilter(std::string name, std::string side, std::string input) {

  Network yarp; 						//	set up yarp

  namesideclass = side;				//	"left or right" (suppose "left")
  nameclass =name + "_" + side;		//	"GaborFilter_left"
  nameinput = input;					//	"Gabor_left"

  std::cout<<"\nConstructing " << nameclass << "...\n";

  /*	COMPUTE GABORFILTER	*/

  //TODO RELEASE IN DECONSTRUCTOR
  mdp_filtersreal = NULL; //IplImage** (before CVImage**)
  mdp_filtersimag = NULL;	//IplImage** (before CVImage**)

  m_windowcreated = false;

  mp_displayreal = NULL;
  mp_displayimag = NULL;

  mdp_responsesreal = NULL;
  mdp_responsesimag = NULL;

  mp_result = NULL;

  m_computemode = ABSOLUTE;

  std::cout<<"finished.\n";

}

GaborFilter::~GaborFilter() 
{

  std::cout<<"\nDestructing " << nameclass << "...";

  /*	COMPUTE GABORFILTER	*/

  if(m_windowcreated) {
      cvDestroyWindow( m_windownamereal.c_str() );
      cvDestroyWindow( m_windownameimag.c_str() );
  }

  if(mp_displayreal) cvReleaseImage(&mp_displayreal);
  if(mp_displayimag) cvReleaseImage(&mp_displayimag);

  if(mdp_responsesreal) {
      for(unsigned int i = 0;i<m_L;++i) cvReleaseImage(&(mdp_responsesreal[i]));
      delete [] mdp_responsesreal;
  }
  if(mdp_responsesimag) {
      for(unsigned int i = 0;i<m_L;++i) cvReleaseImage(&(mdp_responsesimag[i]));
      delete [] mdp_responsesimag;
  }

  if(mp_result) delete [] mp_result;

  std::cout<<"finished.\n";

}

void GaborFilter::setParam(unsigned int L, unsigned int D, float f, float kmax, float sigma, float N)
{

  m_L = L;
  m_D = D;
  m_LD = L*D;
  m_f = f;
  m_kmax = kmax;
  m_sigma = sigma;
  m_N = N;


  mvec_filtersize_filter.clear();
  mvec_filtersize_filter.push_back(55);
  mvec_filtersize_filter.push_back(43);
  mvec_filtersize_filter.push_back(35);
  mvec_filtersize_filter.push_back(25);
  mvec_filtersize_filter.push_back(19);


  if(mvec_filtersize_filter.size() != m_L)
    {
      std::cerr <<"\n"<< nameclass << "::setParam::ERROR::number of filtersizes do not match L\n";
      return;
    }

  generatefilters();


}

void GaborFilter::generatefilters() {

#if DEBUGGABORFILTER
  std::cout<<"\n"<< nameclass << "::generatefilters";
#endif

  if(mdp_filtersreal) {	//IplImage** (before CVImage**)
      for(unsigned int i = 0;i<m_LD;++i)  cvReleaseImage(&(mdp_filtersreal[i]));
      delete [] mdp_filtersreal;
  }

  if(mdp_filtersimag) {	//IplImage** (before CVImage**)
      for(unsigned int i = 0;i<m_LD;++i) cvReleaseImage(&(mdp_filtersimag[i]));
      delete [] mdp_filtersimag;
  }

  mdp_filtersreal = new IplImage* [m_LD];	//IplImage** (before CVImage**)
  mdp_filtersimag = new IplImage* [m_LD];	//IplImage** (before CVImage**)



  for(unsigned int i = 0; i < m_L; i++) {
      int size = mvec_filtersize_filter[i];
      for(unsigned int j = 0; j < m_D; j++) {
          unsigned int tempindex = i*m_D+j;

          CvSize filtersize = cvSize(size, size);

          mdp_filtersreal[tempindex] = cvCreateImage(filtersize,IPL_DEPTH_32F, 1);
          mdp_filtersimag[tempindex] = cvCreateImage(filtersize,IPL_DEPTH_32F, 1);
      }
  }

#if DEBUGGABORFILTER
  std::cout<<"\n"<< nameclass << "::generatefilter mpd_filtersreal = "<<mdp_filtersreal[0]->height<<", "<<mdp_filtersreal[0]->width<<"\n";
  std::cout<<"\n"<< nameclass << "::generatefilter mpd_filtersimag = "<<mdp_filtersimag[0]->height<<", "<<mdp_filtersimag[0]->width<<"\n";

  std::cout<<"\n"<< nameclass << "::generatefilter mpd_filtersreal = "<<mdp_filtersreal[8]->height<<", "<<mdp_filtersreal[8]->width<<"\n";
  std::cout<<"\n"<< nameclass << "::generatefilter mpd_filtersimag = "<<mdp_filtersimag[8]->height<<", "<<mdp_filtersimag[8]->width<<"\n";

  std::cout<<"\n"<< nameclass << "::generatefilter mpd_filtersreal = "<<mdp_filtersreal[16]->height<<", "<<mdp_filtersreal[16]->width<<"\n";
  std::cout<<"\n"<< nameclass << "::generatefilter mpd_filtersimag = "<<mdp_filtersimag[16]->height<<", "<<mdp_filtersimag[16]->width<<"\n";

  std::cout<<"\n"<< nameclass << "::generatefilter mpd_filtersreal = "<<mdp_filtersreal[24]->height<<", "<<mdp_filtersreal[24]->width<<"\n";
  std::cout<<"\n"<< nameclass << "::generatefilter mpd_filtersimag = "<<mdp_filtersimag[24]->height<<", "<<mdp_filtersimag[24]->width<<"\n";

  std::cout<<"\n"<< nameclass << "::generatefilter mpd_filtersreal = "<<mdp_filtersreal[32]->height<<", "<<mdp_filtersreal[32]->width<<"\n";
  std::cout<<"\n"<< nameclass << "::generatefilter mpd_filtersimag = "<<mdp_filtersimag[32]->height<<", "<<mdp_filtersimag[32]->width<<"\n";

  //Window for gabor filter
  cvNamedWindow("filterreal",1);
  cvNamedWindow("filterimag",1);
#endif


  int count = 0;
  float kv, phimu, sigma2 = m_sigma*m_sigma;
  float kv1, kv2, k2, xx, xy, x2, kx;
  float temp3, temp2 = exp(-sigma2/2.0);
  int v2;
  for(unsigned int v = 0;v<m_L;v++){ // 0 ~ L-1
      v2 = m_L - v - 1;
      int size = mvec_filtersize_filter[v];
      for(unsigned int mu = 0;mu<m_D;mu++){ // 0 ~ D-1
          phimu = (float)mu*3.1416 / (float)m_D;
          kv = 3.1416 * (float)pow((double)2.0, (double)-((m_f * v2+2.0)/2.0));  // Erik's CHANGE!!
          kv1 = cos(phimu) * kv;
          kv2 = sin(phimu) * kv;
          k2 = kv1*kv1 + kv2*kv2;

          IplImage* filterreal = mdp_filtersreal[count];
          IplImage* filterimag = mdp_filtersimag[count];
          //			IplImage* filterreal = cvCreateImage( cvSize(mdp_filtersreal[count]->cols,mdp_filtersreal[count]->rows), IPL_DEPTH_32F, 1);
          //			IplImage* filterimag = cvCreateImage( cvSize(mdp_filtersimag[count]->cols,mdp_filtersimag[count]->rows), IPL_DEPTH_32F, 1);

          for(int j=0;j<size;j++){
              for(int i=0;i<size;i++){

                  xx = ((float)i-((float)size-1.0)/2.0)/m_N;
                  xy = ((float)j-((float)size-1.0)/2.0)/m_N;
                  x2 = xx*xx + xy*xy;
                  kx = kv1*xx + kv2*xy;
                  temp3 = k2/sigma2*exp(-k2*x2/(2.0*sigma2));
                  double val1 = temp3 * (cos(kx) - temp2);
                  double val2 = temp3 * sin(kx);

                  cvSetReal2D( filterreal, j, i, val1 );
                  cvSetReal2D( filterimag, j, i, val2 );
              }
          }
          count++;

#if DEBUGGABORFILTER
          std::cout<<"\n"<< nameclass << "::filterreal size= "<<filterreal->height<<", "<<filterreal->width<<"\n";
          std::cout<<"\n"<< nameclass << "::filterimag size= "<<filterimag->height<<", "<<filterimag->width<<"\n";
          std::cout<<"\n"<< nameclass << "::filterreal depth= "<<filterreal->depth<<"\n";
          std::cout<<"\n"<< nameclass << "::filterimag depth= "<<filterimag->depth<<"\n";
          std::cout<<"\n"<< nameclass << "::filterreal channel= "<<filterreal->nChannels<<"\n";
          std::cout<<"\n"<< nameclass << "::filterimag channel= "<<filterimag->nChannels<<"\n";
          cvShowImage("filterreal",filterreal);
          cvShowImage("filterimag",filterimag);
          //cvWaitKey(0);
#endif

      }
  }


}

float* GaborFilter::compute(IplImage* img, CvPoint* pos, float* dst) {

#if DEBUGGABORFILTER
  std::cout<<"\n"<< nameclass << "::compute";
#endif

  if(!img || !pos) { std::cerr <<"\n"<< nameclass <<"::compute::ERROR::One of input is NULL!\n"; return NULL; }

#if DEBUGGABORFILTER
  std::cout<<"\n"<< nameclass << "::execute::img->depth = " << img->depth << " = IPL_DEPTH_32F\n";
  std::cout<<"\n"<< nameclass << "::execute::img->nChannels = " << img->nChannels << "\n";
  //cvWaitKey(0);
#endif

  IplImage* imgflt = NULL;	//(CVImage*)
  if(img->depth == IPL_DEPTH_8U) {

#if DEBUGGABORFILTER
      std::cout<<"\n"<< nameclass << "::execute::convert to Flt\n";
      //cvWaitKey(0);
#endif

      imgflt = m_ConvertFlt.convert(img);
  }
  else if(img->depth == IPL_DEPTH_32F) {

#if DEBUGGABORFILTER
      std::cout<<"\n"<< nameclass << "::execute::not convert to Flt\n";
      //cvWaitKey(0);
#endif

      imgflt = img;
  }
  else {
      std::cerr <<"\n"<< nameclass <<"::conv2DPointImag::ERROR::Unsupported img type!\n"; return NULL;
  }

  if(!dst) { std::cerr <<"\n"<< nameclass <<"::compute::ERROR::One of input is NULL!\n"; return NULL; }
  float* result = dst;

  for(unsigned int i = 0;i<m_LD;++i) {
      float tempreal = conv2DPointReal(imgflt, pos, i);
      float tempimag = conv2DPointImag(imgflt, pos, i);
      if(m_computemode == ABSOLUTE) {
#if DEBUGGABORFILTER
          std::cout<<"\n"<< nameclass << "::compute::computemode ABSOLUTE\n";
          //cvWaitKey(0);
#endif
          result[i] = sqrt(pow(tempreal, 2.0f)+pow(tempimag, 2.0f));
      }
      else if(m_computemode == COMPLEX) {
          result[i*2+0] = tempreal;
          result[i*2+1] = tempimag;
      }
      else  {
          result[i*2+0] = sqrt(pow(tempreal, 2.0f)+pow(tempimag, 2.0f));
          result[i*2+1] = atan2(tempimag, tempreal);
      }


#if DEBUGGABORFILTER
      std::cout<<"\n"<< nameclass << "::compute result["<<i<<"] = "<<result[i]<<"\n";
      //cvWaitKey(0);
#endif

  }


#if DEBUGGABORFILTER
  std::cout<<"\n"<< nameclass << "::compute result[38] = "<<result[38]<<"\n";
  //cvWaitKey(0);
#endif

  return result;
}

float GaborFilter::conv2DPointReal(IplImage* img, CvPoint* pos, unsigned int index) {
#if DEBUGGABORFILTER
  std::cout<<"\n"<< nameclass << "::conv2DPointReal";
#endif

  float result = 0.0;

  if(!img || !pos) {  std::cerr <<"\n"<< nameclass <<"::conv2DPointReal::ERROR::One of input is NULL!\n"; return result; }

  if(index >= m_LD) {  std::cerr <<"\n"<< nameclass <<"::conv2DPointReal::ERROR::index out of range!\n"; return result; }

  IplImage* imgflt;
  if(img->depth == IPL_DEPTH_8U) {
      imgflt = m_ConvertFlt.convert(img);
  }
  else if(img->depth == IPL_DEPTH_32F) {
      imgflt = img;
  }
  else {
      std::cerr <<"\n"<< nameclass <<"::conv2DPointReal::ERROR::Unsupported img type!\n"; return NULL;
  }

  if(!mdp_responsesreal) {
      mdp_responsesreal = new IplImage* [m_L];
      for(unsigned int i = 0; i < m_L; i++) {
          int size = mvec_filtersize_filter[i];
          CvSize filtersize = cvSize(size, size);
          mdp_responsesreal[i] = cvCreateImage(filtersize, IPL_DEPTH_32F, 1);
      }
  }

  unsigned int L = cvFloor(index / m_D);

  IplImage* input = imgflt;
  IplImage* filter = mdp_filtersreal[index];
  IplImage* response = mdp_responsesreal[L];

#if DEBUGGABORFILTER
  std::cout<<"\n"<< nameclass << "::conv2DPointReal index = "<<index<<"\n";
  std::cout<<"\n"<< nameclass << "::conv2DPointReal L = "<<L<<"\n";
  std::cout<<"\n"<< nameclass << "::conv2DPointReal input size = "<<input->height<<", "<<input->width<<"\n";
  std::cout<<"\n"<< nameclass << "::conv2DPointReal filter size = "<<filter->height<<", "<<filter->width<<"\n";
  std::cout<<"\n"<< nameclass << "::conv2DPointReal response size = "<<response->height<<", "<<response->width<<"\n";
  //cvWaitKey(0);
#endif

  CvRect inputrect;
  int size = mvec_filtersize_filter[L];
  int posx = pos->x - cvRound((size-1)/2);
  int posy = pos->y - cvRound((size-1)/2);
  int endx = posx + size;
  int endy = posy + size;
  inputrect.x = posx > 0 ? posx : 0;
  inputrect.y = posy > 0 ? posy : 0;

#if DEBUGGABORFILTER
  std::cout<<"\n"<< nameclass << "::conv2DPointReal posx = "<<posx<<"\n";
  std::cout<<"\n"<< nameclass << "::conv2DPointReal posy = "<<posy<<"\n";
  std::cout<<"\n"<< nameclass << "::conv2DPointReal endx = "<<endx<<"\n";
  std::cout<<"\n"<< nameclass << "::conv2DPointReal endy = "<<endy<<"\n";
  std::cout<<"\n"<< nameclass << "::conv2DPointReal inputrect.x = "<<inputrect.x<<"\n";
  std::cout<<"\n"<< nameclass << "::conv2DPointReal inputrect.y = "<<inputrect.y<<"\n";
  //cvWaitKey(0);
#endif


  if(endx >= input->width) {
      int diff = endx - input->width + 1;
      inputrect.x -= diff;
  }
  if(endy >= input->height) {
      int diff = endy - input->height + 1;
      inputrect.y -= diff;
  }

  inputrect.width = size;
  inputrect.height = size;

  cvSetImageROI(input, inputrect);

  cvMul(input, filter, response, 1.0);

  cvResetImageROI(input);

  CvScalar temp = cvSum(response);

  result = (float)(temp.val[0]);

#if DEBUGGABORFILTER
  std::cout<<"\n"<< nameclass << "::conv2DPointReal result = "<<result<<"\n";
  //cvWaitKey(0);
#endif

  return result;

}

float GaborFilter::conv2DPointImag(IplImage* img, CvPoint* pos, unsigned int index) {

#if DEBUGGABORFILTER
  std::cout<<"\n"<< nameclass << "::conv2DPointImag";
#endif

  float result = 0.0;

  if(!img || !pos) { std::cerr <<"\n"<< nameclass <<"::conv2DPointImag::ERROR::One of input is NULL!\n"; return result; }

  if(index >= m_LD) { std::cerr <<"\n"<< nameclass <<"::conv2DPointImag::ERROR::index out of range!\n"; return result; }

  IplImage* imgflt;
  if(img->depth == IPL_DEPTH_8U) {
      imgflt = m_ConvertFlt.convert(img);
  }
  else if(img->depth == IPL_DEPTH_32F) {
      imgflt = img;
  }
  else {
      std::cerr <<"\n"<< nameclass <<"::conv2DPointImag::ERROR::Unsupported img type!\n"; return NULL;
  }

  if(!mdp_responsesimag) {
      mdp_responsesimag = new IplImage* [m_L];
      for(unsigned int i = 0; i < m_L; i++) {
          int size = mvec_filtersize_filter[i];
          CvSize filtersize = cvSize(size, size);
          mdp_responsesimag[i] = cvCreateImage(filtersize, IPL_DEPTH_32F, 1);
      }
  }

  unsigned int L = cvFloor(index / m_D);

  IplImage* input = imgflt;
  IplImage* filter = mdp_filtersimag[index];
  IplImage* response = mdp_responsesimag[L];
#if DEBUGGABORFILTER
  std::cout<<"\n"<< nameclass << "::conv2DPointImag index = "<<index<<"\n";
  std::cout<<"\n"<< nameclass << "::conv2DPointImag filter imag size = "<<filter->height<<", "<<filter->width<<"\n";
  //cvWaitKey(0);
#endif

  CvRect inputrect;
  int size = mvec_filtersize_filter[L];
  int posx = pos->x - cvRound((size-1)/2);
  int posy = pos->y - cvRound((size-1)/2);
  int endx = posx + size;
  int endy = posy + size;
  inputrect.x = posx > 0 ? posx : 0;
  inputrect.y = posy > 0 ? posy : 0;

  if(endx >= input->width) {
      int diff = endx - input->width + 1;
      inputrect.x -= diff;
  }
  if(endy >= input->height) {
      int diff = endy - input->height + 1;
      inputrect.y -= diff;
  }

  inputrect.width = size;
  inputrect.height = size;

  cvSetImageROI(input, inputrect);

  cvMul(input, filter, response, 1.0);

  cvResetImageROI(input);

  CvScalar temp = cvSum(response);

  result = (float)(temp.val[0]);

#if DEBUGGABORFILTER
  std::cout<<"\n"<< nameclass << "::conv2DPointImag result = "<<result<<"\n";
  //cvWaitKey(0);
#endif

  return result;
}

