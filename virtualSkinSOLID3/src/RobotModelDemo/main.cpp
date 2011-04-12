#include <QApplication>
#include "controlThread.h"

int main(int argc, char *argv[])
{
	QApplication app( argc, argv, true );	// create the QT application
	
	ControlThread myControlThread;
	myControlThread.model.robot->open("../../../../xml/icubSim.xml");
	myControlThread.start();

	return app.exec();						// run the Qt application
}
