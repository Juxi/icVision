#include "iCub/featureProcessing/resize.h"


Resize::Resize(std::string name, std::string side, std::string input,unsigned int mode, bool addnoise) {
	
	Network yarp; 						//	set up yarp

	namesideclass = side;				//	"left or right" (suppose "left")
	nameclass =name + "_" + side;		//	"ClassTemplate_left"
	nameinput = input;					//	"main"
	
	std::cout<<"\nConstructing " << nameclass << "...\n";	

/*	READ A GRAY IMAGE	*/		
	nameGrImageInPort = "/graymage/" + nameclass + "/" + namesideclass + "/in";	//[grayimage/ResizeBigger_left/left/in]
	nameSenderGrImagePort ="/grayimage/"+nameinput+"/"+namesideclass+"/out";	//[image/Harris_left/left/out]
	ReceivingGrImagePort.open(nameGrImageInPort.c_str());
	Network::connect(nameSenderGrImagePort.c_str(),nameGrImageInPort.c_str());	
	receivegrimage = false;
	yarpgrimagein = NULL;
	iplgrimagein = NULL;
	
/*	DISPLAY GRAY IMAGE	*/
	#if DEBUGRESIZE
		nameGrImageOutPort = "/image/"+nameclass+"/"+namesideclass+"/out";//	[image/ResizeBigger_left/left/out]
		SendingGrImagePort.open(nameGrImageOutPort.c_str());
		dispGrImage = NULL;
		if(!dispGrImage) dispGrImage = new DisplayIplImage("DisplayResize",namesideclass.c_str(),nameclass.c_str());
	#endif
	
	
/* COMPUTE RESIZE	*/
	mp_cvimage = NULL;
	mp_fixedOutputSize = NULL;	
	this->setParam(mode, addnoise);
	m_interpolation = CV_INTER_NN;
	//TODO CREATE RECT TO SENDBACK AS OUTPUT
	//rectOut.setBuffer(&m_outputrect);
	
/*	SEND A RESIZED IMAGE	*/	
	nameResImageOutPort = "/resizedimage/" + nameclass + "/" + namesideclass + "/out";//	[image/ClassTemplate_left/left/out]
	SendingResImagePort.open(nameResImageOutPort.c_str());
	//iplimageout = NULL;


	std::cout<<"finished.\n";
}

Resize::~Resize(){

	std::cout<<"\nDestructing " << nameclass << "...";
	
/*	READ A GRAY IMAGE IMAGE	*/
	ReceivingGrImagePort.interrupt();
	ReceivingGrImagePort.close();
	//if(iplgrimagein) cvReleaseImage(&iplgrimagein);	//if uncommented gives SIGFAULT
	
	
/*	DISPLAY GRAY IMAGE	*/	
	#if DEBUGRESIZE
		if(dispGrImage) delete dispGrImage;
		SendingGrImagePort.interrupt();
		SendingGrImagePort.close();
	#endif
	
/*	COMPUTE RESIZE	*/
	if(mp_cvimage != NULL)	cvReleaseImage(&mp_cvimage);	//IplImage* (before CVImage*)
	if(mp_fixedOutputSize != NULL) delete mp_fixedOutputSize;	
	
/*	SEND A RESIZED IMAGE	*/
	SendingResImagePort.interrupt();
	SendingResImagePort.close();
//	cvReleaseImage(&iplgrimagein);
	
	std::cout<< "finished.\n";

}

void Resize::setParam(unsigned int mode, bool addnoise) {
	switch(mode) {
		case CROP:
		case RESCALE:
			m_mode = mode;
			m_addnoise = addnoise;
			break;
		default:
			std::cerr <<"\n"<< nameclass << "::setParam::ERROR::Unsupported mode[" << mode << "]!\n";
	}

	if(m_addnoise == true) 	{
		//m_mode = CROP;
		m_rand = cvRNG(-1);
	}
}

