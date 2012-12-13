/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RL
 *	@{
 */

#ifndef STATE_H_
#define STATE_H_

#include <CGAL/Cartesian_d.h>
//#include <CGAL/Orthogonal_k_neighbor_search.h>
//#include <CGAL/Search_traits.h>

#include "action.h"

class Learner;

class State : public CGAL::Cartesian_d<double>::Point_d
{
private:
    class Action
    {
    public:
        Action( State* destination_state, Learner* learner )
        {
            // assume this new action will not take us to the other states in the learner's model
            for (std::vector<State>::iterator i=learner->states.begin(); i!=learner->states.end(); ++i )
            {
                std::pair< const State*, double > pair(&(*i),0.0);
                transition_belief.push_back(pair);
            }
            // assume this new action will work, taking us to the destination state
            std::pair< const State*, double > pair(destination_state,1.0);
            transition_belief.push_back(pair);
        }
        ~Action(){}
        
    private:
        std::vector< std::pair< const State*, double > > transition_belief;
        //std::vector< std::vector<int> > transition_history;
    };
    std::vector<Action> actions;
    
public:
    State(CGAL::Cartesian_d<double>::Point_d q, Learner* learner) : CGAL::Cartesian_d<double>::Point_d( q.dimension(),
                                                                                                        q.cartesian_begin(),
                                                                                                        q.cartesian_end())
    {
        // append the action 'go to this state' to all the other states in the learner's model
        for (std::vector<State>::iterator i=learner->states.begin(); i!=learner->states.end(); ++i )
            i->actions.push_back(Action(this));
        // append the action 'stay here' to this state
        actions.push_back(Action(this));
    }
    
    void State::appendAction( State* destination_state )
    {
        actions.push_back(Action(destination_state,learner));
    }
    
    bool State::removeAction( State* destination_state )
    {
    }
    
    //void updateTransitionBelief();
    
private:
    
    
    //std::vector< std::vector< double > > transition_belief;
    //std::vector< std::vector< double > > transition_history;
    
    //void appendAction( State* target_state );
    //bool removeAction( State* target_state );
    
};
#endif
/** @} */
