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
 
    //QColor aColor = QColor( qrand()%255, qrand()%255, qrand()%255, 100 );
    //history.setCollidingColor(aColor);
    //history.setFreeColor(aColor);
    
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
    
    QVector<qreal> pose = bodypart->getNormPose();
    Point_d q(pose.size(),pose.begin(),pose.end());
    
    QVector3D marker_position = marker->node()->getPos();
    
    //Vector_d xm,ym,zm;                         // joi
    //QVector3D x(1,0,0), y(0,1,0), z(0,0,1);
	//double  xdm = -1.0, ydm = -1.0, zdm = 0.0;
    
    keepRunning = true;
    
    Point_d x=q, y=q, z=q, nx, ny, nz;
    double xl=0.0,yl=0.0,zl=0.0;
   
    std::vector<Crawler> crawlers;
    crawlers.push_back(Crawler(this,q,Vector_3(1,0,0),Qt::blue));
    crawlers.push_back(Crawler(this,q,Vector_3(0,1,0),Qt::green));
    crawlers.push_back(Crawler(this,q,Vector_3(0,0,1),Qt::red));
    crawlers.push_back(Crawler(this,q,Vector_3(-1,0,0),Qt::blue));
    crawlers.push_back(Crawler(this,q,Vector_3(0,-1,0),Qt::green));
    crawlers.push_back(Crawler(this,q,Vector_3(0,0,-1),Qt::red));
                                          
	while ( keepRunning )
	{
        std::vector<Crawler> newCrawlers;
        for (std::vector<Crawler>::iterator i = crawlers.begin(); i!= crawlers.end(); ++i )
        {
            q = i->next();
            if (i->getLen()>0.1)
            {
                if (i->getDelta() == Vector_3(1,0,0) || i->getDelta() == Vector_3(-1,0,0) ) {
                    newCrawlers.push_back(Crawler(this,q,Vector_3(0,1,0),Qt::green));
                    newCrawlers.push_back(Crawler(this,q,Vector_3(0,0,1),Qt::red));
                    newCrawlers.push_back(Crawler(this,q,Vector_3(0,-1,0),Qt::green));
                    newCrawlers.push_back(Crawler(this,q,Vector_3(0,0,-1),Qt::red));
                }
                else if (i->getDelta() == Vector_3(0,1,0) || i->getDelta() == Vector_3(0,-1,0)) {
                    newCrawlers.push_back(Crawler(this,q,Vector_3(1,0,0),Qt::blue));
                    newCrawlers.push_back(Crawler(this,q,Vector_3(0,0,1),Qt::red));
                    newCrawlers.push_back(Crawler(this,q,Vector_3(-1,0,0),Qt::blue));
                    newCrawlers.push_back(Crawler(this,q,Vector_3(0,0,-1),Qt::red));
                }
                else if (i->getDelta() == Vector_3(0,0,1) || i->getDelta() == Vector_3(0,0,-1)) {
                    newCrawlers.push_back(Crawler(this,q,Vector_3(1,0,0),Qt::blue));
                    newCrawlers.push_back(Crawler(this,q,Vector_3(0,1,0),Qt::green));
                    newCrawlers.push_back(Crawler(this,q,Vector_3(-1,0,0),Qt::blue));
                    newCrawlers.push_back(Crawler(this,q,Vector_3(0,-1,0),Qt::green));
                }
            }
            
        }
        
        printf("\n");
        
        for (std::vector<Crawler>::iterator i = newCrawlers.begin(); i!= newCrawlers.end(); ++i )
            crawlers.push_back(*i);
	}
}

Vector_d PoseFinder::dDelta( bool advance )
{
    CGAL::Random_points_on_sphere_d<Point_d> localSphere(bodypart->size(), 0.01);
    return *localSphere++ - CGAL::ORIGIN;
}

void PoseFinder::setNormPose( Point_d q )
{
    int n = 0;
    QVector<qreal> cmd;
    for ( Point_d::Cartesian_const_iterator i = q.cartesian_begin(); i != q.cartesian_end(); ++i )
        cmd.push_back(*i);
    bodypart->setNormPos(cmd);
    model.computePose();
}

