/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup WorldModelDemo
 *	@{
 */

#ifndef OBJECTSOUP_H_
#define OBJECTSOUP_H_

#include <QThread>
#include "model.h"
#include "util.h"

//class KinematicModel::Model;

class Sample
{
	
public:
	Sample( KinematicModel::Model&      m,
           KinematicModel::BodyPart*    b,
           Point_d                      q,
           int                          n = 0,
           bool                         v = false );
	~Sample() {}
	
    void print();
	void makeBalls();
    QVector<Point_d> project();
        
	void stop();
	
private:
	KinematicModel::Model&                      model;	// the world model
    KinematicModel::BodyPart*                   bodypart;
    
    Point_d                                     config;
    QVector<KinematicModel::Marker*>            markers;
    QVector<KinematicModel::CompositeObject*>   clouds;
    QVector<QVector3D>                          pose;
    
    QVector<Vector_d>                           delta;      // joint space direction vector
    QVector< QVector<Vector_3> >                delta_pose; // marker positions
    QVector< QVector<double> >                dot_prod;
    double                                      max_dx;
    
    
    
	int num;							// number of CompositeObjects in the model
	bool verbose;
	bool keepRunning;
	//int i,j;							// loop indices
	//int numPrimitives;					// the number of primitives in each CompositeObject (randomly selected for each CompostieObject)
	//KinematicModel::GeomType geomType;	// the type of primitive (randomly selected for each primitive)
	//GLfloat color[4];					// the color of each primitive
    
    
    CGAL::Random_points_on_sphere_d<Point_d> localSphere;
	//CGAL::Random_points_on_sphere_d<Point_d> localSphere;
  
    
    void setNormPose( Point_d q ) {
        int n = 0;
        QVector<qreal> cmd;
        for ( Point_d::Cartesian_const_iterator i = q.cartesian_begin(); i != q.cartesian_end(); ++i )
            cmd.push_back(*i);
        bodypart->setNormPos(cmd);
    }
};

#endif
/** @} */