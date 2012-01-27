import sys

import numpy as np
from PyQt4 import QtGui
from PyQt4.QtCore import QThread
from iCubInteractor import *
from pybrain.rl.learners.valuebased import ActionValueTable
from pybrain.rl.agents import LearningAgent
from pybrain.rl.experiments import EpisodicExperiment
from pybrain.datasets import UnsupervisedDataSet
from qtplot import AppForm
from random import choice
from PyBrainFix import Q

def randmax(npArray):
    maxInd = np.flatnonzero(np.max(npArray) == npArray)
    return choice(maxInd)

# RL is run in a separate thread, to make figure updates work
class RLThread(QThread):
    def __init__(self):
        QThread.__init__(self)
        #self.widget = QtAxes(3, 3)
        #self.widget.show()
        self.widget = AppForm(None, 3, 3)
        self.widget.show()
        self.widget.on_draw()
        
    def init(self):
        # iCub environment
        self.environment = iCubEnvironment('/icubSim')
        
        # training modules
        self.initialQ = 0.0
        self.trainTask = intrinsicMotivationTask(self.environment)
        self.nWorldStates = self.trainTask.nWorldStates
        self.nRobotStates = self.trainTask.nRobotStates
        self.nRLStates = self.trainTask.nStates
        self.nPredStates = self.trainTask.nStates
        self.nPredTargets = self.trainTask.nPredTargets
        self.nActions = self.trainTask.nActions
        self.nRLSA = self.nRLStates * self.nActions
        self.nPredSA = self.nPredStates * self.nActions
        
        controller = ActionValueTable(self.nRLStates, self.nActions)
        controller.initialize(self.initialQ)
        self.trainAgent = LearningAgent(controller, Q())
        self.trainExp = EpisodicExperiment(self.trainTask, self.trainAgent)
        
        # initialize multiple controllers
        self.nControllers = 4
        self.controllerExplore = 0.0
        self.controllers = [None] * self.nControllers
        self.models = [None] * self.nControllers
        for iController in xrange(self.nControllers):
            self.models[iController] = TabularModel(self.nRLStates, self.nActions)
            self.controllers[iController] = ActionValueTable(self.nRLStates, self.nActions)
            self.controllers[iController].initialize(self.initialQ)
        
        # test modules
        self.testTask = intrinsicMotivationTask(self.environment)
        self.testTask.epiLen = 6
        testLearner = Q() # need learner to set exploration rate to 0
        testLearner.explorer.epsilon = 0
        self.testAgent = LearningAgent(controller, testLearner)
        self.testExp = EpisodicExperiment(self.testTask, self.testAgent)
        
        # monitoring
        self.selectedModules = UnsupervisedDataSet(1)
        self.errorModules = UnsupervisedDataSet(self.nPredTargets)
        self.errDecrModules = UnsupervisedDataSet(self.nPredTargets)
        self.skillModules = UnsupervisedDataSet(self.nPredTargets)

        
    def run(self):
        self.widget.on_draw()
        self.yieldCurrentThread()
        nController = self.nControllers-1 # start with the random controller
        pController = nController # previous controller (used for generating the policy)
        
        for iEpisode in xrange(200):
            self.widget.axes[4].set_title("Learning episode with " + str(nController))
            self.trainAgent.reset()
            #trainTask.predictor.clear() # should we clear the history of the predictor?
            self.trainTask.errDecrease.clear()
            
            print ""
            print "episode " + str(iEpisode)
            
            self.trainExp.doEpisodes(1)
            episodeHistory = self.trainAgent.history.copy()
            trainHistory = self.trainAgent.history.copy()
            untrainHistory = self.trainAgent.history.copy()
            finalState = self.trainTask.finalState
            
            # select the new controller index
            testControllers = []
            pController = nController
            allRewards = self.trainTask.errDecrease['sample']
            sumRewards = np.sum(allRewards, 0)
            nController = randmax(sumRewards)
            
            if np.sum(sumRewards) == 0: # select the random controller
                nController = self.nControllers-1
                
            if np.random.rand() < self.controllerExplore: # randomly select a controller as exploration strategy
                nController = np.floor(np.random.rand()*self.nControllers)

            # If the robot switches to a different learned controller then train with negative reward for the current module
            # Don't train the controller if the agent switches from or to the fixed, random controller
            if ((pController != (self.nControllers-1)) and (nController != (self.nControllers-1)) and (pController != nController) ):
                print "untraining module " + str(pController)
                testControllers.append(pController)
                actionAgentReward = np.atleast_2d(allRewards[:,pController]).T.copy()
                actionAgentReward[actionAgentReward >  0] = 0;
                actionAgentReward[actionAgentReward <= 0] = 0;
                untrainHistory.setField('reward', actionAgentReward)
                self.models[pController].addSamples(untrainHistory, finalState)

                # train on generated history
                self.setHistory(self.trainAgent, self.models[pController].genSamples(1000))
                self.trainAgent.learn(1)
                               
            # set selected controller to the currently training agent
            self.setController(self.trainAgent, self.controllers[nController])

            # train the selected module with the previous episode
            if nController < (self.nControllers-1): # the final module is for random exploration, so will not be trained    
                print "training module " + str(nController)
                testControllers.append(nController)
                actionAgentReward = np.atleast_2d(allRewards[:,nController]).T.copy()
                actionAgentReward[actionAgentReward >  0] = 1;
                actionAgentReward[actionAgentReward <= 0] = 0;
                trainHistory.setField('reward', actionAgentReward)
                self.models[nController].addSamples(trainHistory, finalState)
                
                # train on generated history
                self.setHistory(self.trainAgent, self.models[nController].genSamples(1000))
                self.trainAgent.learn(1)
                
            # add to monitoring
            self.errDecrModules.appendLinked(sumRewards)
            self.selectedModules.appendLinked(np.array([nController]))
            
            # test
            self.plotPredictor()

            if (iEpisode%10 == 0): # test all controllers every 10 episodes
                testControllers = np.arange(self.nControllers-1)
            self.test(np.array(testControllers))

            self.plotRLModule(episodeHistory, self.controllers[nController].params);
            self.plotEpisodes()
            self.widget.on_draw()
            self.yieldCurrentThread()
           
           
        self.widget.statusBar().showMessage("Experiment done")

    
    def plotEpisodes(self):
        self.widget.axes[6].cla()
        self.widget.axes[6].plot(self.selectedModules['sample'], 'x')
        self.widget.axes[6].plot(self.selectedModules['sample'], 'o')
        self.widget.axes[6].set_title('selected skill')    
        self.widget.axes[6].set_xlabel('episode #')
        self.widget.axes[6].set_ylim([-1, self.nControllers])
        self.widget.axes[6].set_yticks( np.arange(4) )
        self.widget.axes[6].set_yticklabels( ('topple', 'ball x', 'ball y ', 'explore') )

        self.widget.axes[7].cla()
        self.widget.axes[7].plot(self.errorModules['sample'])
        self.widget.axes[7].set_title('prediction error')    
        self.widget.axes[7].set_xlabel('episode #')
        self.widget.axes[7].legend( ('topple', 'ball x', 'ball y') )
        
        self.widget.axes[8].cla()
        self.widget.axes[8].plot(self.errDecrModules['sample'])
        self.widget.axes[8].set_xlabel('episode #')
        self.widget.axes[8].set_title('learning progress')
        self.widget.axes[8].legend( ('topple', 'ball x', 'ball y') )
    
    
    def plotPredictor(self):
        # retreive states, actions and rewards from the trainAgent:
        predData = self.trainTask.predictor.data
        predOutput = self.trainTask.predictor.outputHistory()
        
        dStateAction = np.array(self.trainTask.predictor.moduleIndex)
        dWorldState = np.argmax(predData['input'][:,0:self.nWorldStates],1)
        
        predStateInd = dStateAction + self.nRobotStates*self.nActions*dWorldState
        
        predSumTarget = np.zeros((self.nPredSA, self.nPredTargets))
        predSumOutput = np.zeros((self.nPredSA, self.nPredTargets))
        predAvgTarget = np.zeros((self.nPredSA, self.nPredTargets)) - 1
        predAvgOutput = np.zeros((self.nPredSA, self.nPredTargets)) - 1
        predStateCount = np.zeros(self.nPredSA)
        for istate in xrange(len(predStateInd)):
            predSumTarget[predStateInd[istate],:] += predData['target'][istate,:]
            predSumOutput[predStateInd[istate],:] += predOutput[istate,:]
            predStateCount[predStateInd[istate]] += 1
        
        visitedPredStates = (predStateCount != 0)
        predAvgTarget[visitedPredStates,:] = predSumTarget[visitedPredStates,:] / np.atleast_2d(predStateCount[visitedPredStates]).T
        predAvgOutput[visitedPredStates,:] = predSumOutput[visitedPredStates,:] / np.atleast_2d(predStateCount[visitedPredStates]).T
        
        self.widget.axes[1].cla()
        self.widget.axes[1].pcolor(predAvgOutput.reshape(self.nPredStates, self.nActions*self.nPredTargets), vmin=-1, vmax=1)
        self.widget.axes[1].set_title('predictor output')

        self.widget.axes[2].cla()
        self.widget.axes[2].pcolor(predAvgTarget.reshape(self.nPredStates, self.nActions*self.nPredTargets), vmin=-1, vmax=1)
        self.widget.axes[2].set_title('predictor target')
        
        predAvgErr = predAvgTarget - predAvgOutput
        self.errorModules.appendLinked(np.sqrt(np.sum(np.multiply(predAvgErr, predAvgErr), 0)))

    
    # plot Q table and reward
    def plotRLModule(self, history, qtable):
        rlStateInd = (history['action'][:,0] + history['state'][:,0]*self.nActions)
        rlSumReward   = np.zeros(self.nRLSA)
        rlAvgReward   = np.zeros(self.nRLSA)
        rlStateCount  = np.zeros(self.nRLSA)
        for istate in xrange(len(rlStateInd)):
            rlSumReward[rlStateInd[istate]]  += history['reward'][istate]
            rlStateCount[rlStateInd[istate]] += 1
        
        visitedRLStates = (rlStateCount != 0) 
        rlAvgReward[visitedRLStates] = rlSumReward[visitedRLStates] / rlStateCount[visitedRLStates]

        self.widget.axes[0].cla()
        self.widget.axes[0].pcolor(rlAvgReward.reshape(self.nRLStates, self.nActions))
        self.widget.axes[0].set_title('reward')

        # mask unchanged values        
        if np.any(qtable != self.initialQ):
            maTable = np.ma.masked_equal(qtable, self.initialQ)
        else:
            maTable = qtable
        
        self.widget.axes[3].cla()
        self.widget.axes[3].pcolor(maTable.reshape(self.nRLStates, self.nActions))
        self.widget.axes[3].set_title('Q-table')


    def setController(self, agent, controller):
        agent.module = controller
        agent.learner.module = controller
            
    def setHistory(self, agent, history):
        agent.history = history
        agent.learner.dataset = history
                
    def test(self, testControlInd):
        """ test controllers """
        testRepeat = 1
        skillLearned = np.zeros(self.nPredTargets)
        controllerObjects = [0, 1, 1] # which object is related to which controller
        for iController in xrange(self.nControllers - 1):
            if ( (np.any(testControlInd == iController)) or (len(self.skillModules['sample']) < 1) ):
                self.testAgent.module = self.controllers[iController]
                self.testAgent.learner.module = self.controllers[iController] 
                self.plotRLModule(self.trainAgent.history, self.controllers[iController].params);
    
                self.widget.axes[4].set_title("testing policy " + str(iController))
                self.widget.on_draw()
                
                for iRepeat in xrange(testRepeat):
                    self.testAgent.reset()
                    self.testTask.errDecrease.clear()
                    self.testExp.doEpisodes(1)
                    skillLearned[iController] += int(self.testTask.objectsMoved[controllerObjects[iController]])
                skillLearned[iController] = skillLearned[iController] / testRepeat
            else:
                skillLearned[iController] = self.skillModules['sample'][-1,iController]
                
        self.skillModules.addSample(skillLearned)
        
        self.widget.axes[5].cla()
        self.widget.axes[5].plot(self.skillModules['sample'])
        self.widget.axes[5].set_ylim([-0.01, 1.01])
        self.widget.axes[5].set_title('skill success')    
        self.widget.axes[5].set_xlabel('episode #')
        self.widget.axes[5].legend( ('topple', 'ball x', 'ball y') )


if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    t = RLThread()
    t.init()
    t.start()
    sys.exit(app.exec_()) # closing the window stops the program