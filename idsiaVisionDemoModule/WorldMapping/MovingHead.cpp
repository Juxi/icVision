/*
 * MovingHead.cpp
 *
 *  Created on: Jul 4, 2011
 *      Author: icub
 */

#include "MovingHead.h"

using namespace std;
using namespace cv;
using namespace yarp;
using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;

MovingHead::MovingHead() {
	// TODO Auto-generated constructor stub

	name2DPointLeftInPort = "/MovingHead/left/2Dpoint:i";
	name2DPointRightInPort = "/MovingHead/right/2Dpoint:i";

	//Input image port
	if(! Point2DLeftInPort.open( name2DPointLeftInPort.c_str() )){
		cerr<<"Unable to open port "+name2DPointLeftInPort<<endl;
	}
	cout<<"Opened port "+name2DPointLeftInPort<<endl;

	//Output image port
	if(! Point2DRightInPort.open( name2DPointRightInPort.c_str() )){
		cerr<<"Unable to open port "+name2DPointRightInPort<<endl;
	}
	cout<<"Opened port "+name2DPointRightInPort<<endl;


	receive2DPntLeft = false;
	receive2DPntRight = false;

	/*
	name3DPointLeftInPort = "/WorldMappingModule/left/3Dpoint:i";
	name3DPointRightInPort = "/WorldMappingModule/right/3Dpoint:i";

	//Input image port
	if(! Point3DLeftInPort.open( name3DPointLeftInPort.c_str() )){
		cerr<<"Unable to open port "+name3DPointLeftInPort<<endl;
	}
	cout<<"Opened port "+name3DPointLeftInPort<<endl;

	//Output image port
	if(! Point3DRightInPort.open( name3DPointRightInPort.c_str() )){
		cerr<<"Unable to open port "+name3DPointRightInPort<<endl;
	}
	cout<<"Opened port "+name3DPointRightInPort<<endl;


	receive3DPntLeft = false;
	receive3DPntRight = false;
*/
}

MovingHead::~MovingHead() {
	// TODO Auto-generated destructor stub
	close();
}

bool MovingHead::interrupt(){
	Point2DLeftInPort.interrupt();
	Point2DRightInPort.interrupt();

	/*
	Point3DLeftInPort.interrupt();
	Point3DRightInPort.interrupt();
	*/
}

bool MovingHead::close(){
	interrupt();

	Point2DLeftInPort.close();
	Point2DRightInPort.close();
	/*
	Point3DLeftInPort.close();
	Point3DRightInPort.close();
	*/
}

bool MovingHead::connect(string port2connectleft,string port2connectright){
	 Network::connect(port2connectleft.c_str(),name2DPointLeftInPort.c_str());
	 Network::connect(port2connectright.c_str(),name2DPointRightInPort.c_str());
}

bool MovingHead::get2DPoints(){

	int posxLe = 0;
  	int posyLe = 0;
  	int posxRi = 0;
	int posyRi = 0;

	Pnt2DLeftIn = Point2DLeftInPort.read(&receive2DPntLeft);
	Pnt2DRightIn = Point2DRightInPort.read(&receive2DPntRight);



	posxLe = Pnt2DLeftIn->get(0).asInt();
	posyLe = Pnt2DLeftIn->get(1).asInt();
	posxRi = Pnt2DRightIn->get(0).asInt();
	posyRi = Pnt2DRightIn->get(0).asInt();

  	//DEBUG STUFF
	std::cout<<"Winner Location Left = ["<<posxLe<<","<<posyLe<<"]"<<endl;
	std::cout<<"Winner Location Right = ["<<posxRi<<","<<posyRi<<"]"<<endl;

}

/*bool MovingHead::get3DPoints(){

	int posxLe = 0;
  	int posyLe = 0;
  	int poszLe = 0;
  	int posxRi = 0;
	int posyRi = 0;
	int poszRi = 0;

	Pnt3DLeftIn = Point3DLeftInPort.read(&receive3DPntLeft);
	Pnt3DRightIn = Point3DRightInPort.read(&receive3DPntRight);



	posxLe = Pnt3DLeftIn->get(0).asInt();
	posyLe = Pnt3DLeftIn->get(1).asInt();
	poszLe = Pnt3DLeftIn->get(2).asInt();
	posxRi = Pnt3DRightIn->get(0).asInt();
	posyRi = Pnt3DRightIn->get(0).asInt();
	poszRi = Pnt3DRightIn->get(1).asInt();

  	//DEBUG STUFF
	std::cout<<"Winner Location Left = ["<<posxLe<<","<<posyLe<<","<<poszLe<<"]"<<endl;
	std::cout<<"Winner Location Right = ["<<posxRi<<","<<posyRi<<","<<poszRi<<"]"<<endl;

}
*/
