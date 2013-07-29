#include "sample.h"
#include "sphere.h"
#include "cylinder.h"
#include "box.h"
#include "util.h"

Sample::Sample( KinematicModel::Model& m, KinematicModel::BodyPart* bp, Point_d q, int n, bool v ) : model(m), bodypart(bp), config(q), num(n), verbose(v), keepRunning(true), localSphere(config.dimension(), 0.15)
{
    QTime time = QTime::currentTime();
	qsrand((uint)time.msec());
    
    setNormPose(config);
    model.computePose();
    
    // get op-space marker positions
    markers = bodypart->getMarkers();
    int markernum=0;
    for (QVector<KinematicModel::Marker*>::iterator i = markers.begin(); i!=markers.end(); ++i )
    {
        //printf("marker num: %d\n", markernum++);
        QColor aColor = QColor( qrand()%255, qrand()%255, qrand()%255, 100 );
        
        KinematicModel::CompositeObject* obj = new KinematicModel::CompositeObject( model.OBSTACLE(), model.GHOST() );
        KinematicModel::PrimitiveObject* primitive = new KinematicModel::Sphere( 0.01 );
        
        obj->translate((*i)->node()->getPos());
        obj->appendPrimitive(primitive);
        obj->setCollidingColor(aColor);
        obj->setFreeColor(aColor);
        
        clouds.append(obj);
    }
    
    // sample the joint-space neighborhood
    for (int i=0; i<0; i++) {
        Vector_d dq = *localSphere++ - CGAL::ORIGIN;
        //std::cout << "dq: " << dq << std::endl;
        delta.append( dq );
    }
    
    // compute more op-space marker positions
    max_dx = 0.0;
    
    
    for ( QVector<Vector_d>::iterator i = delta.begin(); i!=delta.end(); ++i)
    {
        Point_d q = config + *i;
        setNormPose(q);
        model.computePose();
        
        //QVector3D x(1,0,0);
        double dx_sum = 0.0;
        //QVector<double> this_dot_prod;
        QVector<Vector_3> this_pose_delta;
        QVector<KinematicModel::Marker*>::iterator marker_it = markers.begin();
        QVector<KinematicModel::CompositeObject*>::iterator cloud_it = clouds.begin();
        for ( ; marker_it!=markers.end() && cloud_it!=clouds.end(); ++marker_it, ++cloud_it )
        {
            QVector3D dx = (*marker_it)->node()->getPos() - (*cloud_it)->getPos();
            //this_dot_prod.append( QVector3D::dotProduct(x, dx/dx.length()) );
            dx_sum += dx.lengthSquared();
            this_pose_delta.append(Vector_3(dx.x(),dx.y(),dx.z()));
            
            KinematicModel::PrimitiveObject* primitive = new KinematicModel::Sphere( 0.01 );
            primitive->translate(dx);
            primitive->setCollidingColor( (*cloud_it)->getCollidingColor() );
            primitive->setFreeColor( (*cloud_it)->getFreeColor() );
            
            (*cloud_it)->appendPrimitive(primitive);
        }
        if (dx_sum > max_dx) {
            max_dx = dx_sum;
        }
        
        //dot_prod.append(this_dot_prod);
        delta_pose.append(this_pose_delta);
    }
    
    //std::cout << " max_dx: " << max_dx << std::endl;
    
    for (QVector<KinematicModel::CompositeObject*>::iterator cloud_it = clouds.begin(); cloud_it!=clouds.end(); ++cloud_it) {
        model.appendObject(*cloud_it);
    }
    
    //setNormPose(config);
    model.computePose();
}

