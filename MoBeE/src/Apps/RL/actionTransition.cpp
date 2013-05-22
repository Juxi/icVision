#include "actionTransition.h"
#include "learner.h"

TransitionAction::TransitionAction( int idx,
                                    State* a,
                                    State* b,
                                    double value,
                                    double reward,
                                    int numTries,
                                    int rate) : Action(idx,a,value,reward,numTries,rate), destination_state(b)
{
    // optimistic initialization
    //S_Prime* new_s_prime = new S_Prime(b, 1.0, 1);
    //transition_belief.push_back( new_s_prime );
    S_Prime* new_s_prime = findOrAppendSPrime(b);
    new_s_prime->prob = 1.0;
    new_s_prime->num = parentState->getLearner()->getStateTransitionInit();
    num=new_s_prime->num;
}

std::pair<const State*,double> TransitionAction::getTransitionBelief()
{
    if (!transition_belief.size())
        return std::pair<const State*,double>(NULL,0.0);
    
    std::pair<const State*,double> max;
    max.first = (*transition_belief.begin())->state;
    max.second = (*transition_belief.begin())->prob;
    for ( std::vector<S_Prime*>::iterator i = transition_belief.begin(); i != transition_belief.end(); ++i ) {
        if ( (*i)->prob > max.second ) {
            max.first = (*i)->state;
            max.second = (*i)->prob;
        }
    }
    return max;
}

bool TransitionAction::threadInit()
{
    Action::threadInit();
    printf("RUNNING STATE TRANSITION ACTION.");
    resultingState = NULL;
    parentState->getLearner()->setAttractor(*destination_state);
    yarp::os::Time::delay(1);
    return 1;
}

void TransitionAction::threadRelease()
{
    if (!resultingState) {
        printf("There is no resulting state!!! WTF?!?\n");
        return;
    }
    
    S_Prime* s_prime = findOrAppendSPrime(resultingState);
    
    num++;
    s_prime->num++;
    s_prime->state->visits++;
    
    if (parentState->getLearner()->isLearningModel())
    {
        double kl = updateTransitionBelief();
        r = 1.0/(num + resultingState->getVisits()) + parentState->getLearner()->modelInterestingness() * kl;
        printf("\tr = %f ... 1/(%d + %d) + %f * %f\n",r,num,s_prime->state->visits,parentState->getLearner()->modelInterestingness(),kl);
    }
    
    Action::threadRelease();
}

/*bool TransitionAction::appendTransitionBelief( State* state )
{
    if (!state)
        return false;
    
    transition_belief.push_back(TransitionAction::S_Prime(state,0.0,0));
}*/

void TransitionAction::computeNewValue()
{
    //printf("\t\tComputing new value for action: %p\n",this);
    newv = 0.0;
    double sum = 0.0;
    for ( std::vector< S_Prime* >::iterator k = transition_belief.begin(); k != transition_belief.end(); ++k ) {
        sum += (*k)->prob * (*k)->state->getValue();
        //printf("\t\t\t sum += %f * %f\n",(*k)->prob,(*k)->state->getValue());
    }
        //newv += (*k)->prob * ( r + parentState->getLearner()->getDiscountFactor() * (*k)->state->getValue() );
    newv = r + parentState->getLearner()->getDiscountFactor() * sum;
    //printf("\t\t\t newv = %f + %f * %f\n",r,parentState->getLearner()->getDiscountFactor(),sum);
    //printf("\t\t\t newv = %f\n",newv);
}


S_Prime* TransitionAction::findOrAppendSPrime( State* s )
{
    // look if the action has ended in this state before
    std::vector<S_Prime*>::iterator i;
    //bool found_s_prime = false;
    
    //printf("\t\tlooking for s_prime: %p\n",s);
    for ( i=transition_belief.begin(); i!=transition_belief.end(); ++i)
    {
        //printf("\t\t%p =? %p\n", (*i)->s_prime, s);
        if ( (*i)->state == s ) {
            //printf("\t\tYES\n");
            //found_s_prime = true;
            break;
        }
    }
    
    if ( i!=transition_belief.end() )
        return *i;
    
    //printf("APPENDING S_PRIME!!!\n");
    double probInit = 0.000000001;
    S_Prime* new_s_prime = new S_Prime(s, probInit, 0);
    for ( i=transition_belief.begin(); i!=transition_belief.end(); ++i) {
        (*i)->prob -= probInit/transition_belief.size();
    }
    transition_belief.push_back( new_s_prime );
    return new_s_prime;
}

double TransitionAction::updateTransitionBelief()
{
    printf("\tUpdating transition beliefs for the state: %p, action: %p, dest: %p, num: %d\n",parentState,this,destination_state,num);
    
    // recompute the probabilities
    double div = 0.0;
    for ( std::vector<S_Prime*>::iterator i=transition_belief.begin(); i!=transition_belief.end(); ++i) {
        double p = (double)(*i)->num/(double)num;
        double q = (*i)->prob;
        //delta += fabs(new_belief - (*i)->prob);
        
        printf("\ts_prime: %p happened %d times. old_prob: %f, new_prob: %f\n",(*i)->state,(*i)->num,q,p);
        printf("\t\tdiv += log(%f/%f)*%f",p,q,p);
        if ( p!=0 && q!=0 ) {
            double foo = log(p/q)*p;
            printf(" = %f\n",foo);
            div += foo;
        } else printf("skipped it\n");
        
        
        (*i)->prob = p;
        
    }
    
    if (div!=0) printf("\tGOT AN INTRINSIC REWARD\n");
    printf("\tKL Divergence = %f\n",div);
    
    return div;
}

void TransitionAction::run()
{
    //printf("\tcurrent state: %p\n",parentLearner->getDiscreteState());
    printf(".");
    if (parentState->getLearner()->isStable())
    {
        printf("\nSTEADY STATE REACHED :-)\n");
        resultingState = parentState->getLearner()->getDiscreteState();
        askToStop();
    }
    else if ( yarp::os::Time::now() - timeStarted > timeout ) {
        printf("\nTRANSITION TIMED OUT :-(\n");
        resultingState = parentState->getLearner()->getDiscreteState();
        askToStop();
    }
 
}