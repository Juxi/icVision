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
    
    double predictReward( Point_3 p );
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
                                    markerPos(0,0,0),
                                    forceGain(1.0),
                                    torqueGain(1.0),
                                    forceTimeout(timeout/2){}
    ~ReachAction(){}
    
    void appendToHistory( Point_3 p, Point_3 q, double r ){ history.push_back( HistoryItem(p,q,r) ); }

    int tempIdx;
    yarp::os::ConstString marker;
    
    double forceGain,torqueGain;
    double forceTimeout;
    
    Point_3 reachTarget;
    Point_3 markerPos;
    
    yarp::os::ConstString mobeeObjectName,eName,nName;
    
    struct HistoryItem {
        Point_3 target;
        Point_3 result;
        double reward;
        HistoryItem(Point_3 p, Point_3 q, double r) : target(p), result(q), reward(r) {}
    };
    std::vector< HistoryItem > history;
    
    //std::vector<Point_3> history;
    
    //void start() { Action::start(); }
    
    //double getErr();
    Vector_3 sendForceCommand(); // returns error
    bool threadInit();
    void threadRelease();
    void run();
    
};

#endif
/** @} */
