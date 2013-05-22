#include <fstream>
#include <unistd.h>
#include <time.h>
//#include <cmath>
#include "learner.h"
//#include "file_reader.h"
#include "actionTransition.h"
#include "actionReach.h"

Learner::Learner( int d, const char* _robotName, const char* _partName, bool connect ) : dimension(d),
    modelUpdate(true),
    discountFactor(0.9),
    modelInterest(100.0),
    rlPrecision(0.001),
    stateTransitionInit(1),
    nextStateIdx(0),
    nextActionIdx(0),
    historyFileName("history.dat"),
    stateFileName("learnerState.dat")
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

/*void Learner::learnStateTransitions( int timesToTryEach )
{
    int count = 0;
    do {
        printf("\nCOUNT: %d\n\n",count);
        if (count%10 == 0) writeNumberedFile("transitions",count);
        
        State* s = getDiscreteState();
        if (s) {
            s->visits++;
            if ( s->exitBelief() < 0.5 ) {
            printf("GOT STUCK IN A STATE WITH NO EXIT\n");
            //break;
            }
        } else break;
        
        Action* a = s->leastTriedTransition();
        if (!a) break;
        
        a->start();
        while ( a->isRunning() ) {yarp::os::Time::delay(1.0);}
        
        count++;
    } while ( leastTriedTransition()->timesTried() < timesToTryEach );
    writeNumberedFile("transitions",count);
}*/


void Learner::initializeReward(double rew)
{
    for ( std::vector<State*>::iterator s = states.begin(); s != states.end(); ++s ){
        for ( std::vector<TransitionAction*>::iterator a = (*s)->transitionActions.begin(); a != (*s)->transitionActions.end(); ++a ){
            (*a)->r = rew;
        }
    }
}

int Learner::statesUnvisited()
{
    int count = 0;
    for ( std::vector<State*>::iterator s = states.begin(); s != states.end(); ++s ){
        if ( (*s)->getVisits() == 0 )
            count++;
    }
    printf("unvisited states: %i\n",count);
    return count;
}

int Learner::actionsUntried()
{
    int count = 0;
    for ( std::vector<State*>::iterator s = states.begin(); s != states.end(); ++s ){
        for ( std::vector<TransitionAction*>::iterator a = (*s)->transitionActions.begin(); a != (*s)->transitionActions.end(); ++a ){
            if ((*a)->getTimesTried() == stateTransitionInit)
                count++;
        }
    }
    printf("untried actions: %i\n",count);
    return count;
}

void Learner::learnModel_IM()
{

    // mark the first state visited
    State* s = getDiscreteState();
    if (s) s->visits++;
    
    int count = 0;
    while (leastTriedTransition()->getTimesTried() < 2)
    {
        valueIteration();
        
        printf("\nCOUNT: %d\n\n",count);
        //if (count%10 == 0) writeNumberedFile("transitionsIM",count);
        writeStateFile();
        
        s = getDiscreteState();
        if (!s) break;

        Action* a = s->greedyTransition();
        if (!a) break;
        
        a->start();
        while ( a->isRunning() ) {yarp::os::Time::delay(1.0);}
        
        //State* lvs = leastVisitedState();
        //if (lvs && lvs->getVisits() > 0)
        //    printf("\nALL STATES HAVE BEEN VISITED!!!\n\n");
        
        count++;
    }
    //writeNumberedFile("transitionsIM",count);
    writeStateFile();
}

void Learner::learnModel_LT()
{
    // mark the first state visited
    State* s = getDiscreteState();
    if (s) s->visits++;
    
    int count = 0;
    while (leastTriedTransition()->getTimesTried() < 1)
    {
        printf("\nCOUNT: %d\n\n",count);
        //if (count%10 == 0) writeNumberedFile("transitionsLT",count);
        writeStateFile();
        
        s = getDiscreteState();
        if (!s) break;
        
        Action* a = s->leastTriedTransition();
        if (!a) break;
        
        a->start();
        while ( a->isRunning() ) {yarp::os::Time::delay(1.0);}
        
        //State* lvs = leastVisitedState();
        //if (lvs && lvs->getVisits() > 0)
        //    printf("\nALL STATES HAVE BEEN VISITED!!!\n\n");
        
        count++;
    }
    //writeNumberedFile("transitionsLT",count);
    writeStateFile();
}

