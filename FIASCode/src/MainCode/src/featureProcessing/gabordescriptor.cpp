#include "iCub/featureProcessing/gabordescriptor.h"


GaborDescriptor::GaborDescriptor(std::string name, std::string side, std::string input) 
{

	Network yarp; 						//	set up yarp

	namesideclass = side;				//	"left or right" (suppose "left")
	nameclass =name + "_" + side;		//	"Gabor_left"
	nameinput = input;					//	"Harris_left"
	
	std::cout<<"\nConstructing " << nameclass << "...\n";
	
/*	READ A GRAY IMAGE	*/		
	nameGrImageInPort = "/grayimage/" + nameclass + "/" + namesideclass + "/in";	//[image/Gabor_left/left/in]
	nameSenderGrImagePort ="/grayimage/"+nameinput+"/"+namesideclass+"/out";	//[image/Harris_left/left/out]
	ReceivingGrImagePort.open(nameGrImageInPort.c_str());
	Network::connect(nameSenderGrImagePort.c_str(),nameGrImageInPort.c_str());	
	receivegrimage = false;
	yarpgrimagein = NULL;
	iplgrimagein = NULL;
	
/*	READ HARRIS	*/		
	nameHarrisInPort = "/harris/" + nameclass + "/" + namesideclass + "/in";		//[harris/Gabor_left/left/in]
	nameSenderHarrisPort = "/harris/" + nameinput +"/" + namesideclass + "/out";	//[harris/Harris_left/left/out]
	ReceivingHarrisPort.open(nameHarrisInPort.c_str());
	Network::connect(nameSenderHarrisPort.c_str(),nameHarrisInPort.c_str());
	receiveharris = false;
	HarrisIn=NULL;
	mp_corners = NULL;	
	//HarrisKeys=NULL;
	
/*	DISPLAY GRAY IMAGE	*/
	#if DEBUGGABOR
		nameGrImageOutPort = "/image/"+nameclass+"/"+namesideclass+"/out";//	[image/Gabor_left/left/out]
		SendingGrImagePort.open(nameGrImageOutPort.c_str());
		dispGrImage = NULL;
		if(!dispGrImage) dispGrImage = new DisplayIplImage("DisplayGray",namesideclass.c_str(),nameclass.c_str());
	#endif
	
/*	COMPUTE GABORDESCRIPTOR	*/

	//TODO CREATE THIS (NOT USED) USED IN GETFILTERIMAGEREAL SEE GABORDESCRIPTOR.CPP ORIGINAL
	//mdp_filtersReal = NULL;
	//mp_filterimgreal = NULL;
	
	m_offsetx = -1;
	m_offsety = -1;
	
	mp_GaborFilter = new GaborFilter("GaborFilter", namesideclass.c_str(),nameclass.c_str());
		
	m_norm = true;
	
	mpc_ResizeBiggerImage = new Resize("ResizeBigger", namesideclass.c_str(), nameinput.c_str(), Resize::CROP, true);

	//TODO CREATE THIS FUNCTION (in the header file)
	//this->setDescriptorType("GJ");

	mp_rescaledimg = NULL; //IplImage* (before CVImage*)

	m_descriptor = NULL;
	
/*	READ A RESIZED IMAGE	*/		
	nameResImageInPort = "/resizedimage/" + nameclass + "/" + namesideclass + "/in";		//[image/ClassTemplate_left/left/in]
	//TODO MAKE GENERAL THIS PORT (NOT HARDCODED resizedimage/ResizeBigger_)
	nameSenderResImagePort = "/resizedimage/ResizeBigger_" + namesideclass +"/" + namesideclass + "/out";	//[image/Camera_left/left/in]
	ReceivingResImagePort.open(nameResImageInPort.c_str());
	Network::connect(nameSenderResImagePort.c_str(),nameResImageInPort.c_str());	
	receiveresimage = false;
	yarpresimagein = NULL;
	iplresimagein = NULL;
	
/*	SEND GABOR	*/
	nameGaborOutPort = "/gabor/" + nameclass + "/" + namesideclass + "/out";//	[gabor/Gabor_left/left/out]
	SendingGaborPort.open(nameGaborOutPort.c_str());

	std::cout<<"finished.\n";
}
 
