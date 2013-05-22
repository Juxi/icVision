//#include <unistd.h>
//#include <time.h>
//#include <cmath>
#include "learner.h"
#include "action.h"

/******************************
 *** ABSTRACT PARENT ACTION ***
 *****************************/

bool Action::threadInit() {
    //printf("\n*** Initializing thread for State::Action - %p::%p ***\n",parentState,this);
    if (parentState->getLearner()->checkMutex()) {
        /*printf("\tmutex.check() succeeded... ");*/
        //printf("\n*** Initializing thread for State::Action - %p::%p ***\n",parentState,this);
        timeStarted = yarp::os::Time::now();
        return 1;
    }
    else { /*printf("\tmutex.check() failed... ");*/ return 0; }
}

void Action::afterStart(bool s)
{
    if (!s)
        printf("EPIC ACTION FAIL...\n");
    //else
        //reward = 0.0;
}

void Action::threadRelease() {
    //printf("*** Releasing thread for State::Action - %p::%p ***\n\n",parentState,this);
    parentState->getLearner()->stopForcing();
    parentState->getLearner()->writeHistoryFile( parentState->getLearner()->statesUnvisited(),
                                                parentState->getLearner()->actionsUntried(),
                                                parentState->getIdx(),
                                                idx,
                                                r,
                                                v);
    waitForSteady();
    parentState->getLearner()->postMutex();
}

void Action::computeNewValue()
{
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
        if (parentState->getLearner()->isStable()) {
            printf("\nSTEADY STATE REACHED! :-)\n");
            return true;
        }
        yarp::os::Time::delay(0.2);
    }
    printf("\nAction::WaitForSteady() TIMED OUT!!!! :-0\n");
    return false;
}
