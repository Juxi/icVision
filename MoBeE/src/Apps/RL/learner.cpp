#include <unistd.h>
#include <time.h>
//#include <cmath>
#include "learner.h"

Learner::Learner( const char* _robotName, const char* _partName, int rate ) : yarp::os::RateThread(rate), currentState(NULL), currentAction(NULL)
{
    // streaming motor encoder positions
    yarp::os::ConstString remoteStatePort("/MoBeE/");
	remoteStatePort += _partName;
	remoteStatePort += "/state:o";
    yarp::os::ConstString localStatePort("/learner/");
    localStatePort += _partName;
    localStatePort += "/state:i";
    
    // streaming commands to the attractor dynamics
    yarp::os::ConstString remoteCommandPort("/MoBeE/");
	remoteCommandPort += _partName;
	remoteCommandPort += "/cmd:i";
    yarp::os::ConstString localCommandPort("/learner/");
    localCommandPort += _partName;
    localCommandPort += "/cmd:o";
    
    // to put objects into the world model to mark targets and such for debugging
    yarp::os::ConstString remoteWorldPort("/MoBeE/world");
    yarp::os::ConstString localWorldPort("/learner/");
    localWorldPort += _partName;
    localWorldPort += "/world";
    worldPortClient.open(localWorldPort);
    
    // to query the state of markers on the robot model... maybe this should be a stream but it's not clear
    yarp::os::ConstString remoteMarkerPort("/MoBeE/");
    remoteMarkerPort += _partName;
    remoteMarkerPort += "/rpc";
    yarp::os::ConstString localMarkerPort("/learner/");
    localMarkerPort += _partName;
    localMarkerPort += "/rpc";
    markerPortClient.open(localMarkerPort);

    statePort.open(localStatePort);
    commandPort.open(localCommandPort);
    yarp::os::Network::connect(remoteStatePort,localStatePort);
    yarp::os::Network::connect(localCommandPort,remoteCommandPort);
    yarp::os::Network::connect(localWorldPort,remoteWorldPort);
    yarp::os::Network::connect(localMarkerPort,remoteMarkerPort);
    
    // prepare the random number generator
    srand(time(0));
}

