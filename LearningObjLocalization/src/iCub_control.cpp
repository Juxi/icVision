//########################################################################################################//
//Aquila - An Open-Source GPU-Accelerated Toolkit for Cognitive and Neuro-Robotics Research               //
//                                                                                                        //
//Copyright (C) 2011    Martin Peniak(www.martinpeniak.com)                                               //
//                      Christopher Larcobme (christopher.larcombe@plymouth.ac.uk)                        //
//                                                                                                        //
//This program is free software: you can redistribute it and/or modify it under the terms of the GNU      //
//General Public License as published by the Free Software Foundation, either version 3 of the License,   //
//or (at your option) any later version.                                                                  //
//########################################################################################################//

#include <iostream>
#include "iCub_control.h"

//using namespace std;


//iCubControl::iCubControl() {
//}

/**
*@brief     constructor
*@param[in] ptr_mw - a pointer to MainWindow
*/
iCubControl::iCubControl(Window *ptr_mw) {
    main_window                                 = ptr_mw;
	
	yarp_network	= new Network();
	simulation		= true;

//    manager                                     = new Manager;

    left_camera                                 = new Camera;
    left_camera->initialised                    = false;

    right_camera                                = new Camera;
    right_camera->initialised                   = false;

	/*    left_arm                                    = new Arm;
    right_arm                                   = new Arm;
    left_leg                                    = new Leg;
    right_leg                                   = new Leg;
    head                                        = new SimplePart;
    torso                                       = new SimplePart;
    inertial_sensor                             = new InertialSensor;
    left_hand_hall_sensors                      = new HandHallSensors;
    right_hand_hall_sensors                     = new HandHallSensors;
    face                                        = new Face;
    simulation                                  = new Simulation;
    speech                                      = new Speech; */

/*    left_arm->initialised                       = false;
    right_arm->initialised                      = false;
    left_leg->initialised                       = false;
    right_leg->initialised                      = false;
    head->initialised                           = false;
    torso->initialised                          = false;
    inertial_sensor->initialised                = false;
    face->initialised                           = false;
    simulation->initialised                     = false;
    speech->initialised                         = false;
    left_hand_hall_sensors->initialised         = false;
    right_hand_hall_sensors->initialised        = false;
    manager->ft_sensors_initialised             = false;*/

/*    left_arm->impedance_mode                    = false;
    right_arm->impedance_mode                   = false;
    left_leg->impedance_mode                    = false;
    right_leg->impedance_mode                   = false;

    left_arm->equilibrium_recorded              = false;
    right_arm->equilibrium_recorded             = false;
    left_leg->equilibrium_recorded              = false;
    right_leg->equilibrium_recorded             = false; */

//    manager->monitor_equilibrium_differences    = false;
//    manager->equilibrium_recorded               = false;

/*    left_arm->equilibrium_threshold             = 0.25;
    right_arm->equilibrium_threshold            = 0.25;
    left_leg->equilibrium_threshold             = 0.25;
    right_leg->equilibrium_threshold            = 0.25;

    manager->num_clients                        = 0;
    left_camera->num_clients                    = 0;
    right_camera->num_clients                   = 0;
    left_arm->num_clients                       = 0;
    right_arm->num_clients                      = 0;
    left_leg->num_clients                       = 0;
    right_leg->num_clients                      = 0;
    head->num_clients                           = 0;
    torso->num_clients                          = 0;
    inertial_sensor->num_clients                = 0;
    face->num_clients                           = 0;
    simulation->num_clients                     = 0;
    speech->num_clients                         = 0;
    left_hand_hall_sensors->num_clients         = 0;
    right_hand_hall_sensors->num_clients        = 0;

    manager->*/
	yarp_network->init();
}

iCubControl::~iCubControl() {
}


/**
*@brief     main thread loop
*/
void iCubControl::run()
{
/*    char    buf[100];
    double  sqr_ft = 0;
    double  leftArmEquilibrium    = 0;
    double  rightArmEquilibrium   = 0;

    while(true)
    {
        if(manager->ft_sensors_initialised && manager->monitor_equilibrium_differences)
        {
            //record equilibrium values of force-torque sensors
            if(!manager->equilibrium_recorded)
            {
                recordLeftArmEquilibrum();
                recordRightArmEquilibrum();
                recordLeftLegEquilibrum();
                recordRightLegEquilibrum();
                manager->equilibrium_recorded = true;
            }
            else
            {
                if(left_arm->initialised)
                {
                    if(getLeftArmEquilibrumDifference()  > left_arm->equilibrium_threshold && !left_arm->impedance_mode)
                    {
                        cout<<"Aquila: left  arm equilibrium disrupted, changing to impedance control mode"<<endl;
                        for(int i=0;i<5;i++) left_arm->mode->setImpedancePositionMode(i);
                        left_arm->impedance_mode = true;
                    }
                }

                if(right_arm->initialised)
                {
                    if(getRightArmEquilibrumDifference() > right_arm->equilibrium_threshold  && !right_arm->impedance_mode)
                    {
                        cout<<"Aquila: right arm equilibrium disrupted, changing to impedance control mode"<<endl;
                        for(int i=0;i<5;i++) right_arm->mode->setImpedancePositionMode(i);
                        right_arm->impedance_mode = true;
                    }
                }

                if(left_leg->initialised)
                {
                    if(getLeftLegEquilibrumDifference()  > left_leg->equilibrium_threshold && !left_leg->impedance_mode)
                    {
                        cout<<"Aquila: left  leg equilibrium disrupted, changing to impedance control mode"<<endl;
                        for(int i=0;i<4;i++) left_leg->mode->setImpedancePositionMode(i);
                        left_leg->impedance_mode = true;
                    }
                }

                if(right_leg->initialised)
                {
                    if(getRightLegEquilibrumDifference() > right_leg->equilibrium_threshold && !right_leg->impedance_mode)
                    {
                        cout<<"Aquila: right leg equilibrium disrupted, changing to impedance control mode"<<endl;
                        for(int i=0;i<4;i++) right_leg->mode->setImpedancePositionMode(i);
                        right_leg->impedance_mode = true;
                    }
                }
            }
        }
        else manager->equilibrium_recorded = false;
    }*/
}

/**
*@brief     initialises left camera
*/
void iCubControl::initLeftCamera() {
    if(!left_camera->initialised) {
		QString name("/");
		name.append("juxi");
		name.append("/");
		name.append("left_cam");
		left_camera->port_name = name;	//, name.toStdString().c_str()
		
		std::cout<<endl<<"initialising left camera ..."<<std::endl;

        left_camera->port = new BufferedPort<ImageOf<PixelRgb> >;
        left_camera->port->open(name.toStdString().c_str());
		
/* if(simulation) */	yarp_network->connect("/icubSim/cam/left", left_camera->port_name.toStdString().c_str());	
		//else
//        {
//            manager->network->connect(left_camera->port_name.toStdString().c_str(),"/aquila/left_cam");
//            manager->network->connect("/icub/cam/left","/aquila/left_cam");
//        }
        left_camera->image = left_camera->port->read();
        left_camera->initialised = true;
    }
}

/**
*@brief     initialises right camera
*/
void iCubControl::initRightCamera() {
    if(!right_camera->initialised) {
		QString name("/");
		name.append("juxi");
		name.append("/");
		name.append("right_cam");
		right_camera->port_name = name;	//, name.toStdString().c_str()
		
		std::cout<<endl<<"initialising right camera ..."<<std::endl;
		
        right_camera->port = new BufferedPort<ImageOf<PixelRgb> >;
        right_camera->port->open(name.toStdString().c_str());
		
		if(simulation) {
			yarp_network->connect("/icubSim/cam/right", right_camera->port_name.toStdString().c_str());				
		}

        right_camera->image = right_camera->port->read();
        right_camera->initialised = true;
    }
}

/**
*@brief     initialises camera
*/
void iCubControl::initCameras() {
    initLeftCamera();
    initRightCamera();
}

/**
*@brief     initialises left arm
*/
//void iCubControl::initLeftArm(bool simulation)
//{
//    if(!left_arm->initialised)
//    {
//        cout<<endl<<"Aquila: initialising left arm for new client connection..."<<endl<<endl;
//
//        Property options;
//        options.put("device", "remote_controlboard");
//        options.put("local", "/aquila/left_arm");
//        if(simulation)options.put("remote", "/icubSim/left_arm");
//        else options.put("remote", "/icub/left_arm");
//
//        left_arm->device = new PolyDriver(options);
//
//        if (!left_arm->device->isValid())             cout<<"Aquila: device is not available"<<endl;
//        if (!left_arm->device->view(left_arm->lim))   cout<<"Aquila: IControlLimits interface is not available"<<endl;
//        if (!left_arm->device->view(left_arm->pos))   cout<<"Aquila: IPositionControl interface is not available"<<endl;
//        if (!left_arm->device->view(left_arm->encs))  cout<<"Aquila: IEncoders interface is not available"<<endl;
//        if (!left_arm->device->view(left_arm->vel))   cout<<"Aquila: IVelocityControl interface is not available"<<endl;
//        if (!left_arm->device->view(left_arm->imp))   cout<<"Aquila: IImpedanceControl interface is not available"<<endl;
//        if (!left_arm->device->view(left_arm->mode))  cout<<"Aquila: IControlMode interface is not available"<<endl;
//
//        left_arm->hand_encoder   = new yarp::sig::Vector;
//        left_arm->encoders       = new yarp::sig::Vector;
//        left_arm->command        = new yarp::sig::Vector;
//
//        left_arm->pos->getAxes(&left_arm->num_joints);
//        left_arm->encoders->resize(left_arm->num_joints);
//        left_arm->command->resize(left_arm->num_joints);
//
//        //allocate memory and set velocities and positions to 0
//        left_arm->velocities         = new double[left_arm->num_joints];
//        left_arm->positions          = new double[left_arm->num_joints];
//        left_arm->min_limit          = new double[left_arm->num_joints];
//        left_arm->max_limit          = new double[left_arm->num_joints];
//        for(int i=0;i<left_arm->num_joints;i++)
//        {
//            left_arm->velocities[i] = 0.0;
//            left_arm->positions[i]  = 0.0;
//            left_arm->lim->getLimits(i,&left_arm->min_limit[i],&left_arm->max_limit[i]);
//        }
//
//        left_arm->initialised = true;
//    }
//    else cout<<endl<<"Aquila: adding new client to left arm connection, "<<"total number of conencted clients: "<<left_arm->num_clients+1<<endl<<endl;
//    left_arm->num_clients++;
//}