bool PoseFinder::sample( Vector_d& x, Vector_d& y, Vector_d& z )
{
    QVector<qreal> pose = bodypart->getNormPose();
    Point_d q(pose.size(),pose.begin(),pose.end());
    QVector3D marker_pose = marker->node()->getPos();
    
    CGAL::Random_points_on_sphere_d<Point_d> localSphere(bodypart->size(), 0.1);
    Vector_d x_winner = *localSphere - CGAL::ORIGIN;
    Vector_d y_winner = *localSphere - CGAL::ORIGIN;
    Vector_d z_winner = *localSphere - CGAL::ORIGIN;
    QVector3D dx(0,0,0);
    QVector3D dy(0,0,0);
    QVector3D dz(0,0,0);
    double xmax = -1.0;
    double ymax = -1.0;
    double zmax = -1.0;
    
    for (int n=0; n<1000; n++)
    {
                                                                        //printf("..%d",n);
        Vector_d candidate = *localSphere++ - CGAL::ORIGIN;             //std::cout << "  candidate: " << candidate << std::endl;
        
        setNormPose(q + candidate);
        
        QVector3D new_marker_pose = marker->node()->getPos();
        QVector3D delta = new_marker_pose - marker_pose;                   //printf(" dx(%f %f %f) = new_pose(%f %f %f) - old_pose(%f %f %f)\n",
                                                                        //dx.x(),dx.y(),dx.z(),new_marker_pose.x(),new_marker_pose.y(),
                                                                        //new_marker_pose.z(),marker_pose.x(),marker_pose.y(),marker_pose.z());
                                                                        //printf("dx.len() = %f\n",dx.length());
        double xdot = QVector3D::dotProduct(QVector3D(1,0,0), delta/delta.length());
        double ydot = QVector3D::dotProduct(QVector3D(0,1,0), delta/delta.length());
        double zdot = QVector3D::dotProduct(QVector3D(0,0,1), delta/delta.length());
                                                                        //std::cout << "  dot_prod: " << dot_prod << std::endl;
        if ( xdot > xmax) {
            xmax = xdot;
            x_winner = candidate;
            dx = delta;
        } else if ( ydot > ymax) {
            ymax = ydot;
            y_winner = candidate;
            dy = delta;
        } else if ( zdot > zmax) {
            zmax = zdot;
            z_winner = candidate;
            dz = delta;
        }
    }
    
}

Crawler::Crawler(PoseFinder* _finder, Point_d _q, Vector_3 _delta, QColor _c): finder(_finder),q(_q),delta(_delta),c(_c),len(0.0) //,localSphere(_q.dimension(), 0.1)
{
    //CGAL::Random_points_on_sphere_d<Point_d> localSphere(finder->bodypart->size(), 0.1);
}
Point_d Crawler::next()
{
    CGAL::Random_points_on_sphere_d<Point_d> localSphere(finder->bodypart->size(), 0.1);
    double max = -1.0;
    //double len = 0.0;
    finder->setNormPose(q);
    QVector3D marker_pose = finder->marker->node()->getPos();
    
    KinematicModel::CompositeObject* obj = new KinematicModel::CompositeObject( finder->model.OBSTACLE(), finder->model.GHOST() );
    KinematicModel::PrimitiveObject* primitive = new KinematicModel::Sphere( 0.01 );
    primitive->setCollidingColor(c);
    primitive->setFreeColor(c);
    primitive->translate(marker_pose);
    obj->appendPrimitive(primitive);
    finder->model.appendObject(obj);
    
    //printf("----------------------\n");
    Vector_d winner = *localSphere - CGAL::ORIGIN;
    QVector3D winner_dx(0,0,0);
    for (int n=0; n<1000; n++)
    {
                //printf("..%d",n);
        Vector_d candidate = *localSphere++ - CGAL::ORIGIN;
                //std::cout << "  candidate: " << candidate << std::endl;
        finder->setNormPose(q + candidate);
        QVector3D new_marker_pose = finder->marker->node()->getPos();
        QVector3D dx = new_marker_pose - marker_pose;
            //printf(" dx(%f %f %f) = new_pose(%f %f %f) - old_pose(%f %f %f)\n", dx.x(),dx.y(),dx.z(),new_marker_pose.x(),new_marker_pose.y(),new_marker_pose.z(),marker_pose.x(),marker_pose.y(),marker_pose.z());
            //printf("dx.len() = %f\n",dx.length());
        double dot_prod = QVector3D::dotProduct(QVector3D(delta.x(),delta.y(),delta.z()), dx/dx.length());
                //std::cout << "  dot_prod: " << dot_prod << std::endl;
        if ( dot_prod > max) {
            max = dot_prod;
            winner = candidate;
            winner_dx = dx;
        }
    }
    //printf("delta: %f %f %f len: %f\n", delta.x(), delta.y(), delta.z(), len);
    q = q + winner;
    //printf("                len(%f) += dx_len(%f)\n",len,winner_dx.length());
    len += winner_dx.length();
    //printf("                len = %f\n",len);
    
    return q;
}

