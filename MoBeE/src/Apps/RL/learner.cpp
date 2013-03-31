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
    query.addDouble(1.0); // velocity threshold
    learner->controllerClient.write(query,reply);
    //printf("query: %s\n",query.toString().c_str());
    //printf("reply: %s\n",reply.toString().c_str());
    
    if ( reply.get(0).asInt() == 0 )
        return false;
    return true;
}

void Learner::State::Action::setAttractor( Point_d q )
{
    // activate the attractor at q
    yarp::os::Bottle& attractorCommand = learner->commandPort.prepare();
    attractorCommand.clear();
    attractorCommand.addVocab(yarp::os::Vocab::encode("qatt"));
    yarp::os::Bottle vec;
    for (Point_d::Cartesian_const_iterator i=q.cartesian_begin(); i!=q.cartesian_end(); ++i)
        vec.addDouble(*i);
    attractorCommand.addList() = vec;
    printf("Sending attractor command: %s\n", attractorCommand.toString().c_str());
    learner->commandPort.writeStrict();
}

void Learner::State::Action::setOpSpace( yarp::os::ConstString name, Vector f, Vector t )
{
    yarp::os::Bottle forceCmd;
    forceCmd.addDouble(f.x());
    forceCmd.addDouble(f.y());
    forceCmd.addDouble(f.z());
    forceCmd.addDouble(t.x());
    forceCmd.addDouble(t.y());
    forceCmd.addDouble(t.z());
    
    // prepare to control the robot with operational space force/torque
    yarp::os::Bottle& opSpaceForceAndTorque = learner->commandPort.prepare();
    opSpaceForceAndTorque.clear();
    opSpaceForceAndTorque.addVocab(yarp::os::Vocab::encode("opsp"));
    opSpaceForceAndTorque.addString(name);
    opSpaceForceAndTorque.addList() = forceCmd;
    
    printf("Sending control command: %s\n", opSpaceForceAndTorque.toString().c_str());
    learner->commandPort.writeStrict();
}

void Learner::State::Action::stopForcing()
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
}

bool Learner::State::Action::waitForSteady()
{
    printf("Waiting for steady state.");
    double tShutdown = yarp::os::Time::now();
    yarp::os::Time::delay(1);
    while ( yarp::os::Time::now() - tShutdown < timeout)
    {
        printf(".");
        if (isStable()) {
            printf("\n");
            return true;
        }
        yarp::os::Time::delay(0.2);
    }
    printf("\n");
    return false;
}

