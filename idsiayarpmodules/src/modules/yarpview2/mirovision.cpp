#include "mirovision.h"
#include <math.h>
#include <stdio.h>
#include <cassert>

#include "utils.h"

// #define SEARCH_BALL
// #define SEARCH_BLUE
// #define SEARCH_YELLOW
// #define SEARCH_GREEN
#define NUM_YELLOWS 16
#define NUM_BLUES 1

#define recordtrackingfilename "trackingdata.txt" 
#define recordimagefilename "image%05u.jpg" 

void MiroVision::print_all_robots () {
	float x, y;
	int id;
	float orientation;
	
	printf("\n");
	printf("=== BALL ===\n");
	if (ball.x != -1)
	{
		printf("PIXEL_POS: (%f, %f)\n", ball.x, ball.y);
	}
	
	printf ("\n");
	printf ("*** BLUE ROBOTS ***\n");
	for (unsigned int i = 0; i < blueRobots.size(); i++) 
	{
		id = blueRobots[i].id;
		if (id != -1) {
			x = blueRobots[i].x;
			y = blueRobots[i].y;
			
			orientation = blueRobots[i].orientation*180.0/PI;
			printf ("ID: %i PIXEL_POS: (%f, %f)  ORIENT: %f\n", id, x, y,  orientation);
		}
	};
	printf ("\n");
	 
	printf ("*** YELLOW ROBOTS ***\n");
	for (unsigned int i = 0; i < yellowRobots.size(); i++) 
	{
		id = yellowRobots[i].id;
		if (id != -1) {
			x = yellowRobots[i].x;
			y = yellowRobots[i].y;
			orientation = yellowRobots[i].orientation*180.0/PI;
			
			printf ("ID: %i PIXEL_POS: (%f, %f)  ORIENT: %f\n", id, x, y,  orientation);
		}
	};
	printf ("\n");
	 
}

MiroVision::~MiroVision() {
	if(trackingfp)
	{
		fclose (trackingfp);
	};
}


MiroVision::MiroVision() {
	
	bDrawAllBlobs = false;
	bDrawBlobs = false;
	bDrawRobots = true;
	bTestRGB = false;
	bTestHSV = true;
	verbose = true;
	viewDetectedPixels = true;
	viewPixels = false;
	viewImage = RGB;
	
	nBlueBots = NUM_BLUES; 
	nYellowBots = NUM_YELLOWS;
	
	blueRobots = std::vector<VisionRobot>(nBlueBots);
	yellowRobots = std::vector<VisionRobot>(nYellowBots);
	
	for (unsigned int i = 0; i < blueRobots.size(); i++) {
		blueRobots[i].id = -1;
		blueRobots[i].x = -1;
		blueRobots[i].y = -1;
		blueRobots[i].orientation = 666.0;
	};
	
	for (unsigned int i = 0; i < yellowRobots.size(); i++) {
		yellowRobots[i].id = -1;
		yellowRobots[i].x = -1;
		yellowRobots[i].y = -1;
		yellowRobots[i].orientation = 666.0;
	};
	
	ball.x = -1;
	ball.y = -1;
	
	lastHeight = 0;
	lastWidth = 0;
	
	ROBOT_BLUE_PIXEL = qRgb(0,0,255);
	ROBOT_YELLOW_PIXEL = qRgb(255,255,0);
	ROBOT_GREEN_PIXEL = qRgb(0,255,0);
	ROBOT_RED_PIXEL = qRgb(255,0,0);
	ROBOT_BLACK_PIXEL = qRgb(0,0,0);
	
	ballParam.setRGBBoundaries(200,255,50,160,20,80);	
	ballParam.setHSVBoundaries(255-40,20,150,250,130,255);
	ballParam.distMax=5;
	ballParam.pixelsMin=20;
	ballParam.pixelsMax=100;
	ballParam.sizeMax=12;
	ballParam.sizeMin=3;
	
	
	yellowParam.redMin=200;
	yellowParam.redMax=255;
	yellowParam.greenMin=178;
	yellowParam.greenMax=255;
	yellowParam.blueMin=40;
	yellowParam.blueMax=93;
	yellowParam.setHSVBoundaries(25,90,150,230,150,255);
	yellowParam.distMax=5;
	yellowParam.pixelsMin=10;
	yellowParam.pixelsMax=110;
	yellowParam.sizeMax=15;
	yellowParam.sizeMin=3;

	blueParam.redMin=35;
	blueParam.redMax=45;
	blueParam.greenMin=60;
	blueParam.greenMax=80;
	blueParam.blueMin=120;
	blueParam.blueMax=150;
	blueParam.setHSVBoundaries(145,175,170,210,80,230);
	blueParam.distMax=5;
	blueParam.pixelsMin=10;
	blueParam.pixelsMax=90;
	blueParam.sizeMax=15;
	blueParam.sizeMin=3;
	
	greenParam.redMin=25;
	greenParam.redMax=49;
	greenParam.greenMin=61;
	greenParam.greenMax=73;
	greenParam.blueMin=40;
	greenParam.blueMax=76;
	greenParam.setHSVBoundaries(110,150,70,120,30,130);
	greenParam.distMax=5;
	greenParam.pixelsMin=20;
	greenParam.pixelsMax=200;
	greenParam.sizeMax=25;
	greenParam.sizeMin=4;
	
	robotDistMax=40;
	greenDistMax=70;
	
	trackingfp = fopen(recordtrackingfilename,"w");
	
	sequencenum = 0;
}

