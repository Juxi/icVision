__author__ = 'Leo Pape, pape@idsia.ch'

from pybrain.rl.learners.valuebased.valuebased import ValueBasedLearner


class LinFALearner(ValueBasedLearner):

    def __init__(self):
        ValueBasedLearner.__init__(self)

    def learnOnDataset(self, module, samples, repeat=1):
        for repeat in xrange(repeat):
            for seq in samples:
                lastaction = None
                laststate = None
                lastreward = None
                
                for state, action, reward in seq:
    
                    action = int(action)
    
                    # first learning call has no last state: skip
                    if laststate is not None:
                        module._updateWeights(laststate, lastaction, lastreward, state, module._greedyAction(state))
                            
                    # move state to laststate
                    laststate = state
                    lastaction = action
                    lastreward = reward

