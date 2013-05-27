#include "learner.h"
#include "state.h"

/*************
 *** STATE ***
 ************/

State::State( int _idx, Point_d q, Learner* learner) : Point_d(q),
                                                        idx(_idx),
                                                        parentLearner(learner),
                                                        v(0.0),
                                                        newv(0.0),
                                                        visits(0)
{
}

//bool State::disconnect( State* ){}
//bool State::connectToAll(){}
//bool State::disconnectFromAll(){}

/*double  State::exitBelief() {
    std::pair<const State*,double> bestExit(NULL,0.0);
    std::vector<TransitionAction*> shuffleTransitions = transitionActions;
    std::random_shuffle(shuffleTransitions.begin(), shuffleTransitions.end());
    for ( std::vector<TransitionAction*>::iterator a = shuffleTransitions.begin(); a != shuffleTransitions.end(); ++a ) {
        std::pair<const State*,double> thisBelief = (*a)->getTransitionBelief();
        if ( thisBelief.first != this && thisBelief.second > bestExit.second )
            bestExit = thisBelief;
    }
    return bestExit.second;
}*/

TransitionAction* State::greedyTransition()
{
    TransitionAction* greedy_action = NULL;
    if ( transitionActions.size() > 0 ) {
        std::vector<TransitionAction*> shuffleTransitions = transitionActions;
        std::random_shuffle(shuffleTransitions.begin(), shuffleTransitions.end());
        std::vector<TransitionAction*>::iterator a = shuffleTransitions.begin();
        for ( a = shuffleTransitions.begin(); a != shuffleTransitions.end(); ++a ) {
            if ( !greedy_action || (*a)->getValue() > greedy_action->getValue() )
                greedy_action = *a;
        }
    }
    return greedy_action;
}

ReachAction* State::greedyReach()
{
    ReachAction* greedy_action = NULL;
    if ( transitionActions.size() > 0 ) {
        std::vector<ReachAction*> shuffleReaches = reachActions;
        std::random_shuffle(shuffleReaches.begin(), shuffleReaches.end());
        std::vector<ReachAction*>::iterator a = shuffleReaches.begin();
        for ( a = shuffleReaches.begin(); a != shuffleReaches.end(); ++a ) {
            if ( !greedy_action || (*a)->getValue() > greedy_action->getValue() )
                greedy_action = *a;
        }
    }
    return greedy_action;
}

Action* State::greedyAction()
{
    Action* greedy_action = NULL;
    TransitionAction* trans = greedyTransition();
    ReachAction* reach = greedyReach();
    if ( reach && trans ) {
        //printf("reach val: %f, trans val: %f\n",reach->getValue(),trans->getValue());
        if ( trans->getValue() > reach->getValue() )
            return trans;
        else
            return reach;
    } if ( reach && !trans )
        return reach;
    else if ( trans && !reach )
        return trans;
    
    printf("No reach or transition found... we got problems!\n");
    return NULL;
}

/*Action* State::explore()
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
}*/

TransitionAction* State::leastTriedTransition()
{
    //printf("getting least tried action (of %d) for state %p\n",transitionActions.size(),this);
    std::vector<TransitionAction*> shuffleTransitions = transitionActions;
    std::random_shuffle(shuffleTransitions.begin(), shuffleTransitions.end());
    
    TransitionAction* leastTriedAction = NULL;//*transitionActions.begin();
    for ( std::vector<TransitionAction*>::iterator a = shuffleTransitions.begin(); a != shuffleTransitions.end(); ++a ) {
        //printf("  action: %p, timesTried: %d\n", *a, (*a)->timesTried() );
        if ( !leastTriedAction || (*a)->getTimesTried() < leastTriedAction->getTimesTried() )
            leastTriedAction = *a;
    }
    //printf("LEAST TRIED TRANSITION FROM STATE %p IS TO DESTINATION STATE %p\n",this,leastTriedAction->getDestination());
    return leastTriedAction;
}

ReachAction* State::leastTriedReach()
{
    //printf("getting least tried reach (of %d) for state %p\n",transitionActions.size(),this);
    std::vector<ReachAction*> shuffleReaches = reachActions;
    std::random_shuffle(shuffleReaches.begin(), shuffleReaches.end());
    
    ReachAction* leastTriedAction = NULL;//*transitionActions.begin();
    for ( std::vector<ReachAction*>::iterator a = shuffleReaches.begin(); a != shuffleReaches.end(); ++a ) {
        //printf("  action: %p, timesTried: %d\n", *a, (*a)->timesTried() );
        if ( !leastTriedAction || (*a)->getTimesTried() < leastTriedAction->getTimesTried() )
            leastTriedAction = *a;
    }
    //printf("LEAST TRIED REACH FROM STATE %p IS TO DESTINATION STATE %p\n",this,leastTriedAction->getDestination());
    return leastTriedAction;
}

TransitionAction* State::randomTransition()
{
    if (!transitionActions.size())
        return NULL;
    
    // Random state transition
    printf("RANDOM STATE TRANSITION...\n");
    std::vector<TransitionAction*>::iterator a = transitionActions.begin();
    return transitionActions.at(rand()%transitionActions.size());
}

ReachAction* State::randomReach()
{
    if (!reachActions.size())
        return NULL;
    
    // Random reach
    printf("RANDOM REACH...\n");
    std::vector<ReachAction*>::iterator a = reachActions.begin();
    return reachActions.at(rand()%reachActions.size());
}

void State::computeNewValue()
{
    //printf("\tComputing new value for state: %p\n",this);
    newv = 0.0;
    for ( std::vector<TransitionAction*>::iterator j = transitionActions.begin(); j != transitionActions.end(); ++j )
    {
        (*j)->computeNewValue();
        if ( (*j)->getNewValue() > newv )
            newv = (*j)->getNewValue();
    }
    for ( std::vector<ReachAction*>::iterator r = reachActions.begin(); r != reachActions.end(); ++r )
    {
        (*r)->computeNewValue();
        if ( (*r)->getNewValue() > newv )
            newv = (*r)->getNewValue();
        
        /*double delta = (*r)->updateValue();
         if ( (*r)->value() > maxValue )
         maxValue = (*r)->value();
         
         if ( delta > maxDelta )
         maxDelta = delta;
         sum += (*r)->value;
         printf("  RAction: %p value: %f, reward: %f\n", *r, (*r)->value(), (*r)->reward());*/
    }
    //printf("\tstate_newv = %f\n",newv);
}

void State::updateValue()
{
    //printf("\tUpdating value for state: %p... v: %f, newv: %f\n",this,v,newv);
    for ( std::vector<TransitionAction*>::iterator j = transitionActions.begin(); j != transitionActions.end(); ++j )
        (*j)->updateValue();
    for ( std::vector<ReachAction*>::iterator j = reachActions.begin(); j != reachActions.end(); ++j )
        (*j)->updateValue();
    v = newv;
}