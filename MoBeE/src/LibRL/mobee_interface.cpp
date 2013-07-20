#include "mobee_interface.h"

yarp::os::Bottle MoBeE_Interface::getMarkers()
{
    yarp::os::Bottle get,list;
    get.addVocab(yarp::os::Vocab::encode("list"));
    controllerClient.write(get,list);
    return list;
}

bool MoBeE_Interface::isStable()
{
    // check if the robot is still moving
    yarp::os::Bottle query,reply;
    query.addVocab(yarp::os::Vocab::encode("stpd"));
    query.addDouble(100); // 50 acceleration threshold
    query.addDouble(2); // 1.0 velocity threshold
    controllerClient.write(query,reply);
    //printf("query: %s\n",query.toString().c_str());
    //printf("reply: %s\n",reply.toString().c_str());
    
    if ( reply.get(0).asInt() == 0 )
        return false;
    return true;
}

void MoBeE_Interface::setAttractor( Point_d q )
{
    // activate the attractor at q
    yarp::os::Bottle& attractorCommand = commandPort.prepare();
    attractorCommand.clear();
    attractorCommand.addVocab(yarp::os::Vocab::encode("qatt"));
    yarp::os::Bottle vec;
    for (Point_d::Cartesian_const_iterator i=q.cartesian_begin(); i!=q.cartesian_end(); ++i)
        vec.addDouble(*i);
    attractorCommand.addList() = vec;
    //printf("Sending attractor command: %s\n", attractorCommand.toString().c_str());
    commandPort.writeStrict();
}

void MoBeE_Interface::setOpSpace( yarp::os::ConstString name, Vector_3 f, Vector_3 t )
{
    yarp::os::Bottle forceCmd;
    forceCmd.addDouble(f.x());
    forceCmd.addDouble(f.y());
    forceCmd.addDouble(f.z());
    forceCmd.addDouble(t.x());
    forceCmd.addDouble(t.y());
    forceCmd.addDouble(t.z());
    
    // prepare to control the robot with operational space force/torque
    yarp::os::Bottle& opSpaceForceAndTorque = commandPort.prepare();
    opSpaceForceAndTorque.clear();
    opSpaceForceAndTorque.addVocab(yarp::os::Vocab::encode("opsp"));
    opSpaceForceAndTorque.addString(name);
    opSpaceForceAndTorque.addList() = forceCmd;
    
    //printf("Sending control command: %s\n", opSpaceForceAndTorque.toString().c_str());
    commandPort.writeStrict();
}

yarp::os::ConstString MoBeE_Interface::mkSphere( double x, double y, double z, double r )
{
    yarp::os::Bottle worldCmd, worldRsp;
    worldCmd.addVocab(yarp::os::Vocab::encode("mk"));
    worldCmd.addVocab(yarp::os::Vocab::encode("sph"));
    worldCmd.addDouble(r);
    worldCmd.addDouble(x);
    worldCmd.addDouble(y);
    worldCmd.addDouble(z);
    worldClient.write(worldCmd,worldRsp);
    //printf("  world command: %s\n", worldCmd.toString().c_str());
    //printf("  world response: %s\n\n", worldRsp.toString().c_str());
    
    // get the name of the newly inserted object
    return worldRsp.get(0).asString();
    
    // make it a "target" object
    //worldCmd.clear();
    //worldRsp.clear();
    //worldCmd.addVocab(yarp::os::Vocab::encode("def"));
    //worldCmd.addString(mobeeObjectName);
    //worldCmd.addVocab(yarp::os::Vocab::encode("tgt"));
    //learner->worldClient.write(worldCmd,worldRsp);
    
    //printf("  world command: %s\n", worldCmd.toString().c_str());
    //printf("  world response: %s\n\n", worldRsp.toString().c_str());
}

void MoBeE_Interface::rmGeom( yarp::os::ConstString& name )
{
    // remove the target from the model
    yarp::os::Bottle worldCmd, worldRsp;
    worldCmd.addVocab(yarp::os::Vocab::encode("rm"));
    worldCmd.addString(name);
    worldClient.write(worldCmd,worldRsp);
    printf("  world command: %s\n", worldCmd.toString().c_str());
    printf("  world response: %s\n\n", worldRsp.toString().c_str());
}


void MoBeE_Interface::defTarget( yarp::os::ConstString& name )
{
    yarp::os::Bottle worldCmd, worldRsp;
    worldCmd.addVocab(yarp::os::Vocab::encode("def"));
    worldCmd.addString(name);
    worldCmd.addVocab(yarp::os::Vocab::encode("tgt"));
    worldClient.write(worldCmd,worldRsp);
}

void MoBeE_Interface::defObstacle( yarp::os::ConstString& name )
{
    yarp::os::Bottle worldCmd, worldRsp;
    worldCmd.addVocab(yarp::os::Vocab::encode("def"));
    worldCmd.addString(name);
    worldCmd.addVocab(yarp::os::Vocab::encode("obs"));
    worldClient.write(worldCmd,worldRsp);
}


void MoBeE_Interface::defGhost(yarp::os::ConstString& name)
{
    yarp::os::Bottle worldCmd, worldRsp;
    worldCmd.addVocab(yarp::os::Vocab::encode("def"));
    worldCmd.addString(name);
    worldCmd.addVocab(yarp::os::Vocab::encode("obs"));
    worldClient.write(worldCmd,worldRsp);
}

void MoBeE_Interface::getMarkerState( yarp::os::ConstString& markerName, Point_3& p, Vector_3& n)
{
    // get the state of the hand
    yarp::os::Bottle get,state;
    get.addVocab(yarp::os::Vocab::encode("get"));
    get.addString(markerName);
    controllerClient.write(get,state);
    
    // get the operational space position of the marker
    p = Point_3(state.get(0).asDouble(),
              state.get(1).asDouble(),
              state.get(2).asDouble());
    n = Vector_3(state.get(3).asDouble(),
               state.get(4).asDouble(),
               state.get(5).asDouble());
}

void MoBeE_Interface::stopForcing(int size)
{
    yarp::os::Bottle& stopBottle = commandPort.prepare();
    stopBottle.clear();
    stopBottle.addVocab(yarp::os::Vocab::encode("qfor"));
    yarp::os::Bottle forceCmd;
    for (int i=0; i<size; i++)
        forceCmd.addDouble(0.0);
    stopBottle.addList() = forceCmd;
    //printf("Sending force command: %s\n", stopBottle.toString().c_str());
    commandPort.writeStrict();
}