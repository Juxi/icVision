#include "iCub/featureProcessing/detectobject.h"



DetectObject::DetectObject(std::string name)
{

	Network yarp; 						//	set up yarp

	nameclass =name;					//	"Match"
	
	std::cout<<"\nConstructing " << nameclass << "...\n";
	
/*	READ HARRIS	LEFT*/		
	nameHarrisLeftInPort = "/harris/" + nameclass + "/left/in";							//[harris/Match/left/in]
	nameSenderHarrisLeftPort = "/harris/Harris_left/left/out";							//[harris/Harris_left/left/out]
	ReceivingHarrisLeftPort.open(nameHarrisLeftInPort.c_str());
	Network::connect(nameSenderHarrisLeftPort.c_str(),nameHarrisLeftInPort.c_str());
	receiveharrisleft = false;
	HarrisLeftIn=NULL;
	mp_cornersleft = NULL;
	
/*	READ HARRIS	RIGHT*/		
	nameHarrisRightInPort = "/harris/" + nameclass + "/right/in";						//[harris/Match/right/in]
	nameSenderHarrisRightPort = "/harris/Harris_right/right/out";						//[harris/Harris_right/right/out]
	ReceivingHarrisRightPort.open(nameHarrisRightInPort.c_str());
	Network::connect(nameSenderHarrisRightPort.c_str(),nameHarrisRightInPort.c_str());
	receiveharrisright = false;
	HarrisRightIn=NULL;
	mp_cornersright = NULL;		
	
/*	READ GABOR	LEFT	*/		
	nameGaborLeftInPort = "/gabor/" + nameclass + "/left/in";							//[gabor/Match/left/in]
	nameSenderGaborLeftPort = "/gabor/Gabor_left/left/out";								//[gabor/Gabor_left/left/out]
	ReceivingGaborLeftPort.open(nameGaborLeftInPort.c_str());
	Network::connect(nameSenderGaborLeftPort.c_str(),nameGaborLeftInPort.c_str());
	receivegaborleft = false;
	GaborLeftIn=NULL;

/*	READ GABOR	RIGHT	*/		
	nameGaborRightInPort = "/gabor/" + nameclass + "/right/in";							//[gabor/Match/right/in]
	nameSenderGaborRightPort = "/gabor/Gabor_right/right/out";							//[gabor/Gabor_right/right/out]
	ReceivingGaborRightPort.open(nameGaborRightInPort.c_str());
	Network::connect(nameSenderGaborRightPort.c_str(),nameGaborRightInPort.c_str());
	receivegaborright = false;
	GaborRightIn=NULL;
	
/*	COMPUTE DETECTION	*/	
	mp_simmat = cvCreateMat(1, 1000, CV_32FC1);
	//TODO SKIPPED NOT USED
	//checkmat = cvCreateMat(240, 320, CV_32FC1);

	//TODO MODIFIED (NOT USED IN THE END)
	//m_leftregiondetect.allocate(1000, 5);
	//m_rightregiondetect.allocate(1000, 5);
	//m_leftregiondetect = new CvPoint2D32f[1000];
	//m_rightregiondetect = new CvPoint2D32f[1000];
	
	//TODO SKIPPED FPR THE MOMENT CSCFeatures
	//mp_featleft = NULL;
	//mp_featright = NULL;
	
	/*
	m_pointsthres = 10;
	m_detectcounter = 0;
	m_detectthres = 5;
	forceMaxSizeL = false;//true
	forceMaxSizeR = false;//true
	m_maxsize = 100;
	*/
	m_descriptorleft = NULL;
	m_descriptorright = NULL;
	
/*	SEND MATCH HARRIS LEFT	*/
	nameHarrisLeftOutPort = "/harris/" + nameclass + "/left/out";//	[harris/Detect/left/out]
	SendingHarrisLeftPort.open(nameHarrisLeftOutPort.c_str());

/*	SEND MATCH HARRIS RIGHT	*/
	nameHarrisRightOutPort = "/harris/" + nameclass + "/right/out";//	[harris/Detect/right/out]
	SendingHarrisRightPort.open(nameHarrisRightOutPort.c_str());

#if SEGMENTATION
/*	SEND MATCH GABOR LEFT	*/
	nameGaborLeftOutPort = "/gabor/" + nameclass + "/left/out";//	[gabor/Detect/left/out]
	SendingGaborLeftPort.open(nameGaborLeftOutPort.c_str());
	
/*	SEND MATCH GABOR RIGHT	*/
	nameGaborRightOutPort = "/gabor/" + nameclass + "/right/out";//	[gabor/Detect/right/out]
	SendingGaborRightPort.open(nameGaborRightOutPort.c_str());
#endif
	
	setParam();

	std::cout<<"finished.\n";
}

