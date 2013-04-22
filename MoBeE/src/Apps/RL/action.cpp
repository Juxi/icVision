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
    if (parentLearner->checkMutex()) {
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
    
    parentLearner->postMutex();
}

double Action::updateValue()
{
    double newVal = r + 0.9 * v;
    double delta = newVal - v;
    v = newVal;
    return delta;
}



bool Action::waitForSteady()
{
    printf("Waiting for steady state.");
    double tShutdown = yarp::os::Time::now();
    yarp::os::Time::delay(1);
    while ( yarp::os::Time::now() - tShutdown < timeout)
    {
        printf(".");
        if (parentLearner->isStable()) {
            printf("\n");
            return true;
        }
        yarp::os::Time::delay(0.2);
    }
    printf("\n");
    return false;
}

void Action::relax()
{
    printf("Waiting for robot to relax.");
    parentLearner->stopForcing();
    if ( waitForSteady() ) printf("now relaxing...\n");
    else printf("RELAX TIMED OUT!!!! :-0\n");
}
