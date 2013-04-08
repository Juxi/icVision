#include <unistd.h>
#include <time.h>
#include "learner.h"

std::vector<Point_d> gridSample( int dim, int num, double scaling )
{
    // Sample num grid points from dim dimensional space in the cube between min and max
    std::vector<Point_d> points;
    points.reserve(num);
    
    
    CGAL::points_on_cube_grid_d( dim, scaling*0.5, (std::size_t)num, std::back_inserter(points), Creator_d(dim));
    
    std::cout << "Using Samples: \n" <<std::endl;
    
    // shift the sample set to the interval [0,1]
    std::vector<double> v;
    for (int i=0; i<dim; i++) v.push_back(0.5);
    Vector_d shift(dim,v.begin(),v.end());
    for ( std::vector<Point_d>::iterator i=points.begin(); i!=points.end(); ++i ){
        *i += shift;
        std::cout << *i << std::endl;
    }
    
    return points;
}

Learner::State::Action* takeRandomAction(Learner& learner)
{
    Learner::State* s = learner.getDiscreteState();
    printf("CURRENT STATE: %p\n",s);
    
    //int transitionIdx,reachIdx;
    if ( rand()%2 )
    {
        // Try a random state transition
        //int idx = rand() % s->transitionActions.size();
        //printf("trying action %d\n",idx);
 
        std::list<Learner::State::TransitionAction*>::iterator a = s->transitionActions.begin();
        Learner::State::TransitionAction* leastTriedAction = *s->transitionActions.begin();
        for ( a = s->transitionActions.begin(); a != s->transitionActions.end(); ++a ) //a++;
        {
            std::pair<const Learner::State*,double> belief = (*a)->belief();
            printf("  action: %p, %d tries, destination: %p it leads to state: %p with prob. %f\n",*a,(*a)->isTried(),(*a)->destination(),belief.first,belief.second);
            if ( (*a)->isTried() < leastTriedAction->isTried() )
                leastTriedAction = *a;
        } 
   
        printf("RUNNING LEAST TRIED STATE TRANSITION: %p\n",leastTriedAction);
        leastTriedAction->start();
        
        return leastTriedAction;
    }
    else
    {   // Try a random reach
        //printf("Reach Actions: %d\n", s->reachActions.size());
        
        // sample near p
        Point p(-0.3,0.1,0.0);
        //Vector noise(0.1*(double)rand()/RAND_MAX-0.05,
        //             0.1*(double)rand()/RAND_MAX-0.05,
        //             0);
        
        printf("RUNNING A REACH\n");
        if ( s->reachActions.size() > 0 ) {
            std::list<Learner::State::ReachAction*>::iterator a = s->reachActions.begin();
            Vector noise((double)rand()/RAND_MAX,(double)rand()/RAND_MAX,0.0);
            noise = noise/noise.squared_length();
            
            //(*a)->runReach((*a)->easyReach());
            (*a)->runReach( p /*+ 0.1*noise*/ );
            return *a;
        }
    }
}

int main(int argc, char *argv[])
{
    // prepare the random number generator
    srand(time(0));
    
    // instantiate reinforcement learner
    Learner learner(16,"icubSim","right_arm",true);
    
    std::vector<Point_d> samples = gridSample(4,33,0.5);
    for ( std::vector<Point_d>::iterator i = samples.begin(); i!=samples.end(); ++i )
        learner.appendState(*i);
    
    int count = 0;
    while ( count < 500)
    {
        printf("\nMain: COUNT = %d\n",count);
        
        Learner::State::Action* a = takeRandomAction(learner);
        learner.doRL();
        
        //printf("Waiting for Action to complete.\n");
        //printf("Action returned reward: %f\n",a->checkReward());
        //while ( a->isRunning() ) {yarp::os::Time::delay(1.0);}
        count++;
    }
 
    printf("Right Arm Learner:\n");
    learner.print(true);
    
    
    printf("All finished\n");
    return 1;
}
