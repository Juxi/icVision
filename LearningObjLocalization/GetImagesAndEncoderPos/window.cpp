/*
 *  window.cpp
 *
 *  Copyright: Juxi Leitner <juxi.leitner@gmail.com>, 2011
 *  based on the helloQt main_window.cpp from Nokia Corporation (Contact: qt-info@nokia.com)
 */

#include <QtGui>

#include "cv_widget.h"
#include "gl_widget.h"
#include "dashboard.h"
#include "window.h"

#include <yarp/sig/all.h>
#include <yarp/os/Network.h>
#include <yarp/os/BufferedPort.h>

#include <QtNetwork/QTcpSocket>
#include <QSound>

#include <opencv/cv.h>

#include <iostream>	//for cout
#include <iomanip>	// for setw
#include <fstream>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BALL_MOVES 2

using namespace yarp::sig;
using namespace yarp::os;


// Constructor
Window::Window() {
	initWindow();
	iCubCtrl = NULL;

	setWindowTitle(tr("GL Test"));	
}

Window::Window(QString &t_in, QString &v_in) {
	initWindow();
	iCubCtrl = NULL;	

	// set title
	title = t_in + " - " + v_in;
    setWindowTitle(title);
}

Window::Window(QString &t_in, QString &v_in, iCubController *iCubCtrl_in)
	: iCubCtrl(iCubCtrl_in)
{

	initWindow();
	// connect to rpc	
	// check whether we have F or not!! TODO
	std::string clientPortName;
	std::string remotePort;

	// set title
	title = t_in + " - " + v_in;
   	setWindowTitle(title);
}

void Window::initWindow() {	
	timer = new QTimer(this);	
	
	srand( time(NULL) );
	
	dash = new Dashboard(iCubCtrl->robotName.toStdString().c_str());
	dash->update();

	vision_widget = new QWidget;
	vision_layout = new QGridLayout;
	lbl_CV_left = new QLabel("<center>Right Eye</center>");
	lbl_CV_right= new QLabel("<center>Left Eye</center>");	
	CV_left		= new CVWidget();	CV_left->sendImage("camera.jpg");
	CV_right	= new CVWidget();	CV_right->sendImage("camera.jpg");
	
	lbl_State_Head = new QLabel("Head");
	txt_State_Head = new QLineEdit();
	lbl_State_Torso = new QLabel("Torso");	
	txt_State_Torso = new QLineEdit();
	lbl_BallPosition = new QLabel("Red Ball in World (3d)");
	txt_BallPosition = new QLineEdit();
	
	btn_get		= new QPushButton("Get Test Readout");
	btn_quit	= new QPushButton("Quit");
	btn_timer	= new QPushButton("Start Gathering Data");

	sld_UpdateInterval = new QSlider(Qt::Horizontal);
	sld_UpdateInterval->setToolTip("Timing Interval");
	sld_UpdateInterval->setRange(100, 2000);
	sld_UpdateInterval->setTickInterval(50);
	sld_UpdateInterval->setValue(500);
	
	//chk_head = new QCheckBox[5]();
	for(int i = 0; i < HEAD_JOINTS; i++) {
		chk_head[i] = new QCheckBox();
		chk_head[i]->setText(QString("Head %1").arg(i));
		if(i < 3) {
			chk_head[i]->setChecked(true);
			
			chk_torso[i] = new QCheckBox(QString("Torso %1").arg(i));
			chk_torso[i]->setChecked(true);			
		}else
			chk_head[i]->setChecked(false);
	}
//	chk_head[0]->setStatusTip("Activate babbling for the head joints");
//	chk_head->setStatusTip("Activate babbling for the torso joints");	
	lbl_ImageCounter = new QLabel("No points gathered yet!");
	
	lbl_CV_left->setMinimumHeight(20);
	lbl_CV_left->setMaximumHeight(60);
	lbl_CV_right->setMinimumHeight(20);
	lbl_CV_right->setMaximumHeight(60);

	int row = 0;
	// Vision Pane (VP)
	// VP - Labels
	vision_layout->addWidget(lbl_CV_left,  row, 0);
	vision_layout->addWidget(lbl_CV_right, row, 1);
	// VP - Images (from iCub)
	row++;
	vision_layout->addWidget(CV_left,  row, 0);
	vision_layout->addWidget(CV_right, row++, 1);

	// VisionPane - info about robot status
	vision_layout->addWidget(lbl_State_Head, row, 0);
	vision_layout->addWidget(txt_State_Head, row++, 1);
	vision_layout->addWidget(lbl_State_Torso, row, 0);
	vision_layout->addWidget(txt_State_Torso, row++, 1);
	vision_layout->addWidget(lbl_BallPosition, row, 0);
	vision_layout->addWidget(txt_BallPosition, row++, 1);
	
	// VP - Images (postprocessed)	
//	vision_layout->addWidget(CV_left_dev, 2, 0);
//	vision_layout->addWidget(CV_right_dev, 2, 1);
	
	// VP - Get Button	
	vision_layout->addWidget(lbl_ImageCounter, row, 0);
	vision_layout->addWidget(btn_get, row++, 1);
	
	
	// Checkboxes for head and torso
	chkH_widget = new QWidget();
	chkT_widget = new QWidget();
	chkH_layout = new QHBoxLayout();
	chkT_layout = new QHBoxLayout();
	
	for(int i = 0;i < HEAD_JOINTS; i++) {
		chk_head[i]->setText(QString("Head %1").arg(i));
		if( i < 5 ) chkH_layout->addWidget(chk_head[i]);
		else		chkT_layout->addWidget(chk_head[i]);
		
	}
	chkH_widget->setLayout(chkH_layout);

	// torso checkers
	for(int i = 0;i < 3; i++)
		chkT_layout->addWidget(chk_torso[i]);
	chkT_widget->setLayout(chkT_layout);

	vision_layout->addWidget(chkH_widget, row, 0);
	vision_layout->addWidget(chkT_widget, row++, 1);
	

	// VP - Start Gathering Data
	vision_layout->addWidget(sld_UpdateInterval, row, 0);
	vision_layout->addWidget(btn_timer, row++, 1);
	
	vision_widget->setLayout(vision_layout);
 	
	central_layout = new QHBoxLayout;

	second_widget = new QWidget;	
	second_layout  = new QVBoxLayout;
	second_layout->setMargin(0);
	second_layout->addWidget(dash);
	second_layout->addWidget(vision_widget);
	second_layout->addWidget(btn_quit);
	second_widget->setLayout(second_layout);
	central_layout->addWidget(second_widget);
//	central_layout->addWidget(gl_widget);
	

//	QString fileName = "test.csv";
//	csvfile = new QFile(fileName);
//	
//	QTextStream stream(csvfile);
//	stream << "This is the csv file:" << "\n" << "imgname,imgname,head-jnt0,head-jnt1...,torso-jnt0,torso-jnt1,torso-jnt2" << "\n";	
//	
//	csvfile->close();
	
	
	imageIndex = 0;
	
	std::string s = "data";
	std::string ext = ".csv";
	std::string filename  = s + ext;
	
//TODO
	std::cout << "Check if file exists?!" << endl;

	csvfile.open (filename.c_str());
	csvfile << "imgname,imgname,head-jnt0,head-jnt1,head-jnt2,head-jnt3,head-jnt4,head-jnt5,torso-jnt0,torso-jnt1,torso-jnt2,ballx,bally,ballz" << "\n";	
	
	setLayout(central_layout);
	move(100,0);
	resize(740, 680);	// width, height
	show();

	
	updateTimer();
}