void Learner::learnModel_Rand()
{
    // mark the first state visited
    State* s = getDiscreteState();
    if (s) s->visits++;
    
    int count = 0;
    while (leastTriedTransition()->getTimesTried() < 1)
    {
        printf("\nCOUNT: %d\n\n",count);
        //if (count%10 == 0)
        writeStateFile();
        
        s = getDiscreteState();
        if (!s) break;
        
        Action* a = s->randomTransition();
        if (!a) break;
        
        a->start();
        while ( a->isRunning() ) {yarp::os::Time::delay(1.0);}
        
        //State* lvs = leastVisitedState();
        //if (lvs && lvs->getVisits() > 0)
        //    printf("\nALL STATES HAVE BEEN VISITED!!!\n\n");
        
        count++;
    }
    writeStateFile();
}

void  Learner::valueIteration()
{
    mutex.wait();
    printf("\nDOING RL...\n");
    
    double maxDelta;
    int count = 0;
    do
    {
        maxDelta=0;
        //printf("*** ITERATION: %d ***\n",count);
        for (std::vector<State*>::iterator i = states.begin(); i!=states.end(); ++i)
        {
            (*i)->computeNewValue();
            
            //printf("\tstate_new_value: %f\n",(*i)->getNewValue());
            //printf("\tstate_value: %f\n",(*i)->getValue());
            
            double delta = fabs( (*i)->getNewValue() - (*i)->getValue() );
            //printf("\tstate_delta: %f\n",delta);
            if ( delta > maxDelta )
                maxDelta = delta;
        }
        for (std::vector<State*>::iterator i = states.begin(); i!=states.end(); ++i)
            (*i)->updateValue();
        //printf("\tmax_delta: %f\n",maxDelta);
        count++;
    } while ( maxDelta > rlPrecision );
    mutex.post();
}


/*void Learner::resetRewardMatrix(std::vector<Action*> rewardedActions)
{
    for ( std::vector<State*>::iterator s = states.begin(); s != states.end(); ++s ){
        for ( std::vector<TransitionAction*>::iterator a = (*s)->transitionActions.begin(); a != (*s)->transitionActions.end(); ++a ){
            (*a)->r = 0.0;
            for ( std::vector<Action*>::iterator ra = rewardedActions.begin(); ra != rewardedActions.end(); ++ra ){
                if ( *ra == *a ) (*a)->r = 1.0;
            }
        }
        for ( std::vector<ReachAction*>::iterator a = (*s)->reachActions.begin(); a != (*s)->reachActions.end(); ++a ){
            (*a)->r = 0.0;
            for ( std::vector<Action*>::iterator ra = rewardedActions.begin(); ra != rewardedActions.end(); ++ra ){
                if ( *ra == *a ) (*a)->r = 1.0;
            }
        }
    }
    
}*/

State* Learner::leastVisitedState()
{
    State* state = NULL;
    std::vector<State*> shuffleStates = states;
    std::random_shuffle(shuffleStates.begin(), shuffleStates.end());
    for ( std::vector<State*>::iterator s = shuffleStates.begin(); s != shuffleStates.end(); ++s ){
        if ( !state || (*s)->getVisits() < state->getVisits() )
            state = *s;
    }
    return state;
}

Action* Learner::leastTriedTransition()
{
    TransitionAction* a = NULL;
    State* origin = NULL;
    
    std::vector<State*> shuffleStates = states;
    std::random_shuffle(shuffleStates.begin(), shuffleStates.end());
    for ( std::vector<State*>::iterator s = shuffleStates.begin(); s != shuffleStates.end(); ++s ){
        if ( (*s)->visits > 0 ) {
            TransitionAction* b = (*s)->leastTriedTransition();
            if ( !a || b->getTimesTried() < a->getTimesTried() ) {
                a = b;
                origin = *s;
            }
            //if ( a->timesTried() == 0 )
            //    return a;
        }
    }
    printf("GLOBALLY LEAST TRIED STATE TRANSITION IS FROM %p TO %p (%d times)\n",origin,a->destination_state,a->getTimesTried());
    return a;
}

