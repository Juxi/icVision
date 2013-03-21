#include <unistd.h>
#include <time.h>
//#include <cmath>
#include "learner.h"

Learner::Learner( const char* _robotName, const char* _partName, bool connect ) //: currentState(NULL), currentAction(NULL)
{
    if (connect)
    {
        printf("Connecting to MoBeE model...\n");
        
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
        worldClient.open(localWorldPort);
        
        // to query the state of markers on the robot model... maybe this should be a stream but it's not clear
        yarp::os::ConstString remoteMarkerPort("/MoBeE/");
        remoteMarkerPort += _partName;
        remoteMarkerPort += "/rpc";
        yarp::os::ConstString localMarkerPort("/learner/");
        localMarkerPort += _partName;
        localMarkerPort += "/rpc";
        controllerClient.open(localMarkerPort);

        statePort.open(localStatePort);
        commandPort.open(localCommandPort);
        yarp::os::Network::connect(remoteStatePort,localStatePort);
        yarp::os::Network::connect(localCommandPort,remoteCommandPort);
        yarp::os::Network::connect(localWorldPort,remoteWorldPort);
        yarp::os::Network::connect(localMarkerPort,remoteMarkerPort);
        
        //currentState = getDiscreteState();
    } else { printf("Not connected to MoBeE model."); }
    
    // prepare the random number generator
    srand(time(0));
}
Learner::~Learner()
{
    mutex.wait();
    statePort.interrupt();
    commandPort.interrupt();
    controllerClient.interrupt();
    worldClient.interrupt();
}

Point_d Learner::getRealState() {
    yarp::os::Bottle* b = statePort.read();
    std::list<double> bList;
    for (int i=0; i<b->size(); i++)
        bList.push_back(b->get(i).asDouble());
    return Point_d(bList.size(),bList.begin(),bList.end());
}

Learner::State* Learner::getDiscreteState() {
    Point_d q = getRealState();
    State* nearestState = NULL;
    if ( states.size() > 0 ) {
        nearestState = *(states).begin();
        for (std::list<State*>::iterator j=states.begin(); j!=states.end(); ++j) {
            //std::cout << "    j: " << **j << "\t\t" << (q-**j).squared_length() << std::endl;
            if ( (q-**j).squared_length() < (q-*nearestState).squared_length() ) {
                nearestState = *j;
            }
        }
    }
    return nearestState;
}

bool Learner::deleteState( const State* s )
{
    for (std::list<State*>::iterator i=states.begin(); i!=states.end(); ++i) {
        for (std::list<State::TransitionAction*>::iterator j=(*i)->transitionActions.begin(); j!=(*i)->transitionActions.end(); ++j) {
            for (std::list< State::TransitionAction::S_Prime >::iterator k=(*j)->transition_belief.begin(); k!=(*j)->transition_belief.end(); ++k) {
                if (k->s_prime==s) {
                    std::list< State::TransitionAction::S_Prime >::iterator K=k;
                    (*j)->transition_belief.erase(k,++K);
                }
            }
            if ((*j)->destination_state == s) {
                std::list<State::TransitionAction*>::iterator J=j,deadTransition=j;
                (*i)->transitionActions.erase(j,++J);
                delete *deadTransition;
            }
        }
        if (*i==s) {
            std::list<State*>::iterator I=i,deadState=i;
            states.erase(i,++I);
            delete *deadState;
        }
    }
    delete s;
}

void Learner::print(bool printAll)
{
    //printf("\n\n");
    int stateCount=0;
    for (std::list<State*>::iterator i=states.begin(); i!=states.end(); ++i)
    {
        printf("State %d, id: %p, q: ",stateCount++,*i);
        std::cout << **i << std::endl;
        
        int actionCount=0;
        for (std::list<State::TransitionAction*>::iterator j=(*i)->transitionActions.begin(); j!=(*i)->transitionActions.end(); ++j)
        {
            printf("  Action %d, destination state: %p\n   Transition belief: ",actionCount++,(*j)->destination_state);
            for (std::list< State::TransitionAction::S_Prime >::iterator k=(*j)->transition_belief.begin(); k!=(*j)->transition_belief.end(); ++k)
            {
                if (printAll) printf("(%p, %f, %d) ", k->s_prime, k->prob, k->num);
                else printf("%f (%d)   ", k->prob, k->num);
            }
            printf("\n");
            //printf("\n Starting Action.....\n");
            //(*j)->start();
            //(*j)->stop();
        }
    }
    printf("\n");
}