// Destructor
Window::~Window() {
	
	csvfile.close();
//	delete csvfile;
	
	delete sld_UpdateInterval;
	
	delete btn_timer;
	delete btn_get;
	delete btn_quit;
	delete dash;
	
//	delete CV_left_dev;
//	delete CV_right_dev;
	
	for(int i=0; i < 5;i++)
		delete chk_head[i];
	for(int i=0; i < 3;i++)
		delete chk_torso[i];
	
	delete chkH_layout;
	delete chkH_widget;	
	delete chkT_layout;	
	delete chkT_widget;
	
	delete lbl_ImageCounter;
	delete lbl_State_Head;
	delete lbl_State_Torso;
	delete lbl_BallPosition;
	delete txt_State_Head;
	delete txt_State_Torso;
	delete txt_BallPosition;
	
	delete CV_left;
	delete CV_right;
	delete lbl_CV_left;
	delete lbl_CV_right;	
	delete vision_layout;
	delete vision_widget;		
	delete second_layout;
	//	delete second_widget;
	delete central_layout;
	
	delete timer;
}

void Window::setupSignalsAndSlots() {
	QObject::connect(btn_quit, SIGNAL(clicked()), 
					 qApp,	   SLOT(quit()));
	
	QObject::connect(btn_get,  SIGNAL(clicked()), 
					 this,	   SLOT(getYarpStatus()));

	QObject::connect(sld_UpdateInterval, SIGNAL(valueChanged(int)), //SIGNAL(sliderMoved(int)), 
					 this,	   SLOT(updateTimer()));

	
	QObject::connect(btn_timer,SIGNAL(clicked()), 
					 this,	   SLOT(collectData()));

	
	if(dash && iCubCtrl) {
		QObject::connect(iCubCtrl, SIGNAL(connectionStatus(bool)),
						 dash,	   SLOT(updateConnectionStatus(bool)));
		QObject::connect(dash->btn_connect, SIGNAL(clicked()),
						 this,	   SLOT(toggleConnection()));		
		QObject::connect(dash->btn_initialize, SIGNAL(clicked()),
						 iCubCtrl,			SLOT(initializeRobot()));		

		
		timer->setInterval(1500);
		connect(timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));		
//		connect(timer, SIGNAL(timeout()), this, SLOT(updateDash()));
		
		//		QObject::connect(dash->btn_connect, SIGNAL(clicked()),
		//						 qApp,			SLOT(quit()));
	}
}


void Window::updateTimer() {
	if (sld_UpdateInterval ) {
		int val = sld_UpdateInterval->value();
		QString s = "Timing Interval";
		s.append(QString(" = %1 ms").arg(val));
		sld_UpdateInterval->setToolTip(s);
		
		if (!timer->isActive()) {
			timer->setInterval(val);
		}
	}
		
}

void Window::toggleTimer() {
    if (!timer->isActive()) {		
		getYarpStatus();
        timer->start();
		btn_timer->setText("Stop Gathering Data");
	} else {
		timer->stop();
		iCubCtrl->head->ctrl->stop();
		iCubCtrl->torso->ctrl->stop();
		btn_timer->setText("Start Gathering Data");		
	}
}