/*void Learner::tryReaches( Point_3 p )
{
    for ( int count = 0; count < 1000; count++ )
    {
        printf("\nCOUNT: %d\n\n",count);
        
        if (count%10==0)
            writeNumberedFile("outFile",count);
        
        State* s = getDiscreteState();
        Action* a = s->explore();

        a->start(p);
        while ( a->isRunning() ) {yarp::os::Time::delay(1.0);}
            
        valueIteration();
    }
    writeNumberedFile("lastFile");
}*/

/*void workOnTarget(Point_3 p, bool endEarly = false)
{
    
}*/

/*void Learner::reachTargets(std::vector<Point_3> targets)
{
    if (!targets.size()) return;
    
     CHOOSE AN RL PROBLEM
        a) enumerate all reach targets
        b) choose the next 'easiest' reach target
            - should predict a high reward
            - should not have been tried very much
    
    
      SOLVE THE RL PROBLEM
        a) work on the problem until it's done
            - try every reach in the state space
        b) work on the rl problem until it becomes boring
            - the more state space is unexplored, the more likely it should be that we continue exploring/learning
            - the better our best reach is, the more likely we move on
 
    
    // initially pick a random reach target
    Point_3 reachTarget = targets.at(rand()%targets.size());
    yarp::os::ConstString targetName;
    
    targetName = mkSphere(reachTarget.x(), reachTarget.y(), reachTarget.z(), 0.02);
    
    

    
    rmGeom(targetName);
}

void Learner::writeNumberedFile( std::string outFileBaseName, int num )
{
    std::stringstream suffix;
    suffix << num << ".ini";
    std::string outFile = outFileBaseName + suffix.str();
    writeFile(outFile);
}*/

State* Learner::appendState( Point_d& p, int numVisits )
{
    State* newState = new State( nextStateIdx++,redimension(p), this );
    newState->visits = numVisits;
    states.push_back(newState);
    return newState;
}

TransitionAction* Learner::appendTransitionAction( State* a, State* b, double val, double rew, int num )
{
    if ( !a || !b ) {
        printf("WARNING: Tried to connect non-existent states\n");
        return NULL;
    }
    TransitionAction* action = new TransitionAction( nextActionIdx++, a, b, val, rew, num);
    a->transitionActions.push_back( action );

    return action;
}

ReachAction* Learner::appendReachAction( State* s, yarp::os::ConstString m, double val, double rew, int num )
{
    if ( !s ) return NULL;
    ReachAction* reach = new ReachAction( nextActionIdx++, m, s, val, rew, num);
    s->reachActions.push_back(reach);
    return reach;
}

/*double Learner::generateValueFunction(Point_3 p)
{
    double maxReward = 0.0;
    for ( std::vector<State*>::iterator i=states.begin(); i!=states.end(); ++i ) {
        for ( std::vector<ReachAction*>::iterator j = (*i)->reachActions.begin(); j != (*i)->reachActions.end(); ++j ) {
            if ( (*j)->predictReward(p) > maxReward )
                maxReward = (*j)->reward();
            printf("Reach action: %p - predicted reward %f\n",*j,(*j)->reward());
        }
    }
    valueIteration();
    return maxReward;
}*/

Point_d Learner::redimension(Point_d& p)
{
    int dimCount = 0;
    std::vector<double> q;
    Point_d::Cartesian_const_iterator i = p.cartesian_begin();
    //std::cout << "Fixing: " << p << std::endl;
    while ( dimCount < dimension ) {
        if ( i != p.cartesian_end() ) {
            q.push_back(*i);
            ++i;
        } else { q.push_back(0.5); }
        dimCount++;
    }
    return Point_d(dimension,q.begin(),q.end());
}

Point_d Learner::getRealState() {
    yarp::os::Bottle* b = statePort.read();
    std::vector<double> bList;
    for (int i=0; i<b->size(); i++)
        bList.push_back(b->get(i).asDouble());
    return Point_d(bList.size(),bList.begin(),bList.end());
}

