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
    //finder.sample();
    finder.start();
            
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
