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

	return app.exec();
}
