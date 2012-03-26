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

from matplotlib.ticker import FixedLocator
import matplotlib.pyplot as plt 
import pylab


onlySingleOutput = True

#from __future__ import print_function 

import numpy
def readAndPlotData():
    # d = numpy.loadtxt("calibData5.csv", delimiter=",")
    d = numpy.loadtxt("WedThurAll.csv", delimiter=",")
    plotx, ploty, plotz = [] , [], []
    for line in d:
        plotx.append ( line[-2] )
        ploty.append ( line[-1] )
        
    print plotx[1]
    print ploty[1]
    print plotx[2]
    print ploty[2]
    # figure(1)

    fig = plt.figure()
    ax = fig.add_subplot(111)
    plt.title("Distribution of Reference Points on the Table")
    ax.axis( [0, 12, 0, 8] )
    plt.xlabel('X Axis')
    plt.ylabel('Distance from the robot')
    ax.xaxis.set_major_locator(FixedLocator([0,6,12]))
    ax.xaxis.set_minor_locator(FixedLocator(range(0,11)))    
    ax.yaxis.set_major_locator(FixedLocator([0,4]))
    ax.yaxis.set_minor_locator(FixedLocator(range(1,11)))    
    ax.grid(True, which='major') 
    ax.scatter(plotx, ploty, s=5, marker='x', facecolors='none', edgecolors='b')
    # pylab.clf()
    # # plt.ion()
    # pylab.title("Distribution of Reference Points on the Table")
    # pylab.axis( [min(plotx)-1, max(plotx)+1, min(ploty)-1, max(ploty)+1] )
    # pylab.xlabel('X Axis')
    # pylab.ylabel('Distance from the robot')
    # pylab.grid(True, with='major') 
    # pylab.scatter(plotx, ploty, s=3, facecolors='none', edgecolors='r')
    # plt.scatter(plotpredy, plotpredx, s=10, facecolors='c', edgecolors='b')    
    #     
    # plt.legend(bbox_to_anchor=(0., 1.02, 1., .102), loc=3,
    #            ncol=4, mode="expand", borderaxespad=0.)
    plt.draw()
    plt.show()    
            
    

    
def main():
    data = readAndPlotData()
    
    # figure(0)    
    # plt.clf()
    # plt.plot(range(0,len(errsTest)), errsTest, 'r-')
    # plt.plot(range(0,len(errsTrain)), errsTrain, 'b-')
    plt.show()
    
    
    
    
if __name__ == '__main__':
    main()

