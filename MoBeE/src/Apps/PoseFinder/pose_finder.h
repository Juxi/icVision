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

//typedef QList< QPair<QVector3D, QVector3D> > Jacobian;

//class KinematicModel::Model;
class PoseFinder;

/*class Crawler
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
};*/


class PoseFinder : public QThread
{
	
public:
    PoseFinder( char* _robot, char* _part );
	~PoseFinder() {}
    
    enum Do_What { ARGMAX_BASIS, SUM_BASIS, JT_BASIS, RAND_SAMPLE, GRID };
    Do_What do_what;
    
    int setNormPose( Point_d q );
    
    std::vector< std::pair<Point_d, QVector3D> > random_sample(int n);
    std::vector< std::pair<Vector_d, QVector3D> > sample_neighborhood( std::pair<Point_d, QVector3D>, int n, double r );
    std::vector< std::pair<Vector_d,QVector3D> > argmaxBasis(std::vector< std::pair<Vector_d,QVector3D> >);
    std::vector< std::pair<Vector_d,QVector3D> > jtBasis();
    
    std::vector< std::pair<Vector_d,QVector3D> > sumBasis();
    
    Vector_d project(std::vector< std::pair<Vector_d,QVector3D> > basis, QVector3D dx);
    
	void stop();
	
private:
    
    //friend class Crawler;

    KinematicModel::Model                       model;	// the world model
    KinematicModel::Robot*                      robot;
    KinematicModel::BodyPart*                   bodypart;
    KinematicModel::Marker*                     marker;
    KinematicModel::CompositeObject             history;
    
    bool keepRunning;
    
	void run();
    
    //void runGrid();
    void runBasis( Point_d q, Do_What w, QColor cx, QColor cy, QColor cz  );
    
    
    std::pair<KinematicModel::CompositeObject*,double> make_rose( Point_d q, std::vector< std::pair<Vector_d,QVector3D> > basis, QColor xc, QColor yc, QColor zc);
    double make_ray(Point_d q, std::vector< std::pair<Vector_d,QVector3D> > basis, QVector3D dir, QColor c, KinematicModel::CompositeObject* obj );
    
    Vector_d dDelta( bool advance = true);
    Vector_d correction(int dim,double ammount);
    //Point_d resize(Point_d);
};

#endif
/** @} */