bool MiroVision::testPixelRGB(int x,int y,int red,int green,int blue,const std::vector<Blob> &blobs,const DotParam& param)
{
	return ((red >= param.redMin) && (red <= param.redMax) &&
				(green >=param.greenMin) && (green <= param.greenMax) &&
				(blue >= param.blueMin) && (blue <= param.blueMax)) ;
};

bool MiroVision::testPixelHSV(int x,int y,int hue,int saturation,int value,const std::vector<Blob> &blobs,const DotParam& param)
{
	if (param.hueMin<param.hueMax)
	{
		return ((hue >= param.hueMin) && (hue <= param.hueMax) &&
				(saturation >=param.saturationMin) && (saturation <= param.saturationMax) &&
				(value >= param.valueMin) && (value <= param.valueMax)) ;
	}
	else // param.hueMin>=param.hueMax around 
	{
		return ( ( (hue >= param.hueMin && hue <= 255) || (hue <= param.hueMax && hue>=0)) &&
				(saturation >=param.saturationMin) && (saturation <= param.saturationMax) &&
				(value >= param.valueMin) && (value <= param.valueMax)) ;
	};
};

bool MiroVision::testPixel(int x,int y,int red,int green,int blue,int hue,int saturation,int value,const std::vector<Blob> &blobs,const DotParam& param)
{
	if (bTestRGB && bTestHSV)
	{
		return testPixelRGB(x,y, red, green, blue,blobs,param) && testPixelHSV( x, y, hue, saturation, value,blobs, param);
	};
	if (bTestRGB)
	{
		return testPixelRGB(x,y, red, green, blue,blobs,param);
	};
	if (bTestHSV)
	{
		return testPixelHSV( x, y, hue, saturation, value,blobs, param);
	};
	return true;
};


