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

from pybrain.structure import FeedForwardNetwork, TanhLayer, SoftmaxLayer, FullConnection, BiasUnit
from pybrain.structure import SigmoidLayer

import matplotlib.pyplot as plt 

from pylab import figure


onlySingleOutput = True

#from __future__ import print_function 

import numpy
def readData():
    d = numpy.loadtxt("reddot_calibData3d_all_b.csv", delimiter=",")
    #d = numpy.loadtxt("data-fr23.csv", delimiter=",")    
    # maybe split into training and testset?
    DS = SupervisedDataSet(13, 1 if onlySingleOutput else 3)
    i = 0
    for data in d:
        data = normalize(data[:])
        if onlySingleOutput: 
            DS.addSample(data[:-3], data[-2:-1])
        else:
            DS.addSample(data[:-3], data[-3:])
        
        # print data
        #print data[:-3], "|", data[-3:-2]
        # i = i + 1
        # if (i == 3): return i
        
    #print "bl:", DS.__len__()
    return DS
    
def read2DData():
    d = numpy.loadtxt("test_Tuesday20120110_botheyes_01234567.csv", delimiter=",")    
    
#    d = numpy.loadtxt("calibData5.csv", delimiter=",")    
    DS = SupervisedDataSet(13, 1 if onlySingleOutput else 2)
    i = 0
    for data in d:
        print data
        data = normalize(data[:])
        print data
        if onlySingleOutput: 
            DS.addSample(data[:13], data[14])
        else:
            DS.addSample(data[:-2], data[12:])
        
        print data
        #print data[:-3], "|", data[-3:-2]
        # i = i + 1
        # if (i == 3): return i
        
    #print "bl:", DS.__len__()
    return DS
    

def readXXData():
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
    
    
def denormalize(val):
    """docstring for fname"""
    vec = []
    limits = [(640,0), (480, 0), (640, 0), (480,0), (10,-40), (10,-10), (10,-10), (25,-25), (25,-25), (10,-10), (20,-20), (15, -15), (5, 0), (600,0)]
    if not onlySingleOutput:
        limits = [(640,0), (480, 0), (640, 0), (480,0), (10,-40), (10,-10), (10,-10), (25,-25), (25,-25), (10,-10), (20,-20), (15, -15), (5, 0), (600,0),  (450,-450),  (500,-300)]
                    
    for l, a in zip(limits, val):
        vec.append(math.round((a + 1.0) / 2 * (l[1] - l[0]) + l[0]))
    
    print vec
    return vec


def normalize(val):
    """docstring for fname"""
    vec = []
    # if onlySingleOutput:
    #     limits = [(-600, 600)]            
    # else:
    #     # limits = [(-600, 600), (-450, 450), (-300, 500)]        
    #     limits = [(640,0), (480, 0), (640, 0), (480,0), (10,-40), (10,-10), (10,-10), (25,-25), (25,-25), (10,-10), (20,-20), (15, -15), (5, 0), (600,0),  (450,-450),  (500,-300)]
    limits = [(640,0), (480, 0), (640, 0), (480,0), (10,-40), (10,-10), (10,-10), (25,-25), (25,-25), (10,-10), (20,-20), (15, -15), (5, 0), (600,0)]
    if not onlySingleOutput:
        limits = [(640,0), (480, 0), (640, 0), (480,0), (10,-40), (10,-10), (10,-10), (25,-25), (25,-25), (10,-10), (20,-20), (15, -15), (5, 0), (600,0),  (450,-450),  (500,-300)]
        

    for l, s in zip(limits, val):
        print l,s
        vec.append( (s - l[0]) / (l[1] - l[0]) * 2 - 1.0 ) 
    return vec

    
    