State* Learner::getDiscreteState() {
    Point_d q = getRealState();
    State* nearestState = NULL;
    if ( states.size() > 0 ) {
        nearestState = *(states).begin();
        for (std::vector<State*>::iterator j=states.begin(); j!=states.end(); ++j) {
            //std::cout << "    j: " << **j << "\t\t" << (q-**j).squared_length() << std::endl;
            if ( (q-**j).squared_length() < (q-*nearestState).squared_length() ) {
                nearestState = *j;
            }
        }
    }
    if (!nearestState)
        printf("\nFAILED TO GET DISCRETE STATE BECAUSE THERE ARE NO STATES IN THE LEARNER!!!\n\n");
    return nearestState;
}

/*bool Learner::deleteState( const State* s )
{
    for (std::vector<State*>::iterator i=states.begin(); i!=states.end(); ++i) {
        for (std::vector<TransitionAction*>::iterator j=(*i)->transitionActions.begin(); j!=(*i)->transitionActions.end(); ++j) {
            for (std::vector< TransitionAction::S_Prime >::iterator k=(*j)->transition_belief.begin(); k!=(*j)->transition_belief.end(); ++k) {
                if (k->s_prime==s) {
                    std::vector< TransitionAction::S_Prime >::iterator K=k;
                    (*j)->transition_belief.erase(k,++K);
                }
            }
            if ((*j)->destination_state == s) {
                std::vector<TransitionAction*>::iterator J=j,deadTransition=j;
                (*i)->transitionActions.erase(j,++J);
                delete *deadTransition;
            }
        }
        if (*i==s) {
            std::vector<State*>::iterator I=i,deadState=i;
            states.erase(i,++I);
            delete *deadState;
        }
    }
    delete s;
}*/

bool Learner::isStable()
{
    // check if the robot is still moving
    yarp::os::Bottle query,reply;
    query.addVocab(yarp::os::Vocab::encode("stpd"));
    query.addDouble(100); // 50 acceleration threshold
    query.addDouble(2); // 1.0 velocity threshold
    controllerClient.write(query,reply);
    //printf("query: %s\n",query.toString().c_str());
    //printf("reply: %s\n",reply.toString().c_str());
    
    if ( reply.get(0).asInt() == 0 )
        return false;
    return true;
}

void Learner::setAttractor( Point_d q )
{
    // activate the attractor at q
    yarp::os::Bottle& attractorCommand = commandPort.prepare();
    attractorCommand.clear();
    attractorCommand.addVocab(yarp::os::Vocab::encode("qatt"));
    yarp::os::Bottle vec;
    for (Point_d::Cartesian_const_iterator i=q.cartesian_begin(); i!=q.cartesian_end(); ++i)
        vec.addDouble(*i);
    attractorCommand.addList() = vec;
    //printf("Sending attractor command: %s\n", attractorCommand.toString().c_str());
    commandPort.writeStrict();
}

void Learner::setOpSpace( yarp::os::ConstString name, Vector_3 f, Vector_3 t )
{
    yarp::os::Bottle forceCmd;
    forceCmd.addDouble(f.x());
    forceCmd.addDouble(f.y());
    forceCmd.addDouble(f.z());
    forceCmd.addDouble(t.x());
    forceCmd.addDouble(t.y());
    forceCmd.addDouble(t.z());
    
    // prepare to control the robot with operational space force/torque
    yarp::os::Bottle& opSpaceForceAndTorque = commandPort.prepare();
    opSpaceForceAndTorque.clear();
    opSpaceForceAndTorque.addVocab(yarp::os::Vocab::encode("opsp"));
    opSpaceForceAndTorque.addString(name);
    opSpaceForceAndTorque.addList() = forceCmd;
    
    //printf("Sending control command: %s\n", opSpaceForceAndTorque.toString().c_str());
    commandPort.writeStrict();
}

