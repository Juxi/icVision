#include "rl_problem.h"

void RL_Problem_Set::sampleInit( double xMin, double xMax, double yMin, double yMax, double zMin, double zMax, double delta )
{
    // table height: z = -0.075
    for ( double x = xMin; x<=xMax; x+=delta ) {
        for ( double y = yMin; y<=yMax; y+=delta ) {
            for ( double z = zMin; z<=zMax; z+=delta ) {
                push_back( new RL_Problem(Point_3(x,y,z)) );
            }
        }
    }
    std::random_shuffle(begin(), end());
    printf("\nGenerated %d RL Problems (reach targets)\n",size());
}

void RL_Problem_Set::evaluateInterest()
{
    printf("Evaluating Interest:\n");
    for ( std::vector<RL_Problem*>::iterator i=begin(); i!=end(); ++i ) {
        (*i)->interest = learner->rewardIntegral((*i)->target)/((*i)->numActions+1);
        printf("\t(%f, %f, %f) interest: %f\n",(*i)->target.x(),(*i)->target.y(),(*i)->target.z(),(*i)->interest);
    }
}

RL_Problem* RL_Problem_Set::mostInteresting()
{
    if ( size() == 0 ) return NULL;
    
    std::vector<RL_Problem*> shuffle = *this;
    std::random_shuffle(shuffle.begin(), shuffle.end());
    RL_Problem* best = *shuffle.begin();
    for ( std::vector<RL_Problem*>::iterator i=shuffle.begin()+1; i!=shuffle.end(); ++i ) {
        if ( (*i)->interest > best->interest )
            best = *i;
    }
    return best;
}

int RL_Problem_Set::leastTried()
{
    if ( size() == 0 ) return 0;
    
    int fewestTries = (*begin())->numActions;
    for ( std::vector<RL_Problem*>::iterator i=begin()+1; i!=end(); ++i ) {
        if ( (*i)->numActions < fewestTries )
            fewestTries = (*i)->numActions;
    }
    return fewestTries;
}

RL_Problem* RL_Problem_Set::leastTriedProblem()
{
    if ( size() == 0 ) return 0;
    
    RL_Problem* ans = *begin();
    int fewestTries = ans->numActions;
    for ( std::vector<RL_Problem*>::iterator i=begin()+1; i!=end(); ++i ) {
        if ( (*i)->numActions < ans->numActions )
            ans = *i;
    }
    return ans;
}