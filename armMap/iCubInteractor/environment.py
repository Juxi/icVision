__author__ = 'Varun Kompella, Leo Pape. {varun,leo}@idsia.ch'

from pybrain.utilities import Named
from pybrain.rl.environments.environment import Environment
import numpy as np
from numpy.linalg import norm
import yarp, time

NUM_PARTS = 4
KEY_HEAD      =  6
KEY_TORSO     =  7
KEY_LEFT_ARM  =  8
KEY_RIGHT_ARM =  9

KEY_BOX_POS   = 10
KEY_BOX_ROT   = 11

NUM_OBJECTS = 0

partNames   = dict({KEY_HEAD:'head', 
                    KEY_TORSO:'torso',
                    KEY_LEFT_ARM:'left_arm', 
                    KEY_RIGHT_ARM:'right_arm'})
allParts    = [KEY_RIGHT_ARM, KEY_LEFT_ARM, KEY_TORSO, KEY_HEAD]#[KEY_HEAD,  KEY_TORSO, KEY_LEFT_ARM, KEY_RIGHT_ARM]
resetParts  = [KEY_RIGHT_ARM, KEY_LEFT_ARM, KEY_TORSO, KEY_HEAD] # parts used in reset function
actionParts = [KEY_RIGHT_ARM, KEY_LEFT_ARM, KEY_TORSO, KEY_HEAD]
sensorParts = [KEY_RIGHT_ARM, KEY_LEFT_ARM, KEY_TORSO, KEY_HEAD]

#initialization sequence performed at the start of the experiment
initSequence1 = dict({KEY_HEAD:np.array([0, 0, 0, 0, 0, 0]),     KEY_RIGHT_ARM:np.array([0, 85, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]),          KEY_LEFT_ARM:np.array([0, 85, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]), KEY_TORSO:np.array([0, 0, 0])})
initSequence2 = dict({KEY_HEAD:np.array([-22, 0, -15, 0, 0, 0]), KEY_RIGHT_ARM:np.array([-95, 50, -1, 80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]), KEY_LEFT_ARM:np.array([0, 12, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]), KEY_TORSO:np.array([0, 0, 19])})

#part positions
partPos = dict({KEY_RIGHT_ARM: np.array([-95, 30, 0, 80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0])})

objects = dict()
#objects[0] = dict({'objectHome':'-0.15 0.665 0.39', 'objectSize':'0.003 0.3 0.003', 'objectColor':'1 0 0', 'objectOrientation':'0 0 0'})
#objects[0] = dict({'objectHome':'-0.14 0.665 0.35', 'objectSize':'0.003 0.3 0.003', 'objectColor':'1 0 0', 'objectOrientation':'0 0 0'})



refSpeed = 80 # joint rotation speed in degrees per second
timeout = 3 # time out for _waitMotionDone in seconds
error_tol = 1 # an independent implementation of checkMotionDone is used, so we need to define error tolerance


