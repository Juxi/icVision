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
                                                idx, r, v);
    parentState->getLearner()->postMutex();
}

double Action::predictReward( Point_3 p )
{
    double r_predicted = 0.0;
    if ( history.size()>0 ) {
        for ( std::vector< HistoryItem >::iterator i = history.begin(); i != history.end(); ++i ) {
            //double d = (i->target-i->result).squared_length();
            //r_predicted += 1.0/((p-i->result).squared_length()+1) - d/((p-i->target).squared_length()+d);
        }
        r_predicted /= history.size();
    }
    //r = r_predicted;
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
