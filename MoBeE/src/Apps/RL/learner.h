/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RL
 *	@{
 */

#ifndef LEARNER_H_
#define LEARNER_H_

#include <yarp/os/all.h>
#include <CGAL/Cartesian_d.h>
#include <list>

typedef CGAL::Cartesian_d<double>::Point_d Point_d;
typedef CGAL::Cartesian_d<double>::Vector_d Vector_d;

class Learner : public yarp::os::RateThread
{
public:
    class State : public Point_d
    {
    public:
        class Action
        {  
        public:
            Action() : num(0), val(0.0) {}
            ~Action(){}
            virtual double run(){ return 0; }
        protected:
            int num;    // number of times this action has been tried
            double val; // Q value of this action
        };
        
        class TransitionAction : public Action
        {
        public:
            TransitionAction( State* _destination_state, Learner* parent ) : destination_state(_destination_state) {
                for (std::list<State*>::iterator i=parent->states.begin(); i!=parent->states.end(); ++i ) {
                    //if (*i==destination_state) transition_belief.push_back(S_Prime(*i,1.0,1));
                    //else
                        transition_belief.push_back(S_Prime(*i,0.0,0));
                }
                //printf("New Action destination: %p\n", destination_state);
            }
            ~TransitionAction(){}
        private:
            const State* destination_state;
            struct S_Prime
            {
                const State* s_prime;
                double prob;
                int num;
                S_Prime(State* s, double p, int n) : s_prime(s), prob(p), num(n){}
            };
            std::list< S_Prime > transition_belief;
        friend class State;
        friend class Learner;
        };
        
        class ReachAction : public Action
        {
            
        };
        
        State(Point_d q, Learner* parent) : Point_d( q.dimension(), q.cartesian_begin(), q.cartesian_end()) {
            parent->states.push_back(this);
            for (std::list<State*>::iterator i=parent->states.begin(); i!=parent->states.end(); ++i ) {
                // add state transition probabilities for all state::actions to lead to this state
                for (std::list<State::TransitionAction>::iterator j=(*i)->transitionActions.begin(); j!=(*i)->transitionActions.end(); ++j )
                    if (*i!=this) j->transition_belief.push_back(TransitionAction::S_Prime(this,0.0,0));
                // append actions 'go to other state' (from here)
                if (*i!=this) transitionActions.push_back(TransitionAction(*i,parent));
                // append actions 'go to this state' (from elsewhere)
                (*i)->transitionActions.push_back(TransitionAction(this,parent));
            }
        }
        ~State(){}
    private:
        std::list<TransitionAction> transitionActions;
        ReachAction reachAction;
        friend class Learner;
    };
    
    Learner( const char* _robotName, const char* _partName, int rate );
    ~Learner(){}
    
    inline void appendState(Point_d& q) { new State(q,this); }
    //inline std::vector<const State*> getStates() const { return std::vector<const State*>(states.begin(), states.end()); }
    //inline int getNumStates() { return states.size(); }
    
    bool randomReach(yarp::os::ConstString);
    bool takeRandomAction();
    
    
    void print(bool printall = false);
    
private:
    
    double updateTransitionBelief( State::TransitionAction*, State* );
    
    bool deleteState( const State* );
    
    virtual bool threadInit();
	virtual void afterStart(bool s);
    virtual void run();
    virtual void threadRelease();
    
    yarp::os::Network network;
    yarp::os::BufferedPort<yarp::os::Bottle> statePort,commandPort;
    yarp::os::RpcClient markerPortClient,worldPortClient;
    std::list<State*> states;
    
    State* currentState;
    State::TransitionAction* currentAction;
    
    yarp::os::Semaphore mutex;
    
};
#endif
/** @} */
