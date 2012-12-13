#include "learner.h"
#include "state.h"

//State::State(){}
State::~State(){}

State::State(CGAL::Cartesian_d<double>::Point_d q, Learner* _learner) : CGAL::Cartesian_d<double>::Point_d( q.dimension(),
                                                                                                       q.cartesian_begin(),
                                                                                                       q.cartesian_end()),
                                                                        learner(_learner)
{
    // append the action 'stay here' to this state
    appendAction(this);
    
    // append the action 'go to this state' to all the other states in the learner's model
    for (std::vector<State>::iterator i=learner->states.begin(); i!=learner->states.end(); ++i )
        i->appendAction(this);
}

void State::appendAction( State* destination_state )
{
    actions.push_back(Action(destination_state,learner));
}

bool State::removeAction( State* destination_state )
{
}

/*
State::State( int num )
{
    for (int i=0; i<num; i++) {
        std::vector<double> row_action_i;
        for (int j=0; j<num; j++){
            if (j==i) row_action_i.push_back(1.0);
            else row_action_i.push_back(0.0);
        }
        transition_belief.push_back(row_action_i);
    }
}

void State::append()
{
    if ( transition_belief.size()<1 ) { // this state has no actions
        
    }
    
    for (std::vector< std::vector<double> >::iterator i=transition_belief.begin(); i!=transition_belief.end(); ++i) {
        i->push_back(0.0);
    }
    for (int j=0; j<num; j++){
        if (j==i) row_action_i.push_back(1.0);
        else row_action_i.push_back(0.0);
    }
    transition_belief.push_back(row_action_i);
}*/
