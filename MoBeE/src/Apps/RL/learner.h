/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RL
 *	@{
 */

#ifndef LEARNER_H_
#define LEARNER_H_

#include "state.h"

class Learner
{
public:
        
    Learner( int dim, const char* _robotName, const char* _partName, bool connect = true );
    ~Learner();
    
    Point_d getRealState();
    State* getDiscreteState();

    Point_d redimension(Point_d& p);
    
    inline void appendState(State* state) { states.push_back(state); }
    bool deleteState( const State* );
    
    void getMarkerState( yarp::os::ConstString& markerName, Point_3& p, Vector_3& n);
    void setAttractor( Point_d q );
    void setOpSpace( yarp::os::ConstString name, Vector_3, Vector_3 );
    void stopForcing();
    bool isStable();
    yarp::os::ConstString mkSphere( double x, double y, double z, double r );
    void rmGeom( yarp::os::ConstString& );
    
    
    
    bool checkMutex() { return mutex.check(); }
    void postMutex() { mutex.post(); }
    void doRL();
    
    //std::list< std::pair<
    //inline std::vector<const State*> getStates() const { return std::vector<const State*>(states.begin(), states.end()); }
    //inline int getNumStates() { return states.size(); }
    
    //bool randomReach(yarp::os::ConstString);
    
    //void print(bool printall = false);
    
private:
    
    int dimension;
    std::list<State*> states;
    //State* currentState;
    //State::TransitionAction* currentAction;
    yarp::os::Semaphore mutex;
    
    yarp::os::Network network;
    yarp::os::BufferedPort<yarp::os::Bottle>    statePort,
                                                commandPort;
    yarp::os::RpcClient controllerClient,
                        worldClient;
    
    std::list< yarp::os::ConstString > markers;
    
};
#endif
/** @} */
