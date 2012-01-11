#!/usr/bin/env python
# encoding: utf-8
"""
learn.py
Created by Juxi Leitner on 2011-09-01.
"""

import sys
import os
import math

from pybrain.datasets            import SupervisedDataSet
from pybrain.utilities           import percentError
from pybrain.tools.shortcuts     import buildNetwork
from pybrain.tools.datasettools  import DataSetNormalizer
from pybrain.supervised.trainers import BackpropTrainer, RPropMinusTrainer
from pybrain.structure.modules   import *

from pybrain.structure import TanhLayer
from pybrain.structure import SoftmaxLayer
from pybrain.structure import SigmoidLayer

import matplotlib.pyplot as plt 


#from __future__ import print_function 

import numpy
def readData():
    d = numpy.loadtxt("data-tu27.csv", delimiter=",")
    #d = numpy.loadtxt("data-fr23.csv", delimiter=",")    
    # maybe split into training and testset?
    DS = SupervisedDataSet(11, 2)
    i = 0
    for data in d:
        h = list(data[:8])  #### do not use the zeroes (eyes)
        h.extend(data[10:13])
        DS.appendLinked(h, (data[-3], data[-1]))
        # print h, (data[-3], data[-1])
        # i = i + 1
        # if (i == 3): return i
        
    #print "bl:", DS.__len__()
    return DS

def readXData():
    d = numpy.loadtxt("data-fr23.csv", delimiter=",")
    # maybe split into training and testset?
    DS = SupervisedDataSet(11, 1)
    for data in d:
#        DS.appendLinked(data[:13], data[-3])
        h = list(data[:8])  #### do not use the zeroes (eyes)
        h.extend(data[10:13])
        DS.appendLinked(h, data[-1])

    #print "bl:", DS.__len__()
    
    return DS
    
def verify(ann, data, testset):
    plotx, ploty = [] , []
    plotpredx, plotpredy = [] , [] 
    avgeuclerr = 0
    avgerr = [0] * data.outdim
    cumerr = [0] * data.outdim    
    for inp, outp in testset:
        net_output = ann.activate(inp)
        err = abs((net_output - outp) * 100)
        euclerr = math.sqrt(err[0] ** 2 + err[1] ** 2)
        cumerr += err
        avgeuclerr = (avgeuclerr + euclerr)/2
        avgerr = (avgerr + err)/2
        #if verbose: print outp, net_output, err
        #print inp, outp, net_output, err1, err2
        # for simon
        if len(outp) > 1:
            # print outp[0], "0.53", outp[1],     \
            #         net_output[0], "0.53", net_output[1],   \
            #         err[0], "0.0", err[1]
            plotx.append(outp[0])
            ploty.append(outp[1])        
            plotpredx.append(net_output[0])
            plotpredy.append(net_output[1])
                    
        else:
            # print outp[0], net_output[0], err[0]
            plotx.append(outp[0])
            plotpredx.append(net_output[0])
            
        #print outp[0], net_output[0], err1
        
    print "AVGEUCLERRORS in cm", avgeuclerr
    print "CUMERRORS in cm", cumerr    

    if len(outp) == 1:
        plt.scatter(plotx, numpy.arange(0, 1, 1.0/len(plotx)), s=10, facecolors='none', edgecolors='r')
        plt.scatter(plotpredx, numpy.arange(0, 1, 1.0/len(plotx)), s=8, facecolors='none', edgecolors='b')    
    else:
        plt.scatter(plotx, ploty, s=10, facecolors='none', edgecolors='r')
        plt.scatter(plotpredx, plotpredy, s=8, facecolors='none', edgecolors='b')    
        
    plt.axis([-1, 1, -1, 1])
    plt.show()
        
    

    
def main():
    if len(sys.argv) > 1 and int(sys.argv[1]) < 2:
        data = readXData()
    else:
        data = readData()
    
    #print data
    
    ## normalize
    DSN = DataSetNormalizer()
    DSN.calculate(data, bounds=[-1,1], field='input')
    DSN.normalize(data, field='input')

#    DSN.calculate(data, bounds=[0,1], field='target')
#    DSN.normalize(data, field='target')    
    
    # setup
    hidden = 15
    ann = buildNetwork( data.indim, hidden, data.outdim, bias=True, hiddenclass=TanhLayer )
    #trainingset, testset = data.splitWithProportion(.2)
    trainingset, testset = data.splitWithProportion(.95)
#    trainer = RPropMinusTrainer(ann, dataset=trainingset, verbose=True)
    trainer = BackpropTrainer( ann, dataset=trainingset, verbose=True,
                learningrate=0.1, lrdecay=1.0, momentum=0.1)
    
    #info
    print "Number of training patterns: ", len(trainingset)
    print "Input/output dimensions: ", trainingset.indim, "/", trainingset.outdim
#    print "Input first line:", trainingset['input'][0]
#    print "target first line:", trainingset['target'][0]    
    err = trainer.train()

    for i in range(1,20):
#    while(err > 0.001):
        err = trainer.train()
        #verify(ann, data, testset)   
        
    verify(ann, data, testset)

#    print "epoch: %4d" % trainer.totalepochs, \
#          "  train error: %5.2f%%" % trainer.trainOnDataset(), \
#          "  test error: %5.2f%%" % trainer.trainOnDataset(dataset=testset)    

#    for i in (0,5):
#    while(trainer.) trainer.trainEpochs(5)
    #    print "epoch: %4d" % trainer.totalepochs 
    
if __name__ == '__main__':
    main()

