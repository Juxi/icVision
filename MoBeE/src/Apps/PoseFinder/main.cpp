//#include <Vector>
#include <QApplication>
#include "model.h"
#include "sphere.h"
#include "cylinder.h"
#include "box.h"
#include "pose_finder.h"
#include "util.h"

using namespace KinematicModel;

#include <QThread>
#include "model.h"

Vector_d correction(int dim,double ammount)
{
    std::vector<double> correction;
    for (int i=0; i<dim; i++) correction.push_back(ammount);
    Vector_d c(dim,correction.begin(),correction.end());
    return c;
}

int main(int argc, char *argv[])
{
    
	bool visualize = true;
	
	QApplication app( argc, argv, visualize );	// create the QT application
    
    PoseFinder finder(argv[1],"right_arm");
    
    // sample in dim dimensions
    int dim = 4;
    
    CGAL::Random_points_in_cube_d<Point_d> globalCube(dim,0.5);
    finder.setNormPose(*globalCube++ + correction(dim,0.5));
    finder.start();
    
    //Sample q0(model,arm,q_arm);
    
    
    
    // one random pose
    //CGAL::Random_points_in_cube_d<Point_d> globalCube(dim,0.5);
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
            //std::vector<Point_d> samples = gridSample(dim,625,1);
            //for ( std::vector<Point_d>::iterator i = samples.begin(); i!=samples.end(); ++i) {
                //std::cout << *i + foo << std::endl;
                //Sample q(model,arm, *i);
                //q.makeBalls();
            //}
    
    printf("ALL OK\n");
    
	int result = app.exec();						// run the Qt application
	
    finder.stop();
	//model.stop();
	
	return result;
}
