/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RL
 *	@{
 */

#ifndef REACHACTION_H_
#define REACHACTION_H_

#include "action.h"

class ReachAction : public Action
{
/* These actions reach for objects, and are responsible for rewarding the learner. In this way the learner learns which 
   states (roadmap nodes) are the good ones from which to reach for objects in the environment */
    friend class Learner;
    friend class State;
    
private:
    
    ReachAction(    int _idx,
                    yarp::os::ConstString m,
                    State* p,
                    double value = 0.0,
                    double reward = 0.0,
                    int numTries = 0,
                    int rate=50 ) : Action( _idx,p,value,reward,numTries,rate),
                                    marker(m),
                                    //reachTarget(0,0,0),
                                    markerPos(0,0,0),
                                    forceGain(1.0),
                                    torqueGain(1.0){}
    ~ReachAction(){}

    yarp::os::ConstString   marker;             // reference to the part of the robot we are controlling
    yarp::os::ConstString   mobeeObjectName,    // visualize target point
                            eName,              // visualize error vector
                            nName;              // visualize marker's normal vector
    double                  forceGain,
                            torqueGain;
    Point_3                 markerPos;
    
    

    Vector_3 sendForceCommand(); // returns error
    
    void afterStart(bool s);
    void relax();
    void run();
    
};

#endif
/** @} */
