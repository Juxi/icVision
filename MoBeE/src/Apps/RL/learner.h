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
            Action( State* _destination_state, Learner* parent ) : destination_state(_destination_state) {
                for (std::list<State*>::iterator i=parent->states.begin(); i!=parent->states.end(); ++i ) {
                    if (*i==destination_state) transition_belief.push_back(std::pair< const State*, double >(*i,1.0));
                    else transition_belief.push_back(std::pair< const State*, double >(*i,0.0));
                }
            }
            ~Action(){}
        private:
            const State* destination_state;
            std::list< std::pair< const State*, double > > transition_belief;
            //std::vector< std::vector<int> > transition_history;
            friend class State;
            friend class Learner;
        };
        
        State(Point_d q, Learner* parent) : Point_d( q.dimension(), q.cartesian_begin(), q.cartesian_end()) {
            parent->states.push_back(this);
            for (std::list<State*>::iterator i=parent->states.begin(); i!=parent->states.end(); ++i ) {
                // add state transition probabilities for all state::actions to lead to this state
                for (std::list<State::Action>::iterator j=(*i)->actions.begin(); j!=(*i)->actions.end(); ++j )
                    if (*i!=this) j->transition_belief.push_back(std::pair<const State*, double>(this,0.0));
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
    
    Learner( int rate ) : yarp::os::RateThread(rate) {}
    ~Learner(){}
    
    inline void appendFullyConnectedState(Point_d& q) { new State(q,this); }
    inline std::vector<const State*> getStates() const { return std::vector<const State*>(states.begin(), states.end()); }
    inline int getNumStates() { return states.size(); }
    
    bool deleteState( const State* );
    void print();
    
private:
    
    virtual bool threadInit();
	virtual void afterStart(bool s);
    virtual void run();
    virtual void threadRelease();
    
    std::list<State*> states;
    
};
#endif
/** @} */