void MiroVision::usePixel(int x,int y,std::vector<Blob> &blobs,const DotParam& param)
{
	if (blobs.size()==0)
	{
		Blob blob(x,y);
		blobs.push_back(blob);
	};
	int nearestBlob = -1;
	for (unsigned int i = 0; i<blobs.size();i++)
	{
		if (y==blobs[i].last_y) 
		{
			if ( (x>=blobs[i].last_row_start_col-param.distMax && x<=blobs[i].last_row_end_col+param.distMax) ||
				(x>=blobs[i].first_x-param.distMax && x<=blobs[i].last_x+param.distMax))
			{
				if (nearestBlob==-1)
				{
					nearestBlob = i;
				}
				else
				{
					blobs[nearestBlob].joinWithBlob(blobs[i]);
					blobs.erase(blobs.begin()+i); // TODO: not effective for vectors!!!
					i--;
				};
			};
		}
		else if (y-blobs[i].last_y<param.distMax)
		{
			if (x>=blobs[i].first_x-param.distMax && x<=blobs[i].last_x+param.distMax)
			{
				if (nearestBlob==-1)
				{
					nearestBlob = i;
				}
				else
				{
					blobs[nearestBlob].joinWithBlob(blobs[i]);
					blobs.erase(blobs.begin()+i); // TODO: not effective for vectors!!!
					i--;
				};
			};
			
		};
	};
	if (nearestBlob !=-1)
	{
		blobs[nearestBlob].addPixel(x,y);
	}
	else
	{
		Blob blob(x,y);
		blobs.push_back(blob);
	};
};

void MiroVision::validateBlobs(std::vector<Blob>& blobs,const DotParam& param)
{
	for (unsigned int k = 0; k<blobs.size();k++)
	{
		if(!blobs[k].isValid(param)) 
		{
			blobs.erase(blobs.begin()+k); // TODO: not effective for vectors!!!
			k--;
		};
	};
};

void MiroVision::printBlobs(std::vector<Blob>& blobs,const DotParam& param)
{
	printf("blobs=%i\n",(int)blobs.size());
	for (unsigned int k = 0; k<blobs.size();k++)
	{
		printf("%i: #%i ||%i =%i",k,blobs[k].getNPixels(),blobs[k].max_col-blobs[k].min_col+1,blobs[k].max_row-blobs[k].min_row+1);
		if(blobs[k].isValid(param))
			printf(" *");
		printf("\n");
	};
};

void MiroVision::drawBlob(const Blob& blob,const DotParam& param,QImage& image) const  // TODO: draw with painter, not direct.
{
	int height = image.height();
	int width = image.width();

	float avg_x = blob.getX();
	float avg_y = blob.getY();
	
	for (int i=blob.min_col;i<=blob.max_col;i++)
	{
		image.setPixel(putInRange(i,0,width),putInRange(blob.min_row,0,height),qRgb(192,192,192));
		image.setPixel(putInRange(i,0,width),putInRange(blob.max_row,0,height),qRgb(192,192,192));
	};
	for (int j = blob.min_row;j<=blob.max_row;j++)
	{
		image.setPixel(putInRange(blob.min_col,0,width),putInRange(j,0,height),qRgb(192,192,192));
		image.setPixel(putInRange(blob.max_col,0,width),putInRange(j,0,height),qRgb(192,192,192));
	};
	
	for (int i=-param.sizeMin/2;i<=param.sizeMin/2;i++)
	{
		image.setPixel(putInRange(avg_x+i,0,width),putInRange(avg_y-param.sizeMin/2,0,height),qRgb(128,128,128));
		image.setPixel(putInRange(avg_x+i,0,width),putInRange(avg_y+param.sizeMin/2,0,height),qRgb(128,128,128));
	};
	for (int j = -param.sizeMin/2;j<=param.sizeMin/2;j++)
	{
		image.setPixel(putInRange(avg_x-param.sizeMin/2,0,width),putInRange(avg_y+j,0,height),qRgb(128,128,128));
		image.setPixel(putInRange(avg_x+param.sizeMin/2,0,width),putInRange(avg_y+j,0,height),qRgb(128,128,128));
	};
	for (int i=-param.sizeMax/2;i<=param.sizeMax/2;i++)
	{
		image.setPixel(putInRange(avg_x+i,0,width),putInRange(avg_y-param.sizeMax/2,0,height),qRgb(255,255,255));
		image.setPixel(putInRange(avg_x+i,0,width),putInRange(avg_y+param.sizeMax/2,0,height),qRgb(255,255,255));
	};
	for (int j = -param.sizeMax/2;j<=param.sizeMax/2;j++)
	{
		image.setPixel(putInRange(avg_x-param.sizeMax/2,0,width),putInRange(avg_y+j,0,height),qRgb(255,255,255));
		image.setPixel(putInRange(avg_x+param.sizeMax/2,0,width),putInRange(avg_y+j,0,height),qRgb(255,255,255));
	};	
};

