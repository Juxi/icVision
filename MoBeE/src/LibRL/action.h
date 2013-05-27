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
#include "util.h"

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
    inline void     updateValue() { v = newv; }
    
    virtual void    computeNewValue();
    
    
    virtual void    start( Point_3 p, int* n = NULL );
    virtual void    start( int* n = NULL ) { start(CGAL::ORIGIN,n); }
    
    double          predictReward( Point_3 p );
    
protected:
    
    Action( int _idx,
            State* p,
            double value = 0.0,
            double reward = 0.0,
            int numTries = 0,
            int rate = 200 ) :  yarp::os::RateThread(rate),
                                idx(_idx),
                                parentState(p),
                                timeStarted(0.0),
                                timeout(20.0),
                                num(numTries),
                                v(value),
                                newv(0.0),
                                r(reward) {}
    virtual ~Action(){}

    int             idx;
    State*          parentState;
    double          timeStarted,
                    timeout;
    int             num;    // number of times this action has been tried
    double          v,newv; // value of this action under the current policy
    double          r;
    
    Point_3         target;
    int*            actionCounter;

    bool            waitForSteady();
    
    virtual bool    threadInit();
    virtual void    afterStart(bool s);
    virtual void    threadRelease();
    
    struct HistoryItem {
        Point_3 target;
        //Point_3 result;
        double reward;
        HistoryItem(Point_3 p, double r) : target(p), reward(r) {}
    };
    std::vector< HistoryItem > history;
    
    void appendToHistory( Point_3 p, double r ){ history.push_back( HistoryItem(p,r) ); }
};

#endif
/** @} */
