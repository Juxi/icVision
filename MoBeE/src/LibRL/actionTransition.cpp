#include "actionTransition.h"
#include "learner.h"

TransitionAction::TransitionAction( int idx,
                                    State* a,
                                    State* b,
                                    double value,
                                    double reward,
                                    int numTries,
                                    bool initialize,
                                    int rate) : Action(idx,a,value,reward,numTries,rate), destination_state(b)
{
    // optimistic initialization
    //S_Prime* new_s_prime = get_sprime(b);
    
    if (initialize) {
        observe(a);
        observe(b);
    }
    
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
        sum += (*k)->p * (*k)->state->getValue();
        //printf("\t\t\t sum += %f * %f\n",(*k)->p,(*k)->state->getValue());
    }
        //newv += (*k)->p * ( r + parentState->getLearner()->getDiscountFactor() * (*k)->state->getValue() );
    newv = r + parentState->getLearner()->getDiscountFactor() * sum;
    //printf("\t\t\t newv = %f + %f * %f\n",r,parentState->getLearner()->getDiscountFactor(),sum);
    //printf("\t\t\t newv = %f\n",newv);
}

S_Prime* TransitionAction::get_sprime( State* s )
{
    printf("\tlooking for sprime...");
    // check if we have seen this sprime before
    std::vector<S_Prime*>::iterator i;
    for ( i=transition_belief.begin(); i!=transition_belief.end(); ++i) {
        if ( (*i)->state == s ) {
            printf("\tfound it\n");
            return *i;
        }
    }
    
    // if not, append it
    S_Prime* new_s_prime = new S_Prime(s);
    transition_belief.push_back( new_s_prime );
    
    printf("\tappended it\n");
    update_transition_probabilities(true);
    
    return new_s_prime;
}

void TransitionAction::update_transition_probabilities( bool verbose )
{
    int num_samples = 0;
    for ( std::vector<S_Prime*>::iterator i=transition_belief.begin(); i!=transition_belief.end(); ++i) {
        num_samples += (*i)->n;
    }
    for ( std::vector<S_Prime*>::iterator i=transition_belief.begin(); i!=transition_belief.end(); ++i) {
        (*i)->q = (*i)->p;
        (*i)->p = (double)(*i)->n/num_samples;
        if (verbose)
            printf("\t\t state: %d p=%f q=%f n=%d\n",(*i)->state->getIdx(),(*i)->p,(*i)->q,(*i)->n);
    }
}

double TransitionAction::compute_kl()
{                   
    double kl_div = 0.0;
    for ( std::vector<S_Prime*>::iterator i=transition_belief.begin(); i!=transition_belief.end(); ++i) {
        if ( (*i)->p!=0 && (*i)->q!=0 )
            kl_div += log((*i)->p/(*i)->q)*(*i)->p;
    }
    
    //printf("\tKL Divergence = %f\n",kl_div);
    
    return kl_div;
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

double TransitionAction::observe(State* s) {
    printf("\tObserved state transition to state %d\n",s->getIdx());
    S_Prime* sprime = get_sprime(s);
    sprime->n++;
    update_transition_probabilities();
    r = compute_kl();
    printf("\tGot reward: %f\n",r);
    return r;
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
    //if (!parentState->getLearner()->isLearningModel() && !expectedTransition(resultingState)) {
    //    r = -1.0;
    //    appendToHistory(target, r);
    //    printf("\tState Transition Action Failed.  Got r = -1\n");
    //}
    
    if (parentState->getLearner()->isLearningModel()) {
        observe(resultingState);
    }
}