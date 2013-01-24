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

class Learner : public yarp::os::RateThread
{
public:
    class State : public Point_d
    {
    public:
        class Action
        {  
        public:
            Action( State* _destination_state, Learner* parent ) : destination_state(_destination_state), num(1), val(0.0) {
                for (std::list<State*>::iterator i=parent->states.begin(); i!=parent->states.end(); ++i ) {
                    if (*i==destination_state) transition_belief.push_back(S_Prime(*i,1.0,1));
                    else transition_belief.push_back(S_Prime(*i,0.0,0));
                }
            }
            ~Action(){}
        private:
            const State* destination_state;
            struct S_Prime
            {
                const State* s_prime;
                double prob;
                int num;
                //Transition_Belief() : s_prime(NULL), prob(0.0), num(0){}
                //Transition_Belief(State* s) : s_prime(s), prob(0.0), num(0){}
                S_Prime(State* s, double p, int n) : s_prime(s), prob(p), num(n){}
            };
            std::list< S_Prime > transition_belief;
            int num;    // number of times this action has been tried
            double val; // Q value of this action
            
            friend class State;
            friend class Learner;
        };
        
        State(Point_d q, Learner* parent) : Point_d( q.dimension(), q.cartesian_begin(), q.cartesian_end()) {
            parent->states.push_back(this);
            for (std::list<State*>::iterator i=parent->states.begin(); i!=parent->states.end(); ++i ) {
                // add state transition probabilities for all state::actions to lead to this state
                for (std::list<State::Action>::iterator j=(*i)->actions.begin(); j!=(*i)->actions.end(); ++j )
                    if (*i!=this) j->transition_belief.push_back(Action::S_Prime(this,0.0,0));
                // append actions 'go to other state' (from here)
                if (*i!=this) actions.push_back(Action(*i,parent));
                // append actions 'go to this state' (from elsewhere)
                (*i)->actions.push_back(Action(this,parent));
            }
        }
        ~State(){}
    private:
        std::list<Action> actions;
        friend class Learner;
    };
    
    Learner( const char* _robotName, const char* _partName, int rate );
    ~Learner(){}
    
    inline void appendState(Point_d& q) { new State(q,this); }
    //inline std::vector<const State*> getStates() const { return std::vector<const State*>(states.begin(), states.end()); }
    //inline int getNumStates() { return states.size(); }
    
    bool takeRandomAction();
    
    
    void print();
    
private:
    
    double updateTransitionBelief( State::Action, State* );
    
    bool deleteState( const State* );
    
    virtual bool threadInit();
	virtual void afterStart(bool s);
    virtual void run();
    virtual void threadRelease();
    
    yarp::os::Network network;
    yarp::os::Port statePort,commandPort;
    std::list<State*> states;
    
    State* currentState;
    State::Action* currentAction;
    
};
#endif
/** @} */
