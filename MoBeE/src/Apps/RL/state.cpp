#include "learner.h"
#include "state.h"
#include "actionTransition.h"
#include "actionReach.h"

/*************
 *** STATE ***
 ************/

State::State(Point_d q) : Point_d(q), value(0.0), visits(0)
{
}

//bool State::disconnect( State* ){}
//bool State::connectToAll(){}
//bool State::disconnectFromAll(){}

double  State::exitBelief() {
    std::pair<const State*,double> bestExit(NULL,0.0);
    for ( std::vector<TransitionAction*>::iterator a = transitionActions.begin(); a != transitionActions.end(); ++a ) {
        std::pair<const State*,double> thisBelief = (*a)->getTransitionBelief();
        if ( thisBelief.first != this && thisBelief.second > bestExit.second )
            bestExit = thisBelief;
    }
    return bestExit.second;
}

Action* State::greedy()
{
    Action* greedy_action = NULL;
    if ( transitionActions.size() > 0 ) {
        std::vector<TransitionAction*>::iterator a = transitionActions.begin();
        for ( a = transitionActions.begin(); a != transitionActions.end(); ++a ) {
            if ( !greedy_action || (*a)->value() > greedy_action->value() )
                greedy_action = *a;
        }
    }
    if ( reachActions.size() > 0 ) {
        std::vector<ReachAction*>::iterator a = reachActions.begin();
        for ( a = reachActions.begin(); a != reachActions.end(); ++a ) {
            if ( !greedy_action || (*a)->value() > greedy_action->value() )
                greedy_action = *a;
        }
    }
    return greedy_action;
}

Action* State::explore()
{
    Action* reach = leastTriedReach();
    Action* transition = leastTriedTransition();
    if ( reach->timesTried() > transition->timesTried() ) return transition;
    else return reach;
}

Action* State::leastTriedReach()
{
    ReachAction* leastTriedReach = *reachActions.begin();
    for ( std::vector<ReachAction*>::iterator a = reachActions.begin(); a != reachActions.end(); ++a ) {
        if ( (*a)->timesTried() < leastTriedReach->timesTried() )
            leastTriedReach = *a;
    }
    
    printf("LEAST TRIED REACH: %p\n",leastTriedReach);
    
    return leastTriedReach;
}

TransitionAction* State::leastTriedTransition()
{
    //printf("getting least tried action (of %d) for state %p\n",transitionActions.size(),this);
    TransitionAction* leastTriedAction = NULL;//*transitionActions.begin();
    for ( std::vector<TransitionAction*>::iterator a = transitionActions.begin(); a != transitionActions.end(); ++a ) {
        //printf("  action: %p, timesTried: %d\n", *a, (*a)->timesTried() );
        if ( !leastTriedAction || (*a)->timesTried() < leastTriedAction->timesTried() )
            leastTriedAction = *a;
    }
    printf("LEAST TRIED TRANSITION FROM STATE %p IS TO STATE %p\n",this,leastTriedAction->destination_state);
    return leastTriedAction;
}

Action* State::randomTransition()
{
    if (!transitionActions.size())
        return NULL;
    
    // Random state transition
    printf("RANDOM STATE TRANSITION...\n");
    std::vector<TransitionAction*>::iterator a = transitionActions.begin();
    return transitionActions.at(rand()%transitionActions.size());
}

Action* State::randomReach()
{
    if (!reachActions.size())
        return NULL;
    
    // Random reach
    printf("RANDOM REACH...\n");
    std::vector<ReachAction*>::iterator a = reachActions.begin();
    return reachActions.at(rand()%reachActions.size());
}

double State::computeValue()
{
    double maxValue = 0;
    double maxDelta = 0;
    
    for ( std::vector<ReachAction*>::iterator r = reachActions.begin(); r != reachActions.end(); ++r )
    {
        double delta = (*r)->updateValue();
        
        if ( (*r)->value() > maxValue )
            maxValue = (*r)->value();
        
        if ( delta > maxDelta )
            maxDelta = delta;
        //sum += (*r)->value;
        
        //printf("  RAction: %p value: %f, reward: %f\n", *r, (*r)->value(), (*r)->reward());
    }
    
    for ( std::vector<TransitionAction*>::iterator j = transitionActions.begin(); j != transitionActions.end(); ++j )
    {
        double delta = (*j)->updateValue();
        
        if ( (*j)->value() > maxValue )
            maxValue = (*j)->value();
        if ( delta > maxDelta )
            maxDelta = delta;
        
        std::pair<const State*, double> s_prime = (*j)->getTransitionBelief();
        //printf("  SAction: %p d: %p, p: %f v: %f, r: %f\n", *j, s_prime.first, s_prime.second, (*j)->value(), (*j)->reward());
    }
    value = maxValue;
    return maxDelta;
}