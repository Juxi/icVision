#include "learner.h"
#include "state.h"
#include "actionTransition.h"
#include "actionReach.h"

/*************
 *** STATE ***
 ************/

State::State(Point_d q) : Point_d(q), value(0.0)
{
}

//bool State::disconnect( State* ){}
//bool State::connectToAll(){}
//bool State::disconnectFromAll(){}


Action* State::greedyAction()
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

Action* State::exploreTransition()
{
    // Try the least tried state transition
    std::vector<TransitionAction*>::iterator a = transitionActions.begin();
    TransitionAction* leastTriedAction = *transitionActions.begin();
    for ( a = transitionActions.begin(); a != transitionActions.end(); ++a ) //a++;
    {
        std::pair<const State*,double> belief = (*a)->getTransitionBelief();
        printf("  action: %p, %d tries, destination: %p it leads to state: %p with prob. %f\n",*a,(*a)->isTried(),(*a)->getDestination(),belief.first,belief.second);
        if ( (*a)->isTried() < leastTriedAction->isTried() )
            leastTriedAction = *a;
    }
    
    printf("LEAST TRIED STATE TRANSITION: %p\n",leastTriedAction);
    //leastTriedAction->start();
    
    return leastTriedAction;
}

Action* State::reach()
{
    printf("REACH\n");
    if ( reachActions.size() > 0 ) {
        return *reachActions.begin();
        //(*a)->runReach( p );
    }
    return NULL;
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