void Window::collectData() {
	bool usingKatana = false;
	bool moveRobot = false;	// just collect images!
	
	int pointsPerRun = 1; //20;	// for katana learning
	
	if( usingKatana ) pointsPerRun = 10;
	
	// todo change
	if(iCubCtrl) {
		
		for(int collectedPoints = 0; collectedPoints < pointsPerRun; collectedPoints++) {
		
			
			if( usingKatana ) {
				/// talk to the katana 
				// yarp

				yarp::os::Bottle cmd, response;
				
				// get information about the object from rpc
				cmd.clear();
				cmd.addString("next");
				
				katanaCtrlPort.write(cmd, response);
				
				if( response.size() != 2 ) {
					std::cout << "ARGHAR something went wrong!!" << response.toString() << std::endl;
					break;
				}
				std::cout << "Katana moved!!" << std::endl;
								
				
				// HTTP (old)
//				QTcpSocket* socket = new QTcpSocket();
//				socket->connectToHost("195.176.191.21", 5677);
//				
//				if( ! (socket->waitForConnected(20000)) )
//				{
//					printf("Unable to connect To Katana Server!\n");
//					return;				
//				}
//				else {
//					printf("Connection to Katana (Simon) established.. \nasking for next point 'N'...");
//					char buf[1] = { 'N' };
//					socket->write(buf, 1);
//					socket->flush();
//					
//					if(socket->waitForReadyRead(10000) == false) {
//						//timedout here;
//						std::cout << "socket timed out!! breaking! " <<std::endl;
//						return;
//						
//					}
//					if(1 == socket->getChar(buf)) { 
//						txt_BallPosition->setText( QString("%1").arg( (int) buf[0]) );
//						printf(".. received %d", (int) *buf);
//					} else {
//						txt_BallPosition->setText( "Nothing_read!" );
//					}
//					
//					socket->disconnect();
//				}
				
				
			}
			
			btn_timer->setEnabled(false);
			
			std::cout << "Collecting Data! (" << collectedPoints << ")" << std::endl;
			
			if(moveRobot) {
				float nextposehead[iCubCtrl->head->ctrl->getNumJoints()];
				float nextposetorso[iCubCtrl->torso->ctrl->getNumJoints()];	
			
				// stop it
				iCubCtrl->head->ctrl->stop();
				iCubCtrl->torso->ctrl->stop();
			
				// move to new position
				// right now only joint 2 of head
		//		for(float v = -10; v <= 10; v += 5 ) {	// joint 2
				int posCnt = 0;
				int numPoses = 3 * 3 * 4 * 4 * 4 * 5;
			
				// joints for the torso
				for(float j0 = -15; j0 <= 15; j0+= 15) {
					nextposetorso[0] = j0;			
				
					// bending sideways
					for(float j1 = -10; j1 <= 10; j1+= 10) {		
						nextposetorso[1] = j1;			
					
						// tilt forward/backward
						for(float v = 10; v <= 40; v+= 10) {
							nextposetorso[2] = v;
						
							if(15 == v) {
								moveTheRobot(nextposetorso);
								// wait longer for the resetting
								sleep(5);	
							}
						
											
							// HEAD up down
							for(float hj0 = -30; hj0 <= 0; hj0+= 10) {
								nextposehead[0] = hj0;

								// HEAD left right
								//for(float hj2 = -30; hj2 <= 30; hj2+= 10) {
		//							nextposehead[2] = hj2;
									nextposehead[2] = 0;							
							
									// HEAD eyes up down
									for(float hj3 = -20; hj3 <= 10; hj3+= 10) {
										nextposehead[3] = hj3;						
								
										// HEAD eyes left right
										for(float hj4 = -20; hj4 <= 20; hj4+= 10) {
											posCnt++;
										
											nextposehead[4] = hj4;
										
											if(rand() % 100 != 0) continue;
										
											// try this one 
											moveTheRobot(nextposetorso);
											usleep(50*1000);										
											moveTheRobotHead(nextposehead);
										
											waitForMotionDone();

											getYarpStatus();			
											writeCSV();	
										
											printf("%c   %d of %d = %3.2g percent\n", 'A' + collectedPoints, posCnt, numPoses, posCnt*1.0/(1.0*numPoses)*100.0);	
										
										}
									}
								//}	
							}
						}
					}
				}
			
				// move back to home position, original pose
				nextposetorso[0] = 0;
				nextposetorso[1] = 0;
				nextposetorso[2] = 15;		
				moveTheRobot(nextposetorso);
				waitForMotionDone();
			
				// play sound!!!
				QSound::play("../sounds/Drill.wav");
		
			//ending for
			}
				
		}
		getYarpStatus();
		
		btn_timer->setEnabled(true);
		
		// stop
		// iCubCtrl->head->ctrl->stop();
		// iCubCtrl->torso->ctrl->stop();
		
	}else{
		// just record position and images and write to csv
		
		// todo look at this!?!
		for(int collectedPoints = 0; collectedPoints < 120; collectedPoints++) {
			if( usingKatana ) {
				/// talk to the katana
				QTcpSocket* socket = new QTcpSocket();
				socket->connectToHost("195.176.191.8", 5677);
				
				if( ! (socket->waitForConnected(20000)) )
				{
					printf("Unable to connect To Katana Server!\n");
					return;				
				}
				else {
					printf("Connection to Katana (Simon) established.. \nasking for next point 'N'...");
					char buf[1] = { 'N' };
					socket->write(buf, 1);
					socket->flush();
					
					if(socket->waitForReadyRead(10000) == false) {
						//timedout here;
						std::cout << "socket timed out!! breaking! " <<std::endl;
						return;
						
					}
					if(1 == socket->getChar(buf)) { 
						txt_BallPosition->setText( QString("%1").arg( (int) buf[0]) );
						printf(".. received %d", (int) *buf);
					} else {
						txt_BallPosition->setText( "Nothing_read!" );
					}
					
					socket->disconnect();
				}
			}
			
			getYarpStatus();			
			writeCSV();	
		}
	}
}

