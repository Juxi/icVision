__author__ = 'Varun Kompella, Leo Pape. {varun,leo}@idsia.ch'

import numpy as np
from pybrain.datasets import ReinforcementDataSet
from pybrain.utilities import drawIndex
from random import choice

class TabularModel():
    
    def __init__(self, nStates, nActions):
        self.nStates = nStates
        self.nActions = nActions
        self.table = np.zeros((nStates, nActions, nStates))
        self.transCount = np.zeros((nStates, nActions, nStates)) # this should be a sparse matrix, but don't know how to do this in Python
        self.transProb  = np.zeros((nStates, nActions, nStates)) # this should be a sparse matrix, but don't know how to do this in Python
        
    # samples should be a ReinforcementDataSet
    # finalStates should be an array of length samples.getNumSequences
    def addSamples(self, samples, finalStates):
        assert (samples.getNumSequences() == len(finalStates))
        thisState = None
        countSeq = 0
        
        for seq in samples:
            thisState = None
            thisAction = None
            thisReward = None
                
            for nextState, nextAction, nextReward in seq:
                nextState = int(nextState)
                nextAction = int(nextAction)

                if thisState != None:
                    self.table[thisState, thisAction, nextState] = thisReward # simply keep track of the last reward seen thus far
                    self.transCount[thisState, thisAction, nextState] += 1

                # move nextState to oldState
                thisState = nextState
                thisAction = nextAction
                thisReward = nextReward
                
        
            # add final state (very important if the reward is received at the final state!)
            if thisState != None:
                self.table[thisState, thisAction, finalStates[countSeq]] = thisReward
                self.transCount[thisState, thisAction, finalStates[countSeq]] += 1
            countSeq += 1
        
        sumCount = np.sum(self.transCount, -1) # probabilities for each next-state should add up to one
        sumCount[sumCount == 0] = 1 # set to 1 to prevent division by 0
        self.transProb = np.divide(self.transCount, np.atleast_3d(sumCount))
      
    # generate length-2 sequences of state-action-rewards from the model
    # fix maxEpisodeSteps to 2, to make sure we don't end up in states without outgoing actions  
    def genSamples(self, nEpisodes=100, module=None, maxEpisodeSteps=2, vector=False):
        if vector:
            rlData = ReinforcementDataSet(self.nStates, 1)
        else:
            rlData = ReinforcementDataSet(1, 1)
            
        posStates = np.flatnonzero(np.sum(np.sum(self.transProb, -1), -1)) # nextState indices that have outgoing actions with nonzero probability
        if (len(posStates) == 0):
            print "Warning; no known transitions in the model"
            return rlData
        
        for iEpisode in xrange(nEpisodes): # divide data in episodes
            deadState = False
            
            # randomly select start state from states that have actions with positive transition values
            thisState = choice(posStates)
            for iStep in xrange(maxEpisodeSteps):
                posActions = np.flatnonzero(np.sum(self.transProb[thisState,:,:], -1))
                if (module != None):
                    thisAction = self.module.getMaxAction(thisState) # on-policy
                    if np.any(np.in1d(thisAction, posActions)):
                        nextState = drawIndex(self.transProb[thisState, thisAction, :])
                        thisReward = self.table[thisState, thisAction, nextState]
                    else:
                        thisAction = -1
                        nextState = -1 #choice(posStates)
                        thisReward = 0
                        deadState = True
                else:
                    if (len(posActions) == 0): # if there are no outgoing actions from thisState
                        thisAction = -1
                        nextState = -1 #choice(posStates)
                        thisReward = 0
                        deadState = True
                    else:
                        thisAction = choice(posActions) # random action
                        nextState = drawIndex(self.transProb[thisState, thisAction, :])
                        thisReward = self.table[thisState, thisAction, nextState]
                        
                if vector:
                    fvec = np.zeros(self.nStates)
                    fvec[thisState] = 1
                    rlData.addSample(fvec, thisAction, thisReward)
                else:
                    rlData.addSample(thisState, thisAction, thisReward)

                thisState = nextState
                
                if deadState:
                    break
                
            rlData.newSequence()
        
        return rlData
