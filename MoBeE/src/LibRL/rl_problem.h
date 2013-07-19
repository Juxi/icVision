/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RL
 *	@{
 */

#ifndef RL_PROB_H_
#define RL_PROB_H_

#include "util.h"
#include "learner.h"

class RL_Problem
{
public:
    RL_Problem( Point_3 p ) : target(p), numActions(0), interest(0.0) {}
    ~RL_Problem(){}
    Point_3 target;
    int numActions;
    double interest;
};

class RL_Problem_Set : public std::vector< RL_Problem* >
{
public:
    RL_Problem_Set( Learner* lrnr ) : learner(lrnr) {}
    ~RL_Problem_Set(){}
    
    void sampleInit( double xMin, double xMax, double yMin, double yMax, double zMin, double zMax, double delta );
    void evaluateInterest();
    
    RL_Problem* mostInteresting();
    RL_Problem* leastTriedProblem();
    int leastTried();
    
private:
    
    //std::vector< RL_Problem* > leastTriedProblems();
    
    Learner* learner;
    //std::vector< RL_Problem* > problems;
    
    
};

/*
static struct ProblemDst{
RL_Problem* p;
bool operator() (RL_Problem* i,RL_Problem* j) { return (p->first - i->first).squared_length() < (p->first - j->first).squared_length(); }
ProblemDst(RL_Problem* prob):p(prob){}
};
 
struct EffortDst{
    bool operator() (RL_Problem* i,RL_Problem* j) { return i->numActions < j->numActions; }
};
*/

#endif
/** @} */