GaborDescriptor::~GaborDescriptor() 
{

	std::cout<<"\nDestructing " << nameclass << "...";
	
/*	READ AN IMAGE	*/
	ReceivingGrImagePort.interrupt();
	ReceivingGrImagePort.close();
	//if(iplgrimagein) cvReleaseImage(&iplgrimagein);
	
/*	READ HARRIS	*/
	ReceivingHarrisPort.interrupt();
	ReceivingHarrisPort.close();
	if(mp_corners) delete [] mp_corners;
	//if(HarrisKeys) delete HarrisKeys;


/*	DISPLAY GRAY IMAGE	*/	
	#if DEBUGGABOR
		if(dispGrImage) delete dispGrImage;
		SendingGrImagePort.interrupt();
		SendingGrImagePort.close();
	#endif
	
/*	COMPUTE GABORDESCRIPTOR	*/
	//TODO WE HAVE TO UNCOMMENT THIS WHEN CREATE THIS CvMat** (NOT USED SEE MDP_FILTERSREAL IN CONSTRUCTOR)
/*	unsigned int temp = m_L*m_D;
	if(mdp_filtersReal) {
		for(unsigned int i = 0; i<temp;i++) cvReleaseMat(&(mdp_filtersReal[i]));
		delete [] mdp_filtersReal;
	}
*/

	
	
	
	if(mpc_ResizeBiggerImage) delete mpc_ResizeBiggerImage;	
	//TODO WE HAVE TO UNCOMMENT THIS WHEN CREATE THIS IplImage* (NOT USED SEE CONSTRUCTOR AND OTHER NOTES)
	//if(mp_filterimgreal) cvRelease(&mp_filterimgreal);
	if(mp_GaborFilter) delete mp_GaborFilter;
	if(mp_rescaledimg) cvReleaseImage(&mp_rescaledimg);	//IplImage* (before CVimage*)

	if(m_descriptor) cvReleaseMat(&m_descriptor);
	
		
/*	READ A RESIZED IMAGE	*/
	ReceivingResImagePort.interrupt();
	ReceivingResImagePort.close();
	//if(&iplimagein) cvReleaseImage(&iplimagein);		//if uncommented gives SIGFAULT
	
/*	SEND GABOR	*/
	SendingGaborPort.interrupt();
	SendingGaborPort.close();

	std::cout<<"finished.\n";
}

void GaborDescriptor::setParam(unsigned int L, unsigned int D, float f, float kmax, float sigma, float N, bool norm, std::vector<int>* filtersize) 
{

	#if DEBUGGABOR
		std::cout<<"\n"<< nameclass << "::setParam";
	#endif


	
/*	COMPUTE GABORDESCRIPTOR	*/
	m_L = L;
	m_D = D;
	m_f = f;
	m_kmax = kmax;
	m_sigma = sigma;
	m_N = N;
	m_norm = norm;
	
	if(filtersize) 
		mvec_filtersize = *filtersize;
	else 
	{
		mvec_filtersize.clear();
		mvec_filtersize.push_back(55);
		mvec_filtersize.push_back(43);
		mvec_filtersize.push_back(35);
		mvec_filtersize.push_back(25);
		mvec_filtersize.push_back(19);
	}
	
	m_gap = -1000;
	for(std::vector<int>::iterator iter = mvec_filtersize.begin(); iter != mvec_filtersize.end(); ++iter) 
	{
		int temp = *iter;
		if( temp > m_gap) 
			m_gap = temp;
	}
	
	mp_GaborFilter->setParam(L, D, f, kmax, sigma, N, &mvec_filtersize);
	
	mp_rescaledimg = cvCreateImage(cvSize(m_gap, m_gap), IPL_DEPTH_32F, 1);

	m_offsetx = -1;
	m_offsety = -1;

	m_init = true;

}