yarp::os::ConstString Learner::mkSphere( double x, double y, double z, double r )
{
    yarp::os::Bottle worldCmd, worldRsp;
    worldCmd.addVocab(yarp::os::Vocab::encode("mk"));
    worldCmd.addVocab(yarp::os::Vocab::encode("sph"));
    worldCmd.addDouble(r);
    worldCmd.addDouble(x);
    worldCmd.addDouble(y);
    worldCmd.addDouble(z);
    worldClient.write(worldCmd,worldRsp);
    //printf("  world command: %s\n", worldCmd.toString().c_str());
    //printf("  world response: %s\n\n", worldRsp.toString().c_str());
    
    // get the name of the newly inserted object
    return worldRsp.get(0).asString();
    
    // make it a "target" object
    //worldCmd.clear();
    //worldRsp.clear();
    //worldCmd.addVocab(yarp::os::Vocab::encode("def"));
    //worldCmd.addString(mobeeObjectName);
    //worldCmd.addVocab(yarp::os::Vocab::encode("tgt"));
    //learner->worldClient.write(worldCmd,worldRsp);
    
    //printf("  world command: %s\n", worldCmd.toString().c_str());
    //printf("  world response: %s\n\n", worldRsp.toString().c_str());
}

void Learner::rmGeom( yarp::os::ConstString& name )
{
    // remove the target from the model
    yarp::os::Bottle worldCmd, worldRsp;
    worldCmd.addVocab(yarp::os::Vocab::encode("rm"));
    worldCmd.addString(name);
    worldClient.write(worldCmd,worldRsp);
    //printf("  world command: %s\n", worldCmd.toString().c_str());
    //printf("  world response: %s\n\n", worldRsp.toString().c_str());
}


void Learner::defTarget( yarp::os::ConstString& name )
{
    yarp::os::Bottle worldCmd, worldRsp;
    worldCmd.addVocab(yarp::os::Vocab::encode("def"));
    worldCmd.addString(name);
    worldCmd.addVocab(yarp::os::Vocab::encode("tgt"));
    worldClient.write(worldCmd,worldRsp);
}

void Learner::defObstacle( yarp::os::ConstString& name )
{
    yarp::os::Bottle worldCmd, worldRsp;
    worldCmd.addVocab(yarp::os::Vocab::encode("def"));
    worldCmd.addString(name);
    worldCmd.addVocab(yarp::os::Vocab::encode("obs"));
    worldClient.write(worldCmd,worldRsp);
}

void Learner::getMarkerState( yarp::os::ConstString& markerName, Point_3& p, Vector_3& n)
{
    // get the state of the hand
    yarp::os::Bottle get,state;
    get.addVocab(yarp::os::Vocab::encode("get"));
    get.addString(markerName);
    controllerClient.write(get,state);
    
    // get the operational space position of the marker
    p = Point_3(state.get(0).asDouble(),
              state.get(1).asDouble(),
              state.get(2).asDouble());
    n = Vector_3(state.get(3).asDouble(),
               state.get(4).asDouble(),
               state.get(5).asDouble());
}

void Learner::stopForcing()
{
    yarp::os::Bottle& stopBottle = commandPort.prepare();
    stopBottle.clear();
    stopBottle.addVocab(yarp::os::Vocab::encode("qfor"));
    yarp::os::Bottle forceCmd;
    for (int i=0; i<dimension; i++)
        forceCmd.addDouble(0.0);
    stopBottle.addList() = forceCmd;
    //printf("Sending force command: %s\n", stopBottle.toString().c_str());
    commandPort.writeStrict();
}


State* Learner::getState(int _idx)
{
    for (std::vector<State*>::iterator i=states.begin(); i!=states.end(); ++i)
        if ((*i)->idx == _idx) return *i;
    return NULL;
}

TransitionAction* Learner::getTransitionAction(int _idx)
{
    for (std::vector<State*>::iterator i=states.begin(); i!=states.end(); ++i)
        for (std::vector<TransitionAction*>::iterator j=(*i)->transitionActions.begin(); j!=(*i)->transitionActions.end(); ++j)
            if ((*j)->idx == _idx) return *j;
    return NULL;
}


