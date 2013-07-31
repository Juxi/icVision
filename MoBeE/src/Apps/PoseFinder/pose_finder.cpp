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
    
    do_what = ARGMAX_BASIS;
    
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
    if ( do_what == GRID ) runGrid();
    else if ( do_what == ARGMAX_BASIS ) runBasis(ARGMAX_BASIS);
    else if ( do_what == SUM_BASIS ) runBasis(SUM_BASIS);
}

void PoseFinder::runBasis( Do_What w )
{
    
    
    // sample in dim dimensions
    int dim = 4;
    
    CGAL::Random_points_in_cube_d<Point_d> globalCube(dim,0.5);
    setNormPose(*globalCube++ + correction(dim,0.5));
    
    QVector<qreal> pose = bodypart->getNormPose();
    Point_d q(pose.size(),pose.begin(),pose.end());
    
    std::vector< std::pair<Vector_d,QVector3D> > argmax_basis = argmaxBasis();
    make_rose(q,argmax_basis,Qt::blue,Qt::green,Qt::red);
    
    setNormPose(q);
    //std::vector< std::pair<Vector_d,QVector3D> > sum_basis = sumBasis();
    //visualize_mapping(q,sum_basis,Qt::cyan,Qt::cyan,Qt::cyan);

    //setNormPose(q);
    //std::vector< std::pair<Vector_d,QVector3D> > jt_basis = jtBasis();
    //visualize_mapping(q,jt_basis,Qt::magenta,Qt::magenta,Qt::magenta);
}

void PoseFinder::make_ray( Point_d q, std::vector< std::pair<Vector_d,QVector3D> > basis, QVector3D dir, QColor c )
{
    KinematicModel::CompositeObject* obj = new KinematicModel::CompositeObject( model.OBSTACLE(), model.GHOST() );
    for ( int i=0; i<10; i++ )
    {
        double scale = (double)(i+1)/10;
        
        // X
        Vector_d dq = project(basis, dir);    //std::cout << "dq***: " << dq << std::endl;
        std::cout << "q: " << q << std::endl;
        std::cout << "dq: " << dq << std::endl;
        setNormPose(q+scale*dq);                                          //printf(" x:%f, y:%f z:%f\n",
        //marker_pose.x(),marker_pose.y(),marker_pose.z());
        QVector3D marker_pose = marker->node()->getPos();
        KinematicModel::PrimitiveObject* primitive = new KinematicModel::Sphere( 0.005 );
        primitive->setCollidingColor(c);
        primitive->setFreeColor(c);
        primitive->translate(marker_pose);
        obj->appendPrimitive(primitive);
    }
    model.appendObject(obj);
        
}

void PoseFinder::make_rose(Point_d q, std::vector< std::pair<Vector_d,QVector3D> > basis, QColor xc, QColor yc, QColor zc)
{
    make_ray(q, basis, QVector3D(1,0,0), Qt::blue);
    make_ray(q, basis, QVector3D(-1,0,0), Qt::darkBlue);
    
    make_ray(q, basis, QVector3D(0,1,0), Qt::green);
    make_ray(q, basis, QVector3D(0,-1,0), Qt::darkGreen);
    
    make_ray(q, basis, QVector3D(0,0,1), Qt::red);
    make_ray(q, basis, QVector3D(0,0,-1), Qt::darkRed);
    
    make_ray(q, basis, QVector3D(1,1,0), Qt::white);
    make_ray(q, basis, QVector3D(1,0,1), Qt::white);
    make_ray(q, basis, QVector3D(0,1,1), Qt::white);
    make_ray(q, basis, QVector3D(1,1,1), Qt::white);
    make_ray(q, basis, QVector3D(-1,-1,0), Qt::white);
    make_ray(q, basis, QVector3D(-1,0,-1), Qt::white);
    make_ray(q, basis, QVector3D(0,-1,-1), Qt::white);
    make_ray(q, basis, QVector3D(-1,-1,-1), Qt::white);
    
    model.computePose();
}

void PoseFinder::runGrid()
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

