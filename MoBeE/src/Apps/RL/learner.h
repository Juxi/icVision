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
    
    // turn model learning on and off
    void    setModelLearning(bool b) { modelUpdate = b; }
    bool    isLearningModel() { return modelUpdate; }
    
    // learning params
    void    setModelInterestingness(double i) { modelInterest = i; }
    double  modelInterestingness() { return modelInterest; }
    void    setRlPrecision( double p ) { rlPrecision = p; }
    double  getRlPrecision() { return rlPrecision; }
    void    setDiscountFactor( double f ) { discountFactor = f; }
    double  getDiscountFactor() { return discountFactor; }
    int     getStateTransitionInit() { return stateTransitionInit; }
    
    
    // run learning algorithms
    void learnModel_IM();
    void learnModel_LT();
    void learnModel_Rand();
    //void learnStateTransitions( int timesToTryEach = 1 );
    void valueIteration();
    //void tryReaches(Point_3 p);
    //void reachTargets(std::vector<Point_3>);
    //void workOnTarget(Point_3 p, bool endEarly = false);
    //double generateValueFunction(Point_3);
    
    // query the learner
    Point_d getRealState();
    State*  getDiscreteState();
    Action* leastTriedTransition();
    State*  leastVisitedState();
    // these may be a const correctness problem
    std::vector< State* > getStates() { return states; } 
    std::vector< yarp::os::ConstString > getMarkers() { return markers; }
    
    
    // file I/O
    bool loadStateFile( std::string& fileName );
    void writeStateFile();
    void writeHistoryFile(int,int,int,int,double,double);
    //void writeNumberedFile( std::string outFileBaseName = "outFile", int num = 0 );
    

    // construct states and actions
    State*              appendState( Point_d& p, int numVisits );
    TransitionAction*   appendTransitionAction( State* a, State* b, double val=0.0, double rew=0.0, int num=0 );
    ReachAction*        appendReachAction( State* s, yarp::os::ConstString m, double val=0.0, double rew=0.0, int num=0 );
    //bool deleteState( const State* );
    
    void initializeReward(double);
    
    int statesUnvisited();
    int actionsUntried();
    
    // communicate with MoBeE model
    void getMarkerState( yarp::os::ConstString& markerName, Point_3& p, Vector_3& n);
    void setAttractor( Point_d q );
    void setOpSpace( yarp::os::ConstString name, Vector_3, Vector_3 );
    void stopForcing();
    bool isStable();
    yarp::os::ConstString mkSphere( double x, double y, double z, double r );
    //yarp::os::ConstString mkCyl( double r, double len, );
    void rmGeom( yarp::os::ConstString& );
    void defTarget( yarp::os::ConstString& );
    void defObstacle( yarp::os::ConstString& );
    
    bool checkMutex() { return mutex.check(); }
    void postMutex() { mutex.post(); }
    
    //std::vector< std::pair<
    //inline std::vector<const State*> getStates() const { return std::vector<const State*>(states.begin(), states.end()); }
    //inline int getNumStates() { return states.size(); }
    
    //bool randomReach(yarp::os::ConstString);
    
    void print(bool printall = false);
    
private:
    
    Point_d redimension(Point_d& p);
    
    State* getState(int);
    TransitionAction* getTransitionAction(int);
    ReachAction* getReachAction(int);
    
    bool checkStateIndex(State*);
    bool checkActionIndex(Action*);
    
    //void resetRewardMatrix(std::vector<Action*>);
    
    //State*  getState( int n );
    //bool    isInList( State* );
    
    int dimension;
    std::vector<State*> states;
    //State* currentState;
    //State::TransitionAction* currentAction;
    yarp::os::Semaphore mutex;
    
    yarp::os::Network network;
    yarp::os::BufferedPort<yarp::os::Bottle>    statePort,
                                                commandPort;
    yarp::os::RpcClient controllerClient,
                        worldClient;
    
    std::vector< yarp::os::ConstString > markers;
    
    bool    modelUpdate;
    double  discountFactor;
    double  modelInterest;
    double  rlPrecision;
    
    int stateTransitionInit;
    
    int nextStateIdx;
    int nextActionIdx;
    
    std::string historyFileName;
    std::string stateFileName;
};
#endif
/** @} */
