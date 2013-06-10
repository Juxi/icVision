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
    S_Prime* new_s_prime = findOrAppendSPrime(b);
    new_s_prime->prob = 1.0;
    new_s_prime->num = parentState->getLearner()->getStateTransitionInit();
    num=new_s_prime->num;
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
        printf(" RUNNING STATE TRANSITION ACTION (%d --> %d)",parentState->getIdx(),destination_state->getIdx());
        resultingState = NULL;
        parentState->getLearner()->mobee.setAttractor(*destination_state);
        yarp::os::Time::delay(0.2);
    }
}

void TransitionAction::learnStuff()
{
    resultingState = parentState->getLearner()->getDiscreteState();
    
    if (!resultingState) {
        printf("There is no resulting state!!! WTF?!?\n");
        return;
    }
    
    else if (!parentState->getLearner()->isLearningModel() && !expectedTransition(resultingState)) {
        r = -1.0;
        appendToHistory(target, r);
        printf("\tState Transition Action Failed.  Got r = -1\n");
    }
    
    else if (parentState->getLearner()->isLearningModel()) {
        S_Prime* s_prime = findOrAppendSPrime(resultingState);
        num++;
        s_prime->num++;
        s_prime->state->visits++;
        double kl = updateTransitionBelief();
        r = 1.0/(num + resultingState->getVisits()) + parentState->getLearner()->modelInterestingness() * kl;
        printf("\tGot kl = %f, r = %f\n",kl,r);
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


S_Prime* TransitionAction::findOrAppendSPrime( State* s )
{
    // look if the action has ended in this state before
    std::vector<S_Prime*>::iterator i;
    for ( i=transition_belief.begin(); i!=transition_belief.end(); ++i) {
        if ( (*i)->state == s )
            break;
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