class iCubEnvironment(Environment, Named):

    #start location of the agent
    S = np.array([40,0])
    
    #State of the agent at t
    St = S.copy()
    StPlus1 = S.copy()
    # actions relative to start position
    F = np.array([1,0])*20
    B = np.array([-1,0])*20
    L = np.array([0,-1])*15
    R = np.array([0,1])*15

    allActions = [F,B,L,R]
    allActionsStr = ['F', 'B', 'L', 'R']

    #Action of the agent at t
    At = 1  
    
    # stochasticity
    stochAction = 0.
    stochObs = 0.

    activeMotor = [3,1]

    def get_collision(self):
        #result = yarp.Bottle()
        result = self.collide.read(False)
        if result == None:
            return False
        return result.toString() != "0"
        
    def get_observations(self):
        #result = yarp.Bottle()
        result = self.observations.read(False)
        if result == None:
            result = self.last_result
        else:
            self.last_result = result
        response = result.toString()
        elements = response.split(')')
        parts = {}
        for e in elements:
            if len(e.strip()) == 0: continue
            info = [i for i in e[1:].split(' ') if len(i.strip())]
            part = info[0]
            values = [float(i) for i in info[1:]]
            parts[part] = values
        return parts
            
    def get_hand_orientation(self):
        observations = self.get_observations()
        
        if not observations:
            return None
        return np.array(observations['right_hand'][0:3] + [observations['right_hand'][6]] + [observations['right_hand'][10]])
    
    def get_hand_orientation_measure(self):
        goal_orientation = np.array([0., 0., -1., 0., 0.])
        orientation = self.get_hand_orientation()
        return np.dot(goal_orientation, orientation) / norm(goal_orientation) / norm(orientation)
    
    def __init__(self, remote_port, virtual_skin = True):
        yarp.Network.init()    #initialize Yarp Network
        self.virtual_skin = virtual_skin
        
        # create world driver
        self.world = yarp.Port()
        
        self.world.open('/iCubInteractor/world')
        yarp.Network.connect('/iCubInteractor/world', str(remote_port + "/world"))

        
        self.drivers = dict(); self.iPosCtrls = dict(); self.iEncs = dict(); 
        self.iLims = dict(); self.limits = dict(); self.naxes = dict();

        # create polydrivers for parts in init_parts
        for ipart in allParts:
            options = yarp.Property()
            options.put("device", "remote_controlboard")
            options.put("local", "/iCubInteractorF/" + partNames[ipart])
            options.put("remote", str(remote_port + "/" + partNames[ipart]))
            self.drivers[ipart]   = yarp.PolyDriver(options) # make sure the drivers don't go out of scope
            self.iPosCtrls[ipart] = self.drivers[ipart].viewIPositionControl()
            self.iEncs[ipart]     = self.drivers[ipart].viewIEncoders()
            self.iLims[ipart]     = self.drivers[ipart].viewIControlLimits()
            self.naxes[ipart]     = self.iPosCtrls[ipart].getAxes()
            self.limits[ipart]    = self._getControlLimits(ipart)
        
        #if initialize_flag:
        self._initIcub()
        self._initWorld()
        self._modifyControlLimits()



    def _initWorld(self):
        sep = ' '
        # delete all existing objects
        query = yarp.Bottle(); answer = yarp.Bottle()
        query.fromString('world del all')
        self.world.write(query, answer)

        if self.virtual_skin:

            
#            self.observations = yarp.Port()
#            self.observations.open('/iCubInteractor/observe')
#            yarp.Network.connect('/observations', '/iCubInteractor/observe')
#            
#            self.collide = yarp.Port()
#            self.collide.open('/iCubInteractor/collisions')
#            yarp.Network.connect('/collisions', '/iCubInteractor/collisions')
#            
            self.observations = yarp.BufferedPortBottle()
            self.observations.open('/iCubInteractor/observe')
            yarp.Network.connect('/observations', '/iCubInteractor/observe')
            
            self.collide = yarp.BufferedPortBottle()
            self.collide.open('/iCubInteractor/collisions')
            yarp.Network.connect('/collisions', '/iCubInteractor/collisions')
            