/**
*@brief     initialises left arm
*/
//void iCubControl::initRightArm(bool simulation)
//{
//    if(!right_arm->initialised)
//    {
//        cout<<endl<<"Aquila: initialising right arm for new client connection..."<<endl<<endl;
//
//        Property options;
//        options.put("device", "remote_controlboard");
//        options.put("local", "/aquila/right_arm");
//        if(simulation)options.put("remote", "/icubSim/right_arm");
//        else options.put("remote", "/icub/right_arm");
//
//        right_arm->device = new PolyDriver(options);
//
//        if (!right_arm->device->isValid())                cout<<"Aquila: device is not available"<<endl;
//        if (!right_arm->device->view(right_arm->lim))     cout<<"Aquila: IControlLimits interface is not available"<<endl;
//        if (!right_arm->device->view(right_arm->pos))     cout<<"Aquila: IPositionControl interface is not available"<<endl;
//        if (!right_arm->device->view(right_arm->encs))    cout<<"Aquila: IEncoders interface is not available"<<endl;
//        if (!right_arm->device->view(right_arm->vel))     cout<<"Aquila: IVelocityControl interface is not available"<<endl;
//        if (!right_arm->device->view(right_arm->imp))     cout<<"Aquila: IImpedanceControl interface is not available"<<endl;
//        if (!right_arm->device->view(right_arm->mode))    cout<<"Aquila: IControlMode interface is not available"<<endl;
//
//        right_arm->hand_encoder  = new yarp::sig::Vector;
//        right_arm->encoders      = new yarp::sig::Vector;
//        right_arm->command       = new yarp::sig::Vector;
//
//        right_arm->pos->getAxes(&right_arm->num_joints);
//        right_arm->encoders->resize(right_arm->num_joints);
//        right_arm->command->resize(right_arm->num_joints);
//
//        //allocate memory and set velocities and positions to 0
//        right_arm->velocities    = new double[right_arm->num_joints];
//        right_arm->positions     = new double[right_arm->num_joints];
//        right_arm->min_limit     = new double[right_arm->num_joints];
//        right_arm->max_limit     = new double[right_arm->num_joints];
//        for(int i=0;i<right_arm->num_joints;i++)
//        {
//            right_arm->velocities[i] = 0.0;
//            right_arm->positions[i]  = 0.0;
//            right_arm->lim->getLimits(i,&right_arm->min_limit[i],&right_arm->max_limit[i]);
//        }
//
//        right_arm->initialised = true;
//    }
//    else cout<<endl<<"Aquila: adding new client to right arm connection, "<<"total number of conencted clients: "<<right_arm->num_clients+1<<endl<<endl;
//    right_arm->num_clients++;
//}

/**
*@brief     initialises left leg
*/
//void iCubControl::initLeftLeg(bool simulation)
//{
//    if(!left_leg->initialised)
//    {
//        cout<<endl<<"Aquila: initialising left leg for new client connection..."<<endl<<endl;
//
//        Property options;
//        options.put("device", "remote_controlboard");
//        options.put("local", "/aquila/left_leg");
//        if(simulation) options.put("remote", "/icubSim/left_leg");
//        else options.put("remote", "/icub/left_leg");
//
//        left_leg->device = new PolyDriver(options);
//
//        if (!left_leg->device->isValid())             cout<<"Aquila: device is not available"<<endl;
//        if (!left_leg->device->view(left_leg->lim))   cout<<"Aquila: IControlLimits interface is not available"<<endl;
//        if (!left_leg->device->view(left_leg->pos))   cout<<"Aquila: IPositionControl interface is not available"<<endl;
//        if (!left_leg->device->view(left_leg->encs))  cout<<"Aquila: IEncoders interface is not available"<<endl;
//        if (!left_leg->device->view(left_leg->vel))   cout<<"Aquila: IVelocityControl interface is not available"<<endl;
//        if (!left_leg->device->view(left_leg->imp))   cout<<"Aquila: IImpedanceControl interface is not available"<<endl;
//        if (!left_leg->device->view(left_leg->mode))  cout<<"Aquila: IControlMode interface is not available"<<endl;
//
//        left_leg->encoders = new yarp::sig::Vector;
//        left_leg->command  = new yarp::sig::Vector;
//
//        left_leg->pos->getAxes(&left_leg->num_joints);
//        left_leg->encoders->resize(left_leg->num_joints);
//        left_leg->command->resize(left_leg->num_joints);
//
//        //allocate memory and set velocities and positions to 0
//        left_leg->velocities = new double[left_leg->num_joints];
//        left_leg->positions  = new double[left_leg->num_joints];
//        left_leg->min_limit  = new double[left_leg->num_joints];
//        left_leg->max_limit  = new double[left_leg->num_joints];
//        for(int i=0;i<left_leg->num_joints;i++)
//        {
//            left_leg->positions[i] = 0.0;
//            left_leg->positions[i] = 0.0;
//            left_leg->lim->getLimits(i,&left_leg->min_limit[i],&left_leg->max_limit[i]);
//        }
//        left_leg->initialised = true;
//    }
//    else cout<<endl<<"Aquila: adding new client to left leg connection, "<<"total number of conencted clients: "<<left_leg->num_clients+1<<endl<<endl;
//    left_leg->num_clients++;
//}

/**
*@brief     initialises right leg
*/
//void iCubControl::initRightLeg(bool simulation)
//{
//    if(!right_leg->initialised)
//    {
//        cout<<endl<<"Aquila: initialising right leg for new client connection..."<<endl<<endl;
//
//        Property options;
//        options.put("device", "remote_controlboard");
//        options.put("local", "/aquila/right_leg");
//        if(simulation) options.put("remote", "/icubSim/right_leg");
//        else options.put("remote", "/icub/right_leg");
//
//        right_leg->device = new PolyDriver(options);
//
//        if (!right_leg->device->isValid())                cout<<"Aquila: device is not available"<<endl;
//        if (!right_leg->device->view(right_leg->lim))     cout<<"Aquila: IControlLimits interface is not available"<<endl;
//        if (!right_leg->device->view(right_leg->pos))     cout<<"Aquila: IPositionControl interface is not available"<<endl;
//        if (!right_leg->device->view(right_leg->encs))    cout<<"Aquila: IEncoders interface is not available"<<endl;
//        if (!right_leg->device->view(right_leg->vel))     cout<<"Aquila: IVelocityControl interface is not available"<<endl;
//        if (!right_leg->device->view(right_leg->imp))     cout<<"Aquila: IImpedanceControl interface is not available"<<endl;
//        if (!right_leg->device->view(right_leg->mode))    cout<<"Aquila: IControlMode interface is not available"<<endl;
//
//        right_leg->encoders = new yarp::sig::Vector;
//        right_leg->command  = new yarp::sig::Vector;
//
//        right_leg->pos->getAxes(&right_leg->num_joints);
//        right_leg->encoders->resize(right_leg->num_joints);
//        right_leg->command->resize(right_leg->num_joints);
//
//        //allocate memory and set velocities and positions to 0
//        right_leg->velocities = new double[right_leg->num_joints];
//        right_leg->positions  = new double[right_leg->num_joints];
//        right_leg->min_limit  = new double[right_leg->num_joints];
//        right_leg->max_limit  = new double[right_leg->num_joints];
//        for(int i=0;i<right_leg->num_joints;i++)
//        {
//            right_leg->positions[i] = 0.0;
//            right_leg->positions[i] = 0.0;
//            right_leg->lim->getLimits(i,&right_leg->min_limit[i],&right_leg->max_limit[i]);
//        }
//        right_leg->initialised = true;
//    }
//    else cout<<endl<<"Aquila: adding new client to right leg connection, "<<"total number of conencted clients: "<<right_leg->num_clients+1<<endl<<endl;
//    right_leg->num_clients++;
//}

/**
*@brief     initialises head
*/
void iCubControl::initHead(bool simulation)
{
    if(!head->initialised)
    {
        cout<<endl<<"Aquila: initialising head for new client connection..."<<endl<<endl;

        Property options;
        options.put("device", "remote_controlboard");
        options.put("local", "/aquila/head");
        if(simulation) options.put("remote", "/icubSim/head");
        else options.put("remote", "/icub/head");

        head->device = new PolyDriver(options);

        if (!head->device->isValid())         cout<<"Aquila: device is not available"<<endl;
        if (!head->device->view(head->lim))   cout<<"Aquila: IControlLimits interface is not available"<<endl;
        if (!head->device->view(head->pos))   cout<<"Aquila: IPositionControl interface is not available"<<endl;
        if (!head->device->view(head->encs))  cout<<"Aquila: IEncoders interface is not available"<<endl;
        if (!head->device->view(head->vel))   cout<<"Aquila: IVelocityControl interface is not available"<<endl;
        if (!head->device->view(head->mode))  cout<<"Aquila: IControlMode interface is not available"<<endl;

        head->encoders = new yarp::sig::Vector;
        head->command  = new yarp::sig::Vector;

        head->pos->getAxes(&head->num_joints);
        head->encoders->resize(head->num_joints);
        head->command->resize(head->num_joints);

        //allocate memory and set velocities and positions to 0
        head->velocities = new double[head->num_joints];
        head->positions  = new double[head->num_joints];
        head->min_limit  = new double[head->num_joints];
        head->max_limit  = new double[head->num_joints];
        for(int i=0;i<head->num_joints;i++)
        {
            head->positions[i] = 0.0;
            head->positions[i] = 0.0;
            head->lim->getLimits(i,&head->min_limit[i],&head->max_limit[i]);
        }
        head->initialised = true;
    }
    else cout<<endl<<"Aquila: adding new client to head connection, "<<"total number of conencted clients: "<<head->num_clients+1<<endl<<endl;
    head->num_clients++;
}

/**
*@brief     initialises left arm
*/
void iCubControl::initTorso(bool simulation)
{
    if(!torso->initialised)
    {
        cout<<endl<<"Aquila: initialising torso for new client connection..."<<endl<<endl;

        Property options;
        options.put("device", "remote_controlboard");
        options.put("local", "/aquila/torso");
        if(simulation) options.put("remote", "/icubSim/torso");
        else options.put("remote", "/icub/torso");

        torso->device = new PolyDriver(options);

        if (!torso->device->isValid())            cout<<"Aquila: device is not available"<<endl;
        if (!torso->device->view(torso->lim))     cout<<"Aquila: IControlLimits interface is not available"<<endl;
        if (!torso->device->view(torso->pos))     cout<<"Aquila: IPositionControl interface is not available"<<endl;
        if (!torso->device->view(torso->encs))    cout<<"Aquila: IEncoders interface is not available"<<endl;
        if (!torso->device->view(torso->vel))     cout<<"Aquila: IVelocityControl interface is not available"<<endl;
        if (!torso->device->view(torso->mode))    cout<<"Aquila: IControlMode interface is not available"<<endl;

        torso->encoders = new yarp::sig::Vector;
        torso->command  = new yarp::sig::Vector;

        torso->pos->getAxes(&torso->num_joints);
        torso->encoders->resize(torso->num_joints);
        torso->command->resize(torso->num_joints);

        //allocate memory and set velocities and positions to 0
        torso->velocities = new double[torso->num_joints];
        torso->positions  = new double[torso->num_joints];
        torso->min_limit  = new double[torso->num_joints];
        torso->max_limit  = new double[torso->num_joints];
        for(int i=0;i<torso->num_joints;i++)
        {
            torso->positions[i] = 0.0;
            torso->positions[i] = 0.0;
            torso->lim->getLimits(i,&torso->min_limit[i],&torso->max_limit[i]);
        }
        torso->initialised = true;
    }
    else cout<<endl<<"Aquila: adding new client to torso connection, "<<"total number of conencted clients: "<<torso->num_clients+1<<endl<<endl;
    torso->num_clients++;
}

