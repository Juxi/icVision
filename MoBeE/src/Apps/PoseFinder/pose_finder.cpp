#include "pose_finder.h"
#include "sphere.h"
#include "cylinder.h"
#include "box.h"
#include "util.h"

PoseFinder::PoseFinder( char* _robot, char* _part) : history( model.OBSTACLE(), model.GHOST() )
{
    //model.setSyncGraphics(false);
	//model.start();
    //while ( !model.isRunning() ) {}
    //model.stop();
    
    robot = model.loadRobot(QString(_robot), false );
    robot->home();
    
    model.computePose();
    
    bodypart = robot->getPartByName(QString(_part));
    marker = *bodypart->getMarkers().begin();
 
    QColor aColor = QColor( qrand()%255, qrand()%255, qrand()%255, 100 );
    history.setCollidingColor(aColor);
    history.setFreeColor(aColor);
    
    //model.appendObject(&history);
    
    keepRunning = false;
    
    // initial robot configuration
    //QVector<qreal> pose = bodypart->getNormPose();
    //Point_d q(pose.size(),pose.begin(),pose.end());
    
}

void PoseFinder::stop()
{
	printf("PoseFinder thread is shutting down ");
	keepRunning = false;
	while ( isRunning() )
	{
		printf(".");
		msleep(20);
	}
	printf("\n");
}

void PoseFinder::run()
{
    CGAL::Random_points_on_sphere_d<Point_d> localSphere(bodypart->size(), 0.1);
    
    //Vector_d xm,ym,zm;                         // joi
    //QVector3D x(1,0,0), y(0,1,0), z(0,0,1);
	//double  xdm = -1.0, ydm = -1.0, zdm = 0.0;
    
    keepRunning = true;
    
	while ( keepRunning )
	{
        QVector<qreal> pose = bodypart->getNormPose();
        Point_d q(pose.size(),pose.begin(),pose.end());
        
        QVector3D marker_position = marker->node()->getPos();
        KinematicModel::CompositeObject* obj = new KinematicModel::CompositeObject( model.OBSTACLE(), model.GHOST() );
        KinematicModel::PrimitiveObject* x_primitive = new KinematicModel::Sphere( 0.01 );
        KinematicModel::PrimitiveObject* y_primitive = new KinematicModel::Sphere( 0.01 );
        KinematicModel::PrimitiveObject* z_primitive = new KinematicModel::Sphere( 0.01 );
        //primitive->setCollidingColor(history.getCollidingColor());
        //primitive->setFreeColor(history.getFreeColor());
        x_primitive->translate(marker_position);
        obj->appendPrimitive(x_primitive);
        model.appendObject(obj);
        
        
        double max = -1.0;
        double len = 0.0;
        Vector_d winner = *localSphere - CGAL::ORIGIN;
        printf("----------------------\n");
        for (int n=0; n<1000; n++) {
                //printf("..%d",n);
            Vector_d candidate = *localSphere++ - CGAL::ORIGIN;
                //std::cout << "  candidate: " << candidate << std::endl;
            setNormPose(q + candidate);
            QVector3D new_marker_pose = marker->node()->getPos();
            QVector3D dx = new_marker_pose - marker_position;
            double dot_prod = QVector3D::dotProduct(QVector3D(1,0,0), dx/dx.length());
                //std::cout << "  dot_prod: " << dot_prod << std::endl;
            if ( dot_prod > max) {
                max = dot_prod;
                len = dx.length();
                winner = candidate;
            }
        }
        std::cout << " winner: " << winner << std::endl;
        std::cout << " dot_prod: " << max << std::endl;
        std::cout << " length: " << len << std::endl;
        setNormPose(q+winner);
        printf("\n");
	}
}

Vector_d PoseFinder::dDelta( bool advance ){
    CGAL::Random_points_on_sphere_d<Point_d> localSphere(bodypart->size(), 0.01);
    return *localSphere++ - CGAL::ORIGIN;
}

void PoseFinder::setNormPose( Point_d q ) {
    int n = 0;
    QVector<qreal> cmd;
    for ( Point_d::Cartesian_const_iterator i = q.cartesian_begin(); i != q.cartesian_end(); ++i )
        cmd.push_back(*i);
    bodypart->setNormPos(cmd);
    model.computePose();
}

