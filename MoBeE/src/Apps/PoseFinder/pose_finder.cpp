#include "pose_finder.h"
#include "sphere.h"
#include "cylinder.h"
#include "box.h"
#include "util.h"
#include "learner.h"

PoseFinder::PoseFinder( char* _robot, char* _part) : history( model.OBSTACLE(), model.GHOST() )
{
    //model.setSyncGraphics(false);
	//model.start();
    //while ( !model.isRunning() ) {}
    //model.stop();
    
    robot = model.loadRobot(QString(_robot), false );
    robot->home();
    model.computePose();
    printf("Home pose collisions: %d\n",model.getNumCollisions());    
    
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
    std::vector< std::pair<Point_d, QVector3D> > samples = random_sample( 100 );
    std::vector< std::pair<Point_d, QVector3D> >::iterator i;
    
    std::list< std::pair<Point_d, double> > list;
    std::list< std::pair<Point_d, double> >::iterator j;
    
    for (i=samples.begin(); i!=samples.end(); ++i)
    {
        std::vector< std::pair<Vector_d,QVector3D> > local_basis = sample_neighborhood(*i, 100, 0.1);
        std::pair<KinematicModel::CompositeObject*,double> rose = make_rose(i->first, argmaxBasis(local_basis), Qt::blue, Qt::green, Qt::red);
        std::pair<Point_d,double> q(i->first,rose.second);
        
        //std::vector< std::pair<Vector_d,QVector3D> > jt_basis = jtBasis();
        //std::pair<KinematicModel::CompositeObject*,double> rose2 = make_rose(i->first, jt_basis, Qt::cyan, Qt::cyan, Qt::cyan);
        
        bool wrote_q = false;
        for (j=list.begin(); j!=list.end(); ++j) {
            if ( q.second > j->second ) {
                list.insert(j,q);
                wrote_q = true;
                break;
            }
        }
        if ( !wrote_q /*&& q.second != 0.0*/ )
            list.push_back(q);
        
        rose.first->kill();
    }
    
    
    // instantiate reinforcement learner and don't try to connect to MoBeE
    Learner learner(16,"icubSim","right_arm",false);
    
    printf("Controlability values:\n");
    int k=0;
    for (j=list.begin(); j!=list.end() && k < 10; ++j, ++k)
    {
        learner.appendState(j->first, 0);
        std::vector< std::pair<Vector_d,QVector3D> > local_basis = sample_neighborhood(map_q(j->first), 1000, 0.1);
        std::pair<KinematicModel::CompositeObject*,double> rose = make_rose(j->first, argmaxBasis(local_basis), Qt::blue, Qt::green, Qt::red);
        printf("\t%f\n",j->second);
    }
    learner.writeStateFile();
}

/*void PoseFinder::runBasis( Point_d _q, Do_What w, QColor cx, QColor cy, QColor cz )
{
    // sample in dim dimensions
    //int dim = 4;
    //CGAL::Random_points_in_cube_d<Point_d> globalCube(dim,0.5);
    //setNormPose(*globalCube++ + correction(dim,0.5));
    
    setNormPose(_q);
    
    printf("--------------------------\n");
    //printf("collisions: %d\n",model.getNumCollisions());
    if (model.getNumCollisions() != 0 || !bodypart->evaluateConstraints()) {
        printf("runBasis() aborted because");
        if (model.getNumCollisions() != 0) printf(" of self collision... ");
        if (!bodypart->evaluateConstraints()) printf(" of joint angle constraints... ");
        return;
    }
    
    QVector<qreal> pose = bodypart->getNormPose();
    Point_d q(pose.size(),pose.begin(),pose.end());
    
    std::vector< std::pair<Vector_d,QVector3D> > basis;
    //if ( w == ARGMAX_BASIS )        basis = argmaxBasis();
    //else if ( w == SUM_BASIS )      basis = sumBasis();
    //else if ( w == JT_BASIS )       basis = jtBasis();
    //else                            basis = argmaxBasis();
    
    double controlability = make_rose(q,basis,cx,cy,cz);
    
    printf("Controlability: %f\n",controlability);
    
    setNormPose(q);
    
    //std::vector< std::pair<Vector_d,QVector3D> > sum_basis = sumBasis();
    //visualize_mapping(q,sum_basis,Qt::cyan,Qt::cyan,Qt::cyan);

    //setNormPose(q);
    //std::vector< std::pair<Vector_d,QVector3D> > jt_basis = jtBasis();
    //visualize_mapping(q,jt_basis,Qt::magenta,Qt::magenta,Qt::magenta);
}*/