/**
*@brief     initialises the world port used by the simulator
*/
//void iCubControl::initWorldPort()
//{
//    if(!simulation->initialised)
//    {
//        cout<<endl<<"Aquila: initialising simulation manager for new client connection..."<<endl<<endl;
//
//        simulation->command      = new Bottle;
//        simulation->world_port   = new Port;
//
//        simulation->world_port->open("/aquila/world");
//        manager->network->connect("/aquila/world", "/icubSim/world");
//
//        simulation->initialised = true;
//    }
//    else cout<<endl<<"Aquila: adding new client to simulation manager connection, "<<"total number of conencted clients: "<<simulation->num_clients+1<<endl<<endl;
//    simulation->num_clients++;
//}

/**
*@brief     initialises the inertial sensor
*/
//void iCubControl::initInertialSensor(bool simulation)
//{
//    if(!inertial_sensor->initialised)
//    {
//        cout<<endl<<"Aquila: initialising inertial sensor for new client connection..."<<endl<<endl;
//
//        inertial_sensor->port = new BufferedPort<yarp::sig::Vector>;
//        inertial_sensor->port->open("/aquila/inertial");
//
//        if(simulation) manager->network->connect("/icubSim/inertial","/aquila/inertial");
//        else manager->network->connect("/icub/inertial", "/aquila/inertial");
//
//        inertial_sensor->initialised = true;
//    }
//    else cout<<endl<<"Aquila: adding new client to inertial sensor connection, "<<"total number of conencted clients: "<<inertial_sensor->num_clients+1<<endl<<endl;
//    inertial_sensor->num_clients++;
//}
//
///**
//*@brief     initialises left hand finger encoders
//*/
//void iCubControl::initLeftHandHallSensors()
//{
//    if(!left_hand_hall_sensors->initialised)
//    {
//        cout<<endl<<"Aquila: initialising left hand finger encoders for new client connection..."<<endl<<endl;
//
//        left_hand_hall_sensors->port  = new BufferedPort<yarp::sig::Vector>;
//        left_hand_hall_sensors->port->open("/aquila/left_hand/analog:i");
//        manager->network->connect("/icub/left_hand/analog:o ","/aquila/left_hand/analog:i");
//
//        left_hand_hall_sensors->initialised = true;
//    }
//    else cout<<endl<<"Aquila: adding new client to inertial sensor connection, "<<"total number of conencted clients: "<<left_hand_hall_sensors->num_clients+1<<endl<<endl;
//    left_hand_hall_sensors->num_clients++;
//}
//
//
///**
//*@brief     initialises right hand finger encoders
//*/
//void iCubControl::initRightHandHallSensors()
//{
//    if(!right_hand_hall_sensors->initialised)
//    {
//        cout<<endl<<"Aquila: initialising right hand finger encoders for new client connection..."<<endl<<endl;
//
//        right_hand_hall_sensors->port  = new BufferedPort<yarp::sig::Vector>;
//        right_hand_hall_sensors->port->open("/aquila/right_hand/analog:i");
//        manager->network->connect("/icub/right_hand/analog:o ","/aquila/right_hand/analog:i");
//
//        right_hand_hall_sensors->initialised = true;
//    }
//    else cout<<endl<<"Aquila: adding new client to inertial sensor connection, "<<"total number of conencted clients: "<<right_hand_hall_sensors->num_clients+1<<endl<<endl;
//    right_hand_hall_sensors->num_clients++;
//}

/**
*@brief     initialises finger encoders for both hands
*/
//void iCubControl::initHandHallSensors()
//{
//    initLeftHandHallSensors();
//    initRightHandHallSensors();
//}

/**
*@brief     initialises force-torque sensors
*/
//void iCubControl::initForceTorqueSensors()
//{
//    if(!manager->ft_sensors_initialised)
//    {
//        cout<<endl<<"Aquila: initialising force torque sensors for new client connection..."<<endl<<endl;
//
//        left_arm->ft_analog_port     =  new BufferedPort<yarp::sig::Vector>;
//        right_arm->ft_analog_port    =  new BufferedPort<yarp::sig::Vector>;
//        left_leg->ft_analog_port     =  new BufferedPort<yarp::sig::Vector>;
//        right_leg->ft_analog_port    =  new BufferedPort<yarp::sig::Vector>;
//
//        left_arm->ft_analog_port->open("/aquila/left_arm/analog:i");
//        right_arm->ft_analog_port->open("/aquila/right_arm/analog:i");
//        left_leg->ft_analog_port->open("/aquila/left_leg/analog:i");
//        right_leg->ft_analog_port->open("/aquila/right_leg/analog:i");
//
//        manager->network->connect("/icub/left_arm/analog:o","/aquila/left_arm/analog:i");
//        manager->network->connect("/icub/right_arm/analog:o","/aquila/right_arm/analog:i");
//        manager->network->connect("/icub/left_leg/analog:o","/aquila/left_leg/analog:i");
//        manager->network->connect("/icub/right_leg/analog:o","/aquila/right_leg/analog:i");
//
//        manager->ft_sensors_initialised = true;
//        start();
//    }
//    else cout<<endl<<"Aquila: adding new client to force torque sensors connection, "<<"total number of conencted clients: "<<manager->num_clients+1<<endl<<endl;
//    manager->num_clients++;
//}

/**
*@brief     initialises force-torque sensors
*/
void iCubControl::initFaceExpressions(bool simulation)
{
    if(!face->initialised)
    {
        cout<<endl<<"Aquila: initialising face expressions..."<<endl<<endl;

        face->low_level_port = new BufferedPort<Bottle>;

        if(simulation)
        {
            if(system("yarp exists /icubSim/face/raw/in")!=0)
            {
                system("cd $ICUB_ROOT/main/src/simulators/iCubSimulation/FaceExpressions/ && ./FaceExpressions &");
                sleep(1);

                yarp_network->connect("/face/eyelids", "/icubSim/face/eyelids");
                yarp_network->connect("/face/image/out", "/icubSim/texture");

                face->low_level_port->open("/aquila/face/raw/out");
                face->low_level_port->setStrict(true);
				yarp_network->connect("/aquila/face/raw/out", "/icubSim/face/raw/in");
            }
            else
            {
                cout<<"Aquila: FaceExpressions module is already running..."<<endl<<endl;
                yarp_network->connect("/face/eyelids", "/icubSim/face/eyelids");
                yarp_network->connect("/face/image/out", "/icubSim/texture");

                face->low_level_port->open("/aquila/face/raw/out");
                face->low_level_port->setStrict(true);

                yarp_network->connect("/aquila/face/raw/out", "/icubSim/face/raw/in");
            }
            yarp_network->connect("/icubSim/face/raw","/face/raw");
        }
        else
        {
            face->hi_level_port = new BufferedPort<Bottle>;
            face->low_level_port->open("/aquila/face/raw/out");
            face->hi_level_port->open("/aquila/face/emotions/out");
            face->low_level_port->setStrict(true);
            face->hi_level_port->setStrict(true);

            yarp_network->connect("/aquila/face/raw/out","/icub/face/raw/in");
            yarp_network->connect("/aquila/face/emotions/out","/icub/face/emotions/in");
        }
        face->initialised = true;
    }
    else cout<<endl<<"Aquila: adding new client to face connection, "<<"total number of conencted clients: "<<face->num_clients+1<<endl<<endl;
    face->num_clients++;
}

/**
*@brief     initialises force-torque sensors
*/
//void iCubControl::initSpeech()
//{
//    if(!speech->initialised)
//    {
//        cout<<endl<<"Aquila: initialising speech port..."<<endl<<endl;
//
//        speech->port = new BufferedPort<Bottle> ;
//        speech->port->open("/aquila/speech_in");
//        Network::connect("/speech","/aquila/speech_in");
//
//        speech->initialised = true;
//    }
//    else cout<<endl<<"Aquila: adding new client to speech connection, "<<"total number of conencted clients: "<<speech->num_clients+1<<endl<<endl;
//    speech->num_clients++;
//}

/**
*@brief     de-initialises everything
*/
void iCubControl::closeAll(bool simulation)
{
    cout<<endl<<"Aquila: disconnecting ports, closing yarp network and freeing memory..."<<endl<<endl;
    closeCameras();
//    closeLeftArm();
//    closeRightArm();
//    closeLeftLeg();
//    closeRightLeg();
    closeHead();
    closeTorso();
//    closeInertialSensor(simulation);
    closeFaceExpressions(simulation);
//    closeSpeech();
//    closeWorldPort();
//    closeHandHallSensors();
//    closeForceTorqueSensors();
}

/**
*@brief     deinitialises left camera
*/
void iCubControl::closeLeftCamera() {
	if(left_camera->initialised) {
            cout<<endl<<"removing connection to left camera..."<<endl;

            if(simulation) yarp_network->disconnect("/icubSim/cam/left", left_camera->port_name.toStdString().c_str());
            else           yarp_network->disconnect("/icub/cam/left", left_camera->port_name.toStdString().c_str());

            left_camera->port->close();

            delete left_camera->image;
            left_camera->port = NULL;

            left_camera->initialised = false;
    }
}

/**
*@brief     deinitialises right camera
*/
void iCubControl::closeRightCamera() {
	if(right_camera->initialised) {
		cout<<endl<<"removing connection to rigth camera..."<<endl;
		
		if(simulation) yarp_network->disconnect("/icubSim/cam/right", right_camera->port_name.toStdString().c_str());
		else           yarp_network->disconnect("/icub/cam/right", right_camera->port_name.toStdString().c_str());
		
		right_camera->port->close();
		
		delete right_camera->image;
		right_camera->port = NULL;
		
		right_camera->initialised = false;
	}
}
/**
*@brief     deinitialises camera
*/
void iCubControl::closeCameras() {
    closeLeftCamera();
    closeRightCamera();
}

