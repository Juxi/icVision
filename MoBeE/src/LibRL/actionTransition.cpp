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
    S_Prime* new_s_prime = appendSPrime(b);
    new_s_prime->prob = 1.0;
    new_s_prime->num = 1;
    
    //new_s_prime->num = parentState->getLearner()->getStateTransitionInit();
    //num=new_s_prime->num;
}

bool TransitionAction::expectedTransition( State* s )
{
    for ( std::vector<S_Prime*>::iterator i = transition_belief.begin(); i != transition_belief.end(); ++i )
        if ( (*i)->state == s ) return true;
    return false;
}

void TransitionAction::afterStart(bool s)
{
    Action::afterStart(s);
    
    if (s) {
        printf(" RUNNING STATE TRANSITION ACTION %d --> %d (%s)",   parentState->getIdx(),
                                                                    destination_state->getIdx(),
                                                                    parentState->getLearner()->getName().c_str());
        resultingState = NULL;
        parentState->getLearner()->mobee.setAttractor(*destination_state);
        yarp::os::Time::delay(0.2);
    }
}

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


S_Prime* TransitionAction::getSPrime(State* s)
{
    S_Prime* sp = NULL;
    std::vector<S_Prime*>::iterator i;
    for ( i=transition_belief.begin(); i!=transition_belief.end(); ++i) {
        if ( (*i)->state == s )
            sp = *i;
    }
    return sp;
}

S_Prime* TransitionAction::appendSPrime( State* s )
{
    // prevent duplicates
    std::vector<S_Prime*>::iterator i;
    for ( i=transition_belief.begin(); i!=transition_belief.end(); ++i) {
        if ( (*i)->state == s )
            break;
    }
    if ( i!=transition_belief.end() )
        return *i;
    
    //printf("APPENDING S_PRIME!!!\n");
    S_Prime* new_s_prime = new S_Prime(s, 0, 0);
    transition_belief.push_back( new_s_prime );
    
    updateTransitionBelief();
    
    return new_s_prime;
}

double TransitionAction::updateTransitionBelief( bool verbose )
{
    // recompute the probabilities
    int num_sPrime = 0;
    for ( std::vector<S_Prime*>::iterator i=transition_belief.begin(); i!=transition_belief.end(); ++i) {
        num_sPrime += (*i)->num;
    }
                        
    double kail_div = 0.0;
    for ( std::vector<S_Prime*>::iterator i=transition_belief.begin(); i!=transition_belief.end(); ++i) {
        double new_prob = (double)(*i)->num/num_sPrime;
        kail_div += fabs(new_prob-(*i)->prob);
        
        if (verbose) printf("\ts_prime: %d happened %d times. old_prob: %f, new_prob: %f\n",(*i)->state->getIdx(),(*i)->num,(*i)->prob,new_prob);
    
        (*i)->prob = new_prob;
    }
    kail_div /= transition_belief.size();
    
    if (verbose) printf("\tKAIL Divergence = %f\n",kail_div);
    
    return kail_div;
}

void TransitionAction::run()
{
    //printf("\tcurrent state: %p\n",parentLearner->getDiscreteState());
    //printf(".");
    if (parentState->getLearner()->mobee.isStable())
    {
        printf("\nTRANSITION REACHED STEADY STATE (%s) :-)\n",parentState->getLearner()->getName().c_str());
        learnStuff();
        askToStop();
    }
    else if ( yarp::os::Time::now() - timeStarted > timeout ) {
        printf("\nTRANSITION TIMED OUT (%s) :-(\n",parentState->getLearner()->getName().c_str());
        learnStuff();
        askToStop();
    }
}

void TransitionAction::learnStuff()
{
    num++;
    resultingState = parentState->getLearner()->getDiscreteState();
    
    if (resultingState) {
        resultingState->visits++;
    } else {
        printf("There is no resulting state!!! WTF?!?\n");
        return;
    }
    
    // TODO expectedTransition(State*) NEEDS WORK IN LIGHT OF THE NEW KL DIVERGENCE
    if (!parentState->getLearner()->isLearningModel() && !expectedTransition(resultingState)) {
        r = -1.0;
        appendToHistory(target, r);
        printf("\tState Transition Action Failed.  Got r = -1\n");
    }
    
    else if (parentState->getLearner()->isLearningModel()) {
        S_Prime* s_prime = getSPrime(resultingState);
        if (!s_prime)
            s_prime = appendSPrime(resultingState);
        
        s_prime->num++;
        double kail_divergence = updateTransitionBelief(true);
        r = 1.0/(num + resultingState->getVisits()) + parentState->getLearner()->modelInterestingness() * kail_divergence;
        printf("\tr = 1/(sa_count + s'_count) + a*KAIL_Divergence) = 1/(%d + %d) + %f*%f = %f\n",num,
               resultingState->getVisits(),
               parentState->getLearner()->modelInterestingness(),
               kail_divergence,
               r);
    }
}