void MiroVision::drawValidBlobs(const std::vector<Blob>& blobs,const DotParam& param,QImage& image)
{
	for (unsigned int k = 0; k<blobs.size();k++)
	{
		if (blobs[k].isValid(param))
		{
			drawBlob(blobs[k],param,image);
		};
	};
};

void MiroVision::eraseNonValidBlobs(std::vector<Blob>& blobs,const DotParam& param) // TODO: more effective implementation.
{
	for (unsigned int k = 0; k<blobs.size();k++)
	{
		if (!blobs[k].isValid(param))
		{
			blobs.erase(blobs.begin()+k);
			k--;
		};
	};
};


void MiroVision::drawAllBlobs(const std::vector<Blob>& blobs,const DotParam& param,QImage& image)
{
	for (unsigned int k = 0; k<blobs.size();k++)
	{
		drawBlob(blobs[k],param,image);
	};
};

float MiroVision::calculateOrientation(float fromX, float fromY,float toX,float toY)
{
	float x = toX-fromX;
	float y = toY-fromY;
	return atan2(y,x);
};

int MiroVision::nearestBlob(std::vector<Blob> blobs, float x,float y)
{
	if (blobs.size()==0)
	{
		assert(0);
		return -1;
	};
	
	float avg_x = blobs[0].getX();
	float avg_y = blobs[0].getY();

	float dx = x-avg_x;
	float dy = y-avg_y;
	float md = dx*dx+dy*dy;
	int mi = 0;
	
	for (unsigned int i=1;i<blobs.size();i++)
	{
		float avg_x = blobs[i].getX();
		float avg_y = blobs[i].getY();
		float dx = x-avg_x;
		float dy = y-avg_y;
		float d = dx*dx+dy*dy;
		if (d<md) 
		{
			md = d;
			mi = i;
		};
	};
	return mi;
};

int MiroVision::getFreeID(const std::vector<VisionRobot>& robots)
{
	for (unsigned int i=0;i<robots.size();i++)
	{
		if (robots[i].id==-1)
		{
			return i;
		};
	};
	assert(0);
	return -1;
};

void MiroVision::updateBall(VisionBall& ball,std::vector<Blob>& ballBlobs) // TODO: more stable
{
	if (ballBlobs.size()>0) 
	{
		int blobnr = nearestBlob(ballBlobs, ball.x,ball.y);
		float avg_x = ballBlobs[blobnr].getX();
		float avg_y = ballBlobs[blobnr].getY();
		
		ball.x = avg_x;
		ball.y = avg_y;
		ball.unseen = 0; // TODO: currently not used
	}
	else
	{
		ball.unseen++;
	};
};

