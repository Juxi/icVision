__author__ = 'Varun Kompella, Leo Pape. {varun,leo}@idsia.ch'

import numpy as np

from pybrain.tools.shortcuts import buildNetwork
from pybrain.datasets import SupervisedDataSet
from pybrain.supervised.trainers import BackpropTrainer

class predModularNNet():
    nModules = 0
    inputSize = 0
    targetSize = 0
    hiddenLayerSize = 0
    maxTrainSamples = 10 # maximum number of samples to train on per module
    learningrate = 0.01
    
    def __init__(self, nModules, inputSize, targetSize):
        self.nModules = nModules
        self.inputSize = inputSize
        self.targetSize = targetSize
        
        # initialize predictor
        self.pred = [None] * nModules
        for module in xrange(nModules):
            if self.hiddenLayerSize == 0:
                self.pred[module] = buildNetwork(inputSize, targetSize)
            else:
                self.pred[module] = buildNetwork(inputSize, self.hiddenLayerSize, targetSize)
            

        # initialize history
        self.data = SupervisedDataSet(inputSize, targetSize) # holds the history per module
        self.moduleIndex = [] # holds the module index for each sample in the history
        
        self.count = 0 #timestep counter
        
        
    # store a new input and target in the history
    # optionally, decide to throw out part of the history
    def addObservation(self, module, input, target):
        self.moduleIndex.append(module)
        self.data.appendLinked(input, target)
        self.count += 1

    # prediction error on last sample 
    def errorLastSample(self):
        module = self.moduleIndex[-1]
        input  = self.data['input'][-1,:]
        target =  self.data['target'][-1,:]
        output = self.pred[module].activate(input)
        diff = (output - target)
        return np.sqrt(np.mean(np.multiply(diff, diff)))
    
    # prediction error on entire history
    def errorHistory(self):
        diff = np.zeros(self.count)
        for module in xrange(self.nModules):
            modInd    = np.array(self.moduleIndex) == module
            modInput  = self.data['input'][modInd,:]
            modTarget = self.data['target'][modInd,:]
            
            modData = SupervisedDataSet(self.inputSize, self.targetSize)
            modData.setField('input', modInput)
            modData.setField('target', modTarget)

            modOutput = self.pred[module].activateOnDataset(modData)
            diff[modInd] = (modOutput - modTarget)
            
        return np.sqrt(np.mean(np.mean(np.multiply(diff, diff))))
    
    # prediction error on all samples in the history for the selected module
    def errorModule(self, module):
        modInd    = (np.array(self.moduleIndex)) == module
        modInput  = self.data['input'][modInd,:]
        modTarget = self.data['target'][modInd,:]
        
        modData = SupervisedDataSet(self.inputSize, self.targetSize)
        modData.setField('input', modInput)
        modData.setField('target', modTarget)
        
        modOutput = self.pred[module].activateOnDataset(modData)
        
        diff = (modOutput - modTarget)
        return np.sqrt(np.mean(np.mean(np.multiply(diff, diff))))
    
    # returns predicted history
    def outputHistory(self):
        output = np.zeros((self.count, self.targetSize))
        for module in xrange(self.nModules):
            modInd    = np.array(self.moduleIndex) == module
            modInput  = self.data['input'][modInd,:]
            modTarget = self.data['target'][modInd,:]
            modData = SupervisedDataSet(self.inputSize, self.targetSize)
            modData.setField('input', modInput)
            modData.setField('target', modTarget)
            
            output[modInd,:] = self.pred[module].activateOnDataset(modData)
 
        #print np.hstack((output, self.data['target']))
        return output
        
    # train predictor on entire history thus far
    def trainModule(self, module):
        # select maximum maxTrainSamples for this module from the history
        modInd      = np.nonzero(np.array(self.moduleIndex) == module)[0]
        nModSamples = min(len(modInd), self.maxTrainSamples)
        #modInd = modInd[np.random.permutation(nModSamples)] # select randomly
        modInd = modInd[-nModSamples:] # select nModSamples most recent samples
        
        # create training dataset for the selected module
        modInput  = self.data['input'][modInd,:]
        modTarget = self.data['target'][modInd,:]
        modData = SupervisedDataSet(self.inputSize, self.targetSize) # holds the samples for the current module
        modData.setField('input', modInput)
        modData.setField('target', modTarget)

        # train module
        trainer = BackpropTrainer(self.pred[module], modData, learningrate=self.learningrate)
        trainer.trainEpochs(1)

    # train predictor on most recent sample
    def trainLastSample(self):
        module = self.moduleIndex[-1]
        sampleData = SupervisedDataSet(self.inputSize, self.targetSize) # holds the last sample
        sampleData.appendLinked(self.data['input'][-1,:], self.data['target'][-1,:])
        trainer = BackpropTrainer(self.pred[module], sampleData, learningrate=self.learningrate)
        #trainer.trainOnDataset(sampleData)
        trainer.trainEpochs(1)
        
    # reset history
    def clear(self):
        self.data.clear()
        self.moduleIndex = []
        self.count = 0
    