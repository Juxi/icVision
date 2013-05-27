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
#include "actionReach.h"
#include "actionTransition.h"
#include "mobee_interface.h"

class Learner
{
    friend class Action;
    friend class TransitionAction;
    friend class ReachAction;
    
public:
    
    // TODO: protect most of this stuff with the mutex
        
    Learner( int dim, const char* _robotName, const char* _partName, bool connect = true );
    ~Learner();
    
    // set file names
    void setStateFileName(const char* s) { stateFileName = std::string(s); }
    void setHistoryFileName(const char* s) { historyFileName = std::string(s); }
    
    // construct states and actions
    void                appendGrid( int dim, int num, double scaling );
    void                connectNearestNeighbors(int);
    void                appendReaches();
    State*              appendState( Point_d& p, int numVisits );
    TransitionAction*   appendTransitionAction( State* a, State* b, double val=0.0, double rew=0.0, int num=0 );
    ReachAction*        appendReachAction( State* s, yarp::os::ConstString m, double val=0.0, double rew=0.0, int num=0 );
    //bool              deleteState( const State* );
    
    // query the learner
    std::string getName() { return name; }
    int         getDimension() { return dimension; }
    int         getUnvisitedStates();
    int         getUntriedActions();
    Point_d     getRealState();
    State*      getDiscreteState();
    Action*     leastTriedTransition();
    Action*     leastTriedReach();
    State*      leastVisitedState();
    void        print(bool printall = false);
    
    // turn model learning on and off
    void    setModelLearning(bool b) { modelUpdate = b; }
    bool    isLearningModel() { return modelUpdate; }
    
    // access to learning params
    void    setModelInterestingness(double i) { modelInterest = i; }
    double  modelInterestingness() { return modelInterest; }
    void    setRlPrecision( double p ) { rlPrecision = p; }
    double  getRlPrecision() { return rlPrecision; }
    void    setDiscountFactor( double f ) { discountFactor = f; }
    double  getDiscountFactor() { return discountFactor; }
    int     getStateTransitionInit() { return stateTransitionInit; }
    
    // these may be a const correctness problem or a threading problem
    std::vector< State* > getStates() { return states; } 
    std::vector< yarp::os::ConstString > getMarkers() { return markers; }
    
    // config from file
    bool loadStateFile( std::string& fileName );
    
    // (re)initialize things
    void initializeTransitionReward(double);
    void initializeReachReward(double);
    
    void valueIteration();
    
    void writeStateFile();
    
    MoBeE_Interface mobee;
    
private:

    
    
    bool checkMutex() { return mutex.check(); }
    void postMutex() { mutex.post(); }
    
    // output files
    void writeHistoryFile(int,int,int,int,double,double);
    
    // bits of learning algorithms
    
    //void tryReaches(Point_3 p);
    //void reachTargets(std::vector<Point_3>);
    //void workOnTarget(Point_3 p, bool endEarly = false);
    //double generateValueFunction(Point_3);
    
    Point_d             redimension(Point_d& p);
    
    State*              getState(int);
    TransitionAction*   getTransitionAction(int);
    ReachAction*        getReachAction(int);
    
    bool checkStateIndex(State*);
    bool checkActionIndex(Action*);
    
    int dimension;
    std::vector<State*> states;
    std::vector< yarp::os::ConstString > markers;
   
    // learning params
    bool    modelUpdate;
    double  discountFactor;
    double  modelInterest;
    double  rlPrecision;
    int     stateTransitionInit;
    
    // counters for indices
    int nextStateIdx;
    int nextActionIdx;
    
    // names of data (output) files
    std::string historyFileName;
    std::string stateFileName;
    std::string name;
    
    yarp::os::Semaphore mutex;
};
#endif
/** @} */
