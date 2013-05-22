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
public:
        
    Learner( int dim, const char* _robotName, const char* _partName, bool connect = true );
    ~Learner();
    
    // construct states and actions
    void                appendGrid( int dim, int num, double scaling );
    void                connectNearestNeighbors(int);
    void                appendReaches();
    State*              appendState( Point_d& p, int numVisits );
    TransitionAction*   appendTransitionAction( State* a, State* b, double val=0.0, double rew=0.0, int num=0 );
    ReachAction*        appendReachAction( State* s, yarp::os::ConstString m, double val=0.0, double rew=0.0, int num=0 );
    //bool              deleteState( const State* );
    
    // query the learner
    int     getDimension() { return dimension; }
    int     getUnvisitedStates();
    int     getUntriedActions();
    Point_d getRealState();
    State*  getDiscreteState();
    Action* leastTriedTransition();
    State*  leastVisitedState();
    void    print(bool printall = false);
    
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
    
    // (re)initialize things
    void initializeReward(double);
    
    // bits of learning algorithms
    void valueIteration();
    //void tryReaches(Point_3 p);
    //void reachTargets(std::vector<Point_3>);
    //void workOnTarget(Point_3 p, bool endEarly = false);
    //double generateValueFunction(Point_3);
    
    // these may be a const correctness problem
    std::vector< State* > getStates() { return states; } 
    std::vector< yarp::os::ConstString > getMarkers() { return markers; }
    
    // file I/O
    void writeStateFile();
    bool loadStateFile( std::string& fileName );
    void writeHistoryFile(int,int,int,int,double,double);
    //void writeNumberedFile( std::string outFileBaseName = "outFile", int num = 0 );

    
    /***********************************************************************
     ***  these should not really be public but the actions need access  ***
     ***********************************************************************/
    MoBeE_Interface mobee;
    bool checkMutex() { return mutex.check(); }
    void postMutex() { mutex.post(); }
    
private:
    
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
    
    yarp::os::Semaphore mutex;
};
#endif
/** @} */
