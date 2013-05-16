/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RL
 *	@{
 */

#ifndef ACTION_H_
#define ACTION_H_

#include <list>
#include <yarp/os/all.h>
#include <CGAL/Cartesian.h>
#include <CGAL/Cartesian_d.h>
#include <CGAL/Point_3.h>
#include <CGAL/Vector_3.h>
#include <CGAL/point_generators_d.h>
#include "types.h"

class Learner;
class State;

/* This abstract class provides a generic action that runs in a thread and is protected by a semaphor
 such that only one action can be running (per learner) at a time */
class Action : public yarp::os::RateThread
{
    friend class Learner;
public:
    
    inline int      getTimesTried() { return num; }
    inline double   getReward() { return r; }
    inline double   getValue() { return v; }
    inline double   getNewValue() { return newv; }
    
    virtual void    computeNewValue();
    inline void     updateValue() { /*printf("\t\tUpdating value for action: %p... v: %f, newv: %f\n",this,v,newv);*/ v = newv; }
    
    virtual void    start() { yarp::os::RateThread::start(); }
    virtual void    start( Point_3 p ) { yarp::os::RateThread::start(); }
    
protected:
    
    Action( //Learner* l,
            State* p,
            double value = 0.0,
            double reward = 0.0,
            int numTries = 0,
            int rate = 200 ) :  yarp::os::RateThread(rate),
                                //parentLearner(l),
                                parentState(p),
                                timeStarted(0.0),
                                timeout(20.0),
                                num(numTries),
                                v(value),
                                newv(0.0),
                                r(reward) {}
    virtual ~Action(){}

    State*          parentState;
    double          timeStarted,
                    timeout;
    int             num;    // number of times this action has been tried
    double          v,newv; // value of this action under the current policy
    double          r; 

    bool            waitForSteady();
    
    virtual bool    threadInit();
    virtual void    afterStart(bool s);
    virtual void    threadRelease();
    virtual void    onStop();
};

#endif
/** @} */
