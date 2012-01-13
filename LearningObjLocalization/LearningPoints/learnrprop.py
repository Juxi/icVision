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
from pybrain.supervised.trainers import RPropMinusTrainer
from pybrain.structure.modules   import *

from pybrain.structure import FeedForwardNetwork, TanhLayer, SoftmaxLayer, FullConnection, BiasUnit
from pybrain.structure import SigmoidLayer

import matplotlib.pyplot as plt 

from pylab import figure


#from __future__ import print_function 

import numpy
def readData():
    d = numpy.loadtxt("reddot_calibData3d_dec6.csv", delimiter=",")
    #d = numpy.loadtxt("data-fr23.csv", delimiter=",")    
    # maybe split into training and testset?
    DS = SupervisedDataSet(13, 3)
    i = 0
    for data in d:
        DS.addSample(data[:-3], data[-3:])
        # print data
        # print data[:-3], data[-3:]
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
    plotx, ploty, plotz = [] , [], []
    plotpredx, plotpredy, plotpredz = [] , [], [] 
    avgeuclerr = 0
    avgerr = [0] * data.outdim
    cumerr = [0] * data.outdim    
    # print "START!!!!!\ninp, outp, net_output"
    for inp, outp in testset:
        net_output = ann.activate(inp)
        
        # print inp
        # print outp, net_output
        
        
        # err = abs((net_output - outp) * 100)
        # euclerr = math.sqrt(err[0] ** 2 + err[1] ** 2)
        # cumerr += err
        # avgeuclerr = (avgeuclerr + euclerr)/2
        # avgerr = (avgerr + err)/2
        # #if verbose: print outp, net_output, err
        # #print inp, outp, net_output, err1, err2
        # # for simon
        # if len(outp) > 1:
        #     # print outp[0], "0.53", outp[1],     \
        #     #         net_output[0], "0.53", net_output[1],   \
        #     #         err[0], "0.0", err[1]
        plotx.append(outp[0])
        ploty.append(outp[1])
        plotz.append(outp[2])        
        plotpredx.append(net_output[0])
        plotpredy.append(net_output[1])
        plotpredz.append(net_output[2])        
        #             
        # else:
        #     # print outp[0], net_output[0], err[0]
        #     plotx.append(outp[0])
        #     plotpredx.append(net_output[0])
            
        #print outp[0], net_output[0], err1
        
        result = []
        limits = [(150, 600), (-450, 450), (-300, 500)]        
        # for l, a in zip(limits, net_output - outp):
        #     r = (a*a + 1.0) / 2 * (l[1] - l[0]) + l[0]
        #     result.append(r)
        # print sum(result)
        
        
    # print "AVGEUCLERRORS in cm", avgeuclerr
    # print "CUMERRORS in cm", cumerr    
    # 
    # if len(outp) == 1:
    # plt.scatter(plotx, numpy.arange(0, 1, 1.0/len(plotx)), s=10, facecolors='none', edgecolors='r')
    # plt.scatter(plotpredx, numpy.arange(0, 1, 1.0/len(plotx)), s=8, facecolors='none', edgecolors='b')    
    # else:
    figure(1)
    plt.clf()
    plt.ion()
    plt.scatter(plotx, ploty, s=8, facecolors='none', edgecolors='r')
    plt.scatter(plotpredx, plotpredy, s=10, facecolors='c', edgecolors='b')    
    #     
    plt.axis([-1, 1, -1, 1])
    # plt.axis([150, 600, -450, 450])    
    plt.draw()

    figure(2)
    plt.clf()
    plt.ion()
    plt.scatter(plotx, plotz, s=8, facecolors='none', edgecolors='r')
    plt.scatter(plotpredx, plotpredz, s=10, facecolors='c', edgecolors='b')    
    #     
    plt.axis([-1, 1, -1, 1])
    # plt.axis([150, 600, -450, 450])    
    plt.draw()
    
    # print "\nEND!!!!!\n"
            
    

    
def main():
    # if len(sys.argv) > 1 and int(sys.argv[1]) < 2:
    #     data = readXData()
    # else:
    data = readData()
    
    # print data
    
    ## normalize
    ## for this we also need to add the min and max values
    dataMinMax = SupervisedDataSet(13, 3)
    dataMinMax.addSample([640, 480, 640, 480, 10, 10, 10, 25, 25, 10, 20, 15, 50],[600,  450,  500])
    dataMinMax.addSample([ 0, 0, 0, 0,       -40,-10,-10,-25,-25,-10,-20,-15, 0], [150, -450, -300])

    DSN = DataSetNormalizer()
    DSN.calculate(dataMinMax, bounds=[-1,1], field='input')
    DSN.normalize(data, field='input')
    DSN.calculate(dataMinMax, bounds=[-1,1], field='target')
    DSN.normalize(data, field='target')
    # print data
    
    # setup
    hidden = 10
    
    # ann = FeedForwardNetwork()
    # ann.addInputModule(LinearLayer(data.indim, name="in"))    
    # ann.addModule(SigmoidLayer(hidden, name = 'h1'))
    # ann.addModule(SigmoidLayer(hidden, name = 'h2'))    
    # ann.addModule(BiasUnit(name = 'bias'))
    # ann.addOutputModule(LinearLayer(data.outdim, name = 'out'))
    # ann.addConnection(FullConnection(ann['in'], ann['h1']))
    # ann.addConnection(FullConnection(ann['h1'], ann['h2']))
    # ann.addConnection(FullConnection(ann['h2'], ann['out']))
    # ann.addConnection(FullConnection(ann['bias'], ann['h1']))    
    # ann.addConnection(FullConnection(ann['bias'], ann['h2']))    
    # ann.sortModules()
    
    ann = buildNetwork( data.indim, hidden, data.outdim, bias=True, hiddenclass=SigmoidLayer)
    #trainingset, testset = data.splitWithProportion(.2)
    trainingset, testset = data.splitWithProportion(.8)
    # testset = trainingset
#    trainer = RPropMinusTrainer(ann, dataset=trainingset, verbose=True)
    trainer = RPropMinusTrainer( ann, dataset=trainingset,
                    delta0 = 1.0
                )
                
                
    #info
    print "Number of training patterns:\t", len(trainingset)
    print "Input/output dimensions: \t", trainingset.indim, "/", trainingset.outdim
    
    # errTest = trainer.trainOnDataset(trainingset)
    errTest = 1
    
    # for i in range(1,20):
    errsTrain = []
    errsTest = []    
    while errTest > 0.0005:
        # err = trainer.train()
        trainer.trainOnDataset(trainingset, 30)
        errTrain = trainer.testOnData()
        errTest = trainer.testOnData(dataset=testset,verbose = False)
        print errTrain, errTest
        
        errsTrain.append(errTrain)
        errsTest.append(errTest)

        verify(ann, data, testset)

#    print "epoch: %4d" % trainer.totalepochs, \
#          "  train error: %5.2f%%" % trainer.trainOnDataset(), \
#          "  test error: %5.2f%%" % trainer.trainOnDataset(dataset=testset)    

#    for i in (0,5):
#    while(trainer.) trainer.trainEpochs(5)
    #    print "epoch: %4d" % trainer.totalepochs 
    
    figure(0)    
    plt.clf()
    plt.plot(range(0,len(errsTest)), errsTest, 'r-')
    plt.plot(range(0,len(errsTrain)), errsTrain, 'b-')
    plt.show()
    
    
    
    
if __name__ == '__main__':
    main()

