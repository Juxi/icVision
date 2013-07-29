//#include <Vector>
#include <QApplication>
#include "model.h"
#include "sphere.h"
#include "cylinder.h"
#include "box.h"
#include "sample.h"
#include "util.h"

using namespace KinematicModel;

#include <QThread>
#include "model.h"


int main(int argc, char *argv[])
{
    
	bool visualize = true;
	
	QApplication app( argc, argv, visualize );	// create the QT application
	
	Model model( visualize, false );
    model.setSyncGraphics(false);
	//model.start();
    //while ( !model.isRunning() ) {}
    //model.stop();
    
    // prepare robot
	printf( "loading robot file: %s\n", argv[1] );
	Robot*      robot = model.loadRobot( QString(argv[1]), false );
    //BodyPart*   head = robot->getPartByName(QString("head"));
    BodyPart*   arm = robot->getPartByName(QString("right_arm"));
    
    // put the robot in a valid pose
    robot->home();
    model.computePose();
    
    // initial robot configuration
    QVector<qreal> arm_pose = arm->getNormPose();
    Point_d q_arm(arm_pose.size(),arm_pose.begin(),arm_pose.end());
    //Sample q0(model,arm,q_arm);
    
    // sample in dim dimensions
    int dim = 4;
    
    // map CGAL's samples from [-0.5 0.5] to [0 1]
    std::vector<double> correction;
    for (int i=0; i<dim; i++) correction.push_back(0.5);
    Vector_d corr(4,correction.begin(),correction.end());
    
    // one random pose
    CGAL::Random_points_in_cube_d<Point_d> globalCube(dim,0.5);
    //Sample q1(model,arm, *globalCube++ + corr);
    //q1.makeBalls();
    
    // other sampling options
            // bend elbow
            /*std::vector<double> mod;
            mod.push_back(0.0);
            mod.push_back(0.0);
            mod.push_back(0.0);
            mod.push_back(1.0);
            Vector_d foo(4,mod.begin(),mod.end());*/
            
            //printf("-------------\n");
            
            // grid sample
            std::vector<Point_d> samples = gridSample(dim,625,1);
            for ( std::vector<Point_d>::iterator i = samples.begin(); i!=samples.end(); ++i) {
                //std::cout << *i + foo << std::endl;
                Sample q(model,arm, *i);
                //q.makeBalls();
            }
    
    printf("ALL OK\n");
    //
    
	int result = app.exec();						// run the Qt application
	
    
	model.stop();
	
	return result;
}
