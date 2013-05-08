#include <fstream>
#include <unistd.h>
#include <time.h>
//#include <cmath>
#include "learner.h"
//#include "file_reader.h"
#include "actionTransition.h"
#include "actionReach.h"

Learner::Learner( int d, const char* _robotName, const char* _partName, bool connect ) : dimension(d)//, currentState(NULL), currentAction(NULL)
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


void Learner::tryStateTransitions( int num )
{
    for ( int count = 0; count < num; count++ )
    {
        printf("\nCOUNT: %d\n\n",count);
        
        if (count%10 == 0)
            writeNumberedFile("outFile",count);
    
        Action* a = getDiscreteState()->leastTriedTransition();
        
        a->start();
        while ( a->isRunning() ) {yarp::os::Time::delay(1.0);}
        
        doRL();
    }
    writeNumberedFile("lastFile");
}

void Learner::tryReaches( Point_3 p )
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
            
        doRL();
    }
    writeNumberedFile("lastFile");
}

void Learner::reachTargets(std::vector<Point_3> targets)
{
    if (!targets.size()) return;
    
    /* CHOOSE AN RL PROBLEM
        a) enumerate all reach targets
        b) choose the next 'easiest' reach target
            - should predict a high reward
            - should not have been tried very much
    */
    
    /*  SOLVE THE RL PROBLEM
        a) work on the problem until it's done
            - try every reach in the state space
        b) work on the rl problem until it becomes boring
            - the more state space is unexplored, the more likely it should be that we continue exploring/learning
            - the better our best reach is, the more likely we move on
    */
    
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
}

State* Learner::appendState( Point_d& p )
{
    State* newState = new State( redimension(p) );
    states.push_back(newState);
    return newState;
}

TransitionAction* Learner::appendTransitionAction( State* a, State* b, double val, double rew, int num )
{
    if ( !a || !b ) {
        printf("WARNING: Tried to connect non-existent states\n");
        return NULL;
    }
    TransitionAction* action = new TransitionAction( this, a, b, val, rew, num);
    a->transitionActions.push_back( action );
    
    // optimistic initialization
    action->appendSPrime( b, 1, 1.0);
    action->num++;
    
    return action;
}

ReachAction* Learner::appendReachAction( State* s, yarp::os::ConstString m, double val, double rew, int num )
{
    if ( !s ) return NULL;
    ReachAction* reach = new ReachAction(this,m,s,val,rew,num);
    s->reachActions.push_back(reach);
    return reach;
}

double Learner::generateValueFunction(Point_3 p)
{
    double maxReward = 0.0;
    for ( std::vector<State*>::iterator i=states.begin(); i!=states.end(); ++i ) {
        for ( std::vector<ReachAction*>::iterator j = (*i)->reachActions.begin(); j != (*i)->reachActions.end(); ++j ) {
            if ( (*j)->predictReward(p) > maxReward )
                maxReward = (*j)->reward();
            printf("Reach action: %p - predicted reward %f\n",*j,(*j)->reward());
        }
    }
    doRL();
    return maxReward;
}

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
    return nearestState;
}

bool Learner::deleteState( const State* s )
{
    /*for (std::vector<State*>::iterator i=states.begin(); i!=states.end(); ++i) {
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
    delete s;*/
}