void GaborDescriptor::execute() 
{
	
	#if DEBUGGABOR
		std::cout<<"\n"<< nameclass << "::execute";
	#endif
	
	if(m_init == false) { 
		std::cerr <<"\n"<< nameclass << "::execute::ERROR::Call setParam first!"; 
		//	TODO understand and uncomment
		//	m_descriptor.setCurrentSize(0);
		//	dataOut.out();
		//
		//////////////////////////////////////////////////////
		return; 
	}
/*	READ A GRAY IMAGE	*/	
	yarpgrimagein = ReceivingGrImagePort.read(&receivegrimage);
	iplgrimagein = ( IplImage*)yarpgrimagein->getIplImage();

	#if DEBUGGABOR
		std::cout<<"\n"<< nameclass << "::execute::iplgrimagein->depth = "<<iplgrimagein->depth<<"\n";
		std::cout<<"\n"<< nameclass << "::execute::iplgrimagein->nChannels = "<<iplgrimagein->nChannels<<"\n";
	#endif

	if(iplgrimagein->nChannels != 1) 
	{ 
		std::cerr <<"\n"<< nameclass << "::execute()::ERROR::grayimg must be single-channel 8-bit image!"; 
		return;
	}
	
/*	READ HARRIS	*/	
	HarrisIn = ReceivingHarrisPort.read(&receiveharris);
	int cornerCount = HarrisIn->get(0).asInt();
	if(cornerCount <= 0) {
		std::cerr <<"\n"<< nameclass << "::execute()::ERROR::cornerCount <= 0!"; 

		//	TODO understand and uncomment
		//	m_descriptor.setCurrentSize(0);
		//	dataOut.out();
		//
		//////////////////////////////////////////////////////
		
		return;
	}

	//TODO MOVE TO EXECUTE
/*	READ HARRIS	*/
	m_cornerCount=cornerCount;
	if(mp_corners) delete [] mp_corners;
	mp_corners = new CvPoint2D32f[m_cornerCount];
	
	vector<cv::KeyPoint> HarrisKeys;	
	for( int k=1 ;k<2*cornerCount+1 ;k=k+2 )
	{         
		KeyPoint *tmp = new KeyPoint(); 
		tmp->pt.x = HarrisIn->get(k).asDouble();
		mp_corners[(k-1)/2].x = HarrisIn->get(k).asDouble();
		tmp->pt.y = HarrisIn->get(k+1).asDouble();
		mp_corners[(k-1)/2].y = HarrisIn->get(k+1).asDouble();
		HarrisKeys.push_back( *tmp ); 
		if(tmp) delete tmp;
	} 
	
/*	DISPLAY GRAY IMAGE	*/	
	#if DEBUGGABOR
		int number = HarrisKeys.size();
		std::cout<<"\n"<< nameclass << "::execute::Number of corners: "<< cornerCount;
		std::cout<<"\n"<< nameclass << "::execute::Number of HarrisKeys: "<< number <<"\n";
		
		std::cout<<"\n"<< nameclass << "::execute::corners[0] = [" << mp_corners[0].x << "," << mp_corners[0].y << "]";
		std::cout<<"\n"<< nameclass << "::execute::HarrisKey[0]: [" << HarrisKeys[0].pt.x << "," << HarrisKeys[0].pt.y << "]\n";

		std::cout<<"\n"<< nameclass << "::execute::corners["<<(cornerCount-1)/2<<"] = ["<< mp_corners[(cornerCount-1)/2].x << "," << mp_corners[(cornerCount-1)/2].y << "]";
		std::cout<<"\n"<< nameclass << "::execute::HarrisKey["<<(number-1)/2<<"] = ["<< HarrisKeys[(number-1)/2].pt.x << "," << HarrisKeys[(number-1)/2].pt.y << "]\n";

		std::cout<<"\n"<< nameclass << "::execute::corners["<<(cornerCount-1)<<"] = [" << mp_corners[cornerCount-1].x << "," << mp_corners[cornerCount-1].y << "]";
		std::cout<<"\n"<< nameclass << "::execute::HarrisKey["<<(number-1)<<"] = [" << HarrisKeys[number-1].pt.x << "," << HarrisKeys[number-1].pt.y << "]\n\n";
			
		CvScalar target_color[4] = { // in RGB order
			{{   0,   0, 255,   0 }},  // blue
			{{   0, 255,   0,   0 }},  // green
			{{ 255,   0,   0,   0 }},  // red
			{{   255, 255, 0,   0 }}   // yellow
		};

		for( int i = 0; i < cornerCount; i++) 
		{
			int radius = 1;
			cvCircle(iplgrimagein,
					cvPoint((int)(mp_corners[i].x + 0.5f),(int)(mp_corners[i].y + 0.5f)),
					radius,
					target_color[3]);
		}
	
		yarpgrimageout.wrapIplImage(iplgrimagein);
		SendingGrImagePort.prepare()=yarpgrimageout;
		SendingGrImagePort.write();
		dispGrImage->execute();
		//cvWaitKey(0);
	#endif
	
	if(m_offsetx == -1)
	{
		m_imagesize.width = iplgrimagein->width;
		m_imagesize.height = iplgrimagein->height;
		m_biggerimagesize.width = m_imagesize.width + (m_gap-1);
		m_biggerimagesize.height = m_imagesize.height + (m_gap-1);
		m_offsetx = cvRound((m_gap-1)/2);
		m_offsety = cvRound((m_gap-1)/2);
		#if DEBUGGABOR
			std::cout<<"\n"<< nameclass << "::execute::m_gap = " << m_gap << "\n";
			std::cout<<"\n"<< nameclass << "::execute::m_imagesize = [" << m_imagesize.width << " " << m_imagesize.height << "]";
			std::cout<<"\n"<< nameclass << "::execute::m_biggerimagesize = [" << m_biggerimagesize.width << " " << m_biggerimagesize.height << "]";
			std::cout<<"\n"<< nameclass << "::execute::m_offsetx = " << m_offsetx;
		#endif
		mpc_ResizeBiggerImage->fixOutputSize(m_biggerimagesize);
	}

	//TODO CREATE A MATRIX THAT WILL CONTAIN THE GABORDESCRIPTORS
	//float* descriptorsdata = m_descriptor.getData();
	//if(descriptorsdata == NULL) {
		//descriptorsdata = m_descriptor.allocate(mp_regions->getHeight(), m_L*m_D);
	//}
	//m_descriptor.setRegions(mp_regions);
	
	
	if(m_descriptor == NULL)
		m_descriptor = cvCreateMat (cornerCount, m_L*m_D, CV_32FC1);
	else
	{
		cvReleaseMat(&m_descriptor);
		m_descriptor = cvCreateMat (cornerCount, m_L*m_D, CV_32FC1);
	}
	
	float* descriptorsdata = m_descriptor->data.fl;

	//TODO	non dovrebbe servire
/*	if(descriptorsdata == NULL) {
		std::cout<<"\n"<< nameclass << "::execute::descriptorsdata = NULL";
		cvWaitKey(0);
		descriptorsdata = new float [cornerCount, m_L*m_D];
	}		
*/
	
	mpc_ResizeBiggerImage->execute();

/*	READ A RESIZED IMAGE	*/	
	yarpresimagein = ReceivingResImagePort.read(&receiveresimage);
	iplresimagein = ( IplImage*)yarpresimagein->getIplImage();	//biggercvimg is iplresimagein

	#if DEBUGGABOR
		std::cout<<"\n"<< nameclass << "::execute::iplresimagein->depth = " << iplresimagein->depth << "\n";
		std::cout<<"\n"<< nameclass << "::execute::iplresimagein->nChannels = " << iplresimagein->nChannels << "\n";
	#endif

	if(iplresimagein->depth == IPL_DEPTH_8U) { 
		mp_cvgrayimgflt = m_ConvertFlt.convert(iplresimagein); //mp_cvgrayimgflt is IplImage* (before CVImage*)
	}
	else{
		mp_cvgrayimgflt = iplresimagein;
	}

	#if DEBUGGABOR
		std::cout<<"\n"<< nameclass << "::execute::mp_cvgrayimgflt->depth = " << mp_cvgrayimgflt->depth << " = IPL_DEPTH_32F\n";
		std::cout<<"\n"<< nameclass << "::execute::mp_cvgrayimgflt->nChannels = " << mp_cvgrayimgflt->nChannels << "\n";
		cvNamedWindow("mp_cvgrayimgflt",1);
		cvShowImage("mp_cvgrayimgflt",mp_cvgrayimgflt);
	#endif	

	this->compute(HarrisKeys);

	#if DEBUGGABORDESCRIPTOR
		float* elementde;
		elementde = m_descriptor->data.fl;
		int   stepde  = m_descriptor->step/sizeof(float);
		for (unsigned int rows = 0; rows < m_descriptor->rows; rows++)
		{
			for (unsigned int cols = 0; cols < m_descriptor->cols; cols ++)
			{
	
				std::cout<<"\n"<<nameclass<<"::execute::m_descriptor["<<rows<<","<<cols<<"] (before normalization)= "<<(elementde+rows*stepde)[cols]<<"\n";
				//cvWaitKey(0);
			}
		}
	#endif

	if(m_norm) 
		normalize();
	
/*	SEND GABOR	*/
	float* element;
	element = m_descriptor->data.fl;
	int   step  = m_descriptor->step/sizeof(float);
	#if DEBUGGABORDESCRIPTOR
		std::cout<<"\n"<< nameclass << "::execute::m_descriptor->step ="<<m_descriptor->step<<"\n";
		std::cout<<"\n"<< nameclass << "::execute::m_descriptor->rows ="<<m_descriptor->rows<<"\n";
		std::cout<<"\n"<< nameclass << "::execute::m_descriptor->cols ="<<m_descriptor->cols<<"\n";
		//cvWaitKey(0);
	#endif
	
	Bottle& GaborOut = SendingGaborPort.prepare();
    GaborOut.clear();
	GaborOut.addInt(m_descriptor->rows);
	GaborOut.addInt(m_descriptor->cols);	
	for (unsigned int rows = 0; rows < m_descriptor->rows; rows++)
	{
		for (unsigned int cols = 0; cols < m_descriptor->cols; cols ++)
		{
	
			#if DEBUGGABORDESCRIPTOR
				std::cout<<"\n"<< nameclass << "::execute::m_descriptor["<<rows<<","<<cols<<"] (after normalization)= " << (element+rows*step)[cols]<<"\n";
				//cvWaitKey(0);
			#endif
	
			GaborOut.addDouble((element+rows*step)[cols]);

		}
	}
	SendingGaborPort.write();

}

