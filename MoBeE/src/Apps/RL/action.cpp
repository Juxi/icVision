#include "learner.h"
//#include "state.h"
#include "action.h"

Action::Action( State* _destination_state, Learner* _learner ) : learner(_learner), destination_state(_destination_state)
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
Action::~Action(){}