void MiroVision::updateRobots(std::vector<VisionRobot>& robots,std::vector<Blob>& mainBlobs,std::vector<Blob>& orientBlobs) 
{
	// remove unseen robots
	for (unsigned int i = 0; i < robots.size(); i++) 
	{
		if(robots[i].unseen>100) //TODO:parameter
		{
			robots[i].id = -1;
			robots[i].unseen=0;
		};
	};
	
	// associate all robots which are seen the last round
	for (unsigned int i = 0; i < robots.size(); i++) 
	{
		if (robots[i].id != -1 &&  mainBlobs.size()>0) // TODO: something with not updated robots (counter??)
		{
			int blobnr = nearestBlob(mainBlobs, robots[i].x,robots[i].y);// TODO: respect other robots, which are near to the nearest position
			float avg_x = mainBlobs[blobnr].getX();
			float avg_y = mainBlobs[blobnr].getY();

			float dx = robots[i].x-avg_x;
			float dy = robots[i].y-avg_y;
			float md = dx*dx+dy*dy;
			
			if (md<900.0) // TODO: make it an parameter
			{
				robots[i].x = avg_x;
				robots[i].y = avg_y;
				robots[i].unseen = 0;
				mainBlobs.erase(mainBlobs.begin()+blobnr);
			}
			else
			{
				robots[i].unseen++;
			};
		}
		else
		{
			robots[i].unseen++;
		};
	};
	// try to associate all robots which are not seen the last time
	for (unsigned int i = 0; i < robots.size(); i++) 
	{
		if (robots[i].id == -1 && mainBlobs.size()>0) // TODO: associate the old values to the nearest blobs (so not two hidden bot change theire position)
		{
			int blobnr = mainBlobs.size()-1;
			float avg_x = mainBlobs[blobnr].getX();
			float avg_y = mainBlobs[blobnr].getY();
			robots[i].id = getFreeID(robots);
			robots[i].x = avg_x;
			robots[i].y = avg_y;
			robots[i].unseen = 0;
			mainBlobs.erase(mainBlobs.begin()+blobnr);
		};
	};
	// We have to find all orientations, which meand finding the nearest green blobs
	for (unsigned int i = 0; i < robots.size(); i++) 
	{
		if (robots[i].id != -1 && orientBlobs.size()>0)
		{
			int blobnr = nearestBlob(orientBlobs, robots[i].x,robots[i].y);// TODO: respect other robots, which are near to the nearest position
			float avg_x = orientBlobs[blobnr].getX();
			float avg_y = orientBlobs[blobnr].getY();
			
			float dx = robots[i].x-avg_x;
			float dy = robots[i].y-avg_y;
			float md = dx*dx+dy*dy;
			if (md<400.0) // TODO: make it a parameter 
			{
				robots[i].orientation = calculateOrientation(avg_x,avg_y,robots[i].x,robots[i].y);
				orientBlobs.erase(orientBlobs.begin()+blobnr);
			};
		};
	};
	// TODO: one loop for updating the the correct robot center position (not the team blob center)
	
};

void MiroVision::drawRobots(const std::vector<VisionRobot>& robots,QImage& image)
{
	QPainter p(&image);
	p.setPen(Qt::white);
	for (unsigned int i = 0; i < robots.size(); i++) 
	{
		if (robots[i].id != -1)
		{
			p.drawArc( robots[i].x-20, robots[i].y-20,40,40,0,16 * 360);
			p.setFont(QFont("Arial", 25));
			p.drawText(robots[i].x+20,robots[i].y+20, QString::number(robots[i].id));
			if (fabs(robots[i].orientation) <= PI)
			{
				p.drawLine(robots[i].x,robots[i].y,20*cos(robots[i].orientation)+robots[i].x,20*sin(robots[i].orientation)+robots[i].y);
			};
		};
	};
	
};
void MiroVision::drawBall(const VisionBall& ball,QImage& image)
{
	QPainter p(&image);
	p.setPen(Qt::white);
	p.drawArc( ball.x-6, ball.y-6,12,12,0,16 * 360);
};

