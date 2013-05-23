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
    //friend class Learner;
    
public:
    
    TransitionAction(   int idx,
                        State* a,
                        State* b,
                        double value = 0.0,
                        double reward = 0.0,
                        int numTries = 0,
                        int rate = 200 );
    ~TransitionAction(){}
    
    const State*                    getDestination() { return destination_state; }
    //std::pair<const State*,double>  getTransitionBelief();
    void                            computeNewValue();
    S_Prime*                        findOrAppendSPrime(State* s);
    std::vector< S_Prime* > const&  getTransitionBeliefs() { return transition_belief; }
    
private:
    
    //int tempIdx;
    const State*            destination_state;
    State*                  resultingState;
    std::vector< S_Prime* > transition_belief;
    
    void        afterStart(bool s);
    void        doModelLearning();
    double      updateTransitionBelief();
    void        run();
};

#endif
/** @} */
