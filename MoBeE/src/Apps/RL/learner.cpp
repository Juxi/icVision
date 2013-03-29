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
        
        // poll the MoBeE model for Markers
        yarp::os::Bottle get,list;
        get.addVocab(yarp::os::Vocab::encode("list"));
        controllerClient.write(get,list);
        if ( list.size() == 0 ) printf("WARNING: No marker for body part! There will be no reach action!!!\n");
        else for (int i=0; i<list.size(); i++) {
            markers.push_back( list.get(i).asString() );
        }
        printf("Found %d markers.\n",markers.size());
        
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
    //int stateCount=0;
    for (std::list<State*>::iterator i=states.begin(); i!=states.end(); ++i)
    {
        printf("\nState %p",*i);
        //std::cout << **i << std::endl;
        
        printf("\t\t\tsPrime:");
       
           for (std::list< State::TransitionAction::S_Prime >::iterator k=(*(*i)->transitionActions.begin())->transition_belief.begin(); k!=(*(*i)->transitionActions.begin())->transition_belief.end(); ++k)
            {
                printf("\t\t%p", k->s_prime);
            }
            printf("\n");
        
        int actionCount=0;
        for (std::list<State::TransitionAction*>::iterator j=(*i)->transitionActions.begin(); j!=(*i)->transitionActions.end(); ++j)
        {
            printf("  Action %p, dest: %p",*j,(*j)->destination_state);
            for (std::list< State::TransitionAction::S_Prime >::iterator k=(*j)->transition_belief.begin(); k!=(*j)->transition_belief.end(); ++k)
            {
                printf("\t(%f, %d)", k->prob, k->num);
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
        /*printf("\tmutex.check() succeeded... ");*/
        //printf("\n*** Initializing thread for State::Action - %p::%p ***\n",parentState,this);
        timeStarted = yarp::os::Time::now();
        return 1;
    }
    else { /*printf("\tmutex.check() failed... ");*/ return 0; }
}
void Learner::State::Action::afterStart(bool s) { if (!s) printf("EPIC AVTION FAIL...\n"); }
void Learner::State::Action::threadRelease() {
    //printf("*** Releasing thread for State::Action - %p::%p ***\n\n",parentState,this);
    learner->mutex.post();
}
bool Learner::State::Action::isStable()
{
    // check if the robot is still moving
    yarp::os::Bottle query,reply;
    query.addVocab(yarp::os::Vocab::encode("stpd"));
    query.addDouble(50); // acceleration threshold
    query.addDouble(1); // velocity threshold
    learner->controllerClient.write(query,reply);
    //printf("query: %s\n",query.toString().c_str());
    //printf("reply: %s\n",reply.toString().c_str());
    
    if ( reply.get(0).asInt() == 0 )
        return false;
    return true;
}
void Learner::State::Action::relax()
{
    yarp::os::Bottle& stopBottle = learner->commandPort.prepare();
    stopBottle.clear();
    stopBottle.addVocab(yarp::os::Vocab::encode("qfor"));
    
    yarp::os::Bottle forceCmd;
    for (int i=0; i<parentState->dimension(); i++)
        forceCmd.addDouble(0.0);
    
    stopBottle.addList() = forceCmd;
    
    printf("Sending force command: %s\n", stopBottle.toString().c_str());
    learner->commandPort.writeStrict();
    
    yarp::os::Time::delay(1);
    
    printf("Waiting for robot to relax.");
    
    double tShutdown = yarp::os::Time::now();
    while ( !isStable() && yarp::os::Time::now() - tShutdown < timeout)
    {
        printf(".");
        yarp::os::Time::delay(0.2);
    }
    printf("\n");
    
    if(isStable()) {
        printf("action relaxed gracefully\n");
    } else if ( yarp::os::Time::now() - tShutdown > timeout) {
        printf("action relaxation timed out\n");
    }
}


/******************************
*** STATE TRANSITION ACTION ***
******************************/

std::pair<const Learner::State*,double> Learner::State::TransitionAction::belief()
{
    std::pair<const Learner::State*,double> max;
    max.first = transition_belief.begin()->s_prime;
    max.second = transition_belief.begin()->prob;
    for ( std::list<S_Prime>::iterator i = transition_belief.begin(); i != transition_belief.end(); ++i ) {
        if ( i->prob > max.second ) {
            max.first = i->s_prime;
            max.second = i->prob;
        }
    }
    return max;
}

bool Learner::State::TransitionAction::threadInit()
{
    Action::threadInit();
    // activate the attractor at destination_state
    yarp::os::Bottle& attractorCommand = learner->commandPort.prepare();
    attractorCommand.clear();
    attractorCommand.addVocab(yarp::os::Vocab::encode("qatt"));
    yarp::os::Bottle vec;
    for (Point_d::Cartesian_const_iterator i=destination_state->cartesian_begin(); i!=destination_state->cartesian_end(); ++i)
        vec.addDouble(*i);
    attractorCommand.addList() = vec;
    learner->commandPort.writeStrict();
    return 1;
}

void Learner::State::TransitionAction::threadRelease()
{
    if ( learner->getDiscreteState() != parentState )
    {
        printf("STATE TRANSITION OCCURRED\n");
        
        // activate the attractor at currentState
        yarp::os::Bottle vec;
        yarp::os::Bottle& attractorCommand = learner->commandPort.prepare();
        attractorCommand.clear();
        attractorCommand.addVocab(yarp::os::Vocab::encode("qatt"));
        for (Point_d::Cartesian_const_iterator i=learner->getDiscreteState()->cartesian_begin(); i!=learner->getDiscreteState()->cartesian_end(); ++i)
            vec.addDouble(*i);
        attractorCommand.addList() = vec;
        learner->commandPort.writeStrict();
    }
    updateTransitionBelief(learner->getDiscreteState());
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
    printf("current state: %p\n",learner->getDiscreteState());
    
    // check if the robot is still moving
    yarp::os::Bottle query,reply;
    query.addVocab(yarp::os::Vocab::encode("stpd"));
    query.addDouble(50); // acceleration threshold
    query.addDouble(1); // velocity threshold
    learner->controllerClient.write(query,reply);
    //printf("query: %s\n",query.toString().c_str());
    //printf("reply: %s\n",reply.toString().c_str());
    
    // stop if the robot is no longer moving
    if (reply.get(0).asInt()==1) {
        printf("ROBOT STOPPED :-)\n");
        stop();
    }
    else if ( yarp::os::Time::now() - timeStarted > timeout ) {
        printf("STATE TRANSITION TIMED OUT :-(\n");
        stop();
    }
 
}

/********************
 *** REACH ACTION ***
 *******************/
bool Learner::State::ReachAction::threadInit()
{
    Action::threadInit();
    
    // visualize the target point in the world model
    yarp::os::Bottle worldCmd, worldRsp;
    worldCmd.addVocab(yarp::os::Vocab::encode("mk"));
    worldCmd.addVocab(yarp::os::Vocab::encode("sph"));
    worldCmd.addDouble(0.04);
    worldCmd.addDouble(target.x());
    worldCmd.addDouble(target.y());
    worldCmd.addDouble(target.z());
    learner->worldClient.write(worldCmd,worldRsp);
    printf("  world command: %s\n", worldCmd.toString().c_str());
    printf("  world response: %s\n\n", worldRsp.toString().c_str());
    
    // get the name of the newly inserted object
    mobeeObjectName = worldRsp.get(0).asString();
    
    // gains for the control signal
    forceGain = 10000.0;
    torqueGain = 10000.0;
    
    //discreteTime = 0;
    
    // start the reach and let it begin to move the robot
    sendForceCommand(true);
    yarp::os::Time::delay(1);
    
    return 1;
}

void Learner::State::ReachAction::threadRelease()
{
    // remove the target from the model
    yarp::os::Bottle worldCmd, worldRsp;
    worldCmd.addVocab(yarp::os::Vocab::encode("rm"));
    worldCmd.addString(mobeeObjectName);
    learner->worldClient.write(worldCmd,worldRsp);
    printf("  world command: %s\n", worldCmd.toString().c_str());
    printf("  world response: %s\n\n", worldRsp.toString().c_str());
    
    Action::threadRelease();
}

void Learner::State::ReachAction::sendForceCommand( bool withTorque )
{
    // get the state of the hand
    yarp::os::Bottle get,state;
    get.addVocab(yarp::os::Vocab::encode("get"));
    get.addString(marker);
    learner->controllerClient.write(get,state);
    
    // prepare to control the robot with operational space force/torque
    yarp::os::Bottle& opSpaceForceAndTorque = learner->commandPort.prepare();
    opSpaceForceAndTorque.clear();
    opSpaceForceAndTorque.addVocab(yarp::os::Vocab::encode("opsp"));
    opSpaceForceAndTorque.addString(marker);
    
    // get the operational space position of the marker
    Point p = Point(state.get(0).asDouble(),
                    state.get(1).asDouble(),
                    state.get(2).asDouble());
    
    // error vector from marker to target
    Vector err = target - p;
    
    // write the force component of the command into a bottle
    yarp::os::Bottle forceCmd;
    forceCmd.addDouble(forceGain*err.x());
    forceCmd.addDouble(forceGain*err.y());
    forceCmd.addDouble(forceGain*err.z());
    //forceCmd.addDouble(0.0);
    //forceCmd.addDouble(0.0);
    //forceCmd.addDouble(0.0);
    
    // now for the torque component of the command
    Vector torque(CGAL::NULL_VECTOR);
    if (withTorque)
    {
        // get the marker's normal vector 
        Vector n = Vector(state.get(3).asDouble(),
                          state.get(4).asDouble(),
                          state.get(5).asDouble());
        
        // normalize
        double errMag   = sqrt(err.squared_length());
        double nMag     = sqrt(n.squared_length());
        
        if (errMag > 0.001 && nMag > 0.001) {
            Vector eDir = err/errMag;
            Vector nDir = n/nMag;
            printf("eDir %f, %f, %f\n",eDir.x(),eDir.y(),eDir.z());
            printf("nDir %f, %f, %f\n",nDir.x(),nDir.y(),nDir.z());
            double torqueMagnitude = -n*eDir + 1;
            printf("torqueMagnitude: %f\n",torqueMagnitude);
            Vector direction = CGAL::cross_product(nDir,eDir);
            printf("torqueDir %f, %f, %f\n",direction.x(),direction.y(),direction.z());
            torque = torqueMagnitude*direction;
        }
    }
    forceCmd.addDouble(torqueGain*torque.x());
    forceCmd.addDouble(torqueGain*torque.y());
    forceCmd.addDouble(torqueGain*torque.z());
    
    opSpaceForceAndTorque.addList() = forceCmd;
    
    printf("Sending control command: %s\n", opSpaceForceAndTorque.toString().c_str());
    learner->commandPort.writeStrict();
}



void Learner::State::ReachAction::run()
{
    sendForceCommand(true);
        
    // stop if the robot is no longer moving
    if (isStable()) {
        printf("ROBOT STOPPED :-)\n");
        relax();
        stop();
    }
    else if ( yarp::os::Time::now() - timeStarted > timeout ) {
        printf("REACH TIMED OUT :-(\n");
        relax();
        stop();
    }
    
}