void MiroVision::findRobotsInImage(QImage& image) 
{
	char buffer[1024];
	sprintf(buffer,recordimagefilename,sequencenum++);
	image.save(buffer);
	int height=image.height(), width=image.width();
	
	if (lastHeight!=height|| lastWidth!=width)
	{
		detectedImage = QImage(width,height,QImage::Format_RGB32);
		lastHeight=height;
		lastWidth=width;
	};
		
	if(viewPixels) detectedImage.fill(ROBOT_BLACK_PIXEL);
		
	std::vector<Blob> ballBlobs;
	std::vector<Blob> yellowBlobs;
	std::vector<Blob> blueBlobs;
	std::vector<Blob> greenBlobs;
	
	for (int y = 0; y < height; y++) 
	{
		for (int x = 0; x < width; x++) 
		{			
			QRgb pixel = image.pixel(x,y);
			int red = qRed(pixel); 
			int green = qGreen(pixel);
			int blue = qBlue(pixel);
			int hue,saturation,value;
			rgb2hsv(red,green,blue,hue,saturation,value);
#ifdef SEARCH_YELLOW
			if (testPixel(x,y,red,green,blue,hue,saturation,value,yellowBlobs,yellowParam)) 
			{
				usePixel( x, y,yellowBlobs,yellowParam);
				if (viewPixels) detectedImage.setPixel(x,y,ROBOT_YELLOW_PIXEL);
			};
#endif			
#ifdef SEARCH_BLUE
			if (testPixel(x,y,red,green,blue,hue,saturation,value,blueBlobs,blueParam)) 
			{
				usePixel( x, y,blueBlobs,blueParam);
				if (viewPixels) detectedImage.setPixel(x,y,ROBOT_BLUE_PIXEL);
			};
#endif			
#ifdef SEARCH_GREEN
			if (testPixel(x,y,red,green,blue,hue,saturation,value,greenBlobs,greenParam)) 
			{
				usePixel( x, y,greenBlobs,greenParam);
				if (viewPixels) detectedImage.setPixel(x,y,ROBOT_GREEN_PIXEL);
			};
#endif
#ifdef SEARCH_BALL
			if (testPixel(x,y,red,green,blue,hue,saturation,value,ballBlobs,ballParam)) 
			{
				usePixel( x, y,ballBlobs,ballParam);
				if (viewPixels) detectedImage.setPixel(x,y,ROBOT_RED_PIXEL);
			};
#endif			
			
		}
	}
	
	if (viewImage!=RGB) // maybe also red green and blue channels?
	{
		for (int y = 0; y < height; y++) 
		{
			for (int x = 0; x < width; x++) 
			{
				int r,g,b,h,s,v;
				QRgb pixel = image.pixel(x,y);
				r = qRed(pixel); 
				g = qGreen(pixel);
				b = qBlue(pixel);
				switch (viewImage)
				{
					case HUE:
						rgb2hsv(r,g,b,h,s,v);
						hsv2rgb(h,255,255,r,g,b); // looking only on the hue value
						image.setPixel(x,y,qRgb(r,g,b));
						break;
					case SATURATION:
						rgb2hsv(r,g,b,h,s,v);
						image.setPixel(x,y,qRgb(s,s,s)); // looking only for the saturation
						break;
					case VALUE:
						rgb2hsv(r,g,b,h,s,v);
						image.setPixel(x,y,qRgb(v,v,v)); // looking only for the value
						break;
					case RED:
						image.setPixel(x,y,qRgb(r,0,0)); // looking only for the value						
						break;
					case GREEN:
						image.setPixel(x,y,qRgb(0,g,0)); // looking only for the value						
						break;
					case BLUE:
						image.setPixel(x,y,qRgb(0,0,b)); // looking only for the value						
						break;
					case RGB:
						// do nothing
						break;
				};
				//rgb2hsl(r,g,b,h,s,l);
				//hsl2rgb(h,255,128,r,g,b); // looking only on the hue value
				//image.setPixel(x,y,qRgb(r,g,b)); 
				//image.setPixel(x,y,qRgb(s,s,s)); 
				//image.setPixel(x,y,qRgb(l,l,l)); 

			};
		};
	};
	if (viewPixels)
	{
		if (bDrawAllBlobs && bDrawBlobs)
		{
#ifdef SEARCH_BALL			
			drawAllBlobs(ballBlobs,ballParam,detectedImage);
#endif
#ifdef SEARCH_YELLOW			
			drawAllBlobs(yellowBlobs,yellowParam,detectedImage);
#endif
#ifdef SEARCH_BLUE			
			drawAllBlobs(blueBlobs,blueParam,detectedImage);
#endif
#ifdef SEARCH_GREEN			
			drawAllBlobs(greenBlobs,greenParam,detectedImage);
#endif
		}
		else if (bDrawBlobs)
		{
#ifdef SEARCH_BALL
			drawValidBlobs(ballBlobs,ballParam,detectedImage);
#endif
#ifdef SEARCH_YELLOW
			drawValidBlobs(yellowBlobs,yellowParam,detectedImage);
#endif
#ifdef SEARCH_BLUE
			drawValidBlobs(blueBlobs,blueParam,detectedImage);
#endif
#ifdef SEARCH_GREEN
			drawValidBlobs(greenBlobs,greenParam,detectedImage);
#endif
		};
	}
	else
	{
		if (bDrawAllBlobs && bDrawBlobs)
		{
#ifdef SEARCH_BALL
			drawAllBlobs(ballBlobs,ballParam,image);
#endif
#ifdef SEARCH_YELLOW
			drawAllBlobs(yellowBlobs,yellowParam,image);
#endif
#ifdef SEARCH_BLUE
			drawAllBlobs(blueBlobs,blueParam,image);
#endif
#ifdef SEARCH_GREEN
			drawAllBlobs(greenBlobs,greenParam,image);
#endif
		}
		else if (bDrawBlobs)
		{
#ifdef SEARCH_BALL
			drawValidBlobs(ballBlobs,ballParam,image);
#endif
#ifdef SEARCH_YELLOW
			drawValidBlobs(yellowBlobs,yellowParam,image);
#endif
#ifdef SEARCH_BLUE
			drawValidBlobs(blueBlobs,blueParam,image);
#endif
#ifdef SEARCH_GREEN
			drawValidBlobs(greenBlobs,greenParam,image);
#endif
		};
	};
	if (verbose)
	{
#ifdef SEARCH_BALL
		printf("Ball:\n");
		printBlobs(ballBlobs,ballParam);
#endif
#ifdef SEARCH_YELLOW
		printf("Yellow:\n");
		printBlobs(yellowBlobs,yellowParam);
#endif
#ifdef SEARCH_BLUE
		printf("Blue:\n");
		printBlobs(blueBlobs,blueParam);
#endif
#ifdef SEARCH_GREEN
		printf("Green:\n");
		printBlobs(greenBlobs,greenParam);
#endif
	};
	
	// remove non valid blobs
	
#ifdef SEARCH_BLUE
	eraseNonValidBlobs(blueBlobs,blueParam);
#endif
#ifdef SEARCH_YELLOW
	eraseNonValidBlobs(yellowBlobs,yellowParam);
#endif
#ifdef SEARCH_GREEN
	eraseNonValidBlobs(greenBlobs,greenParam);
#endif
#ifdef SEARCH_BALL
	eraseNonValidBlobs(ballBlobs,ballParam);
#endif
	
#ifdef SEARCH_BALL
	updateBall(ball,ballBlobs);
#endif
#ifdef SEARCH_BLUE
	updateRobots(blueRobots,blueBlobs,greenBlobs);
#endif
#ifdef SEARCH_YELLOW
	updateRobots(yellowRobots,yellowBlobs,greenBlobs);
#endif		
	if (verbose) {
		print_all_robots ();
	}
	if (trackingfp)
	{
		// ONLY YELLOW AND ONLY POSITION!!!!!!!!!!!!!!!
		unsigned int num = yellowRobots.size(); 
		fprintf (trackingfp,"%u", num);
		
		for (unsigned int i = 0; i < num; i++) 
		{
			int id = yellowRobots[i].id;
			float x = yellowRobots[i].x;
			float y = yellowRobots[i].y;				
			fprintf (trackingfp,"\t%i\t%f\t%f", id, x, y);
		};
		fprintf (trackingfp,"\n");
		
	};
	
	if (viewPixels)
	{
		if (bDrawRobots)
		{
#ifdef SEARCH_BLUE
			drawRobots(blueRobots,detectedImage);
#endif		
#ifdef SEARCH_YELLOW
			drawRobots(yellowRobots,detectedImage);
#endif		
#ifdef SEARCH_BALL
			drawBall(ball,detectedImage);
#endif		
		};
	}
	else
	{
		if (bDrawRobots)
		{
#ifdef SEARCH_BLUE
			drawRobots(blueRobots,image);
#endif		
#ifdef SEARCH_YELLOW
			drawRobots(yellowRobots,image);
#endif		
#ifdef SEARCH_BALL
			drawBall(ball,image);
#endif		
		};
	};
	
}
