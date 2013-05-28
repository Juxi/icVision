//#include <unistd.h>
//#include <time.h>
//#include <cmath>
#include "learner.h"
#include "action.h"

/******************************
 *** ABSTRACT PARENT ACTION ***
 *****************************/
void Action::start( Point_3 p, int* n )
{
    actionCounter = n;
    target = p;
    yarp::os::RateThread::start();
}

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
                                                idx, r);
    parentState->getLearner()->postMutex();
}

double Action::predictReward( Point_3 p, bool b )
{
    double r_predicted = 0.0;
    for ( std::vector< HistoryItem >::iterator i = history.begin(); i != history.end(); ++i )
        r_predicted += i->reward/((p-i->target).squared_length()+1);
    if (b) r = r_predicted;
    return r_predicted;
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
