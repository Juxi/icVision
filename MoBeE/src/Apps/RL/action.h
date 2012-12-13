/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RL
 *	@{
 */

#ifndef ACTION_H_
#define ACTION_H_

#include <vector>

class Learner;
class State;

class Action
{

public:
    
    Action( State* destination_state, Learner* learner )
    {
        // assume this new action will work, taking us to the destination state
        std::pair< const State*, double > pair(destination_state,1.0);
        transition_belief.push_back(pair);
        
        // construct state transition probabilities for this action w.r.t. each state in the learner's model
        for (std::vector<State>::iterator i=learner->states.begin(); i!=learner->states.end(); ++i )
        {
            std::pair< const State*, double > pair(&(*i),0.0);
            transition_belief.push_back(pair);
        }
        
    }
    ~Action(){}
    
private:

    std::vector< std::pair< const State*, double > > transition_belief;
    //std::vector< std::vector<int> > transition_history;

};
#endif
/** @} */