#        # throw away the cube and the ball
#        query.fromString('world set cube ' + str(10) + sep + str(0.52) + sep + str(0.34))
#        self.world.write(query, answer)
#        query.fromString('world set ball ' + str(-10) + sep + str(0.55) + sep + str(0.34))
#        self.world.write(query, answer)


        # add a new box to the simulator world
        for object in xrange(NUM_OBJECTS):
            query.fromString('world mk box ' + objects[object]['objectSize'] + sep + objects[object]['objectHome'] + sep + objects[object]['objectColor'])
            self.world.write(query, answer)


    def _getControlLimits(self, key):
        minp = yarp.DVector(1)
        maxp = yarp.DVector(1)
        lims = np.zeros((self.naxes[key], 2))
        for i in xrange(self.naxes[key]):
            self.iLims[key].getLimits(i, minp, maxp)
            lims[i, 0] = minp[0]
            lims[i, 1] = maxp[0]
        return lims

    def _modifyControlLimits(self):
        self.limits[KEY_RIGHT_ARM][self.activeMotor[1]][1] = 60
        self.limits[KEY_RIGHT_ARM][self.activeMotor[0]][0] = 40
        self.limits[KEY_RIGHT_ARM][self.activeMotor[0]][1] = 80
        
    def _initIcub(self):
        for key in allParts:
            self.iPosCtrls[key].setRefSpeeds(self._numpyArray2DVector(np.ones(self.naxes[key]) * refSpeed))

        self._moveParts(allParts, initSequence1)
        self._moveParts(allParts, initSequence2)
 

    def _moveParts(self, keys, poss, wait=True):
        for key in keys: # first send all move commands
            self.iPosCtrls[key].positionMove(self._numpyArray2DVector(poss[key]))
        
        if wait:
            for key in keys: # then wait for motion to complete
                self._waitMotionDone2(key, poss[key])


    def _movePart(self, key, pos, wait=True):
        self.iPosCtrls[key].positionMove(self._numpyArray2DVector(pos))
        
        if wait:
            self._waitMotionDone2(key, pos)

    def _scalar2DVector(self, scalar):
        dVector = yarp.DVector(1)
        dVector[0] = float(scalar)
        return dVector
 
        
    def _numpyArray2DVector(self, array):
        dVector = yarp.DVector(len(array))
        for i in xrange(len(array)):
            dVector[i] = float(array[i])
        return dVector


    def _dVector2numpyArray(self, dVector):
        array = np.zeros(dVector.size())
        for i in xrange(len(array)):
            array[i] = dVector[i]
        return array


    def _waitMotionDone(self, key):
        c = yarp.BVector(1)
        c[0] = False;
        tstart = time.time()
        time.sleep(0.1) # temporary fix for the problem described at: http://robotcub-hackers.2198711.n2.nabble.com/iCub-Simulator-and-checkMotionDone-td6249329.html#a6275476
        while not c[0]:
            time.sleep(0.01)
            self.iPosCtrls[key].checkMotionDone(c)
            if (time.time() - tstart) > timeout:
                print "Warning: timeout while waiting for part move to complete"
                break

    # The YARP function checkMotionDone(BVector) is of no use here. See: http://robotcub-hackers.2198711.n2.nabble.com/iCub-Simulator-and-checkMotionDone-td6249329.html#a6275476
    # Instead we build a way around it
    def _waitMotionDone2(self, key, pos):
        dPos = yarp.DVector(self.naxes[key])
        done = False
        pos = np.minimum(np.maximum(pos, self.limits[key][:,0]), self.limits[key][:,1]) # make sure the requested position is within the joint limits 
        tstart = time.time()
        while not done:
            time.sleep(0.01)
            self.iEncs[key].getEncoders(dPos)
            done = True
            for i in xrange(self.naxes[key]):
                if np.abs(pos[i] - dPos[i]) > error_tol:
                    done = False
                    break
            if (time.time() - tstart) > timeout:
                print "Warning: timeout while waiting for part move to complete"
                break

    def _limitsCorrection(self,key,val):
        corr_val = []
        for i in xrange(len(val)):
            corr_val.append(np.minimum(np.maximum(val[i],self.limits[key][i][0],self.limits[key][i][1])))
        return corr_val    
    
    def performAction(self, action):
        pos = partPos[KEY_RIGHT_ARM]
        act = self.allActions[action]
        
        self.St = np.array([pos[self.activeMotor[0]], pos[self.activeMotor[1]]])
        self.At = action        

        njs = [3, 5] # discretize into njs values
 
        for i in xrange(len(self.activeMotor)):
            lims = self.limits[KEY_RIGHT_ARM][self.activeMotor[i]]
            CS = pos[self.activeMotor[i]] + act[i]
            p = np.minimum(np.maximum(CS,lims[0]),lims[1])
            pos[self.activeMotor[i]] = self._discretize(p, self.limits[KEY_RIGHT_ARM][self.activeMotor[i]][0], self.limits[KEY_RIGHT_ARM][self.activeMotor[i]][1], njs[i]) 
            
        self._movePart(KEY_RIGHT_ARM, pos)
        self.StPlus1 = np.array([pos[self.activeMotor[0]], pos[self.activeMotor[1]]])
        #print 'The current state is ' + str(self.St) + ' and performed action ' + self.allActionsStr[action]  

    def getSensors(self):
        obs = dict()
        
        # query location of sensor parts
        for key in sensorParts:
            dPos = yarp.DVector(self.naxes[key])
            self.iEncs[key].getEncoders(dPos)
            obs[key] = self._dVector2numpyArray(dPos)
            
        # query location of the box
        npos = 3; # number of coordinates
        query = yarp.Bottle(); answer = yarp.Bottle()
        objind = dict({0:KEY_BOX_POS})
        
        
        for object in xrange(NUM_OBJECTS):
            query.fromString('world get box ' + str(object+1))
            self.world.write(query, answer)
            pos = np.zeros(npos)
            for i in xrange(npos):
                pos[i] = float(answer.toString().split()[i])  # parse the string in the bottle into three coordinates
            obs[objind[object]] = pos 
        return obs

    def _discretize(self, value, min, max, nd):
        states = np.linspace(min, max, nd) # discretize states
        return states[np.argmin(np.abs(states - value))] #discrete state index

    def reset(self):
        
        #self._moveParts(resetParts, partPos)
        