void GaborDescriptor::compute(vector<cv::KeyPoint> KeyPoints) {

	#if DEBUGGABOR
		std::cout<<"\n"<< nameclass << "::compute";
	#endif
	vector<cv::KeyPoint> HarrisKeys = KeyPoints;
	//TODO SKIPPED
	//float* regiondata = mp_regions->getData();
	//TODO HARDCODED
	unsigned int regionwidth = 2;	//mp_regions->getWidth();
	//TODO MODIFIED //float* descdata = m_descriptor.getData();
	float* descdata = m_descriptor->data.fl;
	#if DEBUGGABOR
		std::cout<<"\n"<< nameclass << "::compute::descdata = "<<*descdata<<"\n";
		//cvWaitKey(0);
	#endif
	//TODO HARDCODED
	unsigned int descwidth = 40;	//m_descriptor.getWidth();

	unsigned int startindex = 0;
	unsigned int endindex = HarrisKeys.size()-1;//mp_regions->getCurrentSize() - 1;

	IplImage* img = mp_cvgrayimgflt;
	IplImage* rescaledimg = mp_rescaledimg;

	#if DEBUGGABOR
		std::cout<<"\n"<< nameclass << "::execute::img size = [" << img->width << ", " << img->height <<"]\n";
		std::cout<<"\n"<< nameclass << "::execute::rescaledimg size = [" << rescaledimg->width << ", " << rescaledimg->height <<"]\n";
	#endif	
	
	//TODO CHECK img = mp_cvgrayimgflt non mi convince affatto è troppo bianca
	/*#if DEBUGGABOR
		cvNamedWindow("GABORDESC0",1);
		cvShowImage("GABORDESC0",img);
		cvNamedWindow("GABORDESC1",1);
		cvShowImage("GABORDESC1",rescaledimg);
		cvWaitKey(0);
	#endif	*/

	unsigned int temp = m_gap;

	#if DEBUGGABOR
		std::cout<<"\n"<< nameclass << "::execute::temp= [" << temp <<"]\n";
		//cvWaitKey(0);
	#endif	

	GaborFilter* gabor = mp_GaborFilter;
	
		for(unsigned int i = startindex; i<=endindex; i++) {

		// Rescale image (hopefully cvCopy and cvResize functions are thread-safe)
		unsigned int posx = cvRound(HarrisKeys[i].pt.x) + m_offsetx;
		unsigned int posy = cvRound(HarrisKeys[i].pt.y) + m_offsety;
		#if DEBUGGABOR
			if(i == 0 || i == endindex/2 || i == endindex)
			{
				std::cout<<"\n"<< nameclass << "::compute::Harrisx["<<i<<"] = " << posx-m_offsetx;
				std::cout<<"\n"<< nameclass << "::execute::Harrisy["<<i<<"] = " << posy-m_offsety;
				std::cout<<"\n"<< nameclass << "::compute::posx["<<i<<"] = " << posx;
				std::cout<<"\n"<< nameclass << "::execute::posy["<<i<<"] = " << posy<<"\n";
			}	
		#endif	
		unsigned int width = cvRound(55/2);
		unsigned int height = cvRound(55/2);
		unsigned int startx = posx - cvRound((width-1)/2);
		unsigned int starty = posy - cvRound((height-1)/2);
		CvRect inputrect;
		inputrect.x = startx;
		inputrect.y = starty;
		inputrect.width = width;
		inputrect.height = height;
		#if DEBUGGABOR
			std::cout<<"\n"<< nameclass << "::execute::inputrect.width = "<<inputrect.width;
			std::cout<<"\n"<< nameclass << "::execute::inputrect.height = "<<inputrect.height;
			std::cout<<"\n"<< nameclass << "::execute::img->nChannels = "<<img->nChannels<<"\n";
			//cvWaitKey(0);
		#endif

		cvSetImageROI(img, inputrect);
		if(width == temp && height == temp) {
			// Simple copy operation
			cvCopy(img, rescaledimg, NULL);
			#if DEBUGGABOR
				cvNamedWindow("filterimgor",1);
				cvShowImage("filterimgor",rescaledimg);
				//cvWaitKey(0);
			#endif
		}
		else{
			// Crop-resize the image to match the filter size
			cvResize(img, rescaledimg, CV_INTER_LINEAR);
			#if DEBUGGABOR
				cvNamedWindow("filterimgor",1);
				cvShowImage("filterimgor",rescaledimg);
				//cvWaitKey(0);
			#endif
		}
		cvResetImageROI(img);


		// Convolution
		CvPoint pos;
		pos.x = (width-1)/2;
		pos.y = (height-1)/2;
		float* output = &(descdata[i*descwidth]);
		#if DEBUGGABOR
			std::cout<<"\n"<< nameclass << "::execute::pos.x = "<<pos.x<<"\n";
			std::cout<<"\n"<< nameclass << "::execute::pos.y = "<<pos.y<<"\n";
			std::cout<<"\n"<< nameclass << "::execute::i*descwidth = "<<i*descwidth<<"\n";
			std::cout<<"\n"<< nameclass << "::execute::output = "<<*output<<"\n";		
			//cvWaitKey(0);
		#endif
		gabor->compute(rescaledimg, &pos, output);
	}	



	//cvReleaseImage (&img);
	//cvReleaseImage (&rescaledimg);

}

void GaborDescriptor::normalize() {

	unsigned int index = 0;
	for(unsigned int i = 0;i<m_descriptor->rows;i++) {
		float sum = 0.0;
		float* tempfl = &(m_descriptor->data.fl[index]);
		for(unsigned int j = 0;j<m_descriptor->cols;j++) {
			float temp = tempfl[j];
			sum += temp * temp;
			index++;
		}
		
		if(sum > 0.0) {
			sum = sqrt(sum);
			for(unsigned int j = 0;j<m_descriptor->cols;j++) {
				tempfl[j] /= sum;
			}
		}

		index += ((m_descriptor->step - 4*m_descriptor->cols)/4);
	}
}
