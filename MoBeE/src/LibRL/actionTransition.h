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
    double p;
    double q;
    int n;
    S_Prime(State* s) : state(s), p(1.0), q(1.0), n(1){}
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
                        bool initialize = false,
                        int rate = 200 );
    ~TransitionAction(){}
    
    const State*                    getDestination() { return destination_state; }
    //std::pair<const State*,double>  getTransitionBelief();
    void                            computeNewValue();
    std::vector< S_Prime* > const&  getTransitionBeliefs() { return transition_belief; }
    bool                            expectedTransition( State* s );
    
    S_Prime*    get_sprime(State* s);
    void        update_transition_probabilities(bool verbose = true);
    
    
private:
    
    //int tempIdx;
    State*            destination_state;
    State*                  resultingState;
    std::vector< S_Prime* > transition_belief;
    
    void        afterStart(bool s);
    void        learnStuff();
    
    
    
    double      compute_kl();

    //double      updateTransitionBelief( bool verbose = false );
    double      observe(State*);
    
    void        run();
};

#endif
/** @} */
