#include "partController.h"
#include <iostream>

PartController::PartController( const char* _robotName, const char* _partName, const char* _fileName, int r ) : yarp::os::RateThread(r),
                                                                                                                vel(NULL),
                                                                                                                enc(NULL),
                                                                                                                aMag(0.0),
                                                                                                                vMag(0.0)
{
	printf( "\nOpening Remote Control Board: %s %s\n", _robotName, _partName );
	
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
        printf("Getting number of DOF...");
        while (!pos->getAxes(&numJoints)) {
            printf(".");
        }
        printf(" %d\n",numJoints);
     
        // initialize control vars
        q1 = new double[numJoints];
        q0 = new double[numJoints];
        //qNorm = new double[numJoints];
        v = new double[numJoints];
        e = new double[numJoints];
        
        x = new double[numJoints];
        min = new double[numJoints];
        max = new double[numJoints];
        kfLim = new double[numJoints];
        
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
        
        printf("Getting control params from: %s\n",_fileName);
        
        yarp::os::Property prop;
        bool configOK = prop.fromConfigFile(_fileName);
        yarp::os::Bottle* weight = prop.find("weight").asList();
        yarp::os::Bottle* spring_const = prop.find("spring_const").asList();
        yarp::os::Bottle* damping_const = prop.find("damping_const").asList();
        yarp::os::Bottle* limit_const = prop.find("limit_const").asList();
        yarp::os::Bottle* field_const = prop.find("field_const").asList();
        yarp::os::Bottle* constraint_const = prop.find("constraint_const").asList();
        yarp::os::Bottle* rpc_force_const = prop.find("rpc_force_const").asList();
        yarp::os::Bottle* max_spring_force = prop.find("max_spring_force").asList();
        yarp::os::Bottle* max_limit_force = prop.find("max_limit_force").asList();
        yarp::os::Bottle* max_field_force = prop.find("max_field_force").asList();
        yarp::os::Bottle* max_constraint_force = prop.find("max_constraint_force").asList();
        yarp::os::Bottle* max_rpc_force = prop.find("max_rpc_force").asList();
        
        // make sure we have all the control params
        if ( !weight || weight->size()!=numJoints ||
            !spring_const || spring_const->size()!=numJoints ||
            !damping_const || damping_const->size()!=numJoints ||
            !limit_const || limit_const->size()!=numJoints ||
            !field_const || field_const->size()!=numJoints ||
            !constraint_const || constraint_const->size()!=numJoints ||
            !rpc_force_const || rpc_force_const->size()!=numJoints ||
            !max_spring_force || max_spring_force->size()!=numJoints ||
            !max_limit_force || max_limit_force->size()!=numJoints ||
            !max_field_force || max_field_force->size()!=numJoints ||
            !max_constraint_force || max_constraint_force->size()!=numJoints ||
            !max_rpc_force || max_rpc_force->size()!=numJoints )
        {
            //dd->close();
            //return;
            configOK = false;
        }

        // set control params
        printf("Getting limits and setting control params for joint: ");
		for ( int i = 0; i < numJoints; i++ )
        {
            double _min,_max;
            printf("%d ...",i);
            while (!lim->getLimits( i, &_min, &_max )) {
                printf(".");
            }
            printf(" ");
            
            min[i]      = _min;
            max[i]      = _max;
            
            q1[i]       = 0.0;
            q0[i]       = 0.0;
            e[i]        = 0.0;
            x[i]        = 0.0;
            v[i]        = 0.0;
            a[i]        = 0.0;
            ctrl[i]     = 0.0;
            fX[i]       = 0.0;
            fLim[i]     = 0.0;
            fCst[i]     = 0.0;
            fFld[i]     = 0.0;
            fRPC[i]     = 0.0;
            
            if (configOK) {
                w[i]        = weight->get(i).asDouble();
                k[i]        = spring_const->get(i).asDouble();
                c[i]        = damping_const->get(i).asDouble();
                fXMax[i]    = max_constraint_force->get(i).asDouble();
                kfLim[i]    = limit_const->get(i).asDouble();
                fLimMax[i]  = max_limit_force->get(i).asDouble();
                kfCst[i]    = constraint_const->get(i).asDouble();
                fCstMax[i]  = max_constraint_force->get(i).asDouble();
                kfFld[i]    = field_const->get(i).asDouble();
                fFldMax[i]  = max_field_force->get(i).asDouble();
                kfRPC[i]    = rpc_force_const->get(i).asDouble();
                fRPCMax[i]  = max_rpc_force->get(i).asDouble();
            } else {
                w[i]        = 1.0;
                k[i]        = 1.0;
                c[i]        = 1.0;
                fXMax[i]    = 1.0;
                kfLim[i]    = 1.0;
                fLimMax[i]  = 1.0;
                kfCst[i]    = 1.0;
                fCstMax[i]  = 1.0;
                kfFld[i]    = 1.0;
                fFldMax[i]  = 1.0;
                kfRPC[i]    = 1.0;
                fRPCMax[i]  = 1.0;
            }
            
			//printf("joint %d: min = %f max = %f\n",i,_min,_max);
		}
        printf("\n");
        
        if (configOK) {
            printf("Controller configuration succeeded!!\n");
            controllerIsOn = true;
        } else {
            printf("Controller configuration failed!!\n");
            controllerIsOn = false;
        }
        
        // set the attractor to the current pose
        printf("Getting motor encoder positions...");
		while (!enc->getEncoders(q1)) {
            printf(".");
        }
        printf("\n");
        
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
		commandPort.open( portPrefix + "/cmd:i" );
        statePort.open( portPrefix + "/state:o" );
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
    //printf("setATT: %s\n",list->toString().c_str());
    //printf("q: ");
	for ( int i = 0; i < numJoints; i++ ) {
        if (!list->get(i).isNull())
        {
            double normPos = list->get(i).asDouble();
            if (normPos<0.0) normPos = 0.0;
            else if (normPos>1.0) normPos = 1.0;
            x[i] = min[i] + normPos*(max[i]-min[i]);
            //printf("%f ",x[i]);
        }
    }
    //printf("\n");
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
	b = commandPort.read(false);
	if ( b ) {
		printf("got a bottle: %s\n", b->toString().c_str());
        int cmd = b->get(0).asVocab();
        switch (cmd) {
            case VOCAB_QATTR:
                setATT( b->get(1).asList() );
                printf("  Set attractor position!!! (%s)\n\n", b->get(1).asList()->toString().c_str());
                break;
            case VOCAB_QFORCE:
                setFRPC( b->get(1).asList() );
                printf("  Set joint-space force!!! (%s)\n\n", b->get(1).asList()->toString().c_str());
                break;
            default:
                handler(b);
                break;
        }
	} //else { printf("got no bottle!\n"); }
	
    yarp::os::Bottle view0,view1,view2,view3,view4,view5,view6,view7;

	for ( int j=0; j<numJoints; j++ )
		q0[j] = q1[j];
    
    if ( getEncoders(q1) )
    {
        // process encoder positions... (the derrived class "Controller" sets the robot position in KineamticModel)
        procEncoders(q1);
        
        yarp::os::Bottle& normPose = statePort.prepare();
        normPose.clear();
        
        aMag=0;
        vMag=0;
        
        // compute the current normalized state and the next control command
        for ( int i=0; i<numJoints; i++ )
        {
            // normalize the current pose
            normPose.addDouble( (q1[i]-min[i])/(max[i]-min[i]) );
            
            // update error vector (to attractor) and velocity
            e[i] = w[i]*(x[i] - q1[i]);
            v[i] = 1000.0 * (q1[i] - q0[i]) / getRate();
            
            // compute sigmoidal spring force
            fX[i] = fXMax[i]*e[i]/(fXMax[i]/k[i]+abs(e[i]));
            
            // compute joint limit repulsion
            if ( q1[i] < min[i] + kfLim[i] )         fLim[i] = fLimMax[i] * (min[i]+kfLim[i]-q1[i])/kfLim[i];
            else if ( q1[i] > max[i] - kfLim[i] )    fLim[i] = fLimMax[i] * -(q1[i]-(max[i]-kfLim[i]))/kfLim[i];
            else                                    fLim[i] = 0.0;
      
            // compute acceleration (should squash this too?)
            a[i] =  - c[i]*v[i]
                    + fX[i]
                    + fLim[i]
                    + fCst[i]
                    + fFld[i]
                    + fRPC[i]
                    ;
            
            // compute magnitudes of a and v
            aMag += a[i]*a[i];
            vMag += v[i]*v[i];
            
            // compute next control command
            ctrl[i] = v[i] + a[i] * getRate()/1000.0;
            if ( ctrl[i]*ctrl[i] < 0.1 ) ctrl[i] = 0.0;
            
            if (i<7) {
                view0.addDouble(fX[i]);
                view1.addDouble(fLim[i]);
                view2.addDouble(fCst[i]);
                view3.addDouble(fFld[i]);
                view4.addDouble(fRPC[i]);
                view5.addDouble(x[i]);
                view6.addDouble(- c[i]*v[i]);
                view7.addDouble(ctrl[i]);
            }
        }
        
        // determine if the robot is moving
        aMag = sqrt(aMag);
        vMag = sqrt(vMag);
        
        //printf("x:    %s\n", view5.toString().c_str());
        //printf("fX:   %s\n", view0.toString().c_str());
        
        //printf("fLim: %s\n", view1.toString().c_str());
        //printf("fCst: %s\n", view2.toString().c_str());
        //printf("fFld: %s\n", view3.toString().c_str());
        //printf("fRPC: %s\n", view4.toString().c_str());
        
        //printf("fd:   %s\n", view6.toString().c_str());
        //printf("cmd:  %s\n", view7.toString().c_str());
        //printf("\n");
        
        if ( controllerIsOn )
            vel->velocityMove( ctrl );
        
        statePort.write();
        
        publishState();
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