void Window::timerTimeout() {
    if (timer->isActive()) {
		
		// stop it
		iCubCtrl->head->ctrl->stop();
		iCubCtrl->torso->ctrl->stop();

		// again?
		iCubCtrl->head->ctrl->stop();
		iCubCtrl->torso->ctrl->stop();

		timer->stop();

		// stop
		// iCubCtrl->head->ctrl->stop();
		// iCubCtrl->torso->ctrl->stop();
		
	}
}

void Window::toggleConnection() {
	QPushButton* btn = dash->btn_connect;
	btn->setText("Connecting...");
	btn->setEnabled(false);	
	
	iCubCtrl->toggleConnection();
}

void Window::getYarpStatus() {
	QSound::play("../sounds/Drill.wav");
	
	if ( ! dash->btn_connect->isEnabled() ) {	// then we are connected
		// read
		showEncoderPositions();
		if(iCubCtrl->simulation) showRedBall3DPosition();
		showYarpImages();
		
/*		Bottle cmd, response;
		cmd.clear();
		cmd.addString("get");
		cmd.addString("encs");
		
		katanaPort.write(cmd, response);
		txt_BallPosition->setText( response.get(2).toString().c_str() );
		//writeCSV();
*/
				
	} else {
		// error not connected
		QMessageBox::warning( this, title,
							 "It seems the YARP network is not yet connected"
							 "to the iCub (Simulator)!\n"
							 "Click on the connect button first!",
							 "OK");
		
	}
}

void Window::waitForMotionDone() {
	float old_headjnt_pos[HEAD_JOINTS];
	float old_torsojnt_pos[3];
	float tolerance = 0.1;
	showEncoderPositions();
	printf("waitForMotionDone .");
	
	usleep(500000);
	do {
		for(int i = 0; i < HEAD_JOINTS; i++)
			old_headjnt_pos[i] = headjnt_pos[i];
		for(int i = 0; i < 3; i++)
			old_torsojnt_pos[i] = torsojnt_pos[i];
		
		showEncoderPositions();
		
		int MoveCount = 0;
		for(int i = 0; i < HEAD_JOINTS; i++)
			MoveCount += fabs(old_headjnt_pos[i] - headjnt_pos[i]) >= tolerance ? 1 : 0;
		for(int i = 0; i < 3; i++)
			MoveCount += fabs(old_torsojnt_pos[i] - torsojnt_pos[i]) >= tolerance ? 1 : 0;
		
		if (MoveCount==0)
			return;

		printf(".");		
		usleep(200000);
	}while(true);
	
	
}

void Window::writeCSV() {
	std::cout << "Write CSV () " << std::endl;
	
	std::stringstream ss1;
	ss1 << "left_";
	ss1 << std::setfill('0') << std::setw(8);
	ss1 << imageIndex;
	ss1 << ".ppm";
	imgNameLeft = ss1.str();
	std::stringstream ss2;
	ss2 << "right_";
	ss2 << std::setfill('0') << std::setw(8);
	ss2 << imageIndex;
	ss2 << ".ppm";
	imgNameRight = ss2.str();

	csvfile << imgNameLeft;
	csvfile << ",";	
	csvfile << imgNameRight; //.toStdString();
	csvfile << ",";	
	for(int i = 0; i < 6; i++) {
		csvfile << headjnt_pos[i];
		csvfile << ",";		
	}
//	csvfile << txt_State_Head->text().toStdString() << ",";
	for(int i = 0; i < 3; i++) {
		csvfile << torsojnt_pos[i];
		csvfile << ",";		
	}
	//	csvfile << txt_State_Torso->text().toStdString()  << ",";
	csvfile << txt_BallPosition->text().toStdString();
	csvfile << "\n";		

	
	if(iCubCtrl->left_camera->initialized)
		yarp::sig::file::write(*iCubCtrl->left_camera->last_image,  imgNameLeft.c_str());
	if(iCubCtrl->right_camera->initialized)
		yarp::sig::file::write(*iCubCtrl->right_camera->last_image, imgNameRight.c_str());
	
	++imageIndex;
	
	QString s = QString("Currently at point %1").arg(imageIndex);
	
	lbl_ImageCounter->setText(s);
	
	//	ImageOf<PixelRgb> *yarp_image;
	//	IplImage *img;
	//	yarp_image = iCubCtrl->left_camera->last_image;
	//	img = (IplImage*)yarp_image->getIplImage();
	
	
}

