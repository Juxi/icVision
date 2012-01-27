__author__ = 'Varun Kompella, Leo Pape. {varun,leo}@idsia.ch'

from pybrain.rl.environments import EpisodicTask
import numpy as np
import environment
from predmodlin import predModularLinear
from pybrain.datasets import UnsupervisedDataSet

# static functions
def _discretize(value, min, max, nd):
    states = np.linspace(min, max, nd) # _discretize states
    return np.argmin(np.abs(states - value)) #discrete state index
    
class intrinsicMotivationTask(EpisodicTask):
    
    indim = 1  # length of the action vector that is send to the environment. Here we use just the action index.
    outdim = 1 # length of the observation vector produced by the environment. Here we use just one index in the Q-table.
    nDAxis = 3; # number of discrete axis positions for the sensed axes. Used both for the Q-table and the predictor.
    nDObject = 2; # number of discrete object positions for the sensed objects. Used both for the Q-table and the predictor.
    finalState = None;
    
    def __init__(self, env):
        EpisodicTask.__init__(self, env)
        self.epiLen = 20 # maximum of 20 steps in an episode
        self.count = 0 #timestep counter
    
        self.nWorldStates, self.nRobotStates = self._nStates()
        self.nStates = self.nWorldStates*self.nRobotStates 
        self.nActions = self.env.nActions
        
        # initialize
        self.lastInput = None;
        self.lastModule = -1;
        self.lastReward = 0;

        # create predictor
        nPredModules = self.nRobotStates * self.nActions
        nPredInputs = self.nWorldStates
        self.nPredTargets = 3 # first = box vertical location < 0.53, second and third are two horizontal sphere coordinates 
        self.predictor = predModularLinear(nPredModules, nPredInputs, self.nPredTargets)
        self.errDecrease = UnsupervisedDataSet(self.nPredTargets) # holds the error decrease per target
        self.maxDErr = np.zeros(self.nPredTargets)
        self.objectsMoved = np.zeros(self.nPredTargets, np.bool)
                
        # initialize predictor values at object home positions
        self.env.reset()
        obs = self.env.getSensors()
        self.predictor.initModules(self._predictionTargets(obs))

    def getReward(self):
        if self.lastInput is not None: # for some reason, reward is called twice
            # add new sample to the history
            obs = self.env.getSensors()
            target = self._predictionTargets(obs)
            self.predictor.addObservation(self.lastModule, self.lastInput, target)
            
            
            oldErr = self.predictor.errorModule(self.lastModule)
            #self.predictor.trainModule(self.lastModule)
            self.predictor.trainLastSample()
            newErr = self.predictor.errorModule(self.lastModule)
        
            dErr = oldErr-newErr
            self.maxDErr = np.maximum(self.maxDErr, dErr)
            
            scaledDErr = dErr / self.maxDErr # scale by maximum seen thus far
            scaledDErr[np.isnan(scaledDErr)] = 0;
            scaledDErr[scaledDErr < 0.1] = 0
            
            self.errDecrease.appendLinked(scaledDErr)

            reward = 0
            
            # potentially add external reward here
           
            self.lastReward = reward # no actual reward is used during an episode 
            self.lastInput = None;
        else:
            reward = self.lastReward
        
        return reward
        

    def performAction(self, action):
        # store current state and action as input to predictor
        self._storePredState(action)
        
        # the action vector is stripped and the only element is cast to integer and given to the super class.
        EpisodicTask.performAction(self, int(action[0]))


    def _predictionTargets(self, obs):
        boxToppled = 0.05 * (obs[environment.KEY_BOX_POS][1] < 0.53) # whether the block is toppled or not
        sphPos = obs[environment.KEY_SPH_POS][[0, 2]]# horizontal coordinates of the sphere
        target = np.hstack((boxToppled, sphPos))
        
        return target
            
    # index for robot pose
    def _discreteRobotState(self, obs):
        partSensed = self.env.partSensed
        dRobotState = 0; axesCount = 0
        for key in partSensed.keys():
            lims = self.env.limits[key]
            for i in xrange(len(partSensed[key])):
                value = obs[key][i]
                axis  = partSensed[key][i]
                axisInd = _discretize(value, lims[axis, 0], lims[axis, 1], self.nDAxis) # discrete state index for active axis
                dRobotState = dRobotState*self.nDAxis + axisInd
                axesCount += 1
        
        return dRobotState 


    # index for object states
    def _discreteWorldState(self, obs):
        objectSensed = self.env.objectSensed
        dWorldState = 0; objectCount = 0
        for key in objectSensed.keys():
            # for now, just determine if the object is close to its home location:
            diff = self.env.objectHomes[key] - obs[key]
            dist = np.sqrt(np.sum(np.multiply(diff, diff)))
            objectInd = int(dist > 0.01)
            dWorldState = dWorldState*self.nDObject + objectInd
            objectCount += 1
        
        return dWorldState 
        

    # temporarily store current observation and planned action for predictor
    def _storePredState(self, action):
        obs = self.env.getSensors()

        dWorldState = self._discreteWorldState(obs)
        dRobotState = self._discreteRobotState(obs)
        dAction = int(action[0])
        
        # discrete representation of objects
        objectInput = np.zeros(self.nWorldStates)
        objectInput[dWorldState] = 1.0

        # discrete representation of robot pose
        poseInput = np.zeros(self.nRobotStates)
        poseInput[dRobotState] = 1.0

        # discrete action the robot intends to perform
        actionInput = np.zeros(self.nActions)
        actionInput[dAction] = 1.0
        
        # concatenate input
        #catInput = np.hstack((poseInput, actionInput, objectInput))
        catInput = objectInput
        self.lastInput = catInput;
        #self.lastModule = dRobotState
        self.lastModule = dAction + self.nActions*dRobotState
        

    def _nStates(self):
        # compute the number of states 
        partSensed = self.env.partSensed
        objectSensed = self.env.objectSensed
        objectCount = len(objectSensed)
        nWorldStates = self.nDObject**objectCount;
        
        axesCount = 0
        for key in partSensed.keys():
            axesCount += len(partSensed[key])
        nRobotStates = self.nDAxis**axesCount;
        
        return nWorldStates, nRobotStates
    
    
    # return RL state
    def getObservation(self):
        obs = self.env.getSensors()

        # index for object states
        dWorldState = self._discreteWorldState(obs)
        dRobotState = self._discreteRobotState(obs)
        
        # combine objects state and robot pose into a single state index for q-table
        qindex = dRobotState + self.nRobotStates*dWorldState  # state index in q-table
        #raw_input("Press ENTER to continue")
        return np.array([qindex])
      

    def isFinished(self):
        obs = self.env.getSensors()
        objectSensed = self.env.objectSensed
        keys = objectSensed.keys()
        for i in xrange(len(keys)):
            # determine if the object is close to its home location:
            key = keys[i]
            diff = self.env.objectHomes[key] - obs[key]
            dist = np.sqrt(np.sum(np.multiply(diff, diff)))
            self.objectsMoved[i] = (dist > 0.01) 
        
        moved = np.any(self.objectsMoved)
        
        # sometimes the object falls to the ground on reset, and the episode finishes immediately before the agent can take any steps. This causes errors in PyBrain, so we make sure that the agent does at least one step.
        if ((self.count > self.epiLen) or moved) and (self.count > 0):
            self.finalState = self.getObservation()
            self.res()
            return True
        else:
            self.count += 1
            return False
        
      
    def res(self):
        self.count = 0
        self.lastInput = None