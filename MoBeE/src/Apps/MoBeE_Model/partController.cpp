#include "partController.h"
#include <iostream.h>

PartController::PartController( const char* _robotName, const char* _partName, int r ) : yarp::os::RateThread(r), 
																							vel(NULL),
																							enc(NULL)
{
	printf( "Opening Remote Control Board: %s %s\n", _robotName, _partName );
	
	// make sure we can find a yarp server
	if ( !network.checkNetwork() ) {
		printf("Cannot find YARP network.\n");
	}
	
	// Make local and remote port names
	yarp::os::Property options;
	options.put( "robot", _robotName );
	options.put( "device", "remote_controlboard" );
	yarp::os::ConstString remotePort("/");
	remotePort += _robotName;
	remotePort += "/";
	remotePort += _partName;
	yarp::os::ConstString localPort = remotePort;
	localPort += "/control";
	options.put( "local", localPort.c_str() );
	options.put( "remote", remotePort.c_str() );
	
	//sleep(1);
	
	// connect to the iCub control board interface
	dd = new yarp::dev::PolyDriver(options);
	if (!dd->isValid()) {
		printf("Device not available. Failed to create PolyDriver.\n");
	}
	
	// interface to get motor encoder positions
	dd->view(enc);
	if ( !enc ) printf("IEncoder Error!\n");
	
	// interface to do velocity control
	dd->view(vel);
	if ( !vel ) printf("IVelocityControl Error!\n");
	
	// get number of DOFs and joint position limits, and initialize control parameters
	yarp::dev::IPositionControl *pos;
	yarp::dev::IControlLimits   *lim;
	dd->view(lim);
	dd->view(pos);
	if ( !lim ) printf("Joint Limits Error!\n");
	if ( !pos ) printf("IPositionControl Error!\n");
	if ( pos && lim )
    {
        // get number of controllable DOFs
		pos->getAxes(&numJoints);
     
        // initialize control vars
        q1 = new double[numJoints];
        q0 = new double[numJoints];
        v = new double[numJoints];
        e = new double[numJoints];
        
        x = new double[numJoints];
        min = new double[numJoints];
        max = new double[numJoints];
        nogo = new double[numJoints];
        
        w = new double[numJoints];
        k = new double[numJoints];
        c = new double[numJoints];
        
        fLim = new double[numJoints];
        fLimMax = new double[numJoints];
        
        fX = new double[numJoints];
        fXMax = new double[numJoints];
        
        fFld = new double[numJoints];
        kfFld = new double[numJoints];
        fFldMax = new double[numJoints];
        
        fCst = new double[numJoints];
        kfCst = new double[numJoints];
        fCstMax = new double[numJoints];
        
        fRPC = new double[numJoints];
        kfRPC = new double[numJoints];
        fRPCMax = new double[numJoints];
        
        a = new double[numJoints];
        ctrl = new double[numJoints];

        // set default values
		for ( int i = 0; i < numJoints; i++ )
        {
            double _min,_max;
			lim->getLimits( i, &_min, &_max );
            
            // TODO: should get most of this from config files
            q1[i]       = 0.0;
            q0[i]       = 0.0;
            v[i]        = 0.0;
            e[i]        = 0.0;
            
            x[i]        = 0.0;
            min[i]      = _min;
            max[i]      = _max;
            nogo[i]     = 10.0;
            
            w[i]        = 1.0;
            k[i]        = 50.0;
            c[i]        = 30.0;
            a[i]        = 0.0;
            ctrl[i]     = 0.0;
            
            fX[i]       = 0.0;
            fXMax[i]    = 800.0;
        
            fLim[i]     = 0.0;
            fLimMax[i]  = 1000.0;
            
            fCst[i]     = 0.0;
            kfCst[i]    = 100.0;
            fCstMax[i]  = 1600.0;
            
            fFld[i]     = 0.0;
            kfFld[i]    = 100.0;
            fFldMax[i]  = 1600.0;
            
            fRPC[i]     = 0.0;
            kfRPC[i]    = 100.0;
            fRPCMax[i]  = 1000.0;
            
			//printf("joint %d: min = %f max = %f\n",i,_min,_max);
		}
        
        // set the attractor to the current pose
		enc->getEncoders(q1);
        //std::cout << " q = [";
		for ( int j=0; j<numJoints; j++ ) {
			q0[j] = q1[j];
			x[j] = q1[j];
            //x[j] = 100;
            //std::cout << q1[j] << " ";
		}
        //std::cout << "]" << std::endl;
        
        // open the control port
        portPrefix = "/MoBeE/";
        portPrefix += _partName;
		port.open( portPrefix + "/cmd" );
		srand ( yarp::os::Time::now() );
	} //else { throw("could not connect to robot!"); }
}
PartController::~PartController(){}

bool PartController::isValid()
{
	if ( !dd || !dd->isValid() || !vel || !enc )
		return false;
	return true;
}

bool PartController::threadInit()
{
	if ( !isValid() ) {
		printf("Device driver not valid.\n");
		return false;
	}
	//printf("Starting PartController\n");
	return true;
}

//called by start after threadInit, s is true iff the thread started
//successfully
void PartController::afterStart(bool s)
{
	if (!s)
		printf("PartController did not start\n");
}


