#include "iCubController.h"
#include "roadmap.h"

#include <QtGui>

//#include "graphwidget.h"
//#include "fileDialog.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{	
	printf("Starting Up...\n");
	
	QApplication app(argc, argv);
   // qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
	
	MainWindow window;
    window.show();
	
	//Roadmap roadmap(4);
	//roadmap.openRobot("icubSimF");
	//roadmap.buildRandomMap( 10,		// number of vertices
	//						5 );	// number of neighbors per node
	//roadmap.project2D();	
	//roadmap.startController();
	
	return app.exec();
	
}