bool Learner::randomReach(yarp::os::ConstString markerName)
{
    // estimate limits of the reachable workspace
    double x_min = -0.5,
    x_max = 0.0,
    y_min = -0.5,
    y_max = 0.5,
    z_min = 0.0,
    z_max = 0.5;
    
    // prepare some bottles to poll the MoBeE model
    yarp::os::Bottle get,state;
    get.addVocab(yarp::os::Vocab::encode("get"));
    get.addString(markerName);
    
    // position vector and normal of the hand marker
    double pMarker[3],nMarker[3],err[6];

    // gains for the control signal
    double  forceMagnitude = 10000.0,
    torqueMagnitude = 10000.0;
    
    // reach to each target point
    for (int i=0; i<100; i++)
    {
        // pick a random target to reach to
        double p[3];
        p[0] = x_min + (double)rand()/RAND_MAX * (x_max-x_min);
        p[1] = y_min + (double)rand()/RAND_MAX * (y_max-y_min);
        p[2] = z_min + (double)rand()/RAND_MAX * (z_max-z_min);
        
        // visualize it in the world model
        yarp::os::Bottle cmd, rsp;
        cmd.addVocab(yarp::os::Vocab::encode("mk"));
        cmd.addVocab(yarp::os::Vocab::encode("sph"));
        cmd.addDouble(0.01);
        cmd.addDouble(p[0]);
        cmd.addDouble(p[1]);
        cmd.addDouble(p[2]);
        worldPortClient.write(cmd,rsp);
        printf("\ncommand: %s\n", cmd.toString().c_str());
        printf("  response: %s\n\n", rsp.toString().c_str());
        
        // get the name of the newly inserted object
        yarp::os::ConstString objectName = rsp.get(0).asString();
        
        // clean up the bottles for reuse
        cmd.clear();
        rsp.clear();
        
        // reach to the point p for an arbitrary amount of time
        int count = 0;
        while (count < 1000)
        {
            // get the state of the hand
            worldPortClient.write(get,state);
            pMarker[0] = state.get(0).asDouble();
            pMarker[1] = state.get(1).asDouble();
            pMarker[2] = state.get(2).asDouble();
            pMarker[0] = state.get(3).asDouble();
            pMarker[1] = state.get(4).asDouble();
            pMarker[2] = state.get(5).asDouble();
            
            // error vector from hand to target
            err[0] = p[0] - pMarker[0];
            err[1] = p[1] - pMarker[1];
            err[2] = p[2] - pMarker[2];
            
            // and normalize it
            double mErr = sqrt(pow(err[0],2) + pow(err[1],2) + pow(err[2],2));
            err[0] /= mErr;
            err[1] /= mErr;
            err[2] /= mErr;
            
            // angular error between hand normal and error vector (radians)
            //double theta = acos( err[0]*nHand[0] + err[1]*nHand[1] + err[2]*nHand[2] );
            
            // angular error direction ... (nHand)x(err)
            double t[3];
            t[0] = nMarker[1]*err[2] - nMarker[2]*err[1];
            t[1] = nMarker[2]*err[0] - nMarker[0]*err[2];
            t[2] = nMarker[0]*err[1] - nMarker[1]*err[0];
            
            // and normalize it
            double mt = sqrt(pow(t[0],2) + pow(t[1],2) + pow(t[2],2));
            t[0] /= mt;
            t[1] /= mt;
            t[2] /= mt;
            
            // build up the command bottle
            //yarp::os::Bottle& opSpaceForceAndTorque = right_arm_cmd.prepare();
            yarp::os::Bottle opSpaceForceAndTorque;
            opSpaceForceAndTorque.clear();
            opSpaceForceAndTorque.addVocab(yarp::os::Vocab::encode("opsp"));
            opSpaceForceAndTorque.addString(markerName);
            
            yarp::os::Bottle cmd;
            cmd.addDouble(forceMagnitude*err[0]);
            cmd.addDouble(forceMagnitude*err[1]);
            cmd.addDouble(forceMagnitude*err[2]);
            
            // either with no torque
            cmd.addDouble(0.0);
            cmd.addDouble(0.0);
            cmd.addDouble(0.0);
            
            // or try some torque
            //cmd.addDouble(torqueMagnitude*t[0]);
            //cmd.addDouble(torqueMagnitude*t[1]);
            //cmd.addDouble(torqueMagnitude*t[2]);
            
            opSpaceForceAndTorque.addList() = cmd;
            
            printf("Sending control command: %s\n", opSpaceForceAndTorque.toString().c_str());
            //right_arm_cmd.write();
            
            usleep(20000);
            count++;
        }
        
        // remove the sphere from the model
        cmd.addVocab(yarp::os::Vocab::encode("rm"));
        cmd.addString(objectName);
        worldPortClient.write(cmd,rsp);
        printf("\ncommand: %s\n", cmd.toString().c_str());
        printf("  response: %s\n\n", rsp.toString().c_str());
        
    }
	
	return 1;
}

bool Learner::takeRandomAction()
{
    if (!currentState) {
        printf("takeRandomAction FAILED!!\n");
        return false;
    }
    
    // select a random action
    std::list<State::Action>::iterator a;
    int actionIdx = rand() % (currentState->actions.size());
    a = currentState->actions.begin();
    for (int j=0; j<actionIdx; j++)
        a++;
    currentAction = &*a;
    printf("\nTaking random action from: %p to: %p\n", currentState, currentAction->destination_state);
    
    // while action is not complete
    State* startingState = currentState;
    int num = 0;
    
    while ( num < 20 && currentState == startingState ) {
        printf("... %d ", num);
        num++;
        usleep(1000000);
    }
    
    printf("\nACTION ");
    
    if ( currentState == startingState ) {
        printf("TIMED OUT...\n");
    } else if ( currentState == currentAction->destination_state ) {
        printf("SUCCEEDED...\n");
    } else {
        printf("ENDED UNEXPECTEDLY...\n");
    }
    
    double deltaBelief = updateTransitionBelief( currentAction, currentState );
    printf("Changed transtition belief: %f\n",deltaBelief);
    
    return true;
}