void Learner::State::Action::relax()
{
    printf("Waiting for robot to relax.");
    stopForcing();
    if ( waitForSteady() ) printf("how relaxing...\n");
    else printf("RELAX TIMED OUT!!!! :-0\n");
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
Point Learner::State::ReachAction::easyReach()
{
    Point p;
    Vector norm,noise((double)rand()/RAND_MAX,(double)rand()/RAND_MAX,(double)rand()/RAND_MAX);
    getMarkerState(p, norm);
    norm = norm/sqrt(norm.squared_length());
    return p + 0.15*norm ;//+ 0.1*noise;
}
bool Learner::State::ReachAction::threadInit()
{
    Action::threadInit();
    
    // visualize the target point in the world model
    yarp::os::Bottle worldCmd, worldRsp;
    worldCmd.addVocab(yarp::os::Vocab::encode("mk"));
    worldCmd.addVocab(yarp::os::Vocab::encode("sph"));
    worldCmd.addDouble(0.02);
    worldCmd.addDouble(reachTarget.x());
    worldCmd.addDouble(reachTarget.y());
    worldCmd.addDouble(reachTarget.z());
    learner->worldClient.write(worldCmd,worldRsp);
    printf("  world command: %s\n", worldCmd.toString().c_str());
    printf("  world response: %s\n\n", worldRsp.toString().c_str());
    
    // get the name of the newly inserted object
    mobeeObjectName = worldRsp.get(0).asString();
    
    worldCmd.clear();
    worldRsp.clear();
    worldCmd.addVocab(yarp::os::Vocab::encode("def"));
    worldCmd.addString(mobeeObjectName);
    worldCmd.addVocab(yarp::os::Vocab::encode("tgt"));
    learner->worldClient.write(worldCmd,worldRsp);
    printf("  world command: %s\n", worldCmd.toString().c_str());
    printf("  world response: %s\n\n", worldRsp.toString().c_str());
    
    // gains for the control signal
    forceGain = 1000.0;
    torqueGain = 1000.0;
    
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


void Learner::State::ReachAction::getMarkerState(Point& p, Vector& n)
{
    // get the state of the hand
    yarp::os::Bottle get,state;
    get.addVocab(yarp::os::Vocab::encode("get"));
    get.addString(marker);
    learner->controllerClient.write(get,state);
    
    // get the operational space position of the marker
    p = Point(state.get(0).asDouble(),
              state.get(1).asDouble(),
              state.get(2).asDouble());
    n = Vector(state.get(3).asDouble(),
               state.get(4).asDouble(),
               state.get(5).asDouble());
}

void Learner::State::ReachAction::sendForceCommand( bool withTorque )
{
    // get the state of the hand
    Point p;
    Vector n;
    getMarkerState(p,n);
    
    // error vector from marker to target
    Vector err = reachTarget - p;
    Vector force(forceGain*err);
    
    // now for the torque component of the command
    Vector torque(CGAL::NULL_VECTOR);
    if (withTorque)
    {
        // normalize stuff
        double errMag   = sqrt(err.squared_length());
        double nMag     = sqrt(n.squared_length());
        
        if (errMag > 0.001 && nMag > 0.001)
        {
            Vector eDir = err/errMag;
            Vector nDir = n/nMag;
            //printf("eDir %f, %f, %f\n",eDir.x(),eDir.y(),eDir.z());
            //printf("nDir %f, %f, %f\n",nDir.x(),nDir.y(),nDir.z());
            
            double torqueMagnitude = -n*eDir;
            Vector torqueDirection = CGAL::cross_product(nDir,eDir);
            //printf("torqueMagnitude: %f\n",torqueMagnitude);
            //printf("torqueDir %f, %f, %f\n",direction.x(),direction.y(),direction.z());
            
            //err = err * torqueMagnitude;
            torque = (torqueMagnitude+1)/2*torqueDirection;
        }
    }
    setOpSpace(marker, force, torque);
}



void Learner::State::ReachAction::run()
{
    // get the residual error to target
    Point p;
    Vector n;
    getMarkerState(p,n);
    double err = (reachTarget - p).squared_length();
    printf("REACH |err| = %f \n", err);
    
    if ( err < 0.015 )
    {
        //    history.push_back( std::pair<Point,bool>(reachTarget,true) );
  
        printf("\nFOUND REACH TARGET!!! Set Position pre-reach pose.\n");
        setAttractor(*parentState);
        //yarp::os::Time::delay(5.0);
        
        stopForcing();
        if ( !waitForSteady() ) printf("RELAX TIMED OUT!!!! :-0\n");
        
        //printf("\nRETURNING TO NEAREST STATE.\n");
        //setAttractor(*learner->getDiscreteState());
        stop();
    }
    else if (isStable())
    {
        printf("\n STEADY STATE REACHED, STOP FORCING\n");
        setAttractor(*parentState);
        stopForcing();
        if ( !waitForSteady() ) printf("RELAX TIMED OUT!!!! :-0\n");
        stop();
    }
    else if ( yarp::os::Time::now() - timeStarted > timeout ) {
        printf("REACH TIMED OUT, STOP FORCING :-(\n");
        setAttractor(*parentState);
        stopForcing();
        if ( !waitForSteady() ) printf("RELAX TIMED OUT!!!! :-0\n");
        stop();
    }
    
    //setAttractor(learner->getRealState());
    sendForceCommand(true);
}