DetectObject::~DetectObject()
{

	std::cout<<"\nDestructing " << nameclass << "...";
	
/*	READ HARRIS LEFT	*/
	ReceivingHarrisLeftPort.interrupt();
	ReceivingHarrisLeftPort.close();
	if(mp_cornersleft) delete [] mp_cornersleft;
	//if(HarrisKeys) delete HarrisKeys;
			
/*	READ HARRIS RIGHT	*/
	ReceivingHarrisRightPort.interrupt();
	ReceivingHarrisRightPort.close();
	if(mp_cornersright) delete [] mp_cornersright;
	//if(HarrisKeys) delete HarrisKeys;
	
/*	READ GABOR LEFT	*/
	ReceivingGaborLeftPort.interrupt();
	ReceivingGaborLeftPort.close();
			
/*	READ GABOR RIGHT	*/
	ReceivingGaborRightPort.interrupt();
	ReceivingGaborRightPort.close();

/*	SEND MATCHING HARRIS LEFT	*/
	SendingHarrisLeftPort.interrupt();
	SendingHarrisLeftPort.close();
	
/*	SEND MATCHING HARRIS RIGHT	*/
	SendingHarrisRightPort.interrupt();
	SendingHarrisRightPort.close();
	
#if SEGMENTATION
/*	SEND MATCHING GABOR LEFT	*/
	SendingGaborLeftPort.interrupt();
	SendingGaborLeftPort.close();
	
/*	SEND MATCHING GABOR RIGHT	*/
	SendingGaborRightPort.interrupt();
	SendingGaborRightPort.close();
#endif
		
/*	COMPUTE DETECTION	*/
	//TODO SKIPPED FOR THE MOMENT
	/*if(mp_featleft) {
		mp_featleft->point(NULL);
		delete mp_featleft;
	}
	if(mp_featright) {
		mp_featright->point(NULL);
		delete mp_featright;
	}*/
	
	//TODO NOT USED IN THE END
	//if(m_leftregiondetect) delete [] m_leftregiondetect;
	//if(m_rightregiondetect) delete [] m_rightregiondetect;
	cvReleaseMat(&mp_simmat);
	//TODO SKIPPED NOT USED
	//cvReleaseMat(&checkmat);
	if(m_descriptorleft) cvReleaseMat(&m_descriptorleft);
	if(m_descriptorright) cvReleaseMat(&m_descriptorright);	


	std::cout<<"finished.\n";
}

void DetectObject::setParam(float simthres, float hordispthres, float verdispthres, unsigned int opmode) 
{

	#if DEBUGDETECT
		std::cout<<"\n"<< nameclass << "::setParam\n";
	#endif

	if(simthres >= 1.0 || simthres <= 0.0) { 	std::cerr <<"\n"<< nameclass << "::setParam::ERROR::simthres out of range (0, 1)!...\n"; return; }

	if(hordispthres <= 0.0) { std::cerr <<"\n"<< nameclass << "::setParam::ERROR::hordispthres out of range (0, inf)!...\n"; return; }

	if(verdispthres <= 0.0) { std::cerr <<"\n"<< nameclass << "::setParam::ERROR::verdispthres out of range (0, inf)!...\n"; return; }
	
	//TODO SKIPPED NOT USED
	//cvSetZero(checkmat);
	m_simthres = simthres;
	m_hordispthres = hordispthres;
	m_verdispthres = verdispthres;
	m_opmode = opmode;

}