bool Learner::isStable()
{
    // check if the robot is still moving
    yarp::os::Bottle query,reply;
    query.addVocab(yarp::os::Vocab::encode("stpd"));
    query.addDouble(50); // acceleration threshold
    query.addDouble(1.0); // velocity threshold
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

void  Learner::doRL()
{
    mutex.wait();
    printf("\nDOING RL...\n");
    
    double maxDelta;
    do
    {
        maxDelta=0;
        //printf("***************************************************\n");
        for (std::vector<State*>::iterator i = states.begin(); i!=states.end(); ++i)
        {
            //printf("State: %p\n",*i);
            double delta = (*i)->computeValue();
            if ( delta>maxDelta ) maxDelta = delta;
        } 
    } while ( maxDelta > 0.01 );
    mutex.post();
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
                printf("\t(%p - %f, %d)", (*k)->s_prime, (*k)->prob, (*k)->num);
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

void Learner::loadFile( std::string& filename )
{
    std::vector<TransitionAction*> transitionActions;
    std::vector<ReachAction*> reachActions;
    
	std::ifstream in_file(filename.c_str());
    
	if (!in_file) {
		std::cerr << "No File Found: " << filename << std::endl;
		return;
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
            while (line_reader.good()) {
                float number;
                line_reader >> number;
                pose.push_back(number);
            }
            if (!pose.size())
                continue;
            Point_d q(pose.size(),pose.begin(),pose.end());
            appendState(q);
        }
        else if ( doWhat == 1 ) // connect the states with transition actions
        {
            int a,b,num;
            double reward,value;
            std::istringstream line_reader(line);
            line_reader >> a;
            line_reader >> b;
            line_reader >> num;
            line_reader >> reward;
            line_reader >> value;
            transitionActions.push_back( appendTransitionAction(states.at(a),states.at(b),value,reward,num) );
        }
        else if ( doWhat == 2 ) // initialize the state transition beliefs/probabilities
        {
            int action, s_prime, num;
            double prob;
            std::istringstream line_reader(line);
            line_reader >> action;
            line_reader >> s_prime;
            line_reader >> num;
            line_reader >> prob;
            transitionActions.at(action)->appendSPrime(states.at(s_prime), num, prob);
        }
        else if ( doWhat == 3 ) // append reach actions to states
        {
            int stateIdx,num;
            std::string m;
            double reward,value;
            std::istringstream line_reader(line);
            line_reader >> stateIdx;
            line_reader >> m;
            line_reader >> num;
            line_reader >> reward;
            line_reader >> value;
            reachActions.push_back( appendReachAction(states.at(stateIdx),m.c_str(),value,reward,num) );
        }
        else if ( doWhat == 4 ) // append reward beliefs to reach actions
        {
            int reachIdx;
            double x,y,z,reward;
            std::istringstream line_reader(line);
            line_reader >> reachIdx;
            line_reader >> x;
            line_reader >> y;
            line_reader >> z;
            line_reader >> reward;
            Point_3 p(x,y,z);
            reachActions.at(reachIdx)->appendToHistory(p,reward);
        }
        
    }
}

void Learner::writeFile( std::string& filename )
{
	std::ofstream out_file(filename.c_str());
	//out_file.precision(35);
   
    int stateCount = 0;
    out_file << "STATES" << std::endl;
    for ( std::vector<State*>::iterator i=states.begin(); i!=states.end(); ++i ) {
        (*i)->tempIdx = stateCount;
        for ( Point_d::Cartesian_const_iterator j = (*i)->cartesian_begin(); j != (*i)->cartesian_end(); ++j ) {
            out_file << *j << " ";
        }
        out_file << std::endl;
        stateCount++;
    }
    out_file << std::endl;
    
    printf("\n\n********************\n");
    int transitionCount = 0;
    out_file << "TRANSITION_ACTIONS" << std::endl;
    for ( std::vector<State*>::iterator i=states.begin(); i!=states.end(); ++i ) {
        //printf("state: %p\n",*i);
        for ( std::vector<TransitionAction*>::iterator j = (*i)->transitionActions.begin(); j != (*i)->transitionActions.end(); ++j ) {
            //printf(" transition: %p\n",*j);
            (*j)->tempIdx = transitionCount;
            out_file << (*j)->parentState->tempIdx << " "
                     << (*j)->destination_state->tempIdx << " "
                     << (*j)->timesTried() << " "
                     << (*j)->reward() << " "
                     << (*j)->value()
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
                out_file << (*j)->tempIdx << " "
                         << (*k)->s_prime->tempIdx << " "
                         << (*k)->num << " "
                << (*k)->prob << std::endl;
            }
        }
    }
    out_file << std::endl;
    
    int reachCount = 0;
    out_file << "REACH_ACTIONS" << std::endl;
    for ( std::vector<State*>::iterator i=states.begin(); i!=states.end(); ++i ) {
        for ( std::vector<ReachAction*>::iterator j = (*i)->reachActions.begin(); j != (*i)->reachActions.end(); ++j ) {
            (*j)->tempIdx = reachCount++;
            out_file << (*j)->parentState->tempIdx << " "
            << (*j)->marker << " "
            << (*j)->num << " "
            << (*j)->r << " "
            << (*j)->v
            << std::endl;
        }
    }
    out_file << std::endl;
    
    out_file << "REACH_BELIEFS" << std::endl;
    for ( std::vector<State*>::iterator i=states.begin(); i!=states.end(); ++i ) {
        for ( std::vector<ReachAction*>::iterator j = (*i)->reachActions.begin(); j != (*i)->reachActions.end(); ++j ) {
            for ( std::vector< std::pair<Point_3,double> >::iterator k=(*j)->history.begin(); k!=(*j)->history.end(); ++k ) {
                out_file << (*j)->tempIdx << " "
                << k->first.cartesian(0) << " "
                << k->first.cartesian(1) << " "
                << k->first.cartesian(2) << " "
                << k->second
                << std::endl;
            }
            
        }
    }
    out_file << std::endl;
}