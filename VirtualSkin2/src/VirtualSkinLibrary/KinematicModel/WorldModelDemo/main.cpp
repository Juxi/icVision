#include <QApplication>
#include "model.h"
#include "sphere.h"
#include "cylinder.h"
#include "box.h"
#include "objectSoup.h"

using namespace KinematicModel;

int main(int argc, char *argv[])
{
	bool visualize = true;
	
	QApplication app( argc, argv, visualize );	// create the QT application
	
	Model model( visualize, false );
	model.start();	/* if we want display lists to be created automatically,
					   the model must be started prior to appending objects */
	
	//const char* robotFile = *argv[0]
	printf( "loading file: %s\n", argv[1] );
	model.loadRobot( QString(argv[1]), false );

	ObjectSoup soup( model, 5 );
	soup.start();

	return app.exec();						// run the Qt application
	
	printf("\n\n\n\n HIT THE BUTTON \n\n\n\n");
	soup.stop();
	
	model.stop();
}
