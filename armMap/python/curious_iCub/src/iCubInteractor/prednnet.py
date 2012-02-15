__author__ = 'Varun Kompella, Leo Pape. {varun,leo}@idsia.ch'

import numpy as np

from pybrain.tools.shortcuts import buildNetwork
from pybrain.datasets import SupervisedDataSet
from pybrain.supervised.trainers import BackpropTrainer

class predNNet():
    inputSize = 0
    targetSize = 0
    hiddenLayerSize = 6
    sameThreshold = 0.01 # threshold used in errorSameSamples
     
    def __init__(self, inputSize, targetSize):
        self.inputSize = inputSize
        self.targetSize = targetSize
        
        # initialize predictor
        self.nnet = buildNetwork(inputSize, self.hiddenLayerSize, targetSize)

        # initialize history
        self.data = SupervisedDataSet(inputSize, targetSize) # holds the history
        self.count = 0 #timestep counter
    
    # store a new input and target in the history
    # optionally, it can decide to throw out part of the history
    def addObservation(self, input, target):
        self.data.appendLinked(input, target)
        self.count += 1

    # prediction error on last sample 
    def errorLastSample(self):
        predSample = self.nnet.activate(self.data['input'][self.count-1,:])
        diff = (predSample - self.data['target'][self.count-1,:])
        return np.sqrt(np.mean(np.multiply(diff, diff)))
    
    # prediction error on entire history
    def errorHistory(self):
        predHistory = self.nnet.activateOnDataset(self.data)
        diff = (predHistory - self.data['target'])
        return np.sqrt(np.mean(np.mean(np.multiply(diff, diff))))
    
    # prediction error on the same samples as <input>
    def errorSameSamples(self, input):
        #sameIndices = np.all(np.subtract(self.data['input'], input) == 0, 1)
        sameIndices = np.all(np.abs(self.data['input'] - input) < self.sameThreshold, 1)

        sameData = SupervisedDataSet(self.inputSize, self.targetSize)        
        sameData.setField('input', self.data['input'][sameIndices,:])
        sameData.setField('target', self.data['target'][sameIndices,:])
        
        predSame = self.nnet.activateOnDataset(sameData)
        diff = (predSame - sameData['target'])
        return np.sqrt(np.mean(np.mean(np.multiply(diff, diff))))
    
    
    # train predictor on entire history thus far
    def trainHistory(self):
        trainer = BackpropTrainer(self.nnet, self.data)
        trainer.trainEpochs(10)

    # train predictor on most recent sample
    def trainLastSample(self):
        dataSample = SupervisedDataSet(self.inputSize, self.targetSize) # holds the last sample
        dataSample.appendLinked(self.data['input'][self.count-1,:], self.data['target'][self.count-1,:])
        trainer = BackpropTrainer(self.nnet, learningrate=0.000001)
        trainer.trainOnDataset(dataSample)

        
    # train predictor on last sample only
#    def adaptSample(self):
#        trainer = BackpropTrainer(self.nnet, self.data)
#        trainer.adapt(self.data['input'][self.count-1,:], self.data['target'][self.count-1,:])
        
    def clear(self):
        self.data.clear()
        self.count = 0
    