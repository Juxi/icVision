/*******************************************************************
 ***               Copyright (C) 2011 Mikhail Frank              ***
 ***  CopyPolicy: Released under the terms of the GNU GPL v2.0.  ***
 ******************************************************************/

/** \addtogroup RL
 *	@{
 */

#ifndef MOBEE_INTERFACE_H_
#define MOBEE_INTERFACE_H_

#include <yarp/os/all.h>
#include "util.h"

class MoBeE_Interface
{
public:
    
    MoBeE_Interface(){}
    ~MoBeE_Interface()
    {
        statePort.interrupt();
        commandPort.interrupt();
        controllerClient.interrupt();
        worldClient.interrupt();
    }
    
    // open stuff
    void openStatePort(yarp::os::ConstString s) { statePort.open(s); }
    void openCommandPort(yarp::os::ConstString s) { commandPort.open(s); }
    void openControllerClient(yarp::os::ConstString s) { controllerClient.open(s); }
    void openWorldClient(yarp::os::ConstString s) { worldClient.open(s); }
    
    // communicate with MoBeE model
    yarp::os::Bottle* getState() { return statePort.read(); }
    yarp::os::Bottle getMarkers();
    void getMarkerState( yarp::os::ConstString& markerName, Point_3& p, Vector_3& n);
    void setAttractor( Point_d q );
    void setOpSpace( yarp::os::ConstString name, Vector_3, Vector_3 );
    void stopForcing(int);
    bool isStable();
    yarp::os::ConstString mkSphere( double x, double y, double z, double r );
    //yarp::os::ConstString mkCyl( double r, double len, );
    void rmGeom( yarp::os::ConstString& );
    void defTarget( yarp::os::ConstString& );
    void defObstacle( yarp::os::ConstString& );
    
private:

    yarp::os::Network network;
    yarp::os::BufferedPort<yarp::os::Bottle>    statePort,
                                                commandPort;
    yarp::os::RpcClient controllerClient,
                        worldClient;
};
#endif
/** @} */