/**
*@brief     de-initialises left arm
*/
//void iCubControl::closeLeftArm()
//{
//    if(left_arm->initialised)
//    {
//        if(left_arm->num_clients == 1)
//        {
//            cout<<endl<<"Aquila: removing last client connection to left arm..."<<endl<<endl;
//
//            left_arm->device->close();
//
//            delete   left_arm->device;
//            delete   left_arm->encoders;
//            delete   left_arm->command;
//            delete[] left_arm->velocities;
//            delete[] left_arm->positions;
//            delete[] left_arm->min_limit;
//            delete[] left_arm->max_limit;
//
//            left_arm->device                 = NULL;
//            left_arm->lim                    = NULL;
//            left_arm->pos                    = NULL;
//            left_arm->encs                   = NULL;
//            left_arm->vel                    = NULL;
//            left_arm->imp                    = NULL;
//            left_arm->mode                   = NULL;
//
//            left_arm->num_clients--;
//            left_arm->initialised            = false;
//        }
//        else
//        {
//            if(left_arm->num_clients-1>1) cout<<endl<<"Aquila: removing client connection to left arm, "<<left_arm->num_clients-1<<" clients are still connected..."<<endl<<endl;
//            else                          cout<<endl<<"Aquila: removing client connection to left arm, one client is still connected..."<<endl<<endl;
//            left_arm->num_clients--;
//        }
//    }
//    else cout<<endl<<"Aquila: closing call ignored, left arm was not initialised..."<<endl<<endl;
//}
//
///**
//*@brief     de-initialises right
//*/
//void iCubControl::closeRightArm()
//{
//    if(right_arm->initialised)
//    {
//        if(right_arm->num_clients == 1)
//        {
//            cout<<endl<<"Aquila: removing last client connection to right arm..."<<endl<<endl;
//
//            right_arm->device->close();
//
//            delete   right_arm->device;
//            delete   right_arm->encoders;
//            delete   right_arm->command;
//            delete[] right_arm->velocities;
//            delete[] right_arm->positions;
//            delete[] right_arm->min_limit;
//            delete[] right_arm->max_limit;
//
//            right_arm->device                = NULL;
//            right_arm->lim                   = NULL;
//            right_arm->pos                   = NULL;
//            right_arm->encs                  = NULL;
//            right_arm->vel                   = NULL;
//            right_arm->imp                   = NULL;
//            right_arm->mode                  = NULL;
//
//            right_arm->num_clients--;
//            right_arm->initialised           = false;
//        }
//        else
//        {
//            if(right_arm->num_clients-1>1) cout<<endl<<"Aquila: removing client connection to right arm, "<<right_arm->num_clients-1<<" clients are still connected..."<<endl<<endl;
//            else                           cout<<endl<<"Aquila: removing client connection to right arm, one client is still connected..."<<endl<<endl;
//            right_arm->num_clients--;
//        }
//    }
//    else cout<<endl<<"Aquila: closing call ignored, right arm was not initialised..."<<endl<<endl;
//}
//
/**
*@brief     de-initialises left leg
*/
//void iCubControl::closeLeftLeg()
//{
//    if(left_leg->initialised)
//    {
//        if(left_leg->num_clients == 1)
//        {
//            cout<<endl<<"Aquila: removing last client connection to left leg..."<<endl<<endl;
//
//            left_leg->device->close();
//
//            delete   left_leg->device;
//            delete   left_leg->encoders;
//            delete   left_leg->command;
//            delete   left_leg->analog_port;
//            delete[] left_leg->velocities;
//            delete[] left_leg->positions;
//            delete[] left_leg->min_limit;
//            delete[] left_leg->max_limit;
//
//            left_leg->device                 = NULL;
//            left_leg->encoders               = NULL;
//            left_leg->command                = NULL;
//            left_leg->lim                    = NULL;
//            left_leg->pos                    = NULL;
//            left_leg->encs                   = NULL;
//            left_leg->vel                    = NULL;
//            left_leg->imp                    = NULL;
//            left_leg->mode                   = NULL;
//            left_leg->analog_port            = NULL;
//
//            left_leg->num_clients--;
//            left_leg->initialised            = false;
//        }
//        else
//        {
//            if(left_leg->num_clients-1>1)  cout<<endl<<"Aquila: removing client connection to left leg, "<<left_leg->num_clients-1<<" clients are still connected..."<<endl<<endl;
//            else                           cout<<endl<<"Aquila: removing client connection to left leg, one client is still connected..."<<endl<<endl;
//            left_leg->num_clients--;
//        }
//    }
//    else cout<<endl<<"Aquila: closing call ignored, left leg was not initialised..."<<endl<<endl;
//}
//
///**
//*@brief     de-initialises right leg
//*/
//void iCubControl::closeRightLeg()
//{
//    if(right_leg->initialised)
//    {
//        if(right_leg->num_clients == 1)
//        {
//            cout<<endl<<"Aquila: removing last client connection to right leg..."<<endl<<endl;
//
//            right_leg->device->close();
//
//            delete   right_leg->device;
//            delete   right_leg->encoders;
//            delete   right_leg->command;
//            delete   right_leg->analog_port;
//            delete[] right_leg->velocities;
//            delete[] right_leg->positions;
//            delete[] right_leg->min_limit;
//            delete[] right_leg->max_limit;
//
//            right_leg->device                = NULL;
//            right_leg->encoders              = NULL;
//            right_leg->command               = NULL;
//            right_leg->lim                   = NULL;
//            right_leg->pos                   = NULL;
//            right_leg->encs                  = NULL;
//            right_leg->vel                   = NULL;
//            right_leg->imp                   = NULL;
//            right_leg->mode                  = NULL;
//            right_leg->analog_port           = NULL;
//
//            right_leg->num_clients--;
//            right_leg->initialised           = false;
//        }
//        else
//        {
//            if(right_leg->num_clients-1>1) cout<<endl<<"Aquila: removing client connection to right leg, "<<right_leg->num_clients-1<<" clients are still connected..."<<endl<<endl;
//            else                           cout<<endl<<"Aquila: removing client connection to right leg, one client is still connected..."<<endl<<endl;
//            right_leg->num_clients--;
//        }
//    }
//    else cout<<endl<<"Aquila: closing call ignored, right leg was not initialised..."<<endl<<endl;
//}

/**
*@brief     de-initialises head
*/
void iCubControl::closeHead()
{
    if(head->initialised)
    {
        if(head->num_clients == 1)
        {
            cout<<endl<<"Aquila: removing last client connection to head..."<<endl<<endl;

            head->device->close();

            delete   head->device;
            delete   head->encoders;
            delete   head->command;
            delete[] head->velocities;
            delete[] head->positions;
            delete[] head->min_limit;
            delete[] head->max_limit;

            head->device     = NULL;
            head->encoders   = NULL;
            head->command    = NULL;
            head->lim        = NULL;
            head->pos        = NULL;
            head->encs       = NULL;
            head->vel        = NULL;
            head->mode       = NULL;

            head->num_clients--;
            head->initialised = false;
        }
        else
        {
            if(head->num_clients-1>1) cout<<endl<<"Aquila: removing client connection to head, "<<head->num_clients-1<<" clients are still connected..."<<endl<<endl;
            else                      cout<<endl<<"Aquila: removing client connection to head, one client is still connected..."<<endl<<endl;
            head->num_clients--;
        }
    }
    else cout<<endl<<"Aquila: closing call ignored, head was not initialised..."<<endl<<endl;
}

/**
*@brief     de-initialises torso
*/
void iCubControl::closeTorso()
{
    if(torso->initialised)
    {
        if(torso->num_clients == 1)
        {
            cout<<endl<<"Aquila: removing last client connection to torso..."<<endl<<endl;

            torso->device->close();

            delete   torso->device;
            delete   torso->encoders;
            delete   torso->command;
            delete[] torso->velocities;
            delete[] torso->positions;
            delete[] torso->min_limit;
            delete[] torso->max_limit;

            torso->device        = NULL;
            torso->encoders      = NULL;
            torso->command       = NULL;
            torso->lim           = NULL;
            torso->pos           = NULL;
            torso->vel           = NULL;
            torso->mode          = NULL;
            torso->encs          = NULL;

            torso->num_clients--;
            torso->initialised   = false;
        }
        else
        {
            if(torso->num_clients-1>1) cout<<endl<<"Aquila: removing client connection to torso, "<<torso->num_clients-1<<" clients are still connected..."<<endl<<endl;
            else                       cout<<endl<<"Aquila: removing client connection to torso, one client is still connected..."<<endl<<endl;
            torso->num_clients--;
        }
    }
    else cout<<endl<<"Aquila: closing call ignored, head was not initialised..."<<endl<<endl;
}

/**
*@brief     de-initialises inertial sensor
*/
//void iCubControl::closeInertialSensor(bool simulation)
//{
//    if(inertial_sensor->initialised)
//    {
//        if(inertial_sensor->num_clients == 1)
//        {
//            cout<<endl<<"Aquila: removing last client connection to inertial sensor..."<<endl<<endl;
//
//            if(simulation) manager->network->disconnect("/icubSim/inertial","/aquila/inertial");
//            else manager->network->disconnect("/icub/inertial", "/aquila/inertial");
//            //inertial_sensor->port->close();
//
//            delete inertial_sensor->port;
//            inertial_sensor->port = NULL;
//
//            inertial_sensor->num_clients--;
//            inertial_sensor->initialised = false;
//        }
//        else
//        {
//            if(inertial_sensor->num_clients-1>1) cout<<endl<<"Aquila: removing client connection to inertial sensor, "<<inertial_sensor->num_clients-1<<" clients are still connected..."<<endl<<endl;
//            else                                 cout<<endl<<"Aquila: removing client connection to inertial sensor, one client is still connected..."<<endl<<endl;
//            inertial_sensor->num_clients--;
//        }
//    }
//    else cout<<endl<<"Aquila: closing call ignored, inertial sensor was not initialised..."<<endl<<endl;
//}

/**
*@brief     de-initialises face expressions
*/
void iCubControl::closeFaceExpressions(bool simulation)
{
    if(face->initialised)
    {
        if(face->num_clients == 1)
        {
            cout<<endl<<"Aquila: removing last client connection to face..."<<endl<<endl;

            if(simulation)
            {
                system("killall FaceExpressions");
                yarp_network->disconnect("/aquila/face/out", "/icubSim/face/raw/in");
                face->low_level_port->close();
            }
            else
            {
                face->low_level_port->close();
                face->hi_level_port->close();

                delete face->hi_level_port;
                face->hi_level_port = NULL;
            }

            delete face->low_level_port;
            face->low_level_port = NULL;

            face->num_clients--;
            face->initialised = false;
        }
        else
        {
            if(face->num_clients-1>1) cout<<endl<<"Aquila: removing client connection to face, "<<face->num_clients-1<<" clients are still connected..."<<endl<<endl;
            else                      cout<<endl<<"Aquila: removing client connection to face, one client is still connected..."<<endl<<endl;
            face->num_clients--;
        }
    }
    else cout<<endl<<"Aquila: closing call ignored, face was not initialised..."<<endl<<endl;
}