void Sample::makeBalls()
{
    // make a composite object for each point cloud which represents a set of marker poses
    QVector<KinematicModel::CompositeObject*> new_clouds;
    for (QVector<KinematicModel::Marker*>::iterator i = markers.begin(); i!=markers.end(); ++i )
    {
        //printf("marker num: %d\n", markernum++);
        QColor aColor = QColor( 255, 255, 255, 255 );
        
        KinematicModel::CompositeObject* obj = new KinematicModel::CompositeObject( model.OBSTACLE(), model.GHOST() );
        
        obj->translate((*i)->node()->getPos());
        obj->setCollidingColor(aColor);
        obj->setFreeColor(aColor);
        
        new_clouds.append(obj);
    }
    
    //figure out how to spread it out
    
    // push in a direction
    Vector_d wx_dir(*delta.begin()-*delta.begin());
    Vector_d wy_dir(*delta.begin()-*delta.begin());
    Vector_d wz_dir(*delta.begin()-*delta.begin());
    QVector< QVector<Vector_3> >::iterator j = delta_pose.begin();
    for ( QVector<Vector_d>::iterator i = delta.begin(); i!=delta.end(), j!=delta_pose.end(); ++i,++j )
    {
        QVector3D delta_pose_dir(j->begin()->x(), j->begin()->y(), j->begin()->z());
        delta_pose_dir/=delta_pose_dir.length();
        double wx = fabs(QVector3D::dotProduct(QVector3D(1,0,0),delta_pose_dir));
        double wy = fabs(QVector3D::dotProduct(QVector3D(0,1,0),delta_pose_dir));
        double wz = fabs(QVector3D::dotProduct(QVector3D(0,0,1),delta_pose_dir));
        //printf("(%f %f %f)*(-1,0,0)=\t%f\n", j->begin()->x(), j->begin()->y(), j->begin()->z(),w);
        wx_dir += wx * *i;
        wy_dir += wy * *i;
        wz_dir += wz * *i;
    }
    wx_dir /= sqrt(wx_dir.squared_length());
    wx_dir *= 0.2;
    wy_dir /= sqrt(wy_dir.squared_length());
    wy_dir *= 0.2;
    wz_dir /= sqrt(wz_dir.squared_length());
    wz_dir *= 0.2;
    //std::cout << "good_dir: " << good_dir << std::endl;
    
    
    
    
    //QVector< QVector<Vector_3> >::iterator
    j = delta_pose.begin();
    for ( QVector<Vector_d>::iterator i = delta.begin(); i!=delta.end(), j!=delta_pose.end(); ++i,++j )
    {
        
        double this_dx = 0;
        for (QVector<Vector_3>::iterator k = (*j).begin(); k!=(*j).end(); ++k) {
            this_dx += k->squared_length();
            //dot_prod = QVector3D::dotProduct(QVector3D(1,0,0), QVector3D(k->x(),k->y(),k->z()) );
        }
        
        
        
        /*** PROJECT ***/
        // attenuate
        //Vector_d new_v = *i;
        //new_v *= 1-this_dx/max_dx;

        // push
        Vector_d xv = *i + wx_dir;
        Vector_d yv = *i + wy_dir;
        Vector_d zv = *i + wz_dir;
        
        Point_d qx = config + xv;
        Point_d qy = config + yv;
        Point_d qz = config + zv;
        
        //std::cout << " max_dx: " << max_dx << " this_dx: " << this_dx << " \tratio: " << max_dx/this_dx << std::endl;
        std::cout << " qx: " << qx << std::endl;
        std::cout << " qy: " << qy << std::endl;
        std::cout << " qz: " << qz << std::endl;
        
        ////////////////////////////////////////////////////////////////////////////////////////////////
        setNormPose(qx);
        model.computePose();
        
        //double dx_sum = 0.0;
        //QVector<Vector_3> this_pose_delta;
        QVector<KinematicModel::Marker*>::iterator marker_it = markers.begin();
        QVector<KinematicModel::CompositeObject*>::iterator cloud_it = new_clouds.begin();
        for ( ; marker_it!=markers.end() && cloud_it!=clouds.end(); ++marker_it, ++cloud_it )
        {
            KinematicModel::PrimitiveObject* primitive = new KinematicModel::Sphere( 0.005 );
            QVector3D dx = (*marker_it)->node()->getPos() - (*cloud_it)->getPos();
            primitive->translate(dx);
            primitive->setCollidingColor( Qt::red );
            primitive->setFreeColor( Qt::red );
            
            (*cloud_it)->appendPrimitive(primitive);
        }
        
        ////////////////////////////////////////////////////////////////////////////////////////////////
        setNormPose(qy);
        model.computePose();
        
        //double dx_sum = 0.0;
        //QVector<Vector_3> this_pose_delta;
        marker_it = markers.begin();
        cloud_it = new_clouds.begin();
        for ( ; marker_it!=markers.end() && cloud_it!=clouds.end(); ++marker_it, ++cloud_it )
        {
            KinematicModel::PrimitiveObject* primitive = new KinematicModel::Sphere( 0.005 );
            QVector3D dx = (*marker_it)->node()->getPos() - (*cloud_it)->getPos();
            primitive->translate(dx);
            primitive->setCollidingColor( Qt::green );
            primitive->setFreeColor( Qt::green );
            
            (*cloud_it)->appendPrimitive(primitive);
        }
        
        ////////////////////////////////////////////////////////////////////////////////////////////////
        setNormPose(qz);
        model.computePose();
        
        //double dx_sum = 0.0;
        //QVector<Vector_3> this_pose_delta;
        marker_it = markers.begin();
        cloud_it = new_clouds.begin();
        for ( ; marker_it!=markers.end() && cloud_it!=clouds.end(); ++marker_it, ++cloud_it )
        {
            KinematicModel::PrimitiveObject* primitive = new KinematicModel::Sphere( 0.005 );
            QVector3D dx = (*marker_it)->node()->getPos() - (*cloud_it)->getPos();
            primitive->translate(dx);
            primitive->setCollidingColor( Qt::blue );
            primitive->setFreeColor( Qt::blue );
            
            (*cloud_it)->appendPrimitive(primitive);
        }
        
        
        //delta_pose.append(this_pose_delta);
    }
    
    for ( QVector<KinematicModel::CompositeObject*>::iterator cloud_it = new_clouds.begin(); cloud_it!=new_clouds.end(); ++cloud_it ) {
        clouds.append(*cloud_it);
        model.appendObject(*cloud_it);
    }
	
    model.computePose();
}

    /*void visualize_markers()
     {
     QVector<Marker*> markers = p->getMarkers();
     for ( QVector<Marker*>::iterator j=markers.begin(); j!=markers.end(); ++j )
     {
     QColor collidingColor = Qt::white;
     QColor freeColor = collidingColor;
     freeColor.setAlphaF( 1.0 );
     
     CompositeObject* obj = new CompositeObject( m.OBSTACLE(), m.GHOST() );
     obj->setCollidingColor(collidingColor);
     obj->setFreeColor(freeColor);
     
     // add primitive
     KinematicModel::PrimitiveObject* primitive;
     primitive = new KinematicModel::Sphere( 0.005 );
     primitive->setCollidingColor( collidingColor );
     primitive->setFreeColor(freeColor);
     obj->appendPrimitive(primitive);
     obj->setPosition((*j)->node()->getPos());
     m.appendObject(obj);
     
     /*QVector<double> x; x.append(-1.0); x.append(0.0); x.append(0.0); x.append(0.0); x.append(0.0); x.append(0.0);
     QVector<double> v;
     
     if (p->projectToJointSpace((*j)->name(), x, v) )
     {
     printf("Joint space force:");
     for (QVector<double>::iterator i=v.begin(); i!=v.end(); ++i){
     printf(" %f",*i);
     }
     printf("\n");
     }
     else printf("FUCK\n");
     }
     }*/
