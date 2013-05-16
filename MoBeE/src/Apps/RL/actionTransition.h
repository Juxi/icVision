/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RL
 *	@{
 */

#ifndef TRANSITIONACTION_H_
#define TRANSITIONACTION_H_

#include "action.h"

struct S_Prime
{
    State* state;
    double prob;
    int num;
    S_Prime(State* s, double p, int n) : state(s), prob(p), num(n){}
};

/* These actions move the attractor in the MoBeE model. This corresponds to position control, and is used to implement deliberate
 motion planning through a roadmap graph (the nodes of which are "States") using reinforcement learning */
class TransitionAction : public Action
{
    friend class Learner;
    //friend class State;
    
public:
    
    //int                             timesTried() { return num; }
    const State*                    getDestination() { return destination_state; }
    std::pair<const State*,double>  getTransitionBelief();
    
    
    void                          computeNewValue();  
    
private:
    
    int tempIdx;
    const State*            destination_state;
    std::vector< S_Prime* > transition_belief;
    
    TransitionAction(   //Learner* l,
                        State* a,
                        State* b,
                        double value = 0.0,
                        double reward = 0.0,
                        int numTries = 0,
                        int rate = 200 );
    ~TransitionAction(){}
    
    S_Prime*                        findOrAppendSPrime(State* s);
    
    
    bool                            threadInit();
    void                            threadRelease();
    double                          updateTransitionBelief();
    void                            run();
};

#endif
/** @} */
