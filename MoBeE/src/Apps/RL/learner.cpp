#include "learner.h"

void Learner::print()
{
    int stateCount=0;
    for (std::vector<State*>::iterator i=states.begin(); i!=states.end(); ++i)
    {
        printf("State %d: %p\n",stateCount++,&(*i));
        int actionCount=0;
        for (std::vector<State::Action>::iterator j=(*i)->actions.begin(); j!=(*i)->actions.end(); ++j)
        {
            printf("  Action %d: ",actionCount++);
            for (std::vector< std::pair<const State*,double> >::iterator k=j->transition_belief.begin(); k!=j->transition_belief.end(); ++k)
            {
                //printf("%f ", j->transition_belief.at(k).second);
                printf("(%p %f) ", k->first, k->second);
            }
            printf("\n");
        }
    }
}