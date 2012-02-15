
__author__ = 'Varun Kompella, Leo Pape. {varun,leo}@idsia.ch'

from iCubInteractor import *
from pybrain.rl.learners.valuebased import ActionValueTable
from pybrain.rl.agents import LearningAgent
from pybrain.rl.learners import Q
from pybrain.rl.experiments import EpisodicExperiment
from matplotlib import pyplot as plt

plt.gray()
plt.ion()

controller = ActionValueTable(15, 4)
controller.initialize(1.)
learner = Q()
learner.explorer.epsilon = 0.5
agent = LearningAgent(controller, learner)
environment = iCubEnvironment('/icubSim')
task = intrinsicMotivationTask(environment)
experiment = EpisodicExperiment(task, agent)

for i in xrange(100):
    print "episode " + str(i)
    experiment.doEpisodes(1)
    agent.learn()
    agent.reset()

    plt.clf()
    pltx = 2; plty = 3
    
    plt.subplot(pltx,plty,1)
    plt.pcolor(controller.params.reshape(15, 4))
    plt.xlim([0,4])
    plt.ylim([0,15])
    plt.title('Q-Table')
    
    plt.subplot(pltx,plty,2)
    plt.pcolor(task.prediction)
    plt.xlim([0,4])
    plt.ylim([0,15])
    plt.title('Predictor')
    
    plt.subplot(pltx,plty,3)
    plt.pcolor(controller.params.reshape(15, 4).max(1).reshape(3,5))
    plt.title('State-Value Table')
    
    plt.subplot(pltx,plty,4)
    plt.plot(task.prediction.max(1))
    plt.xlim([0,14])
    plt.ylim([-1,1])
    plt.title('State-Confidence')

    plt.subplot(pltx,plty,5)
    plt.pcolor(task.predictionError)
    plt.xlim([0,4])
    plt.ylim([0,15])
    plt.title('Prediction Error')
    
    plt.subplot(pltx,plty,6)
    plt.plot(task.predictionError.max(1))
    plt.xlim([0,14])
    plt.ylim([-1,1])
    plt.title('State-Pred Error')
    
#    plt.subplot(pltx,plty,7)
#    plt.pcolor(task.dPredictionError)
#    plt.xlim([0,4])
#    plt.ylim([0,15])
#    plt.title('Pred-Derivative')
#    
#    plt.subplot(pltx,plty,8)
#    plt.plot(task.dPredictionError.max(1))
#    plt.xlim([0,14])
#    plt.ylim([-1,1])
#    plt.title('State-DPred Error')

    plt.draw()