ReachAction* Learner::getReachAction(int _idx)
{
    for (std::vector<State*>::iterator i=states.begin(); i!=states.end(); ++i)
        for (std::vector<ReachAction*>::iterator j=(*i)->reachActions.begin(); j!=(*i)->reachActions.end(); ++j)
            if ((*j)->idx == _idx) return *j;
    return NULL;
}

bool Learner::checkStateIndex(State* s)
{
    for (std::vector<State*>::iterator i=states.begin(); i!=states.end(); ++i)
        if ( *i != s && (*i)->idx == s->idx) return false;
    return true;
}

bool Learner::checkActionIndex(Action* a)
{
    for (std::vector<State*>::iterator i=states.begin(); i!=states.end(); ++i) {
        for (std::vector<TransitionAction*>::iterator j=(*i)->transitionActions.begin(); j!=(*i)->transitionActions.end(); ++j)
            if ( *j != a && (*j)->idx == a->idx) return false;
        for (std::vector<ReachAction*>::iterator j=(*i)->reachActions.begin(); j!=(*i)->reachActions.end(); ++j)
            if ( *j != a && (*j)->idx == a->idx) return false;
    }   
    return true;
}

void Learner::print(bool printAll)
{
    //printf("\n\n");
    //int stateCount=0;
    for (std::vector<State*>::iterator i=states.begin(); i!=states.end(); ++i)
    {
        printf("\nState %p - ",*i);
        std::cout << **i << std::endl;
        
        //printf("\t\t\tsPrime:");
       
           /*for (std::vector< S_Prime >::iterator k=(*(*i)->transitionActions.begin())->transition_belief.begin(); k!=(*(*i)->transitionActions.begin())->transition_belief.end(); ++k)
            {
                printf("\t\t%p", k->s_prime);
            }
            printf("\n");
        */
        
        int actionCount=0;
        for (std::vector<TransitionAction*>::iterator j=(*i)->transitionActions.begin(); j!=(*i)->transitionActions.end(); ++j)
        {
            printf("  Action %p, dest: %p, times: %f",*j,(*j)->destination_state,(*j)->num);
            for (std::vector< S_Prime* >::iterator k=(*j)->transition_belief.begin(); k!=(*j)->transition_belief.end(); ++k)
            {
                printf("\t(%p - %f, %d)", (*k)->state, (*k)->prob, (*k)->num);
            }
            printf("\n");
            //printf("\n Starting Action.....\n");
            //(*j)->start();
            //(*j)->stop();
        }
        for (std::vector<ReachAction*>::iterator j=(*i)->reachActions.begin(); j!=(*i)->reachActions.end(); ++j)
        {
            printf("  ReachAction %p, marker: %s\n",*j,(*j)->marker.c_str());
        }
    }
    printf("\n");
}