void Window::changeBallPos() {
	// old rng (quantized)
	int rnr1 = rand() % 11 - 5;
	int rnr2 = (rand() % 45 + 25)/100.0;	
	double rnr = rnr1/22.0;	
	double x = rnr;
	double y = 0.53;	// on the table
	double z = rnr2;

	// new rng
	x = -0.22 + (0.44) * (qreal)rand()/RAND_MAX;
	// fix one dimension, 	x = 0.0;
	z = 0.25 + (0.41) * (qreal)rand()/RAND_MAX;
	
	printf("z=%f\n", z);
	///////
		
	iCubCtrl->setWorldObjectPosition("ssph1", x, y, z);	
}


void Window::showRedBall3DPosition() {
	if(! iCubCtrl->simulation) return;
	
	// get position from the simulator
	std::cout << "Show red ball position() " << std::endl;	

	// not yet initialized
	if(! iCubCtrl) return;
	
	QString s = "";
	double x, y, z;
	iCubCtrl->getWorldObjectPosition("ssph1", x, y, z);
	
	s += QString::number(x) + ",";
	s += QString::number(y) + ",";	
	s += QString::number(z);	
	std::cout << "got " << s.toStdString().c_str() << endl;

//	s.replace(" ", ",");
	txt_BallPosition->setText(s);
}


// read encoder positions
void Window::showEncoderPositions() {
//	std::cout << "Show encoder positions() " << std::endl;	
	
	QString s = "";
    Port *yarp_port;

	// Head data
	if(! iCubCtrl->head->port) return;
	else yarp_port = iCubCtrl->head->port;			
    Bottle input;
	yarp_port->read(input);
	
	if (input != NULL) {
		//std::cout << "got " << input.toString().c_str() << endl;
		s = input.toString();
		
		for(int i = 0; i < input.size(); i++) {
			if(input.get(i).isDouble() && i < HEAD_JOINTS) {
				headjnt_pos[i] = input.get(i).asDouble();
// DEBUG
//				std::cout << "#########Head joints " << i << ": " << headjnt_pos[i] << endl;
			}
		}
	} else {
		s = "NULL";
	}
	s.replace(" ", ",");
	txt_State_Head->setText(s);

	// Torso data
	if(! iCubCtrl->torso->port) return;
	else yarp_port = iCubCtrl->torso->port;			
    Bottle input2;
	yarp_port->read(input2);
	if (input2 != NULL) {
		s = input2.toString();

		for(int i = 0; i < input2.size(); i++) {
			if(input2.get(i).isDouble() && i < 3) {
				torsojnt_pos[i] = input2.get(i).asDouble();
// DEBUG
//				std::cout << "#########Torso joints " << i << ": " << torsojnt_pos[i] << endl;
			}
		}
		
	} else {
		s = "NULL";
	}
	s.replace(" ", ",");	
	txt_State_Torso->setText(s);
}

// show images
void Window::showYarpImages() {
	std::cout << "Show yarp images() " << std::endl;
	ImageOf<PixelRgb> *yarp_image;
	IplImage *img;
	BufferedPort< ImageOf<PixelRgb> > *yarp_port;

	if(iCubCtrl->left_camera->initialized && iCubCtrl->left_camera->port) {		
		// get port
		yarp_port = iCubCtrl->left_camera->port;	
					
		// read an image from the port
		yarp_image = yarp_port->read();
		iCubCtrl->left_camera->last_image = yarp_image;

		if (yarp_image != NULL) {
			//converts data to OpenCV IplImage format
			img = (IplImage*)yarp_image->getIplImage();
			CV_left->sendImage(img);			
		} else 
			CV_left->sendImage("camera.jpg");
	}	
	
	if(iCubCtrl->right_camera->initialized && iCubCtrl->right_camera->port) {		
		// get port
		yarp_port = iCubCtrl->right_camera->port;	
		
		// read an image from the port
		yarp_image = yarp_port->read();
		iCubCtrl->right_camera->last_image = yarp_image;
		
		if (yarp_image != NULL) {
			//converts data to OpenCV IplImage format
			img = (IplImage*)yarp_image->getIplImage();
			
			// display image
			CV_right->sendImage(img);
		} else 
			CV_right->sendImage("camera.jpg");
	}

	// doSomethingWithImages
	
}

void Window::moveTheRobot(float value[]) {	
	// call the part babbler (kail's code) and change it to be stopped after ...
	
	// for the head
	PartController *ctrl = NULL;
	
	qreal min, max, aPos;
	QVector<qreal> pose;
	
	// HEAD
//	if( iCubCtrl->head->initialized ) {
//		
//		ctrl = iCubCtrl->head->ctrl;
//		for (int i = 0; i < ctrl->getNumJoints(); i++) {
//			// limit the head movement to one joint
//			if ( i == 2 ) {
//				ctrl->lim->getLimits(i, &min, &max);
//				std::cout << "min: " << min << " max: " << max << " v: "<< value[i] << std::endl;
//				if( value[i] < max && value[i] > min ) { aPos = value[i]; }
//				else { aPos = headjnt_pos[i]; }				
//			} else { 
//				aPos = headjnt_pos[i];
//			}				
//			pose.append( aPos );
//			
//		}
//		
//		iCubCtrl->head->ctrl->setRefSpeeds( 10.0 );
//		
//		if ( iCubCtrl->head->ctrl->positionMove( pose ) ) { printf("H"); }		
//		else { printf("-\n"); }
//		
//		//		iCubCtrl->head->ctrl->positionMove();
//	}
//	
//	pose.clear();
	
	if( iCubCtrl->torso->initialized ) {
		
		ctrl = iCubCtrl->torso->ctrl;
		for (int i = 0; i < ctrl->getNumJoints(); i++) {
//			if ( i == 2 || i == 0 ) {
				ctrl->lim->getLimits(i, &min, &max);
//				std::cout << "min: " << min << " max: " << max << " v: "<< value[i] << std::endl;
				if( value[i] < max && value[i] > min ) { aPos = value[i]; }
				else { aPos = headjnt_pos[i]; }				
//			} else { 
//				aPos = headjnt_pos[i];
//			}				
			pose.append( aPos );
			
		}
		
		ctrl->setRefSpeeds( 7.0 );
		
		if ( iCubCtrl->torso->ctrl->positionMove( pose ) ) { printf("T"); }		
		else { printf("-\n"); }
		
	}
	
	pose.clear();
}