/*********************
 *** PARENT ACTION ***
 ********************/

bool Learner::State::Action::threadInit() {
    //printf("\n*** Initializing thread for State::Action - %p::%p ***\n",parentState,this);
    if (learner->mutex.check()) {
        printf("\n*** Initializing thread for State::Action - %p::%p ***\n",parentState,this);
        /*printf("\tmutex.check() succeeded... ");*/ return 1; }
    else { /*printf("\tmutex.check() failed... ");*/ return 0; }
}
void Learner::State::Action::afterStart(bool s) { if (s) printf("Running Action..."); printf("\n"); }
void Learner::State::Action::threadRelease() {
    printf("*** Releasing thread for State::Action - %p::%p ***\n\n",parentState,this);
    learner->mutex.post();
}

/******************************
*** STATE TRANSITION ACTION ***
******************************/

void Learner::State::TransitionAction::threadRelease() {
    updateTransitionBelief(current_discrete_state);
    Action::threadRelease();
}

double Learner::State::TransitionAction::updateTransitionBelief( const State* s )
{
    printf("\tUPDATING TRANSITION BELIEFS for the action: %p -> %p\n",parentState,this);
    double delta = 0.0;
    num++;
    //printf("num: %d\n",num);
    for ( std::list<State::TransitionAction::S_Prime>::iterator i=transition_belief.begin(); i!=transition_belief.end(); ++i){
        if ( i->s_prime == s ) i->num++;
        //printf("inum: %d\n",i->num);
        double new_belief = (double)i->num/(double)num;
        delta += fabs(new_belief - i->prob);
        i->prob = new_belief;
        printf("\ts_prime: %p happened %d times. transition prob is: %f\n",i->s_prime,i->num,i->prob);
    }
    return delta;
}

void Learner::State::TransitionAction::run()
{
    current_real_state = learner->getRealState();
    current_discrete_state = learner->getDiscreteState();
    printf("\tcurrent_discrete_state = %p\n",current_discrete_state);
    
    // activate the attractor at currentState
    yarp::os::Bottle vec;
    yarp::os::Bottle& attractorCommand = learner->commandPort.prepare();
    attractorCommand.clear();
    attractorCommand.addVocab(yarp::os::Vocab::encode("qatt"));
    for (Point_d::Cartesian_const_iterator i=current_discrete_state->cartesian_begin(); i!=current_discrete_state->cartesian_end(); ++i)
        vec.addDouble(*i);
    attractorCommand.addList() = vec;
    learner->commandPort.writeStrict();
    
    // force the system toward the desired next state
    yarp::os::Bottle& forceCommand = learner->commandPort.prepare();
    forceCommand.clear();
    vec.clear();
    forceCommand.addVocab(yarp::os::Vocab::encode("qfor"));
    f += 5.0 * (*destination_state-current_real_state);                 //TODO: Look into controlling jerk in partController
    //f = 1000.0 * (*destination_state-current_real_state);
    for (Vector_d::Cartesian_const_iterator i=f.cartesian_begin(); i!=f.cartesian_end(); ++i)
        vec.addDouble(*i);
    forceCommand.addList() = vec;
    //printf("  forceCommand: %s\n",forceCommand.toString().c_str());
    learner->commandPort.writeStrict();
    
    // check if the robot is still moving
    yarp::os::Bottle query,reply;
    query.addVocab(yarp::os::Vocab::encode("stpd"));
    query.addDouble(1.0); // acceleration threshold
    query.addDouble(1.0); // velocity threshold
    learner->controllerClient.write(query,reply);
    //printf("query: %s\n",query.toString().c_str());
    //printf("reply: %s\n",reply.toString().c_str());
    
    // stop if the robot is no longer moving
    if (reply.get(0).asInt()==1) {
        printf("\n\n\n\n\n*** ROBOT STOPPED ***\n\n\n\n\n\n");
        stop();
    }
    
    // stop if the robot changes state
    else if ( current_discrete_state != parentState ) {
        printf("STATE TRANSITION OCCURRED\n");
        stop();
        yarp::os::Time::delay(0.5);
    }
}

/********************
 *** REACH ACTION ***
 *******************/

void Learner::State::ReachAction::run()
{/*
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
        worldClient.write(cmd,rsp);
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
            worldClient.write(get,state);
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
        worldClient.write(cmd,rsp);
        printf("\ncommand: %s\n", cmd.toString().c_str());
        printf("  response: %s\n\n", rsp.toString().c_str());
        
    }
*/}