#        print 'entered reset and the current state is '
        pos = partPos[KEY_RIGHT_ARM]
        njs = [3, 5] # discretize into njs values
        for i in xrange(len(self.activeMotor)):        
            # set active motor to random start location between minimum location and start location S
            randActiveMotor = np.random.random_integers(self.S[i], self.limits[KEY_RIGHT_ARM][self.activeMotor[i]][1])
            pos[self.activeMotor[i]] = self._discretize(randActiveMotor, self.limits[KEY_RIGHT_ARM][self.activeMotor[i]][0], self.limits[KEY_RIGHT_ARM][self.activeMotor[i]][1], njs[i]) 
            
        self._movePart(KEY_RIGHT_ARM, pos)
#        print  str(pos[self.activeMotor[0]]) + ' ' + str(pos[self.activeMotor[1]])

        sep = ' '
        for object in xrange(NUM_OBJECTS):
            query = yarp.Bottle();answer = yarp.Bottle()
            
            s = 'world set box ' + str(object+1) + sep + objects[object]['objectHome']
            query.fromString(s)
            self.world.write(query, answer)
            
            s = 'world rot box ' + str(object+1) + sep + objects[object]['objectOrientation']
            query.fromString(s)
            self.world.write(query, answer)

    def getYarpTuple(self, string):
        query = yarp.Bottle(); 
        answer = yarp.Bottle()
        query.fromString(string)
        self.world.write(query, answer)
        return np.array([float(i) for i in answer.toString().split(" ") if len(i.strip())])
        
    def getHandPos(self):
        observations = self.get_observations()
        
        #print observations['right_hand'], observations['right_hand'][13:16]
        if not observations:
            return None
        return np.array(observations['right_hand'][12:15])
        
        # delete all existing objects
#        query = yarp.Bottle(); 
#        answer = yarp.Bottle()
#        query.fromString('world get rhand')
#        self.world.write(query, answer)
#        return np.array([float(i) for i in answer.toString().split(" ") if len(i.strip())])

    def getHandOrientation(self):
        # delete all existing objects
        query = yarp.Bottle(); 
        answer = yarp.Bottle()
        query.fromString('world orx rhand')
        self.world.write(query, answer)
        return np.array([float(i) for i in answer.toString().split(" ") if len(i.strip())])
        #help(answer)

    def printOrientations(self):
        print self.getYarpTuple("world orx rhand")
        print self.getYarpTuple("world ory rhand")
        print self.getYarpTuple("world orz rhand")
