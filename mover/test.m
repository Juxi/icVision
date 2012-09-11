% yarp rpc /world
% get cup1

classdef test < hgsetget

	properties (Constant)
        timeout  = 15; % wait time for general timeout
        trajTime = 5; % iKin trajectory time (s)
        waitTime = 1; % time to wait after iKin trajectory finishes (s)
        openTime = 1; % time for opening the hand (s)
        closeTime = 2; % time for closing the hand (s)
        
        LEFT_ARM = 1;
        RIGHT_ARM = 2;
        VSKIN_TARGET = 1;
        VSKIN_OBSTACLE = 2;
        
        activeArm = 1;
        otherArm = 2;
        
    end

    properties
        partNames;
        nBodyParts;
        
        portGrasp; portVSkinWorld; portMover;
       
        query; response;
    end
    
    
    methods
        function obj = controller_icub(robot)
        %CONTROLLER: constructor; initialize YARP
            currentscript = mfilename('fullpath');
            path = fileparts(fileparts(currentscript));
            jpath = fullfile(path, 'jyarp');
            %ypath = fullfile(fullfile((fileparts(path)), 'yarp'), 'yarp.jar');
            ypath = 'C:\Program Files\YARP\yarp.jar';
            addpath(jpath);
            if ~any(strcmp(jpath, javaclasspath())); javaaddpath(jpath); end
            if ~any(strcmp(ypath, javaclasspath())); javaaddpath(ypath); end
            LoadYarpICUB();
            %yarp.Drivers.factory().toString_c
            
            obj.partNames = {'right_arm'};
            obj.nBodyParts = length(obj.partNames);
            
            open(obj, robot);
            init(obj);
        end
        
        
        function open(obj, robot)
        %OPEN: create and connect drivers and ports
            
            % first close all open ports
            close(obj);
            
            fprintf(1, 'Starting and connecting drivers...\n');
            % create bottles
            obj.query = yarp.Bottle();
            obj.response = yarp.Bottle();
            
            % open world robot
            obj.portMover = yarp.Port();
            obj.portMover.open('/matlab/pathplanner');
            yarp.Network.connect('/matlab/pathplanner', '/icubSim/pathplanner');
            
            % open virtual skin world port
            obj.portVSkinStatus = yarp.BufferedPortBottle();
            obj.portVSkinStatus.open('/matlab/vskin/world');
            yarp.Network.connect('/world', '/matlab/vskin/world');
            
            % open virtual skin world port
            obj.portVSkinWorld = yarp.Port();
            obj.portVSkinWorld.open('/matlab/vskin/world');
            yarp.Network.connect('/matlab/vskin/world', ['/world']);
            
            % connect gaze controller
            gazeOptions = yarp.Property;
            gazeOptions.put('device', 'gazecontrollerclient');
            gazeOptions.put('remote', '/iKinGazeCtrl');
            gazeOptions.put('local',  '/matlab/gaze');
            obj.gazeDriver = yarp.PolyDriver(gazeOptions);
            if ~(obj.gazeDriver.isValid)
                warning('yarp:gaze_not_found', 'Could not connect to cartesian controller for gaze');
            end
            obj.gazeCtrl = obj.gazeDriver.viewIGazeControl();

            % connect grasp controller
            obj.portGrasp = yarp.Port();
            obj.portGrasp.open('/matlab/grasp');
            yarp.Network.connect('/matlab/grasp', '/graspController');

            % connect controllers for left arm and right arm
            obj.cartDrivers = cell(obj.nBodyParts, 1);
            obj.partDrivers = cell(obj.nBodyParts, 1);
            obj.cartCtrls = cell(obj.nBodyParts, 1);
            obj.posCtrls = cell(obj.nBodyParts, 1);
            obj.pidCtrls = cell(obj.nBodyParts, 1);
            obj.encoders = cell(obj.nBodyParts, 1);

            for ib = 1:obj.nBodyParts
                partname = obj.partNames{ib};

                % cartesian controls
                cartOptions = yarp.Property;
                cartOptions.put('device', 'cartesiancontrollerclient');
                cartOptions.put('local',  ['/matlab/cartesianController/' partname]);
                cartOptions.put('remote', ['/' robot '/cartesianController/' partname]);

                obj.cartDrivers{ib} = yarp.PolyDriver(cartOptions);
                if ~(obj.cartDrivers{ib}.isValid)
                    warning('yarp:cart_not_found', ['Could not connect to cartesian controller for body part ''' partname '''']);
                    continue;
                end

                obj.cartCtrls{ib} = obj.cartDrivers{ib}.viewICartesianControl();

                % position, pid and encoders
                partOptions = yarp.Property;
                partOptions.put('robot', robot);
                partOptions.put('device', 'remote_controlboard');
                partOptions.put('controlmode', 'velocity');
                partOptions.put('local', ['/matlab/' partname]);
                partOptions.put('remote', ['/' robot '/' partname]);

                obj.partDrivers{ib} = yarp.PolyDriver(partOptions);
                if ~(obj.partDrivers{ib}.isValid)
                    warning('yarp:part_not_found', ['Could not connect to body part ''' partname '''']);
                    continue;
                end
                obj.posCtrls{ib}  = obj.partDrivers{ib}.viewIPositionControl;
                obj.pidCtrls{ib}  = obj.partDrivers{ib}.viewIPidControl;
                obj.encoders{ib}  = obj.partDrivers{ib}.viewIEncoders;
            end
            fprintf('Startup finished.\n');
        end
        
        
        function close(obj)
        %CLOSE: close ports and drivers; remove objects
            stop(obj);
            fprintf(1, 'Closing controllers...');
            try obj.portVSkinWorld.close; end; %#ok<TRYNC>   
            try obj.portVSkinStatus.close; end; %#ok<TRYNC>   
            try obj.portSimWorld.close; end; %#ok<TRYNC>
            try obj.gazeDriver.close; end; %#ok<TRYNC>
            try obj.portGrasp.close(); end; %#ok<TRYNC>       
            for ib = 1:obj.nBodyParts; try obj.cartDrivers{ib}.close; end; end; %#ok<TRYNC>
            for ib = 1:obj.nBodyParts; try obj.partDrivers{ib}.close; end; end %#ok<TRYNC>
            
            if ~isempty(obj.query), try obj.query.clear(); end; end;%#ok<TRYNC>
            if ~isempty(obj.response), try obj.response.clear(); end; end; %#ok<TRYNC>
            fprintf(1, ' done.\n');
        end
        
        
        function delete(obj)
        %DELETE: destructor; close YARP ports
            close(obj);
        end