/*void Sample::compute_dx(Point_d& q_of_max_dx, double& dx)
    {
        dx = 0.0;
        for ( QVector<Point_d>::const_iterator qi = q.begin(); qi!=q.end(); ++qi )
        {
            // compute fwd kinematics
            setPose(p, *qi);
            m.computePose();
            
            QVector<Marker*> markers = p->getMarkers();
            
            double this_dx = 0.0;
            QVector<Marker>::iterator i=q_markers.begin();
            for ( QVector<Marker*>::iterator j=markers.begin(); i!=q_markers.end() && j!=markers.end(); ++i, ++j )
                this_dx += ((*j)->node()->getPos() - i->node()->getPos()).length();
            
            if (this_dx > dx ) {
                q_of_max_dx = *qi;
                dx = this_dx;
            }
        }
        
    }
  */
void Sample::stop()
{
	printf("ObjectSoup thread is shutting down ");
	keepRunning = false;
	while ( isRunning() )
	{
		printf(".");
		msleep(1);
	}
	printf("\n");
}

void Sample::run()
{
	QTime time = QTime::currentTime();
	qsrand((uint)time.msec());
	
	
	// fill the world with objects
	KinematicModel::CompositeObject* obj[num];
	for ( int i =0; i<num; ++i )
	{
		// make a new object
		if (verbose) printf( "Composite Object: %d\n", i );	
		//obj[i] = makeARandomObjectLikeAMothaFucka();
	}
	
	/**/
	int j;
	time.start();
	while ( keepRunning )
	{
		j = qrand() % (num);
		if (verbose) printf( "replacing object: %d\n", j );
		obj[j]->kill();
		//obj[j] = makeARandomObjectLikeAMothaFucka();
		msleep(20);
	}
	
	//printf("ObjectSoup::run() returned\n");
}



void Sample::print() {
    QVector<Vector_d>::iterator dq;
    //QVector<Vector_3>::iterator dx;
    QVector< QVector<Vector_3> >::iterator dx;
    //std::cout << "delta_q: " << delta
    for ( dq=delta.begin(), dx = delta_pose.begin(); dq!=delta.end() && dx!=delta_pose.end(); ++dq, ++dx ) {
        std::cout << "dq: " << *dq << std::endl;
        std::cout << "dx: " << std::endl;
        for (QVector<Vector_3>::iterator i=dx->begin(); i!=dx->end(); ++i) {
            std::cout << "    " << i->x() << " " << i->y() << " " << i->z() << std::endl;
            
        }
    }
}

QVector<Point_d> Sample::project() {
    QVector<Point_d> cloud;
    QVector<Vector_d>::iterator dq;
    QVector< QVector<Vector_3> >::iterator dx;
    //std::cout << "delta_q: " << delta
    
    for ( dq=delta.begin(), dx = delta_pose.begin(); dq!=delta.end() && dx!=delta_pose.end(); ++dq, ++dx )
    {
        std::cout << "dq: " << *dq << std::endl;
        std::cout << "dx: " << std::endl;
        double dx_mag = 0.0;
        for (QVector<Vector_3>::iterator i=dx->begin(); i!=dx->end(); ++i) {
            std::cout << "    " << i->x() << " " << i->y() << " " << i->z() << std::endl;
            dx_mag += i->squared_length();
        }
        std::cout << "dx_max/dx_mag = " << max_dx/dx_mag << std::endl << std::endl;
        Point_d q_new = config + max_dx/dx_mag * *dq;
        cloud.append(q_new);
    }
    return cloud;
}