/**
*@brief     de-initialises left hand finger encoders
*/
//void iCubControl::closeLeftHandHallSensors()
//{
//    if(left_hand_hall_sensors->initialised)
//    {
//        if(left_hand_hall_sensors->num_clients == 1)
//        {
//            cout<<endl<<"Aquila: removing last client connection to left hand hall sensors..."<<endl<<endl;
//
//            manager->network->disconnect("/icub/left_hand/analog:o ","/aquila/left_hand/analog:i");
//
//            delete left_hand_hall_sensors->port;
//            left_hand_hall_sensors->port = NULL;
//
//            left_hand_hall_sensors->num_clients--;
//            left_hand_hall_sensors->initialised = false;
//        }
//        else
//        {
//            if(left_hand_hall_sensors->num_clients-1>1)  cout<<endl<<"Aquila: removing client connection to left hand hall sensors, "<<left_hand_hall_sensors->num_clients-1<<" clients are still connected..."<<endl<<endl;
//            else                                        cout<<endl<<"Aquila: removing client connection to left hand hall sensors, one client is still connected..."<<endl<<endl;
//            left_hand_hall_sensors->num_clients--;
//        }
//    }
//    else cout<<endl<<"Aquila: closing call ignored, left hand hall sensors were not initialised..."<<endl<<endl;
//}

/**
*@brief     de-initialises right hand finger encoders
*/
//void iCubControl::closeRightHandHallSensors()
//{
//    if(right_hand_hall_sensors->initialised)
//    {
//        if(right_hand_hall_sensors->num_clients == 1)
//        {
//            cout<<endl<<"Aquila: removing last client connection to right hand hall sensors..."<<endl<<endl;
//
//            manager->network->disconnect("/icub/right_hand/analog:o ","/aquila/right_hand/analog:i");
//
//            delete right_hand_hall_sensors->port;
//            right_hand_hall_sensors->port = NULL;
//
//            right_hand_hall_sensors->num_clients--;
//            right_hand_hall_sensors->initialised = false;
//        }
//        else
//        {
//            if(right_hand_hall_sensors->num_clients-1>1) cout<<endl<<"Aquila: removing client connection to right hand hall sensors, "<<right_hand_hall_sensors->num_clients-1<<" clients are still connected..."<<endl<<endl;
//            else                                        cout<<endl<<"Aquila: removing client connection to right hand hall sensors, one client is still connected..."<<endl<<endl;
//            right_hand_hall_sensors->num_clients--;
//        }
//    }
//    else cout<<endl<<"Aquila: closing call ignored, right hand hall sensors were not initialised..."<<endl<<endl;
//}

/**
*@brief     de-initialises finger encoders for both hands
*/
//void iCubControl::closeHandHallSensors()
//{
//    closeLeftHandHallSensors();
//    closeRightHandHallSensors();
//}

/**
*@brief     de-initialises face expressions
*/
//void iCubControl::closeSpeech()
//{
//    if(speech->initialised)
//    {
//        if(speech->num_clients == 1)
//        {
//            cout<<endl<<"Aquila: removing last client connection to speech..."<<endl<<endl;
//
//            yarp_network->disconnect("/speech","/aquila/speech_in");
//
//            speech->port->close();
//
//            delete speech->port;
//            speech->port = NULL;
//
//            speech->initialised = false;
//        }
//        else
//        {
//            if(speech->num_clients-1>1) cout<<endl<<"Aquila: removing client connection to speech, "<<speech->num_clients-1<<" clients are still connected..."<<endl<<endl;
//            else                        cout<<endl<<"Aquila: removing client connection to speech, one client is still connected..."<<endl<<endl;
//            speech->num_clients--;
//        }
//    }
//    else cout<<endl<<"Aquila: closing call ignored, speech was not initialised..."<<endl<<endl;
//}

/**
*@brief     de-initialises world port
*/
//void iCubControl::closeWorldPort()
//{
//    if(simulation->initialised)
//    {
//        if(simulation->num_clients == 1)
//        {
//            cout<<endl<<"Aquila: removing last client connection to simulation..."<<endl<<endl;
//
//            manager->network->disconnect("/aquila/world", "/icubSim/world");
//            simulation->world_port->close();
//
//            delete simulation->world_port;
//            simulation->world_port = NULL;
//
//            simulation->initialised = false;
//        }
//        else
//        {
//            if(simulation->num_clients-1>1)  cout<<endl<<"Aquila: removing client connection to simulation, "<<simulation->num_clients-1<<" clients are still connected..."<<endl<<endl;
//            else                            cout<<endl<<"Aquila: removing client connection to simulation, one client is still connected..."<<endl<<endl;
//            simulation->num_clients--;
//        }
//    }
//    else cout<<endl<<"Aquila: closing call ignored, simulation manager was not initialised..."<<endl<<endl;
//}

/**
*@brief     de-initialises force-torque sensors
*/
//void iCubControl::closeForceTorqueSensors()
//{
//    if(manager->ft_sensors_initialised)
//    {
//        if(manager->num_clients == 1)
//        {
//            cout<<endl<<"Aquila: removing last client connection to force torque sensors..."<<endl<<endl;
//
//            manager->ft_sensors_initialised             = false;
//            manager->monitor_equilibrium_differences    = false;
//
//            left_arm->impedance_mode                    = false;
//            right_arm->impedance_mode                   = false;
//            left_leg->impedance_mode                    = false;
//            right_leg->impedance_mode                   = false;
//
//            left_arm->equilibrium_recorded              = false;
//            right_arm->equilibrium_recorded             = false;
//            left_leg->equilibrium_recorded              = false;
//            right_leg->equilibrium_recorded             = false;
//
//            left_arm->equilibrium                       = 0;
//            right_arm->equilibrium                      = 0;
//            left_leg->equilibrium                       = 0;
//            right_leg->equilibrium                      = 0;
//
//            left_arm->equilibrium_threshold             = 0.25;
//            right_arm->equilibrium_threshold            = 0.25;
//            left_leg->equilibrium_threshold             = 0.25;
//            right_leg->equilibrium_threshold            = 0.25;
//
//            //wait for monitoring thread to finish
//            sleep(1);
//
//            manager->network->disconnect("/icub/left_arm/analog:o","/aquila/left_arm/analog:i");
//            manager->network->disconnect("/icub/right_arm/analog:o","/aquila/right_arm/analog:i");
//            manager->network->disconnect("/icub/left_leg/analog:o","/aquila/left_leg/analog:i");
//            manager->network->disconnect("/icub/right_leg/analog:o","/aquila/right_leg/analog:i");
//
//            left_arm->ft_analog_port->close();
//            right_arm->ft_analog_port->close();
//            left_leg->ft_analog_port->close();
//            right_leg->ft_analog_port->close();
//
//            delete left_arm->ft_analog_port;
//            delete right_arm->ft_analog_port;
//            delete left_leg->ft_analog_port;
//            delete right_leg->ft_analog_port;
//
//            left_arm->ft_analog_port  = NULL;
//            right_arm->ft_analog_port = NULL;
//            left_leg->ft_analog_port  = NULL;
//            right_leg->ft_analog_port = NULL;
//
//            manager->num_clients--;
//            terminate();
//        }
//        else
//        {
//            if(manager->num_clients-1>1)  cout<<endl<<"Aquila: removing client connection to force torque sensors, "<<manager->num_clients-1<<" clients are still connected..."<<endl<<endl;
//            else                          cout<<endl<<"Aquila: removing client connection to force torque sensors, one client is still connected..."<<endl<<endl;
//            manager->num_clients--;
//        }
//    }
//    else cout<<endl<<"Aquila: closing call ignored, force torque were not initialised..."<<endl<<endl;
//}

/**
*@brief     gets the control mode of left arm joints
*@return     mode - QStringList containing all the modes
*/
//QStringList iCubControl::getLeftArmMode()
//{
//    int ctrl_mode;
//    QStringList mode;
//    for(int i=0;i<left_arm->num_joints;i++)
//    {
//        left_arm->mode->getControlMode(i,&ctrl_mode);
//        mode.push_back(Vocab::decode(ctrl_mode).c_str());
//    }
//    return mode;
//}
//
///**
//*@brief     gets the control mode of right arm joints
//*@return     mode - QStringList containing all the modes
//*/
//QStringList iCubControl::getRightArmMode()
//{
//    int ctrl_mode;
//    QStringList mode;
//    for(int i=0;i<right_arm->num_joints;i++)
//    {
//        right_arm->mode->getControlMode(i,&ctrl_mode);
//        mode.push_back(Vocab::decode(ctrl_mode).c_str());
//    }
//    return mode;
//}

/**
*@brief     gets the control mode of left leg joints
*@return     mode - QStringList containing all the modes
*/
//QStringList iCubControl::getLeftLegMode()
//{
//    int ctrl_mode;
//    QStringList mode;
//    for(int i=0;i<left_leg->num_joints;i++)
//    {
//        left_leg->mode->getControlMode(i,&ctrl_mode);
//        mode.push_back(Vocab::decode(ctrl_mode).c_str());
//    }
//    return mode;
//}
//
/**
*@brief     gets the control mode of right leg joints
*@return     mode - QStringList containing all the modes
*/
//QStringList iCubControl::getRightLegMode()
//{
//    int ctrl_mode;
//    QStringList mode;
//    for(int i=0;i<right_leg->num_joints;i++)
//    {
//        right_leg->mode->getControlMode(i,&ctrl_mode);
//        mode.push_back(Vocab::decode(ctrl_mode).c_str());
//    }
//    return mode;
//}

/**
*@brief     gets the control mode of head joints
*@return     mode - QStringList containing all the modes
*/
QStringList iCubControl::getHeadMode()
{
    int ctrl_mode;
    QStringList mode;
    for(int i=0;i<head->num_joints;i++)
    {
        head->mode->getControlMode(i,&ctrl_mode);
        mode.push_back(Vocab::decode(ctrl_mode).c_str());
    }
    return mode;
}

/**
*@brief     gets the control mode of torso joints
*@return     mode - QStringList containing all the modes
*/
QStringList iCubControl::getTorsoMode()
{
    int ctrl_mode;
    QStringList mode;
    for(int i=0;i<torso->num_joints;i++)
    {
        torso->mode->getControlMode(i,&ctrl_mode);
        mode.push_back(Vocab::decode(ctrl_mode).c_str());
    }
    return mode;
}

/**
*@brief     sets joint velocities for the head
*@param[in] velocities - a pointer to array holding velocities for all the joints in the head
*/
void iCubControl::headSetVelocity(const double *velocities)
{
    if(head->initialised) head->pos->setRefSpeeds(velocities);
    else cout<<"Aquila: ERROR: you are trying to set velocites of the head, which was not initialised."<<endl<<endl;
}

/**
*@brief     sets joint velocities for the left arm
*@param[in] velocities - a pointer to array holding velocities for all the joints in the left arm
*/
//void iCubControl::leftArmSetVelocity(const double *velocities)
//{
//    if(left_arm->initialised) left_arm->pos->setRefSpeeds(velocities);
//    else cout<<"Aquila: ERROR: you are trying to set velocites of the left arm, which was not initialised."<<endl<<endl;
//}

