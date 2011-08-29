#include <QtCore>
#include <QCoreApplication>

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

bool readStringList(const string& filename, vector<string>& l);
bool generateStatMatrix(vector<string> &filelist, Mat& meanMatrix, Mat& covMatrix);
bool generateStatMatrixYCrCb(vector<string> &filelist, Mat& meanMatrix, Mat& covMatrix);

void testclassification(Mat &imagetest, Mat &m, Mat &cov);
void testclassificationYCrCb(Mat &imagetest, Mat &m, Mat &cov);


int main(int argc, char *argv[])
{
	
    vector<string> filelistRed, filelistGreen, filelistBlue, currentfilelist;
    readStringList("blueSimTraining.yaml", filelistBlue);
    readStringList("greenTraining.yaml", filelistGreen);
    readStringList("redSimTraining.yaml", filelistRed);

    Mat blueMeanMatrix, blueCovMatrix;
    Mat greenMeanMatrix, greenCovMatrix;
    Mat redMeanMatrix, redCovMatrix;

    vector<Mat> colorMatrix;

    //generateStatMatrixYCrCb(filelistBlue, blueMeanMatrix, blueCovMatrix);
    generateStatMatrix(filelistBlue, blueMeanMatrix, blueCovMatrix);
    generateStatMatrix(filelistGreen, greenMeanMatrix, greenCovMatrix);
    generateStatMatrix(filelistRed, redMeanMatrix, redCovMatrix);

    colorMatrix.push_back(redMeanMatrix);
    colorMatrix.push_back(redCovMatrix);
    colorMatrix.push_back(greenMeanMatrix);
    colorMatrix.push_back(greenCovMatrix);
    colorMatrix.push_back(blueMeanMatrix);
    colorMatrix.push_back(blueCovMatrix);

    FileStorage fs("colorSimMatrices.yaml", CV_STORAGE_WRITE);
    if (!fs.isOpened())
        return false;

    char numberColor[4];

    int dim = colorMatrix.size()/2;
    fs<<"NumberOfColors"<<dim;

    for(int i=0; i<colorMatrix.size(); i+=2){
        stringstream meanName, covName;
        sprintf(numberColor, "%.04d", i/2);
        meanName<<"meanMatrix"<<numberColor;
        covName<<"covMatrix"<<numberColor;


        fs << meanName.str()<< colorMatrix[i];
        fs << covName.str()<< colorMatrix[i+1];
    }
    fs.release();

    Mat imagetest = imread("/home/migliore/Desktop/tmp/ColorTrainingSet/SimulatedObject/im_000023.ppm", CV_LOAD_IMAGE_ANYCOLOR);

    namedWindow("TestImage");

    imshow("TestImage", imagetest);

//    testclassificationYCrCb(imagetest, blueMeanMatrix, blueCovMatrix);
   // testclassification(imagetest, blueMeanMatrix, blueCovMatrix);
    testclassification(imagetest, redMeanMatrix, redCovMatrix);

    return true;
}


bool readStringList(const string& filename, vector<string>& l)
{
  l.resize(0);
  FileStorage fs(filename, FileStorage::READ);
  if (!fs.isOpened())
    return false;
  FileNode n = fs.getFirstTopLevelNode();
  if (n.type() != FileNode::SEQ)
    return false;
  FileNodeIterator it = n.begin(), it_end = n.end();
  for (; it != it_end; ++it)
    l.push_back((string)*it);
  return true;
}


bool generateStatMatrix(vector<string> &filelist, Mat& meanMatrix, Mat& covMatrix){

  Mat imageBGR, imageHSV;
  vector<Mat> listOfPoints;

  for (uint i = 0; i< filelist.size(); i++){

       imageBGR = imread(filelist[i], CV_LOAD_IMAGE_ANYCOLOR);

       cvtColor(imageBGR, imageHSV, CV_BGR2HSV_FULL);

       for(int x = 0; x <imageHSV.cols; x++){
           for(int y = 0; y<imageHSV.rows; y++){
               Mat pixel = Mat::zeros(3,1,CV_64FC1);
               pixel.at<double>(0,0) = imageHSV.at<Vec3b>(y,x)[0];
               pixel.at<double>(1,0) = imageHSV.at<Vec3b>(y,x)[1];
               pixel.at<double>(2,0) = imageHSV.at<Vec3b>(y,x)[2];

               listOfPoints.push_back(pixel);

               //Add illumination change
               if(  pixel.at<double>(2,0) - 5 > 0){
                   pixel.at<double>(2,0)-= 5;
                   listOfPoints.push_back(pixel);

               }
               if(  pixel.at<double>(2,0) + 5 < 255){
                   pixel.at<double>(2,0)+= 5;
                   listOfPoints.push_back(pixel);
               }



           }
       }



  }

  //calculate covariance and mean matrices
  calcCovarMatrix((Mat*)listOfPoints.data(), (int)listOfPoints.size(), covMatrix, meanMatrix, CV_COVAR_NORMAL, CV_64F);
  listOfPoints.clear();
}


