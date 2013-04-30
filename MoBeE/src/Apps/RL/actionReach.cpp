#include "actionReach.h"
#include "learner.h"

/*Point_3 ReachAction::easyReach()
{
    Point_3 p;
    Vector_3 norm,noise((double)rand()/RAND_MAX,(double)rand()/RAND_MAX,(double)rand()/RAND_MAX);
    parentLearner->getMarkerState(marker, p, norm);
    norm = norm/sqrt(norm.squared_length());
    return p + 0.15*norm ;//+ 0.1*noise;
}*/


void ReachAction::predictReward( Point_3 p )
{
    r = 0.0;
    if ( history.size()>0 ) {
        for ( std::vector< std::pair<Point_3,double> >::iterator i = history.begin(); i != history.end(); ++i )
            r += 1.0/((p-i->first).squared_length()+1) * i->second;
        r /= history.size();
    }
}

bool ReachAction::threadInit()
{
    Action::threadInit();
    
    // visualize the target point in the world model
    //mobeeObjectName = parentLearner->mkSphere(reachTarget.x(), reachTarget.y(), reachTarget.z(), 0.02);
    
    parentLearner->setAttractor(*parentLearner->getDiscreteState());
    yarp::os::Time::delay(1);
    
    // gains for the control signal
    forceGain = 5000.0;
    torqueGain = 5000.0;
    
    //discreteTime = 0;
    
    // start the reach and let it begin to move the robot
    //Point_3 p;
    //Vector_3 n;
    //parentLearner->getMarkerState(marker,p,n);
    if ( sendForceCommand() )
        yarp::os::Time::delay(1);
    
    return 1;
}

void ReachAction::threadRelease()
{
    num++;
    history.push_back( std::pair<Point_3,double>(reachTarget,r) );
    //parentLearner->rmGeom(mobeeObjectName);
    
    relax();
    //parentLearner->stopForcing();
    //if ( !waitForSteady() )
    //    printf("RELAX TIMED OUT!!!! :-0\n");
    
    Action::threadRelease();
}

bool ReachAction::sendForceCommand()
{
    Vector_3 force(CGAL::NULL_VECTOR);  // force command
    Vector_3 torque(CGAL::NULL_VECTOR); // torque command
    Vector_3 n,err;   // error vector to target
    Point_3  p;     // reach target on hand
    double  nMag,
            errMag;
    
    parentLearner->getMarkerState(marker,p,n);
    
    // error vector from marker to target
    err = reachTarget - p;
    errMag = sqrt(err.squared_length());
    nMag = sqrt(n.squared_length());
    //printf("raw n: %f,%f,%f\n",n.x(),n.y(),n.z());
    
    //yarp::os::ConstString tName;
    //tName = parentLearner->mkSphere(t.x(), t.y(), t.z(), 0.03);
    
    Vector_3 eDir = err/errMag;
    Vector_3 nDir = n/nMag;
    
        //printf("eDir %f, %f, %f\n",eDir.x(),eDir.y(),eDir.z());
        //printf("nDir %f, %f, %f\n",nDir.x(),nDir.y(),nDir.z());
    
        //printf("eDir.squaredlength() %f\n",eDir.squared_length());
        //printf("nDir.squaredlength() %f\n",nDir.squared_length());
    
    double angularErrMag = (-nDir*eDir+1)/2;
    Vector_3 torqueDirection = CGAL::cross_product(nDir,eDir);
    torque = torqueGain * errMag * angularErrMag * torqueDirection;
    force = forceGain * err;
    
    printf("err = %f \n", errMag );
    printf("angular err = %f \n", angularErrMag );
    printf("\n");
    
    printf("|force| = %f \n", sqrt(force.squared_length()));
    printf("|torque| = %f \n", sqrt(torque.squared_length()) );
    
    printf("\n");
    printf("\n");
    
    if (errMag > 0.1) {
        parentLearner->setOpSpace(marker, force, torque);
        return true;
    }
    else return false;
}

/*double ReachAction::getErr()
{
    Point_3 p;
    Vector_3 n;
    parentLearner->getMarkerState(marker,p,n);
    
    // define where we want 'reachTarget' relative to the hand
    double nMag = sqrt(n.squared_length());
    Point_3 t = p + 0.02*n/nMag;
    //printf("raw n: %f,%f,%f\n",n.x(),n.y(),n.z());
    
    // error to target
    return sqrt((reachTarget - t).squared_length());
}*/

void ReachAction::run()
{
    // get the residual error to target
    //Point_3 p;
    //Vector_3 n;
    //parentLearner->getMarkerState(marker,p,n);
    //double err = sendForceCommand(p,n);
    //printf("REACH |err| = %f \n", err);
    
    if ( !sendForceCommand() ) {
        r = 1.0;
        printf("\nFOUND REACH TARGET!!! Got reward: %f\n",r);
        parentLearner->setAttractor(parentLearner->getRealState());
        parentLearner->stopForcing();
        stop();
    }
    else if (parentLearner->isStable()) {
        r = -1.0;
        printf("\n STEADY STATE REACHED!!! Got reward: %f\n",r);
        stop();
    }
    else if ( yarp::os::Time::now() - timeStarted > timeout ) {
        r = -1.0;
        printf("REACH TIMED OUT!!! Got reward: %f\n",r);
        stop();
    }

    
}