/**
*@brief     sets joint velocities for the right arm
*@param[in] velocities - a pointer to array holding velocities for all the joints in the right arm
*/
//void iCubControl::rightArmSetVelocity(const double *velocities)
//{
//    if(right_arm->initialised) right_arm->pos->setRefSpeeds(velocities);
//    else cout<<"Aquila: ERROR: you are trying to set velocites of the right arm, which was not initialised."<<endl<<endl;
//}

/**
*@brief     sets joint velocities for the left leg
*@param[in] velocities - a pointer to array holding velocities for all the joints in the left leg
*/
//void iCubControl::leftLegSetVelocity(const double *velocities)
//{
//    if(left_leg->initialised) left_leg->pos->setRefSpeeds(velocities);
//    else cout<<"Aquila: ERROR: you are trying to set velocites of the left leg, which was not initialised."<<endl<<endl;
//}

/**
*@brief     sets joint velocities for the right leg
*@param[in] velocities - a pointer to array holding velocities for all the joints in the right leg
*/
//void iCubControl::rightLegSetVelocity(const double *velocities)
//{
//    if(right_leg->initialised) right_leg->pos->setRefSpeeds(velocities);
//    else cout<<"Aquila: ERROR: you are trying to set velocites of the right leg, which was not initialised."<<endl<<endl;
//}

/**
*@brief     sets joint velocities for the torso
*@param[in] velocities - a pointer to array holding velocities for all the joints in the torso
*/
void iCubControl::torsoSetVelocity(const double *velocities)
{
    if(torso->initialised) torso->pos->setRefSpeeds(velocities);
    else cout<<"Aquila: ERROR: you are trying to set velocites of the torso, which was not initialised."<<endl<<endl;
}

/**
*@brief     sets joint velocities for the head
*@param[in] velocity - a single value setting all the joint velocities in the head to the same speed
*/
void iCubControl::headSetVelocity(double velocity)
{
    if(head->initialised) for(int i=0; i<head->num_joints; i++) head->pos->setRefSpeed(i,velocity);
    else cout<<"Aquila: ERROR: you are trying to set velocites of the head, which was not initialised."<<endl<<endl;
}

/**
*@brief     sets joint velocities for the left arm
*@param[in] velocity - a single value setting all the joint velocities in the left arm to the same speed
*/
//void iCubControl::leftArmSetVelocity(double velocity)
//{
//    if(left_arm->initialised) for(int i=0; i<left_arm->num_joints; i++) left_arm->pos->setRefSpeed(i,velocity);
//    else cout<<"Aquila: ERROR: you are trying to set velocites of the left arm, which was not initialised."<<endl<<endl;
//}

/**
*@brief     sets joint velocities for the right arm
*@param[in] velocity - a single value setting all the joint velocities in the right arm to the same speed
*/
//void iCubControl::rightArmSetVelocity(double velocity)
//{
//    if(right_arm->initialised) for(int i=0; i<right_arm->num_joints; i++) right_arm->pos->setRefSpeed(i,velocity);
//    else cout<<"Aquila: ERROR: you are trying to set velocites of the right arm, which was not initialised."<<endl<<endl;
//}

/**
*@brief     sets joint velocities for the left leg
*@param[in] velocity - a single value setting all the joint velocities in the left leg to the same speed
*/
//void iCubControl::leftLegSetVelocity(double velocity)
//{
//    if(left_leg->initialised) for(int i=0; i<left_leg->num_joints; i++) left_leg->pos->setRefSpeed(i,velocity);
//    else cout<<"Aquila: ERROR: you are trying to set velocites of the left leg, which was not initialised."<<endl<<endl;
//}

/**
*@brief     sets joint velocities for the right leg
*@param[in] velocity - a single value setting all the joint velocities in the right leg to the same speed
*/
//void iCubControl::rightLegSetVelocity(double velocity)
//{
//    if(right_leg->initialised) for(int i=0; i<right_leg->num_joints; i++) right_leg->pos->setRefSpeed(i,velocity);
//    else cout<<"Aquila: ERROR: you are trying to set velocites of the right leg, which was not initialised."<<endl<<endl;
//}

/**
*@brief     sets joint velocities for the torso
*@param[in] velocity - a single value setting all the joint velocities in the torso to the same speed
*/
void iCubControl::torsoSetVelocity(double velocity)
{
    if(torso->initialised) for(int i=0; i<torso->num_joints; i++) torso->pos->setRefSpeed(i,velocity);
    else cout<<"Aquila: ERROR: you are trying to set velocites of the torso, which was not initialised."<<endl<<endl;
}

void iCubControl::setJointVelocity(int part, int joint, double velocity)
{
    switch (part)
    {
    case 0:
        if(head->initialised) head->pos->setRefSpeed(joint, velocity);
        else cout<<"Aquila: ERROR: you are trying to set velocites of the head, which was not initialised."<<endl<<endl;
        break;
    case 1:
//        if(left_arm->initialised) left_arm->pos->setRefSpeed(joint, velocity);
//        else cout<<"Aquila: ERROR: you are trying to set velocites of the left arm, which was not initialised."<<endl<<endl;
        break;
    case 2:
//        if(right_arm->initialised) right_arm->pos->setRefSpeed(joint, velocity);
//        else cout<<"Aquila: ERROR: you are trying to set velocites of the right arm, which was not initialised."<<endl<<endl;
        break;
    case 3:
//        if(left_leg->initialised) left_leg->pos->setRefSpeed(joint, velocity);
//        else cout<<"Aquila: ERROR: you are trying to set velocites of the left leg, which was not initialised."<<endl<<endl;
        break;
    case 4:
//        if(right_leg->initialised) right_leg->pos->setRefSpeed(joint, velocity);
//        else cout<<"Aquila: ERROR: you are trying to set velocites of the right leg, which was not initialised."<<endl<<endl;
        break;
    case 5:
        if(torso->initialised) torso->pos->setRefSpeed(joint, velocity);
        else cout<<"Aquila: ERROR: you are trying to set velocites of the torso, which was not initialised."<<endl<<endl;
        break;
    }
}
void iCubControl::setJointVelocity(int jointId, double velocity)
{
//    setJointVelocity(getPartIndexFromJointId(jointId), getPartJointIndexFromJointId(jointId), velocity);
}

/**
*@brief     sets left arm impedance parameters
*@param[in] stiffness - a floating point value defining stiffness of all the joints
*@param[in] damping   - a floating point value defining damping of all the joints
*@param[in] offset    - a floating point value defining offset of all the joints
*/
//void iCubControl::leftArmSetImpedanceMode(float stiffness, float damping, float offset)
//{
//    for(int i=0;i<5;i++) left_arm->imp->setImpedance(i, stiffness, damping, offset);
//}
//
///**
//*@brief     sets right arm impedance parameters
//*@param[in] stiffness - a floating point value defining stiffness of all the joints
//*@param[in] damping   - a floating point value defining damping of all the joints
//*@param[in] offset    - a floating point value defining offset of all the joints
//*/
//void iCubControl::rightArmSetImpedanceMode(float stiffness, float damping, float offset)
//{
//    for(int i=0;i<5;i++) right_arm->imp->setImpedance(i, stiffness, damping, offset);
//}
//
///**
//*@brief     sets left leg impedance parameters
//*@param[in] stiffness - a floating point value defining stiffness of all the joints
//*@param[in] damping   - a floating point value defining damping of all the joints
//*@param[in] offset    - a floating point value defining offset of all the joints
//*/
//void iCubControl::leftLegSetImpedanceMode(float stiffness, float damping, float offset)
//{
//    for(int i=0;i<4;i++) left_leg->imp->setImpedance(i, stiffness, damping, offset);
//}

/**
*@brief     sets right leg impedance parameters
*@param[in] stiffness - a floating point value defining stiffness of all the joints
*@param[in] damping   - a floating point value defining damping of all the joints
*@param[in] offset    - a floating point value defining offset of all the joints
*/
//void iCubControl::rightLegSetImpedanceMode(float stiffness, float damping, float offset)
//{
//    for(int i=0;i<4;i++) right_leg->imp->setImpedance(i, stiffness, damping, offset);
//}

/**
*@brief     sets joint positions for the head
*@param[in] positions - a pointer to array holding positions of all the joints in the head
*/
void iCubControl::headPositionMove(const double *positions)
{
    if(head->initialised)
    {
        for(int i=0;i<head->num_joints;i++) head->command->data()[i] = positions[i];
        head->pos->positionMove(head->command->data());
    }
    else cout<<"Aquila: ERROR: you are trying to do a position move on the head, which was not initialised."<<endl<<endl;
}

/**
*@brief     sets joint positions for the left arm
*@param[in] positions - a pointer to array holding positions of all the joints in the left arm
*/
//void iCubControl::leftArmPositionMove(const double *positions)
//{
//    if(left_arm->initialised)
//    {
//        for(int i=0;i<left_arm->num_joints;i++) left_arm->command->data()[i] = positions[i];
//        left_arm->pos->positionMove(left_arm->command->data());
//    }
//    else cout<<"Aquila: ERROR: you are trying to do a position move on the left arm, which was not initialised."<<endl<<endl;
//}

/**
*@brief     sets joint positions for the right arm
*@param[in] positions - a pointer to array holding velocities for all the joints in the right arm
*/
//void iCubControl::rightArmPositionMove(const double *positions)
//{
//    if(right_arm->initialised)
//    {
//        for(int i=0;i<right_arm->num_joints;i++) right_arm->command->data()[i] = positions[i];
//        right_arm->pos->positionMove(right_arm->command->data());
//    }
//    else cout<<"Aquila: ERROR: you are trying to do a position move on the right arm, which was not initialised."<<endl<<endl;
//}

/**
*@brief     sets joint positions for the left leg
*@param[in] positions - a pointer to array holding positions of all the joints in the left leg
*/
//void iCubControl::leftLegPositionMove(const double *positions)
//{
//    if(left_leg->initialised)
//    {
//        for(int i=0;i<left_leg->num_joints;i++) left_leg->command->data()[i] = positions[i];
//        left_leg->pos->positionMove(left_leg->command->data());
//    }
//    else cout<<"Aquila: ERROR: you are trying to do a position move on the left leg, which was not initialised."<<endl<<endl;
//}

/**
*@brief     sets joint positions for the right leg
*@param[in] positions - a pointer to array holding velocities for all the joints in the right leg
*/
//void iCubControl::rightLegPositionMove(const double *positions)
//{
//    if(right_leg->initialised)
//    {
//        for(int i=0;i<right_leg->num_joints;i++) right_leg->command->data()[i] = positions[i];
//        right_leg->pos->positionMove(right_leg->command->data());
//    }
//    else cout<<"Aquila: ERROR: you are trying to do a position move on the right leg, which was not initialised."<<endl<<endl;
//}

/**
*@brief     sets joint positions for the torso
*@param[in] positions - a pointer to array holding velocities for all the joints in the torso
*/
void iCubControl::torsoPositionMove(const double *positions)
{
    if(torso->initialised)
    {
        for(int i=0;i<torso->num_joints;i++) torso->command->data()[i] = positions[i];
        torso->pos->positionMove(torso->command->data());
    }
    else cout<<"Aquila: ERROR: you are trying to do a position move on the torso, which was not initialised."<<endl<<endl;
}