void DetectObject::execute() 
{

	#if DEBUGDETECT
		std::cout<<"\n"<< nameclass << "::execute\n";
	#endif

	unsigned int points = compareleftright();
	
	if (points <=0)
		std::cout<<"\n"<< nameclass << "::execute::No matching point detected\n";

	
}

unsigned int DetectObject::compareleftright() 
{

	#if DEBUGDETECT
		std::cout<<"\n"<< nameclass << "::compareleftright\n";
	#endif
	
/*	READ HARRIS	LEFT AND RIGHT*/	
	HarrisLeftIn = ReceivingHarrisLeftPort.read(&receiveharrisleft);
	HarrisRightIn = ReceivingHarrisRightPort.read(&receiveharrisright);
	int cornerCountLeft = HarrisLeftIn->get(0).asInt();
	int cornerCountRight = HarrisRightIn->get(0).asInt();
	if(cornerCountLeft <= 0 || cornerCountRight <= 0) {
		std::cerr <<"\n"<< nameclass << "::compareleftright::ERROR::cornerCountLeft <= 0 or cornerCountRight <= 0!"; 
		return 0;
	}

	m_cornerCountLeft=cornerCountLeft;
	m_cornerCountRight=cornerCountRight;
	if(mp_cornersleft) delete [] mp_cornersleft;
	if(mp_cornersright) delete [] mp_cornersright;
	mp_cornersleft = new CvPoint2D32f[m_cornerCountLeft];
	mp_cornersright = new CvPoint2D32f[m_cornerCountRight];
	
	vector<cv::KeyPoint> HarrisLeftKeys;
	for( int k=1 ;k<2*cornerCountLeft+1 ;k=k+2 )
	{         
		KeyPoint *tmpleft = new KeyPoint(); 
		tmpleft->pt.x = HarrisLeftIn->get(k).asDouble();
		mp_cornersleft[(k-1)/2].x = HarrisLeftIn->get(k).asDouble();
		tmpleft->pt.y = HarrisLeftIn->get(k+1).asDouble();
		mp_cornersleft[(k-1)/2].y = HarrisLeftIn->get(k+1).asDouble();
		HarrisLeftKeys.push_back( *tmpleft ); 
		if(tmpleft) delete tmpleft;
	} 

	vector<cv::KeyPoint> HarrisRightKeys;	
	for( int k=1 ;k<2*cornerCountRight+1 ;k=k+2 )
	{         
		KeyPoint *tmpright = new KeyPoint(); 
		tmpright->pt.x = HarrisRightIn->get(k).asDouble();
		mp_cornersright[(k-1)/2].x = HarrisRightIn->get(k).asDouble();
		tmpright->pt.y = HarrisRightIn->get(k+1).asDouble();
		mp_cornersright[(k-1)/2].y = HarrisRightIn->get(k+1).asDouble();
		HarrisRightKeys.push_back( *tmpright ); 
		if(tmpright) delete tmpright;
	} 

	#if DEBUGDETECT
		int numberleft = HarrisLeftKeys.size();
		int numberright = HarrisRightKeys.size();
		
		std::cout<<"\n"<< nameclass << "::compareleftright::Number of corners left: "<< cornerCountLeft;
		std::cout<<"\n"<< nameclass << "::compareleftright::Number of corners right: "<< cornerCountRight;

		std::cout<<"\n"<< nameclass << "::compareleftright::Number of HarrisKeys left: "<< numberleft;
		std::cout<<"\n"<< nameclass << "::compareleftright::Number of HarrisKeys right: "<< numberright <<"\n";
		
		std::cout<<"\n"<< nameclass << "::compareleftright::cornersleft[0] = [" << mp_cornersleft[0].x << "," << mp_cornersleft[0].y << "]";
		std::cout<<"\n"<< nameclass << "::compareleftright::cornersright[0] = [" << mp_cornersright[0].x << "," << mp_cornersright[0].y << "]";

		std::cout<<"\n"<< nameclass << "::compareleftright::HarrisLetKey[0]: [" << HarrisLeftKeys[0].pt.x << "," << HarrisLeftKeys[0].pt.y << "]";
		std::cout<<"\n"<< nameclass << "::compareleftright::HarrisRightKey[0]: [" << HarrisRightKeys[0].pt.x << "," << HarrisRightKeys[0].pt.y << "]\n";

		std::cout<<"\n"<< nameclass << "::compareleftright::cornersleft["<<(cornerCountLeft-1)/2<<"] = ["<< mp_cornersleft[(cornerCountLeft-1)/2].x << "," << mp_cornersleft[(cornerCountLeft-1)/2].y << "]";
		std::cout<<"\n"<< nameclass << "::compareleftright::cornersright["<<(cornerCountRight-1)/2<<"] = ["<< mp_cornersright[(cornerCountRight-1)/2].x << "," << mp_cornersright[(cornerCountRight-1)/2].y << "]";

		std::cout<<"\n"<< nameclass << "::compareleftright::HarrisLeftKey["<<(numberleft-1)/2<<"] = ["<< HarrisLeftKeys[(numberleft-1)/2].pt.x << "," << HarrisLeftKeys[(numberleft-1)/2].pt.y << "]";
		std::cout<<"\n"<< nameclass << "::compareleftright::HarrisRightKey["<<(numberright-1)/2<<"] = ["<< HarrisRightKeys[(numberright-1)/2].pt.x << "," << HarrisRightKeys[(numberright-1)/2].pt.y << "]\n";

		std::cout<<"\n"<< nameclass << "::compareleftright::cornersleft["<<(cornerCountLeft-1)<<"] = [" << mp_cornersleft[cornerCountLeft-1].x << "," << mp_cornersleft[cornerCountLeft-1].y << "]";
		std::cout<<"\n"<< nameclass << "::compareleftright::cornersright["<<(cornerCountRight-1)<<"] = [" << mp_cornersright[cornerCountRight-1].x << "," << mp_cornersright[cornerCountRight-1].y << "]";
		
		std::cout<<"\n"<< nameclass << "::compareleftright::HarrisLeftKey["<<(numberleft-1)<<"] = [" << HarrisLeftKeys[numberleft-1].pt.x << "," << HarrisLeftKeys[numberleft-1].pt.y << "]";
		std::cout<<"\n"<< nameclass << "::compareleftright::HarrisRightKey["<<(numberright-1)<<"] = [" << HarrisRightKeys[numberright-1].pt.x << "," << HarrisRightKeys[numberright-1].pt.y << "]\n";
		
		//cvWaitKey(0);
	#endif
	

	//TODO SKIPPED FOR THE MOMENT
	/*if(!mp_featleft){
		mp_featleft = reinterpret_cast<CSCFeature*>(GetNewDataPtr(mp_leftdesc->getDescriptorType()));
		mp_featright = reinterpret_cast<CSCFeature*>(GetNewDataPtr(mp_rightdesc->getDescriptorType()));
	}*/

	
/*	READ GABOR LEFT AND RIGHT	*/	
	GaborLeftIn = ReceivingGaborLeftPort.read(&receivegaborleft);
	GaborRightIn = ReceivingGaborRightPort.read(&receivegaborright);

	//TODO MODIFIED
	//unsigned int sizeleft = m_cornerCountLeft;
	//unsigned int sizeright = m_cornerCountRight;
	unsigned int sizeleft = GaborLeftIn->get(0).asInt();
	unsigned int sizeright = GaborRightIn->get(0).asInt();	
	unsigned int descwidthleft = GaborLeftIn->get(1).asInt();
	unsigned int descwidthright = GaborRightIn->get(1).asInt();	

	#if DEBUGDETECT
		std::cout<<"\n"<< nameclass << "::compareleftright::sizeleft: "<< sizeleft;
		std::cout<<"\n"<< nameclass << "::compareleftright::sizeright: "<< sizeright<<"\n";
		std::cout<<"\n"<< nameclass << "::compareleftright::descwidthleft: "<< descwidthleft;
		std::cout<<"\n"<< nameclass << "::compareleftright::descwidthright: "<< descwidthright<<"\n";
		//cvWaitKey(0);
	#endif

	if(m_descriptorleft == NULL)
		m_descriptorleft = cvCreateMat (sizeleft, descwidthleft, CV_32FC1);
	else
	{
		cvReleaseMat(&m_descriptorleft);
		m_descriptorleft = cvCreateMat (sizeleft, descwidthleft, CV_32FC1);
	}
	if(m_descriptorright == NULL)
		m_descriptorright = cvCreateMat (sizeright, descwidthright, CV_32FC1);
	else
	{
		cvReleaseMat(&m_descriptorright);
		m_descriptorright = cvCreateMat (sizeright, descwidthright, CV_32FC1);
	}	
	
	unsigned int   stepleft  = m_descriptorleft->step/sizeof(float);
	float* descdataleft = m_descriptorleft->data.fl;
	unsigned int elleft = 2;
	for (unsigned int rows = 0; rows < m_descriptorleft->rows; rows++)
	{
		for (unsigned int cols = 0; cols < m_descriptorleft->cols; cols ++)
		{
			(descdataleft+rows*stepleft)[cols] = GaborLeftIn->get(elleft).asDouble();
			/*
			#if DEBUGDETECT
				std::cout<<"\n"<< nameclass << "::compareleftright::m_descriptorleft["<<rows<<","<<cols<<"] = " << (descdataleft+rows*stepleft)[cols]<<"\n";
			#endif
			*/
			elleft++;

		}
		/*
		#if DEBUGDETECT
			cvWaitKey(0);
		#endif
		*/

	}

	unsigned int   stepright  = m_descriptorright->step/sizeof(float);
	float* descdataright = m_descriptorright->data.fl;
	unsigned int elright = 2;
	for (unsigned int rows = 0; rows < m_descriptorright->rows; rows++)
	{
		for (unsigned int cols = 0; cols < m_descriptorright->cols; cols ++)
		{
			(descdataright+rows*stepright)[cols] = GaborRightIn->get(elright).asDouble();
			/*
			#if DEBUGDETECT
				std::cout<<"\n"<< nameclass << "::compareleftright::m_descriptorright["<<rows<<","<<cols<<"] = " << (descdataright+rows*stepright)[cols]<<"\n";
			#endif
			*/
			elright++;
		}
		/*
		#if DEBUGDETECT
			cvWaitKey(0);
		#endif
		*/
	}

	#if DEBUGDETECT
		std::cout<<"\n"<< nameclass << "::compareleftright::m_descriptorleft->step ="<<m_descriptorleft->step<<"\n";
		std::cout<<"\n"<< nameclass << "::compareleftright::m_descriptorright->step ="<<m_descriptorright->step<<"\n";
		/*
		for (unsigned int i = 0; i < 40; i++)
		{
			std::cout<<"\n"<<nameclass<<"::compareleftright::descdataleft[0,"<<i<<"] = ["<<(descdataleft+10*stepright)[i]<<"]";
			std::cout<<"\n"<<nameclass<<"::compareleftright::descdataright[0,"<<i<<"] = ["<<(descdataright+0*stepright)[i]<<"]\n";
			std::cout<<"\n"<<nameclass<<"::compareleftright::descdataleft[2,"<<i<<"] = ["<<(descdataleft+15*stepright)[i]<<"]";
			std::cout<<"\n"<<nameclass<<"::compareleftright::descdataright[2,"<<i<<"] = ["<<(descdataright+8*stepright)[i]<<"]\n";
			cvWaitKey(0);	
		}
		
		cvWaitKey(0);
		*/
	#endif
	
	//TODO SKIPPED I CAN USE MY STRUCTURE!? mp_leftregions AND mp_rightregions are mp_cornersleft and mp_cornersright CvPoint2D32f* CONTAINING HARRIS
	//crea un puntatore regiondataleft che punta alla struttura contenente gli harris corner point
	/*unsigned int regionwidthleft = mp_leftregions->getWidth();
	unsigned int regionwidthright = mp_rightregions->getWidth();
	float* regiondataleft = mp_leftregions->getData();
	float* regiondataright = mp_rightregions->getData();*/
	
	// Exhaustive comparison (all to all)
	
	unsigned int pointsbehind = 0, pointsinfront = 0, pointson = 0, pointsbelowthres = 0, pointsaboveverdispthres = 0;	
	
	mvec_indexesonleft.clear();
	mvec_indexesonright.clear();
	mvec_hordisp.clear();
	mvec_verdisp.clear();

	//TODO USED MY STRUCTURE		m_leftregiondetect AND m_rightregiondetect are CvPoint2D32f* [1000] TO FILL IN WITH MATCHING HARRIS
	//crea un puntatore detectregionleft che punta alla struttura vuota m_leftregiondetect, si userà detectregionleft per riempire m_leftregiondetect
	/*float* detectregionleft = m_leftregiondetect.getData();		
	float* detectregionright = m_rightregiondetect.getData();
	unsigned int detectregionwidth = m_leftregiondetect.getWidth();*/
	
	unsigned int detectregioncounter = 0;
	
	cvSetZero(mp_simmat);
	vector<cv::KeyPoint> detectregionleft;
	vector<cv::KeyPoint> detectregionright;
	
	//Exhaustive comparison (all to all) (Other comparison not yet implemented)
	if(m_opmode == 0)
	{
		
		float* leftdata = m_descriptorleft->data.fl;
		float* rightdata = m_descriptorright->data.fl;

		for (unsigned int i = 0; i < sizeleft; i++)
		{
			for (unsigned int j = 0; j < sizeright; j ++)
			{
				float sim = 0.0;
				for (unsigned int k = 0; k < descwidthleft;k++)
				{
					sim += (leftdata+i*stepleft)[k] * (rightdata+j*stepright)[k];
				}

				#if DEBUGDETECT
					//std::cout<<"\n"<< nameclass << "::compareleftright::sim ="<<sim<<"\n";
					//cvWaitKey(0);
				#endif

				cvmSet(mp_simmat, 0, j, sim );
				
			}

			cvMinMaxLoc(mp_simmat, &m_minval, &m_maxval, &m_minloc, &m_maxloc, NULL);
			
			if(m_maxval > (double)m_simthres) 
			{
			
				unsigned int rightindex = m_maxloc.x;
				float dispx = mp_cornersleft[i].x - mp_cornersright[rightindex].x;
				float dispy = mp_cornersleft[i].y - mp_cornersright[rightindex].y;
				
				//TODO USEFUL FOR FOVEA FILTERING
				/*
				float distxL = mp_cornersleft[i].x - 160;
				float distyL = mp_cornersleft[i].y - 120;
				float distxR = mp_cornersright[rightindex].x - 160;
				float distyR = mp_cornersright[rightindex].y - 120;

				float distleft = sqrt(distxL*distxL + distyL*distyL);
				float distright = sqrt(distxR*distxR + distyR*distyR);
				*/
				
				//TODO UNCOMMENT (AND CORRECT) FOR STABLE HARRIS
				/*
				int xchk=regiondataleft[i*regionwidthleft + 0];
				int ychk=regiondataleft[i*regionwidthleft + 1];
				int t = cvmGet(checkmat,ychk,xchk)+2.0;
				cvmSet(checkmat,ychk,xchk,t);
				int g = cvmGet(checkmat,ychk,xchk);
				*/
				//TODO FOR STABLE HARRIS IMPLEMENT THAT PART
				//TODO FOR FOVEA FILTERING IMPLEMENT THAT PART
				
				/*
				#if DEBUGDETECT
					std::cout<<"\n"<< nameclass << "::compareleftright::dispx ="<<dispx";
					std::cout<<"\n"<< nameclass << "::compareleftright::dispy ="<<dispy<<"\n";
					//cvWaitKey(0);
				#endif
				*/
                                //11.0 is for compensating the vertical shift(in pixels) in cameras
                                if(dispy-11.0 > - m_verdispthres && dispy-11.0 < m_verdispthres)
                                //if(fabs(dispy) < m_verdispthres)
				{
					if(dispx < -m_hordispthres)
					{
						pointsbehind++;
					}
					else if(dispx > m_hordispthres)
					{
						pointsinfront++;
					}
					else
					{
						pointson++;
						mvec_indexesonleft.push_back(i);
						mvec_indexesonright.push_back(rightindex);
						mvec_hordisp.push_back(dispx);
						mvec_verdisp.push_back(dispy);
						KeyPoint *tmpleft = new KeyPoint();
						KeyPoint *tmpright = new KeyPoint();						
						tmpleft->pt.x =  mp_cornersleft[i].x;
						tmpleft->pt.y =  mp_cornersleft[i].y;
						tmpright->pt.x = mp_cornersright[rightindex].x;
						tmpright->pt.y = mp_cornersright[rightindex].y;						
						//detectregionleft detectregionright and will contain the matching harris corner point (first left matchs with first right etc.)
						detectregionleft.push_back( *tmpleft );
						detectregionright.push_back( *tmpright );
						if(tmpleft) delete tmpleft;
						if(tmpright) delete tmpright;
						//std::cout<<"\nleftindex = "<<mvec_indexesonleft[detectregioncounter];
						//std::cout<<"\nrightindex = "<<mvec_indexesonright[detectregioncounter]<<endl;
						//cvWaitKey(0);
						detectregioncounter++;
					}
				}
				else
				{
					pointsaboveverdispthres++;
				}
			}
			else
			{
				pointsbelowthres++;
			}
		}
	}
	// Only comparing interest points within certain bounding box (not implemented yet)
	else if(m_opmode == 1)
	{
		std::cerr <<"\n"<< nameclass << "::compareleftright::ERROR::m_opmode == 1 not implemented yet...I'll do it in another life!...\n"; return 0;
	}
	// Only comparing interest points within vertical limit (not yet implemented)
	else if(m_opmode == 2)
	{
		std::cerr <<"\n"<< nameclass << "::compareleftright::ERROR::m_opmode == 2 not implemented yet...you should wait 2 lifes...\n"; return 0;
	}
	else
	{
		std::cerr <<"\n"<< nameclass << "::compareleftright::ERROR::Unsupported mode!\n";
	}
	
	#if DEBUGDETECT
		std::cout<<"\n"<< nameclass << "::compareleftright::sizeleft = " << sizeleft;
		std::cout<<"\n"<< nameclass << "::compareleftright::pointson = " << pointson;
		std::cout<<"\n"<< nameclass << "::compareleftright::pointsbehind = " << pointsbehind;
		std::cout<<"\n"<< nameclass << "::compareleftright::pointsinfront = " << pointsinfront;
		std::cout<<"\n"<< nameclass << "::compareleftright::pointsbelowthres = " << pointsbelowthres;
		std::cout<<"\n"<< nameclass << "::compareleftright::pointsaboveverdispthres = " << pointsaboveverdispthres << "\n";
		/*
		for (unsigned int i = 0; i < detectregioncounter; i++)
		{
			std::cout<<"\nmvec_indexesonleft["<<i<<"] = "<<mvec_indexesonleft[i];
			std::cout<<"\nmvec_indexesonright["<<i<<"] = "<<mvec_indexesonright[i]<<endl;
			cvWaitKey(0);
		}
		*/
	#endif


	//TODO SEND GABOR CHECK
/*	SEND MATCHING HARRIS LEFT AND RIGHT	(I can put it in the previous cicle	for	*/	
	Bottle& HarrisLeftOut = SendingHarrisLeftPort.prepare();
	Bottle& HarrisRightOut = SendingHarrisRightPort.prepare();
    HarrisLeftOut.clear();
    HarrisRightOut.clear();
	HarrisLeftOut.addInt(detectregioncounter);
	HarrisRightOut.addInt(detectregioncounter);

        #if SEGMENTATION
		Bottle& GaborLeftOut = SendingGaborLeftPort.prepare();
		Bottle& GaborRightOut = SendingGaborRightPort.prepare();
		GaborLeftOut.clear();
		GaborRightOut.clear();
		GaborLeftOut.addInt(detectregioncounter);
		GaborRightOut.addInt(detectregioncounter);
		GaborLeftOut.addInt(40);
		GaborRightOut.addInt(40);
	#endif
	
	for (int i = 0; i<detectregioncounter; i++)
	{
		HarrisLeftOut.addDouble(detectregionleft[i].pt.x);
		HarrisLeftOut.addDouble(detectregionleft[i].pt.y);
                #if SEGMENTATION
			for(int j = 0; j<40; j++)
			{
			GaborLeftOut.addDouble((descdataleft+mvec_indexesonleft[i]*stepleft)[j]);
			}
			//std::cout<<"mvec_indexsonleft["<<i<<"] = "<<mvec_indexesonleft[i]<<endl;
			//cvWaitKey(0);
		#endif
	}	
	for (int i = 0; i<detectregioncounter; i++)
	{
		HarrisRightOut.addDouble(detectregionright[i].pt.x);
		HarrisRightOut.addDouble(detectregionright[i].pt.y);
                #if SEGMENTATION
			for(int j = 0; j<40; j++)
			{
				GaborRightOut.addDouble((descdataright+mvec_indexesonright[i]*stepright)[j]);
			}
			//std::cout<<"mvec_indexsonright["<<i<<"] = "<<mvec_indexesonright[i]<<endl;
			//cvWaitKey(0);
		#endif
	}	
	SendingHarrisLeftPort.write();	
	SendingHarrisRightPort.write();	
	
        #if SEGMENTATION
		SendingGaborLeftPort.write();	
		SendingGaborRightPort.write();	
	#endif

	#if DEBUGDETECT

		std::cout<<"\n"<< nameclass << "::compareleftright::Number of detectregionleft: "<< detectregionleft.size();
		std::cout<<"\n"<< nameclass << "::compareleftright::Number of detectregionright: "<< detectregionright.size() <<"\n";

		std::cout<<"\n"<< nameclass << "::compareleftright::HarrisLetKey[0]: [" << detectregionleft[0].pt.x << "," << detectregionleft[0].pt.y << "]";
		std::cout<<"\n"<< nameclass << "::compareleftright::HarrisRightKey[0]: [" << detectregionright[0].pt.x << "," << detectregionright[0].pt.y << "]\n";

		std::cout<<"\n"<< nameclass << "::compareleftright::HarrisLetKey[1]: [" << detectregionleft[1].pt.x << "," << detectregionleft[1].pt.y << "]";
		std::cout<<"\n"<< nameclass << "::compareleftright::HarrisRightKey[1]: [" << detectregionright[1].pt.x << "," << detectregionright[1].pt.y << "]\n";

		std::cout<<"\n"<< nameclass << "::compareleftright::HarrisLeftKey["<<(detectregionleft.size()-1)<<"] = [" << detectregionleft[detectregionleft.size()-1].pt.x << "," << detectregionleft[detectregionleft.size()-1].pt.y << "]";
		std::cout<<"\n"<< nameclass << "::compareleftright::HarrisRightKey["<<(detectregionright.size()-1)<<"] = [" << detectregionright[detectregionright.size()-1].pt.x << "," << detectregionright[detectregionright.size()-1].pt.y << "]\n";
		
		//cvWaitKey(0);
	#endif

	
	//TODO SKIPPED NOT USED
	/*
	int feat_count =0;
	for(int i =0; i<320;i++)
	{
		for(int j=0; j<240;j++)
		{
			double y = (double)cvmGet(checkmat,j,i);
				if (y>0)
				{
					y=y-1;
					cvmSet(checkmat,j,i,y);
					feat_count ++;
				}
				else cvmSet(checkmat,j,i,0);
		}
	}
	*/
	
	return (pointson);
}
