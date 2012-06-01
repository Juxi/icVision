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
        self.Obs = 0
        self.obsHist = [[[] for j in range(4)] for i in range(15)]
        self.prediction = np.zeros([15,4])
        self.predictionError = np.zeros([15,4])
        self.dPredictionError = np.zeros([15,4])
        self.eta = 0.3
        
    def _predictor(self, obsHist):
        y = np.asarray(obsHist)
        x = np.arange(len(y))
        A = np.vstack([x,np.ones(len(x))]).T
        m,c = np.linalg.lstsq(A, y)[0]
        return m*(x[-1]+1) + c      
    
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
        

        prevPredError = self.predictionError[qindex,at]
        
        #compute the prediction error        
        predError = np.abs(ydObs - self.prediction[qindex,at])
        self.predictionError[qindex,at] = predError

#        if self.predictionError[qindex,at] > 0.0:
#            print "Observation: " + str(ydObs) + " Prediction: " + str(self.prediction[qindex,at]) + " obsHist: " + str(self.obsHist[qindex][at])

        #update the predictor with the new observation      
        self.obsHist[qindex][at].append(ydObs)
        self.prediction[qindex,at] = (1-self.eta)*self.prediction[qindex,at] + self.eta*self._predictor(self.obsHist[qindex][at])

        self.dPredictionError[qindex,at] = predError - prevPredError
        print self.predictionError[qindex,at], self.dPredictionError[qindex,at]
        
        return self.predictionError[qindex,at]
   
    def getReward(self):
        obs = self.env.getSensors()
        objectObs = (obs[environment.KEY_BOX_POS][1] < self.epsFinTrshd)
        yd = int(objectObs)
        if yd and (self.env.At == 2):
            yd = np.asscalar(2*np.random.rand(1))
        intrinsicReward = self._predictorUpdate(yd)
        
        #compute and return a fake reward for toppling the box
        reward = float(intrinsicReward)
        
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