void Window::moveTheRobotHead(float value[]) {	
	// call the part babbler (kail's code) and change it to be stopped after ...
	
	// for the head
	PartController *ctrl = NULL;
	
	qreal min, max, aPos;
	QVector<qreal> pose;
	
	if( iCubCtrl->head->initialized ) {
		
		ctrl = iCubCtrl->head->ctrl;
		for (int i = 0; i < ctrl->getNumJoints(); i++) {
//			// limit the head movement to one joint
//			if ( i == 2 ) {
				ctrl->lim->getLimits(i, &min, &max);
//				std::cout << "min: " << min << " max: " << max << " v: "<< value[i] << std::endl;
				if( value[i] < max && value[i] > min ) { aPos = value[i]; }
				else { aPos = headjnt_pos[i]; }				
//			} else { 
//				aPos = headjnt_pos[i];
//			}				
			pose.append( aPos );
			
		}
		
		iCubCtrl->head->ctrl->setRefSpeeds( 8.0 );
		
		if ( iCubCtrl->head->ctrl->positionMove( pose ) ) { printf("H"); }		
		else { printf("-\n"); }
		
		//		iCubCtrl->head->ctrl->positionMove();
	}
	

	pose.clear();
}

void Window::doTheBabbling() {			
	
	
//	PartController *ctrl = NULL;
//	
//	qreal min, max, aPos;
//	QVector<qreal> pose;
//	
//	
//	// HEAD
//	if( iCubCtrl->head->initialized ) {
//		
//		ctrl = iCubCtrl->head->ctrl;
//		for (int i = 0; i < ctrl->getNumJoints(); i++) {
//
//			// limit the head movement to not include eyes
//			//if ( i < 4 ) {
////				ctrl->lim->getLimits(i, &min, &max);
////				aPos = min + (max-min) * (qreal)rand()/RAND_MAX;
////			} else { aPos = 0; }
//
////			// limit the head movement to one joint!
////			if ( i == 0 ) {
////				ctrl->lim->getLimits(i, &min, &max);
////				aPos = min + (max-min) * (qreal)rand()/RAND_MAX;
////			} else { aPos = 0; }
//			
//			// limit the head movement to only selected ones ...
//
//			if ( chk_head[i] && chk_head[i]->isChecked() ) {
//				ctrl->lim->getLimits(i, &min, &max);
//				aPos = min + (max-min) * (qreal)rand()/RAND_MAX;
//			} else { 
////				if( i < 4) 
//					aPos = headjnt_pos[i];
////				else aPos = 0;
//			}
//			
//			
//			pose.append( aPos );
//				
//		}
//		
//		iCubCtrl->head->ctrl->setRefSpeeds( 2.0 );
//			
//		if ( iCubCtrl->head->ctrl->positionMove( pose ) ) { printf("H"); }		
//		else { printf("-\n"); }
//		
////		iCubCtrl->head->ctrl->positionMove();
//	}
//	
//	pose.clear();
//
//	// now the torso
//	
//	if( iCubCtrl->torso->initialized ) {
//		ctrl = iCubCtrl->torso->ctrl;
//		for (int i = 0; i < ctrl->getNumJoints(); i++) {
//			if ( chk_torso[i] && chk_torso[i]->isChecked() ) {
//				ctrl->lim->getLimits(i, &min, &max);
//				aPos = min + (max-min) * (qreal)rand()/RAND_MAX;
//			} else { 
//				aPos = torsojnt_pos[i];
//			}
//			pose.append(aPos);
//		}
//
//		iCubCtrl->torso->ctrl->setRefSpeeds( 2.5 );
//		
//		if ( iCubCtrl->torso->ctrl->positionMove( pose ) ) { printf("T"); }		
//		else { printf("*\n"); }
//		
////		if ( iCubCtrl->torso->ctrl->randomPosMove( 2.0, false ) ) { printf("T"); }
////		else { printf("*\n"); }
//	}
//	
//	//so have a wait or something?
//	{ printf("\n"); }
	
}

void Window::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Escape ||
		e->key() == 'q' || e->key() == 'Q')
        close();
    else
        QWidget::keyPressEvent(e);
}