void iCubControl::jointPositionMove(int part, int joint, const double position)
{
    switch (part)
    {
    case 0:
        if(head->initialised) head->pos->positionMove(joint, position);
        else cout<<"Aquila: ERROR: you are trying to set velocites of the head, which was not initialised."<<endl<<endl;
        break;
    case 1:
//        if(left_arm->initialised) left_arm->pos->positionMove(joint, position);
//        else cout<<"Aquila: ERROR: you are trying to set velocites of the left arm, which was not initialised."<<endl<<endl;
        break;
    case 2:
//        if(right_arm->initialised) right_arm->pos->positionMove(joint, position);
//        else cout<<"Aquila: ERROR: you are trying to set velocites of the right arm, which was not initialised."<<endl<<endl;
        break;
    case 3:
//        if(left_leg->initialised) left_leg->pos->positionMove(joint, position);
//        else cout<<"Aquila: ERROR: you are trying to set velocites of the left leg, which was not initialised."<<endl<<endl;
        break;
    case 4:
//        if(right_leg->initialised) right_leg->pos->positionMove(joint, position);
//        else cout<<"Aquila: ERROR: you are trying to set velocites of the right leg, which was not initialised."<<endl<<endl;
        break;
    case 5:
        if(torso->initialised) torso->pos->positionMove(joint, position);
        else cout<<"Aquila: ERROR: you are trying to set velocites of the torso, which was not initialised."<<endl<<endl;
        break;
    }
}
void iCubControl::jointPositionMove(int jointId, const double position)
{
//    jointPositionMove(getPartIndexFromJointId(jointId), getPartJointIndexFromJointId(jointId), position);
}

/**
*@brief     sets joint velocities for the head in order to reach a target position
*@param[in] positions - a pointer to array holding velocities for all the joints in the head
*@param[in] scaling_factor - velocity multiplier
*/
void iCubControl::headVelocityMove(const double *positions, double scaling_factor)
{
    if(calculateHeadDesiredVelocities(positions,scaling_factor)) head->vel->velocityMove(head->velocities);
}

/**
*@brief     sets joint velocities for the left arm in order to reach a target position
*@param[in] positions - a pointer to array holding velocities for all the joints in the left arm
*@param[in] scaling_factor - velocity multiplier
*/
//void iCubControl::leftArmVelocityMove(const double *positions, double scaling_factor)
//{
//    if(calculateLeftArmDesiredVelocities(positions,scaling_factor)) left_arm->vel->velocityMove(left_arm->velocities);
//}

/**
*@brief     sets joint velocities for the right arm in order to reach a target position
*@param[in] positions - a pointer to array holding velocities for all the joints in the right arm
*@param[in] scaling_factor - velocity multiplier
*/
//void iCubControl::rightArmVelocityMove(const double *positions, double scaling_factor)
//{
//    if(calculateRightArmDesiredVelocities(positions,scaling_factor)) right_arm->vel->velocityMove(right_arm->velocities);
//}

/**
*@brief     sets joint velocities for the left leg in order to reach a target position
*@param[in] positions - a pointer to array holding velocities for all the joints in the left leg
*@param[in] scaling_factor - velocity multiplier
*/
//void iCubControl::leftLegVelocityMove(const double *positions, double scaling_factor)
//{
//    if(calculateLeftLegDesiredVelocities(positions,scaling_factor)) left_leg->vel->velocityMove(left_leg->velocities);
//}

/**
*@brief     sets joint velocities for the right leg in order to reach a target position
*@param[in] positions - a pointer to array holding velocities for all the joints in the right leg
*@param[in] scaling_factor - velocity multiplier
*/
//void iCubControl::rightLegVelocityMove(const double *positions, double scaling_factor)
//{
//    if(calculateRightLegDesiredVelocities(positions,scaling_factor)) right_leg->vel->velocityMove(right_leg->velocities);
//}

/**
*@brief     sets joint velocities for the torso in order to reach a target position
*@param[in] positions - a pointer to array holding velocities for all the joints in the torso
*@param[in] scaling_factor - velocity multiplier
*/
void iCubControl::torsoVelocityMove(const double *positions, double scaling_factor)
{
    if(calculateTorsoDesiredVelocities(positions,scaling_factor)) torso->vel->velocityMove(torso->velocities);
}

/**
*@brief     calculates velocities for the head
*@param[in] positions - a pointer to array holding velocities for all the joints in the torso
*@param[in] scaling_factor - velocity multiplier
*@return    true if encoders returned value
*/
bool iCubControl::calculateHeadDesiredVelocities(const double *positions, double scaling_factor)
{
    for(int i=0;i<head->num_joints;i++) head->command->data()[i] = positions[i];//check if this is necessary like this!!!
    head->encs->getEncoders(head->encoders->data());

    float encoder_sum = 0;
    for(int i=0;i<head->num_joints;i++)
    {
        encoder_sum += head->encoders->data()[i];
        head->velocities[i] = (head->command->data()[i] - head->encoders->data()[i])*scaling_factor;
        if(head->velocities[i] < -30)  head->velocities[i] = -30;
        if(head->velocities[i] > 30)   head->velocities[i] = 30;
    }
    if(encoder_sum == 0.0)
    {
        cout<<"Aquila: ERROR: problems with head encoders, setting velocities to zero..."<<endl<<endl;
        for(int i=0;i<head->num_joints;i++)head->velocities[i] = 0.0;
        return false;
    }
    else return true;
}

/**
*@brief     calculates velocities for the left arm
*@param[in] positions - a pointer to array holding velocities for all the joints in the torso
*@param[in] scaling_factor - velocity multiplier
*@return    true if encoders returned value
*/
//bool iCubControl::calculateLeftArmDesiredVelocities(const double *positions, double scaling_factor)
//{
//    for(int i=0;i<left_arm->num_joints;i++) left_arm->command->data()[i] = positions[i];//check if this is necessary like this!!!
//    left_arm->encs->getEncoders(left_arm->encoders->data());
//
//    float encoder_sum = 0;
//    for(int i=0;i<left_arm->num_joints;i++)
//    {
//        encoder_sum += left_arm->encoders->data()[i];
//        left_arm->velocities[i] = (left_arm->command->data()[i] - left_arm->encoders->data()[i])*scaling_factor;
//        if(left_arm->velocities[i] < -30)  left_arm->velocities[i] = -30;
//        if(left_arm->velocities[i] > 30)   left_arm->velocities[i] = 30;
//    }
//    if(encoder_sum == 0.0)
//    {
//        cout<<"Aquila: ERROR: problems with left arm encoders, setting velocities to zero..."<<endl<<endl;
//        for(int i=0;i<left_arm->num_joints;i++)left_arm->velocities[i] = 0.0;
//        return false;
//    }
//    else return true;
//}

/**
*@brief     calculates velocities for the right arm
*@param[in] positions - a pointer to array holding velocities for all the joints in the torso
*@param[in] scaling_factor - velocity multiplier
*@return    true if encoders returned value
*/
//bool iCubControl::calculateRightArmDesiredVelocities(const double *positions, double scaling_factor)
//{
//    for(int i=0;i<right_arm->num_joints;i++) right_arm->command->data()[i] = positions[i];//check if this is necessary like this!!!
//    right_arm->encs->getEncoders(right_arm->encoders->data());
//
//    float encoder_sum = 0;
//    for(int i=0;i<right_arm->num_joints;i++)
//    {
//        encoder_sum += right_arm->encoders->data()[i];
//        right_arm->velocities[i] = (right_arm->command->data()[i] - right_arm->encoders->data()[i])*scaling_factor;
//        if(right_arm->velocities[i] < -30)  right_arm->velocities[i] = -30;
//        if(right_arm->velocities[i] > 30)   right_arm->velocities[i] = 30;
//    }
//    if(encoder_sum == 0.0)
//    {
//        cout<<"Aquila: ERROR: problems with right arm encoders, setting velocities to zero..."<<endl<<endl;
//        for(int i=0;i<right_arm->num_joints;i++)right_arm->velocities[i] = 0.0;
//        return false;
//    }
//    else return true;
//}

/**
*@brief     calculates velocities for the left leg
*@param[in] positions - a pointer to array holding velocities for all the joints in the torso
*@param[in] scaling_factor - velocity multiplier
*@return    true if encoders returned value
*/
//bool iCubControl::calculateLeftLegDesiredVelocities(const double *positions, double scaling_factor)
//{
//    for(int i=0;i<left_leg->num_joints;i++) left_leg->command->data()[i] = positions[i];//check if this is necessary like this!!!
//    left_leg->encs->getEncoders(left_leg->encoders->data());
//
//    float encoder_sum = 0;
//    for(int i=0;i<left_leg->num_joints;i++)
//    {
//        encoder_sum += left_leg->encoders->data()[i];
//        left_leg->velocities[i] = (left_leg->command->data()[i] - left_leg->encoders->data()[i])*scaling_factor;
//        if(left_leg->velocities[i] < -30)  left_leg->velocities[i] = -30;
//        if(left_leg->velocities[i] > 30)   left_leg->velocities[i] = 30;
//    }
//    if(encoder_sum == 0.0)
//    {
//        cout<<"Aquila: ERROR: problems with left leg encoders, setting velocities to zero..."<<endl<<endl;
//        for(int i=0;i<left_leg->num_joints;i++)left_leg->velocities[i] = 0.0;
//        return false;
//    }
//    else return true;
//}

/**
*@brief     calculates velocities for the right leg
*@param[in] positions - a pointer to array holding velocities for all the joints in the torso
*@param[in] scaling_factor - velocity multiplier
*@return    true if encoders returned value
*/
//bool iCubControl::calculateRightLegDesiredVelocities(const double *positions, double scaling_factor)
//{
//    for(int i=0;i<right_leg->num_joints;i++) right_leg->command->data()[i] = positions[i];//check if this is necessary like this!!!
//    right_leg->encs->getEncoders(right_leg->encoders->data());
//
//    float encoder_sum = 0;
//    for(int i=0;i<right_leg->num_joints;i++)
//    {
//        encoder_sum += right_leg->encoders->data()[i];
//        right_leg->velocities[i] = (right_leg->command->data()[i] - right_leg->encoders->data()[i])*scaling_factor;
//        if(right_leg->velocities[i] < -30)  right_leg->velocities[i] = -30;
//        if(right_leg->velocities[i] > 30)   right_leg->velocities[i] = 30;
//    }
//    if(encoder_sum == 0.0)
//    {
//        cout<<"Aquila: ERROR: problems with right leg encoders, setting velocities to zero..."<<endl<<endl;
//        for(int i=0;i<right_leg->num_joints;i++)right_leg->velocities[i] = 0.0;
//        return false;
//    }
//    else return true;
//}

