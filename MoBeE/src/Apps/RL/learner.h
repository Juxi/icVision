/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RL
 *	@{
 */

#ifndef LEARNER_H_
#define LEARNER_H_

#include <CGAL/Cartesian_d.h>
typedef CGAL::Cartesian_d<double>::Point_d Point_d;

class Learner
{
    
private:
    
    class State : public Point_d
    {
    public:
        class Action
        {  
        public:
            Action( State* destination_state, Learner* parent ) {
                for (std::vector<State*>::iterator i=parent->states.begin(); i!=parent->states.end(); ++i ) {
                    if (*i==destination_state) transition_belief.push_back(std::pair< const State*, double >(*i,1.0));
                    else transition_belief.push_back(std::pair< const State*, double >(*i,0.0));
                }
            }
            ~Action(){}
            std::vector< std::pair< const State*, double > > transition_belief;
            //std::vector< std::vector<int> > transition_history;
        };
        
        State(Point_d q, Learner* parent) : Point_d( q.dimension(), q.cartesian_begin(), q.cartesian_end()) {
            parent->states.push_back(this);
            for (std::vector<State*>::iterator i=parent->states.begin(); i!=parent->states.end(); ++i ) {
                // add state transition probabilities for all state::actions to lead to this state
                for (std::vector<State::Action>::iterator j=(*i)->actions.begin(); j!=(*i)->actions.end(); ++j )
                    if (*i!=this) j->transition_belief.push_back(std::pair<const State*, double>(this,0.0));
                // append actions 'go to other state' (from here)
                if (*i!=this) actions.push_back(Action(*i,parent));
                // append actions 'go to this state' (from elsewhere)
                (*i)->actions.push_back(Action(this,parent));
            }
        }
        ~State(){}
        std::vector<Action> actions;
    };
    std::vector<State*> states;

public:
    
	Learner(){}
    ~Learner(){}
    
    inline void appendFullyConnectedState(Point_d& q) { new State(q,this); }
    inline int getNumStates() { return states.size(); }
    
    void print();
    
    //inline std::vector< State >& getStates() { return states; }
};
#endif
/** @} */
