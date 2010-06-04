#include <QApplication>
#include <iostream>
#include "window.h"
#include "robotinterface.h"
#include "RobotFilter.h"
#include "timer.h"

using namespace std;

int main(int argc, char *argv[]) {
	RobotInterface* robot;
	Window* window;
	bool result = false;

	Timer* timer;

	robot = new RobotInterface();
	//timer = new Timer( robot, 20, 1000 );
	timer = new Timer( robot, 20 );

	bool visualize = true;
	QString fileName = "robot.xml";
	if (argc > 1) {
		fileName = QString(argv[1]);
	}
	//if ( argc > 1 && QString(argv[1]) == QString("noVisual")) { visualize = false; } // whether or not to start the gui
	//Alex: you asked me: the "standard" library for parsing command line arguments is optarg. but a 'quasi' standard is also gnu's "getopt.h".
	//it's more flexible and aloows also long optinal names, like --version instead of -v.

	QApplication app(argc, argv, visualize);
	cout << "Launching... " << endl;

	// VISUALIZATION
	if (visualize) {
		cout << "   -with visualization" << endl;
		window = new Window;

        QObject::connect( robot->getQObject(), SIGNAL(finishLink(Link*)), window->getGlWidget(), SLOT(createGLObject(Link*)));
        QObject::connect( timer, SIGNAL(tock()), window->getGlWidget(), SLOT(callUpdateGL()));

        //		QObject::connect( timer, SIGNAL(tock()), window->getGlWidget(), SLOT(callUpdateGL()));
//		QObject::connect( robot->getQObject(), SIGNAL(newJoint(int)),
//				window->getGlWidget(), SLOT(newObject(int)));
//		QObject::connect( robot->getQObject(), SIGNAL(finishJoint( int, const QVector3D&, const QVector3D&, qreal, int )),
//				window->getGlWidget(), SLOT(createDisplayList( int, const QVector3D&, const QVector3D&, qreal, int )));
//		QObject::connect( robot->getQObject(), SIGNAL(jointMoved( int, const QMatrix4x4& )),
//				window->getGlWidget(), SLOT(updateTxfrMatrix( int, const QMatrix4x4& )));
		window->show();
	} else {
		cout << "   -without visualization" << endl;
	}

	// CONFIGURE THE ROBOT
	//cout << "Opening File '" << robot.getFileName().toStdString() << "'" << endl;
	if (!robot->configure(fileName)) {
		cout << "Robot Configuration ERROR!!" << endl;
	} else {
		//robot->setPos(0,9,1);
		//robot->updateTxfrMatrices();

		robot->printBranches();
		//robot->printJoints();
		timer->start(); // oscillate all joints and periodically refresh the display
	}

	// the YARP connection to the iCubSim
	RobotFilter robotFilter;
	if (true == robotFilter.open(*robot)) {

		result = app.exec();

		robotFilter.close();
	}

	// clean up
	timer->stopThread();

	delete robot;
	delete window;
	delete timer;

	return result;
}