double PartController::magnitude(yarp::os::Bottle* list)
{
    double m = 0.0;
    for ( int i = 0; i < numJoints; i++ ) {
        if (!list->get(i).isNull()) {
            m+=list->get(i).asDouble()*list->get(i).asDouble();
        }
    }
    return sqrt(m);
}

void PartController::setATT( yarp::os::Bottle* list )
{
	for ( int i = 0; i < numJoints; i++ ) {
        if (!list->get(i).isNull())
        {
            double normPos = list->get(i).asDouble();
            if (normPos<0) normPos = 0.0;
            else if (normPos>1) normPos = 1.0;
            x[i] = min[i] + normPos*(max[i]-min[i]);
        }
    }
}

void PartController::setFCST(yarp::os::Bottle* list)
{
    //printf("setFCST: %s\n",list->toString().c_str());

    // expects a vector where each element is on the interval [-1,1]
    // enforces a hard limit on each dimension of the force
    for ( int i = 0; i < numJoints; i++ ) {
        if (!list->get(i).isNull()) {
            double f = list->get(i).asDouble();
            if (f>1.0) f=1.0;
            else if (f<-1.0) f=-1.0;
            fCst[i] = f*fCstMax[i];
        }
    }
}

void PartController::setFFLD(yarp::os::Bottle* list)
{
    // squash the force contained in 'list' but preserve its direction
    double mag = magnitude(list);
    for ( int i = 0; i < numJoints; i++ ) {
        if (!list->get(i).isNull())
            fFld[i] = fFldMax[i]/(fFldMax[i]/kfFld[i]+mag) * list->get(i).asDouble();
    }
}

void PartController::setFRPC( yarp::os::Bottle* list )
{
    // squash the force contained in 'list' but preserve its direction
    double mag = magnitude(list);
    //printf("RPC magnitude: %f\n", mag);
	for ( int i = 0; i < numJoints; i++ ) {
        if (!list->get(i).isNull()) {
            fRPC[i] = fRPCMax[i]/(fRPCMax[i]/kfRPC[i]+mag) * list->get(i).asDouble();
        }
    }
}

void PartController::run() 
{
	if ( !isValid() ) {
		printf("Device driver not valid.\n");
		return;
	}
	
	// read control commands from socket and handle them
	yarp::os::Bottle* b = NULL;
	b = port.read(false);
	if ( b ) {
		//printf("got a bottle: %s\n", b->toString().c_str());
        int cmd = b->get(0).asVocab();
        switch (cmd) {
            case VOCAB_QATTR:
                setATT( b->get(1).asList() );
                printf("\nSet attractor position!!! (%s)\n\n", b->get(1).asList()->toString().c_str());
                break;
            case VOCAB_QFORCE:
                setFRPC( b->get(1).asList() );
                printf("\nSet joint-space force!!! (%s)\n\n", b->get(1).asList()->toString().c_str());
                break;
            default:
                handler(b);
                break;
        }
	}
	
    yarp::os::Bottle view0,view1,view2,view3,view4;

	for ( int j=0; j<numJoints; j++ )
		q0[j] = q1[j];
    
    if ( getEncoders(q1) )
    {
        // process encoder positions... (the derrived class "Controller" sets the robot position in KineamticModel)
        procEncoders(q1);
        
        // compute the next control command
        for ( int i=0; i<numJoints; i++ )
        {
            // update error vector (to attractor) and velocity
            e[i] = w[i]*(x[i] - q1[i]);
            v[i] = 1000.0 * (q1[i] - q0[i]) / getRate();
            
            // compute sigmoidal spring force
            fX[i] = fXMax[i]*e[i]/(fXMax[i]/k[i]+abs(e[i]));
            
            // compute joint limit repulsion
            if ( q1[i] < min[i] + nogo[i] )         fLim[i] = fLimMax[i] * (min[i]+nogo[i]-q1[i])/nogo[i];
            else if ( q1[i] > max[i] - nogo[i] )    fLim[i] = fLimMax[i] * -(q1[i]-(max[i]-nogo[i]))/nogo[i];
            else                                    fLim[i] = 0.0;
      
            // compute acceleration (should squash this too)
            a[i] =  - c[i]*v[i]
                    + fX[i]
                    + fLim[i]
                    + fCst[i]
                    + fFld[i]
                    + fRPC[i]
                    ;
            
            // compute next control command
            ctrl[i] = v[i] + a[i] * getRate()/1000.0;
            
            if (i<7) {
                view0.addDouble(fX[i]);
                view1.addDouble(fLim[i]);
                view2.addDouble(fCst[i]);
                view3.addDouble(fFld[i]);
                view4.addDouble(fRPC[i]);
            }
        }
        //printf("fX:   %s\n", view0.toString().c_str());
        //printf("fLim: %s\n", view1.toString().c_str());
        //printf("fCst: %s\n", view2.toString().c_str());
        //printf("fFld: %s\n", view3.toString().c_str());
        //printf("fRPC: %s\n", view4.toString().c_str());
        //printf("\n");
        
        vel->velocityMove( ctrl );
    }
}

void PartController::threadRelease()
{
	vel->stop();
	
	printf("\n*** Goodbye from PartController ***\n\n");
	if ( dd ) { 
		dd->close();
		delete dd;
		dd = NULL;
		enc = NULL;
		vel = NULL;
	}
	
	numJoints = 0;
	
}

