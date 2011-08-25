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

#ifndef ICUB_CONTROL_H
#define ICUB_CONTROL_H

#include <yarp/os/Network.h>
#include <yarp/dev/ControlBoardInterfaces.h>
#include <yarp/dev/PolyDriver.h>
#include <yarp/sig/all.h>
#include "window.h"

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;

/**
*@brief    iCub control
*/
class iCubControl : public QThread {
    public:
    iCubControl(Window *ptr_mw);
	~iCubControl();

	//pointers to objects
    Window *main_window;
	Network *yarp_network;
	bool simulation;

    //structures
    struct Camera {
        QString                             port_name;
        BufferedPort<ImageOf<PixelRgb> >    *port;
        bool                                initialised;
        ImageOf<PixelRgb>                   *image;
    };
    Camera *left_camera, *right_camera;

//    struct Arm
//    {
//        IControlLimits                  *lim;
//        IPositionControl                *pos;
//        IVelocityControl                *vel;
//        IImpedanceControl               *imp;
//        IControlMode                    *mode;
//        IEncoders                       *encs;
//        yarp::sig::Vector               *hand_encoder;
//        yarp::sig::Vector               *encoders;
//        yarp::sig::Vector               *command;
//        PolyDriver                      *device;
//        BufferedPort<yarp::sig::Vector> *ft_analog_port;
//        yarp::sig::Vector               ft_analog_encoder;
//        int                             num_joints;
//        int                             num_clients;
//        double                          equilibrium;
//        double                          *velocities;
//        double                          *positions;
//        double                          *min_limit;
//        double                          *max_limit;
//        double                          equilibrium_threshold;
//        bool                            initialised;
//        bool                            impedance_mode;
//        bool                            equilibrium_recorded;
//    };
//    Arm *left_arm, *right_arm;
//
//    struct HandHallSensors
//    {
//        BufferedPort<yarp::sig::Vector> *port;
//        int                             num_clients;
//        bool                            initialised;
//    };
//    HandHallSensors *left_hand_hall_sensors, *right_hand_hall_sensors;
//
//    struct Leg
//    {
//        IControlLimits                  *lim;
//        IPositionControl                *pos;
//        IVelocityControl                *vel;
//        IImpedanceControl               *imp;
//        IControlMode                    *mode;
//        IEncoders                       *encs;
//        yarp::sig::Vector               *encoders;
//        yarp::sig::Vector               *command;
//        PolyDriver                      *device;
//        BufferedPort<yarp::sig::Vector> *ft_analog_port;
//        BufferedPort<Bottle>            *analog_port;
//        int                             num_joints;
//        int                             num_clients;
//        double                          equilibrium;
//        double                          *velocities;
//        double                          *positions;
//        double                          *min_limit;
//        double                          *max_limit;
//        double                          equilibrium_threshold;
//        bool                            initialised;
//        bool                            ft_sensors_initialised;
//        bool                            finger_encoders_initialised;
//        bool                            impedance_mode;
//        bool                            equilibrium_recorded;
//    };
//    Leg *left_leg, *right_leg;

    struct SimplePart
    {
        IControlLimits                  *lim;
        IPositionControl                *pos;
        IVelocityControl                *vel;
        IControlMode                    *mode;
        IEncoders                       *encs;
        yarp::sig::Vector               *encoders;
        yarp::sig::Vector               *command;
        PolyDriver                      *device;
        int                             num_joints;
        int                             num_clients;
        double                          *velocities;
        double                          *positions;
        double                          *min_limit;
        double                          *max_limit;
        bool                            initialised;
    };
    SimplePart *head,*torso;

//    struct InertialSensor
//    {
//        BufferedPort<yarp::sig::Vector> *port;
//        yarp::sig::Vector               *encoder;
//        int                             num_clients;
//        bool                            initialised;
//    };
//    InertialSensor *inertial_sensor;

    struct Face
    {
        Bottle                          *command;
        BufferedPort<Bottle>            *hi_level_port;
        BufferedPort<Bottle>            *low_level_port;
        int                             num_clients;
        bool                            initialised;
    };
    Face *face;

/*    struct Simulation
    {
        Bottle                          *command;
        Port                            *world_port;
        int                             num_clients;
        bool                            initialised;
    };
    Simulation *simulation;*/

//    struct Speech
//    {
//        BufferedPort<Bottle>            *port;
//        int                             num_clients;
//        bool                            initialised;
//    };
//    Speech *speech;

    //methods
    virtual void run();

    void initLeftCamera();
    void initRightCamera();
    void initCameras();
//    void initLeftArm(bool simulation);
//    void initRightArm(bool simulation);
//    void initLeftLeg(bool simulation);
//    void initRightLeg(bool simulation);
    void initHead(bool simulation);
    void initTorso(bool simulation);
//    void initInertialSensor(bool simulation);
    void initFaceExpressions(bool simulation);
//    void initWorldPort();
//    void initLeftHandEncoders();
//    void initRightHandEncoders();
//    void initHandHallSensors();
//    void initLeftHandHallSensors();
//    void initRightHandHallSensors();
//    void initForceTorqueSensors();
//    void initSpeech();
//    void initJointIDs();

    void closeAll(bool simulation);
    void closeLeftCamera();
    void closeRightCamera();
    void closeCameras();
//    void closeLeftArm();
//    void closeRightArm();
//    void closeLeftLeg();
//    void closeRightLeg();
    void closeHead();
    void closeTorso();
//    void closeInertialSensor(bool simulation);
    void closeFaceExpressions(bool simulation);
//    void closeSpeech();
//    void closeWorldPort();
//    void closeLeftHandHallSensors();
//    void closeRightHandHallSensors();
//    void closeHandHallSensors();
//    void closeForceTorqueSensors();

    QStringList getLeftArmMode();
    QStringList getRightArmMode();
    QStringList getLeftLegMode();
    QStringList getRightLegMode();
    QStringList getHeadMode();
    QStringList getTorsoMode();

    void headSetVelocity(const double *velocities);
    void torsoSetVelocity(const double *velocities);
    void headSetVelocity(double velocity);
    void torsoSetVelocity(double velocity);

    void setJointVelocity(int part, int joint, double velocity);
    void setJointVelocity(int jointId, double velocity);

    void headPositionMove(const double *positions);
    void torsoPositionMove(const double *positions);

    void jointPositionMove(int part, int joint, const double position);
    void jointPositionMove(int jointId, const double position);

    void headVelocityMove(const double *positions, double scaling_factor);
    void torsoVelocityMove(const double *positions, double scaling_factor);
    bool calculateHeadDesiredVelocities(const double *positions, double scaling_factor);
    bool calculateTorsoDesiredVelocities(const double *positions, double scaling_factor);

};

#endif // ICUB_CONTROL_H