/**
*@brief     calculates velocities for the torso
*@param[in] positions - a pointer to array holding velocities for all the joints in the torso
*@param[in] scaling_factor - velocity multiplier
*@return    true if encoders returned value
*/
bool iCubControl::calculateTorsoDesiredVelocities(const double *positions, double scaling_factor)
{
    for(int i=0;i<torso->num_joints;i++) torso->command->data()[i] = positions[i];//check if this is necessary like this!!!
    torso->encs->getEncoders(torso->encoders->data());

    float encoder_sum = 0;
    for(int i=0;i<torso->num_joints;i++)
    {
        encoder_sum += torso->encoders->data()[i];
        torso->velocities[i] = (torso->command->data()[i] - torso->encoders->data()[i])*scaling_factor;
        if(torso->velocities[i] < -30)  torso->velocities[i] = -30;
        if(torso->velocities[i] > 30)   torso->velocities[i] = 30;
    }
    if(encoder_sum == 0.0)
    {
        cout<<"Aquila: ERROR: problems with torso encoders, setting velocities to zero..."<<endl<<endl;
        for(int i=0;i<torso->num_joints;i++)torso->velocities[i] = 0.0;
        return false;
    }
    else return true;
}

/**
*@brief     sets joint positions for the left arm and uses impedance position control to reach them
*@param[in] positions - a pointer to array holding positions of all the joints in the left arm
*/
//void iCubControl::leftArmImpedanceMove(const double *positions)
//{
//    if(left_arm->initialised)
//    {
//        for(int i=0;i<left_arm->num_joints;i++)
//        {
//            if(i<5) left_arm->mode->setImpedancePositionMode(i);
//            left_arm->command->data()[i] = positions[i];
//        }
//        left_arm->pos->positionMove(left_arm->command->data());
//    }
//    else cout<<"Aquila: ERROR: you are trying to do a position move on the left arm, which was not initialised."<<endl<<endl;
//}

/**
*@brief     sets joint positions for the right arm and uses impedance position control to reach them
*@param[in] positions - a pointer to array holding velocities for all the joints in the right arm
*/
//void iCubControl::rightArmImpedanceMove(const double *positions)
//{
//    if(right_arm->initialised)
//    {
//        for(int i=0;i<right_arm->num_joints;i++)
//        {
//            if(i<5) right_arm->mode->setImpedancePositionMode(i);
//            right_arm->command->data()[i] = positions[i];
//        }
//        right_arm->pos->positionMove(right_arm->command->data());
//    }
//    else cout<<"Aquila: ERROR: you are trying to do a position move on the right arm, which was not initialised."<<endl<<endl;
//}

/**
*@brief     sets joint positions for the left leg and uses impedance position control to reach them
*@param[in] positions - a pointer to array holding positions of all the joints in the left leg
*/
//void iCubControl::leftLegImpedanceMove(const double *positions)
//{
//    if(left_leg->initialised)
//    {
//        for(int i=0;i<left_leg->num_joints;i++)
//        {
//            if(i<4) left_leg->mode->setImpedancePositionMode(i);
//            left_leg->command->data()[i] = positions[i];
//        }
//        left_leg->pos->positionMove(left_leg->command->data());
//    }
//    else cout<<"Aquila: ERROR: you are trying to do a position move on the left leg, which was not initialised."<<endl<<endl;
//}
//
///**
//*@brief     sets joint positions for the right leg and uses impedance position control to reach them
//*@param[in] positions - a pointer to array holding velocities for all the joints in the right leg
//*/
//void iCubControl::rightLegImpedanceMove(const double *positions)
//{
//    if(right_leg->initialised)
//    {
//        for(int i=0;i<right_leg->num_joints;i++)
//        {
//            if(i<4) right_leg->mode->setImpedancePositionMode(i);
//            right_leg->command->data()[i] = positions[i];
//        }
//        right_leg->pos->positionMove(right_leg->command->data());
//    }
//    else cout<<"Aquila: ERROR: you are trying to do a position move on the right arm, which was not initialised."<<endl<<endl;
//}
//
///**
//*@brief     record the ambient squared value of force-torqe sensors
//*/
//void iCubControl::recordLeftArmEquilibrum()
//{
//    yarp::sig::Vector *tmp;
//    tmp = left_arm->ft_analog_port->read();
//    left_arm->equilibrium = 0;
//    for(int i=0;i<6;i++) left_arm->equilibrium += tmp->data()[i];
//    left_arm->equilibrium = sqrt(left_arm->equilibrium);
//    left_arm->equilibrium_recorded = true;
//    cout<<"Aquila: left arm equilibrium calculated..."<<endl<<endl;
//}
//
///**
//*@brief     record the ambient squared value of force-torqe sensors
//*/
//void iCubControl::recordRightArmEquilibrum()
//{
//    yarp::sig::Vector *tmp;
//    tmp = right_arm->ft_analog_port->read();
//    right_arm->equilibrium = 0;
//    for(int i=0;i<6;i++) right_arm->equilibrium += tmp->data()[i];
//    right_arm->equilibrium = sqrt(right_arm->equilibrium);
//    right_arm->equilibrium_recorded = true;
//    cout<<"Aquila: right arm equilibrium calculated..."<<endl<<endl;
//}
//
///**
//*@brief     record the ambient squared value of force-torqe sensors
//*/
//void iCubControl::recordLeftLegEquilibrum()
//{
//    yarp::sig::Vector *tmp;
//    tmp = left_leg->ft_analog_port->read();
//    left_leg->equilibrium = 0;
//    for(int i=0;i<4;i++) left_leg->equilibrium += tmp->data()[i];
//    left_leg->equilibrium = sqrt(left_leg->equilibrium);
//    left_leg->equilibrium_recorded = true;
//    cout<<"Aquila: left leg equilibrium calculated..."<<endl<<endl;
//}
//
///**
//*@brief     record the ambient squared value of force-torqe sensors
//*/
//void iCubControl::recordRightLegEquilibrum()
//{
//    yarp::sig::Vector *tmp;
//    tmp = right_leg->ft_analog_port->read();
//    right_leg->equilibrium = 0;
//    for(int i=0;i<4;i++) right_leg->equilibrium += tmp->data()[i];
//    right_leg->equilibrium = sqrt(right_leg->equilibrium);
//    right_leg->equilibrium_recorded = true;
//    cout<<"Aquila: right leg equilibrium calculated..."<<endl<<endl;
//}
//
///**
//*@brief     squared difference between equilibrium values of force-torque sensors and current values of the left arm
//*@return    difference - a double containing the difference between equilibrium and current reading values
//*/
//double iCubControl::getLeftArmEquilibrumDifference()
//{
//    double current_reading  = 0;
//    double difference       = 0;
//    for(int i=0;i<6;i++) current_reading += left_arm->ft_analog_port->read()->data()[i];
//    current_reading = sqrt(current_reading);
//    if(current_reading<left_arm->equilibrium) difference = sqrt(left_arm->equilibrium - current_reading);
//    else difference = sqrt(current_reading - left_arm->equilibrium);
//    return difference;
//}
//
///**
//*@brief     squared difference between equilibrium values of force-torque sensors and current values of the right arm
//*@return    difference - a double containing the difference between equilibrium and current reading values
//*/
//double iCubControl::getRightArmEquilibrumDifference()
//{
//    double current_reading  = 0;
//    double difference       = 0;
//    for(int i=0;i<6;i++) current_reading += right_arm->ft_analog_port->read()->data()[i];
//    current_reading = sqrt(current_reading);
//    if(current_reading<right_arm->equilibrium) difference = sqrt(right_arm->equilibrium - current_reading);
//    else difference = sqrt(current_reading - right_arm->equilibrium);
//    return difference;
//}

/**
*@brief     squared difference between equilibrium values of force-torque sensors and current values of the left leg
*@return    difference - a double containing the difference between equilibrium and current reading values
*/
//double iCubControl::getLeftLegEquilibrumDifference()
//{
//    double current_reading  = 0;
//    double difference       = 0;
//    for(int i=0;i<4;i++) current_reading += left_leg->ft_analog_port->read()->data()[i];
//    current_reading = sqrt(current_reading);
//    if(current_reading<left_leg->equilibrium) difference = sqrt(left_leg->equilibrium - current_reading);
//    else difference = sqrt(current_reading - left_leg->equilibrium);
//    return difference;
//}

/**
*@brief     squared difference between equilibrium values of force-torque sensors and current values of the right leg
*@return    difference - a double containing the difference between equilibrium and current reading values
*/
//double iCubControl::getRightLegEquilibrumDifference()
//{
//    double current_reading  = 0;
//    double difference       = 0;
//    for(int i=0;i<4;i++) current_reading += right_leg->ft_analog_port->read()->data()[i];
//    current_reading = sqrt(current_reading);
//    if(current_reading<right_leg->equilibrium) difference = sqrt(right_leg->equilibrium - current_reading);
//    else difference = sqrt(current_reading - right_leg->equilibrium);
//    return difference;
//}

/**
*@brief     initialises joint IDs and create unique indices
*/
//void iCubControl::initJointIDs()
//{
//    manager->total_parts             = 6;
//    manager->total_joints            = 0;
//    manager->part_joint_totals[0]    = 6;
//    manager->part_joint_totals[1]    = 16;
//    manager->part_joint_totals[2]    = 16;
//    manager->part_joint_totals[3]    = 6;
//    manager->part_joint_totals[4]    = 6;
//    manager->part_joint_totals[5]    = 3;
//
//    for (int i = 0; i < manager->total_parts; i++) manager->total_joints += manager->part_joint_totals[i];
//
//    size_t joints_memory_size = manager->total_joints * sizeof(int);
//    manager->joint_id_parts = (int *)malloc(joints_memory_size);
//    manager->joint_id_part_joints = (int *)malloc(joints_memory_size);
//
//    int joint_id = 0;
//    for (int part = 0; part < manager->total_parts; part++)
//    {
//        for (int joint = 0; joint < manager->part_joint_totals[part]; joint++)
//        {
//            manager->joint_id_parts[joint_id] = part;
//            manager->joint_id_part_joints[joint_id] = joint;
//            joint_id++;
//        }
//    }
//}

/**
*@brief     gets part index from joint ID
*@param[in] jointId
*/
//int iCubControl::getPartIndexFromJointId(int jointId)
//{
//    return manager->joint_id_parts[jointId];
//}

/**
*@brief     gets part jonit index from jonit ID
*@param[in] jointId
*/
//int iCubControl::getPartJointIndexFromJointId (int jointId)
//{
//    return manager->joint_id_part_joints[jointId];
//}

/**
*@brief     gets part ID and joint indices
*@param[in] part
*@param[in] joint
*/
//int iCubControl::getJointIdFromPartAndJointIndexes (int part, int joint)
//{
//    int total = 0;
//    for (int p = 0; p < part; p++)
//        total += manager->part_joint_totals[p];
//    return total + joint;
//}
