__author__ = 'Varun Kompella, Leo Pape. {varun,leo}@idsia.ch'

from pybrain.utilities import Named
from pybrain.rl.environments.environment import Environment
import numpy as np
import yarp, time

KEY_HEAD      =  6
KEY_TORSO     =  7
KEY_LEFT_ARM  =  8
KEY_RIGHT_ARM =  9
KEY_LEFT_LEG  = 10
KEY_RIGHT_LEG = 11
KEY_BOX_POS   = 12
KEY_BOX_ROT   = 13
KEY_SPH_POS   = 14


def _array2string(npArray):
        return str(npArray).replace('[','').replace(']','').replace('  ', ' ').replace('  ', ' ')

def _parsePosition(stringPos):
    npos = 3; # number of coordinates
    pos = np.zeros(npos)
    # parse the string in the bottle into three coordinates
    for i in xrange(npos):
        pos[i] = float(stringPos.split()[i]) # if you get an error here, the connection to the simulator is probably lost  
    return pos

def _scalar2DVector(scalar):
    dVector = yarp.DVector(1)
    dVector[0] = float(scalar)
    return dVector

def _numpyArray2DVector(array):
    dVector = yarp.DVector(len(array))
    for i in xrange(len(array)):
        dVector[i] = float(array[i])
    return dVector

def _dVector2numpyArray(dVector):
    array = np.zeros(dVector.size())
    for i in xrange(len(array)):
        array[i] = dVector[i]
    return array
    
