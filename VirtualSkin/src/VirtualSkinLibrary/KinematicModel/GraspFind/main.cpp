#include <QApplication>
#include "model.h"
#include "sphere.h"
#include "cylinder.h"
#include "box.h"
#include "graspfinder.h"

using namespace KinematicModel;

int main(int argc, char *argv[])
{
	bool visualize = true;
	
	QApplication app( argc, argv, visualize );	// create the QT application
	
	Model model( visualize, false );
	model.start();	/* if we want display lists to be created automatically,
					   the model must be started prior to appending objects */
	
	printf( "loading robot file: %s\n", argv[1] );
	Robot &the_robot = *model.loadRobot( QString(argv[1]), false );

	printf( "loading world file: %s\n", argv[1] );
	model.loadWorld( QString(argv[2]), false );
	
	GraspFinder grasp_finder( model, the_robot, false );
	grasp_finder.start();

	int result = app.exec();						// run the Qt application
	
	grasp_finder.stop();
	model.stop();
	
	return result;
}
