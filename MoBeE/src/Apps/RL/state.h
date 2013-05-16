/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RL
 *	@{
 */

#ifndef STATE_H_
#define STATE_H_

#include "action.h"

class TransitionAction;
class ReachAction;

class State : public Point_d
{
    friend class Learner;
    friend class TransitionAction;
    
public:
    
    int     getVisits() { return visits; }
    double  getValue() const { return v; }
    double  getNewValue() { return newv; }
    
    void    computeNewValue();
    void    updateValue();
    
    //double  exitBelief();
    
    
    //Action* leastTriedReach();
    //Action* explore();
    
    TransitionAction*   greedyTransition();
    TransitionAction*   randomTransition();
    TransitionAction*   leastTriedTransition();
    ReachAction*        randomReach();
    //Action* explore();
    
    Learner* getLearner() { return parentLearner; }
    
private:
     
    State( Point_d q, Learner* l );
    ~State(){}
    
    //bool appendTransitionAction( State* );
    //bool appendTransitionBelief( State* );
    //bool appendReachAction(Learner* learner,yarp::os::ConstString markerName );
    
    //bool disconnect( State* );
    //bool connectToAll();
    //bool disconnectFromAll();
    

    Learner* parentLearner;
    int tempIdx;
    double v,newv;
    int visits;
    std::vector<TransitionAction*> transitionActions; // Instead of keeping multiple lists here i could dynamic_cast...  dunno
    std::vector<ReachAction*> reachActions;
};

#endif
/** @} */