bool Learner::loadStateFile( std::string& filename )
{
    int maxStateIdx = 0;
    int maxActionIdx = 0;

	std::ifstream in_file(filename.c_str());
    
	if (!in_file) {
		std::cerr << "No File Found: " << filename << std::endl;
		return false;
	}
    
    int doWhat = -1;
    while ( in_file.good() )
    {
		std::string line;
		getline(in_file, line);
        //std::remove(line.begin(), line.end(), ' ');
        
        if ( line.empty() || line.compare(0,1,"#") == 0 ) { continue; }
        else if ( line.compare("STATES")==0 ) { doWhat = 0; continue; }
        else if ( line.compare("TRANSITION_ACTIONS")==0 ) { doWhat = 1; continue; }
        else if ( line.compare("TRANSITION_BELIEFS")==0 ) { doWhat = 2; continue; }
        else if ( line.compare("REACH_ACTIONS")==0 ) { doWhat = 3; continue; }
        else if ( line.compare("REACH_BELIEFS")==0 ) { doWhat = 4; continue; }
        
        if ( doWhat == 0 ) // get the states/poses
        {
            std::istringstream line_reader(line);
            std::vector<double> pose;
            double visits;
            int idx;
            line_reader >> idx;
            line_reader >> visits;
            while (line_reader.good()) {
                float number;
                line_reader >> number;
                pose.push_back(number);
            }
            if (!pose.size())
                continue;
            Point_d q(pose.size(),pose.begin(),pose.end());
            State* s = appendState(q,visits);
            s->idx = idx;
            if ( idx > maxStateIdx )
                maxStateIdx = idx;
            if (!checkStateIndex(s)) {
                printf("FATAL FILE READ ERROR: Invalid state index.\n");
                return false;
            }
        }
        else if ( doWhat == 1 ) // connect the states with transition actions
        {
            int idx,a,b,num;
            double reward,value;
            std::istringstream line_reader(line);
            line_reader >> idx;
            line_reader >> a;
            line_reader >> b;
            line_reader >> num;
            line_reader >> reward;
            line_reader >> value;
            //transitionActions.push_back(  );
            State* stateA = getState(a);
            State* stateB = getState(b);
            if (!stateA || !stateB) {
                printf("FATAL FILE READ ERROR: State index (for a transition) not found.\n");
                return false;
            }
            TransitionAction* t = appendTransitionAction(stateA,stateB,value,reward,num);
            t->idx = idx;
            if ( idx > maxActionIdx )
                maxActionIdx = idx;
            if (!checkActionIndex(t)) {
                printf("FATAL FILE READ ERROR: Invalid state transition index.\n");
                return false;
            }
        }
        else if ( doWhat == 2 ) // initialize the state transition beliefs/probabilities
        {
            int transitionIdx, s_prime, num;
            double prob;
            std::istringstream line_reader(line);
            line_reader >> transitionIdx;
            line_reader >> s_prime;
            line_reader >> num;
            line_reader >> prob;
            
            TransitionAction* transition = getTransitionAction(transitionIdx);
            if (!transition) {
                printf("FATAL FILE READ ERROR: State transition index not found.\n");
                return false;
            }
            
            State* s = getState(s_prime);
            if (!s) {
                printf("FATAL FILE READ ERROR: S_Prime index not found.\n");
                return false;
            }
            
            printf("appending s_prime: %p to action: %p\n",s,transition);
            
            S_Prime* sp = transition->findOrAppendSPrime(s);
            sp->num = num;
            sp->prob = prob;
        }
        else if ( doWhat == 3 ) // append reach actions to states
        {
            int stateIdx,reachIdx,num;
            std::string m;
            double reward,value;
            std::istringstream line_reader(line);
            line_reader >> reachIdx;
            line_reader >> stateIdx;
            line_reader >> m;
            line_reader >> num;
            line_reader >> reward;
            line_reader >> value;
            
            State* s = getState(stateIdx);
            if (!s) {
                printf("FATAL FILE READ ERROR: State index (for a reach) not found.\n");
                return false;
            }
            ReachAction* r = appendReachAction(s,m.c_str(),value,reward,num);
            r->idx = reachIdx;
            if ( reachIdx > maxActionIdx )
                maxActionIdx = reachIdx;
            if (!checkActionIndex(r)) {
                printf("FATAL FILE READ ERROR: Invalid reach index.\n");
                return false;
            }
            
        }
        else if ( doWhat == 4 ) // append reward beliefs to reach actions
        {
            int reachIdx;
            double x,y,z,a,b,c,r;
            std::istringstream line_reader(line);
            line_reader >> reachIdx;
            line_reader >> x;
            line_reader >> y;
            line_reader >> z;
            line_reader >> a;
            line_reader >> b;
            line_reader >> c;
            line_reader >> r;
            
            ReachAction* reach = getReachAction(reachIdx);
            
            if (!reach) {
                printf("FATAL FILE READ ERROR: Reach index not found.\n");
                return false;
            }
            reach->appendToHistory(Point_3(x,y,z),Point_3(a,b,c),r);
        }
    }
    nextStateIdx = maxStateIdx + 1;
    nextActionIdx = maxActionIdx + 1;
    return true;
}

