/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup WorldModelDemo
 *	@{
 */

#ifndef POSEFINDER_H_
#define POSEFINDER_H_

#include <QThread>
#include "model.h"
#include "util.h"

//class KinematicModel::Model;
class PoseFinder;

class Crawler
{
    
public:
    Crawler(PoseFinder* _finder,
            Point_d _q,
            Vector_3 _delta,
            QColor _c = Qt::white);
    ~Crawler()
    {
    }
    
    Point_d next();
    double getLen() {return len;}
    Vector_3 getDelta() {return delta;}
    Point_d get_q() { return q; }
    
private:
    PoseFinder* finder;
    Point_d q;
    Vector_3 delta;
    QColor c;
    double len;
    //KinematicModel::Model* model;
    //KinematicModel::BodyPart* bodypart;
    //CGAL::Random_points_on_sphere_d<Point_d> localSphere;
};


class PoseFinder : public QThread
{
	
public:
    PoseFinder( char* _robot, char* _part );
	~PoseFinder() {}
    
    void setNormPose( Point_d q );
    bool sample( Vector_d& x, Vector_d& y, Vector_d& z );
	void stop();
	
private:
    
    friend class Crawler;

    KinematicModel::Model                       model;	// the world model
    KinematicModel::Robot*                      robot;
    KinematicModel::BodyPart*                   bodypart;
    KinematicModel::Marker*                     marker;
    KinematicModel::CompositeObject             history;
    
    bool keepRunning;
    
	void run();
    //Point_d next(Point_d q, Vector_3 delta, QColor c = Qt::green );
    
    Vector_d dDelta( bool advance = true);
};

#endif
/** @} */