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

class PoseFinder : public QThread
{
	
public:
    PoseFinder( char* _robot, char* _part );
	~PoseFinder() {}
    
    void setNormPose( Point_d q );
	void stop();
	
private:

    KinematicModel::Model                       model;	// the world model
    KinematicModel::Robot*                      robot;
    KinematicModel::BodyPart*                   bodypart;
    KinematicModel::Marker*                     marker;
    KinematicModel::CompositeObject             history;
    
    bool keepRunning;
    
	void run();
    Point_d next(Point_d q, Vector_3 delta, QColor c = Qt::green );
    
    Vector_d dDelta( bool advance = true);
};

#endif
/** @} */