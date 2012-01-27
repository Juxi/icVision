import sys

from PyQt4 import QtGui
from PyQt4.QtCore import QThread
from iCubInteractor import *
from pybrain.rl.learners.valuebased import ActionValueTable
from pybrain.rl.agents import LearningAgent
from pybrain.rl.learners import Q
from pybrain.rl.experiments import EpisodicExperiment
from qtplot import QtAxes


# RL is run in a separate thread, to make figure updates work
class RLThread(QThread):
    def __init__(self):
        QThread.__init__(self)
        self.widget = QtAxes()
        self.widget.show()
    
    def run(self):
        controller = ActionValueTable(20, 3)
        controller.initialize(1.) #optimistic Q values
        learner = Q()
        agent = LearningAgent(controller, learner)
        environment = iCubEnvironment('/icubSim')
        task = intrinsicMotivationTask(environment)
        experiment = EpisodicExperiment(task, agent)

        for i in xrange(100):
            print "episode " + str(i)
            experiment.doEpisodes(1)
            
            # make a copy of the e
            agent.learn()
            #agent.reset()
            self.widget.ax.pcolor(controller.params.reshape(20, 3))
            self.widget.draw()
            self.yieldCurrentThread()


if __name__ == '__main__':
    app = QtGui.QApplication(sys.argv)
    t = RLThread()
    t.start()
    sys.exit(app.exec_()) # closing the window stops the program