std::pair<KinematicModel::CompositeObject*,double> PoseFinder::make_rose(Point_d q, std::vector< std::pair<Vector_d,QVector3D> > basis, QColor xc, QColor yc, QColor zc)
{
    setNormPose(q);
    
    //printf("--------------------------\n");
    //printf("collisions: %d\n",model.getNumCollisions());
    //if (model.getNumCollisions() != 0 || !bodypart->evaluateConstraints()) {
        //printf("runBasis() aborted because");
        //if (model.getNumCollisions() != 0) printf(" of self collision... ");
        //if (!bodypart->evaluateConstraints()) printf(" of joint angle constraints... ");
    //    return;
    //}
    
    KinematicModel::CompositeObject* obj = new KinematicModel::CompositeObject( model.GHOST(), model.GHOST() );
    
    double xLen = make_ray(q, basis, QVector3D(1,0,0), xc, obj);
    xLen += make_ray(q, basis, QVector3D(-1,0,0), xc, obj);
    //printf("xQuality: %f\n",xLen);
    
    double yLen = make_ray(q, basis, QVector3D(0,1,0), yc, obj);
    yLen+= make_ray(q, basis, QVector3D(0,-1,0), yc, obj);
    //printf("yQuality: %f\n",yLen);
    
    double zLen = make_ray(q, basis, QVector3D(0,0,1), zc, obj);
    zLen += make_ray(q, basis, QVector3D(0,0,-1), zc, obj);
    //printf("zQuality: %f\n",zLen);
    
    /*make_ray(q, basis, QVector3D(1,1,0), Qt::white);
     make_ray(q, basis, QVector3D(1,0,1), Qt::white);
     make_ray(q, basis, QVector3D(0,1,1), Qt::white);
     make_ray(q, basis, QVector3D(1,1,1), Qt::white);
     make_ray(q, basis, QVector3D(-1,-1,0), Qt::white);
     make_ray(q, basis, QVector3D(-1,0,-1), Qt::white);
     make_ray(q, basis, QVector3D(0,-1,-1), Qt::white);
     make_ray(q, basis, QVector3D(-1,-1,-1), Qt::white);*/
    
    model.appendObject(obj);
    model.computePose();
    
    double min=xLen,med=xLen,max=xLen;
    
    return std::pair<KinematicModel::CompositeObject*,double>(obj,xLen*yLen*zLen);
}

double PoseFinder::make_ray( Point_d q, std::vector< std::pair<Vector_d,QVector3D> > basis, QVector3D dir, QColor clr, KinematicModel::CompositeObject* obj )
{    
    setNormPose(q);
    //if ( model.getNumCollisions() != 0 )
    //    return 0.0;
    
    //KinematicModel::CompositeObject* obj = new KinematicModel::CompositeObject( model.GHOST(), model.GHOST() );
    
    QVector3D   root_marker_pose = marker->node()->getPos();
    QVector3D   pose0 = root_marker_pose,
                pose1 = root_marker_pose;
    double      integral = 0.0,
                len = 0.0;
    
    //printf("----------------------\n");
    for ( int i=0; i<50; i++ )
    {
        double scale = (double)(i+1)/10;
        Vector_d dq = project(basis, dir);   //std::cout << "q: " << q << std::endl; std::cout << "dq: " << dq << std::endl;
        setNormPose(q+scale*dq);             //printf(" x:%f, y:%f z:%f\n", marker_pose.x(),marker_pose.y(),marker_pose.z());
        if ( model.getNumCollisions() != 0 || !bodypart->evaluateConstraints() ) {
            //printf("makeRay() returned because");
            //if (model.getNumCollisions() != 0) printf(" of self collision... ");
            //if (!bodypart->evaluateConstraints()) printf(" joint angle constraints... ");
            //printf("\n");
            break;
        }
        
        pose0 = pose1;
        pose1 = marker->node()->getPos();
        QVector3D delta = pose1-pose0;
        
        // compute this part of the integral
        double a = pose0.distanceToLine(root_marker_pose, dir);
        double b = pose1.distanceToLine(root_marker_pose, dir);     // distance error
        double c = QVector3D::dotProduct(delta,dir/dir.length());
        
        
        // angular error
        double angl_err = fabs((QVector3D::dotProduct(dir/dir.length(), delta/delta.length()) - 1))/2 ;

        //printf("\tdistance err: %f,\t angl_err: %f,\t integral: %f\n",b,angl_err,integral);
        
        QColor color;
        if (b < 0.1 && angl_err < 0.1) {
            color = clr;
            len += c;
            integral += a*c + c/2 * (a+b);
        }
        else {
            color = Qt::white;
            break;
        }
        
        KinematicModel::PrimitiveObject* primitive = new KinematicModel::Sphere( 0.005 );
        primitive->setCollidingColor(color);
        primitive->setFreeColor(color);
        primitive->translate(pose1);
        obj->appendPrimitive(primitive);
    }
    return len*len/(integral+0.001);
}