Vector_d PoseFinder::correction(int dim,double ammount)
{
    std::vector<double> correction;
    for (int i=0; i<dim; i++) correction.push_back(ammount);
    Vector_d c(dim,correction.begin(),correction.end());
    return c;
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

std::vector< std::pair<Vector_d,QVector3D> > PoseFinder::jtBasis()
{
    std::vector< std::pair<Vector_d,QVector3D> > basis;
    QVector3D x(1,0,0), y(0,1,0), z(0,0,1);
    
    // make the J^T matrix into a basis of 3 joint space directions
    std::vector<double> q_of_x, q_of_y, q_of_z;
    QList< QPair<QVector3D, QVector3D> > J = marker->node()->computeJacobian();
    int dims = 0;
    for (QList< QPair<QVector3D, QVector3D> >::iterator i = J.begin(); i!=J.end(); ++i)
    {
        q_of_x.push_back( QVector3D::dotProduct( i->first, x ) );
        q_of_y.push_back( QVector3D::dotProduct( i->first, y ) );
        q_of_z.push_back( QVector3D::dotProduct( i->first, z ) );
        dims++;
    }
    while (dims < bodypart->size()) {
        q_of_x.push_back( 0.0 );
        q_of_y.push_back( 0.0 );
        q_of_z.push_back( 0.0 );
        dims++;
    }
    
    Vector_d dqx(q_of_x.size(),q_of_x.begin(),q_of_x.end());
    Vector_d dqy(q_of_y.size(),q_of_y.begin(),q_of_y.end());
    Vector_d dqz(q_of_z.size(),q_of_z.begin(),q_of_z.end());
    
    dqx/=sqrt(dqx.squared_length());
    dqy/=sqrt(dqz.squared_length());
    dqy/=sqrt(dqz.squared_length());
    
    std::cout << "dqx: " << dqx << std::endl;
    std::cout << "dqy: " << dqy << std::endl;
    std::cout << "dqz: " << dqz << std::endl;
    
    basis.push_back(std::pair<Vector_d,QVector3D>(dqx,x));
    basis.push_back(std::pair<Vector_d,QVector3D>(dqy,y));
    basis.push_back(std::pair<Vector_d,QVector3D>(dqz,z));
    
    return basis;
}

std::vector< std::pair<Vector_d,QVector3D> > PoseFinder::argmaxBasis()
{
    std::vector< std::pair<Vector_d,QVector3D> > bsis;
    
    QVector<qreal> pose = bodypart->getNormPose();
    Point_d q(pose.size(),pose.begin(),pose.end());
    QVector3D marker_pose = marker->node()->getPos();
    
    Vector_d xdq,ydq,zdq;
    QVector3D xdx,ydx,zdx;
    double xmax = 0.0, ymax = 0.0, zmax = 0.0;
    double xdot,ydot,zdot;
    CGAL::Random_points_on_sphere_d<Point_d> localSphere(bodypart->size(), 0.1); localSphere++;
    for (int n=0; n<1000; n++)
    {
                                                                        //printf("..%d",n);
        Vector_d candidate = *localSphere++ - CGAL::ORIGIN;             //std::cout << "  candidate: " << candidate << std::endl;
        setNormPose(q + candidate);
        
        QVector3D new_marker_pose = marker->node()->getPos();
        QVector3D delta = new_marker_pose - marker_pose;
        delta /= delta.length();        // normalize or no???
        
        xdot = QVector3D::dotProduct(delta, QVector3D(1,0,0));
        ydot = QVector3D::dotProduct(delta, QVector3D(0,1,0));
        zdot = QVector3D::dotProduct(delta, QVector3D(0,0,1));
        
        if ( xdot > xmax ) {
            xmax = xdot;
            xdq = candidate/sqrt(candidate.squared_length());
            xdx = delta;
        } else if ( ydot > ymax ) {
            ymax = ydot;
            ydq = candidate/sqrt(candidate.squared_length());
            ydx = delta;
        } else if ( zdot > zmax ) {
            zmax = zdot;
            zdq = candidate/sqrt(candidate.squared_length());
            zdx = delta;
        }
    }
    
    std::cout << "XDX: "<< xdx.x() << " " << xdx.y() << " " << xdx.z() << std::endl;
    std::cout << "YDX: "<< ydx.x() << " " << ydx.y() << " " << ydx.z() << std::endl;
    std::cout << "ZDX: "<< zdx.x() << " " << zdx.y() << " " << zdx.z() << std::endl;
    
    bsis.push_back(std::pair<Vector_d,QVector3D>(xdq,xdx));
    bsis.push_back(std::pair<Vector_d,QVector3D>(ydq,ydx));
    bsis.push_back(std::pair<Vector_d,QVector3D>(zdq,zdx));
    
    return bsis;
}

std::vector< std::pair<Vector_d,QVector3D> > PoseFinder::sumBasis()
{
    std::vector< std::pair<Vector_d,QVector3D> > bsis;
    
    QVector<qreal> pose = bodypart->getNormPose();
    Point_d q(pose.size(),pose.begin(),pose.end());
    QVector3D marker_pose = marker->node()->getPos();
    
    CGAL::Random_points_on_sphere_d<Point_d> localSphere(bodypart->size(), 0.1); localSphere++;
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
        bsis.push_back(std::pair<Vector_d,QVector3D>(candidate/sqrt(candidate.squared_length()),
                                                     delta/delta.length()));
        
    }
    
    return bsis;
}


Vector_d PoseFinder::project(std::vector< std::pair<Vector_d,QVector3D> > basis, QVector3D dx)
{
    dx /= dx.length();
    Vector_d v(basis.begin()->first.dimension(),CGAL::NULL_VECTOR);
    for (std::vector< std::pair<Vector_d,QVector3D> >::iterator i=basis.begin(); i!=basis.end(); ++i )
    {
        //printf("(%f %f %f)*(%f %f %f)\n",i->second.x(),i->second.y(),i->second.z(),dx.x(),dx.y(),dx.z());
        double a = QVector3D::dotProduct(i->second, dx);
        v += a * i->first;
    }
    
    //std::cout<< std::endl << "v : " << v << std::endl;
    return v/sqrt(v.squared_length());
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