//void Window::doSomethingWithImages() {
//	std::cout << "do something with images" << std::endl;
//	// do something with images
//	
//	// exit
////	return;
//	
//	vector<vector<cv::Point> > left_contours, right_contours;
//	
//	vector<vector<cv::Point> >	obj_list;
//	
//	if(iCubCtrl && iCubCtrl->left_camera && iCubCtrl->left_camera->last_image) {
//		IplImage *iplImg = (IplImage *) iCubCtrl->left_camera->last_image->getIplImage();		
//		cv::Mat imgMat(iplImg);
//		cv::Mat imgMat_gray;
//		cv::Mat tmp;
//		cv::Mat bModel, binaryImage;		
//
//		cv::cvtColor(imgMat, imgMat_gray, CV_RGB2GRAY);
//		
//		//Acquire Background Model
//		imgMat_gray.copyTo(bModel);
//		cv::blur(bModel, bModel, cv::Size(3,3));
//		bool isBEstimated = true;
//		
//		
//		//Detect Object
//				/**
//				 * BACKGROUND SUBTRACTION
//				 */
//		cv::Mat imgMat_gray_blurred;
//		cv::blur(imgMat_gray, imgMat_gray_blurred, cv::Size(5,3));
//		cv::Mat imagediff;
//		cv::Sobel(imgMat_gray_blurred, tmp, -1, 0, 1, 1); //ksizes[i]);		
////		cv::absdiff(bModel, imgMat_gray_blurred, imagediff);
//		cv::compare(imgMat_gray_blurred /*imagediff*/, 55, binaryImage, cv::CMP_GE );
//		cv::Mat element4BS;		
//		cv::morphologyEx(binaryImage, binaryImage, 2 /*3 == CV_MOP_CLOSE*/, element4BS);		
//		cv::cvtColor(binaryImage, imgMat, CV_GRAY2RGB);	
//		
//		vector<cv::Vec4i> hierarchy;
//				
//			//Detect connected component
//		cv::findContours( binaryImage, left_contours, hierarchy, 2/* 2 == cv::CV_RETR_CCOMP*/, 2/*cv::CV_CHAIN_APPROX_SIMPLE*/ );
//		
//
//		// mark objects
//		if( left_contours.size() > 0 ) {
//			int info = 0;
//					// iterate through all the top-level contours,
//					// draw each connected component with its own random color
//					for(int idx = 0; idx >= 0; idx = hierarchy[idx][0] ) {
//						cv::Rect objRect =  boundingRect( cv::Mat(left_contours[idx]) );
//						cv::Scalar color;
//						
//						if(objRect.area() > 140 && objRect.area() < 0.95 * imgMat.size[0] * imgMat.size[1] ) //objRect.area() > 100 && objRect.x > 30 && objRect.x < 290) //&& objRect.area() > 1000) && object_list.size()== 0 /*tested only on one object*/)
//						{
//							switch (info++ % 3) {
//								case 0:
//									color = cv::Scalar (0, 0, 255, 0); /* the color; blue */;
//									break;
//								case 1:
//									color = cv::Scalar(0, 255, 0, 0); /* the color; blue */;
//									break;
//								case 2:
//									color = cv::Scalar(255, 0, 0, 0); /* the color; blue */;
//									break;
//								default:
//									break;
//							}
//							
////							draw on the image
////							yarp::sig::draw::addCircle(*yarp_image,blue,ct,50,10);							
//							PixelRgb b(0,0,128);
//							cv::rectangle(imgMat,                    /* the dest image */
//										cv::Point(objRect.x, objRect.y),        /* top left point */
//										cv::Point(objRect.x + objRect.width, 
//										objRect.y + objRect.height),       /* bottom right point */
//										color,
//										1, 8, 0);    							
////							yarp::sig::draw::addRectangleOutline(* (iCubCtrl->left_camera->last_image), b, objRect.x + objRect.width/2, objRect.y + objRect.height/2, objRect.width, objRect.height);
//							
//							//TODO remove
////							if (object_list.size() == 0)
////								object_list.push_back(WorldObject(contours[idx]));
////							else
////								object_list[objidx].setBB(objRect);
//							
//							obj_list.push_back(left_contours[idx]);							
//						}
//					}
//				}
//		
//		
//		// do feature detection
////		cv::Sobel(imgMat, tmp, -1, 0, 1, 1); //ksizes[i]);
//		
////		cv::Sobel(imgMat, tmp, -1, 0, 1, 5); //ksizes[i]);
//		
////		iplImg = (IplImage *) iCubCtrl->left_camera->last_image->getIplImage();		
////		imgMat = iplImg;
//		
//		IplImage img = imgMat; //_gray_blurred; //tmp;
//		
//		//CV_left_dev->sendImage(&img);
//		CV_left->sendImage(&img);
//	}else {
//		CV_left_dev->sendImage("camera.png");		
//		CV_left->sendImage("camera.png");		
//	}
//	
//	if(iCubCtrl && iCubCtrl->right_camera && iCubCtrl->right_camera->last_image) {
//		IplImage *iplImg = (IplImage *) iCubCtrl->right_camera->last_image->getIplImage();		
////		cv::Mat imgMat(iplImg);
////		cv::Mat tmp;
////		
////		// do feature detection
////		cv::Sobel(imgMat, tmp, -1, 0, 1, 1); //ksizes[i]);
////		
////		//		cv::Sobel(imgMat, tmp, -1, 0, 1, 5); //ksizes[i]);
////		
////		IplImage img = tmp;
//
//		cv::Mat imgMat(iplImg);
//		cv::Mat imgMat_gray;
//		cv::Mat tmp;
//		cv::Mat bModel, binaryImage;		
//		
//		cv::cvtColor(imgMat, imgMat_gray, CV_RGB2GRAY);
//		
//		//Acquire Background Model
//		imgMat_gray.copyTo(bModel);
//		cv::blur(bModel, bModel, cv::Size(3,3));
//		bool isBEstimated = true;
//		
//		
//		//Detect Object
//		/**
//		 * BACKGROUND SUBTRACTION
//		 */
//		cv::Mat imgMat_gray_blurred;
//		cv::blur(imgMat_gray, imgMat_gray_blurred, cv::Size(5,3));
//		cv::Mat imagediff;
//		cv::Sobel(imgMat_gray_blurred, tmp, -1, 0, 1, 1); //ksizes[i]);		
//		//		cv::absdiff(bModel, imgMat_gray_blurred, imagediff);
//		cv::compare(imgMat_gray_blurred /*imagediff*/, 55, binaryImage, cv::CMP_GE );
//		cv::Mat element4BS;		
//		cv::morphologyEx(binaryImage, binaryImage, 2 /*3 == CV_MOP_CLOSE*/, element4BS);		
//		cv::cvtColor(binaryImage, imgMat, CV_GRAY2RGB);	
//		
//		vector<vector<cv::Point> > contours;
//		vector<cv::Vec4i> hierarchy;
//		
//		//Detect connected component
//		cv::findContours( binaryImage, right_contours, hierarchy, 1/*1 seems to be without hierachy, check that!,, 2 == cv::CV_RETR_CCOMP*/, 2/*cv::CV_CHAIN_APPROX_SIMPLE*/ );
//		
//		// mark objects
//		if( right_contours.size() > 0 ) {
//			int info = 0;
//			// iterate through all the top-level contours,
//			// draw each connected component with its own random color
//			for(int idx = 0; idx >= 0; idx = hierarchy[idx][0] ) {
//				cv::Rect objRect = boundingRect(cv::Mat(right_contours[idx]));
//				cv::Scalar color;
//				
//				if(objRect.area() > 250 && objRect.area() < 0.9 * imgMat.size[0] * imgMat.size[1] ) //objRect.area() > 100 && objRect.x > 30 && objRect.x < 290) //&& objRect.area() > 1000) && object_list.size()== 0 /*tested only on one object*/)
//				{
//					
//					switch (info++ % 3) {
//						case 0:
//							color = cv::Scalar (0, 0, 255, 0); /* the color; blue */;
//							break;
//						case 1:
//							color = cv::Scalar(0, 255, 0, 0); /* the color; blue */;
//							break;
//						case 2:
//							color = cv::Scalar(255, 0, 0, 0); /* the color; blue */;
//							break;
//						default:
//							break;
//					}
//					
//					//							draw on the image
//					//							yarp::sig::draw::addCircle(*yarp_image,blue,ct,50,10);							
//					PixelRgb b(0,0,128);
//					cv::rectangle(imgMat,                    /* the dest image */
//								  cv::Point(objRect.x, objRect.y),        /* top left point */
//								  cv::Point(objRect.x + objRect.width, 
//											objRect.y + objRect.height),       /* bottom right point */
//								  color,
//								  1, 8, 0);    							
//					//							yarp::sig::draw::addRectangleOutline(* (iCubCtrl->left_camera->last_image), b, objRect.x + objRect.width/2, objRect.y + objRect.height/2, objRect.width, objRect.height);
//					
//					//TODO remove
//					//							if (object_list.size() == 0)
//					//								object_list.push_back(WorldObject(contours[idx]));
//					//							else
//					//								object_list[objidx].setBB(objRect);
//					
//					obj_list.push_back(right_contours[idx]);
//					
//				}
//			}
//		}
//		
//		
//		// do feature detection
//		//		cv::Sobel(imgMat, tmp, -1, 0, 1, 1); //ksizes[i]);
//		
//		//		cv::Sobel(imgMat, tmp, -1, 0, 1, 5); //ksizes[i]);
//		
//		//		iplImg = (IplImage *) iCubCtrl->left_camera->last_image->getIplImage();		
//		//		imgMat = iplImg;
//		
//		IplImage img = imgMat; //_gray_blurred; //tmp;
////		CV_right_dev->sendImage(&img);
//		CV_right->sendImage(&img);		
//	}else {
////		CV_right_dev->sendImage("camera.png");		
//		CV_right->sendImage("camera.png");		
//	}
//	
//	// got obj?list?
//	if( obj_list.size() > 1 ) {
//		// TODO need to fix figuring out which one is which picture, left or right ... :)
//		int middle = obj_list.size()/2;
//		for(int idx = 0; idx < middle; idx++) {
//			cv::Rect left = boundingRect(cv::Mat(obj_list[idx]));
//			cv::Rect right = boundingRect(cv::Mat(obj_list[idx + middle]));
//
////			cv::Point(left.x, left.y),        /* top left point */
////			cv::Point(left.x + left.width/2, left.y + left.height/2),       /* middle point */
//			
//			std::cout << " Midlle point offset for obj" << idx << ":" << (left.x + left.width/2) - (right.x + right.width/2)
//				<< "/" << (left.y + left.height/2) - (right.y + right.height/2) << std::endl;
//			
//			
//		}
//	}
//}
