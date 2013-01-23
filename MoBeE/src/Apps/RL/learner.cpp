#include "learner.h"

Learner::Learner( const char* _robotName, const char* _partName, int rate ) : yarp::os::RateThread(rate)
{
    yarp::os::ConstString remoteStatePort("/");
	remoteStatePort += _robotName;
	remoteStatePort += "/";
	remoteStatePort += _partName;
	remoteStatePort += "/state:o";
    yarp::os::ConstString localStatePort("/learner/");
    localStatePort += _partName;
    localStatePort += "/state:i";
    
    yarp::os::ConstString remoteCommandPort("/MoBeE/");
	remoteCommandPort += _partName;
	remoteCommandPort += "/cmd:i";
    yarp::os::ConstString localCommandPort("/learner/");
    localCommandPort += _partName;
    localCommandPort += "/cmd:o";

    statePort.open(localStatePort);
    commandPort.open(localCommandPort);
    yarp::os::Network::connect(remoteStatePort,localStatePort);
    yarp::os::Network::connect(localCommandPort,remoteCommandPort);
}

bool Learner::threadInit()
{
	printf("Starting Reinforcement Learner\n");
	return true;
}

//called by start after threadInit, s is true iff the thread started successfully
void Learner::afterStart(bool s)
{
	if (!s)
		printf("Reinforcement Learner did not start\n");
}

void Learner::run()
{
    // get the current robot state and put it in a Point_d
    yarp::os::Bottle b;
    statePort.read(b);
    std::list<double> bList;
    for (int i=0; i<b.size(); i++)
        bList.push_back(b.get(i).asDouble());
    Point_d q(bList.size(),bList.begin(),bList.end());
    
    State* nearestState = NULL;
    if ( states.size() > 0 ) {
        nearestState = *states.begin();
        for (std::list<State*>::iterator j=states.begin(); j!=states.end(); ++j) {
            if ( (q-**j).squared_length() < (q-*nearestState).squared_length() ) {
                nearestState = *j;
            }
        }
    }
    currentState = nearestState;
    
    // activate the current nearest state
    if ( currentState != NULL ) {
        yarp::os::Bottle cmd,vec;
        cmd.addVocab(yarp::os::Vocab::encode("qatt"));
        for (Point_d::Cartesian_const_iterator i=nearestState->cartesian_begin(); i!=nearestState->cartesian_end(); ++i)
            vec.addDouble(*i);
        cmd.addList() = vec;
        printf("sending: %s\n",cmd.toString().c_str());
        commandPort.write(cmd);
    }
}

void Learner::threadRelease()
{
	printf("\n*** Goodbye from Reinforcement Learner ***\n\n");
}

bool Learner::deleteState( const State* s )
{
    for (std::list<State*>::iterator i=states.begin(); i!=states.end(); ++i) {
        for (std::list<State::Action>::iterator j=(*i)->actions.begin(); j!=(*i)->actions.end(); ++j) {
            for (std::list< std::pair<const State*,double> >::iterator k=j->transition_belief.begin(); k!=j->transition_belief.end(); ++k) {
                if (k->first==s) {
                    std::list< std::pair<const State*,double> >::iterator K=k;
                    j->transition_belief.erase(k,++K);
                }
            }
            if (j->destination_state == s) {
                std::list<State::Action>::iterator J=j;
                (*i)->actions.erase(j,++J);
            }
        }
        if (*i==s) {
            std::list<State*>::iterator I=i;
            states.erase(i,++I);
        }
    }
    delete s;
}

void Learner::print()
{
    int stateCount=0;
    for (std::list<State*>::iterator i=states.begin(); i!=states.end(); ++i)
    {
        printf("State %d: %p\n",stateCount++,*i);
        int actionCount=0;
        for (std::list<State::Action>::iterator j=(*i)->actions.begin(); j!=(*i)->actions.end(); ++j)
        {
            printf("  Action %d: ",actionCount++);
            for (std::list< std::pair<const State*,double> >::iterator k=j->transition_belief.begin(); k!=j->transition_belief.end(); ++k)
            {
                //printf("(%p %f) ", k->first, k->second);
                printf("%f ", k->second);
            }
            printf("\n");
        }
    }
}