class iCubEnvironment(Environment, Named):

    """ robot settings """
    refSpeed = 100 # joint rotation speed in degrees per second
    timeout = 0.5 # time out for _waitMotionDone and _waitObject in seconds
    error_tol = 1 # an independent implementation of checkMotionDone is used, so we need to define error tolerance (in degrees)

    # active parts    
    partNames = dict({ KEY_HEAD:      'head', 
                       KEY_TORSO:     'torso',
                       KEY_LEFT_ARM:  'left_arm', 
                       KEY_RIGHT_ARM: 'right_arm'})
    partKeys = partNames.keys()

    # home positions used in reset
    partResetHome  = dict({ KEY_TORSO:     np.array([-25, 0, 20]), 
                            KEY_RIGHT_ARM: np.array([-60, 0, -1, 35, 10, -12, -6, 15, 30, 5, 5, 2, 0, 5, 7, 10]) })
    partRandomInit = [ dict({ KEY_RIGHT_ARM: np.array([ [1, 0], [3, 85] ]) }),
                       dict({ KEY_RIGHT_ARM: np.array([ [1,40], [3, 85] ]) }),
                       dict({ KEY_RIGHT_ARM: np.array([ [1,80], [3, 85] ]) }),
                       #dict({ KEY_RIGHT_ARM: np.array([ [1, 0], [3,  35] ]) }),
                       dict({ KEY_RIGHT_ARM: np.array([ [1,40], [3,  35] ]) }),
                       dict({ KEY_RIGHT_ARM: np.array([ [1,80], [3,  35] ]) })
                      ]
                            
                      
    
    # custom part limits
    partLimits  = dict({ KEY_TORSO:     dict({0:[-50, 0]}), 
                         KEY_RIGHT_ARM: dict({1:[0, 80], 3:[35, 85]}) })
    
    # parts reported by getSensors; specify array of axes per body part
    #partSensed = dict({ KEY_TORSO:     np.array([0]), 
    #                    KEY_RIGHT_ARM: np.array([1, 3]) })
    partSensed = dict({ KEY_RIGHT_ARM: np.array([1, 3]) })
    
    
    """ object settings """
    # home locations of the objects
    
    objectHomes = dict({ #KEY_BOX_POS: np.array([0.08, 0.565, 0.35]),
                         KEY_BOX_POS: np.array([0.08, 0.565, 0.38]), 
                         KEY_SPH_POS: np.array([-0.2, 0.565, 0.45]) })

    # store the objects under the table during robot reset (to prevent simulator crashes when the robot hits the objects hard)
    objectTempStore = dict({ KEY_BOX_POS: np.array([0.1, 0.1, 0.1]), 
                             KEY_SPH_POS: np.array([0.2, 0.1, 0.2]) })
    
    # object make commands: [name, size/radius, location, color] 
    objectMakeCmds = dict({ KEY_BOX_POS: 'world mk box 0.01 0.1 0.01 ' + _array2string(objectTempStore[KEY_BOX_POS]) + ' 1 0 0', 
                            KEY_SPH_POS: 'world mk sph 0.05 '          + _array2string(objectTempStore[KEY_SPH_POS]) + ' 0 1 1' })

    # results of these commands are returned by getSensors
    objectGetCmds = dict({ KEY_BOX_POS: 'world get box 1', 
                           KEY_SPH_POS: 'world get sph 1' })
    
    # object values reported by getSensors;
    objectSensed = dict({ KEY_BOX_POS: np.array([0, 1, 2]), 
                          KEY_SPH_POS: np.array([0, 1, 2]) })


    # these commands are executed on reset
    objectResetCmds = dict({ KEY_BOX_POS: 'world set box 1 ' + _array2string(objectHomes[KEY_BOX_POS]), 
                             KEY_BOX_ROT: 'world rot box 1 ' + '0 0 0',
                             KEY_SPH_POS: 'world set sph 1 ' + _array2string(objectHomes[KEY_SPH_POS]) })
    
    objectTempStoreCmds = dict({ KEY_BOX_POS: 'world set box 1 ' + _array2string(objectTempStore[KEY_BOX_POS]), 
                                 KEY_SPH_POS: 'world set sph 1 ' + _array2string(objectTempStore[KEY_SPH_POS]) })
    
    
    """ RL settings """    
    # actions: [body_part axis relative_degrees]
    R_ARM_F = [[KEY_RIGHT_ARM, 1, 40]] 
    R_ARM_B = [[KEY_RIGHT_ARM, 1, -40]]
    R_ARM_U = [[KEY_RIGHT_ARM, 3,  50]]
    R_ARM_D = [[KEY_RIGHT_ARM, 3,  -50]]
    
    allActions = [R_ARM_F, R_ARM_B, R_ARM_U, R_ARM_D]
    allActionNames = ['left', 'right', 'up', 'down']
    nActions   = len(allActions)

    # stochasticity
    stochAction = 0.
    stochObs = 0.

    def __init__(self, remote_port):
        yarp.Network.init() # initialize Yarp Network

        # create world driver
        self.world = yarp.Port()
        self.world.open('/iCubInteractor/world')
        yarp.Network.connect('/iCubInteractor/world', str(remote_port + "/world"))

        self.drivers = dict(); self.iPosCtrls = dict(); self.iEncs = dict(); 
        self.iLims = dict(); self.limits = dict(); self.naxes = dict();

        # create polydrivers for parts in init_parts
        for ipart in self.partKeys:
            options = yarp.Property()
            options.put("device", "remote_controlboard")
            options.put("local", "/iCubInteractor/" + self.partNames[ipart])
            options.put("remote", str(remote_port + "/" + self.partNames[ipart]))
            self.drivers[ipart]   = yarp.PolyDriver(options) # make sure the drivers don't go out of scope
            if not self.drivers[ipart].isValid():
                raise Exception('iCubEnvironment: no connection established')
            
            self.iPosCtrls[ipart] = self.drivers[ipart].viewIPositionControl()
            self.iEncs[ipart]     = self.drivers[ipart].viewIEncoders()
            self.iLims[ipart]     = self.drivers[ipart].viewIControlLimits()
            self.naxes[ipart]     = self.iPosCtrls[ipart].getAxes()
            self.limits[ipart]    = self._getControlLimits(ipart)
        
        self._initWorld()
        self._initIcub()
    
    
    def _initWorld(self):
        sep = ' '
        # delete all existing objects
        query = yarp.Bottle(); answer = yarp.Bottle()
        query.fromString('world del all')
        self.world.write(query, answer)

        # throw away the cube and the ball
        query.fromString('world set cube ' + str(10) + sep + str(0.52) + sep + str(0.34))
        self.world.write(query, answer)
        query.fromString('world set ball ' + str(-10) + sep + str(0.55) + sep + str(0.34))
        self.world.write(query, answer)

        # create table borders
        query.fromString('world mk sbox 0.01 0.13 0.4 0.25 0.5 0.45 1 1 1') # left of robot
        self.world.write(query, answer)
        query.fromString('world mk sbox 0.01 0.13 0.4 -0.35 0.5 0.45 1 1 1') # right of robot
        self.world.write(query, answer)
        query.fromString('world mk sbox 0.7 0.13 0.01 0 0.5 0.25 1 1 1') # close to robot
        self.world.write(query, answer)
        query.fromString('world mk sbox 0.7 0.13 0.01 0 0.5 0.65 1 1 1') # away from robot
        self.world.write(query, answer)

        # guiding rail
        query.fromString('world mk sbox 0.27 0.13 0.01 -0.3 0.5 0.37 1 1 1') #right, close to robot
        self.world.write(query, answer)
        query.fromString('world rot sbox 5 ' + '0 65 0')
        self.world.write(query, answer)
                       
        # add objects to the world
        for key in self.objectMakeCmds.keys():
            query.fromString(self.objectMakeCmds[key])
            self.world.write(query, answer)


    def _getControlLimits(self, key):
        minp = yarp.DVector(1)
        maxp = yarp.DVector(1)
        lims = np.zeros((self.naxes[key], 2))
        for iax in xrange(self.naxes[key]):
            self.iLims[key].getLimits(iax, minp, maxp)
            lims[iax, 0] = minp[0]
            lims[iax, 1] = maxp[0]
            if self.partLimits.has_key(key) and self.partLimits[key].has_key(iax): # custom limits
                lims[iax, 0] = max(lims[iax, 0], self.partLimits[key][iax][0])
                lims[iax, 1] = min(lims[iax, 1], self.partLimits[key][iax][1])
        
        return lims

        
    def _initIcub(self):
        # set speeds
        for key in self.partKeys:
            self.iPosCtrls[key].setRefSpeeds(_numpyArray2DVector(np.ones(self.naxes[key]) * self.refSpeed))

        #initialization sequence performed at the start of the experiment
        initSequence0 = dict({KEY_TORSO:np.array([0, 0, 0])})
        initSequence1 = dict({KEY_HEAD:np.array([  0, 0,   0, 0, 0, 0]), KEY_RIGHT_ARM:np.array([  0, 85,  0, 5,    0,   0,  0, 15, 30, 5, 5, 2, 0, 5, 7, 10]) })
        initSequence2 = dict({KEY_HEAD:np.array([7, -4, -26, -33, 0, 0]), KEY_RIGHT_ARM:np.array([-60, 81, -1, 56, -25, -12, -6, 15, 30, 5, 5, 2, 0, 5, 7, 10]), KEY_LEFT_ARM:np.array([0, 12, 0, 5, 0, 0, 0, 15, 30, 5, 5, 2, 0, 5, 7, 10]), KEY_TORSO:np.array([0, 0, 29])})

        self._moveParts(initSequence0.keys(), initSequence0, limit=False)
        self._moveParts(initSequence1.keys(), initSequence1, limit=False)
        self._moveParts(initSequence2.keys(), initSequence2, limit=False)


    def _moveParts(self, keys, poss, wait=True, limit=True):
        for key in keys: # first send all move commands
            if limit:
                poss[key] = np.minimum(np.maximum(poss[key], self.limits[key][:,0]), self.limits[key][:,1])
            self.iPosCtrls[key].positionMove(_numpyArray2DVector(poss[key]))
        
        if wait:
            for key in keys: # then wait for motion to complete
                self._waitMotionDone(key, poss[key], limit)


    def _movePart(self, key, pos, wait=True, limit=True):
        if limit:
            pos = np.minimum(np.maximum(pos, self.limits[key][:,0]), self.limits[key][:,1])
        self.iPosCtrls[key].positionMove(_numpyArray2DVector(pos))
        
        if wait:
            self._waitMotionDone(key, pos, limit)


    """
    def _waitMotionDone_old(self, key):
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
    """


    # The YARP function checkMotionDone(BVector) is of no use here. See: http://robotcub-hackers.2198711.n2.nabble.com/iCub-Simulator-and-checkMotionDone-td6249329.html#a6275476
    # Instead we build a way around it
    def _waitMotionDone(self, key, pos, limit=True):
        dPos = yarp.DVector(self.naxes[key])
        done = False
        if limit:
            pos = np.minimum(np.maximum(pos, self.limits[key][:,0]), self.limits[key][:,1]) # make sure the requested position is within the joint limits 
        tstart = time.time()
        while not done:
            time.sleep(0.01)
            self.iEncs[key].getEncoders(dPos)
            done = True
            for i in xrange(self.naxes[key]):
                if np.abs(pos[i] - dPos[i]) > self.error_tol:
                    done = False
                    break
            if (time.time() - tstart) > self.timeout:
                print "Warning: timeout while waiting for part move to complete"
                break

    # wait until objects stop moving
    def _waitObjects(self):
        checkMoveTime = .1
        obsStart = dict()
        obsEnd = dict()
        done = False
        query = yarp.Bottle(); answer = yarp.Bottle()
        tstart = time.time()
        while not done:
            time.sleep(0.01)
            done = True
            for key in self.objectSensed.keys():
                query.fromString(self.objectGetCmds[key])
                self.world.write(query, answer)
                pos = _parsePosition(answer.toString())
                obsStart[key] = pos[self.objectSensed[key]]
            time.sleep(checkMoveTime)
            for key in self.objectSensed.keys():
                query.fromString(self.objectGetCmds[key])
                self.world.write(query, answer)
                pos = _parsePosition(answer.toString())
                obsEnd[key] = pos[self.objectSensed[key]]
    
                diff = obsEnd[key] - obsStart[key]
                dist = np.sum(np.multiply(diff, diff))
                if dist > 1e-6:
                    done = False
            
            if (time.time() - tstart) > self.timeout:
                print "Warning: timeout while waiting for object motion stop"
                break

    def _getPosition(self, key):
        dPos = yarp.DVector(self.naxes[key])
        self.iEncs[key].getEncoders(dPos)
        return _dVector2numpyArray(dPos)
    
    
    def performAction(self, action):
        actionMoves = self.allActions[action]
        #print self.allActionNames[action]
        for im in xrange(len(actionMoves)):
            key   = actionMoves[im][0]
            motor = actionMoves[im][1]
            value = actionMoves[im][2]
            pos = self._getPosition(key)
            pos[motor] += value
            
            # add some noise to speeds
            currentSpeeds = np.ones(self.naxes[key])*self.refSpeed
            randSpeeds = currentSpeeds + 10*(np.random.rand(self.naxes[key]) - 0.5) 
            self.iPosCtrls[key].setRefSpeeds(_numpyArray2DVector(randSpeeds))

            self._movePart(key, pos) 
            
            # wait for objects to stop moving
            self._waitObjects()
        #print 'performed action ' + str(action)


    def getSensors(self):
        obs = dict()
        
        # query location of sensor parts
        for key in self.partSensed.keys():
            obs[key] = self._getPosition(key)[self.partSensed[key]]
            
        # query location of the objects
        query = yarp.Bottle(); answer = yarp.Bottle()
        for key in self.objectSensed.keys():
            query.fromString(self.objectGetCmds[key])
            self.world.write(query, answer)
            pos = _parsePosition(answer.toString())
            obs[key] = pos[self.objectSensed[key]] 

        return obs


    def reset(self):
        query = yarp.Bottle(); answer = yarp.Bottle()
        for key in self.objectTempStoreCmds.keys():
            query.fromString(self.objectTempStoreCmds[key])
            self.world.write(query, answer)
            
        nRandomInit = len(self.partRandomInit)
        iRandomInit = np.int(np.floor(np.random.rand() * nRandomInit))
        thisRandomInit = self.partRandomInit[iRandomInit]
        thisResetHome  = self.partResetHome
        for key in thisRandomInit.keys():
            thisResetHome[key][thisRandomInit[key][:,0]] = thisRandomInit[key][:,1]

        self._moveParts(thisResetHome.keys(), thisResetHome)
        
        query = yarp.Bottle(); answer = yarp.Bottle()
        for key in self.objectResetCmds.keys():
            query.fromString(self.objectResetCmds[key])
            self.world.write(query, answer)
 
        self._waitObjects()
       
        