void Learner::writeStateFile()
{
	std::ofstream out_file(stateFileName.c_str());
   
    int stateCount = 0;
    out_file << "STATES" << std::endl;
    for ( std::vector<State*>::iterator i=states.begin(); i!=states.end(); ++i ) {
        //(*i)->tempIdx = stateCount;
        out_file << (*i)->idx << "\t";
        out_file << (*i)->visits << "\t";
        for ( Point_d::Cartesian_const_iterator j = (*i)->cartesian_begin(); j != (*i)->cartesian_end(); ++j ) {
            out_file << *j << " ";
        }
        out_file << std::endl;
        stateCount++;
    }
    out_file << std::endl;
    
    
    out_file << std::fixed << std::setprecision(10);
    
    printf("\n\n********************\n");
    int transitionCount = 0;
    out_file << "TRANSITION_ACTIONS" << std::endl;
    for ( std::vector<State*>::iterator i=states.begin(); i!=states.end(); ++i ) {
        //printf("state: %p\n",*i);
        for ( std::vector<TransitionAction*>::iterator j = (*i)->transitionActions.begin(); j != (*i)->transitionActions.end(); ++j ) {
            //printf(" transition: %p\n",*j);
            //(*j)->tempIdx = transitionCount;
            out_file << (*j)->idx << "\t"
                     << (*j)->parentState->idx << "\t"
                     << (*j)->destination_state->idx << "\t"
                     << (*j)->getTimesTried() << "\t"
                     << (*j)->getReward() << "\t"
                     << (*j)->getValue()
                     << std::endl;
            transitionCount++;
        }
    }
    out_file << std::endl;
    //printf("\n********************\n\n");
    
    out_file << "TRANSITION_BELIEFS" << std::endl;
    for ( std::vector<State*>::iterator i=states.begin(); i!=states.end(); ++i ) {
        for ( std::vector<TransitionAction*>::iterator j = (*i)->transitionActions.begin(); j != (*i)->transitionActions.end(); ++j ) {
            for ( std::vector<S_Prime*>::iterator k = (*j)->transition_belief.begin(); k != (*j)->transition_belief.end(); ++k ) {
                out_file << (*j)->idx << "\t"
                         << (*k)->state->idx << "\t"
                         << (*k)->num << "\t"
                << (*k)->prob << std::endl;
            }
        }
    }
    out_file << std::endl;
    
    int reachCount = 0;
    out_file << "REACH_ACTIONS" << std::endl;
    for ( std::vector<State*>::iterator i=states.begin(); i!=states.end(); ++i ) {
        for ( std::vector<ReachAction*>::iterator j = (*i)->reachActions.begin(); j != (*i)->reachActions.end(); ++j ) {
            //(*j)->tempIdx = reachCount++;
            out_file << (*j)->idx << "\t"
                     << (*j)->parentState->idx << "\t"
                     << (*j)->marker << "\t"
                     << (*j)->num << "\t"
                     << (*j)->r << "\t"
                     << (*j)->v
                     << std::endl;
        }
    }
    out_file << std::endl;
    
    out_file << "REACH_BELIEFS" << std::endl;
    for ( std::vector<State*>::iterator i=states.begin(); i!=states.end(); ++i ) {
        for ( std::vector<ReachAction*>::iterator j = (*i)->reachActions.begin(); j != (*i)->reachActions.end(); ++j ) {
            for ( std::vector< ReachAction::HistoryItem >::iterator k=(*j)->history.begin(); k!=(*j)->history.end(); ++k ) {
                out_file << (*j)->idx << "\t"
                         << k->target.cartesian(0) << "\t"
                         << k->target.cartesian(1) << "\t"
                         << k->target.cartesian(2) << "\t"
                         << k->result.cartesian(0) << "\t"
                         << k->result.cartesian(1) << "\t"
                         << k->result.cartesian(2) << "\t"
                         << k->reward
                         << std::endl;
            }
            
        }
    }
    out_file << std::endl;
}

void Learner::writeHistoryFile( int unvisited, int untried, int state, int action, double reward, double value ) {
    std::ofstream myfile;
    myfile << std::fixed << std::setprecision(10);
    myfile.open (historyFileName.c_str(), std::ios_base::app);
    myfile << unvisited << "\t" << untried << "\t" << state << "\t" << action << "\t" << reward << "\t" << value << "\n";
    myfile.close();
}