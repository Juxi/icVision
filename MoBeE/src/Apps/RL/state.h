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
    //friend class TransitionAction;
    
public:
    
    double  getValue() const { return value; }
    
    Action* greedyAction();
    Action* exploreTransition();
    Action* reach();
    //Action* explore();
    
private:
     
    State( Point_d q );
    ~State(){}
    
    //bool appendTransitionAction( State* );
    //bool appendTransitionBelief( State* );
    //bool appendReachAction(Learner* learner,yarp::os::ConstString markerName );
    
    //bool disconnect( State* );
    //bool connectToAll();
    //bool disconnectFromAll();
    
    
    double  computeValue();
    

    //Learner* parentLearner;
    int tempIdx;
    double value;
    std::vector<TransitionAction*> transitionActions; // Instead of keeping multiple lists here i could dynamic_cast...  dunno
    std::vector<ReachAction*> reachActions;
};

#endif
/** @} */
