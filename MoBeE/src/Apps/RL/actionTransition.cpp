#include "actionTransition.h"
#include "learner.h"

std::pair<const State*,double> TransitionAction::getTransitionBelief()
{
    if (!transition_belief.size())
        return std::pair<const State*,double>(NULL,0.0);
    
    std::pair<const State*,double> max;
    max.first = (*transition_belief.begin())->s_prime;
    max.second = (*transition_belief.begin())->prob;
    for ( std::vector<S_Prime*>::iterator i = transition_belief.begin(); i != transition_belief.end(); ++i ) {
        if ( (*i)->prob > max.second ) {
            max.first = (*i)->s_prime;
            max.second = (*i)->prob;
        }
    }
    return max;
}

bool TransitionAction::threadInit()
{
    Action::threadInit();
    parentLearner->setAttractor(*destination_state);
    yarp::os::Time::delay(1);
    return 1;
}

void TransitionAction::threadRelease()
{
    if ( parentLearner->getDiscreteState() != parentState )
        printf("STATE TRANSITION OCCURRED\n");

    parentLearner->setAttractor(*parentLearner->getDiscreteState());
    yarp::os::Time::delay(1);
    updateTransitionBelief(parentLearner->getDiscreteState());
    Action::threadRelease();
}

/*bool TransitionAction::appendTransitionBelief( State* state )
{
    if (!state)
        return false;
    
    transition_belief.push_back(TransitionAction::S_Prime(state,0.0,0));
}*/

double TransitionAction::updateValue()
{
    double  sum = 0.0;
    for ( std::vector< S_Prime* >::iterator k = transition_belief.begin(); k != transition_belief.end(); ++k )
        sum += (*k)->prob * ( r + 0.9 * (*k)->s_prime->getValue() );
    double delta = sum - v;
    v = sum;
    return delta;
}


S_Prime* TransitionAction::appendSPrime( State* s_prime, int num, double prob )
{
    S_Prime* new_s_prime = new S_Prime(s_prime, prob, num);
    transition_belief.push_back( new_s_prime );
    return new_s_prime;
}

double TransitionAction::updateTransitionBelief( State* s )
{
    printf("\tUPDATING TRANSITION BELIEFS for the action: %p -> %p\n",parentState,this);
    
    // look if the action has ended in this state before
    std::vector<S_Prime*>::iterator i;
    bool found_s_prime = false;
    for ( i=transition_belief.begin(); i!=transition_belief.end(); ++i) {
        if ( (*i)->s_prime == s ) {
            found_s_prime = true;
            break;
        }
    }
    
    // if not, add this state to the list of possible outcomes
    if (!found_s_prime) {
        appendSPrime(s);
        //transition_belief.push_back( S_Prime(s,0.0,0.0) );
        i = --transition_belief.end();
    }
    
    num++;              //printf("num: %d\n",num);
    (*i)->num++;           //printf("inum: %d\n",i->num);
    
    // recompute the probabilities
    double delta = 0.0;
    for ( i=transition_belief.begin(); i!=transition_belief.end(); ++i) {
        double new_belief = (double)(*i)->num/(double)num;
        delta += fabs(new_belief - (*i)->prob);
        (*i)->prob = new_belief;
        printf("\ts_prime: %p happened %d times. transition prob is: %f\n",(*i)->s_prime,(*i)->num,(*i)->prob);
    }
    
    return delta;
}

void TransitionAction::run()
{
    printf("current state: %p\n",parentLearner->getDiscreteState());
    
    if (parentLearner->isStable())
    {
        printf("\n STEADY STATE REACHED  :-)\n");
        //if (learner->getDiscreteState() == *learner->states.begin()) {
        //    printf("Got REWARD = 1 for being in STATE 0");
        //    reward=1;
        //}
        stop();
    }
    else if ( yarp::os::Time::now() - timeStarted > timeout ) {
        printf("REACH TIMED OUT :-(\n");
        stop();
    }
 
}