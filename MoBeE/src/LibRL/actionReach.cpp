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


double ReachAction::predictReward( Point_3 p )
{
    double r_predicted = 0.0;
    if ( history.size()>0 ) {
        for ( std::vector< HistoryItem >::iterator i = history.begin(); i != history.end(); ++i ) {
            double d = (i->target-i->result).squared_length();
            r_predicted += 1.0/((p-i->result).squared_length()+1) - d/((p-i->target).squared_length()+d);
        }
        r_predicted /= history.size();
    }
    //r = r_predicted;
    return r_predicted;
}

/*bool ReachAction::threadInit()
{
    Action::threadInit();
    
    // visualize the target point in the world model
    //mobeeObjectName = parentLearner->mkSphere(reachTarget.x(), reachTarget.y(), reachTarget.z(), 0.02);
    
    //parentLearner->setAttractor(*parentLearner->getDiscreteState());
    //yarp::os::Time::delay(1);
    
    // gains for the control signal
    forceGain = 5000.0;
    torqueGain = 5000.0;
    
    sendForceCommand();
        yarp::os::Time::delay(1);
    
    return 1;
}*/

void ReachAction::afterStart(bool s)
{
    Action::afterStart(s);
    
    if (s) {
        printf(" RUNNING REACH ACTION %p from state %p (%s)",this,parentState,parentState->getLearner()->getName().c_str());
        forceGain = 5000.0;
        torqueGain = 5000.0;
        sendForceCommand();
        yarp::os::Time::delay(0.2);
    }
}

/*void ReachAction::threadRelease()
{
    num++;
    //parentLearner->rmGeom(mobeeObjectName);
    
    //relax();
    
    Action::threadRelease();
}*/

void ReachAction::relax()
{
    num++;
    parentState->getLearner()->mobee.stopForcing(parentState->getLearner()->getDimension());
    waitForSteady();
}

Vector_3 ReachAction::sendForceCommand()
{
    Vector_3 force(CGAL::NULL_VECTOR);  // force command
    Vector_3 torque(CGAL::NULL_VECTOR); // torque command
    Vector_3 n,err;   // error vector to target
    double  nMag,
            errMag;
    
    parentState->getLearner()->mobee.getMarkerState(marker,markerPos,n);
    
    // error vector from marker to target
    err = reachTarget - markerPos;
    errMag = sqrt(err.squared_length());
    nMag = sqrt(n.squared_length());
    //printf("raw n: %f,%f,%f\n",n.x(),n.y(),n.z());
    
    //yarp::os::ConstString tName;
    //tName = parentLearner->mkSphere(t.x(), t.y(), t.z(), 0.03);
    
    Vector_3    eDir = err/errMag;
    Vector_3    nDir = n/nMag;
    Vector_3    torqueDirection = CGAL::cross_product(nDir,eDir);
    double      angularErrMag = (-nDir*eDir+1)/2;
    
    torque = torqueGain * errMag * angularErrMag * torqueDirection;
    force = forceGain * err;
    
    //printf("eDir %f, %f, %f\n",eDir.x(),eDir.y(),eDir.z());
    //printf("nDir %f, %f, %f\n",nDir.x(),nDir.y(),nDir.z());
    
    //printf("eDir.squaredlength() %f\n",eDir.squared_length());
    //printf("nDir.squaredlength() %f\n",nDir.squared_length());
    
    /*printf("--------------------------\n");
    printf("err = %f \n", errMag );
    printf("angular err = %f \n", angularErrMag );
    printf("\n");
    
    printf("|force| = %f \n", sqrt(force.squared_length()));
    printf("|torque| = %f \n", sqrt(torque.squared_length()) );
    
    printf("\n");
    printf("\n");*/

    
    Vector_3 zero(0,0,0);
    parentState->getLearner()->mobee.setOpSpace(marker, force, zero);
    
    return err;
}

void ReachAction::run()
{
    Vector_3 err = sendForceCommand();
    double errMag = 100*sqrt(err.squared_length()); // in cm i think
    
    if (parentState->getLearner()->mobee.isStable()) {
        Point_3 p;
        Vector_3 n;
        parentState->getLearner()->mobee.getMarkerState(marker,p,n);
        
        r = 1.0/(errMag +2);
        history.push_back(HistoryItem(reachTarget,p,r));
    
        printf("\n STEADY STATE REACHED!!! errMag: %f, reward: %f\n",errMag,r);
        relax();
        askToStop();
    }
    else if ( yarp::os::Time::now() - timeStarted > timeout ) {
        r = 0.0;
        printf("REACH TIMED OUT!!! errMag: %f, reward: %f\n",errMag,r);
        relax();
        askToStop();
    }
}