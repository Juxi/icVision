#include <QApplication>
#include "model.h"
#include "sphere.h"
#include "cylinder.h"
#include "box.h"
#include "graspfinder.h"
#include "poses_reader.h"
#include <iostream>
#include <vector>

using namespace KinematicModel;
using namespace std;

int main(int argc, char *argv[])
{
	bool visualize = true;
	
	//vector<vector<float> > some_poses = read_poses("poses.save")["CFGSPACE"];

	QApplication app( argc, argv, visualize );	// create the QT application
	
	Model model( visualize, false );
	model.start();	/* if we want display lists to be created automatically,
					   the model must be started prior to appending objects */
	
	printf( "loading robot file: %s\n", argv[1] );
	Robot &the_robot = *model.loadRobot( QString(argv[1]), false );

	printf( "loading world file: %s\n", argv[1] );
	model.loadWorld( QString(argv[2]), false );
	
	GraspFinder grasp_finder( model, the_robot, false );
	for (float z(-.2); z < .2; z += .03)
		grasp_finder.add_ball(-.2, .15, z);
	for (float z(-.2); z < .2; z += .03)
			grasp_finder.add_ball(-.2, .3, z);

	grasp_finder.start();

	int result = app.exec();						// run the Qt application
	
	grasp_finder.stop();
	model.stop();
	
	return result;
}