void Resize::addNoise() {
	cvRandArr( &m_rand, mp_cvimage, CV_RAND_UNI, cvScalar(0,0,0,0), cvScalar(255,255,255,0) );
}

void Resize::fixOutputSize(const CvSize& size) {
	if(mp_fixedOutputSize == NULL) {
		mp_fixedOutputSize = new CvSize;
	}
	*mp_fixedOutputSize = size;

}

void Resize::execute() {

/*	READ A GRAY IMAGE	*/	
	yarpgrimagein = ReceivingGrImagePort.read(&receivegrimage);
	iplgrimagein = ( IplImage*)yarpgrimagein->getIplImage();
	if(iplgrimagein->nChannels != 1) 
	{ 
		std::cerr <<"\n"<< nameclass << "::execute()::ERROR::grayimg must be single-channel 8-bit image!"; 
		return;
	}
	
	#if DEBUGRESIZE
		std::cout<<"\n"<< nameclass << "::execute::iplgrimagein->depth = "<<iplgrimagein->depth<<"\n";
		std::cout<<"\n"<< nameclass << "::execute::iplgrimagein->nChannels = "<<iplgrimagein->nChannels<<"\n";
		//cvWaitKey(0);
	#endif

	
	//TODO MODIFIED
	CvRect* inputrect = NULL;	// Read some ports eventually rectIn.getBuffer();
	CvSize* outputsize = NULL;	// Read some ports eventually outputSizeIn.getBuffer();
	
	if(outputsize == NULL && mp_fixedOutputSize == NULL) { std::cerr <<"\n"<< nameclass << "::execute()::ERROR::sizeIn is NULL!...\n"; return; }
	if(outputsize == NULL) outputsize = mp_fixedOutputSize;
	
	#if DEBUGRESIZE
		std::cout<<"\n"<< nameclass << "::execute::outputsize=["<<outputsize->width<<","<<outputsize->height<<"]";			
	#endif
	
	//TODO MODIFIED
	float* scaleptr = NULL; // Read some ports eventually scaleIn.getBuffer();
	
	int width, height;
	if(inputrect != NULL) {
		width = inputrect->width;
		height = inputrect->height;
	}
	else{
		width = iplgrimagein->width;
		height = iplgrimagein->height;
	}

	#if DEBUGRESIZE
		std::cout<<"\n"<< nameclass << "::execute::width=["<<width<<"]";
		std::cout<<"\n"<< nameclass << "::execute::height=["<<height<<"]";
	#endif
	
	if(mp_cvimage != NULL) 
	{
		#if DEBUGRESIZE
			std::cout<<"\n"<< nameclass << "::execute::mp_cvimage is NOT NULL";
		#endif
		if(mp_cvimage->width != outputsize->width || mp_cvimage->height != outputsize->height) 
		{
			#if DEBUGRESIZE
				std::cout<<"\n"<< nameclass << "::execute::size of mp_cvimage =! outputsize";
			#endif
			delete mp_cvimage;
			mp_cvimage = NULL;
		}
	}
	
	if(mp_cvimage == NULL) 
	{
		#if DEBUGRESIZE
			std::cout<<"\n"<< nameclass << "::execute::mp_cvimage is NULL";
		#endif
		mp_cvimage = cvCreateImage (*outputsize,iplgrimagein->depth,1);
		#if DEBUGRESIZE
				std::cout<<"\n"<< nameclass << "::execute::size of mp_cvimage is ["<<mp_cvimage->width<<","<<mp_cvimage->height<<"]";
		#endif
		//TODO SEND MP_CVIMAGE that is the resized (send not here but later)
		//cvImageOut.setBuffer(mp_cvimage);	
	}

	if(m_addnoise == false) {
		#if DEBUGRESIZE
				std::cout<<"\n"<< nameclass << "::execute::m_addnoise = FALSE";
		#endif
		cvSetZero(mp_cvimage);
	}
	else{
		#if DEBUGRESIZE
				std::cout<<"\n"<< nameclass << "::execute::m_addnoise = TRUE";
		#endif
		this->addNoise();
	}

	
	switch(m_mode) {
		case CROP :
			#if DEBUGRESIZE
				std::cout<<"\n"<< nameclass << "::execute::mode CROP";
			#endif
			if(inputrect != NULL) 
			{
				#if DEBUGRESIZE
					std::cout<<"\n"<< nameclass << "::execute::inputrect != NULL";	
				#endif
				if(width == mp_cvimage->width && height == mp_cvimage->height) {
					m_outputrect.x = cvRound((mp_cvimage->width - width)/2.0);
					m_outputrect.y = cvRound((mp_cvimage->height - height)/2.0);
					m_outputrect.width = width;
					m_outputrect.height = height;
					cvSetImageROI(iplgrimagein, *inputrect);
					cvSetImageROI(iplgrimagein, m_outputrect);
					cvCopy(iplgrimagein, mp_cvimage);
					cvResetImageROI(iplgrimagein);
					cvResetImageROI(mp_cvimage);
				}
				else 
				{
					std::cerr <<"\n"<< nameclass << "::execute::ERROR::Incorrect inputrect or outputsize!\n";
				}
			}
			else
			{
				#if DEBUGRESIZE
					std::cout<<"\n"<< nameclass << "::execute::inputrect == NULL";
				#endif
				int newwidth = mp_cvimage->width;
				int newheight = mp_cvimage->height;
				#if DEBUGRESIZE
					std::cout<<"\n"<< nameclass << "::execute::newwidht is ["<<newwidth<<"]";
					std::cout<<"\n"<< nameclass << "::execute::newheight is ["<<newheight<<"]\n";
				#endif

				if(width <= newwidth && height <= newheight) 
				{
					m_outputrect.x = cvRound((newwidth - width)/2.0);
					m_outputrect.y = cvRound((newheight - height)/2.0);
					m_outputrect.width = width;
					m_outputrect.height = height;
					cvSetImageROI(mp_cvimage, m_outputrect);
					cvCopy(iplgrimagein, mp_cvimage);
					cvResetImageROI(mp_cvimage);
				}
				else 
				{
					std::cerr <<"\n"<< nameclass << "::execute::ERROR::Incorrect inputrect or outputsize!\n";
				}
			}
			//TODO NOW WE CAN SEND THE RESIZED IMAGE (mp_cvimage) and rectOut (m_outputrect)
			//cvImageOut.out();
			//rectOut.out();
/*			SEND A RESIZED IMAGE	*/
			yarpresimageout.wrapIplImage(mp_cvimage);
			SendingResImagePort.prepare()=yarpresimageout;	
			SendingResImagePort.write();

			break;

		case RESCALE :

			#if DEBUGRESIZE
				std::cout<<"\n"<< nameclass << "::execute::mode RESCALE\n";	
			#endif
			if(inputrect != NULL) 
			{
				int newwidth, newheight;
				if(scaleptr != NULL) 
				{
					float scale = *scaleptr;
					newwidth = cvRound( (inputrect->width)*scale );
					newheight = cvRound( (inputrect->height)*scale );
				}
				else
				{
					newwidth = outputsize->width;
					newheight = outputsize->height;
				}
				temprect.x = cvRound(( (mp_cvimage->width) - newwidth )/2.0);
				temprect.y = cvRound(( (mp_cvimage->height) - newheight )/2.0);
				temprect.width = newwidth;
				temprect.height = newheight;
				if(temprect.x < 0 || temprect.y < 0 || (temprect.x+temprect.width) > mp_cvimage->width || (temprect.y+temprect.height) > mp_cvimage->height) 
				{
					std::cerr <<"\n"<< nameclass << "::execute::ERROR::Output image is too small";
					std::cerr <<"\n"<< nameclass << "::execute::RESCALE::inputrect != NULL";
					std::cerr <<"\n"<< nameclass << "::execute::temprect = [" << temprect.x << "," << temprect.y << "," << temprect.width << "," << temprect.height << "]";
					std::cerr <<"\n"<< nameclass << "::execute::mp_cvimage = [" << mp_cvimage->width << "," << mp_cvimage->height << "]\n";
				}
				else
				{
					if(inputrect->width == temprect.width && inputrect->height == temprect.height) 
					{
						cvSetImageROI(iplgrimagein, *inputrect);
						//TODO SEND iplgrimagein
						//cvImageOut.setBuffer(cvimg);
						//cvImageOut.out();
/*						SEND A RESIZED IMAGE	*/
						yarpresimageout.wrapIplImage(iplgrimagein);
						SendingResImagePort.prepare()=yarpresimageout;
						SendingResImagePort.write();
					}
					else
					{
						cvSetImageROI(iplgrimagein, *inputrect);
						cvSetImageROI(mp_cvimage, temprect);
						cvResize(iplgrimagein, mp_cvimage, m_interpolation);
						cvResetImageROI(iplgrimagein);
						cvResetImageROI(mp_cvimage);
						//TODO SEND mp_cvimage
						//cvImageOut.setBuffer(mp_cvimage);
						//cvImageOut.out();
/*						SEND A RESIZED IMAGE	*/
						yarpresimageout.wrapIplImage(mp_cvimage);
						SendingResImagePort.prepare()=yarpresimageout;
						SendingResImagePort.write();
					}

				}
			}
			else
			{
				float scale = 1.0;
				int newwidth, newheight;
				if(scaleptr != NULL) 
				{
					scale = *scaleptr;
					newwidth = cvRound( (iplgrimagein->width)*scale);
					newheight = cvRound( (iplgrimagein->height)*scale);
				}
				else
				{
					newwidth = outputsize->width;
					newheight = outputsize->height;
				}

				temprect.x = cvRound(( (mp_cvimage->width) - newwidth )/2.0);
				temprect.y = cvRound(( (mp_cvimage->height) - newheight )/2.0);
				temprect.width = newwidth;
				temprect.height = newheight;
				if(temprect.x < 0 || temprect.y < 0 || (temprect.x+temprect.width) > mp_cvimage->width || (temprect.y+temprect.height) > mp_cvimage->height) 
				{
					temprect.width = cvRound( (iplgrimagein->width)/scale);
					temprect.height = cvRound( (iplgrimagein->width)/scale);

					temprect.x = cvRound(( iplgrimagein->width - temprect.width )/2.0);
					temprect.y = cvRound(( iplgrimagein->height - temprect.height )/2.0);
					
					cvSetImageROI(iplgrimagein, temprect);
					cvResize(iplgrimagein, mp_cvimage, m_interpolation);
					cvResetImageROI(iplgrimagein);
				}
				else
				{
					cvSetImageROI(mp_cvimage, temprect);
					cvResize(iplgrimagein, mp_cvimage, m_interpolation);
					cvResetImageROI(mp_cvimage);
				}
				//TODO SEND mp_cvimage
				//cvImageOut.out();
/*				SEND A GRAY IMAGE	*/
				yarpresimageout.wrapIplImage(mp_cvimage);
				SendingResImagePort.prepare()=yarpresimageout;	
				SendingResImagePort.write();
				//TODO SEND RECTOUT (temprect)
				//rectOut.setBuffer(&temprect);
				//rectOut.out();
			}

			break;

		default :
			std::cerr <<"\n"<< nameclass << "::execute::ERROR::Unsupported mode[" << m_mode << "]!\n";

	}

	/*	DISPLAY GRAY (RESIZED) IMAGE	*/	
	#if DEBUGRESIZE
		yarpgrimageout.wrapIplImage(mp_cvimage);
		SendingGrImagePort.prepare()=yarpgrimageout;
		SendingGrImagePort.write();
		dispGrImage->execute();
	#endif
}
