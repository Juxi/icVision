#include "learner.h"

bool Learner::deleteState( const State* s )
{
    for (std::list<State*>::iterator i=states.begin(); i!=states.end(); ++i) {
        for (std::list<State::Action>::iterator j=(*i)->actions.begin(); j!=(*i)->actions.end(); ++j) {
            for (std::list< std::pair<const State*,double> >::iterator k=j->transition_belief.begin(); k!=j->transition_belief.end(); ++k) {
                if (k->first==s) {
                    std::list< std::pair<const State*,double> >::iterator K=k;
                    j->transition_belief.erase(k,++K);
                }
            }
            if (j->destination_state == s) {
                std::list<State::Action>::iterator J=j;
                (*i)->actions.erase(j,++J);
            }
        }
        if (*i==s) {
            std::list<State*>::iterator I=i;
            states.erase(i,++I);
        }
    }
    delete s;
}

void Learner::print()
{
    int stateCount=0;
    for (std::list<State*>::iterator i=states.begin(); i!=states.end(); ++i)
    {
        printf("State %d: %p\n",stateCount++,*i);
        int actionCount=0;
        for (std::list<State::Action>::iterator j=(*i)->actions.begin(); j!=(*i)->actions.end(); ++j)
        {
            printf("  Action %d: ",actionCount++);
            for (std::list< std::pair<const State*,double> >::iterator k=j->transition_belief.begin(); k!=j->transition_belief.end(); ++k)
            {
                //printf("%f ", j->transition_belief.at(k).second);
                printf("(%p %f) ", k->first, k->second);
            }
            printf("\n");
        }
    }
}