/* DepthYARP.h
 *
 * Author: Juxi Leitner
 * Last update: 2007-Sep-10
 * Class declaration file for the DepthYARP class.
 * This class is the encapsulation for a YARP module that runs the Depth Perception algorithm.
 */
#pragma once
#include <yarp/os/all.h>
#include <yarp/sig/all.h>
using namespace yarp::os;
using namespace yarp::sig;

class DepthYARP : public Module {
private:
// the yarp connection ports
	// stereo images input
	BufferedPort< ImageOf<PixelBgr> > leftIn;
	BufferedPort< ImageOf<PixelBgr> > rightIn;

	// disparity map output
	BufferedPort< ImageOf<PixelBgr> > outPort;
	
	
	RpcServer rpcPort; 	//!< The port to handle RPC messages (e.g. quit)	
	

	// reference to the Depth Perception algorithm.
	DisparityMapper *mapper;

	// the current disparitymap
	IplImage *dispMap;
	
	double scalingFactor;

public:
	// constructor & destructor
	DepthYARP();
	~DepthYARP();

	// standard functions for YARP modules
	virtual bool open(Searchable &config);
	virtual bool close();
	virtual bool interruptModule();

	virtual double getPeriod();
	

	// the actual run function
	virtual bool updateModule();
};