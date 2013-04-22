#include "actionReach.h"
#include "learner.h"

Point_3 ReachAction::easyReach()
{
    Point_3 p;
    Vector_3 norm,noise((double)rand()/RAND_MAX,(double)rand()/RAND_MAX,(double)rand()/RAND_MAX);
    parentLearner->getMarkerState(marker, p, norm);
    norm = norm/sqrt(norm.squared_length());
    return p + 0.15*norm ;//+ 0.1*noise;
}

bool ReachAction::threadInit()
{
    Action::threadInit();
    
    parentLearner->setAttractor(*parentLearner->getDiscreteState());
    yarp::os::Time::delay(1);
    
    // visualize the target point in the world model
    mobeeObjectName = parentLearner->mkSphere(reachTarget.x(), reachTarget.y(), reachTarget.z(), 0.02);
    
    // gains for the control signal
    forceGain = 5000.0;
    torqueGain = 5000.0;
    
    //discreteTime = 0;
    
    // start the reach and let it begin to move the robot
    sendForceCommand(true);
    yarp::os::Time::delay(1);
    
    return 1;
}

void ReachAction::threadRelease()
{
    num++;
    parentLearner->rmGeom(mobeeObjectName);
    Action::threadRelease();
}

void ReachAction::sendForceCommand( bool withTorque )
{
    // get the state of the hand
    Point_3 p;
    Vector_3 n;
    parentLearner->getMarkerState(marker,p,n);
    
    // error vector from marker to target
    Vector_3 err = reachTarget - p;
    Vector_3 force(forceGain*err);
    
    // now for the torque component of the command
    Vector_3 torque(CGAL::NULL_VECTOR);
    if (withTorque)
    {
        // normalize stuff
        double errMag   = sqrt(err.squared_length());
        double nMag     = sqrt(n.squared_length());
        
        if (errMag > 0.001 && nMag > 0.001)
        {
            Vector_3 eDir = err/errMag;
            Vector_3 nDir = n/nMag;
            //printf("eDir %f, %f, %f\n",eDir.x(),eDir.y(),eDir.z());
            //printf("nDir %f, %f, %f\n",nDir.x(),nDir.y(),nDir.z());
            
            double torqueMagnitude = -n*eDir;
            Vector_3 torqueDirection = CGAL::cross_product(nDir,eDir);
            //printf("torqueMagnitude: %f\n",torqueMagnitude);
            //printf("torqueDir %f, %f, %f\n",direction.x(),direction.y(),direction.z());
            
            //err = err * torqueMagnitude;
            torque = (torqueMagnitude+1)/2*torqueDirection;
        }
    }
    parentLearner->setOpSpace(marker, force, torque);
}


void ReachAction::run()
{
    // get the residual error to target
    Point_3 p;
    Vector_3 n;
    parentLearner->getMarkerState(marker,p,n);
    double err = (reachTarget - p).squared_length();
    printf("REACH |err| = %f \n", err);
    
    if (parentLearner->isStable()) {
        if ( err < 0.003 )
        {
            // GIVE REWARD
            r = 1.0; 
            history.push_back( reachTarget );
      
            printf("\nFOUND REACH TARGET!!! Got reward: %f\n",r);
            //printf("\nFOUND REACH TARGET!!! Set Position pre-reach pose.\n");
            //setAttractor(learner->getRealState());
            
            //for (int i=0; i<10; i++ ) {
            //    printf("am i on the target???\n");
            //    yarp::os::Time::delay(1.0);
            //}
            
        } else
            printf("\n STEADY STATE REACHED, STOP FORCING\n");
        
        //printf("\nRETURNING TO NEAREST STATE.\n");
        //setAttractor(*learner->getDiscreteState());
        parentLearner->stopForcing();
        if ( !waitForSteady() ) printf("RELAX TIMED OUT!!!! :-0\n");
        stop();
    }
    else if ( yarp::os::Time::now() - timeStarted > timeout )
    {
        printf("REACH TIMED OUT, STOP FORCING :-(\n");
        //setAttractor(*parentState);
        parentLearner->stopForcing();
        if ( !waitForSteady() ) printf("RELAX TIMED OUT!!!! :-0\n");
        stop();
    }
    
    //setAttractor(learner->getRealState());
    sendForceCommand(true);
}