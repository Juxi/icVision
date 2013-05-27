//#include <unistd.h>
//#include <time.h>
//#include <cmath>
#include "learner.h"
#include "action.h"

/******************************
 *** ABSTRACT PARENT ACTION ***
 *****************************/

bool Action::threadInit() {
    if (parentState->getLearner()->checkMutex()) {
        timeStarted = yarp::os::Time::now();
        return 1;
    }
    return 0;
}

void Action::afterStart(bool s)
{
    if (s) {
        if (actionCounter) (*actionCounter)++;
    } else
        printf("EPIC ACTION FAIL...\n");
}

void Action::threadRelease()
{
    //printf("*** Releasing thread for State::Action - %p::%p ***\n\n",parentState,this);
    parentState->getLearner()->valueIteration();
    parentState->getLearner()->writeStateFile();
    parentState->getLearner()->writeHistoryFile( parentState->getLearner()->getUnvisitedStates(),
                                                parentState->getLearner()->getUntriedActions(),
                                                parentState->getIdx(),
                                                idx, r, v);
    parentState->getLearner()->postMutex();
}

void Action::computeNewValue()
{
    //printf("called Action::computeNewValue()\n");
    newv = r + parentState->getLearner()->getDiscountFactor() * v;
}

bool Action::waitForSteady()
{
    printf("Waiting for steady state.");
    double tShutdown = yarp::os::Time::now();
    yarp::os::Time::delay(1);
    while ( yarp::os::Time::now() - tShutdown < timeout)
    {
        printf(".");
        if (parentState->getLearner()->mobee.isStable()) {
            printf("\nSTEADY STATE REACHED! :-)\n");
            return true;
        }
        yarp::os::Time::delay(0.2);
    }
    printf("\nAction::WaitForSteady() TIMED OUT!!!! :-0\n");
    return false;
}