int PoseFinder::setNormPose( Point_d q )
{
    int n = 0;
    QVector<qreal> cmd;
    for ( Point_d::Cartesian_const_iterator i = q.cartesian_begin(); i != q.cartesian_end(); ++i )
        cmd.push_back(*i);
    bodypart->setNormPos(cmd);
    model.computePose();
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
    
    //std::cout << "dqx: " << dqx << std::endl;
    //std::cout << "dqy: " << dqy << std::endl;
    //std::cout << "dqz: " << dqz << std::endl;
    
    basis.push_back(std::pair<Vector_d,QVector3D>(dqx,x));
    basis.push_back(std::pair<Vector_d,QVector3D>(dqy,y));
    basis.push_back(std::pair<Vector_d,QVector3D>(dqz,z));
    
    return basis;
}

std::vector< std::pair<Vector_d,QVector3D> > PoseFinder::argmaxBasis( std::vector< std::pair<Vector_d,QVector3D> > big_basis )
{
    QVector<qreal> pose = bodypart->getNormPose();
    Point_d q(pose.size(),pose.begin(),pose.end());
    QVector3D marker_pose = marker->node()->getPos();
    
    Vector_d xdq,ydq,zdq;
    QVector3D xdx,ydx,zdx;
    
    
    double xmax = -1.0, ymax = -1.0, zmax = -1.0;
    double xdot,ydot,zdot;
    
    for (std::vector< std::pair<Vector_d,QVector3D> >::iterator i=big_basis.begin(); i!=big_basis.end(); i++)
    {
        xdot = QVector3D::dotProduct(i->second, QVector3D(1,0,0));
        ydot = QVector3D::dotProduct(i->second, QVector3D(0,1,0));
        zdot = QVector3D::dotProduct(i->second, QVector3D(0,0,1));
        
        if ( xdot > xmax ) {
            xmax = xdot;
            xdq = i->first;
            xdx = i->second;
        } else if ( ydot > ymax ) {
            ymax = ydot;
            ydq = i->first;
            ydx = i->second;
        } else if ( zdot > zmax ) {
            zmax = zdot;
            zdq = i->first;
            zdx = i->second;
        }
    }
 
    std::vector< std::pair<Vector_d,QVector3D> > bsis;
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

std::pair<Point_d, QVector3D> PoseFinder::map_q( Point_d _q ) {
    setNormPose(_q);
    QVector<qreal> pose = bodypart->getNormPose();
    Point_d q(pose.size(),pose.begin(),pose.end());
    QVector3D p = marker->node()->getPos();
    return std::pair<Point_d,QVector3D>(q,p);
}

std::vector< std::pair<Point_d, QVector3D> > PoseFinder::random_sample(int n)
{
    // sample in dim dimensions
    int dim = 4;
    std::vector< std::pair<Point_d, QVector3D> > samples;
    
    for ( int i=0; i<n; i++ ) {
        CGAL::Random_points_in_cube_d<Point_d> globalCube(dim,0.5);
        Point_d s = *globalCube++ + correction(dim,0.5);
        //setNormPose(s);
        //QVector<qreal> pose = bodypart->getNormPose();
        //Point_d q(pose.size(),pose.begin(),pose.end());
        //QVector3D p = marker->node()->getPos();
        samples.push_back(map_q(s));
    }
    return samples;
}

std::vector< std::pair< Vector_d, QVector3D > > PoseFinder::sample_neighborhood( std::pair<Point_d, QVector3D> qp, int n, double r )
{
    setNormPose(qp.first);
    
    CGAL::Random_points_on_sphere_d<Point_d> localSphere(bodypart->size(), r); localSphere++;
    std::vector< std::pair< Vector_d, QVector3D > > samples;
    for ( int i=0; i<n; i++)
    {
        Point_d s = *localSphere++;
        //std::cout << "s: " << s <<std::endl;
        Vector_d delta = *localSphere++ - CGAL::ORIGIN;
        setNormPose(qp.first+delta);
        QVector3D marker_delta = marker->node()->getPos() - qp.second;
        samples.push_back( std::pair<Vector_d,QVector3D>(delta/sqrt(delta.squared_length()), marker_delta/marker_delta.length()) );
    }
    return samples;
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



/*******************************************************/

/*void PoseFinder::runGrid()
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
}*/