bool generateStatMatrixYCrCb(vector<string> &filelist, Mat& meanMatrix, Mat& covMatrix){

  Mat imageBGR, imageYCrCb;
  vector<Mat> listOfPoints;

  for (uint i = 0; i< filelist.size(); i++){

       imageBGR = imread(filelist[i], CV_LOAD_IMAGE_ANYCOLOR);

       cvtColor(imageBGR, imageYCrCb, CV_BGR2YCrCb);

       for(int x = 0; x <imageYCrCb.cols; x++){
           for(int y = 0; y<imageYCrCb.rows; y++){
               Mat pixel = Mat::zeros(3,1,CV_64FC1);
               pixel.at<double>(0,0) = imageYCrCb.at<Vec3b>(y,x)[0];
               pixel.at<double>(1,0) = imageYCrCb.at<Vec3b>(y,x)[1];
               pixel.at<double>(2,0) = imageYCrCb.at<Vec3b>(y,x)[2];

               listOfPoints.push_back(pixel);

               //Add illumination change
               if(  pixel.at<double>(2,0) - 5 > 0){
                   pixel.at<double>(2,0)-= 5;
                   listOfPoints.push_back(pixel);

               }
               if(  pixel.at<double>(2,0) + 5 < 255){
                   pixel.at<double>(2,0)+= 5;
                   listOfPoints.push_back(pixel);
               }



           }
       }



  }

  //calculate covariance and mean matrices
  calcCovarMatrix((Mat*)listOfPoints.data(), (int)listOfPoints.size(), covMatrix, meanMatrix, CV_COVAR_NORMAL, CV_64F);
  listOfPoints.clear();
}


void testclassification(Mat &imagetest, Mat &m, Mat &cov){

  Mat icov = cov.inv();


  cvtColor(imagetest, imagetest, CV_BGR2HSV_FULL);

  Mat resultingImage = Mat::zeros(imagetest.rows, imagetest.cols, CV_64FC1);
  for(unsigned int r = 0; r<imagetest.rows; r++){
         for(unsigned int c = 0; c<imagetest.cols; c++){

             Mat color2test = (Mat_<double>(3,1) << (int)imagetest.at<Vec3b>(r,c)[0],(int)imagetest.at<Vec3b>(r,c)[1],(int)imagetest.at<Vec3b>(r,c)[2]);

             resultingImage.at<double>(r,c) = Mahalanobis(color2test, m, icov);
         }
  }

  //cout<<resultingImage<<endl;
  Mat dst;
  compare(resultingImage, 0.06, dst, CMP_LE);


  Mat element = getStructuringElement(MORPH_ELLIPSE, Size(3, 3) );
  morphologyEx(dst, dst, CV_MOP_ERODE, element);
  morphologyEx(dst, dst, CV_MOP_ERODE, element);
  morphologyEx(dst, dst, CV_MOP_ERODE, element);
  morphologyEx(dst, dst, CV_MOP_DILATE, element);
  morphologyEx(dst, dst, CV_MOP_DILATE, element);
  morphologyEx(dst, dst, CV_MOP_DILATE, element);

  namedWindow("Sticazzi");
  imshow("Sticazzi", dst);
  waitKey();
}


void testclassificationYCrCb(Mat &imagetest, Mat &m, Mat &cov){
  Mat icov = cov.inv();


   cvtColor(imagetest, imagetest, CV_BGR2YCrCb);

   Mat resultingImage = Mat::zeros(imagetest.rows, imagetest.cols, CV_64FC1);
   for(unsigned int r = 0; r<imagetest.rows; r++){
          for(unsigned int c = 0; c<imagetest.cols; c++){

              Mat color2test = (Mat_<double>(3,1) << (int)imagetest.at<Vec3b>(r,c)[0],(int)imagetest.at<Vec3b>(r,c)[1],(int)imagetest.at<Vec3b>(r,c)[2]);

              resultingImage.at<double>(r,c) = Mahalanobis(color2test, m, icov);
          }
   }

   //cout<<resultingImage<<endl;
   Mat dst;
   compare(resultingImage, 0.01, dst, CMP_LE);


   Mat element = getStructuringElement(MORPH_ELLIPSE, Size(3, 3) );
   morphologyEx(dst, dst, CV_MOP_ERODE, element);
   morphologyEx(dst, dst, CV_MOP_ERODE, element);
   morphologyEx(dst, dst, CV_MOP_ERODE, element);
   morphologyEx(dst, dst, CV_MOP_DILATE, element);
   morphologyEx(dst, dst, CV_MOP_DILATE, element);
   morphologyEx(dst, dst, CV_MOP_DILATE, element);

   namedWindow("Sticazzi");
   imshow("Sticazzi", dst);
   waitKey();
}
