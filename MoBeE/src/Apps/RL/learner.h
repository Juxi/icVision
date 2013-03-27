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
#include <CGAL/Cartesian.h>
#include <CGAL/Cartesian_d.h>
#include <CGAL/Point_3.h>
#include <CGAL/Vector_3.h>
#include <list>

typedef CGAL::Cartesian_d<double>::Point_d Point_d;
typedef CGAL::Cartesian_d<double>::Vector_d Vector_d;
typedef CGAL::Point_3< CGAL::Cartesian<double> > Point;
typedef CGAL::Vector_3< CGAL::Cartesian<double> > Vector;


class Learner
{
public:
    class State : public Point_d
    {
    public:
        class Action : public yarp::os::RateThread  
        {
        /* This abstract class provides a generic action that runs in a thread and is protected by a semaphor
           such that only one action can be running (per learner) at a time */
        public:
            Action( Learner* l, State* p, int rate ) : yarp::os::RateThread(rate), learner(l), parentState(p), timeStarted(0.0), num(0), val(0.0) {}
            ~Action(){}
        protected:
            Learner*        learner;
            State*          parentState;
            double          timeStarted;
            int             num;    // number of times this action has been tried
            double          val; // Q value of this action
            virtual bool    threadInit();
            virtual void    afterStart(bool s);
            virtual void    threadRelease();
        };
        
        class TransitionAction : public Action
        {
        /* These actions move the attractor in the MoBeE model. This corresponds to position control, and is used to implement deliberate
           motion planning through a roadmap graph (the nodes of which are "States") using reinforcement learning */
        public:
            TransitionAction( Learner* l, State* a, State* b, int rate=200 ) : Action(l,a,rate), destination_state(b), f(*b-*b) {
                for (std::list<State*>::iterator i=learner->states.begin(); i!=learner->states.end(); ++i ) {
                    transition_belief.push_back(S_Prime(*i,0.0,0));
                }
                //printf("New Action destination: %p\n", destination_state);
            }
            ~TransitionAction(){}
        private:
            struct S_Prime
            {
                const State* s_prime;
                double prob;
                int num;
                S_Prime(State* s, double p, int n) : s_prime(s), prob(p), num(n){}
            };
            std::list< S_Prime > transition_belief;
            Point_d current_real_state;
            const State* current_discrete_state;
            const State* destination_state;
            Vector_d f;
            
            void threadRelease();
            double updateTransitionBelief( const State* endState );
            void run();
        friend class State;
        friend class Learner;
        };
        
        class ReachAction : public Action
        {
        /* These actions reach for objects, and are responsible for rewarding the learner. In this way the learner learns which 
           states (roadmap nodes) are the good ones from which to reach for objects in the environment */
        public:
            ReachAction( Learner* l, yarp::os::ConstString m, State* p, int rate ) : Action(l,p,rate), marker(m), timeout(10.0), target(0,0,0) {}
            ~ReachAction(){}
        private:
            yarp::os::ConstString marker;
            double timeout;
            Point target;
            void run();
        friend class State;
        friend class Learner;
        };
        
        State(Point_d q, Learner* l) : Point_d( q.dimension(), q.cartesian_begin(), q.cartesian_end())
        {
            l->states.push_back(this);
            //if (parent->states.size()==1) parent->currentState = this;
            
            for (std::list<State*>::iterator i=l->states.begin(); i!=l->states.end(); ++i )
            {
                // add state transition probabilities for all state::actions to lead to this state
                for (std::list<TransitionAction*>::iterator j=(*i)->transitionActions.begin(); j!=(*i)->transitionActions.end(); ++j )
                    if (*i!=this) (*j)->transition_belief.push_back(TransitionAction::S_Prime(this,0.0,0));
                
                TransitionAction    *thereToHere = new TransitionAction(l,*i,this),
                                    *hereToThere = new TransitionAction(l,this,*i);
                
                // append actions 'go to other state' (from here)
                if (*i!=this) {
                    transitionActions.push_back(hereToThere);
                    //actions.push_back(hereToThere);
                }
                
                // append actions 'go to this state' (from elsewhere)
                if (*i!=this){
                    (*i)->transitionActions.push_back(thereToHere);
                    //(*i)->actions.push_back(thereToHere);
                }
            }
            
            for (std::list<yarp::os::ConstString>::iterator i = l->markers.begin(); i!= l->markers.end(); ++i )
            {
                reachActions.push_back(new ReachAction(l,*i,this,200));
            }
        }
        ~State(){}
        //std::list<Action*> actions;
        std::list<TransitionAction*> transitionActions; // Instead of keeping multiple lists here i could dynamic_cast...  dunno
        std::list<ReachAction*> reachActions;
    friend class Learner;
    };
    
    std::list<State*> states;
    //State* currentState;
    //State::TransitionAction* currentAction;
    yarp::os::Semaphore mutex;
    
    Learner( const char* _robotName, const char* _partName, bool connect = true );
    ~Learner();
    
    Point_d getRealState();
    State* getDiscreteState();

    inline void appendState(Point_d& q) { new State(q,this); }
    bool deleteState( const State* );
    //inline std::vector<const State*> getStates() const { return std::vector<const State*>(states.begin(), states.end()); }
    //inline int getNumStates() { return states.size(); }
    
    //bool randomReach(yarp::os::ConstString);
    
    void print(bool printall = false);
    
private:
    
    yarp::os::Network network;
    yarp::os::BufferedPort<yarp::os::Bottle>    statePort,
                                                commandPort;
    yarp::os::RpcClient controllerClient,
                        worldClient;
    
    std::list< yarp::os::ConstString > markers;
    
};
#endif
/** @} */