double Learner::updateTransitionBelief( State::Action* a, State* s )
{
    printf("UPDATING TRANSITION BELIEFS for the action: %p -> %p\n");
    double delta = 0.0;
    a->num++;
    
    for ( std::list<State::Action::S_Prime>::iterator i=a->transition_belief.begin(); i!=a->transition_belief.end(); ++i){
        if ( i->s_prime == s ) i->num++;
        double new_belief = (double)i->num/a->num;
        delta += fabs(new_belief - i->prob);
        i->prob = new_belief;
        printf("  s_prime: %p happened %d times. transition prob is: %f\n",i->s_prime,i->num,i->prob);
    }

    return delta;
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
    //printf("\nRUN\n\n");
    
    // get the current robot state and put it in a Point_d
    yarp::os::Bottle* b = statePort.read();
    std::list<double> bList;
    for (int i=0; i<b->size(); i++)
        bList.push_back(b->get(i).asDouble());
    
    // The current real-valued state from the robot's continuous state space
    Point_d q(bList.size(),bList.begin(),bList.end());
    
    //std::cout << "  q: " << q << std::endl;
    
    // set 'currentState' based on the nearest attractor to the actual robot state q
    State* nearestState = NULL;
    if ( states.size() > 0 ) {
        nearestState = *states.begin();
        for (std::list<State*>::iterator j=states.begin(); j!=states.end(); ++j) {
            //std::cout << "    j: " << **j << "\t\t" << (q-**j).squared_length() << std::endl;
            if ( (q-**j).squared_length() < (q-*nearestState).squared_length() ) {
                nearestState = *j;
            }
        }
    }
    currentState = nearestState;
    
    //std::cout << "  Q: " << *currentState << "\t\t" << (q-*currentState).squared_length() << std::endl;
    
    // a vector for both the following commands
    yarp::os::Bottle vec;
    
    // activate the attractor at currentState
    yarp::os::Bottle& attractorCommand = commandPort.prepare();
    attractorCommand.clear();
    attractorCommand.addVocab(yarp::os::Vocab::encode("qatt"));
    for (Point_d::Cartesian_const_iterator i=nearestState->cartesian_begin(); i!=nearestState->cartesian_end(); ++i)
        vec.addDouble(*i);
    attractorCommand.addList() = vec;
    commandPort.writeStrict();
    //printf("  attractorCommand: %s\n",attractorCommand.toString().c_str());
    
    // force the system toward the desired next state
    yarp::os::Bottle& forceCommand = commandPort.prepare();
    forceCommand.clear();
    vec.clear();
    forceCommand.addVocab(yarp::os::Vocab::encode("qfor"));
    if ( currentAction != NULL /*&& currentState != currentAction->destination_state*/) {
        Vector_d f = 1000.0 * (*(currentAction->destination_state)-q);
        for (Vector_d::Cartesian_const_iterator i=f.cartesian_begin(); i!=f.cartesian_end(); ++i)
            vec.addDouble(*i);
    } else {
        for (Vector_d::Cartesian_const_iterator i=q.cartesian_begin(); i!=q.cartesian_end(); ++i)
            vec.addDouble(0.0);
    }
    forceCommand.addList() = vec;
    //printf("  forceCommand: %s\n",forceCommand.toString().c_str());
    commandPort.writeStrict();
}

void Learner::threadRelease()
{
	printf("\n*** Goodbye from Reinforcement Learner ***\n\n");
}

bool Learner::deleteState( const State* s )
{
    for (std::list<State*>::iterator i=states.begin(); i!=states.end(); ++i) {
        for (std::list<State::Action>::iterator j=(*i)->actions.begin(); j!=(*i)->actions.end(); ++j) {
            for (std::list< State::Action::S_Prime >::iterator k=j->transition_belief.begin(); k!=j->transition_belief.end(); ++k) {
                if (k->s_prime==s) {
                    std::list< State::Action::S_Prime >::iterator K=k;
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

void Learner::print(bool printAll)
{
    int stateCount=0;
    for (std::list<State*>::iterator i=states.begin(); i!=states.end(); ++i)
    {
        printf("State %d: %p\n",stateCount++,*i);
        int actionCount=0;
        for (std::list<State::Action>::iterator j=(*i)->actions.begin(); j!=(*i)->actions.end(); ++j)
        {
            printf("  Action %d, %p: ",actionCount++,j->destination_state);
            for (std::list< State::Action::S_Prime >::iterator k=j->transition_belief.begin(); k!=j->transition_belief.end(); ++k)
            {
                if (printAll) printf("(%p, %f, %d) ", k->s_prime, k->prob, k->num);
                else printf("%f (%d)   ", k->prob, k->num);
            }
            printf("\n");
        }
    }
}