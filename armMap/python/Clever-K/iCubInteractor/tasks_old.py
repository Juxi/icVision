__author__ = 'Varun Kompella, Leo Pape. {varun,leo}@idsia.ch'

from pybrain.rl.environments import EpisodicTask
import numpy as np
import environment
import matplotlib.mlab as mlab

class intrinsicMotivationTask(EpisodicTask):
    
    indim = 1  # length of the action vector that is send to the environment. Here we use just the action index.
    outdim = 1 # length of the observation vector produced by the environment. Here we use just one index in the Q-table.

    def __init__(self, env):
        EpisodicTask.__init__(self, env)
        self.epiLen = 25 # maximum of 20 steps in an episode
        self.count = 0 #timestep counter
        self.epsFinTrshd = 0.6635
        self.predictor = np.zeros([15,4])
        self.predictionError = np.zeros([15,4])
        self.dPredictionError = np.zeros([15,4])
        self.beta = 0.2
        self.Obs = 0
 
    def _initpredictor(self, xl, yl, means):
        '''Not Used'''
        x = np.arange(0,xl)
        y = np.arange(0,yl)
        X, Y = np.meshgrid(x, y)
        Z = np.zeros([yl,xl])
        for i in xrange(means.shape[0]):
            Z += -mlab.bivariate_normal(X, Y, 1, 1, means[i,0], means[i,1])
        return Z.T
 
    def _predictorUpdate(self, ydObs):
        
        cst = self.env.St.copy()
        at = self.env.At
        sep = ' '
        
        njs = [3, 5] # discretize into njs values
        qindex = 0
        lims = self.env.limits[environment.KEY_RIGHT_ARM]
        for i in xrange(len(self.env.activeMotor)):
            activeMotor = self.env.activeMotor[i] # the active motor
            jd = self.discretize(cst[i], lims[activeMotor, 0], lims[activeMotor, 1], njs[i]) # discrete state index for active motor
            qindex = qindex*njs[i] + jd
        
        prevPred = self.predictor[qindex,at]
        prevPredError = self.predictionError[qindex,at]
        self.predictor[qindex,at] = prevPred + self.beta*(ydObs - prevPred)
        self.predictionError[qindex,at] = np.abs(ydObs - prevPred)
        #self.dPredictionError[qindex,at] = np.abs(prevPredError - self.predictionError[qindex,at])

        #return self.dPredictionError[qindex,at]
        return self.predictionError[qindex,at]
   
    def getReward(self):
        obs = self.env.getSensors()
        objectObs = (obs[environment.KEY_BOX_POS][1] < self.epsFinTrshd)
        yd = int(objectObs)
        intrinsicReward = self._predictorUpdate(yd)
        
        #compute and return a fake reward for toppling the box
        reward = float(intrinsicReward)
        
        #Extrinsic Reward Signal
        #compute and return a fake reward for toppling the box
        #reward = float(objectObs)

        #print 'reward is ' + str(reward) 
        return reward

    def performAction(self, action):
        """ The action vector is stripped and the only element is cast to integer and given
            to the super class.
        """
        EpisodicTask.performAction(self, int(action[0]))


    def discretize(self, value, min, max, nd):
        states = np.linspace(min, max, nd) # discretize states
        return np.argmin(np.abs(states - value)) #discrete state index

    def getObservation(self):
        njs = [3, 5] # discretize into njs values
        qindex = 0
        lims = self.env.limits[environment.KEY_RIGHT_ARM]
        for i in xrange(len(self.env.activeMotor)):
            activeMotor = self.env.activeMotor[i] # the active motor
            jd = self.discretize(self.Obs[environment.KEY_RIGHT_ARM][activeMotor], lims[activeMotor, 0], lims[activeMotor, 1], njs[i]) # discrete state index for active motor
            qindex = qindex*njs[i] + jd
        print qindex
        return np.array([qindex])
    
    def isFinished(self):
        self.Obs = self.env.getSensors()
        hasFallen = (self.Obs[environment.KEY_BOX_POS][1] < self.epsFinTrshd)# end episode if the object has fallen.
        # sometimes the object falls to the ground on reset, and the episode finishes immediately before the agent can take any steps. This causes errors in PyBrain, so we make sure that the agent does at least one step
        if ((self.count > self.epiLen) or hasFallen) and (self.count >= 1):
            self.res()
            print "episode is finished"
            return True
        else:
            self.count += 1
            return False
          
    def res(self):
        """ sets counter and history back, increases incremental counter """
        self.count = 0