def verify(ann, data, testset):
    plotx, ploty, plotz = [] , [], []
    plotpredx, plotpredy, plotpredz = [] , [], [] 
    avgeuclerr = 0
    avgerr = [0] * data.outdim
    cumerr = [0] * data.outdim    
    # print "START!!!!!\ninp, outp, net_output"
    result = []    
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
        
        # outp = denormalize(outp)
        #         net_output = denormalize(net_output)
        
        plotx.append(outp[0])
        if not onlySingleOutput:
            ploty.append(outp[1])
            plotz.append(outp[2])        

        plotpredx.append(net_output[0])
        if not onlySingleOutput:
            plotpredy.append(net_output[1])
            plotpredz.append(net_output[2])        
        #             
        # else:
        #     # print outp[0], net_output[0], err[0]
        #     plotx.append(outp[0])
        #     plotpredx.append(net_output[0])
            
        # print outp[0], net_output[0]#, err1
        
        if onlySingleOutput:
            err = abs(outp - net_output)
        else:
            err = 0
            for o,n in zip(outp, net_output):
                print o - n
                err =  err + (o - n)**2
            err = math.sqrt(err)
                    
        # print sum(result)
        result.append(err)
        
    # print result
    # print "min: ", min(result)
    # print "Test Set: avg error: ", math.sqrt(sum(result)/len(result)), "mm"
    # print "\tmin error: ", math.sqrt(min(result)), "mm",
    # print "\tmax error: ", math.sqrt(max(result)), "mm"   
    print "Test Set: avg error: ", (sum(result)/len(result)), "mm"
    print "\tmin error: ", (min(result)), "mm",
    print "\tmax error: ", (max(result)), "mm"   
        
    # print "Training Set: avg error: ", math.sqrt(sum(trainresult)), "mm"
    # print "\tmin error: ", math.sqrt(min(traintresult)), "mm",
    # print "\tmax error: ", math.sqrt(max(trainresult)), "mm"   
        
        
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
    if onlySingleOutput:
        ploty = range(0,len(plotx))
        plotpredy = range(0,len(plotx))
    plt.scatter(ploty, plotx, s=8, facecolors='none', edgecolors='r')
    plt.scatter(plotpredy, plotpredx, s=10, facecolors='c', edgecolors='b')    
    #     
    if onlySingleOutput:
        plt.axis([0, len(plotx), min(plotx)-0.1, max(plotx) + 0.1])    
    else:
        # plt.axis([-1, 1, -1, 1])
        plt.axis([min(plotx), max(plotx), min(ploty), max(ploty)])        
    # plt.axis([150, 600, -450, 450])    
    plt.draw()

    if not onlySingleOutput:
        figure(2)
        plt.clf()
        plt.ion()
        plt.scatter(plotx, plotz, s=8, facecolors='none', edgecolors='r')
        plt.scatter(plotpredx, plotpredz, s=10, facecolors='c', edgecolors='b')    
        #     
        plt.axis([-1, 1, -1, 1])
        plt.axis([min(plotx), max(plotx), min(plotz), max(plotz)])  
        # plt.axis([150, 600, -450, 450])    
        plt.draw()
    
    # print "\nEND!!!!!\n"
            
    

    
def main():
    # if len(sys.argv) > 1 and int(sys.argv[1]) < 2:
    #     data = readXData()
    # else:
    # data = readData()
    data = read2DData()    
    
    # print data
    
    ## normalize
    ## for this we also need to add the min and max values
    # if not onlySingleOutput:
    #     dataMinMax = SupervisedDataSet(13, 3)
    #     dataMinMax.addSample([640, 480, 640, 480, 10, 10, 10, 25, 25, 10, 20, 15, 50],[600,  450,  500])
    #     dataMinMax.addSample([ 0, 0, 0, 0,       -40,-10,-10,-25,-25,-10,-20,-15, 0], [0, -450, -300])
    # else:
    #     dataMinMax = SupervisedDataSet(13, 1)
    #     dataMinMax.addSample([640, 480, 640, 480, 10, 10, 10, 25, 25, 10, 20, 15, 50],[600])
    #     dataMinMax.addSample([ 0, 0, 0, 0,       -40,-10,-10,-25,-25,-10,-20,-15, 0], [-600])
    #     
    # DSN = DataSetNormalizer()
    # DSN.calculate(data, bounds=[-1,1], field='input')
    # DSN.normalize(data, field='input')
    # DSN.calculate(data, bounds=[-1,1], field='target')
    # DSN.normalize(data, field='target')
    # # DSN.calculate(dataMinMax, bounds=[-1,1], field='input')
    # # DSN.normalize(data, field='input')
    # # DSN.calculate(dataMinMax, bounds=[-1,1], field='target')
    # # DSN.normalize(data, field='target')
    # # print data

    
    # setup
    hidden = 20
    
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
    trainer = BackpropTrainer( ann, dataset=trainingset,
                verbose = True,
                learningrate = 0.35,
                # lrdecay=.2, 
                momentum = 0.1
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
        trainer.trainOnDataset(trainingset, 10)
        print "epoch: %4d" % trainer.totalepochs, \
              "  train error: %.7f" % trainer.testOnData(dataset=trainingset), \
              "  test error: %.7f" % trainer.testOnData(dataset=testset)    
        # errTrain = trainer.testOnData(dataset=trainingset)
        # errTest = trainer.testOnData(dataset=testset,verbose = False)
        # print errTrain, errTest
        # errsTrain.append(errTrain)
        # errsTest.append(errTest)

        verify(ann, data, testset)


#    for i in (0,5):
#    while(trainer.) trainer.trainEpochs(5)
    #    print "epoch: %4d" % trainer.totalepochs 
    
    # figure(0)    
    # plt.clf()
    # plt.plot(range(0,len(errsTest)), errsTest, 'r-')
    # plt.plot(range(0,len(errsTrain)), errsTrain, 'b-')
    # plt.show()
    
    
    
    
if __name__ == '__main__':
    main()

