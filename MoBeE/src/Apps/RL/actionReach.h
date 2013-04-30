/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RL
 *	@{
 */

#ifndef REACHACTION_H_
#define REACHACTION_H_ACTION_H_

#include "action.h"
//#include "primitiveObject.h"

class ReachAction : public Action
{
/* These actions reach for objects, and are responsible for rewarding the learner. In this way the learner learns which 
   states (roadmap nodes) are the good ones from which to reach for objects in the environment */
    friend class Learner;
    friend class State;
    
public:
    
    void predictReward( Point_3 p );
    void start( Point_3 p ) { reachTarget = p; yarp::os::RateThread::start(); }
    
private:
    
    ReachAction(    Learner* l,
                    yarp::os::ConstString m,
                    State* p,
                    double value = 0.0,
                    double reward = 0.0,
                    int numTries = 0,
                    int rate=50 ) : Action(l,p,value,reward,numTries,rate),
                                    marker(m),
                                    reachTarget(0,0,0),
                                    forceGain(1.0),
                                    torqueGain(1.0),
                                    forceTimeout(timeout/2){}
    ~ReachAction(){}
    
    void appendToHistory( Point_3 p, double r ){ history.push_back( std::pair<Point_3,double>(p,r) ); }
    
    //double predictReward(Point_3);
    
    //void runReach(Point_3 p)
    //Point_3 easyReach();

    int tempIdx;
    yarp::os::ConstString marker;
    //KinematicModel::PrimitiveObject* e,n;
    
    double forceGain,torqueGain;
    double forceTimeout;
    
    Point_3 reachTarget;
    yarp::os::ConstString mobeeObjectName,eName,nName;
    std::vector< std::pair<Point_3,double> > history;
    //std::vector<Point_3> history;
    
    //void start() { Action::start(); }
    
    //double getErr();
    bool sendForceCommand(); // false if the error is gone
    bool threadInit();
    void threadRelease();
    void run();
    
};

#endif
/** @} */
