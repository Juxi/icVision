#include "actionTransition.h"
#include "learner.h"

TransitionAction::TransitionAction( Learner* l,
                                    State* a,
                                    State* b,
                                    double value,
                                    double reward,
                                    int numTries,
                                    int rate) : Action(l,a,value,reward,numTries,rate), destination_state(b)
{
    // optimistic initialization
    S_Prime* new_s_prime = new S_Prime(b, 1.0, 1);
    transition_belief.push_back( new_s_prime );
    num++;
}

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
    //if ( parentLearner->getDiscreteState() != parentState )
    //    printf("STATE TRANSITION OCCURRED\n");
    //parentLearner->setAttractor(*parentLearner->getDiscreteState());
    //relax();
    
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


S_Prime* TransitionAction::getSPrime( State* s )
{
    // look if the action has ended in this state before
    std::vector<S_Prime*>::iterator i;
    //bool found_s_prime = false;
    
    //printf("\t\tlooking for s_prime: %p\n",s);
    for ( i=transition_belief.begin(); i!=transition_belief.end(); ++i)
    {
        //printf("\t\t%p =? %p\n", (*i)->s_prime, s);
        if ( (*i)->s_prime == s ) {
            //printf("\t\tYES\n");
            //found_s_prime = true;
            break;
        }
    }
    
    if ( i!=transition_belief.end() )
        return *i;
    
    S_Prime* new_s_prime = new S_Prime(s, 0.0, 0);
    transition_belief.push_back( new_s_prime );
    return new_s_prime;
}

double TransitionAction::updateTransitionBelief( State* s )
{
    printf("\tUpdating transition beliefs for the state: %p, action: %p, dest: %p\n",parentState,this,destination_state);
    
    S_Prime* s_prime = getSPrime(s);
    
    num++;              //printf("num: %d\n",num);
    (s_prime)->num++;           //printf("inum: %d\n",i->num);
    
    // recompute the probabilities
    double delta = 0.0;
    for ( std::vector<S_Prime*>::iterator i=transition_belief.begin(); i!=transition_belief.end(); ++i) {
        double new_belief = (double)(*i)->num/(double)num;
        delta += fabs(new_belief - (*i)->prob);
        (*i)->prob = new_belief;
        printf("\ts_prime: %p happened %d times. transition prob is: %f\n",(*i)->s_prime,(*i)->num,(*i)->prob);
    }
    
    return delta;
}

void TransitionAction::run()
{
    printf("\tcurrent state: %p\n",parentLearner->getDiscreteState());
    
    if (parentLearner->isStable())
    {
        printf("STEADY STATE REACHED :-)\n");
        stop();
    }
    else if ( yarp::os::Time::now() - timeStarted > timeout ) {
        printf("TRANSITION TIMED OUT :-(\n");
        stop();
    }
 
}